#!/usr/bin/env python3
"""Paired Frida capture for many MVP natives in one session per side.

Retail and the candidate hold a single-instance mutex, so they are captured
strictly one after the other: launch, hook every target in one session,
observe, close completely, and only then start the other side.

Every MVP native is identified in retail by its image-relative VA. The same
native sits at a different linked address in the candidate, so the candidate
address is resolved from the freshly built executable's symbol table; a native
that is not linked is skipped and reported as such rather than silently
producing zero calls.

The output is a raw paired capture, not a verification claim: turning it into
a frida-runtime-report is a separate, reviewable step.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import time
from pathlib import Path

import native_frida_diff as nfd
from verification_scope import file_digest, stage_candidate

R2 = "/opt/homebrew/bin/r2"
ROOT = Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts" / "frida_mvp_batch.js"
TARGETS = ROOT / "scripts" / "mvp_native_targets.json"


def candidate_symbol_table(executable: Path) -> dict[str, str]:
    """Linked VA per demangled symbol name, without the argument list."""
    result = subprocess.run(
        ["i686-w64-mingw32-nm", "-C", str(executable)],
        capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read symbols from {executable}: {result.stderr[-300:]}")
    table: dict[str, str] = {}
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3 or parts[1] not in {"T", "t"}:
            continue
        name = " ".join(parts[2:])
        base = name.split("(")[0].strip()
        table.setdefault(base, f"0x{int(parts[0], 16):08X}")
    return table


ALIASES_PATH = ROOT / "scripts" / "mvp_candidate_symbol_aliases.json"


def load_symbol_aliases() -> dict[str, str]:
    """Address to candidate symbol, for natives whose catalog and port names differ."""
    if not ALIASES_PATH.is_file():
        return {}
    data = json.loads(ALIASES_PATH.read_text())
    aliases = data.get("aliases") or {}
    return {address.lower(): entry.get("symbol", "") for address, entry in aliases.items()
            if isinstance(entry, dict) and entry.get("symbol")}


SYMBOL_ALIASES = load_symbol_aliases()


def resolve(table: dict[str, str], native_name: str, address: str = "") -> str | None:
    """The candidate VA for one native, allowing a namespace prefix or a known alias.

    The catalog name and the ported name are not always the same word - the port
    sometimes implements an address under a different name, and for a few MVP
    entries the catalog name does not describe the code at the address at all.
    When an alias is recorded for the address it wins, because the address is
    what binds the implementation to the retail image.
    """
    candidates = []
    alias = SYMBOL_ALIASES.get(address.lower())
    if alias:
        candidates.append(alias)
    candidates.append(native_name)
    for wanted in candidates:
        if wanted in table:
            return table[wanted]
        suffix = "::" + wanted
        for base, resolved in table.items():
            if base.endswith(suffix):
                return resolved
    return None


def aggregate(observed: dict, address: str) -> dict:
    """Merge the periodic per-address snapshots into one aggregate.

    The probe sends a growing aggregate several times per run, so the final
    picture is the union of the class sets and the largest hit count seen.
    """
    wanted = f"0x{int(address, 16):x}"
    padded = f"0x{int(address, 16):08x}"
    payloads = observed.get(wanted) or observed.get(padded) or []
    hits = 0
    ret_classes: set[str] = set()
    arg_patterns: set[str] = set()
    for payload in payloads:
        if payload.get("role") != "native":
            continue
        snapshot = payload.get("aggregate") or {}
        try:
            hits = max(hits, int(snapshot.get("hits") or 0))
        except (TypeError, ValueError):
            continue
        ret_classes.update(snapshot.get("ret_classes") or [])
        arg_patterns.update(snapshot.get("arg_patterns") or [])
    return {"hits": hits, "ret_classes": sorted(ret_classes),
            "arg_patterns": sorted(arg_patterns)}


EAX_WRITE = re.compile(
    r"^(mov|movzx|movsx|lea|xor|or|and|sub|add|imul|inc|dec|pop|shl|shr|sar|neg|not|sete|setne|setl|setg)"
    r"\s+(e?ax|al|eax)\b", re.IGNORECASE)


COMPARABILITY_PATH = ROOT / "scripts" / "mvp_runtime_comparability.json"


def load_comparability_overrides() -> dict[str, dict]:
    """Curated per-address exceptions to the return-class comparison."""
    if not COMPARABILITY_PATH.is_file():
        return {}
    data = json.loads(COMPARABILITY_PATH.read_text())
    overrides = data.get("overrides") or {}
    return {address: {"comparable": bool(entry.get("comparable", True)),
                      "reason": str(entry.get("reason", ""))}
            for address, entry in overrides.items() if isinstance(entry, dict)}


COMPARABILITY_OVERRIDES = load_comparability_overrides()


def return_comparability(r2: str, binary: Path, addresses: list[str]) -> dict[str, dict]:
    """Decide whether each native's return value can be compared across images.

    A function that returns nothing leaves an arbitrary EAX: whatever its last
    callee happened to leave there. Comparing that value turns a correct port
    into a mismatch, or a broken one into a match, purely because two unrelated
    numbers differ. The retail body is the authority: if it never writes EAX on
    any path, the return value is not part of its contract.
    """
    result: dict[str, dict] = dict(COMPARABILITY_OVERRIDES)
    for address in addresses:
        if address.lower() in {key.lower() for key in result}:
            continue
        out = subprocess.run(
            [r2, "-q", "-e", "bin.relocs.apply=true", "-e", "scr.color=false",
             "-c", f"s {address}; af; pdfj; q", str(binary)],
            capture_output=True, text=True, timeout=300, check=False).stdout
        try:
            ops = json.loads(out).get("ops") or []
        except ValueError:
            result[address] = {"comparable": True, "reason": "disassembly unavailable"}
            continue
        writes = [op for op in ops
                  if op.get("type") != "call" and EAX_WRITE.match(op.get("disasm", ""))]
        if writes:
            result[address] = {
                "comparable": True,
                "reason": f"the body writes EAX ({writes[-1].get('disasm', '')}) before returning",
            }
        else:
            result[address] = {
                "comparable": False,
                "reason": ("the body never writes EAX, so the return value is whatever its "
                           "last callee left there and is not part of this function's contract"),
            }
    return result


def compare(retail: dict, candidate: dict, comparability: dict | None = None) -> dict:
    """Compare what is actually comparable across two separately timed runs.

    The two runs attach at different points of a level load and advance at
    different frame rates, so call index i in one run has no correspondence to
    index i in the other and call counts cannot be compared. What is
    comparable is the set of return-value classes each image produced: a
    function that returns only handles on one side and status codes on the
    other is a real difference. Argument patterns are recorded but are not a
    verdict, because their frequency depends on the same timing.
    """
    both_observed = retail["hits"] > 0 and candidate["hits"] > 0
    verdict = comparability or {"comparable": True, "reason": ""}
    comparable = bool(verdict.get("comparable", True))
    classes_match = retail["ret_classes"] == candidate["ret_classes"]
    if comparable:
        matched = both_observed and classes_match
        basis = "reachability plus return-value class set"
        note = ("Call counts and per-call arguments differ because the two runs are "
                "timed independently; only the return-value class set is compared.")
    else:
        # Reachability is still worth recording, but it is not a return-value
        # agreement and the report must say so rather than imply one.
        matched = both_observed
        basis = "reachability only; the native's return value is not comparable"
        note = ("The retail body does not write EAX, so it returns nothing and the "
                "observed value belongs to its callee. This native is compared on "
                "reachability, not on a return value.")
    return {
        "basis": basis,
        "return_comparison": "classes" if comparable else "not_applicable",
        "return_comparability_reason": verdict.get("reason", ""),
        "retail_observed_calls": retail["hits"],
        "candidate_observed_calls": candidate["hits"],
        "retail_ret_classes": retail["ret_classes"],
        "candidate_ret_classes": candidate["ret_classes"],
        "ret_classes_match": both_observed
        and retail["ret_classes"] == candidate["ret_classes"],
        "retail_argument_class_patterns": retail["arg_patterns"][:12],
        "candidate_argument_class_patterns": candidate["arg_patterns"][:12],
        "arguments_compared": False,
        "note": note,
        "match": matched,
    }


def start_with_arguments(executable: str, level: int, extra_arguments: tuple[str, ...]):
    """Launch an image with the retail window/level words plus extra scene modes."""
    environment = dict(nfd.os.environ)
    return nfd.subprocess.Popen(
        ["env", "LC_ALL=C", "LANG=C", nfd.vnb.WINE, "--bottle", nfd.vnb.BOTTLE, executable,
         "window", f"level{level}", *extra_arguments],
        cwd=Path(nfd.vnb.GAME_DIR), stdout=nfd.subprocess.DEVNULL,
        stderr=nfd.subprocess.DEVNULL, env=environment)


def capture(process_name: str, executable: str, addresses: list[str], observe: float,
            settle: float, level: int, result: dict,
            extra_arguments: tuple[str, ...] = (), spawn: bool = False) -> None:
    """Start one image and observe the selected natives.

    The retail vocabulary is "window levelN"; the candidate also understands
    scene modes the reconstruction uses to reach more of its own code (devmode,
    inputdemo), so those can be added for the candidate run without changing the
    retail invocation.
    """
    process = None
    if not spawn:
        if extra_arguments:
            process = start_with_arguments(executable, level, extra_arguments)
        else:
            process = nfd.vnb.start_game(executable, level, game_directory=Path(nfd.vnb.GAME_DIR))
    try:
        probe_source = PROBE.read_text()
        meta: dict = {}
        if spawn:
            result["observed"] = nfd.run_observation_spawn(
                executable, "Z:" + str(Path(nfd.vnb.GAME_DIR)).replace("/", "\\"),
                ["window", f"level{level}", *extra_arguments], addresses, observe,
                probe_source, settle=settle, meta=meta)
            result["pid"] = 0
            result["meta"] = meta
            if not result["observed"]:
                result["error"] = (f"no native call observed for {process_name} "
                                   f"(spawn mode; settleExpiredWithoutCall="
                                   f"{meta.get('settleExpiredWithoutCall')})")
            return
        pid = 0
        for _ in range(120):
            pid = max(nfd.frida_list().get(process_name, [0]))
            if pid:
                break
            time.sleep(0.5)
        if not pid:
            result["error"] = f"Frida did not see {process_name}"
            return
        result["pid"] = pid
        result["observed"] = nfd.run_observation(
            pid, addresses, observe, probe_source, settle=settle, meta=meta)
        result["meta"] = meta
        if not result["observed"]:
            result["error"] = (f"no native call observed for {process_name} "
                               f"(settleExpiredWithoutCall={meta.get('settleExpiredWithoutCall')})")
    except Exception as error:  # keep both captures independent
        result["error"] = str(error)
    finally:
        if process is not None:
            nfd.vnb.stop_game(process)
        for _ in range(60):
            if not nfd.frida_list().get(process_name):
                break
            time.sleep(0.5)
        frida_kill = str(Path.home() / ".local" / "bin" / "frida-kill")
        host = nfd.os.environ.get("FRIDA_HOST", "127.0.0.1:27042")
        for _ in range(8):
            pids = nfd.frida_list().get(process_name, [])
            if not pids:
                break
            for pid in pids:
                subprocess.run([frida_kill, "-H", host, str(pid)],
                               capture_output=True, text=True, timeout=30)
            time.sleep(2)
        subprocess.run(["pkill", "-f", process_name], capture_output=True)
        time.sleep(3)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", type=Path,
                        default=ROOT / "build-win32-no-verify-gate" / "igi1.exe")
    parser.add_argument("--targets", type=Path, default=TARGETS)
    parser.add_argument("--level", type=int, default=5)
    parser.add_argument("--observe", type=float, default=6.0,
                        help="observation window for retail")
    parser.add_argument("--candidate-args", nargs="*", default=(),
                        help="extra scene modes passed to the candidate only, such as "
                             "devmode or inputdemo; retail always gets window levelN")
    parser.add_argument("--observe-candidate", type=float, default=None,
                        help="observation window for the candidate (defaults to --observe). "
                             "The candidate reaches gameplay later than retail, so it "
                             "usually needs a longer window.")
    parser.add_argument("--reuse-retail", type=Path, default=None,
                        help="reuse the retail observations of an earlier capture whose "
                             "retail image digest still matches, instead of relaunching retail")
    parser.add_argument("--settle", type=float, default=30.0)
    parser.add_argument("--spawn", action="store_true",
                        help="spawn each image suspended and install hooks before its entry "
                             "point, so natives called during startup are observed too")
    parser.add_argument("--limit", type=int, default=0,
                        help="capture only the first N linked natives (0 = all)")
    parser.add_argument("--recompare", type=Path, default=None,
                        help="recompute the verdicts of an existing capture offline, using the "
                             "current comparison rules, instead of running the games again")
    parser.add_argument("--out", type=Path,
                        default=ROOT / "build" / "runtime-traces" / "mvp-frida-pair.json")
    args = parser.parse_args()

    if args.recompare:
        capture_data = json.loads(args.recompare.read_text())
        binary = Path(capture_data["retail"])
        comparability = return_comparability(
            R2, binary, [n["retail_va"] for n in capture_data["natives"]])
        for entry in capture_data["natives"]:
            prior = entry["comparison"]
            retail_agg = {"hits": entry["retail_observed_calls"],
                          "ret_classes": prior["retail_ret_classes"],
                          "arg_patterns": prior["retail_argument_class_patterns"]}
            candidate_agg = {"hits": entry["candidate_observed_calls"],
                             "ret_classes": prior["candidate_ret_classes"],
                             "arg_patterns": prior["candidate_argument_class_patterns"]}
            entry["comparison"] = compare(
                retail_agg, candidate_agg, comparability.get(entry["retail_va"]))
            entry["result"] = ("match" if entry["comparison"]["match"]
                               else "inconclusive"
                               if not retail_agg["hits"] or not candidate_agg["hits"]
                               else "mismatch")
        capture_data["returnComparability"] = comparability
        counts = {"matched": sum(1 for n in capture_data["natives"] if n["result"] == "match"),
                  "mismatched": sum(1 for n in capture_data["natives"] if n["result"] == "mismatch"),
                  "inconclusive": sum(1 for n in capture_data["natives"] if n["result"] == "inconclusive")}
        capture_data["summary"].update(counts)
        out = args.out or args.recompare
        out.write_text(json.dumps(capture_data, indent=1) + "\n")
        print(json.dumps({"planned": len(capture_data["natives"]),
                          "unlinked": len(capture_data.get("unlinked") or []),
                          "retail_observed": sum(1 for n in capture_data["natives"]
                                                 if n["retail_observed_calls"]),
                          "candidate_observed": sum(1 for n in capture_data["natives"]
                                                    if n["candidate_observed_calls"]),
                          **counts}, indent=1))
        print(f"wrote {out}")
        return 0

    targets = json.loads(args.targets.read_text())
    candidate = args.candidate.resolve()
    table = candidate_symbol_table(candidate)
    retail = Path(nfd.vnb.GAME_DIR) / "igi.exe"

    plan = []
    unlinked = []
    for target in targets:
        address = target["address"]
        linked = resolve(table, target["name"], address)
        if linked is None:
            unlinked.append({"name": target["name"], "address": address})
            continue
        plan.append({"name": target["name"], "address": address,
                     "retail_va": address, "candidate_va": linked,
                     "candidate_symbol": SYMBOL_ALIASES.get(address.lower(), target["name"])})
    if args.limit:
        plan = plan[:args.limit]
    print(f"linked {len(plan)} / {len(targets)} MVP natives; "
          f"{len(unlinked)} have no candidate symbol", flush=True)

    existing = nfd.frida_list()
    if existing.get("igi.exe") or existing.get("open-igi.exe") or existing.get("igi1.exe"):
        raise RuntimeError("close existing game sessions before the paired capture")

    retail_result: dict = {}
    candidate_result: dict = {}
    reused = None
    if args.reuse_retail:
        prior = json.loads(args.reuse_retail.read_text())
        # Only reuse observations that belong to the same retail image: a
        # different build would make the captured addresses meaningless.
        if prior.get("retailSha256") == file_digest(retail):
            prior_observed = prior.get("retailObservations") or {}
            if not prior_observed:
                # Archived captures drop the raw snapshots and keep only the
                # per-native aggregate. That is enough to reuse: rebuild one
                # final snapshot per native from the recorded counts.
                prior_observed = {}
                for entry in prior.get("natives") or []:
                    hits = entry.get("retail_observed_calls") or 0
                    comparison = entry.get("comparison") or {}
                    key = f"0x{int(entry['retail_va'], 16):x}"
                    prior_observed[key] = [{
                        "role": "native",
                        "aggregate": {
                            "hits": hits,
                            "ret_classes": comparison.get("retail_ret_classes") or [],
                            "arg_patterns": comparison.get("retail_argument_class_patterns") or [],
                        },
                    }]
            retail_result = {"observed": prior_observed,
                             "pid": (prior.get("capture") or {}).get("retail", {}).get("pid"),
                             "meta": {"reused_from": str(args.reuse_retail)}}
            reused = args.reuse_retail
            print(f"reusing retail observations from {args.reuse_retail}", flush=True)
        else:
            print("retail image changed since the prior capture; capturing retail", flush=True)

    if not reused:
        print(f"capturing retail igi.exe (level {args.level}) ...", flush=True)
        capture("igi.exe", "Z:" + str(retail).replace("/", "\\"),
                [item["retail_va"] for item in plan], args.observe, args.settle,
                args.level, retail_result, spawn=args.spawn)
        print(f"retail: {len(retail_result.get('observed') or {})} natives observed"
              f" error={retail_result.get('error')}", flush=True)

    print("retail closed; capturing the candidate ...", flush=True)
    with stage_candidate(candidate, retail.resolve().parent) as staged:
        nfd.os.environ["IGI_SCENE_LOG"] = "open-igi-scene.log"
        candidate_window = args.observe_candidate if args.observe_candidate else args.observe
        capture(staged.name.lower(), "Z:" + str(staged).replace("/", "\\"),
                [item["candidate_va"] for item in plan], candidate_window, args.settle,
                args.level, candidate_result,
                extra_arguments=tuple(args.candidate_args or ()), spawn=args.spawn)
    print(f"candidate: {len(candidate_result.get('observed') or {})} natives observed"
          f" error={candidate_result.get('error')}", flush=True)

    retail_observed = retail_result.get("observed") or {}
    candidate_observed = candidate_result.get("observed") or {}

    comparability = return_comparability(
        R2, retail, [item["retail_va"] for item in plan])
    natives = []
    for item in plan:
        retail_agg = aggregate(retail_observed, item["retail_va"])
        candidate_agg = aggregate(candidate_observed, item["candidate_va"])
        entry = dict(item)
        entry.update({
            "retail_observed_calls": retail_agg["hits"],
            "candidate_observed_calls": candidate_agg["hits"],
            "comparison": compare(retail_agg, candidate_agg,
                                  comparability.get(item["retail_va"])),
        })
        entry["result"] = ("match" if entry["comparison"]["match"]
                           else "inconclusive"
                           if not retail_agg["hits"] or not candidate_agg["hits"]
                           else "mismatch")
        natives.append(entry)

    report = {
        "schema_version": 1,
        "kind": "mvp-paired-capture",
        "tool": "frida",
        "captureMode": "sequential",
        "probe": str(PROBE.relative_to(ROOT)),
        "probeSha256": file_digest(PROBE),
        "retail": str(retail),
        "retailSha256": file_digest(retail),
        "retailObservations": retail_observed,
        "candidate": str(candidate),
        "candidateSha256": file_digest(candidate),
        "level": args.level,
        "observe_seconds": args.observe,
        "settle_seconds": args.settle,
        "capture": {
            "retail": {"pid": retail_result.get("pid"), **(retail_result.get("meta") or {}),
                       "error": retail_result.get("error")},
            "candidate": {"pid": candidate_result.get("pid"),
                          **(candidate_result.get("meta") or {}),
                          "error": candidate_result.get("error")},
        },
        "summary": {
            "planned": len(plan),
            "unlinked": len(unlinked),
            "retail_observed": sum(1 for n in natives if n["retail_observed_calls"]),
            "candidate_observed": sum(1 for n in natives if n["candidate_observed_calls"]),
            "matched": sum(1 for n in natives if n["result"] == "match"),
            "mismatched": sum(1 for n in natives if n["result"] == "mismatch"),
            "inconclusive": sum(1 for n in natives if n["result"] == "inconclusive"),
        },
        "returnComparability": comparability,
        "unlinked": unlinked,
        "natives": natives,
        "limitations": [
            "The probe records arguments and return values as process-independent "
            "classes: pointers cannot be compared by value because heap addresses "
            "differ, so only the class (null/small-exact/pointer) is comparable.",
            "Retail and the candidate are captured one at a time because both hold a "
            "single-instance mutex, and the two runs attach at different points of "
            "level load, so call indices, call counts and per-call arguments do not "
            "correspond. Only the return-value class set is compared.",
            "A captured call proves the native was reached in both images and that "
            "both returned the same class of value; it does not prove behavioural "
            "parity and does not establish any side effect.",
        ],
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report["summary"], indent=1))
    print(f"wrote {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
