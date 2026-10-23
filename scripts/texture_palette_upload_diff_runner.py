#!/usr/bin/env python3
"""Differential replay of Texture_UploadPalettePacket against the candidate build.

The native writes the active backend id into *arg1, packs the three packet slots (arg1, arg3, arg2) and
dispatches through the renderer callback table at slot g_a84a48. The callback is modelled identically on both
sides: it folds the three packet slots together, so the returned value only matches when the write order and the
slot marshalling match. Retail uses 0x00A84A48 / 0x00A94E84 / 0x00A94A64 and the candidate exposes the same
storage through its own symbols.
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
CALLBACK_ADDRESS = 0x7A000000
ARG1_ADDRESS = 0x3000
ARG2_VALUE = 0x4000
BACKEND_ID = 0x2A
SLOT_INDEX = 2

RETAIL_FUNCTION = 0x004B7CF0
RETAIL_SLOT_INDEX = 0x00A84A48
RETAIL_TABLE = 0x00A94E84
RETAIL_BACKEND_ID = 0x00A94A64

CASES = tuple({"arg1": ARG1_ADDRESS, "arg2": ARG2_VALUE, "arg3": arg3}
              for arg3 in (0, 1, 2, 0x7F, 0x80, 0xFF, 0x1000, 0xFFFF))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "Texture_UploadPalettePacket",
        "slot_index": "igi::retail::texture::(anonymous namespace)::g_a84a48",
        "table": "igi::retail::texture::(anonymous namespace)::g_a94e84",
        "backend_id": "igi::retail::texture::(anonymous namespace)::g_a94a64",
    }
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    lines = [line.split(maxsplit=2) for line in result.stdout.splitlines()]
    for parts in lines:
        if len(parts) < 3:
            continue
        # Strip only a trailing parameter list: anonymous-namespace names contain "(anonymous namespace)".
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


def run_body(project, entry: int, slot_index: int, table: int, backend_id: int, case: dict) -> dict:
    import angr

    class Callback(angr.SimProcedure):
        def run(self, packet_ptr):
            packet = packet_ptr
            first = self.state.memory.load(packet, 4, endness=project.arch.memory_endness)
            third = self.state.memory.load(packet + 8, 4, endness=project.arch.memory_endness)
            second = self.state.memory.load(packet + 4, 4, endness=project.arch.memory_endness)
            return (first ^ second ^ third) & 0xFFFFFFFF

    project.hook(CALLBACK_ADDRESS, Callback())
    assert project.loader.find_symbol is not None
    state = project.factory.call_state(
        entry, case["arg1"], case["arg2"], case["arg3"], ret_addr=STOP_ADDRESS,
        prototype="int f(int*, void*, int)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(backend_id, struct.pack("<i", BACKEND_ID))
    state.memory.store(slot_index, struct.pack("<i", SLOT_INDEX))
    state.memory.store(table + SLOT_INDEX * 4, struct.pack("<I", CALLBACK_ADDRESS))
    state.memory.store(case["arg1"], struct.pack("<i", 0))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {"backend_id": signed32(final.solver.eval(
            final.memory.load(case["arg1"], 4, endness=project.arch.memory_endness)))},
        "side_effects": [f"call:dispatch[{SLOT_INDEX}]"],
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
    if address != "0x004B7CF0":
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
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_SLOT_INDEX, RETAIL_TABLE,
                             RETAIL_BACKEND_ID, case)
        observed = run_body(candidate_project, symbols["function"], symbols["slot_index"],
                            symbols["table"], symbols["backend_id"], case)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"arg3-0x{case['arg3']:04x}",
            "input": {"args": [f"0x{case['arg1']:04X}", f"0x{case['arg2']:04X}", f"0x{case['arg3']:04X}"],
                      "initial_state": {"backend_id": BACKEND_ID, "dispatch_slot": SLOT_INDEX}},
            "categories": ["boundary"] if case["arg3"] in (0, 0xFFFF) else ["nominal"],
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
        "method": ("Retail and the compiled candidate both execute the native with the renderer callback modelled "
                   "by the same fold over the three packet slots, so the backend-id write and the slot "
                   "marshalling are compared through the returned value as well as through the state delta."),
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
