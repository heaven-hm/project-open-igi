#!/usr/bin/env python3
"""Assemble one per-native static+runtime verification record.

Inputs are the Tester's own captured evidence:
  - static capture from scripts/tester/static_probe_capture.py
  - paired runtime reports from scripts/tester/frida_probe_driver.py
  - ordered comparison from scripts/tester/compare_native_capture.py

It applies a native-specific invariant profile (only for natives whose retail
semantics were established statically) to the retail observations, and records
the verdict honestly:
  static:   PASS / FAIL / BLOCKED  (structure, decompiler/cfg/angr agreement,
            exact strings, candidate symbol + source binding)
  retail:   OBSERVED(n) / UNOBSERVED
  candidate OBSERVED(n) / UNOBSERVED
  parity:   MATCH / MISMATCH / UNOBSERVED
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def entry_words(payload: dict) -> list[int | None]:
    return [word.get("value") for word in payload.get("args") or []]


def dword_le(hex_text: str, byte_offset: int) -> int | None:
    """Read one little-endian dword from a hex byte string (probe stores bytes in order)."""
    chunk = hex_text[byte_offset * 2:byte_offset * 2 + 8]
    if len(chunk) != 8:
        return None
    return int("".join(reversed([chunk[index:index + 2] for index in range(0, 8, 2)])), 16)


def header(payload: dict, key: str = "memory"):
    """Return [count, capacity] from an 8-byte little-endian list-header snapshot."""
    snapshot = (payload.get(key) or {}).get("snapshot") or {}
    if snapshot.get("status") != "read":
        return None
    data = snapshot.get("hex") or ""
    if len(data) < 16:
        return None
    count, capacity = dword_le(data, 0), dword_le(data, 4)
    if count is None or capacity is None:
        return None
    return [count, capacity]


def check_new(entries: list[dict], returns: list[dict]) -> list[str]:
    problems = []
    for index, (entry, returned) in enumerate(zip(entries, returns), 1):
        words = entry_words(entry)
        capacity = words[0] if words else None
        eax = returned.get("eaxValue")
        if eax is None:
            problems.append(f"call {index}: return value unrecorded")
            continue
        snapshot = header(returned, "returnMemory")
        if snapshot is None:
            problems.append(f"call {index}: return memory snapshot unavailable")
        elif snapshot[0] != 0 or snapshot[1] != capacity:
            problems.append(f"call {index}: header {snapshot} != [0, capacity={capacity}]")
    return problems


def check_register(entries: list[dict], returns: list[dict]) -> list[str]:
    problems = []
    for index, (entry, returned) in enumerate(zip(entries, returns), 1):
        words = entry_words(entry)
        list_pointer = words[0] if words else None
        before = header(entry, "memory")
        after = header(returned, "memory")
        eax = returned.get("eaxValue")
        if before is None or after is None:
            problems.append(f"call {index}: header snapshot unavailable")
        else:
            if not 0 <= before[0] < before[1]:
                problems.append(f"call {index}: entry count {before[0]} not < capacity {before[1]}")
            if after[0] != before[0] + 1 or after[1] != before[1]:
                problems.append(f"call {index}: header delta {before} -> {after} != count+1")
        if eax is not None and list_pointer is not None and eax != list_pointer:
            problems.append(f"call {index}: return {hex(eax)} != list pointer {hex(list_pointer)}")
    return problems


def check_remove(entries: list[dict], returns: list[dict]) -> list[str]:
    problems = []
    for index, (entry, returned) in enumerate(zip(entries, returns), 1):
        before = header(entry, "memory")
        after = header(returned, "memory")
        eax = returned.get("eaxValue")
        if before is None or after is None:
            problems.append(f"call {index}: header snapshot unavailable")
            continue
        if before[0] <= 0:
            problems.append(f"call {index}: entry count {before[0]} <= 0")
        if after[0] != before[0] - 1 or after[1] != before[1]:
            problems.append(f"call {index}: header delta {before} -> {after} != count-1")
        if eax is None or not 0 <= eax < max(before[0], 1):
            problems.append(f"call {index}: returned index {eax} outside [0,{before[0]})")
    return problems


INVARIANTS = dict(new=check_new, register=check_register, remove=check_remove)


LIVE_STATIC_LIMITATION = ("no repository evidence artifact existed; live capture is the static "
                          "evidence")


def compare_static(static: dict, live_scope: bool = False) -> dict:
    bn = static.get("binary_ninja", {})
    r2 = static.get("radare2", {})
    angr = static.get("angr", {})
    cfg_bn = bn.get("cfg")
    cfg_r2 = r2.get("cfg")
    cross = static.get("cfg_cross_check") or {}
    evidence = static.get("evidence_cfg") or {}
    manifest = static.get("manifest") or {}
    symbols = static.get("candidate_facts", {}).get("symbols") or {}
    symbol_hits = {name: hits for name, hits in symbols.items() if hits}
    checks = {
        "binary_ninja_decompiled": bool(bn.get("decompile_text")) and bn.get("decompile_exit") == 0,
        "bn_cfg_recovered": bool(cfg_bn and cfg_bn.get("nodes")),
        "r2_cfg_recovered": bool(cfg_r2 and cfg_r2.get("nodes")),
        "bn_r2_block_parity": bool(cross) and cross.get("left_nodes") == cross.get("right_nodes")
                              and cross.get("left_edges") == cross.get("right_edges"),
        "angr_exact_entry": angr.get("function_entry") == static.get("address")
                            and angr.get("has_unresolved_jumps") is False,
        "evidence_bn_nodes_match_live": bool(evidence.get("binary_ninja"))
            and (static.get("evidence_vs_live_bn") or {}).get("node_jaccard") == 1.0,
        "evidence_r2_nodes_match_live": bool(evidence.get("radare2"))
            and (static.get("evidence_vs_live_r2") or {}).get("node_jaccard") == 1.0,
        "candidate_symbol_present": bool(symbol_hits),
        "implementation_source_present": bool(manifest.get("implementation")),
        "implementation_matches_manifest_sha": manifest.get("implementation_current") is True,
    }
    if live_scope:
        checks["evidence_bn_nodes_match_live"] = None
        checks["evidence_r2_nodes_match_live"] = None
    effective = [value for value in checks.values() if value is not None]
    if live_scope:
        verdict = ("PASS_WITH_DOCUMENTED_SCOPE" if all(effective) else
                   ("BLOCKED" if not checks["candidate_symbol_present"] or not checks["bn_cfg_recovered"]
                    else "FAIL"))
    else:
        verdict = ("PASS" if all(effective) else
                   ("BLOCKED" if not checks["candidate_symbol_present"] or not checks["bn_cfg_recovered"]
                    else "FAIL"))
    return dict(checks=checks,
                verdict=verdict,
                bn_blocks=len((cfg_bn or {}).get("nodes", [])),
                bn_edges=len((cfg_bn or {}).get("edges", [])),
                r2_blocks=len((cfg_r2 or {}).get("nodes", [])),
                r2_edges=len((cfg_r2 or {}).get("edges", [])),
                angr=angr, cross=cross,
                evidence_paths=sorted((static.get("evidence") or {}).keys()),
                documented_scope=(dict(
                    limitation=LIVE_STATIC_LIMITATION,
                    components=dict(
                        binary_ninja_decompiler=dict(
                            available=bool((static.get("binary_ninja") or {}).get("decompile_text")),
                            exit_code=(static.get("binary_ninja") or {}).get("decompile_exit")),
                        bn_r2_cfg_parity=dict(
                            left_nodes=(cross or {}).get("left_nodes"),
                            right_nodes=(cross or {}).get("right_nodes"),
                            left_edges=(cross or {}).get("left_edges"),
                            right_edges=(cross or {}).get("right_edges"),
                            node_jaccard=(cross or {}).get("node_jaccard")),
                        angr=dict(function_entry=(angr or {}).get("function_entry"),
                                  has_unresolved_jumps=(angr or {}).get("has_unresolved_jumps")),
                        bytes_crosscheck=bool((static.get("bytes_crosscheck") or {}).get("match")
                                              or (static.get("expected_bytes") or {}).get("match")),
                        candidate_symbols=[dict(address=item["address"], symbol=item["symbol"])
                                           for hits in symbol_hits.values() for item in hits],
                        manifest=dict(implementation=manifest.get("implementation"),
                                      digest_now=manifest.get("implementation_sha256_now"),
                                      digest_recorded=manifest.get("implementation_sha256_recorded"),
                                      current=manifest.get("implementation_current")),
                    )) if live_scope else None),
                candidate_symbols={name: [dict(address=item["address"], symbol=item["symbol"],
                                               candidate_callers=len(item.get("candidate_caller_lines", [])))
                                          for item in hits]
                                   for name, hits in symbol_hits.items()},
                manifest=manifest)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--address", required=True)
    ap.add_argument("--name", required=True)
    ap.add_argument("--invariant", choices=("none", "new", "register", "remove"), default="none")
    ap.add_argument("--static", type=Path, required=True)
    ap.add_argument("--retail", type=Path, required=True)
    ap.add_argument("--candidate", type=Path, required=True)
    ap.add_argument("--comparison", type=Path, required=True)
    ap.add_argument("--out", type=Path, required=True)
    ap.add_argument("--live-static-scope", action="store_true",
                    help="documented-scope static acceptance for addresses without a repository "
                         "evidence artifact: evidence cross-checks are recorded as unavailable (None), "
                         "never as PASS")
    args = ap.parse_args()

    static = load(args.static)
    retail = load(args.retail)
    candidate = load(args.candidate)
    comparison = load(args.comparison)
    target = next((item for item in comparison.get("targets", [])
                   if item["address"].lower() == args.address.lower()), None)
    if target is None:
        raise SystemExit(f"address {args.address} absent from comparison report")
    retail_obs = [payload for payload in retail.get("entries", []) + retail.get("returns", [])
                  if payload.get("address", "").lower() == args.address.lower()]
    retail_entries = [item for item in retail_obs if item.get("phase") == "entry"]
    retail_returns = [item for item in retail_obs if item.get("phase") == "return"]
    candidate_obs = [payload for payload in candidate.get("entries", []) + candidate.get("returns", [])
                     if payload.get("address", "").lower() == args.address.lower()]
    candidate_entries = [item for item in candidate_obs if item.get("phase") == "entry"]
    candidate_returns = [item for item in candidate_obs if item.get("phase") == "return"]
    problems = []
    if args.invariant != "none":
        problems = INVARIANTS[args.invariant](retail_entries, retail_returns)
    static_result = compare_static(static, live_scope=args.live_static_scope)
    record = dict(
        kind="igi-tester-individual-native-record",
        schema_version=1,
        address=args.address,
        name=args.name,
        executable_identity=dict(retail=retail.get("executable"), candidate=candidate.get("executable")),
        probes={target["address"]: dict(probe_sha256=next(
            (t["probe_sha256"] for t in retail.get("targets", [])
             if t["address"].lower() == args.address.lower()), None))},
        static=static_result,
        runtime=dict(
            retail=dict(report=str(args.retail), entries=len(retail_entries),
                        returns=len(retail_returns),
                        entries_alive_at_end=(retail.get("capture") or {}).get("alive_at_end"),
                        invariant_profile=args.invariant,
                        invariant_violations=problems[:20], invariant_violation_count=len(problems)),
            candidate=dict(report=str(args.candidate),
                           entries=len(candidate_entries),
                           returns=len(candidate_returns),
                           entries_alive_at_end=(candidate.get("capture") or {}).get("alive_at_end")),
            comparison=target),
        limitations=[
            "Retail and candidate were run sequentially in separate processes; pointer values are "
            "compared only as opaque 32-bit words, never as process addresses.",
            "Observation window is a bounded idle interval on full-level-1 startup; natives first "
            "reached later in the level are not covered by this record.",
            "The candidate hooks are installed at the candidate's own symbol addresses while probe "
            "messages keep the retail VA, so the same probe file is reused unchanged on both sides.",
            "Static verdict covers one decompiler (Binary Ninja), Radare2 CFG and angr exact entry; "
            "CFG agreement is structural evidence, not behavioral proof.",
            *([LIVE_STATIC_LIMITATION + "; evidence_bn_nodes_match_live / "
               "evidence_r2_nodes_match_live are recorded as unavailable (None), not PASS; this is "
               "documented-scope static acceptance, not full verification and not universal parity."]
              if args.live_static_scope else []),
        ],
        verdict=dict(
            static=static_result["verdict"],
            retail="OBSERVED" if retail_entries else "UNOBSERVED",
            candidate="OBSERVED" if candidate_entries else "UNOBSERVED",
            parity=target["verdict"],
            invariants="PASS" if args.invariant != "none" and not problems else
                       ("FAIL" if args.invariant != "none" else "NOT_APPLIED"),
        ),
    )
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(record, indent=2) + "\n")
    print(json.dumps(record["verdict"], indent=2), "->", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
