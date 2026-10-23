#!/usr/bin/env python3
"""Differential replay of the upload-flag store natives against the candidate build.

Both sides execute real x86: the retail image at its own entry point and the candidate
executable at the linked address of its compiled function. The only inputs are the flag
byte's initial value and the caller's EAX, so the report isolates the store.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import subprocess
import sys

STOP_ADDRESS = 0x13370000
SEEDED_EAX = 0x12345678
# Retail store targets, read from the two 6-byte bodies:
#   0x004B26A0  mov byte [0x943b3c], 1 ; ret
#   0x004B26B0  mov byte [0x943b3c], 0 ; ret
RETAIL_FLAG_ADDRESS = 0x00943B3C
NATIVES = {
    "0x004B26A0": ("Texture_EnableUpload", 1),
    "0x004B26B0": ("Texture_DisableUpload", 0),
}
FLAG_VALUES = (0, 1, 2, 0x55, 0x7F, 0x80, 0xAA, 0xFF)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path, function: str) -> tuple[int, int]:
    """Linked addresses of the compiled function and its translation-unit flag."""
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    function_address = None
    flag_address = None
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3:
            continue
        name = " ".join(parts[2:])
        # The flag lives in an anonymous namespace, so its demangled name contains a space and
        # parentheses; match the suffix on the full name rather than on the argument-less base.
        if function_address is None and name.split("(")[0].strip().endswith("::" + function):
            function_address = int(parts[0], 16)
        if flag_address is None and name.endswith("::g_943b3c"):
            flag_address = int(parts[0], 16)
    if function_address is None or flag_address is None:
        raise RuntimeError("candidate build does not expose the upload-flag native or its global")
    return function_address, flag_address


def run_body(project, entry: int, flag_address: int, flag_value: int) -> dict:
    import angr

    state = project.factory.call_state(
        entry, ret_addr=STOP_ADDRESS, prototype="void f()",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(flag_address, bytes([flag_value]))
    state.regs.eax = SEEDED_EAX
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    flag_after = final.solver.eval(final.memory.load(flag_address, 1))
    return {
        "return_value": final.solver.eval(final.regs.eax),
        "state_delta": {"upload_flag": flag_after},
        "side_effects": [f"write:0x{RETAIL_FLAG_ADDRESS:08X}"],
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--candidate", type=pathlib.Path)
    args = parser.parse_args()

    address = "0x" + args.address[-8:].upper()
    if address not in NATIVES:
        raise SystemExit(f"unsupported native address: {args.address}")
    name, stored_value = NATIVES[address]
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")
    candidate = (args.candidate or root / "build-win32-verify-noinline" / "igi1.exe").resolve()
    if not candidate.is_file():
        raise SystemExit(f"candidate build is missing: {candidate}")

    try:
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])

    retail = angr.Project(str(binary), auto_load_libs=False)
    rebuilt = angr.Project(str(candidate), auto_load_libs=False)
    candidate_entry, candidate_flag = candidate_symbols(candidate, name)
    retail_entry = int(address, 16)

    cases = []
    passed = failed = 0
    for flag_value in FLAG_VALUES:
        reference = run_body(retail, retail_entry, RETAIL_FLAG_ADDRESS, flag_value)
        observed = run_body(rebuilt, candidate_entry, candidate_flag, flag_value)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"initial-flag-{flag_value}",
            "input": {"args": [], "initial_state": {"upload_flag": flag_value, "eax": SEEDED_EAX}},
            "categories": ["boundary"] if flag_value in (0, 1, 0x7F, 0x80, 0xFF) else ["nominal"],
            "reference_output": reference,
            "candidate_output": observed,
        })
        if reference["state_delta"]["upload_flag"] != stored_value:
            raise RuntimeError("retail image did not perform the expected store; the flag address is stale")

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 and passed == len(cases) else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": ("Retail and the compiled candidate both execute the native body in angr with the "
                   "upload-flag byte and caller EAX seeded; the flag byte after the store, the final "
                   "EAX and the write side effect are compared."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "retail_execution": {
            "tool": "angr",
            "engine": "VEX",
            "version": getattr(angr, "__version__", "unknown"),
            "function_entry": address,
            "binary_sha256": args.binary_sha256,
            "candidate_sha256": digest(candidate),
        },
        "cases": cases,
    }
    json.dump(report, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if report["result"] == "passed" else 1


if __name__ == "__main__":
    raise SystemExit(main())
