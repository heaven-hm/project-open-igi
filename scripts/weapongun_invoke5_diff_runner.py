#!/usr/bin/env python3
"""Retail-vs-x86-candidate differential runner for the WeaponGun_Invoke5 natives."""
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

TARGETS = {
    "0x00473AB0": "WeaponGun_Invoke5_473AB0",
    "0x00472E10": "WeaponGun_Invoke5_472E10",
    "0x004734D0": "WeaponGun_Invoke5_4734D0",
    "0x00475460": "WeaponGun_Invoke5_475460",
}

WRAPPER = r'''
#include <stdint.h>
#include <stdio.h>
extern "C" {
__declspec(dllexport) void (*g_a96ae0[0x10000])(void*, int);
__declspec(dllexport) unsigned short g_5be3c6;
unsigned int Weapon_ComputeAccuracy_5(void) { return g_5be3c6; }
}
int FN(void* arg1, int arg2);
extern "C" __declspec(dllexport) int NativeEntry(void* arg1, int arg2) { return FN(arg1, arg2); }
'''


def digest(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


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
        raise ValueError(f"unsupported WeaponGun_Invoke5 native: {address}")
    if digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256:
        raise RuntimeError("retail binary or current implementation hash changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if not compiler:
        raise RuntimeError("i686-w64-mingw32-g++ is required for x86 candidate execution")
    helper = root / "scripts/angr_weapongun_invoke5_execute.py"
    with tempfile.TemporaryDirectory(prefix="igi-weapongun-diff-") as temp:
        tempdir = pathlib.Path(temp)
        wrapper = tempdir / "weapongun_entry.cpp"
        candidate = tempdir / "weapongun_candidate.dll"
        wrapper.write_text(WRAPPER, encoding="utf-8")
        build_command = [
            compiler, "-std=c++20", "-O0", "-fpermissive", f"-DFN={TARGETS[address]}",
            f"-I{implementation.parent}", f"-I{root / 'engine/include'}", "-shared",
            "-o", str(candidate), str(wrapper), str(implementation),
        ]
        built = subprocess.run(build_command, cwd=root, text=True, capture_output=True,
                               check=False, timeout=90)
        if built.returncode != 0:
            raise RuntimeError(f"x86 candidate DLL failed to build: {built.stderr[-4000:]}")
        angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
        if not angr_python.is_file():
            angr_python = root / ".venv-angr/bin/python"
        if not angr_python.is_file():
            angr_python = pathlib.Path(os.environ.get(
                "RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent")) / ".venv/bin/python"
        if not angr_python.is_file():
            raise RuntimeError("set IGI_ANGR_PYTHON to a Python environment with angr installed")
        command = [str(angr_python), str(helper), "--binary", str(binary),
                   "--candidate", str(candidate), "--address", address]
        executed = subprocess.run(command, cwd=root, text=True, capture_output=True,
                                  check=False, timeout=900)
        if executed.returncode != 0:
            raise RuntimeError(f"angr WeaponGun comparison failed: {executed.stderr[-3000:]}")
        raw = json.loads(executed.stdout)
        candidate_sha256 = digest(candidate)
    observations = raw["cases"]
    for case in observations:
        idx = case["input"]["index"]
        case["categories"] = ["boundary" if idx in (0, 0xFFFF) else "nominal"]
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
        "method": "Both the retail PE32 function and a freshly compiled x86 candidate DLL were executed with angr VEX. sprintf and the 0x00A96AE0 handler table were replaced with deterministic recording procedures; Weapon_ComputeAccuracy_5 reads the word at 0x005BE3C6.",
        "observable_fields": ["return_value", "esp_delta", "preserved_registers", "buffer", "events"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations),
        "tests_passed": len(observations) - failed,
        "tests_failed": failed,
        "cases": observations,
        "runner": {
            "path": "scripts/weapongun_invoke5_diff_runner.py",
            "sha256": digest(pathlib.Path(__file__).resolve()),
            "command": [sys.executable, *sys.argv],
        },
        "retail_execution": {
            "engine": raw["engine"], "tool": raw["tool"], "version": raw["version"],
            "analysis": raw["analysis"], "function_entry": raw["function_entry"],
            "helper_path": "scripts/angr_weapongun_invoke5_execute.py",
            "helper_sha256": digest(helper),
            "candidate_target": "PE32 x86 Windows DLL",
            "candidate_build_command": build_command,
            "candidate_sha256": candidate_sha256,
            "binary_path": str(binary),
            "binary_sha256": args.binary_sha256,
        },
        "limitations": [
            "Handler-table targets and sprintf are modeled boundary procedures; their real bodies are not executed.",
            "Only the 0x300-byte object buffer and the first 0x10000 handler-table slots at sampled indexes are observed.",
            "The game process is not running; angr provides instruction-level execution.",
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
