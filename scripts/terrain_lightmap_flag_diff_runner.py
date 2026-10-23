#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for the two terrain-lightmap flags.

Covers 0x0048F250 App_ContextGetTerrainLightmapsUsed and
0x0048F260 AppContext_SetTerrainLightmapsUsed, which share the dword flag at
0x005C8BE8.

Retail bytes execute under unicorn x86-32 with the flag seeded per case. The
candidate is built from its GameNatives transcription with the mingw 32-bit
compiler and run under CrossOver, because a native x86 binary cannot execute on
this arm64 host. Both sides report the returned dword and the resulting flag, so
the setter is compared by its store and not only by its return.
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

GETTER = 0x0048F250
SETTER = 0x0048F260
FLAG_CELL = 0x005C8BE8
WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"
CROSS_CXX = "i686-w64-mingw32-g++"


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def cases() -> list[dict]:
    seeds = [
        ("flag-zero-set-zero", "nominal", 0x00000000, 0),
        ("flag-zero-set-one", "nominal", 0x00000000, 1),
        ("flag-one-set-zero", "nominal", 0x00000001, 0),
        ("flag-one-set-one", "nominal", 0x00000001, 1),
        ("flag-pattern-set-neg", "boundary", 0xA5A5A5A5, -1),
        ("flag-max-set-byte-max", "boundary", 0x7FFFFFFF, 127),
        ("flag-min-set-byte-min", "boundary", -2147483648, -128),
        ("flag-dead-set-42", "boundary", 0xDEADBEEF, 42),
    ]
    return [
        {"id": name, "categories": [category], "seed_flag": seed, "value": value,
         "seed": index}
        for index, (name, category, seed, value) in enumerate(seeds)
    ]


def retail_observation(binary: pathlib.Path, address: int, case: dict) -> dict:
    import unicorn

    from native_unicorn_runner import PebImage, new_emulator, run_with_arguments

    emulator = new_emulator(PebImage(binary))
    emulator.mem_write(FLAG_CELL, struct.pack("<I", case["seed_flag"] & 0xFFFFFFFF))
    arguments = [] if address == GETTER else [case["value"]]
    run_with_arguments(emulator, address, arguments, 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    stored = struct.unpack("<i", emulator.mem_read(FLAG_CELL, 4))[0]
    return {
        "return_value": eax & 0xFFFFFFFF,
        "state_delta": {"terrain_lightmaps_flag": stored},
        "side_effects": [],
    }


def candidate_observations(root: pathlib.Path, implementation: pathlib.Path,
                           address: int, cases_: list[dict]) -> dict[str, dict]:
    is_getter = address == GETTER
    with tempfile.TemporaryDirectory(prefix="igi-lightmap-flag-") as workspace:
        work = pathlib.Path(workspace)
        harness = work / "harness.cpp"
        executable = work / "harness.exe"
        def as_signed(value: int) -> int:
            masked = value & 0xFFFFFFFF
            return masked - 0x100000000 if masked & 0x80000000 else masked

        rows = "\n".join(
            f'    {{"{case["id"]}", {as_signed(case["seed_flag"])}, {case["value"]}}},'
            for case in cases_
        )
        declaration = (
            "int App_ContextGetTerrainLightmapsUsed(void);"
            if is_getter else
            "void AppContext_SetTerrainLightmapsUsed(std::int8_t value);"
        )
        body = (
            "        const std::int32_t result = App_ContextGetTerrainLightmapsUsed();"
            if is_getter else
            "        AppContext_SetTerrainLightmapsUsed(static_cast<std::int8_t>(item.value));\n"
            "        const std::int32_t result = g_5c8be8;"
        )
        harness.write_text(
            "#include <cstdio>\n#include <cstdint>\n"
            "// The transcription declares this flag extern; define it here.\n"
            "std::int32_t g_5c8be8 = 0;\n"
            f"{declaration}\n"
            "namespace {\n"
            "struct Case { const char* id; int seed_flag; int value; };\n"
            f"Case cases[] = {{\n{rows}\n}};\n"
            "}\n"
            "int main()\n"
            "{\n"
            "    for (const Case& item : cases) {\n"
            "        g_5c8be8 = item.seed_flag;\n"
            f"{body}\n"
            '        std::printf("{\\"id\\":\\"%s\\",\\"return_value\\":%u,'
            '\\"state_delta\\":{\\"terrain_lightmaps_flag\\":%d},\\"side_effects\\":[]}\\n",\n'
            "                    item.id, static_cast<unsigned>(result), g_5c8be8);\n"
            "    }\n"
            "    return 0;\n"
            "}\n"
        )
        build = subprocess.run(
            [CROSS_CXX, "-std=c++20", "-O1", "-w", str(harness), str(implementation),
             "-o", str(executable)],
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

    address = int(arguments.address, 16)
    if address not in (GETTER, SETTER):
        raise SystemExit(f"unsupported native address: {arguments.address}")
    if digest(arguments.binary) != arguments.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(arguments.implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    catalogue = cases()
    compiled = candidate_observations(
        arguments.root, arguments.implementation, address, catalogue)

    report_cases = []
    passed = failed = 0
    for case in catalogue:
        reference = retail_observation(arguments.binary, address, case)
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
        "address": arguments.address,
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "method": (
            "Retail bytes execute under unicorn x86-32; the candidate GameNatives transcription "
            "is compiled with i686-w64-mingw32-g++ and run under CrossOver. Each case seeds the "
            "terrain-lightmap flag at 0x005C8BE8, then compares the returned dword and the "
            "resulting flag, so the setter is checked by its store."
        ),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "limitations": [
            "The flag is compared inside each process's own address space, not across them.",
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
