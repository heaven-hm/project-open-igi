#!/usr/bin/env python3
"""Differential replay of Qvm_ExecuteInstructions against the candidate build.

The native clears stop_flag, then loops: on a non-zero error_status it returns that value; otherwise it reads the
opcode byte at code_base + instruction_offset, dispatches handlers[0] for that opcode with (state, 0, opcode, 0)
and repeats while stop_flag is zero. The dispatch handler is modelled identically on both sides: it records the
opcode it received and sets stop_flag, so the opcode fetch, the handler-table index, the argument order and both
exit paths (error and stop) are all observable. The returned value is the terminating field retail leaves in EAX.
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
CODE_ADDRESS = 0x6600
PROBE_ADDRESS = 0x6800
HANDLER_ADDRESS = 0x7B000000

OFFSET_CODE_BASE = 0x00
OFFSET_INSTRUCTION_OFFSET = 0x14
OFFSET_ERROR_STATUS = 0x20
OFFSET_STOP_FLAG = 0x2C
HANDLER_RECORD_SIZE = 24

RETAIL_FUNCTION = 0x004BCFD0
RETAIL_HANDLER_TABLE = 0x00A96108

# (opcode byte, initial error_status)
CASES = ((0, 0), (1, 0), (2, 0), (3, 0), (0x10, 0), (0x21, 0), (0x30, 0), (0, 7))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "Qvm_ExecuteInstructions",
        "handler_table": "igi::retail::script::QvmOpcodeHandlerTable",
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


def run_body(project, entry: int, handler_table: int, case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    opcode, error_status = case
    dispatched = {"opcode": None}

    class Handler(angr.SimProcedure):
        def run(self, state_ptr, argument_2, received_opcode, argument_4):
            self.state.memory.store(PROBE_ADDRESS, received_opcode, size=4, endness=endness)
            self.state.memory.store(state_ptr + OFFSET_STOP_FLAG, 1, size=4, endness=endness)
            return 0

    project.hook(HANDLER_ADDRESS, Handler())
    state = project.factory.call_state(
        entry, STATE_ADDRESS, ret_addr=STOP_ADDRESS,
        prototype="unsigned int f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    block = bytearray(0x40)
    struct.pack_into("<I", block, OFFSET_CODE_BASE, CODE_ADDRESS)
    struct.pack_into("<I", block, OFFSET_INSTRUCTION_OFFSET, 0)
    struct.pack_into("<I", block, OFFSET_ERROR_STATUS, error_status)
    struct.pack_into("<I", block, OFFSET_STOP_FLAG, 0)
    state.memory.store(STATE_ADDRESS, bytes(block))
    state.memory.store(CODE_ADDRESS, bytes([opcode, 0, 0, 0]))
    state.memory.store(PROBE_ADDRESS, struct.pack("<I", 0xFFFFFFFF))
    state.memory.store(handler_table + opcode * HANDLER_RECORD_SIZE,
                       struct.pack("<I", HANDLER_ADDRESS))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    solver = final.solver
    probe = solver.eval(final.memory.load(PROBE_ADDRESS, 4, endness=endness))
    return {
        "return_value": signed32(solver.eval(final.regs.eax)),
        "state_delta": {
            "dispatched_opcode": None if probe == 0xFFFFFFFF else probe,
            "stop_flag": solver.eval(final.memory.load(STATE_ADDRESS + OFFSET_STOP_FLAG, 4, endness=endness)),
            "error_status": solver.eval(
                final.memory.load(STATE_ADDRESS + OFFSET_ERROR_STATUS, 4, endness=endness)),
        },
        "side_effects": [] if error_status else [f"call:handler[{opcode}]"],
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
    if address != "0x004BCFD0":
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
    for opcode, error_status in CASES:
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_HANDLER_TABLE,
                             (opcode, error_status))
        observed = run_body(candidate_project, symbols["function"], symbols["handler_table"],
                            (opcode, error_status))
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"opcode-0x{opcode:02x}-error-{error_status}",
            "input": {"args": [f"0x{STATE_ADDRESS:04X}"],
                      "initial_state": {"code_byte": opcode, "instruction_offset": 0,
                                        "error_status": error_status}},
            "categories": ["boundary"] if error_status or opcode in (0, 0x30) else ["nominal"],
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
        "method": ("Retail and the compiled candidate execute the dispatcher with the opcode handler modelled by "
                   "the same probe; the opcode fetch, handler-table index, handler arguments and both exit paths "
                   "(error_status and stop_flag) are compared, together with the terminating field retail "
                   "leaves in EAX."),
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
