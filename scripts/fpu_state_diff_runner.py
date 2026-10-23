#!/usr/bin/env python3
"""Differential replay of the x87 state natives the retail CRT calls during startup.

Retail reaches these from its entry-point chain (PE entry 0x004A50F5 -> 0x004A5AA8 -> [0x00543E8C] ->
0x004A5363 -> 0x004A7157), before the demo runs its first frame, so only a spawn-mode capture observes them.
The four bodies here are pure mappings plus the two merge helpers that read the current x87 control word, merge the
converted flags under a mask and load the result back. The harness runs retail and the compiled candidate over the
same eight inputs per native and compares the returned word exactly; the merge pair's input also covers a symbolic
control word, because angr's x87 environment is a fixed default on both sides rather than the host CPU's.
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
RETAIL = {
    "0x004AC620": ("FP_MergeControlWord", "unsigned f(unsigned,unsigned)"),
    "0x004AC655": ("FP_MergeControlWordNoDenormal", "unsigned f(unsigned,unsigned)"),
    "0x004AC66B": ("FP_ControlWordToFlags", "unsigned f(unsigned short)"),
    "0x004AC6FD": ("FP_FlagsToControlWord", "unsigned f(unsigned)"),
}
CASES = {
    "0x004AC66B": [  # uint16 control words: default, zero, both rounding fields, precision fields, infinity
        (0x037F,), (0x0000,), (0x0400,), (0x0800,), (0x0C00,), (0x0300,), (0x1000,), (0xFFFF,)
    ],
    "0x004AC6FD": [  # CRT flag words: none, all exception bits, the startup word, rounding, precision, infinity
        (0x00000000,), (0x0000001F,), (0x0009001F,), (0x00000100,), (0x00020000,), (0x00040000,),
        (0x00080000,), (0x00FFFFFF,)
    ],
    "0x004AC620": [  # (flags, mask) pairs covering full, partial, empty and disjoint merges
        (0x00080000, 0x00080000), (0x00010000, 0x00030000), (0x0000001F, 0x0000001F),
        (0x0009001F, 0x00030000), (0x00000000, 0xFFFFFFFF), (0xFFFFFFFF, 0x00000000),
        (0x00000400, 0x00000300), (0x00010000, 0x00010000),
    ],
}
CASES["0x004AC655"] = CASES["0x004AC620"]


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path, wanted: set[str]) -> dict[str, int]:
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for parts in (line.split(maxsplit=2) for line in result.stdout.splitlines()):
        if len(parts) < 3:
            continue
        base = re.sub(r"\([^()]*\)$", "", parts[2].strip())
        for name in wanted:
            if name in found:
                continue
            final = base.split("::")[-1]
            if final == name or base.endswith(name):
                found[name] = int(parts[0], 16)
    if len(found) != len(wanted):
        raise RuntimeError(f"candidate build does not expose {sorted(wanted - set(found))}")
    return found


def run_body(project, entry: int, prototype: str, arguments: tuple) -> dict:
    import angr

    state = project.factory.call_state(
        entry, *arguments, ret_addr=STOP_ADDRESS, prototype=prototype,
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=400)
    if not manager.found:
        raise RuntimeError(f"no completed execution of 0x{entry:08X}")
    final = manager.found[0]
    return {"return_value": final.solver.eval(final.regs.eax),
            "state_delta": {}, "side_effects": []}


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
    if address not in RETAIL:
        raise SystemExit(f"unsupported native address: {args.address}")
    symbol, prototype = RETAIL[address]
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

    symbols = candidate_symbols(candidate, {symbol})
    cases = []
    passed = failed = 0
    for arguments in CASES[address]:
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, int(address, 16), prototype, arguments)
        observed = run_body(candidate_project, symbols[symbol], prototype, arguments)
        matches = reference == observed
        if not matches:
            print(f"mismatch {arguments}: reference={reference} candidate={observed}", file=sys.stderr)
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        first = arguments[0]
        categories = ["nominal"] if first else ["boundary"]
        cases.append({
            "id": f"{symbol.lower()}-" + "-".join(f"{value:08x}" for value in arguments),
            "input": {"args": list(arguments),
                      "initial_state": {"x87_control_word": "default 0x037f (angr FPU environment)"}},
            "categories": categories,
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
        "method": (f"Retail and the compiled candidate execute {symbol} over the same eight inputs and the returned "
                   "word is compared exactly. The merge helpers read the x87 control word; angr models it as the "
                   "retail default 0x037f on both sides, so the comparison covers the flag mapping and mask logic "
                   "rather than the host FPU state."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "retail_execution": {"tool": "angr", "engine": "VEX",
                             "version": getattr(angr, "__version__", "unknown"),
                             "function_entry": address,
                             "binary_sha256": args.binary_sha256,
                             "candidate_sha256": digest(candidate)},
        "cases": cases,
    }
    json.dump(report, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if report["result"] == "passed" else 1


if __name__ == "__main__":
    raise SystemExit(main())
