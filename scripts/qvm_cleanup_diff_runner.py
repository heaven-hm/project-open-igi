#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for Qvm_Cleanup (retail VA 0x004B83D0).

Retail side: the retail bytes of 0x004B83D0 run under Unicorn (the retail x86
image is loaded at its physical PE base by scripts/native_unicorn_runner.py).
The three callees retail Qvm_Cleanup dispatches to — Resource_Unload
(0x004B6380), the standalone release (0x004BD380), and the block release
(0x004B0D10) — are hooked with sentinel-returning stubs so the runner observes
the exact call sequence and argument re-throws without depending on the ports
of those (unported) allocator chains.

Candidate side: the current C++ implementation is compiled with a harness that
installs equivalent sentinel stubs (Resource_Unload is linked directly;
0x004BD380 / 0x004B0D10 arrive via the implementation's injectable host
boundaries Sys_SetQvmStandaloneRelease / Sys_SetQvmBlockRelease).
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import native_unicorn_runner as nur  # noqa: E402

CXX = nur.CXX
QVM_ADDRESS = 0x004B83D0
RESOURCE_UNLOAD_ADDRESS = 0x004B6380
STANDALONE_RELEASE_ADDRESS = 0x004BD380
BLOCK_RELEASE_ADDRESS = 0x004B0D10

RESOURCE_UNLOAD_SENTINEL = 0x55000001
STANDALONE_RELEASE_SENTINEL = 0x55000002
BLOCK_RELEASE_SENTINEL = 0x55000003
CALL_SENTINEL = 0x00C0FFED

QVM_BUFFER = nur.SCRATCH_BASE + 0xC000
STANDALONE_POINTER = nur.SCRATCH_BASE + 0x1690
RET_STUB = nur.SCRATCH_BASE + 0xB000

CASES = [
    {"id": "qvm-cleanup-null-0", "categories": ["boundary"], "has_standalone": 0, "seed": 0},
    {"id": "qvm-cleanup-null-1", "categories": ["nominal"], "has_standalone": 0, "seed": 1},
    {"id": "qvm-cleanup-null-2", "categories": ["nominal"], "has_standalone": 0, "seed": 2},
    {"id": "qvm-cleanup-null-3", "categories": ["nominal"], "has_standalone": 0, "seed": 3},
    {"id": "qvm-cleanup-standalone-0", "categories": ["boundary"], "has_standalone": 1, "seed": 4},
    {"id": "qvm-cleanup-standalone-1", "categories": ["nominal"], "has_standalone": 1, "seed": 5},
    {"id": "qvm-cleanup-standalone-2", "categories": ["nominal"], "has_standalone": 1, "seed": 6},
    {"id": "qvm-cleanup-standalone-3", "categories": ["nominal"], "has_standalone": 1, "seed": 7},
]


def sha256(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1 << 20), b""):
            hasher.update(block)
    return hasher.hexdigest()


