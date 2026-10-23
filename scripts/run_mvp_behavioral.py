#!/usr/bin/env python3
"""Run each MVP native's behavioral runner and bind its report to the manifest.

The gate runs the runner itself, but the manifest still needs the report it
produced, the runner's digest and the VERIFIED status, so this script runs
each native's configured runner, writes the report under
docs/fidelity/evidence/ and updates the manifest record.

A native is only raised to VERIFIED when its report records
executed=true, oracle=retail-binary-execution and no failing case, and the
report's runner digest matches the runner file on disk.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
EVIDENCE = ROOT / "docs/fidelity/evidence"
RETAIL = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
RUNNER_MAP = {key.upper(): value for key, value in
               json.loads((ROOT / "scripts/mvp_runner_map.json").read_text()).items()}


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def mvp_addresses() -> list[str]:
    """The MVP native list, in the order the verification runner uses."""
    source = (ROOT / "scripts/verify_demo_natives.py").read_text()
    start = source.index("TARGET_METHODS: list[dict[str, str]] = [")
    end = source.index("\n]\n", start)
    block = source[start:end]
    addresses = []
    for line in block.split("\n"):
        if '"va":' in line:
            addresses.append(line.split('"va":')[1].split('"')[1].strip())
    return addresses


def run_one(record: dict, address: str) -> tuple[bool, str]:
    verification = record.get("verification", {})
    runner_config = verification.get("behavioral_runner")
    if not isinstance(runner_config, dict):
        return False, "no behavioral runner configured"
    runner_path = runner_config.get("path")
    if not isinstance(runner_path, str) or not runner_path.strip():
        return False, "no behavioral runner path"
    # The runner that actually covers this address wins over the generic one.
    dedicated = RUNNER_MAP.get(address.upper())
    if dedicated and dedicated != runner_path:
        runner_path = dedicated
        runner_config = dict(runner_config, path=runner_path)
    runner_file = ROOT / runner_path
    if not runner_file.is_file():
        return False, f"runner file missing: {runner_path}"
    implementation = (record.get("implementation") or {}).get("path", "")
    implementation_file = ROOT / implementation
    if not implementation_file.is_file():
        return False, f"implementation file missing: {implementation}"
    raw_args = runner_config.get("args") or []
    substitutions = {
        "address": address,
        "binary": str(RETAIL),
        "binary_sha256": RETAIL_SHA256,
        "implementation": str(implementation_file),
        "implementation_sha256": sha256(implementation_file),
        "root": str(ROOT),
    }
    args = [re.sub(r"\{([a-z0-9_]+)\}",
                   lambda match: substitutions.get(match.group(1), match.group(0)),
                   arg)
            for arg in raw_args]
    timeout = runner_config.get("timeout_seconds", 180)
    # Some runners execute the retail in angr, which needs its own interpreter.
    environment = dict(os.environ)
    angr = environment.get("IGI_ANGR_PYTHON")
    if not angr or not pathlib.Path(angr).is_file():
        for candidate in (ROOT / ".venv-angr/bin/python", ROOT.parents[1] / ".venv-angr/bin/python"):
            if candidate.is_file():
                angr = str(candidate)
                break
    if angr:
        environment["IGI_ANGR_PYTHON"] = angr
    try:
        completed = subprocess.run(
            [sys.executable, str(runner_file), *args],
            cwd=ROOT, capture_output=True, text=True, timeout=int(timeout), env=environment,
        )
    except subprocess.TimeoutExpired:
        return False, f"runner timed out after {timeout}s"
    output = completed.stdout.strip()
    if not output.startswith("{"):
        return False, f"runner produced no report: {completed.stderr.strip()[-200:]}"
    try:
        report = json.loads(output)
    except json.JSONDecodeError:
        return False, "runner report is not JSON"
    if report.get("executed") is not True:
        return False, "report does not record executed=true"
    if report.get("oracle") != "retail-binary-execution":
        return False, "report does not identify the retail oracle"
    if report.get("tests_failed"):
        return False, f"{report.get('tests_failed')} case(s) failed"
    if report.get("result") != "passed":
        return False, f"report result is {report.get('result')!r}"
    report["runner"] = {
        "path": runner_path,
        "sha256": sha256(runner_file),
        "command": [sys.executable, str(runner_file), *args],
    }
    report_path = EVIDENCE / f"0x{address[-8:].upper()}-behavioral.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n")
    verification["behavioral"] = {
        "status": "PASS",
        "evidence_path": f"docs/fidelity/evidence/{report_path.name}",
        "evidence_sha256": sha256(report_path),
        "oracle": "retail-binary-execution",
        "tests_passed": report.get("tests_passed"),
        "tests_total": report.get("tests_total"),
    }
    verification["status"] = "VERIFIED"
    record["status"] = "VERIFIED"
    return True, f"{report.get('tests_passed')}/{report.get('tests_total')} cases"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", action="append", default=[],
                        help="Only run these addresses; the default is the whole MVP list.")
    parser.add_argument("--dry-run", action="store_true")
    arguments = parser.parse_args()
    manifest = json.loads(MANIFEST.read_text())
    records = {"0x" + r["address"][-8:].upper(): r for r in manifest["natives"]}
    addresses = arguments.address or mvp_addresses()
    passed = 0
    failed = 0
    for address in addresses:
        key = address if address.upper().startswith("0X") else "0x" + address[2:].upper()
        record = records.get(key)
        if record is None:
            print(f"{key}  no manifest record")
            failed += 1
            continue
        if arguments.dry_run:
            print(f"{key}  {record.get('name')}  "
                  f"{(record.get('verification') or {}).get('behavioral_runner', {}).get('path', 'no runner')}")
            continue
        ok, detail = run_one(record, key)
        if ok:
            passed += 1
            print(f"{key}  {record.get('name'):34s} PASS  {detail}")
        else:
            failed += 1
            print(f"{key}  {record.get('name'):34s} FAIL  {detail}")
    if not arguments.dry_run:
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"\npassed: {passed}  failed: {failed}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
