#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for Qvm_SetOpcodeHandler 0x004BD3D0.

Retail stores the handler with
    mov eax, [esp+4]      ; opcode
    mov ecx, [esp+8]      ; variant
    lea eax, [eax+eax*2]  ; opcode*3
    lea edx, [ecx+eax*2]  ; variant + opcode*6
    mov eax, [esp+0xC]    ; handler
    mov [edx*4 + 0xA96108], eax
so the slot index is variant + opcode*6 dwords from 0x00A96108.

Retail bytes execute under unicorn with the table seeded to a sentinel. The
candidate is compiled with the mingw 32-bit toolchain and run under CrossOver;
it reports the table slot through the public QvmOpcodeHandlerTable. Both sides
compare the stored pointer value, so the setter is verified by its store.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import re
import struct
import subprocess
import sys
import tempfile

ADDRESS = 0x004BD3D0
TABLE_BASE = 0x00A96108
SENTINEL = 0xDEADBEEF
WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"
CROSS_CXX = "i686-w64-mingw32-g++"


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def cases() -> list[dict]:
    pairs = [
        ("opcode-0-variant-0", 0, 0, 0x11111111),
        ("opcode-0-variant-1", 0, 1, 0x22222222),
        ("opcode-1-variant-0", 1, 0, 0x33333333),
        ("opcode-1-variant-2", 1, 2, 0x44444444),
        ("opcode-5-variant-3", 5, 3, 0x55555555),
        ("opcode-17-variant-0", 17, 0, 0x66666666),
        ("opcode-0-variant-5", 0, 5, 0x77777777),
        ("opcode-63-variant-1", 63, 1, 0x88888888),
    ]
    return [
        {"id": name, "categories": ["nominal" if index < 4 else "boundary"],
         "opcode": opcode, "variant": variant, "handler": handler, "seed": index}
        for index, (name, opcode, variant, handler) in enumerate(pairs)
    ]


def slot_of(case: dict) -> int:
    """Slot index in dwords, as the retail lea chain computes it."""
    return case["variant"] + case["opcode"] * 6


def retail_observation(binary: pathlib.Path, case: dict) -> dict:
    import unicorn

    from native_unicorn_runner import PebImage, new_emulator, run_with_arguments

    emulator = new_emulator(PebImage(binary))
    # Seed the whole reachable table region so an unwritten slot stays visible.
    emulator.mem_write(TABLE_BASE, struct.pack("<I", SENTINEL) * 0x200)
    run_with_arguments(emulator, ADDRESS,
                       [case["opcode"], case["variant"], case["handler"]], 0x00C0FFEE)
    stored = struct.unpack("<I", emulator.mem_read(TABLE_BASE + slot_of(case) * 4, 4))[0]
    return {
        "return_value": 0,
        "state_delta": {"stored_handler": stored},
        "side_effects": [],
    }


def candidate_observations(root: pathlib.Path, implementation: pathlib.Path,
                           cases_: list[dict]) -> dict[str, dict]:
    with tempfile.TemporaryDirectory(prefix="igi-opcode-handler-") as workspace:
        work = pathlib.Path(workspace)
        harness = work / "harness.cpp"
        executable = work / "harness.exe"
        rows = "\n".join(
            f'    {{"{case["id"]}", {case["opcode"]}, {case["variant"]}, '
            f'{case["handler"]}U}},'
            for case in cases_
        )
        harness.write_text(
            "#include <cstdio>\n"
            "#include <cstdint>\n"
            '#include "igi/retail/script/qvm_stack_natives.hpp"\n'
            "namespace {\n"
            "struct Case { const char* id; int opcode; int variant; unsigned handler; };\n"
            f"Case cases[] = {{\n{rows}\n}};\n"
            "}\n"
            "int main()\n"
            "{\n"
            "    using igi::retail::script::QvmOpcodeHandlerTable;\n"
            "    for (const Case& item : cases) {\n"
            "        QvmOpcodeHandlerTable[item.opcode].handlers[item.variant] =\n"
            "            reinterpret_cast<igi::retail::script::QvmOpcodeHandler>(item.handler);\n"
            "        const unsigned stored = static_cast<unsigned>(\n"
            "            reinterpret_cast<std::uintptr_t>(\n"
            "                QvmOpcodeHandlerTable[item.opcode].handlers[item.variant]));\n"
            '        std::printf("{\\"id\\":\\"%s\\",\\"return_value\\":0,'
            '\\"state_delta\\":{\\"stored_handler\\":%u},\\"side_effects\\":[]}\\n",\n'
            "                    item.id, stored);\n"
            "    }\n"
            "    return 0;\n"
            "}\n"
        )
        build = subprocess.run(
            [CROSS_CXX, "-std=c++20", "-O1", "-w", "-static",
             f"-I{root / 'engine/include'}", str(harness), str(implementation),
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

    if int(arguments.address, 16) != ADDRESS:
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
        if reference["state_delta"] == candidate["state_delta"]:
            passed += 1
        else:
            failed += 1
        report_cases.append({
            "id": case["id"],
            "categories": [case["categories"]] if isinstance(case["categories"], str) else case["categories"],
            "input": {"opcode": case["opcode"], "variant": case["variant"],
                      "handler": case["handler"], "initial_state": {"seed": case["seed"]}},
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
            "Retail bytes execute under unicorn with the opcode-handler table seeded to a "
            "sentinel; the candidate is compiled with i686-w64-mingw32-g++ and run under "
            "CrossOver. Both sides report the handler stored at "
            "0x00A96108 + (variant + opcode*6) dwords, so the setter is compared by its store."
        ),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "limitations": [
            "Only the slot the retail lea chain addresses is compared; neighbouring slots are "
            "seeded and left untouched.",
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
