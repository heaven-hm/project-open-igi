#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for the direct context getters.

These natives are one basic block: `mov eax, dword [<global>]; ret`. The retail
side is replayed in radare2 ESIL against the real image, seeding the global with
each probed value; the candidate side runs the actual compiled x86 source with a
matching definition of the same global. Observable: the returned dword only.

A getter has no inputs and no side effects, so the seeded global value is the
only variable. Anything else is reported as an unsupported shape rather than
guessed at.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import subprocess
import sys
import tempfile

# address -> (candidate symbol, global symbol, retail global VA)
TARGETS = {
    "0x0048F230": ("App_ContextGetLightmapsUsed", "g_5c8be4", 0x005C8BE4),
    "0x0048F1B0": ("App_ContextGetDebugged", "g_5c8bf8", 0x005C8BF8),
    "0x0048F1D0": ("Script_GetDebugCompileState", "g_5c8bec", 0x005C8BEC),
    "0x0048F340": ("Script_GetForcePrecompiledState", "g_5c8bf0", 0x005C8BF0),
}

PROBES = [0x00000000, 0x00000001, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000,
          0x00000100, 0x0000FFFF, 0xDEADBEEF, 0x12345678, 0xA5A5A5A5]


def digest(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def retail_return(binary: pathlib.Path, entry: int, global_va: int, value: int) -> int:
    """Replay the getter in retail ESIL with the global seeded to `value`."""
    stack = 0x70008000
    # The global lives in the image's zero-initialised data, which ESIL does not
    # back on its own: map a globals region over it and seed that map, otherwise
    # the emulated load reads zero regardless of the probe value.
    globals_base = global_va & ~0xFFFF
    commands = ";".join([
        "aei", "aeim 0x70000000 0x10000 stack",
        f"aeim {globals_base:#x} 0x30000 globals",
        f"aer esp=0x{stack:08x}",
        f"ae 0x{value & 0xFFFFFFFF:08x},{global_va:#x},=[4]",
        f"s 0x{entry:x}", f"aer eip=0x{entry:x}", f"aesp 0x{entry:x} 4", "aer eax",
    ])
    process = subprocess.run(
        ["r2", "-2", "-q", "-e", "bin.relocs.apply=true", "-e", "asm.arch=x86", "-e", "asm.bits=32", "-c", commands, str(binary)],
        text=True, capture_output=True, timeout=60,
    )
    # r2 prints the bare register value for `aer eax` in this mode, and the
    # verbose form in others; accept either rather than silently reporting zero.
    match = (re.search(r"^eax = 0x([0-9a-fA-F]+)", process.stdout, re.M)
             or re.search(r"^0x([0-9a-fA-F]+)\s*$", process.stdout.strip(), re.M))
    if not match:
        raise RuntimeError(f"retail replay produced no EAX: {process.stdout[-400:]}{process.stderr[-400:]}")
    return int(match.group(1), 16)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=pathlib.Path, required=True)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", type=pathlib.Path, required=True)
    parser.add_argument("--implementation-sha256", required=True)
    args = parser.parse_args()

    address = f"0x{int(args.address, 16):08X}"
    if address not in TARGETS:
        raise SystemExit(f"{address} is not a supported direct context getter")
    symbol, global_symbol, global_va = TARGETS[address]

    binary = args.binary.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 does not match")
    implementation = args.implementation.resolve()
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    cases = []
    with tempfile.TemporaryDirectory(prefix="igi-getter-diff-") as temp:
        work = pathlib.Path(temp)
        harness = work / "harness.cpp"
        harness.write_text(
            "#include <cstdint>\n"
            "#include <cstdlib>\n"
            f"std::int32_t {global_symbol} = 0;\n"
            f"int {symbol}(void);\n"
            "int main(int argc, char** argv) {\n"
            f"    {global_symbol} = static_cast<std::int32_t>(std::strtoul(argv[1], nullptr, 16));\n"
            f"    return {symbol}() & 0xFF;\n"
            "}\n"
        )
        exe = work / "harness.exe"
        built = subprocess.run(
            ["i686-w64-mingw32-g++", "-std=c++20", "-O0", "-w",
             f"-I{args.root.resolve() / 'engine/include'}",
             str(harness), str(implementation), "-o", str(exe)],
            text=True, capture_output=True, timeout=180,
        )
        if built.returncode != 0:
            raise SystemExit(f"candidate compilation failed: {built.stderr[-3000:]}")

        for value in PROBES:
            reference = retail_return(binary, int(address, 16), global_va, value)
            run = subprocess.run(
                ["/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine",
                 "--bottle", "open-igi", "--cx-app",
                 "Z:" + str(exe).replace("/", "\\"), f"{value:x}"],
                text=True, capture_output=True, timeout=180,
            )
            candidate = run.returncode & 0xFF
            # A dword return cannot survive the 8-bit process exit status; compare
            # the low byte and record that scope limit explicitly.
            cases.append({
                "id": f"seed-{value:08x}",
                "categories": ["boundary" if value in (0, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000) else "nominal"],
                "input": {"global_value": value},
                "reference_output": {"return_low_byte": reference & 0xFF},
                "candidate_output": {"return_low_byte": candidate},
                "passed": (reference & 0xFF) == candidate,
            })

    failed = [c for c in cases if not c["passed"]]
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "method": (
            "Retail was replayed in radare2 ESIL with the target global seeded to each probe "
            "value; the candidate was compiled with i686-w64-mingw32-g++ and executed under "
            "CrossOver with the same seeded global. Only the returned low byte is observable "
            "through the process exit status."
        ),
        "observable_fields": ["return_low_byte"],
        "limitations": [
            "Only the low byte of the returned dword is observable through a process exit status.",
            "The getter reads one global and has no other input, so the seed is the entire state.",
        ],
        "cases": cases,
        "tests_passed": len(cases) - len(failed),
        "tests_total": len(cases),
        "tests_failed": len(failed),
        "result": "passed" if not failed else "failed",
    }
    print(json.dumps(report, indent=2))
    return 0 if not failed else 1


if __name__ == "__main__":
    raise SystemExit(main())
