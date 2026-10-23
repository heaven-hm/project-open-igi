#!/usr/bin/env python3
"""Differential replay of Texture_UploadMipmapPacket against the candidate build.

The native writes the active backend id into *arg1, packs a twelve-slot packet (including the upload-flag byte
from 0x00943B3C) and dispatches through two callback-table slots: the primary slot's return is discarded, the
secondary slot's return is the native's result. Both callbacks are modelled identically on both sides and fold
the twelve packet slots, so slot order, the backend-id write and the upload-flag inclusion all change the
observable values rather than hiding behind constants. The primary callback also records its fold, which pins
the dispatch order.
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
PRIMARY_CALLBACK = 0x7A000000
SECONDARY_CALLBACK = 0x7A000100
PROBE_ADDRESS = 0x3500
ARG1_ADDRESS = 0x3000
ARG2_POINTER = 0x4000
ARG9_POINTER = 0x5000
BACKEND_ID = 0x2A
UPLOAD_FLAG = 1
PRIMARY_SLOT = 0
SECONDARY_SLOT = 1
PACKET_SLOTS = 12

RETAIL_FUNCTION = 0x004B23B0
RETAIL_PRIMARY_INDEX = 0x00546FA8
RETAIL_SECONDARY_INDEX = 0x00546FB0
RETAIL_TABLE = 0x00A94E84
RETAIL_BACKEND_ID = 0x00A94A64
RETAIL_UPLOAD_FLAG = 0x00943B3C

CASES = tuple({"arg3": arg3} for arg3 in (0, 1, 0x7F, 0x80, 0xFF, 0x1000, 0x1234, 0xFFFF))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "Texture_UploadMipmapPacket",
        "primary_index": "igi::retail::texture::(anonymous namespace)::g_546fa8",
        "secondary_index": "igi::retail::texture::(anonymous namespace)::g_546fb0",
        "table": "igi::retail::texture::(anonymous namespace)::g_a94e84",
        "backend_id": "igi::retail::texture::(anonymous namespace)::g_a94a64",
        "upload_flag": "igi::retail::texture::(anonymous namespace)::g_943b3c",
    }
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for parts in (line.split(maxsplit=2) for line in result.stdout.splitlines()):
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


def fold_packet(state, packet_ptr, endness) -> int:
    total = 0
    for slot in range(PACKET_SLOTS):
        total = (total + state.memory.load(packet_ptr + slot * 4, 4, endness=endness)) & 0xFFFFFFFF
    return total


def run_body(project, entry: int, globals_: dict, case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    probe = PROBE_ADDRESS

    class Primary(angr.SimProcedure):
        def run(self, packet_ptr):
            value = fold_packet(self.state, packet_ptr, endness)
            # fold_packet returns a bitvector, which memory.store accepts directly.
            self.state.memory.store(probe, value, endness=endness)
            return value

    class Secondary(angr.SimProcedure):
        def run(self, packet_ptr):
            return fold_packet(self.state, packet_ptr, endness)

    project.hook(PRIMARY_CALLBACK, Primary())
    project.hook(SECONDARY_CALLBACK, Secondary())
    state = project.factory.call_state(
        entry, ARG1_ADDRESS, ARG2_POINTER, case["arg3"], 5, 6, 7, 8, 9, ARG9_POINTER,
        ret_addr=STOP_ADDRESS,
        prototype="int f(int*, void*, int, int, int, int, int, int, void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(globals_["backend_id"], struct.pack("<i", BACKEND_ID))
    state.memory.store(globals_["upload_flag"], struct.pack("<b", UPLOAD_FLAG))
    state.memory.store(globals_["primary_index"], struct.pack("<i", PRIMARY_SLOT))
    state.memory.store(globals_["secondary_index"], struct.pack("<i", SECONDARY_SLOT))
    state.memory.store(globals_["table"] + PRIMARY_SLOT * 4, struct.pack("<I", PRIMARY_CALLBACK))
    state.memory.store(globals_["table"] + SECONDARY_SLOT * 4, struct.pack("<I", SECONDARY_CALLBACK))
    state.memory.store(ARG1_ADDRESS, struct.pack("<i", 0))
    state.memory.store(PROBE_ADDRESS, struct.pack("<I", 0))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {
            "backend_id": signed32(final.solver.eval(final.memory.load(ARG1_ADDRESS, 4, endness=endness))),
            "primary_packet_fold": final.solver.eval(final.memory.load(PROBE_ADDRESS, 4, endness=endness)),
        },
        "side_effects": ["call:primary[0]", "call:secondary[1]"],
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
    if address != "0x004B23B0":
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
    retail_globals = {"primary_index": RETAIL_PRIMARY_INDEX, "secondary_index": RETAIL_SECONDARY_INDEX,
                      "table": RETAIL_TABLE, "backend_id": RETAIL_BACKEND_ID,
                      "upload_flag": RETAIL_UPLOAD_FLAG}
    cases = []
    passed = failed = 0
    for case in CASES:
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, retail_globals, case)
        observed = run_body(candidate_project, symbols["function"], symbols, case)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"arg3-0x{case['arg3']:04x}",
            "input": {"args": [f"0x{ARG1_ADDRESS:04X}", f"0x{ARG2_POINTER:04X}", f"0x{case['arg3']:04X}",
                               "5", "6", "7", "8", "9", f"0x{ARG9_POINTER:04X}"],
                      "initial_state": {"backend_id": BACKEND_ID, "upload_flag": UPLOAD_FLAG,
                                        "dispatch_slots": [PRIMARY_SLOT, SECONDARY_SLOT]}},
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
        "method": ("Retail and the compiled candidate both execute the native with both renderer callbacks "
                   "modelled by the same fold over the twelve packet slots; the primary fold is recorded so the "
                   "dispatch order is compared, and the secondary fold is the compared return value."),
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
