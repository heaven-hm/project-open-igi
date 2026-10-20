#!/usr/bin/env python3
"""Differential check of the natives: retail igi.exe against open-igi.exe.

One small probe session is attached to each side, with a hook on every native.
The retail game is observed to call its natives, and the reconstruction is
observed to call its own implementations, so both sides report the argument
words and the result they really produced.

The probe records naturally occurring calls only. It does not synthesize calls
or replay retail arguments in the reconstruction: pointers, globals, and
process-specific state make raw argument replay unsafe. A missing candidate call
or a call without a verified same-input semantic comparison is an explicit
verification failure, not evidence of parity.
"""

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path
from verification_scope import (address as normalize_address, file_digest, load_scope,
                                pair_typed_sequences, stage_candidate, validate_runtime_contract)
from check_frida_differential import behavior_matches

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import importlib.util  # noqa: E402
import hashlib

spec = importlib.util.spec_from_file_location(
    "vnb", ROOT / "scripts" / "verify_natives_batch.py")
vnb = importlib.util.module_from_spec(spec)
spec.loader.exec_module(vnb)

FRIDA_PYTHON = str(Path.home() / "Library" / "Application Support" / "pipx" /
                   "venvs" / "frida-tools" / "bin" / "python")
PROBE = ROOT / "scripts" / "frida_native_diff.js"
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def scalar_abi_types(signature: str) -> tuple[bool, list[str] | None]:
    """Return whether the declared return/arguments are safely scalar-comparable.

    This deliberately recognizes only primitive C ABI values. Pointer, struct,
    reference, and unknown types need an explicit semantic comparator; raw
    addresses from two processes are never treated as equivalent data.
    """
    import re

    match = re.match(r"\s*(.*?)\s+\w+\s*\((.*)\)\s*;?\s*$", signature)
    if not match:
        return False, None
    return_type, argument_text = match.groups()
    # The generic hook reads x86 argument words and EAX, not x87/ST0, doubles,
    # EDX:EAX or aggregates. Unsupported ABIs need a typed per-native probe.
    primitive = re.compile(r"^(?:u?int(?:8|16|32)_t|char|short|int|long|bool|size_t)$")

    def is_scalar(type_name: str) -> bool:
        normalized = " ".join(type_name.strip().split())
        return primitive.fullmatch(normalized) is not None

    arguments = []
    if argument_text.strip() not in ("", "void"):
        # Native catalog declarations are C-style parameter lists. Retain each
        # type up to its parameter name; nested templates are not inferred.
        for argument in argument_text.split(","):
            declaration = argument.strip()
            declaration = re.sub(r"\s+[A-Za-z_]\w*$", "", declaration)
            arguments.append(declaration)
    return is_scalar(return_type), arguments if all(map(is_scalar, arguments)) and len(arguments) <= 6 else None

CHILD = r'''
import hashlib, json, os, sys, time
import frida
from pathlib import Path

pid = int(sys.argv[1])
targets = json.loads(sys.argv[2])
script_source = sys.stdin.buffer.read().decode("utf-8")
result_path = sys.argv[4]
observe = float(sys.argv[5])

device = frida.get_device_manager().add_remote_device(os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))
session = device.attach(pid)
script = session.create_script(script_source)
observed = {}
failures = []
trace_dir = Path(result_path).parent / "runtime-traces"
trace_dir.mkdir(parents=True, exist_ok=True)
raw_log = (trace_dir / f"native-{pid}-{time.time_ns()}.jsonl").open("w")
raw_log.write(json.dumps({"type": "probe-source", "sha256": hashlib.sha256(script_source.encode("utf-8")).hexdigest()}) + "\n")
raw_log.flush()


def on_message(message, data):
    raw_log.write(json.dumps(message) + "\n")
    raw_log.flush()
    payload = message.get("payload") or {}
    if message.get("type") == "error" or payload.get("type") == "hook_failed":
        failures.append(message)
    if payload.get("type") == "observed":
        calls = observed.setdefault(payload.get("address"), [])
        if len(calls) < 10000:
            calls.append(payload)
        elif not failures:
            failures.append({"error": "native call sequence exceeds capture capacity; incomplete evidence"})


script.on("message", on_message)
script.load()
for address in targets:
    script.exports_sync.hook(address)
time.sleep(observe)
if failures:
    raise RuntimeError(f"Frida probe failed: {failures}")
json.dump(observed, open(result_path, "w"))
session.detach()
raw_log.close()
'''


