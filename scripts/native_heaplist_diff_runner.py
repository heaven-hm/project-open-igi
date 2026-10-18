#!/usr/bin/env python3
"""Live retail-vs-x86 behavioral comparison for HeapList_Insert."""

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
    root = args.root.resolve()
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    if address not in {"0x004AF8F0", "0x004AF960"}:
        raise ValueError(f"unsupported native {address}")
    if digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256:
        raise RuntimeError("retail binary or candidate source hash changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if compiler is None:
        raise RuntimeError("i686-w64-mingw32-g++ is required")
    angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
    if not angr_python.is_file():
        angr_python = pathlib.Path(os.environ.get("RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent")) / ".venv/bin/python"
    if not angr_python.is_file():
        raise RuntimeError("angr Python is unavailable; configure IGI_ANGR_PYTHON")

    wrapper_text = r'''#include <cstdint>
#include "igi/retail/mem/heap_list.hpp"
extern "C" __declspec(dllexport) void* NativeInsert(std::uint32_t header, std::uint32_t node) {
    return igi::retail::mem::HeapList_Insert(reinterpret_cast<void*>(header), reinterpret_cast<void**>(node));
}
extern "C" __declspec(dllexport) void** NativeRemove(std::uint32_t node) {
    return igi::retail::mem::HeapList_Remove(reinterpret_cast<void**>(node));
}
'''
    if address == "0x004AF8F0":
        cases = [
            {"id": f"append-to-{count}-existing", "operation": "insert", "existing_nodes": count,
             "categories": ["boundary" if count in (0, 1) else "nominal"]}
            for count in range(12)
        ]
    else:
        cases = [
            {"id": f"remove-index-{index}-from-{count}", "operation": "remove",
             "existing_nodes": count, "remove_index": index,
             "categories": ["boundary" if index in (0, count - 1) else "nominal"]}
            for count in range(1, 7)
            for index in sorted({0, count // 2, count - 1})
        ]
    helper = root / "scripts/angr_heaplist_execute.py"
    with tempfile.TemporaryDirectory(prefix="igi-heaplist-diff-") as temp:
        candidate = pathlib.Path(temp) / "heaplist_candidate.dll"
        wrapper = pathlib.Path(temp) / "heaplist_entry.cpp"
        wrapper.write_text(wrapper_text, encoding="utf-8")
        build_command = [
            compiler, "-std=c++20", "-O0", "-ffunction-sections", "-fdata-sections",
            f"-I{root / 'engine/include'}", "-shared", "-Wl,--gc-sections",
            "-o", str(candidate), str(wrapper), str(implementation),
        ]
        built = subprocess.run(build_command, cwd=root, text=True, capture_output=True, timeout=90)
        if built.returncode:
            raise RuntimeError(f"candidate x86 build failed: {built.stderr[-3000:]}")
        executed = subprocess.run(
            [str(angr_python), str(helper), "--binary", str(binary), "--candidate", str(candidate)],
            cwd=root, input=json.dumps(cases), text=True, capture_output=True, timeout=240,
        )
        if executed.returncode:
            raise RuntimeError(f"angr differential execution failed: {executed.stderr[-3000:]}")
        actual = json.loads(executed.stdout)
        candidate_sha256 = digest(candidate)

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
        "method": "Retail PE32 code and a freshly compiled candidate PE32 DLL executed under angr/VEX; the return pointer and exact list/node memory bytes were compared for valid intrusive-list layouts.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations),
        "tests_passed": len(observations) - failures,
        "tests_failed": failures,
        "cases": observations,
        "retail_execution": {
            "engine": actual["engine"],
            "tool": actual["tool"],
            "version": actual["version"],
            "analysis": actual["analysis"],
            "function_entry": actual["function_entry"],
            "candidate_target": "PE32 x86 Windows DLL",
            "candidate_sha256": candidate_sha256,
            "candidate_build_command": build_command,
            "binary_path": str(binary),
            "binary_sha256": args.binary_sha256,
            "helper_path": "scripts/angr_heaplist_execute.py",
            "helper_sha256": digest(helper),
        },
        "limitations": [
            "These tests cover valid list layouts and first/middle/last removal positions; they do not test invalid pointers or malformed list state.",
            "Execution is instruction-level VEX under angr, not a live retail game process.",
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
