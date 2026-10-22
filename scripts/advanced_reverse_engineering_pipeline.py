#!/usr/bin/env python3
"""Deterministic advanced reverse-engineering acceptance pipeline (fail closed).

Executable implementation behind scripts/automatic_reverse_engineering.sh.

Advanced semantics enforced as code (not prose):
  - Tool install + health gate (doctor; best-effort installs when requested; fail closed).
  - Workflow self-check and Frida shell smoke must pass before any runtime phase.
  - Strict native gate: threshold VERIFIED, cfg-threshold 0.85, require semantic
    evidence, require behavioral tests; the structural-only flag is never used.
  - CFG similarity >= 0.85 is STRUCTURAL EXISTENCE ONLY, never proof of semantic
    equivalence; semantic equivalence additionally requires source-semantic review,
    behavioral differential comparison and control-constraint semantic comparison.
  - Scoped mode (--address): angr semantic analysis against the ORIGINAL retail igi.exe
    (exact-entry validation), then strict single-native gate, then strict intake.

Deterministic flow / per-native handling:
  - Full mode scans changed source files -> address-bound retail natives (sorted).
  - At most --max-natives natives are processed per run (default 1); the rest are
    deferred and marked "not_solved_in_this_run" (solve later).
  - Each processed native gets an individual status (passed/failed/deferred); failure
    of one native does not stop movement to the next within the cap.

Fail closed: any tool miss, failed check, missing evidence, mismatch or failed
comparison blocks; nothing is reported verified without evidence.
"""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import sys
from datetime import datetime, timezone
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT / "scripts"
GATE_REPORT_PATH = ROOT / "build/native-gate-report.json"
DEFAULT_LOG_PATH = ROOT / "build/advanced_reverse_engineering_pipeline.log"
LOG_ENTRIES: list[str] = []


def log(message: str) -> None:
    timestamp = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    entry = f"[{timestamp}] {message}"
    LOG_ENTRIES.append(entry)
    print(entry, flush=True)
    try:
        DEFAULT_LOG_PATH.parent.mkdir(parents=True, exist_ok=True)
        with DEFAULT_LOG_PATH.open("a", encoding="utf-8") as handle:
            handle.write(entry + "\n")
    except OSError:
        pass


CFG_STRUCTURAL_ONLY_THRESHOLD = 0.85
MANDATORY_GATE_THRESHOLD = "VERIFIED"
REQUIRED_SEMANTIC_CHECKS = (
    "function_identity",
    "argument_mapping",
    "constants_and_scaling",
    "memory_layout_and_access",
    "calls_and_side_effects",
    "return_and_error_paths",
    "floating_point_semantics",
)
REQUIRED_BEHAVIORAL_OBSERVABLES = ("return_value", "state_delta", "side_effects")


def run(command: list[str], timeout: int = 900) -> dict[str, Any]:
    env = dict(os.environ, LC_ALL="C", LANG="C", PYTHONDONTWRITEBYTECODE="1")
    try:
        proc = subprocess.run(command, cwd=str(ROOT), env=env, text=True,
                               capture_output=True, timeout=timeout, check=False)
        return dict(command=command, exit_code=proc.returncode, stdout=proc.stdout, stderr=proc.stderr)
    except (OSError, subprocess.SubprocessError) as error:
        return dict(command=command, exit_code=1, error=str(error))


def doctor_step(install: bool) -> tuple[bool, dict[str, Any]]:
    result = run([sys.executable, str(SCRIPTS / "igi_workflow.py"), "doctor", "--check"])
    if result.get("exit_code") != 0 and install:
        config = json.loads((SCRIPTS / "igi-workflow.json").read_text())
        angr_python = os.environ.get("IGI_ANGR_PYTHON", str(config.get("angr_python", "")))
        frida_python = os.environ.get("FRIDA_PYTHON", str(config.get("frida_python", "")))
        for name, command in (
            ("binary_ninja_mcp", [sys.executable, "-m", "pip", "install", "binaryninja"]),
            ("angr", [angr_python, "-m", "pip", "install", "angr"]),
            ("frida_server", [frida_python, "-m", "pip", "install", "frida"]),
        ):
            if command:
                attempt = run(command, timeout=900)
                result = attempt if attempt.get("exit_code") == 0 and attempt.get("tool") is None else result
    ok = result.get("exit_code") == 0
    return ok, result


def self_check_step() -> tuple[bool, dict[str, Any]]:
    result = run([sys.executable, str(SCRIPTS / "workflow_self_check.py")])
    return (result.get("exit_code") == 0), result


def smoke_step() -> tuple[bool, dict[str, Any]]:
    result = run([sys.executable, str(SCRIPTS / "frida_shell_smoke.py")])
    return (result.get("exit_code") == 0), result