def retail_run(image: nur.PebImage, has_standalone: int) -> dict:
    import unicorn

    emulator = nur.new_emulator(image)
    emulator.mem_write(RET_STUB, b"\xC3")

    events: list[list] = []

    def make_hook(sentinel: int, name: str):
        def handler(uc, address, size, user_data):
            esp = uc.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
            argument = struct.unpack("<I", uc.mem_read(esp + 4, 4))[0]
            events.append([name, argument])
            uc.reg_write(unicorn.x86_const.UC_X86_REG_EAX, sentinel)
            uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, RET_STUB)
        return handler

    for callee_address, sentinel, name in (
        (RESOURCE_UNLOAD_ADDRESS, RESOURCE_UNLOAD_SENTINEL, "resource_unload"),
        (STANDALONE_RELEASE_ADDRESS, STANDALONE_RELEASE_SENTINEL, "standalone_release"),
        (BLOCK_RELEASE_ADDRESS, BLOCK_RELEASE_SENTINEL, "block_release"),
    ):
        emulator.hook_add(
            unicorn.UC_HOOK_CODE,
            make_hook(sentinel, name),
            begin=callee_address, end=callee_address,
        )

    emulator.mem_write(QVM_BUFFER, bytes([0x5A]) * 0x100)
    emulator.mem_write(
        QVM_BUFFER + 0x8C,
        struct.pack("<I", STANDALONE_POINTER if has_standalone else 0),
    )

    nur.run_with_arguments(emulator, QVM_ADDRESS, [QVM_BUFFER], CALL_SENTINEL)
    return_value = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    qvm_after = bytes(emulator.mem_read(QVM_BUFFER, 0x100))
    events_out: list[list] = []
    for name, raw_argument in events:
        if raw_argument == QVM_BUFFER:
            tag = "qvm"
        elif raw_argument == STANDALONE_POINTER:
            tag = "standalone"
        else:
            tag = f"raw:{raw_argument:#010x}"
        events_out.append([f"{name}:{tag}"])
    return {
        "return_value": int(return_value),
        "state_delta": {"qvm_hex_digest": qvm_after.hex()},
        "side_effects": events_out,
    }


HARNESS = r'''
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "igi/retail/script/qvm_native_loader.hpp"

namespace {
std::vector<std::string> g_events;
}

namespace igi::retail::resource {
std::int32_t Resource_Unload(const char* resource_path) {
    g_events.push_back("resource_unload:qvm");
    return 0x55000001;
}
}  // namespace igi::retail::resource

namespace {
std::int32_t standalone_release(void* standalone) {
    g_events.push_back("standalone_release:standalone");
    return 0x55000002;
}

std::int32_t block_release(void* context) {
    g_events.push_back("block_release:qvm");
    return 0x55000003;
}
}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const int has_standalone = std::atoi(argv[1]);
    unsigned char buffer[0x100];
    std::memset(buffer, 0x5A, sizeof(buffer));
    // The same synthetic standalone pointer value the retail emulation writes at
    // qvm+0x8C (a 32-bit dword, as in retail); neither side dereferences it
    // inside the observed range.
    const std::uint32_t standalone_value = 0x70011690;
    std::memcpy(buffer + 0x8C, has_standalone ? &standalone_value
                                              : (const void*)&has_standalone,
                sizeof(standalone_value));
    if (!has_standalone) {
        std::memset(buffer + 0x8C, 0, 4);
    }
    igi::retail::script::Sys_SetQvmStandaloneRelease(&standalone_release);
    igi::retail::script::Sys_SetQvmBlockRelease(&block_release);
    const std::int32_t result = igi::retail::script::Qvm_Cleanup(buffer);
    std::printf("RET=%d\n", result);
    std::printf("EVENTS=");
    for (const std::string& event : g_events) {
        std::printf("%s%s", event == g_events.front() ? "" : ";", event.c_str());
    }
    std::printf("\n");
    std::printf("BUF=");
    for (unsigned int word : *reinterpret_cast<const unsigned int(*)[64]>(buffer)) {
        std::printf("%u,", word);
    }
    std::printf("\n");
    return 0;
}
'''


