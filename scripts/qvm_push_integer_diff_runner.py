#!/usr/bin/env python3
"""Differential replay of Qvm_PushInteger against the candidate build.

The native decrements both stack indexes, writes 1 into the type stack and the pushed value into the value
stack. Both sides execute against a state block laid out per the documented retail offsets, so the comparison
covers the two index writes, the type slot, the value slot and the value the caller receives in EAX.
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
VALUE_STACK = 0x6200
TYPE_STACK = 0x6400
STACK_DEPTH = 8

# Documented retail offsets from QvmNativeStatePrefix.
OFFSET_VALUE_INDEX = 0x18
OFFSET_TYPE_INDEX = 0x1C
OFFSET_VALUE_STACK = 0x24
OFFSET_TYPE_STACK = 0x28

RETAIL_FUNCTION = 0x004BDA00
CASES = tuple({"value": value} for value in (1, 2, 3, 10, 11, 12, 13, 14))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {"function": "Qvm_PushInteger"}
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


def run_body(project, entry: int, case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    state = project.factory.call_state(
        entry, STATE_ADDRESS, case["value"], ret_addr=STOP_ADDRESS,
        prototype="void f(void*, unsigned int)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    block = bytearray(0x40)
    struct.pack_into("<I", block, OFFSET_VALUE_INDEX, STACK_DEPTH)
    struct.pack_into("<I", block, OFFSET_TYPE_INDEX, STACK_DEPTH)
    struct.pack_into("<I", block, OFFSET_VALUE_STACK, VALUE_STACK)
    struct.pack_into("<I", block, OFFSET_TYPE_STACK, TYPE_STACK)
    state.memory.store(STATE_ADDRESS, bytes(block))
    state.memory.store(VALUE_STACK, bytes(0x40))
    state.memory.store(TYPE_STACK, bytes(0x40))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    solver = final.solver
    value_index = signed32(solver.eval(final.memory.load(STATE_ADDRESS + OFFSET_VALUE_INDEX, 4, endness=endness)))
    type_index = signed32(solver.eval(final.memory.load(STATE_ADDRESS + OFFSET_TYPE_INDEX, 4, endness=endness)))
    slot_type = solver.eval(final.memory.load(TYPE_STACK + type_index * 4, 4, endness=endness))
    slot_value = solver.eval(final.memory.load(VALUE_STACK + value_index * 4, 4, endness=endness))
    return {
        "return_value": signed32(solver.eval(final.regs.eax)),
        "state_delta": {
            "value_stack_index": value_index,
            "type_stack_index": type_index,
            "type_slot": slot_type,
            "value_slot": slot_value,
        },
        "side_effects": ["write:value_stack", "write:type_stack"],
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
    if address != "0x004BDA00":
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
    for case in CASES:
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, case)
        observed = run_body(candidate_project, symbols["function"], case)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"push-{case['value']}",
            "input": {"args": [f"0x{STATE_ADDRESS:04X}", str(case["value"])],
                      "initial_state": {"value_stack_index": STACK_DEPTH, "type_stack_index": STACK_DEPTH}},
            "categories": ["boundary"] if case["value"] in (1, 14) else ["nominal"],
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
        "method": ("Retail and the compiled candidate execute the push against a state block at the documented "
                   "offsets; the comparison covers both stack indexes, the type slot, the value slot and EAX."),
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
