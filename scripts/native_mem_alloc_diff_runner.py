#!/usr/bin/env python3
"""Compare retail Mem_Alloc with a freshly compiled PE32 candidate."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile


def digest(path: pathlib.Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", type=pathlib.Path, required=True)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", type=pathlib.Path, required=True)
    args = parser.parse_args()
    address = "0x" + args.address[-8:].upper()
    if address != "0x004B0C60":
        raise ValueError(f"unsupported native {address}")
    root = args.root.resolve()
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    if digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256:
        raise RuntimeError("retail binary or implementation hash changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if compiler is None:
        raise RuntimeError("i686-w64-mingw32-g++ is required")
    angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
    if not angr_python.is_file():
        # The project's own angr environment, the one the gate uses.
        angr_python = pathlib.Path(__file__).resolve().parent.parent / ".venv-angr/bin/python"
    if not angr_python.is_file():
        angr_python = pathlib.Path(os.environ.get("RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent")) / ".venv/bin/python"
    if not angr_python.is_file():
        raise RuntimeError("angr Python is unavailable; configure IGI_ANGR_PYTHON")

    wrapper_text = r'''#include <cstdint>
#include "igi/retail/mem/mem_alloc.hpp"
extern "C" __declspec(dllexport) std::uint32_t NativeObservations[11];
std::uint32_t NativeObservations[11] = {};
extern "C" __declspec(dllexport) void* NativeEntry(std::uint32_t size, std::uint32_t alignment) {
    using namespace igi::retail::mem;
    Mem_ResetGlobalsForTest();
    g_936250 = 0;
    g_c3242c = 0;
    g_c32428 = nullptr;
    g_c321fc = 0;
    g_5466d4 = 0;
    void* result = Mem_Alloc(static_cast<std::int32_t>(size), static_cast<std::int32_t>(alignment));
    NativeObservations[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(result));
    NativeObservations[1] = static_cast<std::uint32_t>(g_942300);
    NativeObservations[2] = g_9422fc;
    NativeObservations[3] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(g_a960f0));
    NativeObservations[4] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(g_a960f4));
    NativeObservations[5] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(g_a960f8));
    NativeObservations[6] = static_cast<std::uint32_t>(g_a960e0);
    NativeObservations[7] = g_9422fd;
    NativeObservations[8] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_a960f0));
    NativeObservations[9] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_a960f4));
    NativeObservations[10] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_a960f8));
    return result;
}
'''
    cases = [
        {"id": f"alloc-{size}-align-{alignment}", "size": size, "alignment": alignment,
         "categories": ["boundary" if size in (0, 1, 15, 16, 17) or alignment in (1, 2, 32, 64) else "nominal"]}
        for size, alignment in [
            (0, 4), (1, 4), (15, 4), (16, 4), (17, 4), (31, 4), (32, 4), (33, 4),
            (255, 4), (256, 4), (257, 4), (4096, 4),
            (31, -1), (31, 1), (31, 2), (31, 3), (31, 6), (31, 7), (31, 8), (31, 15), (31, 16), (31, 32), (31, 64),
        ]
    ]
    helper = root / "scripts/angr_mem_alloc_execute.py"
    with tempfile.TemporaryDirectory(prefix="igi-mem-alloc-diff-") as temp:
        candidate = pathlib.Path(temp) / "mem_alloc_candidate.dll"
        wrapper = pathlib.Path(temp) / "mem_alloc_entry.cpp"
        wrapper.write_text(wrapper_text, encoding="utf-8")
        build_command = [
            compiler, "-std=c++20", "-O0", "-ffunction-sections", "-fdata-sections",
            f"-I{root / 'engine/include'}", "-shared", "-Wl,--gc-sections",
            "-o", str(candidate), str(wrapper), str(implementation),
            str(root / "engine/src/retail/mem/heap_list.cpp"),
            str(root / "engine/src/retail/error/error_show.cpp"),
        ]
        built = subprocess.run(build_command, cwd=root, text=True, capture_output=True, timeout=90)
        if built.returncode:
            raise RuntimeError(f"candidate x86 build failed: {built.stderr[-4000:]}")
        executed = subprocess.run(
            [str(angr_python), str(helper), "--binary", str(binary), "--candidate", str(candidate)],
            cwd=root, input=json.dumps(cases), text=True, capture_output=True, timeout=360,
        )
        if executed.returncode:
            raise RuntimeError(f"angr Mem_Alloc comparison failed: {executed.stderr[-4000:]}")
        actual = json.loads(executed.stdout)
        candidate_hash = digest(candidate)

    observations = actual["cases"]
    failures = sum(item["reference_output"] != item["candidate_output"] for item in observations)
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failures == 0 else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "runner": {
            "path": "scripts/native_mem_alloc_diff_runner.py",
            "sha256": digest(pathlib.Path(__file__).resolve()),
            "command": ["scripts/native_mem_alloc_diff_runner.py", *sys.argv[1:]],
        },
        "method": "Retail PE32 Mem_Alloc and a freshly compiled x86 candidate DLL executed under angr/VEX with identical synthetic allocation addresses. The comparison covers return, allocation request, raw block metadata/back-pointer relationships, and heap initialization/list state for non-negative sizes with positive power-of-two, positive non-power-of-two, and -1 alignment inputs.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations),
        "tests_passed": len(observations) - failures,
        "tests_failed": failures,
        "cases": observations,
        "retail_execution": {
            "engine": actual["engine"], "tool": actual["tool"], "version": actual["version"],
            "analysis": actual["analysis"], "function_entry": actual["function_entry"],
            "candidate_target": "PE32 x86 Windows DLL", "candidate_sha256": candidate_hash,
            "candidate_build_command": build_command, "binary_path": str(binary),
            "binary_sha256": args.binary_sha256, "helper_path": "scripts/angr_mem_alloc_execute.py",
            "helper_sha256": digest(helper),
        },
        "limitations": [
            "Only non-negative byte counts with allocation success were exercised; allocation-failure behavior was not compared.",
            "The game process and CRT heap are not running; allocation APIs are deterministic angr boundary procedures.",
        ],
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.TimeoutExpired, json.JSONDecodeError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
