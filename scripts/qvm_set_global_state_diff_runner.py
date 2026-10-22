#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for Qvm_SetGlobalState 0x004B8A10.

Retail getter bytes execute under unicorn x86-32. The candidate setter is built
with the mingw 32-bit compiler and executed under CrossOver, because a native
x86 binary cannot run on this arm64 host.

Both sides report the returned dword and the stored global, the latter read back
through the implementation's own Qvm_GetLastPopStack23Value accessor, so the
comparison covers the store and not only the return value.
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
import tempfile

ADDRESS = 0x004B8A10
GLOBAL_CELL = 0x00A965A0
WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"
CROSS_CXX = "i686-w64-mingw32-g++"


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def cases() -> list[dict]:
    values = [
        ("state-nominal", "nominal", 7),
        ("state-zero", "nominal", 0),
        ("state-one", "nominal", 1),
        ("state-minus-one", "boundary", 0xFFFFFFFF),
        ("state-max", "boundary", 0x7FFFFFFF),
        ("state-min", "boundary", 0x80000000),
        ("state-pattern", "boundary", 0xA5A5A5A5),
        ("state-other-pattern", "boundary", 0xDEADBEEF),
    ]
    return [
        {"id": name, "categories": [category], "value": value, "seed": index}
        for index, (name, category, value) in enumerate(values)
    ]


def retail_observation(binary: pathlib.Path, case: dict) -> dict:
    import unicorn

    from native_unicorn_runner import PebImage, new_emulator, run_with_arguments

    emulator = new_emulator(PebImage(binary))
    # Seed a distinct sentinel so a missing store cannot look like success.
    emulator.mem_write(GLOBAL_CELL, struct.pack("<I", 0xCCCCCCCC))
    run_with_arguments(emulator, ADDRESS, [case["value"]], 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    stored = struct.unpack("<I", emulator.mem_read(GLOBAL_CELL, 4))[0]
    return {
        "return_value": eax & 0xFFFFFFFF,
        "state_delta": {"global_state": stored},
        "side_effects": [],
    }


def candidate_observations(root: pathlib.Path, implementation: pathlib.Path,
                           cases_: list[dict]) -> dict[str, dict]:
    with tempfile.TemporaryDirectory(prefix="igi-qvm-setter-") as workspace:
        work = pathlib.Path(workspace)
        harness = work / "harness.cpp"
        executable = work / "harness.exe"
        rows = "\n".join(
            f'    {{"{case["id"]}", {case["value"]}U}},' for case in cases_
        )
        harness.write_text(
            '#include <cstdio>\n#include <cstdint>\n'
            '#include "igi/retail/script/qvm_native_loader.hpp"\n'
            "namespace {\n"
            "struct Case { const char* id; unsigned value; };\n"
            f"Case cases[] = {{\n{rows}\n}};\n"
            "}\n"
            "int main()\n"
            "{\n"
            "    for (const Case& item : cases) {\n"
            "        const std::int32_t result = igi::retail::script::Qvm_SetGlobalState(\n"
            "            static_cast<std::int32_t>(item.value));\n"
            "        const std::int32_t stored = igi::retail::script::Qvm_GetLastPopStack23Value();\n"
            '        std::printf("{\\"id\\":\\"%s\\",\\"return_value\\":%u,'
            '\\"state_delta\\":{\\"global_state\\":%u},\\"side_effects\\":[]}\\n",\n'
            "                    item.id, static_cast<unsigned>(result),\n"
            "                    static_cast<unsigned>(stored));\n"
            "    }\n"
            "    return 0;\n"
            "}\n"
        )
        build = subprocess.run(
            [CROSS_CXX, "-std=c++20", "-O1", "-w", "-ffunction-sections",
             "-fdata-sections", f"-I{root / 'engine/include'}",
             str(harness), str(implementation), "-o", str(executable),
             "-Wl,--gc-sections", "-Wl,--unresolved-symbols=ignore-all"],
            cwd=root, text=True, capture_output=True, timeout=300, check=False,
        )
        if build.returncode != 0:
            raise SystemExit(f"candidate build failed: {build.stderr[-1500:]}")
        run = subprocess.run(
            [WINE, "--bottle", BOTTLE, "--cx-app",
             "Z:" + str(executable).replace("/", "\\")],
            text=True, capture_output=True, timeout=300, check=False,
        )
    observations = {}
    for line in run.stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            record = json.loads(line)
            observations[record["id"]] = record
    return observations


def main() -> int:
    try:
        import unicorn  # noqa: F401
    except ImportError:
        interpreter = pathlib.Path(os.environ.get(
            "IGI_UNICORN_PYTHON", "build/unicorn-rosetta/bin/python"))
        if interpreter.is_file():
            os.execv(str(interpreter), [str(interpreter), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])
        raise SystemExit(
            "unicorn is required; set IGI_UNICORN_PYTHON to an x86_64 Python with unicorn")

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    arguments = parser.parse_args()

    if f"0x{int(arguments.address, 16):08X}" != "0x004B8A10":
        raise SystemExit(f"unsupported native address: {arguments.address}")
    if digest(arguments.binary) != arguments.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(arguments.implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    catalogue = cases()
    compiled = candidate_observations(arguments.root, arguments.implementation, catalogue)

    report_cases = []
    passed = failed = 0
    for case in catalogue:
        reference = retail_observation(arguments.binary, case)
        candidate = compiled.get(case["id"])
        if candidate is None:
            raise SystemExit(f"candidate produced no observation for {case['id']}")
        stored_matches = reference["state_delta"] == candidate["state_delta"]
        returned_matches = reference["return_value"] == candidate["return_value"]
        if stored_matches and returned_matches:
            passed += 1
        else:
            failed += 1
        report_cases.append({
            "id": case["id"],
            "categories": case["categories"],
            "input": {"args": [case["value"]], "initial_state": {"seed": case["seed"]}},
            "reference_output": reference,
            "candidate_output": {
                "return_value": candidate["return_value"],
                "state_delta": candidate["state_delta"],
                "side_effects": [],
            },
        })

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "address": "0x004B8A10",
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "method": (
            "Retail bytes execute under unicorn x86-32; the candidate is compiled with "
            "i686-w64-mingw32-g++ and run under CrossOver. Each case compares the returned "
            "dword and the stored global at 0x00A965A0, read back through "
            "Qvm_GetLastPopStack23Value, so the store itself is compared."
        ),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "limitations": [
            "The global is compared inside each process's own address space, not across them.",
        ],
        "cases": report_cases,
        "tests_passed": passed,
        "tests_total": len(report_cases),
        "tests_failed": failed,
        "result": "passed" if failed == 0 else "failed",
    }
    print(json.dumps(report, indent=1))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
