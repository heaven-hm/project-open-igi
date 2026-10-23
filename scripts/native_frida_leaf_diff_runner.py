#!/usr/bin/env python3
"""Per-native Frida differential for the retail natives against the port.

The retail game and the reconstruction run the same scene with the same input,
each with a probe on one native: the retail virtual address on the retail side
and the candidate symbol on the port side. Both sides report the calls that
naturally happened, and the runner compares them.

The generic observer supports only reviewed scalar ABIs with a matching probe
contract that explicitly proves absence of side effects. Unknown ABIs and
pointer/state effects require per-native typed probes and block acceptance.

    python3 scripts/native_frida_leaf_diff_runner.py --address 0x00405850 ...
"""

import argparse
import importlib.util
import hashlib
import json
import os
import pathlib
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

import verification_scope  # noqa: E402
from verification_scope import validate_native_symbol_binding

spec = importlib.util.spec_from_file_location(
    "nfd", ROOT / "scripts" / "native_frida_diff.py")
nfd = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nfd)

SYMBOL_MAP = ROOT / "assets" / "candidate_symbol_map.json"
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
RETAIL_NAME = "igi.exe"
CANDIDATE_NAME = "open-igi.exe"


def load_symbols() -> dict:
    entries = json.loads(SYMBOL_MAP.read_text())
    for entry in entries:
        validate_native_symbol_binding(entry)
    return {entry["address"].lower(): entry for entry in entries}


def load_signature(address: str) -> str:
    manifest = json.loads(MANIFEST.read_text())
    for record in manifest.get("natives", []):
        if record.get("address", "").lower() == address.lower():
            return (record.get("implementation") or {}).get("signature", "unknown")
    return "unknown"


def observe(executable: str, process_name: str, addresses: list, level: int,
            game_directory: pathlib.Path, observe_seconds: float, settle: float,
            probe_source: str, escape_source: str) -> dict:
    """Attach and clean up only the exact PID returned by our Frida spawn."""
    import frida
    from frida_dynobj_link_dynamic_cube_pair import process_names
    device = frida.get_device_manager().add_remote_device(os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))
    if process_names(device):
        raise RuntimeError("pre-existing game is active; refusing to attach or terminate it")
    cwd = "Z:\\" + str(game_directory.resolve())[1:].replace("/", "\\")
    pid = device.spawn([executable, "window", f"level{level}"], cwd=cwd)
    session = None
    try:
        session = device.attach(pid)
        escape_script = session.create_script(escape_source)
        escape_script.load()
        device.resume(pid)
        time.sleep(settle)
        if pid not in {p.pid for p in device.enumerate_processes()}:
            raise RuntimeError("owned game exited before observation")
        escape = escape_script.exports_sync.press_escape()
        if escape.get('sent') is not True:
            raise RuntimeError('Escape was not delivered to the owned game window')
        observations = nfd.run_observation(pid, addresses, observe_seconds, probe_source)
        if not observations:
            raise RuntimeError("owned game observation is empty")
        observations['__scenario'] = {'escape': escape, 'owned_pid': pid, 'startup_calls_observed': False}
        return observations
    finally:
        try:
            device.kill(pid)
        except frida.ProcessNotFoundError:
            pass
        if session is not None:
            try:
                session.detach()
            except frida.InvalidOperationError:
                pass
        if pid in {p.pid for p in device.enumerate_processes()}:
            raise RuntimeError("owned game remains alive; refusing the next capture")


def calls_for(observations: dict, address: str) -> list:
    """Select an address once, preserving its observed call order."""
    wanted = address.lower()
    return [call for key, values in observations.items() if key.lower() == wanted for call in values]


