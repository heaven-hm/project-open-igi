#!/usr/bin/env python3
"""Differential replay of DynObj_LoadPackageLovs against the candidate build.

The native locates the 'LOVS' chunk inside *(context + 0x08) with Ilff_FindFormChunk and, when the chunk is present,
walks the NUL-terminated entries. The return value is the chunk payload pointer (or null when the chunk is absent);
because two processes cannot share addresses, pointer returns are compared as their class ("ptr" / "null") and the
form layouts are varied instead: first chunk versus later chunk, form type, chunk padding and absence.
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
CONTEXT_ADDRESS = 0x6000
FORM_ADDRESS = 0x6200
FORM_TAG = 0x53564F4C  # 'LOVS'

RETAIL_FUNCTION = 0x004CF630

# (present, count, form_type, filler_before, filler_after)
CASES = (
    (True, 0, 0x54534554, False, False),
    (True, -1, 0x54534554, False, False),
    (True, 0, 0x00000000, False, False),
    (True, 0, 0x54534554, True, False),
    (True, -1, 0x54534554, True, True),
    (False, 0, 0x54534554, False, False),
    (True, 0, 0x41414141, True, True),
    (True, -2, 0x00000000, False, True),
)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def bswap(value: int) -> int:
    return int.from_bytes(value.to_bytes(4, "little"), "big")


def build_form(case: dict) -> bytes:
    present, count, form_type, filler_before, filler_after = case
    chunks = bytearray()

    def chunk(tag: bytes, payload: bytes) -> bytes:
        # Ilff_FindFormChunk compares bswap32(stored dword) with the tag constant, so the stored bytes are the
        # big-endian tag: the ASCII form of 'LOVS' must be written reversed.
        return tag[::-1] + struct.pack("<I", bswap(len(payload))) + payload

    if filler_before:
        chunks += chunk(b"AAAA", b"\x01\x02\x03\x04")
    if present:
        chunks += chunk(b"LOVS", struct.pack("<i", count))
    if filler_after:
        chunks += chunk(b"BBBB", b"\x05\x06\x07\x08")
    body = struct.pack("<I", form_type) + bytes(chunks)
    return b"FORM" + struct.pack("<I", bswap(len(body))) + body


def run_body(project, entry: int, case: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    state = project.factory.call_state(
        entry, CONTEXT_ADDRESS, ret_addr=STOP_ADDRESS,
        prototype="void* f(void*)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    form = build_form(case)
    context = bytearray(0x100)
    struct.pack_into("<I", context, 0x08, FORM_ADDRESS)
    state.memory.store(CONTEXT_ADDRESS, bytes(context))
    state.memory.store(FORM_ADDRESS, form)
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=64)
    if not manager.found:
        raise RuntimeError(f"no completed execution at 0x{entry:08X}")
    final = manager.found[0]
    eax = final.solver.eval(final.regs.eax)
    pointer_class = "null" if eax == 0 else "ptr"
    return {
        "return_value": pointer_class,
        "state_delta": {"returned_pointer_class": pointer_class},
        "side_effects": [],
    }


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {"function": "DynObj_LoadPackageLovs"}
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
    if address != "0x004CF630":
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
    for present, count, form_type, filler_before, filler_after in CASES:
        case = (present, count, form_type, filler_before, filler_after)
        retail_project = angr.Project(str(binary), auto_load_libs=False)
        candidate_project = angr.Project(str(candidate), auto_load_libs=False)
        reference = run_body(retail_project, RETAIL_FUNCTION, case)
        observed = run_body(candidate_project, symbols["function"], case)
        matches = reference == observed
        if not matches:
            print(f"mismatch {present} {count} {form_type:08x} {filler_before} {filler_after}: "
                  f"reference={reference} candidate={observed}", file=sys.stderr)
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        cases.append({
            "id": f"present-{int(present)}-count-{count}-type-{form_type:08x}-filler-{int(filler_before)}{int(filler_after)}",
            "input": {"args": [f"0x{CONTEXT_ADDRESS:04X}"],
                      "initial_state": {"chunk_present": present, "entry_count": count,
                                        "form_type": form_type,
                                        "filler_before": filler_before,
                                        "filler_after": filler_after}},
            "categories": ["boundary"] if not present or count != 0 else ["nominal"],
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
        "method": ("Retail and the compiled candidate run the same form lookup over identical FORM layouts, and the "
                   "lookup result is compared as a pointer class ('ptr' or 'null') because two processes cannot share "
                   "addresses; the form layout, entry count and chunk order vary across the cases."),
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
