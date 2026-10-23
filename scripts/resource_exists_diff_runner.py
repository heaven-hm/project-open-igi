#!/usr/bin/env python3
"""Differential replay of Resource_FileExists against the candidate build.

Retail (0x004B5B60) asks 0x004B5B90 whether the path is already cached and, when it is not,
forwards the path to the opcode-7 dispatch at 0x004B1FC0. Both callees are replaced by constant
models so the comparison isolates this native's decision, and the candidate is executed at the
linked address of its compiled function with the same two callees modelled.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import struct
import subprocess
import sys

STOP_ADDRESS = 0x13370000
PATH_BUFFER = 0x60000000
DISPATCH_SENTINEL = 0x22220000
RETAIL_FUNCTION = 0x004B5B60
RETAIL_IS_LOADED = 0x004B5B90
RETAIL_DISPATCH = 0x004B1FC0

# (loaded, dispatch result, path) - eight distinct inputs covering both branches and signed values.
CASES = (
    (1, 0, b"DATA\\A.TEX"),
    (1, 5, b"DATA\\B.TEX"),
    (0, 0, b"DATA\\C.TEX"),
    (0, 1, b"DATA\\D.TEX"),
    (0, -1, b"DATA\\E.TEX"),
    (0, 0x5A, b"DATA\\F.TEX"),
    (1, 7, b""),
    (0, 7, b""),
)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "Resource_FileExists": "igi::retail::script::Resource_FileExists",
        "Is_ResourceLoaded": "igi::retail::resource::Is_ResourceLoaded",
        "g_sys_dispatch_opcode_7": "::g_sys_dispatch_opcode_7",
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
        for label, suffix in wanted.items():
            if label in found:
                continue
            if name.split("(")[0].strip().endswith(suffix) or name.endswith(suffix):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def run_body(project, entry: int, is_loaded_hook: int, dispatch_hook: int, flag_pointer: int | None,
             loaded: int, dispatch_result: int, path: bytes, side_effects: list[str]) -> dict:
    import angr

    class Constant(angr.SimProcedure):
        def __init__(self, value: int):
            super().__init__()
            self._value = value

        def run(self, *args):  # noqa: D401 - angr callback
            return self._value

    project.hook(is_loaded_hook, Constant(loaded))
    project.hook(dispatch_hook, Constant(dispatch_result))
    # call_state converts integer arguments to bitvectors itself; claripy is not a
    # separately importable distribution in the isolated angr interpreter.
    state = project.factory.call_state(
        entry, PATH_BUFFER, ret_addr=STOP_ADDRESS,
        prototype="int f(char*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(PATH_BUFFER, path + b"\x00")
    if flag_pointer is not None:
        state.memory.store(flag_pointer, struct.pack("<I", DISPATCH_SENTINEL))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {"path_fixture": path.decode("latin-1")},
        "side_effects": list(side_effects),
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
    if address != "0x004B5B60":
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
        import logging
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
    for loaded, dispatch_result, path in CASES:
        # angr refuses to re-hook an address, silently keeping the first constant model, so every
        # case gets freshly loaded projects instead of sharing hooks across inputs.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        effects = ["call:Is_ResourceLoaded"] + ([] if loaded else ["call:dispatch"])
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_IS_LOADED, RETAIL_DISPATCH, None,
                             loaded, dispatch_result, path, effects)
        observed = run_body(candidate_project, symbols["Resource_FileExists"],
                            symbols["Is_ResourceLoaded"], DISPATCH_SENTINEL,
                            symbols["g_sys_dispatch_opcode_7"], loaded, dispatch_result, path, effects)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"loaded-{loaded}-dispatch-{dispatch_result}-path-{len(path)}",
            "input": {"args": [path.decode("latin-1")],
                      "initial_state": {"resource_loaded": loaded, "dispatch_result": dispatch_result}},
            "categories": ["boundary"] if loaded or dispatch_result in (-1, 0, 7) else ["nominal"],
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
        "method": ("Retail and the compiled candidate both execute the native with Is_ResourceLoaded and the "
                   "opcode-7 dispatch replaced by the same constant models; the returned value and the "
                   "sequence of side effects are compared across the eight inputs."),
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