def validate_leaf_inputs(arguments, entry, signature, probe_digest=None):
    """Reject unsupported ABI/effects and stale identities before starting a game."""
    root = arguments.root.resolve()
    source = arguments.implementation.resolve()
    if (verification_scope.file_digest(arguments.binary) != arguments.binary_sha256 or
            verification_scope.file_digest(source) != arguments.implementation_sha256):
        raise ValueError("retail or implementation hash differs from the gate inputs")
    records = json.loads((root / "docs/fidelity/native-verification.json").read_text())["natives"]
    record = next(r for r in records if int(r["address"], 16) == int(arguments.address, 16))
    review = record.get("verification", {}).get("semantic_review", {})
    review_path = (root / review.get("evidence_path", "")).resolve()
    if (not review_path.is_relative_to(root / "docs/fidelity/evidence") or not review_path.is_file()
            or verification_scope.file_digest(review_path) != review.get("evidence_sha256")):
        raise ValueError("a current source-semantic review is required before runtime acceptance")
    contract = json.loads(review_path.read_text()).get("runtime_contract", {})
    probe_hash = probe_digest or verification_scope.file_digest(nfd.PROBE)
    scalar_return, argument_types = nfd.scalar_abi_types(signature)
    if not scalar_return or argument_types is None:
        raise ValueError("unknown/pointer ABI requires a typed per-native Frida probe; raw words cannot pass")
    return_type = signature.split("(", 1)[0].rsplit(None, 1)[0].strip()
    word_types = {"int32_t", "uint32_t", "int", "long", "size_t"}
    if return_type not in word_types or any(kind not in word_types for kind in argument_types):
        raise ValueError("narrow/unknown scalar ABI requires a typed probe; raw EAX/words include unspecified bits")
    if contract.get("probe_sha256") != probe_hash or contract.get("no_side_effects") is not True:
        raise ValueError("generic scalar probe requires a reviewed matching probe and proven absence of side effects")
    candidate = pathlib.Path(os.environ.get("IGI_CANDIDATE_EXE", str(
        root / "build-win32-no-verify/engine/platform/windows/open-igi.exe"))).resolve()
    from candidate_build_receipt import validate_receipt
    receipt = pathlib.Path(os.environ.get("IGI_CANDIDATE_BUILD_RECEIPT", str(
        root / "build/diagnostic-candidate-build.json")))
    validate_receipt(receipt, root, candidate, [source])
    symbol = entry.get("candidateSymbol", {}).get("symbol")
    if not symbol:
        raise ValueError("candidate native has no symbol identity")
    table = subprocess.run(["i686-w64-mingw32-nm", "--defined-only", str(candidate)],
                           text=True, capture_output=True, check=True, timeout=60).stdout
    import re
    found = re.search(r"^([0-9a-fA-F]+) T " + re.escape(symbol) + r"$", table, re.M)
    if found is None:
        raise ValueError("native symbol is absent from the newly compiled candidate")
    return candidate, f"0x{int(found.group(1), 16):08X}", len(argument_types)


