#!/usr/bin/env python3
"""Differential replay of FP_SetDefaultPrecision (0x004A7157), the CRT's x87 default-precision setter.

The native takes no stack arguments: its only input is the x87 control word in effect when it is called. That is a real
input, so this runner varies it instead of pretending the function is untestable. Retail executes its own bytes under
unicorn with the control word written before entry; the candidate executes the compiled implementation in a small
harness that loads the same control word, calls the function and reads the resulting word back. Both sides report the
returned flag word and the control word after the call, and the runner compares them exactly.

Unicorn read-back and Wine read-back disagree on one bit: bit 6 (0x40) is reserved in the x87 control word, and the
Wine x87 implementation under this bottle reports it as set after FLDCW while unicorn (and the retail hardware model)
keeps it clear. The runner masks that reserved bit out on both sides and says so in the report; nothing else is
normalised, and a one-bit difference anywhere in the defined bits fails the case.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import subprocess
import sys
import tempfile

ADDRESS = "0x004A7157"
SYMBOL = "FP_SetDefaultPrecision"
RESERVED_CONTROL_BIT = 0x40
CASES = [
    # control words: retail default, 24-bit precision, 53-bit precision, extended precision,
    # round-to-nearest / down / up / toward-zero, everything unmasked, a bare word, all bits set
    0x037F, 0x007F, 0x027F, 0x0C7F, 0x0E3F, 0x063F, 0x0A3F, 0xFFFF,
]
STOP_ADDRESS = 0x13370000
SENTINEL = 0x00C0FFEE
CROSS_CXX = "i686-w64-mingw32-g++"
WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"

HARNESS = r"""
#include <cstdio>
#include <cstdint>
#include "igi/retail/core/fpu_state.hpp"

int main()
{
    const unsigned cases[] = CASES;
    for (unsigned value : cases)
    {
        unsigned short word = static_cast<unsigned short>(value);
        __asm__ volatile("fwait\n\tfldcw %0" : : "m"(word) : "memory");
        const std::uint32_t returned = igi::retail::core::FP_SetDefaultPrecision();
        unsigned short after = 0;
        __asm__ volatile("fwait\n\tfnstcw %0" : "=m"(after) : : "memory");
        std::printf("{\"x87_control_word\":%u,\"return_value\":%u,\"control_word_after\":%u}\n",
                    value, returned, static_cast<unsigned>(after));
    }
    return 0;
}
"""


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def retail_observations(binary: pathlib.Path) -> dict[int, dict]:
    import unicorn

    from native_unicorn_runner import PebImage, new_emulator, run_with_arguments

    emulator = new_emulator(PebImage(binary))
    observed: dict[int, dict] = {}
    for word in CASES:
        emulator.reg_write(unicorn.x86_const.UC_X86_REG_FPCW, word)
        run_with_arguments(emulator, int(ADDRESS, 16), [], STOP_ADDRESS)
        returned = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
        after = emulator.reg_read(unicorn.x86_const.UC_X86_REG_FPCW)
        observed[word] = {
            "return_value": returned,
            "state_delta": {"x87_control_word_after": after & ~RESERVED_CONTROL_BIT},
            "side_effects": [],
        }
    return observed


def candidate_observations(root: pathlib.Path, implementation: pathlib.Path) -> dict[int, dict]:
    harness_source = HARNESS.replace("CASES", "{" + ", ".join(f"0x{word:04X}U" for word in CASES) + "}")
    with tempfile.TemporaryDirectory(prefix="igi-fpu-cw-") as workspace:
        work = pathlib.Path(workspace)
        harness = work / "harness.cpp"
        executable = work / "harness.exe"
        harness.write_text(harness_source)
        build = subprocess.run(
            [CROSS_CXX, "-std=c++20", "-O1", "-static", "-w", f"-I{root / 'engine/include'}",
             str(harness), str(implementation), "-o", str(executable)],
            cwd=root, capture_output=True, text=True, timeout=300, check=False)
        if build.returncode != 0:
            raise SystemExit(f"candidate build failed: {build.stderr[-1500:]}")
        run = subprocess.run(
            [WINE, "--bottle", BOTTLE, "--cx-app", "Z:" + str(executable).replace("/", "\\")],
            text=True, capture_output=True, timeout=300, check=False)
    observed: dict[int, dict] = {}
    for line in run.stdout.splitlines():
        line = line.strip()
        if not line.startswith("{"):
            continue
        record = json.loads(line)
        word = int(record["x87_control_word"])
        observed[word] = {
            "return_value": int(record["return_value"]),
            "state_delta": {"x87_control_word_after": int(record["control_word_after"]) & ~RESERVED_CONTROL_BIT},
            "side_effects": [],
        }
    missing = [word for word in CASES if word not in observed]
    if missing:
        raise SystemExit(f"candidate harness did not report control words {missing}; stderr={run.stderr[-500:]}")
    return observed


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    args = parser.parse_args()

    if "0x" + args.address[-8:].upper() != ADDRESS:
        raise SystemExit(f"unsupported native address: {args.address}")
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    try:
        import unicorn  # noqa: F401
    except ImportError:
        interpreter = os.environ.get("IGI_UNICORN_PYTHON", str(root / "build/unicorn-rosetta/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise SystemExit("unicorn is required; set IGI_UNICORN_PYTHON")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])

    reference = retail_observations(binary)
    candidate = candidate_observations(root, implementation)
    reports = []
    passed = failed = 0
    for word in CASES:
        matches = reference[word] == candidate[word]
        if not matches:
            print(f"mismatch control word 0x{word:04X}: {reference[word]} vs {candidate[word]}", file=sys.stderr)
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        reports.append({
            "id": f"fp-default-precision-{word:04x}",
            "input": {"args": [], "initial_state": {"x87_control_word": f"0x{word:04x}"}},
            "categories": ["nominal"] if word in (0x037F, 0x027F) else ["boundary"],
            "reference_output": reference[word],
            "candidate_output": candidate[word],
        })

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 and passed == len(reports) else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": ADDRESS,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": (f"The native's only input is the x87 control word, so the runner varies it: retail executes its own bytes "
                   "under unicorn with the word written before entry, and the compiled candidate executes FP_SetDefaultPrecision "
                   "after loading the same word. Both sides report the returned flag word and the control word after the call; "
                   f"bit 6 (0x{RESERVED_CONTROL_BIT:02X}), reserved in the control word, is masked out because the Wine x87 "
                   "implementation reports it as set while unicorn does not. Nothing else is normalised."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(reports),
        "tests_passed": passed,
        "tests_failed": failed,
        "retail_execution": {"tool": "unicorn", "engine": "unicorn x86-32",
                             "version": getattr(__import__("unicorn"), "__version__", "unknown"),
                             "function_entry": ADDRESS, "binary_sha256": args.binary_sha256},
        "cases": reports,
    }
    json.dump(report, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if report["result"] == "passed" else 1


if __name__ == "__main__":
    raise SystemExit(main())
