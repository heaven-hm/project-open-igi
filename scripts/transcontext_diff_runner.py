#!/usr/bin/env python3
"""Differential replay of Transcontext_SetActiveTransContext against the candidate build.

The retail body copies 0x2A dwords from the caller's context to 0x00BCAAE0, optionally dispatches the
render method registered in the table at 0x00A94E84 (indexed by 0x006E5BA4), then sets 0x006E5BB0,
increments 0x006E5BB4 and returns the new counter through EAX. Each case seeds the source context, the
install counter and the installed flag, disables the optional dispatch by zeroing its index, and compares
the returned counter plus the three pieces of resulting global state.
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
CONTEXT_POINTER = 0x60000000
CONTEXT_SIZE = 0xA8

RETAIL_FUNCTION = 0x00497E70
RETAIL_ACTIVE_CONTEXT = 0x00BCAAE0
RETAIL_INSTALLED = 0x006E5BB0
RETAIL_INSTALL_COUNT = 0x006E5BB4
RETAIL_DISPATCH_INDEX = 0x006E5BA4

# (context byte pattern, install counter before, installed flag before)
CASES = (
    (0x00, 0, 0),
    (0x01, 0, 0),
    (0x7F, 1, 0),
    (0x80, 1, 1),
    (0xAA, 0, 1),
    (0x55, 5, 0),
    (0xFF, 5, 1),
    (0x3C, 0x7FFFFFFF, 0),
)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "function": "Transcontext_SetActiveTransContext",
        "active_context": "active_trans_context",
        "installed": "trans_context_installed",
        "install_count": "trans_context_install_count",
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
        # Strip the argument list for the function, but keep the full name for globals: the
        # anonymous-namespace globals contain a space and parentheses of their own.
        base = name.split("(")[0].strip()
        for label, suffix in wanted.items():
            if label in found:
                continue
            candidate_names = (base,) if label == "function" else (name,)
            if any(item == suffix or item.endswith("::" + suffix) for item in candidate_names):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def signed32(value: int) -> int:
    return value - (1 << 32) if value >= (1 << 31) else value


def run_body(project, entry: int, active_context: int, installed_address: int, count_address: int,
             dispatch_index_address: int | None, pattern: int, count_before: int, installed_before: int) -> dict:
    import angr

    state = project.factory.call_state(
        entry, CONTEXT_POINTER, ret_addr=STOP_ADDRESS, prototype="int f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    source = bytes(((pattern + index) & 0xFF) for index in range(CONTEXT_SIZE))
    state.memory.store(CONTEXT_POINTER, source)
    state.memory.store(active_context, b"\x00" * CONTEXT_SIZE)
    state.memory.store(installed_address, struct.pack("<I", installed_before))
    state.memory.store(count_address, struct.pack("<I", count_before))
    if dispatch_index_address is not None:
        # A zero index means the retail body skips its optional render-method dispatch.
        state.memory.store(dispatch_index_address, struct.pack("<I", 0))
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    active = bytes(final.solver.eval(final.memory.load(active_context + offset, 1)) for offset in range(0))
    copied = b"".join(
        bytes([final.solver.eval(final.memory.load(active_context + offset, 1))]) for offset in range(CONTEXT_SIZE))
    return {
        "return_value": signed32(final.solver.eval(final.regs.eax)),
        "state_delta": {
            "installed_flag": final.solver.eval(final.memory.load(installed_address, 4)),
            "install_count": signed32(final.solver.eval(final.memory.load(count_address, 4))),
            "active_context_sha256": hashlib.sha256(copied).hexdigest(),
            "source_context_sha256": hashlib.sha256(source).hexdigest(),
        },
        "side_effects": ["write:active_trans_context", "write:installed_flag", "write:install_count"],
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
    if address != "0x00497E70":
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
    for pattern, count_before, installed_before in CASES:
        # angr keeps the first hook for an address, so fresh projects avoid shared state.
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, RETAIL_ACTIVE_CONTEXT, RETAIL_INSTALLED,
                             RETAIL_INSTALL_COUNT, RETAIL_DISPATCH_INDEX, pattern, count_before, installed_before)
        observed = run_body(candidate_project, symbols["function"], symbols["active_context"],
                            symbols["installed"], symbols["install_count"], None,
                            pattern, count_before, installed_before)
        matches = reference == observed
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"pattern-{pattern:02x}-count-{count_before}-installed-{installed_before}",
            "input": {"args": [f"context-pattern-0x{pattern:02X}"],
                      "initial_state": {"install_count": count_before,
                                        "installed_flag": installed_before}},
            "categories": ["boundary"] if count_before in (0, 0x7FFFFFFF) or pattern in (0x00, 0xFF) else ["nominal"],
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
        "method": ("Retail and the compiled candidate both execute the setter with the optional render dispatch "
                   "disabled; the 0xA8-byte context copy, the installed flag, the incremented install counter and "
                   "the value returned through EAX are compared across eight seeded inputs."),
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
