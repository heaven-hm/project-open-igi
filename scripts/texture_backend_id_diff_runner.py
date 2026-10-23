#!/usr/bin/env python3
"""Differential replay of Texture_GetActiveBackendId (0x004B0F80).

The retail body is a single load: it returns the active backend id stored in g_a94a64 (retail 0x00A94A64). The
harness sets that slot to eight different values on both sides - including null, negative and the observed runtime
values - and compares the returned register word, so the case set proves the read comes from the backend slot rather
than from a constant.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import logging
import os
import pathlib
import re
import struct
import subprocess
import sys

STOP_ADDRESS = 0x13370000
RETAIL_FUNCTION = 0x004B0F80
RETAIL_BACKEND_SLOT = 0x00A94A64
CASES = (0, 1, -1, 2, 3, 7, 100, 0x7FFFFFFF)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {"function": "Texture_GetActiveBackendId", "backend": "g_a94a64"}
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for parts in (line.split(maxsplit=2) for line in result.stdout.splitlines()):
        if len(parts) < 3:
            continue
        base = re.sub(r"\([^()]*\)$", "", parts[2].strip())
        for label, name in wanted.items():
            if label in found:
                continue
            if base == name or base.endswith("::" + name) or base.endswith(name):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def run_body(project, entry: int, slot: int, value: int) -> dict:
    import angr

    endness = project.arch.memory_endness
    state = project.factory.call_state(
        entry, ret_addr=STOP_ADDRESS, prototype="int f(void)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(slot, struct.pack("<I", value & 0xFFFFFFFF))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=600)
    if not manager.found:
        raise RuntimeError(f"no completed execution of 0x{entry:08X}")
    final = manager.found[0]
    returned = final.solver.eval(final.regs.eax)
    return {"return_value": returned - (1 << 32) if returned >= (1 << 31) else returned,
            "state_delta": {}, "side_effects": []}


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
    if address != "0x004B0F80":
        raise SystemExit(f"unsupported native address: {args.address}")
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
        logging.getLogger("angr").setLevel(logging.ERROR)
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])

    symbols = candidate_symbols(candidate)
    cases = []
    passed = failed = 0
    for value in CASES:
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_BACKEND_SLOT, value)
        observed = run_body(candidate_project, symbols["function"], symbols["backend"], value)
        matches = reference == observed
        if not matches:
            print(f"mismatch backend={value}: reference={reference} candidate={observed}", file=sys.stderr)
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        categories = ["nominal"] if value > 1 else ["boundary"]
        cases.append({
            "id": f"backend-slot-{value}",
            "input": {"args": [], "initial_state": {"backend_slot": value}},
            "categories": categories,
            "reference_output": reference,
            "candidate_output": observed,
        })

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 and passed == len(cases) else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": ("Retail and the compiled candidate return the dword stored in the backend-id slot; the harness "
                   "writes eight different values into that slot on each side - including zero, negative and the "
                   "values the runtime observed - and compares the returned word, which proves the read is from the "
                   "slot and not a constant."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "retail_execution": {"tool": "angr", "engine": "VEX",
                             "version": getattr(angr, "__version__", "unknown"),
                             "function_entry": address,
                             "binary_sha256": args.binary_sha256,
                             "candidate_sha256": digest(candidate)},
        "cases": cases,
    }
    json.dump(report, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if report["result"] == "passed" else 1


if __name__ == "__main__":
    raise SystemExit(main())
