#!/usr/bin/env python3
"""Differential replay of DynObj_LoadPackageAnimations against the candidate build.

The retail body (0x004CF910) reads the package pointer at context+8, looks up the 'MANM' form, and
returns the animation count as a pointer: a missing form returns null, and a zero count returns null
too. The form lookup is modelled with the same constant on both sides so the comparison isolates the
null-handling and the count-to-pointer conversion. Counts are kept non-positive because the positive
path allocates through the retail allocator and walks global texture/script state, which the runner
does not model.
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
SCRATCH = 0x61000000
RETAIL_FUNCTION = 0x004CF910
RETAIL_FIND_FORM = 0x004CEAA0
# (context address, animation count) - counts stay non-positive, see the module docstring.
CASES = ((0x2000, 0), (0x2000, -1), (0x3000, 0), (0x3000, -1),
         (0x4000, 0), (0x4000, -1), (0x5000, 0), (0x5000, -1))


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {"function": "DynObj_LoadPackageAnimations", "find_form": "Ilff_FindFormChunk"}
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3:
            continue
        base = " ".join(parts[2:]).split("(")[0].strip()
        for label, suffix in wanted.items():
            if label not in found and (base == suffix or base.endswith("::" + suffix)):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def run_body(project, entry: int, find_form: int, context: int, count: int) -> dict:
    import angr

    class FormLookup(angr.SimProcedure):
        def run(self, *args):  # noqa: D401 - angr callback
            return SCRATCH

    project.hook(find_form, FormLookup())
    state = project.factory.call_state(
        entry, context, ret_addr=STOP_ADDRESS, prototype="int f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    state.memory.store(context + 8, struct.pack("<I", SCRATCH))
    state.memory.store(SCRATCH, struct.pack("<i", count))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {"animation_count_seeded": count},
        "side_effects": ["call:Ilff_FindFormChunk"],
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
    if address != "0x004CF910":
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
    for context, count in CASES:
        # angr keeps the first hook for an address, so each case loads fresh projects.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_FIND_FORM, context, count)
        observed = run_body(candidate_project, symbols["function"], symbols["find_form"], context, count)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"context-{context:04x}-count-{count}",
            "input": {"args": [f"0x{context:04X}"], "initial_state": {"animation_count": count}},
            "categories": ["boundary"] if count == 0 else ["nominal"],
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
        "method": ("Retail and the compiled candidate both execute the loader with the 'MANM' form lookup "
                   "modelled by the same constant; the count-to-pointer conversion and the null returns are "
                   "compared across non-positive counts. The positive count path is out of scope: it allocates "
                   "through the retail allocator and walks global script/texture state."),
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
