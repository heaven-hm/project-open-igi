#!/usr/bin/env python3
"""Differential replay of TerrainLightMap_PushEntry against the candidate build.

The native reads a parse mode and a DWORD counter, validates the counter, writes the push flag at +0xE0, stores the
DWORD at +0xD8 onto the host stack and returns the new stack index (or the object when the counter is zero). The
replay varies the counter and the pushed value, and observes the returned index, the stack index after the call,
the pushed value and the push flag on both sides.
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
STATE_ADDRESS = 0x6000
OFFSET_COUNTER = 0x108
OFFSET_PUSHED = 0xD8
OFFSET_FLAG = 0xE0

RETAIL_FUNCTION = 0x004F82B0
RETAIL_STACK_INDEX = 0x00B97D24
RETAIL_STACK = 0x00B97B20

# (counter value, pushed value)
CASES = ((1, 0), (2, 7), (3, 0x1234), (4, 0x7FFFFFFF), (5, 0), (6, 0x1234), (9, 7), (0, 0))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "TerrainLightMap_PushEntry",
        "stack_index": "igi::retail::terrain::g_b97d24",
        "stack": "igi::retail::terrain::g_data_b97b20",
    }
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


def run_body(project, entry: int, stack_index_address: int, stack_address: int, case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    counter, pushed = case
    state = project.factory.call_state(
        entry, STATE_ADDRESS, ret_addr=STOP_ADDRESS,
        prototype="void* f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    block = bytearray(0x200)
    struct.pack_into("<I", block, OFFSET_COUNTER, counter)
    struct.pack_into("<I", block, OFFSET_PUSHED, pushed)
    struct.pack_into("<I", block, OFFSET_FLAG, 0xFFFFFFFF)
    state.memory.store(STATE_ADDRESS, bytes(block))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    solver = final.solver
    index = solver.eval(final.memory.load(stack_index_address, 4, endness=endness))
    stored = solver.eval(final.memory.load(stack_address, 4, endness=endness))
    flag = solver.eval(final.memory.load(STATE_ADDRESS + OFFSET_FLAG, 4, endness=endness))
    return {
        "return_value": signed32(solver.eval(final.regs.eax)),
        "state_delta": {"stack_index": index, "pushed_value": signed32(stored),
                        "flag_0xe0": flag},
        "side_effects": ["store:stack"] if counter != 0 else [],
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
    if address != "0x004F82B0":
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
    for counter, pushed in CASES:
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_STACK_INDEX, RETAIL_STACK,
                             (counter, pushed))
        observed = run_body(candidate_project, symbols["function"], symbols["stack_index"],
                            symbols["stack"], (counter, pushed))
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"counter-{counter}-pushed-{pushed}",
            "input": {"args": [f"0x{STATE_ADDRESS:04X}"],
                      "initial_state": {"counter": counter, "pushed_value": pushed}},
            "categories": ["boundary"] if counter in (0, 9) else ["nominal"],
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
        "method": ("Retail and the compiled candidate execute the push with the same counter and pushed value; the "
                   "returned stack index, the host stack index after the call, the stored value and the +0xE0 push "
                   "flag are compared."),
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
