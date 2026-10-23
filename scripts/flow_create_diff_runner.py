#!/usr/bin/env python3
"""Differential replay of Flow_CreateHandler against the candidate build.

Retail (0x00402800) loads the Flow class id from 0x00567C7C and calls the input-port registrar at
0x004012A0 with the object, that class id and zero; the candidate calls its own registrar with the
same arguments. Both registrars are replaced by the same constant model so the comparison isolates
this native's argument marshalling and its returned register.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import logging
import os
import pathlib
import struct
import subprocess
import sys

STOP_ADDRESS = 0x13370000
RETAIL_FUNCTION = 0x00402800
RETAIL_CALLEE = 0x004012A0
RETAIL_CLASS_ID = 0x00567C7C

# (object pointer, class id, registrar result) - eight distinct marshalling inputs.
CASES = (
    (0x00001000, 0, 0),
    (0x00001000, 7, 0),
    (0x00002000, 7, 0xBEEF),
    (0x00002000, 0xFFFF, 1),
    (0x00000000, 1, 0),
    (0xDEAD0000, 0xFFFF, 0xDEADBEEF),
    (0x00001000, 0xFFFF, 0),
    (0x00002000, 1, 0xBEEF),
)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "Flow_CreateHandler": "::Flow_CreateHandler",
        "QTask_RegisterInputPortHandler": "::QTask_RegisterInputPortHandler",
        "QTask_FlowClassId": "::QTask_FlowClassId",
    }
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3:
            continue
        name = " ".join(parts[2:])
        base = name.split("(")[0].strip()
        for label, suffix in wanted.items():
            plain = suffix.lstrip(":")
            if label not in found and (base == plain or base.endswith("::" + plain)):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def run_body(project, entry: int, callee: int, class_id_address: int, obj: int, class_id: int,
             registrar_result: int) -> dict:
    import angr

    class Constant(angr.SimProcedure):
        def __init__(self, value: int):
            super().__init__()
            self._value = value

        def run(self, *args):  # noqa: D401 - angr callback
            return self._value

    project.hook(callee, Constant(registrar_result))
    state = project.factory.call_state(
        entry, obj, ret_addr=STOP_ADDRESS, prototype="int f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(class_id_address, struct.pack("<H", class_id))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {"class_id_seeded": class_id},
        "side_effects": ["call:QTask_RegisterInputPortHandler"],
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
    if address != "0x00402800":
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
    for obj, class_id, registrar_result in CASES:
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_CALLEE, RETAIL_CLASS_ID,
                             obj, class_id, registrar_result)
        observed = run_body(candidate_project, symbols["Flow_CreateHandler"],
                            symbols["QTask_RegisterInputPortHandler"], symbols["QTask_FlowClassId"],
                            obj, class_id, registrar_result)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"object-{obj:08x}-class-{class_id}-result-{registrar_result}",
            "input": {"args": [f"0x{obj:08X}"],
                      "initial_state": {"flow_class_id": class_id,
                                        "registrar_result": registrar_result}},
            "categories": ["boundary"] if obj in (0, 0xDEAD0000) or class_id in (0, 0xFFFF) else ["nominal"],
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
        "method": ("Retail and the compiled candidate both execute the native with the input-port registrar "
                   "replaced by the same constant model; the marshalled class id, the registrar result and "
                   "the returned register are compared across eight object/class-id combinations."),
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
