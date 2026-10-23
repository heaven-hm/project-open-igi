#!/usr/bin/env python3
"""Differential replay of TextureModifier_Push against the candidate build.

The native validates a source row selected by the counter, stores the DWORD at +0xE4 onto the host stack, folds the
packed bitmask, sets the +0xDC push flag and returns scratch+1; a rejected source row clears the flag and returns
the object. The replay varies the counter, the scratch value and the row validity, and observes the returned value,
the stack index after the call, the stored value, the mask, the flag and the store side effects on both sides.
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
OFFSET_COUNTER = 0xBC
OFFSET_PUSHED = 0xE4
OFFSET_FLAG = 0xDC
OFFSET_TABLE = 0x68
OFFSET_ROW_POINTER = 0xDCC
TABLE_ADDRESS = 0x6A00
ROW_ADDRESS = 0x6C00

RETAIL_FUNCTION = 0x00505E40
RETAIL_STACK_INDEX = 0x00B97DF8
RETAIL_STACK = 0x00B97DA0
RETAIL_MASK = 0x00BA5060

# (counter value, scratch value, source row accepts the push)
CASES = ((0, 0, True), (0, 1, True), (1, 2, True), (2, 3, True), (0, 7, True), (1, 0x1234, True),
         (0, 0, False), (3, 5, True))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "TextureModifier_Push",
        "stack_index": "igi::retail::terrain::g_b97df8",
        "stack": "igi::retail::terrain::g_b97da0",
        "mask": "igi::retail::terrain::g_ba5060",
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


def run_body(project, entry: int, stack_index_address: int, stack_address: int, mask_address: int,
             case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    counter, pushed, row_accepts = case
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
    struct.pack_into("<I", block, OFFSET_TABLE, TABLE_ADDRESS)
    state.memory.store(STATE_ADDRESS, bytes(block))
    table = bytearray(0xDD0)
    struct.pack_into("<I", table, OFFSET_ROW_POINTER, ROW_ADDRESS)
    state.memory.store(TABLE_ADDRESS, bytes(table))
    row = [1 if row_accepts else 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0]
    state.memory.store(ROW_ADDRESS, struct.pack(f"<{len(row)}I", *row))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    solver = final.solver
    index = solver.eval(final.memory.load(stack_index_address, 4, endness=endness))
    stored = solver.eval(final.memory.load(stack_address, 4, endness=endness))
    flag = solver.eval(final.memory.load(STATE_ADDRESS + OFFSET_FLAG, 4, endness=endness))
    mask = solver.eval(final.memory.load(mask_address, 4, endness=endness))
    return {
        "return_value": signed32(solver.eval(final.regs.eax)),
        "state_delta": {"stack_index": index, "pushed_value": signed32(stored),
                        "mask": mask, "flag_0xdc": flag},
        "side_effects": ["store:stack", "write:flag"] if row_accepts else [],
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
    if address != "0x00505E40":
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
    for counter, pushed, row_accepts in CASES:
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        case = (counter, pushed, row_accepts)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_STACK_INDEX, RETAIL_STACK,
                             RETAIL_MASK, case)
        observed = run_body(candidate_project, symbols["function"], symbols["stack_index"],
                            symbols["stack"], symbols["mask"], case)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"counter-{counter}-pushed-{pushed}-row-{int(row_accepts)}",
            "input": {"args": [f"0x{STATE_ADDRESS:04X}"],
                      "initial_state": {"counter": counter, "scratch_value": pushed,
                                        "source_row_accepts": row_accepts}},
            "categories": ["boundary"] if not row_accepts or counter == 3 else ["nominal"],
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
        "method": ("Retail and the compiled candidate execute the push with the same counter, scratch value and "
                   "source row; the returned value, the host stack index after the call, the stored value, the packed "
                   "mask, the +0xDC push flag and the side effects are compared."),
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
