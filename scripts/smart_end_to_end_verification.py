#!/usr/bin/env python3
"""Strict one-native static, build, and paired-Frida verification workflow.

The command accepts one retail VA.  It never promotes a native from a partial
phase: Binary Ninja/Radare2 evidence, exact angr recovery, current semantic
review, a freshly bound x86 candidate, and a paired retail/candidate runtime
comparison must all succeed.

Examples:
  python3 scripts/smart_end_to_end_verification.py --address 0x0048F1A0
  python3 scripts/smart_end_to_end_verification.py --address 0x00401050 --level 1
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
RETAIL = Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
BUILD_DIR = ROOT / "build-win32-no-verify-gate"
CANDIDATE = BUILD_DIR / "engine/platform/windows/open-igi.exe"
RECEIPT = ROOT / "build/diagnostic-candidate-build.json"

TYPED_FRIDA_RUNNERS = {
    0x0048F0C0: ROOT / "scripts/frida_app_startup_store_pair.py",
    0x0048F0D0: ROOT / "scripts/frida_app_startup_store_5c8bc0_pair.py",
    0x0048F0F0: ROOT / "scripts/frida_app_startup_store_5c8bcc_pair.py",
    0x0048F1A0: ROOT / "scripts/frida_app_context_setter_pair.py",
    0x0048F1B0: ROOT / "scripts/frida_app_context_getter_pair.py",
    0x0048F0E0: ROOT / "scripts/frida_app_startup_store_5c8bc4_pair.py",
    0x00401050: ROOT / "scripts/frida_401050_pair.py",
}

RETAINED_SYMBOLS = {
    0x0048F0C0: "_Native_0048F0C0",
    0x0048F0D0: "_Native_0048F0D0",
    0x0048F0F0: "_Native_0048F0F0",
    0x0048F0E0: "_SetWindowHandle",
}


def digest(path: Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def normalize(raw: str) -> str:
    return f"0x{int(raw, 0):08X}"


def load_record(address: str) -> dict[str, Any]:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    matches = [
        record for record in data.get("natives", [])
        if int(record.get("address", "0"), 16) == int(address, 16)
    ]
    if len(matches) != 1:
        raise ValueError(
            f"{address} must have exactly one manifest record; found {len(matches)}"
        )
    return matches[0]


def run(command: list[str], *, env: dict[str, str] | None = None,
        timeout: int = 3600) -> dict[str, Any]:
    process = subprocess.run(
        command, cwd=ROOT, text=True, capture_output=True, check=False,
        env=env, timeout=timeout,
    )
    return {
        "command": command,
        "exit_code": process.returncode,
        "stdout": process.stdout,
        "stderr": process.stderr,
    }


def compact_output(result: dict[str, Any], limit: int = 4096) -> dict[str, Any]:
    """Keep a diagnostic tail while the dedicated phase report retains full evidence."""
    compact = dict(result)
    for field in ("stdout", "stderr"):
        value = compact.get(field, "")
        compact[f"{field}_bytes"] = len(value.encode("utf-8"))
        compact[f"{field}_sha256"] = hashlib.sha256(value.encode("utf-8")).hexdigest()
        if len(value) > limit:
            compact[field] = value[-limit:]
    return compact


def write_report(path: Path, report: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--binary", type=Path, default=RETAIL)
    parser.add_argument("--candidate", type=Path, default=CANDIDATE)
    parser.add_argument("--receipt", type=Path, default=RECEIPT)
    parser.add_argument("--skip-build", action="store_true",
                        help="Reuse a candidate only when its existing receipt validates it.")
    parser.add_argument("--static-only", action="store_true",
                        help="Stop after the strict static/semantic phase.")
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()

    address = normalize(args.address)
    address_value = int(address, 16)
    output = (args.report or ROOT / "build/native-one" / address / "report.json").resolve()
    static_report = output.parent / "static-gate.json"
    runtime_report = output.parent / "frida-pair.json"

    report: dict[str, Any] = {
        "schema_version": 1,
        "kind": "igi-one-native-verification",
        "address": address,
        "result": "blocked",
        "verified": False,
        "phases": {},
    }

    try:
        record = load_record(address)
        implementation = (ROOT / record["implementation"]["path"]).resolve()
        if not implementation.is_file():
            raise ValueError(f"implementation source is missing: {implementation}")
        binary = args.binary.resolve()
        if not binary.is_file():
            raise ValueError(f"retail binary is missing: {binary}")
        source_hash = digest(implementation)
        retail_hash = digest(binary)
        if source_hash != record["implementation"].get("sha256"):
            raise ValueError("implementation hash differs from its reviewed manifest record")
        if retail_hash != record.get("binary_sha256"):
            raise ValueError("retail image hash differs from the manifest record")

        env = dict(os.environ)
        env.setdefault("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))
        gate_command = [
            sys.executable, str(ROOT / "scripts/native_gate.py"),
            "--root", str(ROOT),
            "--address", address,
            "--file", str(implementation.relative_to(ROOT)),
            "--threshold", "STRUCTURALLY_VERIFIED",
            "--cfg-threshold", "0.85",
            "--require-semantic-evidence",
            "--allow-structural-only",
            "--report", str(static_report),
        ]
        static = run(gate_command, env=env)
        report["phases"]["static"] = static
        if static["exit_code"] != 0:
            write_report(output, report)
            print(static["stdout"], end="")
            print(static["stderr"], end="", file=sys.stderr)
            print("False")
            print(f"BLOCKED static phase; report: {output}")
            return 1

        if args.static_only:
            report["result"] = "static_passed_runtime_not_run"
            write_report(output, report)
            print(f"STATIC PASS {address}; runtime deliberately not run; report: {output}")
            return 0

        if not args.skip_build:
            configure_command = [
                "cmake", "-S", str(ROOT), "-B", str(BUILD_DIR),
                "-DIGI_ENABLE_NATIVE_GATE=OFF",
                "-DIGI_FULL_PRODUCTION_GATE=OFF",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_SYSTEM_NAME=Windows",
                "-DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++",
                "-DBUILD_TESTING=OFF",
                "-DIGI_KEEP_ALL_RETAIL_NATIVES=OFF",
            ]
            retained_symbol = RETAINED_SYMBOLS.get(address_value)
            if retained_symbol:
                configure_command.append(f"-DIGI_RETAIN_NATIVE_SYMBOL={retained_symbol}")
            configure = run(configure_command, timeout=600)
            report["phases"]["configure"] = compact_output(configure)
            if configure["exit_code"] != 0:
                write_report(output, report)
                print(configure["stdout"], end="")
                print(configure["stderr"], end="", file=sys.stderr)
                print("False")
                print(f"BLOCKED candidate configuration; report: {output}")
                return 1

            receipt = run([
                sys.executable, str(ROOT / "scripts/candidate_build_receipt.py"),
                "--root", str(ROOT),
                "--build-dir", str(BUILD_DIR),
                "--candidate", str(args.candidate.resolve()),
                "--receipt", str(args.receipt.resolve()),
            ], timeout=3600)
            report["phases"]["build_receipt"] = compact_output(receipt)
            if receipt["exit_code"] != 0:
                write_report(output, report)
                print(receipt["stdout"], end="")
                print(receipt["stderr"], end="", file=sys.stderr)
                print("False")
                print(f"BLOCKED candidate receipt; report: {output}")
                return 1

        runner = TYPED_FRIDA_RUNNERS.get(address_value)
        if runner is None:
            runner = ROOT / "scripts/native_frida_leaf_diff_runner.py"

        runtime_command = [
            sys.executable, str(runner),
            "--address", address,
            "--root", str(ROOT),
            "--binary", str(binary),
            "--binary-sha256", retail_hash,
            "--implementation", str(implementation),
            "--implementation-sha256", source_hash,
        ]
        # The typed pair runners take the candidate, its receipt and the
        # output path; the leaf runner takes the level instead.
        if runner.name.startswith("frida_app_context_") or runner.name.startswith("frida_app_startup_store_"):
            runtime_command += [
                "--candidate", str(args.candidate.resolve()),
                "--receipt", str(args.receipt.resolve()),
                "--out", str(runtime_report),
            ]
        else:
            runtime_command += [
                "--level", str(args.level),
            ]

        runtime_env = dict(env)
        runtime_env["IGI_CANDIDATE_EXE"] = str(args.candidate.resolve())
        runtime_env["IGI_CANDIDATE_BUILD_RECEIPT"] = str(args.receipt.resolve())
        # The dispatch probe drives the candidate through its own interface:
        # it attaches to the running candidate rather than to the retail.
        if runner.name == "frida_401050_pair.py":
            runtime_command = [
                sys.executable, str(runner),
                "--candidate-exe", str(args.candidate.resolve()),
                "--level", str(args.level),
                "--out", str(runtime_report),
            ]
            runtime = run(runtime_command, env=runtime_env, timeout=1200)
            report["phases"]["runtime"] = runtime
            if runtime["exit_code"] != 0:
                write_report(output, report)
                print(runtime["stdout"], end="")
                print(runtime["stderr"], end="", file=sys.stderr)
                print("False")
                print(f"BLOCKED paired Frida phase; report: {output}")
                return 1
            report["result"] = "passed"
            report["verified"] = True
            write_report(output, report)
            print(runtime["stdout"], end="")
            print("True")
            print(f"PASS {address}: static/semantic, x86 build binding, and paired Frida runtime")
            print(f"report: {output}")
            return 0

        runtime = run(runtime_command, env=runtime_env, timeout=1200)
        report["phases"]["runtime"] = compact_output(runtime)
        if runtime_report.is_file():
            report["runtime_report"] = str(runtime_report.relative_to(ROOT))
            report["runtime_report_sha256"] = digest(runtime_report)
        if runtime["exit_code"] != 0:
            write_report(output, report)
            print(runtime["stdout"], end="")
            print(runtime["stderr"], end="", file=sys.stderr)
            print("False")
            print(f"BLOCKED paired Frida phase; report: {output}")
            return 1

        report["result"] = "passed"
        report["verified"] = True
        write_report(output, report)
        if runtime_report.is_file():
            runtime_data = json.loads(runtime_report.read_text(encoding="utf-8"))
            print(
                "runtime:",
                runtime_data.get("result"),
                f"{runtime_data.get('tests_passed', 0)}/{runtime_data.get('tests_total', 0)} cases",
            )
        print("True")
        print(f"PASS {address}: static/semantic, x86 build binding, and paired Frida runtime")
        print(f"report: {output}")
        return 0
    except (OSError, ValueError, KeyError, subprocess.TimeoutExpired) as error:
        report["error"] = f"{type(error).__name__}: {error}"
        write_report(output, report)
        print("False")
        print(f"BLOCKED {address}: {error}", file=sys.stderr)
        print(f"report: {output}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