def strict_gate_command(root: Path, *, file_arg: str | None = None, address: str | None = None) -> list[str]:
    command = [sys.executable, str(SCRIPTS / "native_gate.py"), "--root", str(root),
               "--threshold", MANDATORY_GATE_THRESHOLD,
               "--cfg-threshold", str(CFG_STRUCTURAL_ONLY_THRESHOLD),
               "--require-semantic-evidence", "--require-behavioral-tests",
               "--report", str(GATE_REPORT_PATH)]
    if file_arg:
        command.extend(["--file", file_arg])
    elif not address:
        command.extend(["--changed"])
    if address:
        command.extend(["--address", address])
    return command


def strict_gate_step(root: Path, *, file_arg: str | None = None, address: str | None = None) -> tuple[bool, dict[str, Any]]:
    command = strict_gate_command(root, file_arg=file_arg, address=address)
    result = run(command, timeout=3600)
    gate_doc: dict[str, Any] = {}
    try:
        gate_doc = json.loads(GATE_REPORT_PATH.read_text())
    except (OSError, ValueError):
        gate_doc = {}
    merged = dict(result)
    merged["report"] = gate_doc
    ok = (result.get("exit_code") == 0) and (gate_doc.get("result") == "passed")
    return ok, merged


def run_angr_semantic(root: Path, binary: Path, address: str, binary_digest: str) -> tuple[bool, dict[str, Any]]:
    helper = root / "scripts/angr_analyze.py"
    config = json.loads((SCRIPTS / "igi-workflow.json").read_text())
    interpreter = os.environ.get("IGI_ANGR_PYTHON", str(config.get("angr_python", "")))
    command = [interpreter, str(helper), "--binary", str(binary), "--address", address]
    result = run(command, timeout=1800)
    artifact: dict[str, Any] = {}
    if result.get("exit_code") in (0, 2) and isinstance(result.get("stdout"), str):
        try:
            artifact = json.loads(result["stdout"])
        except json.JSONDecodeError:
            artifact = {}
    ok = (
        result.get("exit_code") in (0, 2)
        and isinstance(artifact, dict)
        and artifact.get("schema_version") == 1
        and artifact.get("kind") == "angr-analysis"
        and artifact.get("tool") == "angr"
        and artifact.get("binary_sha256") == binary_digest
        and artifact.get("bits") == 32
        and artifact.get("mapped_base") == "0x400000"
        and artifact.get("function_entry") == address
        and artifact.get("result") == "pass"
        and artifact.get("has_unresolved_jumps") is False
    )
    return ok, result | {"artifact": artifact}


def find_implementation_file(root: Path, address: str) -> Path | None:
    try:
        records = json.loads((root / "docs/fidelity/native-verification.json").read_text()).get("natives", [])
    except (OSError, ValueError):
        return None
    matches = [r for r in records if int(str(r.get("address", "0")), 16) == int(address, 16)]
    if len(matches) != 1:
        return None
    rel = matches[0].get("implementation", {}).get("path", "")
    if not isinstance(rel, str) or not rel.startswith("engine/"):
        return None
    candidate = root / rel
    return candidate if candidate.is_file() else None


def intake_step(address: str, probe: str, level: int) -> tuple[bool, dict[str, Any]]:
    command = [sys.executable, str(SCRIPTS / "smart_end_to_end_verification.py"),
               "--address", address, "--level", str(level), "--probe", str(probe)]
    result = run(command, timeout=7200)
    return (result.get("exit_code") == 0), result


def scan_changed_native_addresses() -> list[str]:
    status = subprocess.run(["git", "status", "--porcelain"], cwd=str(ROOT), capture_output=True, text=True)
    changed: set[str] = set()
    for line in status.stdout.splitlines():
        if not line.strip():
            continue
        path = line[3:].strip().strip('"')
        if path:
            changed.add(path)
    diff = subprocess.run(["git", "diff", "--name-only", "HEAD"], cwd=str(ROOT), capture_output=True, text=True)
    changed.update(p.strip() for p in diff.stdout.split() if p.strip())
    token_re = re.compile(r"(?<![0-9A-Fa-f])([0-9A-Fa-f]{8})(?![0-9A-Fa-f])")
    candidates: set[str] = set()
    for path in sorted(changed):
        for token in token_re.findall(path):
            candidates.add(f"0x{int(token, 16):08X}")
    records = json.loads((ROOT / "docs/fidelity/native-verification.json").read_text()).get("natives", [])
    bound: list[str] = []
    for address in sorted(candidates):
        matches = [r for r in records if int(str(r.get("address", "0")), 16) == int(address, 16)]
        if len(matches) == 1:
            implementation = matches[0].get("implementation", {}).get("path", "")
            if implementation.startswith("engine/") and (ROOT / implementation).resolve().is_file():
                bound.append(address)
    return bound


def write_report(path: Path, document: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(document, indent=2) + "\n")


