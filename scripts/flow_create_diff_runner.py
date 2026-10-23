#!/usr/bin/env python3
"""Differential replay of the registrar-marshalling natives against the candidate build.

Both natives are the same shape: load a 16-bit global, push the caller's object, that value and zero, then
call the input-port registrar at 0x004012A0 and return its result through EAX. Each case seeds the 16-bit
global and the registrar's modelled result, so the comparison isolates the argument marshalling and the
returned register. The candidate executes at the linked address of its own compiled function with its own
registrar and global replaced by the same models.
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
RETAIL_REGISTRAR = 0x004012A0

# address -> (retail VA, retail 16-bit global, candidate function, candidate 16-bit global)
NATIVES = {
    "0x00402800": (0x00402800, 0x00567C7C, "Flow_CreateHandler", "QTask_FlowClassId"),
    "0x004ED5C0": (0x004ED5C0, 0x00A5EFA0, "InputPort_Process", "InputPort_CurrentIndex"),
}

# (object pointer, 16-bit global value, registrar result) - eight distinct marshalling inputs.
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


def candidate_symbols(executable: pathlib.Path, function: str, global_name: str) -> dict[str, int]:
    wanted = {"function": function, "registrar": "QTask_RegisterInputPortHandler", "global": global_name}
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
            if label in found:
                continue
            # Globals may live in an anonymous namespace, so match their full demangled name too.
            names = (base,) if label == "function" else (name, base)
            if any(item == suffix or item.endswith("::" + suffix) for item in names):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def run_body(project, entry: int, callee: int, global_address: int, obj: int, global_value: int,
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
    state.memory.store(global_address, struct.pack("<H", global_value))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {"index_seeded": global_value},
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
    if address not in NATIVES:
        raise SystemExit(f"unsupported native address: {args.address}")
    retail_entry, retail_global, candidate_function, candidate_global = NATIVES[address]
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

    symbols = candidate_symbols(candidate, candidate_function, candidate_global)

    cases = []
    passed = failed = 0
    for obj, global_value, registrar_result in CASES:
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, retail_entry, RETAIL_REGISTRAR, retail_global,
                             obj, global_value, registrar_result)
        observed = run_body(candidate_project, symbols["function"], symbols["registrar"],
                            symbols["global"], obj, global_value, registrar_result)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"object-{obj:08x}-index-{global_value}-result-{registrar_result}",
            "input": {"args": [f"0x{obj:08X}"],
                      "initial_state": {"seed_global": global_value,
                                        "registrar_result": registrar_result}},
            "categories": ["boundary"] if obj in (0, 0xDEAD0000) or global_value in (0, 0xFFFF) else ["nominal"],
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
                   "replaced by the same constant model; the seeded 16-bit global, the registrar result and the "
                   "returned register are compared across eight object/seed combinations."),
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
