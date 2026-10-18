#!/usr/bin/env python3
"""Retail-vs-x86-candidate differential runner for the allocator natives."""

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
from typing import Any


TARGETS = {"0x004AA670", "0x004AA682", "0x004AA6AE"}


def digest(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def cases_for(address: str) -> list[dict[str, Any]]:
    sizes = [0, 1, 15, 16, 17, 31, 32, 33, 255, 256, 257, 4096]
    if address == "0x004AA682":
        values = [
            {"id": f"retry-size-{size:x}", "size": size, "sentinel": 0xFFFFFFFF,
             "mode": 0, "allocation_succeeds": True}
            for size in sizes
        ]
        values.extend([
            {"id": "retry-null-sentinel-zero", "size": 17, "sentinel": 0,
             "mode": 0, "allocation_succeeds": False},
            {"id": "retry-null-sentinel-nonzero", "size": 17, "sentinel": 0xFFFFFFFF,
             "mode": 0, "allocation_succeeds": False},
            {"id": "retry-size-upper-bound", "size": 0xFFFFFFE0, "sentinel": 0xFFFFFFFF,
             "mode": 0, "allocation_succeeds": True},
            {"id": "retry-size-overflow-bound", "size": 0xFFFFFFE1, "sentinel": 0xFFFFFFFF,
             "mode": 0, "allocation_succeeds": True},
        ])
        return values
    if address == "0x004AA6AE":
        values = [
            {"id": f"raw-default-{size:x}", "size": size, "mode": 0,
             "pool_threshold": 0, "bucket_bound": 0, "allocation_succeeds": True}
            for size in sizes
        ]
        values.extend([
            {"id": f"raw-mode1-{size:x}", "size": size, "mode": 1,
             "pool_threshold": 0, "bucket_bound": 0, "allocation_succeeds": True}
            for size in (1, 16, 17, 256)
        ])
        values.extend([
            {"id": f"raw-mode2-fallback-{size:x}", "size": size, "mode": 2,
             "pool_threshold": 0, "bucket_bound": 0, "allocation_succeeds": True}
            for size in (0, 1, 16, 17)
        ])
        values.extend([
            {"id": f"raw-mode3-fallback-{size:x}", "size": size, "mode": 3,
             "pool_threshold": 0, "bucket_bound": 0, "allocation_succeeds": True}
            for size in (1, 16, 17, 256)
        ])
        values.extend([
            {"id": f"raw-allocation-fails-{size:x}", "size": size, "mode": 0,
             "pool_threshold": 0, "bucket_bound": 0, "allocation_succeeds": False}
            for size in (0, 1, 16, 17, 4096)
        ])
        return values
    return [
        {"id": f"chunk-size-{size:x}", "size": size, "mode": 0,
         "sentinel": 0, "allocation_succeeds": True}
        for size in sizes
    ] + [
        {"id": "chunk-allocation-fails", "size": 17, "mode": 0,
         "sentinel": 0xFFFFFFFF, "allocation_succeeds": False}
    ]


def candidate_wrapper() -> str:
    return r'''#include <cstdint>
#include "igi/retail/mem/mem_alloc.hpp"

extern "C" __declspec(dllexport)
void* NativeEntry(std::uint32_t address, std::uint32_t size, std::uint32_t sentinel,
                  std::uint32_t mode, std::uint32_t pool_threshold,
                  std::uint32_t bucket_bound) {
    using namespace igi::retail::mem;
    g_936250 = sentinel;
    g_936254 = nullptr;
    g_c3242c = mode;
    g_c321fc = pool_threshold;
    g_5466d4 = bucket_bound;
    g_c32428 = nullptr;
    if (address == 0x004AA670U) return Mem_AllocChunk(size);
    if (address == 0x004AA682U) return Mem_AllocWithRetry(size, sentinel);
    if (address == 0x004AA6AEU) return Mem_AllocateRawBlock(size);
    return nullptr;
}
'''


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    args = parser.parse_args()
    address = "0x" + args.address[-8:].upper()
    root = args.root.resolve()
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    if address not in TARGETS:
        raise ValueError(f"unsupported allocator native: {address}")
    if digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256:
        raise RuntimeError("retail binary or current implementation hash changed")

    compiler = shutil.which("i686-w64-mingw32-g++")
    if not compiler:
        raise RuntimeError("i686-w64-mingw32-g++ is required for x86 candidate execution")
    cases = cases_for(address)
    helper = root / "scripts/angr_abi_execute.py"
    with tempfile.TemporaryDirectory(prefix="igi-allocator-diff-") as temp:
        tempdir = pathlib.Path(temp)
        wrapper = tempdir / "allocator_entry.cpp"
        candidate = tempdir / "allocator_candidate.dll"
        wrapper.write_text(candidate_wrapper(), encoding="utf-8")
        build_command = [
            compiler, "-std=c++20", "-O0", "-ffunction-sections", "-fdata-sections",
            f"-I{root / 'engine/include'}", "-shared", "-Wl,--gc-sections",
            "-o", str(candidate), str(wrapper), str(implementation),
            str(root / "engine/src/retail/mem/heap_list.cpp"),
            str(root / "engine/src/retail/error/error_show.cpp"),
        ]
        built = subprocess.run(build_command, cwd=root, text=True, capture_output=True,
                               check=False, timeout=90)
        if built.returncode != 0:
            raise RuntimeError(f"x86 candidate DLL failed to build: {built.stderr[-4000:]}")

        angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
        if not angr_python.is_file():
            re_agent = pathlib.Path(os.environ.get(
                "RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent"))
            angr_python = re_agent / ".venv/bin/python"
        if not angr_python.is_file():
            raise RuntimeError("set IGI_ANGR_PYTHON to a Python environment with angr installed")
        command = [str(angr_python), str(helper), "--binary", str(binary),
                   "--candidate", str(candidate), "--address", address]
        executed = subprocess.run(command, cwd=root, input=json.dumps(cases), text=True,
                                  capture_output=True, check=False, timeout=240)
        if executed.returncode != 0:
            raise RuntimeError(f"angr allocator comparison failed: {executed.stderr[-3000:]}")
        raw = json.loads(executed.stdout)
        candidate_sha256 = digest(candidate)

    observations = raw["cases"]
    for case in observations:
        case["categories"] = ["boundary" if case["input"]["size"] in (0, 1, 15, 16, 17, 0xFFFFFFE0, 0xFFFFFFE1)
                              else "nominal"]
    failed = sum(c["reference_output"] != c["candidate_output"] for c in observations)
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": "Both the retail PE32 function and a freshly compiled x86 candidate DLL were executed with angr VEX. HeapAlloc and malloc were replaced with deterministic boundary procedures that record requested byte counts and return the same synthetic address (or null). Modes that enter unimplemented custom-pool behavior are excluded from this documented default-heap scope.",
        "observable_fields": ["return_value", "return_class", "requested_allocation_sizes"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations),
        "tests_passed": len(observations) - failed,
        "tests_failed": failed,
        "cases": observations,
        "runner": {
            "path": "scripts/native_abi_diff_runner.py",
            "sha256": digest(pathlib.Path(__file__).resolve()),
            "command": [sys.executable, *sys.argv],
        },
        "retail_execution": {
            "engine": raw["engine"],
            "tool": raw["tool"],
            "version": raw["version"],
            "analysis": raw["analysis"],
            "function_entry": raw["function_entry"],
            "helper_path": "scripts/angr_abi_execute.py",
            "helper_sha256": digest(helper),
            "candidate_target": "PE32 x86 Windows DLL",
            "candidate_build_command": build_command,
            "candidate_sha256": candidate_sha256,
            "binary_path": str(binary),
            "binary_sha256": args.binary_sha256,
        },
        "limitations": [
            "Return values are compared exactly under the shared synthetic allocator address; this checks pointer offsets but does not establish real heap address parity.",
            "Allocator metadata, global state changes, and complete memory side effects are not measured; this supplemental report does not satisfy the release gate's behavioral schema.",
            "Mem_AllocateRawBlock cases avoid custom-pool allocations in modes 2 and 3; mode 3 cases use sizes above the configured pool threshold and mode 2 uses a zero pool bound.",
            "Mem_AllocWithRetry cases use the retail default null new-handler; retry-after-successful-new-handler behavior is not covered.",
            "The game process and host graphics backend are not running; angr provides instruction-level execution with modeled allocation boundaries.",
        ],
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.TimeoutExpired,
            json.JSONDecodeError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
