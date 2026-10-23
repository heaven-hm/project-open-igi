#!/usr/bin/env python3
"""Regenerate saved behavioural reports by running each record's own runner.

The audit found that most unbacked MVP records fail not because the behaviour is
unverified but because the saved behavioural artifact is malformed or stale: it
does not identify the runner, carries the wrong command shape, or records a
runner digest that no longer matches. The only honest fix is to run the runner
the record configures and store what it actually returns.

For each MVP record this tool:

  * substitutes the record's own implementation and binary digests into its
    configured runner arguments,
  * runs the runner,
  * on a valid `behavioral-test-report` with a non-zero test count, stores it at
    the record's behavioural evidence path and rebinds the digest,
  * on failure (an address the runner does not support, a crash, malformed
    output) leaves the record untouched and reports it.

Nothing is invented: a record whose runner cannot produce a report keeps no
valid report and is listed as a gap.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

import native_gate  # noqa: E402

MVP = ROOT / "scripts" / "mvp_native_targets.json"
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
RETAIL = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
# The gate runs its behavioural runners with these interpreters in the
# environment; a runner started without them reports "angr is unavailable".
RUNNER_ENVIRONMENT = {
    "IGI_ANGR_PYTHON": "/Users/haseeb-mir/Documents/Code/project-open-igi/.venv-angr/bin/python",
    "IGI_UNICORN_PYTHON": "/Users/haseeb-mir/Documents/Code/project-open-igi/build/unicorn-rosetta/bin/python",
}


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def substitute(args: list[str], values: dict[str, str]) -> list[str]:
    out = []
    for arg in args:
        for key, value in values.items():
            arg = arg.replace("{" + key + "}", value)
        out.append(arg)
    return out


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=pathlib.Path, default=MANIFEST)
    parser.add_argument("--report", type=pathlib.Path,
                        default=ROOT / "docs" / "fidelity" / "mvp-behavioral-refresh.json")
    parser.add_argument("--write", action="store_true",
                        help="store the runner reports; without this it is a dry run")
    parser.add_argument("--only", nargs="*", default=None)
    args = parser.parse_args()

    targets = json.loads(MVP.read_text())
    manifest = json.loads(args.manifest.read_text())
    records = {r["address"].lower(): r for r in manifest.get("natives") or []}
    binary_sha = (sha256(RETAIL) if RETAIL.exists()
                  else "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01")

    results = []
    written = unsupported = failed = skipped = 0
    for target in targets:
        address = target["address"]
        if args.only and native_gate.normalize_address(address) not in {
                native_gate.normalize_address(a) for a in args.only}:
            continue
        record = records.get(address.lower())
        if record is None:
            continue
        verification = record.get("verification") or {}
        runner = verification.get("behavioral_runner")
        if not isinstance(runner, dict) or not runner.get("path"):
            results.append({"address": address, "name": target["name"],
                            "outcome": "no runner configured"})
            skipped += 1
            continue
        implementation = record.get("implementation") or {}
        impl_path = ROOT / str(implementation.get("path", ""))
        if not impl_path.exists():
            results.append({"address": address, "name": target["name"],
                            "outcome": "implementation file is missing"})
            skipped += 1
            continue
        impl_sha = sha256(impl_path)
        if impl_sha != implementation.get("sha256"):
            results.append({"address": address, "name": target["name"],
                            "outcome": "record implementation digest is stale; "
                                       "regenerate its evidence first"})
            skipped += 1
            continue
        argv = [sys.executable, str(ROOT / runner["path"])] + substitute(
            list(runner.get("args") or []),
            {"address": address, "binary": str(RETAIL), "binary_sha256": binary_sha,
             "implementation": str(impl_path.relative_to(ROOT)),
             "implementation_sha256": impl_sha, "root": str(ROOT)})
        try:
            environment = dict(os.environ)
            environment.update(RUNNER_ENVIRONMENT)
            completed = subprocess.run(argv, capture_output=True, text=True,
                                       env=environment,
                                       timeout=int(runner.get("timeout_seconds") or 600))
        except subprocess.TimeoutExpired:
            results.append({"address": address, "name": target["name"],
                            "outcome": "runner timed out", "command": argv})
            failed += 1
            continue
        raw = completed.stdout.strip()
        try:
            report = json.loads(raw)
        except ValueError:
            report = None
        if (completed.returncode != 0 or not isinstance(report, dict)
                or report.get("kind") != "behavioral-test-report"
                or not report.get("tests_total")):
            message = (completed.stderr or raw or "").strip().splitlines()
            results.append({
                "address": address, "name": target["name"],
                "outcome": ("runner does not support this address"
                            if "unsupported native address" in (completed.stderr or "")
                            else "runner did not produce a behavioural report"),
                "detail": message[-1] if message else "",
                "exit_code": completed.returncode,
            })
            if "unsupported native address" in (completed.stderr or ""):
                unsupported += 1
            else:
                failed += 1
            continue

        # Some runners emit their observations without naming who produced
        # them. The orchestrator knows the exact program it executed, so it can
        # attest that identity truthfully: the runner path, its digest, and the
        # argv that ran. This is a wrapper record of the invocation, not an
        # invented result.
        existing_runner = report.get("runner")
        existing_command = existing_runner.get("command") if isinstance(existing_runner, dict) else None
        command_is_argv = (isinstance(existing_command, list) and existing_command
                           and all(isinstance(item, str) and item.strip() for item in existing_command))
        if not isinstance(existing_runner, dict) or not command_is_argv:
            report["runner"] = {
                "path": runner["path"],
                "sha256": sha256(ROOT / runner["path"]),
                "command": argv,
            }

        # Never downgrade a record: a runner can emit a report the gate rejects,
        # for example one that does not identify the runner that produced it.
        # Validate before storing, and leave the existing artifact alone if the
        # new one would not satisfy the gate.
        validation_errors: list[str] = []
        # The gate only accepts evidence under docs/fidelity/evidence, so stage
        # the candidate report there, validate it, and remove it again.
        scratch = ROOT / "docs" / "fidelity" / "evidence" / "_validation-candidate.json"
        scratch.write_text(json.dumps(report, indent=1) + "\n")
        try:
            native_gate.validate_report_artifact(
                ROOT, {"evidence_path": str(scratch.relative_to(ROOT)),
                       "evidence_sha256": sha256(scratch)},
                report_name="behavioural tests", expected_kind="behavioral-test-report",
                native_address=native_gate.normalize_address(address),
                binary_digest=binary_sha, implementation_digest=impl_sha,
                errors=validation_errors)
        finally:
            scratch.unlink(missing_ok=True)
        if validation_errors:
            results.append({"address": address, "name": target["name"],
                            "outcome": "runner report fails gate validation; left the existing artifact in place",
                            "detail": validation_errors[0][:120]})
            failed += 1
            continue

        ref_key = "runtime_tests" if verification.get("runtime_tests") else "behavioral"
        ref = verification.get(ref_key)
        if not isinstance(ref, dict) or not ref.get("evidence_path"):
            results.append({"address": address, "name": target["name"],
                            "outcome": "record has no behavioural evidence path to rebind"})
            skipped += 1
            continue
        path = ROOT / ref["evidence_path"]
        results.append({
            "address": address, "name": target["name"], "outcome": "report stored",
            "result": report.get("result"), "tests_passed": report.get("tests_passed"),
            "tests_total": report.get("tests_total"), "evidence_path": ref["evidence_path"],
        })
        if not args.write:
            continue
        path.write_text(json.dumps(report, indent=1) + "\n")
        ref["evidence_sha256"] = sha256(path)
        ref["result"] = report.get("result")
        ref["tests_passed"] = report.get("tests_passed")
        ref["tests_total"] = report.get("tests_total")
        written += 1

    if args.write:
        args.manifest.write_text(json.dumps(manifest, indent=2) + "\n")
    args.report.write_text(json.dumps({
        "summary": {"stored": written, "unsupported": unsupported,
                    "failed": failed, "skipped": skipped},
        "natives": results}, indent=1) + "\n")

    print(f"stored: {written}  unsupported: {unsupported}  failed: {failed}  skipped: {skipped}")
    for row in results:
        if row["outcome"] != "report stored":
            print(f"  {row['address']} {row['name']:34} {row['outcome']}"
                  + (f"  ({row['detail'][:70]})" if row.get("detail") else ""))
    print(f"wrote {args.report.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