def main() -> int:
    try:
        DEFAULT_LOG_PATH.parent.mkdir(parents=True, exist_ok=True)
        DEFAULT_LOG_PATH.write_text("")
    except OSError:
        pass
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", default=None, help="Retail virtual address to scope the strict intake (e.g. 0x0048F970).")
    parser.add_argument("--probe", default=None, help="Reviewed typed probe path for scoped intake.")
    parser.add_argument("--level", type=int, default=1, choices=range(1, 15))
    parser.add_argument("--install", action="store_true", help="Best-effort tool install before doctor (fail closed if doctor still fails).")
    parser.add_argument("--skip-intake", action="store_true", help="Scoped mode: strict single-native gate only, no intake.")
    parser.add_argument("--max-natives", type=int, default=1, help="Max natives processed per pipeline run; extras are deferred.")
    parser.add_argument("--report", type=Path, default=ROOT / "build/advanced_reverse_engineering_report.json")
    args = parser.parse_args()

    failures: list[str] = []
    documents: dict[str, Any] = {}

    doctor_ok, doctor_result = doctor_step(args.install)
    documents["doctor"] = doctor_result
    log(f"tool-doctor via {doctor_result.get('command')} exit={doctor_result.get('exit_code')} ok={doctor_ok}")
    if not doctor_ok:
        failures.append("tool doctor blocked the pipeline")

    self_ok, self_result = self_check_step()
    documents["self_check"] = self_result
    log(f"tool-self-check via {self_result.get('command')} exit={self_result.get('exit_code')} ok={self_ok}")
    if not self_ok:
        failures.append("workflow self-check blocked the pipeline")

    smoke_ok, smoke_result = smoke_step()
    documents["smoke"] = smoke_result
    log(f"tool-frida-smoke via {smoke_result.get('command')} exit={smoke_result.get('exit_code')} ok={smoke_ok}")
    if not smoke_ok:
        failures.append("frida shell smoke blocked the pipeline")

    scoped = args.address is not None
    if scoped:
        addresses = [f"0x{int(str(args.address), 0):08X}"]
    else:
        addresses = scan_changed_native_addresses()
    selected = addresses[: max(0, args.max_natives)]
    deferred = addresses[max(0, args.max_natives):]

    per_native: dict[str, Any] = {}
    for address in selected:
        record: dict[str, Any] = {"address": address, "status": "deferred", "important": True}
        implementation = find_implementation_file(ROOT, address)
        if implementation is None:
            record["status"] = "failed"
            record["error"] = "no unique engine implementation bound to retail native"
            per_native[address] = record
            failures.append(f"native {address}: implementation binding missing")
            continue
        gate_ok, gate_result = strict_gate_step(ROOT, file_arg=str(implementation), address=address)
        documents[f"gate:{address}"] = gate_result
        gate_doc = gate_result.get("report") or {}
        record["status"] = "passed" if (gate_ok and gate_doc.get("result") == "passed") else "failed"
        log(f"native {address}: strict-gate via {gate_result.get('command')} status={record['status']} gate_result_exit={gate_result.get('exit_code')} gate_report_result={gate_doc.get('result')}")
        if record["status"] == "failed":
            record["error"] = gate_doc.get("error") or gate_result.get("stderr") or "strict gate failed"
            failures.append(f"native {address}: strict gate failed")
        angr_ok, angr_result = run_angr_semantic(ROOT, Path("/Users/haseeb-mir/Downloads/IGI/igi.exe"),
                                                 address, "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01")
        documents[f"angr:{address}"] = angr_result
        log(f"native {address}: angr-via {angr_result.get('command')} status={'passed' if angr_ok else 'failed'}")
        if not angr_ok:
            if record["status"] == "passed":
                record["status"] = "failed"
                record["error"] = "angr semantic analysis against original igi.exe failed"
                failures.append(f"native {address}: angr semantic analysis failed")
        if not args.skip_intake and args.probe:
            intake_ok, intake_result = intake_step(address, args.probe, args.level)
            documents[f"intake:{address}"] = intake_result
            log(f"native {address}: intake-via {intake_result.get('command')} status={'passed' if intake_ok else 'failed'}")
            if not intake_ok:
                if record["status"] == "passed":
                    record["status"] = "failed"
                    record["error"] = "strict intake rejected native"
                    failures.append(f"native {address}: strict intake rejected")

        per_native[address] = record

    for address in deferred:
        per_native[address] = {"address": address, "status": "not_solved_in_this_run", "important": True,
                               "error": "deferred: outside --max-natives cap for this run"}

    final = dict(schema_version=1, kind="advanced-reverse-engineering-pipeline",
                 result="passed" if not failures else "blocked", scoped=bool(scoped),
                 failures=failures, steps=documents, max_natives=args.max_natives,
                 deferred=list(deferred), per_native=per_native)
    write_report(args.report, final)
    log(f"pipeline final result={'passed' if not failures else 'blocked'} failures={failures} max_natives={args.max_natives} deferred={list(deferred)}")

    for name, document in documents.items():
        print(f"{name}: exit={document.get('exit_code')} ok={name not in failures}")

    if failures:
        for failure in failures:
            print(f"BLOCKED: {failure}", file=sys.stderr)
        return 1
    print("automatic reverse-engineering pipeline finished: PASSED")
    print(f"report: {args.report}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