def typed_observation(calls: list, scalar: bool, argument_count: int = 0) -> dict:
    """The comparable part of the observed calls.

    With a scalar signature the argument words and the result are exact. Without
    one the raw words are addresses from two different processes, so only the
    number of calls and whether the result was null are comparable.
    """
    if scalar:
        return {
            "scope": "scalar-arguments-and-result",
            "inputs": [[call.get("args", [])[index] for index in range(argument_count)] for call in calls],
            "outputs": [call.get("retval") for call in calls],
            "sideEffects": [],
        }
    raise ValueError("pointer/unknown ABI needs a typed comparator; counts and returns cannot pass")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--level", type=int, default=4)
    parser.add_argument("--observe", type=float, default=20.0)
    parser.add_argument("--settle", type=float, default=40.0)
    arguments = parser.parse_args()

    address = "0x" + arguments.address[-8:].upper()
    symbols = load_symbols()
    entry = symbols.get(address.lower())
    if entry is None or not entry.get("implemented"):
        raise SystemExit(f"unsupported native address: {arguments.address}")

    game_directory = arguments.binary.resolve().parent
    retail_calls_address = [address]
    signature = load_signature(address)
    probe_bytes = nfd.PROBE.read_bytes()
    probe_source = probe_bytes.decode('utf-8')
    probe_digest = hashlib.sha256(probe_bytes).hexdigest()
    escape_path = ROOT / 'scripts/frida_dynobj_link_dynamic_cube_probe.js'
    escape_bytes = escape_path.read_bytes()
    escape_source = escape_bytes.decode('utf-8')
    escape_digest = hashlib.sha256(escape_bytes).hexdigest()
    candidate_executable, candidate_address, argument_count = validate_leaf_inputs(arguments, entry, signature, probe_digest)
    candidate_digest = verification_scope.file_digest(candidate_executable)
    scalar = True
    try:
        import frida
    except ImportError:
        python = pathlib.Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python"
        os.execv(str(python), [str(python), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])

    retail = observe("Z:" + str(arguments.binary.resolve()).replace("/", "\\"),
                     RETAIL_NAME, retail_calls_address, arguments.level, game_directory,
                     arguments.observe, arguments.settle, probe_source, escape_source)
    with verification_scope.stage_candidate(candidate_executable, game_directory) as staged:
        candidate = observe("Z:" + str(staged).replace("/", "\\"), staged.name.lower(),
                            [candidate_address], arguments.level, game_directory,
                            arguments.observe, arguments.settle, probe_source, escape_source)

    if (verification_scope.file_digest(nfd.PROBE) != probe_digest or
            verification_scope.file_digest(escape_path) != escape_digest or
            verification_scope.file_digest(candidate_executable) != candidate_digest or
            verification_scope.file_digest(arguments.implementation) != arguments.implementation_sha256 or
            verification_scope.file_digest(arguments.binary) != arguments.binary_sha256):
        raise ValueError("probe, candidate, source or retail identity changed during capture")
    retail_calls = calls_for(retail, address)
    candidate_calls = calls_for(candidate, candidate_address)
    if not retail_calls:
        raise SystemExit(f"the retail game did not call {address}")
    if not candidate_calls:
        raise SystemExit(f"the candidate did not call {entry['cppName']}")

    retail_typed = typed_observation(retail_calls, scalar, argument_count)
    candidate_typed = typed_observation(candidate_calls, scalar, argument_count)
    matched = retail_typed == candidate_typed

    cases = [{
        "id": f"{address}-natural-calls",
        "categories": ["runtime"],
        "input": {"args": [], "initial_state": {}},
        "reference_output": retail_typed,
        "candidate_output": candidate_typed,
    }]
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if matched else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "method": (
            "The retail igi.exe and the candidate open-igi.exe ran the same scene with the "
            "same input, with a probe on the retail virtual address and on the candidate "
            "symbol. The naturally occurring calls were compared."
        ),
        "observable_fields": ["inputs", "outputs", "sideEffects"],
        "scope": retail_typed["scope"],
        "probe_sha256": probe_digest,
        "raw_observations": {"retail": retail, "candidate": candidate},
        "escape_helper_sha256": escape_digest,
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": len(cases) if matched else 0,
        "tests_failed": 0 if matched else len(cases),
        "cases": cases,
        "retail_execution": {
            "engine": "frida",
            "tool": "frida",
            "version": "generic-leaf-probe",
            "analysis": f"retail igi.exe at {address}",
            "function_entry": address,
            "candidate_target": CANDIDATE_NAME,
            "candidate_sha256": candidate_digest,
            "binary_path": str(arguments.binary),
            "binary_sha256": arguments.binary_sha256,
            "helper_path": "scripts/frida_native_diff.js",
            "helper_sha256": probe_digest,
        },
        "limitations": [
            "The probe records natural calls only; it never replays retail arguments in the "
            "candidate.",
            "Only reviewed 32-bit scalar ABIs with proven absence of side effects are supported.",
            "Late attach misses startup calls; absent calls block and cannot validate startup behavior.",
        ],
    }
    print(json.dumps(report))
    return 0 if matched else 1


if __name__ == "__main__":
    sys.exit(main())