def frida_list():
    """The processes the remote Frida server can see, keyed by name."""
    p = subprocess.run([str(Path.home() / ".local" / "bin" / "frida-ps"),
                        "-H", os.environ.get("FRIDA_HOST", "127.0.0.1:27042")],
                       capture_output=True, text=True, timeout=60)
    if p.returncode:
        raise RuntimeError(f"Frida process enumeration failed: {p.stderr.strip()}")
    out = {}
    for line in p.stdout.splitlines():
        parts = line.split()
        if len(parts) < 2 or not parts[0].isdigit():
            continue
        out.setdefault(parts[1].lower(), []).append(int(parts[0]))
    return out


def run_observation(pid: int, addresses: list[str], observe: float, probe_source: str) -> dict:
    child = ROOT / "build" / "frida_observe_child.py"
    child.parent.mkdir(parents=True, exist_ok=True)
    child.write_text(CHILD)
    out = ROOT / "build" / "frida_observed.json"
    out.unlink(missing_ok=True)
    cmd = [FRIDA_PYTHON, str(child), str(pid), json.dumps(addresses),
           "-", str(out), str(observe)]
    try:
        p = subprocess.run(cmd, input=probe_source, capture_output=True, text=True,
                           encoding="utf-8", timeout=observe + 180)
    except subprocess.TimeoutExpired:
        print("  observation timed out")
        return {}
    if p.returncode != 0 or not out.exists():
        print(f"  observation failed: {(p.stderr or p.stdout or '')[-600:]}")
        return {}
    data = json.loads(out.read_text())
    out.unlink()
    if not data:
        print(f"  observation produced no calls; child output: "
              f"{(p.stderr or p.stdout or '')[-300:]}")
    return data


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--symbols", default=str(ROOT / "assets" / "candidate_symbol_map.json"))
    ap.add_argument("--gate-report", type=Path,
                    help="Require static/semantic gate success and probe exactly its native addresses")
    ap.add_argument("--candidate-exe", type=Path,
                    help="Newly compiled Windows x86 candidate, before deployment")
    ap.add_argument("--probe", type=Path, default=PROBE,
                    help="Same evidence-backed probe for both images; typedState observations enable state comparison")
    ap.add_argument("--level", type=int, default=4)
    ap.add_argument("--observe", type=float, default=20.0)
    ap.add_argument("--settle", type=float, default=45.0)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--out", default=str(ROOT / "build" / "native-frida-differential.json"))
    a = ap.parse_args()
    # Freeze once. Both sessions receive these exact bytes over stdin, even if
    # another agent edits the source file during the paired experiment.
    probe_bytes = a.probe.read_bytes()
    probe_source = probe_bytes.decode("utf-8")
    probe_digest = hashlib.sha256(probe_bytes).hexdigest()

    rows = [r for r in json.load(open(a.symbols)) if r.get("implemented")]
    scope_digest = None
    if a.gate_report:
        required, scope_digest, gate = load_scope(a.gate_report)
        rows = [r for r in rows if normalize_address(r["address"]) in required]
        missing = required - {normalize_address(r["address"]) for r in rows}
        if missing:
            raise ValueError(f"changed natives lack candidate mappings: {sorted(missing)}")
        if a.limit:
            raise ValueError("--limit cannot truncate a required verification scope")
    candidate_path = (a.candidate_exe or Path(vnb.GAME_DIR) / "open-igi.exe").resolve()
    retail_path = Path(os.environ.get("IGI_RETAIL_EXE", str(Path(vnb.GAME_DIR) / "igi.exe")))
    retail_digest = file_digest(retail_path)
    candidate_digest = file_digest(candidate_path)
    from build_attestation import require_x86_pe
    require_x86_pe(candidate_path)
    require_x86_pe(retail_path)
    if a.gate_report and gate.get("binary_sha256") != retail_digest:
        raise ValueError("runtime retail image differs from the static/semantic gate")
    # Resolve symbols from this candidate, never reuse addresses from an older build.
    nm = subprocess.run(["i686-w64-mingw32-nm", "--defined-only", str(candidate_path)],
                        capture_output=True, text=True, check=True, timeout=60)
    symbols = {}
    for line in nm.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            symbols[parts[2]] = f"0x{int(parts[0], 16):08X}"
    for row in rows:
        symbol = (row.get("candidateSymbol") or {}).get("symbol")
        if not symbol or symbol not in symbols:
            raise ValueError(f"candidate symbol unavailable in current executable: {row['address']} {symbol}")
        row["candidateSymbol"] = {"symbol": symbol, "vaddr": symbols[symbol]}
    master = {int(n["address"], 16): n for n in json.load(
        open(ROOT / "assets" / "IGINatives_All_GameNatives.json"))["Natives"]}
    for r in rows:
        r["retailSignature"] = master.get(int(r["address"], 16), {}).get("signature", "")
    if a.limit:
        rows = rows[:a.limit]
    print(f"natives to compare: {len(rows)}", flush=True)

    existing = frida_list()
    if existing.get("igi.exe") or existing.get("open-igi.exe"):
        raise RuntimeError("Close existing game sessions before the paired test; they are not owned by this runner")
    retail_addresses = [r["address"] for r in rows]
    candidate_addresses = [r["candidateSymbol"]["vaddr"] for r in rows]

    def observe_game(exe, process_name, addresses):
        process = vnb.start_game(exe, a.level, game_directory=retail_path.resolve().parent)
        try:
            time.sleep(a.settle)
            pid = max(frida_list().get(process_name, [0]))
            if not pid:
                raise RuntimeError(f"Frida did not see {process_name} after launch")
            observations = run_observation(pid, addresses, a.observe, probe_source)
            if not observations:
                raise RuntimeError(
                    f"Frida observation for {process_name} is empty or incomplete; "
                    "refusing to write a differential report"
                )
            return observations
        finally:
            vnb.stop_game(process)
            # CrossOver may keep the Windows game alive after terminating the
            # host-side wine wrapper. Stop only the process name this runner
            # launched, and verify it is gone before starting the other game.
            remaining = frida_list().get(process_name, [])
            for pid in remaining:
                subprocess.run([str(Path.home() / ".local" / "bin" / "frida-kill"),
                                "-H", os.environ.get("FRIDA_HOST", "127.0.0.1:27042"),
                                str(pid)], capture_output=True, text=True, timeout=30)
            if frida_list().get(process_name):
                raise RuntimeError(f"{process_name} remains alive; refusing to launch the next game")

    print("observing retail natives ...", flush=True)
    retail_obs = observe_game("Z:" + str(retail_path.resolve()).replace("/", "\\"),
                              "igi.exe", retail_addresses)
    print("observing newly built OpenIGI natives ...", flush=True)
    with stage_candidate(candidate_path, retail_path.resolve().parent) as staged:
        candidate_obs = observe_game("Z:" + str(staged).replace("/", "\\"),
                                     staged.name.lower(), candidate_addresses)

    results = []
    for row in rows:
        address = row["address"]
        symbol = row.get("candidateSymbol")
        entry = {"address": address, "name": row["retailName"],
                 "cppName": row["cppName"], "implementation": row["enginePath"]}
        # Frida reports the address as it was written, so both spellings are
        # looked up
        retail_calls = (retail_obs.get(address)
                       or retail_obs.get(f"0x{int(address, 16):x}")
                       or retail_obs.get(int(address, 16)) or [])
        retail_call = retail_calls[-1] if retail_calls else {}
        if not retail_call:
            entry["result"] = "not_observed_in_retail"
            results.append(entry)
            continue
        entry["retailArgs"] = retail_call.get("args")
        entry["retailReturn"] = retail_call.get("retval")
        entry["retailCalled"] = True
        signature = row.get("retailSignature") or ""
        scalar_return, scalar_arguments = scalar_abi_types(signature)
        if symbol:
            candidate_address = symbol["vaddr"]
            candidate_calls = (candidate_obs.get(candidate_address)
                              or candidate_obs.get(f"0x{int(candidate_address, 16):x}")
                              or candidate_obs.get(int(candidate_address, 16)) or [])
            candidate_call = candidate_calls[-1] if candidate_calls else {}
            entry["candidateArgs"] = candidate_call.get("args")
            entry["candidateReturn"] = candidate_call.get("retval")
            entry["candidateCalled"] = bool(candidate_call)
            entry["candidateReachable"] = True
        else:
            entry["candidateReachable"] = False
            entry["candidateCalled"] = False
        entry["retailSignature"] = signature
        entry["scalarReturn"] = scalar_return
        if (entry.get("candidateCalled") and scalar_arguments is not None
                and isinstance(entry.get("retailArgs"), list)
                and isinstance(entry.get("candidateArgs"), list)
                and len(entry["retailArgs"]) >= len(scalar_arguments)
                and len(entry["candidateArgs"]) >= len(scalar_arguments)):
            parameter_count = len(scalar_arguments)
            entry["argumentsMatch"] = (
                entry["retailArgs"][:parameter_count] == entry["candidateArgs"][:parameter_count]
            )
            entry["argumentComparison"] = "raw scalar ABI words"
        else:
            entry["argumentsMatch"] = None
            entry["argumentComparison"] = "inconclusive: pointer or unknown ABI types"
        entry["valueMatch"] = bool(
            scalar_return and entry.get("argumentsMatch") is True and
            entry["retailReturn"] == entry.get("candidateReturn") and
            entry.get("candidateReturn") is not None
        )
        comparison = pair_typed_sequences(retail_calls, candidate_calls if symbol else [])
        entry["retailCalls"] = retail_calls
        entry["candidateCalls"] = candidate_calls if symbol else []
        if comparison is not None:
            entry["comparison"] = comparison
            entry["argumentsMatch"] = comparison["inputs"]["retail"] == comparison["inputs"]["candidate"]
            entry["semanticComparator"] = probe_digest
            entry["semanticMatch"] = behavior_matches(entry)
        entry["match"] = False
        if a.gate_report and comparison is not None:
            contracts = {normalize_address(n["address"]): n.get("runtime_contract") for n in gate["checked_natives"]}
            try:
                validate_runtime_contract(entry, contracts[normalize_address(address)], probe_digest)
                entry["match"] = entry.get("semanticMatch") is True
            except ValueError as error:
                entry["comparisonError"] = str(error)
        entry["result"] = ("match" if entry["match"] else
                           "mismatch" if comparison is not None and entry.get("semanticMatch") is False
                           else "inconclusive")
        results.append(entry)
        print(f"  {entry['address']} {entry['name']:<34} "
              f"retail={entry['retailReturn']} candidate={entry.get('candidateReturn')} "
              f"called={entry.get('candidateCalled')} {entry['result']}", flush=True)

    observed = [r for r in results if r.get("retailCalled")]
    matched = [r for r in observed if r.get("match")]
    called_both = [r for r in observed if r.get("candidateCalled")]
    doc = {
        "tool": "frida",
        "retail": "igi.exe",
        "candidate": str(candidate_path),
        "candidateRuntimeName": staged.name,
        "gameDirectory": str(retail_path.resolve().parent),
        "arguments": ["window", f"level{a.level}"],
        "captureMode": "sequential",
        "binarySha256": retail_digest,
        "candidateSha256": candidate_digest,
        "staticGateSha256": scope_digest,
        "requiredAddresses": sorted(normalize_address(r["address"]) for r in rows),
        "probeSha256": probe_digest,
        "level": a.level,
        "method": "one probe session per side; preserve complete ordered call observations. "
                  "Acceptance requires typed state comparisons and a source-semantic-reviewed "
                  "runtime contract bound to the same probe hash",
        "limitations": [
            "Late attachment misses startup/load calls; those natives remain unverified",
            "The generic probe compares EAX only and does not validate memory/global side effects",
            "a native the retail game does not call in this scenario has no "
            "observed input, so it is not compared",
            "a native the reconstruction does not call has no candidate result; "
            "this is a wiring gap and blocks acceptance",
            "pointer and unknown arguments/results require a semantic comparator; "
            "raw process addresses are never considered equivalent",
        ],
        "summary": {
            "natives": len(results),
            "observedInRetail": len(observed),
            "observedInCandidate": len(called_both),
            "matched": len(matched),
            "notWiredInHost": len([r for r in observed if not r.get("candidateCalled")]),
            "incomparable": len([r for r in observed if r.get("candidateCalled") and
                                 r.get("argumentsMatch") is not True]),
        },
        "results": results,
    }
    Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    Path(a.out).write_text(json.dumps(doc, indent=2) + "\n")
    print(f"\n{json.dumps(doc['summary'], indent=2)} -> {a.out}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(f"native_runtime: BLOCKED: {error}")