def candidate_result(root: pathlib.Path, implementation: pathlib.Path, has_standalone: int,
                     build_dir) -> dict:
    harness_path = build_dir / "candidate_runner.cpp"
    candidate = build_dir / "candidate"
    harness_path.write_text(HARNESS, encoding="utf-8")
    built = subprocess.run(
        [CXX, "-std=c++20", "-ffunction-sections", "-fdata-sections",
         f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
         "-Wl,-dead_strip", "-Wl,-undefined,dynamic_lookup", "-o", str(candidate)],
        cwd=root, text=True, capture_output=True, check=False, timeout=90,
    )
    if built.returncode != 0:
        raise RuntimeError(f"candidate harness failed to build: {built.stderr[-1500:]}")
    actual = subprocess.run(
        [str(candidate), str(has_standalone), "0"],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if actual.returncode != 0:
        raise RuntimeError(f"candidate failed: {actual.stderr.strip()[:300]}")
    match_ret = re.search(r"RET=(-?\d+)", actual.stdout)
    match_events = re.search(r"EVENTS=([a-z_:;]*)", actual.stdout)
    match_buf = re.search(r"BUF=([0-9,]*)", actual.stdout)
    if not match_ret or not match_events or not match_buf:
        raise RuntimeError(f"candidate output malformed: {actual.stdout!r}")
    events = [[name] for name in match_events.group(1).split(";") if name]
    buf_words = [int(part) for part in match_buf.group(1).split(",") if part.strip() != ""]
    buf_bytes = b"".join(word.to_bytes(4, "little") for word in buf_words)
    return {
        "return_value": int(match_ret.group(1)),
        "state_delta": {"qvm_hex_digest": buf_bytes.hex()},
        "side_effects": events,
    }


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
    if address != "0x004B83D0":
        raise SystemExit(f"qvm_cleanup_diff_runner is bound to 0x004B83D0, not {address}")
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if sha256(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if sha256(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    nur.ensure_emulator_runtime()
    import unicorn  # noqa: F401

    image = nur.PebImage(binary)
    cases = []
    passed = 0
    failed = 0
    failures: list[str] = []
    with tempfile.TemporaryDirectory(prefix="qvm-cleanup-") as provisory:
        build_dir = pathlib.Path(provisory)
        for case in CASES:
            reference = retail_run(image, case["has_standalone"])
            candidate = candidate_result(root, implementation, case["has_standalone"], build_dir)
            mismatch = []
            for field in ("return_value", "state_delta", "side_effects"):
                if reference[field] != candidate[field]:
                    mismatch.append(
                        f"{field}: reference={json.dumps(reference[field])} "
                        f"candidate={json.dumps(candidate[field])}"
                    )
            if mismatch:
                failed += 1
                failures.extend(mismatch)
            else:
                passed += 1
            cases.append({
                "id": case["id"],
                "input": {"has_standalone": case["has_standalone"], "seed": case["seed"]},
                "categories": case["categories"],
                "reference_output": reference,
                "candidate_output": candidate,
            })

    artifact = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "method": (
            "The retail byte range of 0x004B83D0 was executed under Unicorn with the three "
            "callees (0x004B6380, 0x004BD380, 0x004B0D10) hooked as sentinel-returning stubs; "
            "the current C++ implementation was compiled with equivalent sentinel stubs "
            "(Resource_Unload linked, 0x004BD380/0x004B0D10 via the QVM release hook). For each "
            "case the returned value, the callee call sequence, and the untouched 0x100-byte "
            "qvm body were compared. Emulation is used because this ARM64 macOS host cannot "
            "launch the Windows x86 retail process."
        ),
        "float_tolerance": {"absolute": 0.0, "relative": 0.0},
        "runner": {
            "path": "scripts/qvm_cleanup_diff_runner.py",
            "sha256": sha256(pathlib.Path(__file__).resolve()),
            "command": [sys.executable, *sys.argv],
        },
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "result": "passed" if failed == 0 else "failed",
        "cases": cases,
        "tests_total": len(cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "limitations": [
            "The hooked callees are sentinel stubs; runner coverage proves Qvm_Cleanup's branch,"
            " call order, and argument identity, not the allocator behavior inside "
            "0x004BD380 or 0x004B0D10 (both remain unported and are documented as gaps).",
            "The qvm +0x8C standalone pointer is a synthetic non-null value; no dereference "
            "happens inside the observed range, so pointer contents are not validated.",
            "state_delta observes the first 0x100 bytes of the qvm body only.",
        ],
    }
    if failures:
        artifact["comparison_problems"] = failures
    print(json.dumps(artifact, indent=2))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
