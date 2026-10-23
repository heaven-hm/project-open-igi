#!/usr/bin/env python3
"""Audit every MVP manifest record using the gate's own validators.

Running the full gate over 120 natives means angr, radare2 and a behavioural
runner per native, which is too slow to sweep. This audit answers the narrower
question cheaply: for each MVP native, is the evidence behind its status label
present, current and accepted by the gate's own artifact validation?

It reuses `native_gate.validate_report_artifact` rather than reimplementing the
schema, so a record this audit reports as backed is a record the gate would also
accept. Two levels are counted separately, because they are different claims:

  * default evidence - build report, saved behavioural report and semantic
    review, the definition the manifest's VERIFIED label uses;
  * end-to-end evidence - the above plus a paired Frida report that records
    calls in both retail and the candidate.

It never edits anything.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

import native_gate  # noqa: E402

MVP = ROOT / "scripts" / "mvp_native_targets.json"
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
RETAIL_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def audit_record(record: dict, target: dict) -> dict:
    errors: list[str] = []
    address = native_gate.normalize_address(target["address"])
    implementation = record.get("implementation") or {}
    impl_path = ROOT / str(implementation.get("path", ""))
    if not impl_path.exists():
        errors.append(f"missing implementation {implementation.get('path')}")
        impl_digest = ""
    else:
        impl_digest = sha256(impl_path)
        if impl_digest != implementation.get("sha256"):
            errors.append("implementation digest does not match the record")
    binary_digest = (record.get("binary_sha256") or RETAIL_SHA).lower()
    verification = record.get("verification") or {}
    status = verification.get("status")
    rank = native_gate.STATUS_RANK.get(status, 0)

    build_errors: list[str] = []
    behavioral_errors: list[str] = []
    semantic_errors: list[str] = []
    if rank >= native_gate.STATUS_RANK["BUILD_VERIFIED"]:
        native_gate.validate_report_artifact(
            ROOT, verification.get("build"), report_name="build",
            expected_kind="build-report", native_address=address,
            binary_digest=binary_digest, implementation_digest=impl_digest,
            errors=build_errors)
    if rank >= native_gate.STATUS_RANK["RUNTIME_TESTED"]:
        # Older records keep the saved behavioural report under runtime_tests;
        # the generator writes it under behavioral. Either is the same claim.
        saved_ref = verification.get("runtime_tests") or verification.get("behavioral")
        native_gate.validate_report_artifact(
            ROOT, saved_ref, report_name="behavioural tests",
            expected_kind="behavioral-test-report", native_address=address,
            binary_digest=binary_digest, implementation_digest=impl_digest,
            errors=behavioral_errors)
    if rank >= native_gate.STATUS_RANK["VERIFIED"]:
        native_gate.validate_report_artifact(
            ROOT, verification.get("semantic_review"), report_name="semantic review",
            expected_kind="semantic-review-report", native_address=address,
            binary_digest=binary_digest, implementation_digest=impl_digest,
            errors=semantic_errors)

    frida_errors: list[str] = []
    if verification.get("frida_runtime"):
        artifact = native_gate.validate_report_artifact(
            ROOT, verification.get("frida_runtime"), report_name="Frida runtime probes",
            expected_kind="frida-runtime-report", native_address=address,
            binary_digest=binary_digest, implementation_digest=impl_digest,
            errors=frida_errors)
        if artifact is not None:
            if artifact.get("candidate_executable_sha256") in (None, ""):
                frida_errors.append("Frida evidence is not bound to a candidate executable")
            for key in ("retail_observed_calls", "candidate_observed_calls"):
                value = artifact.get(key)
                if not isinstance(value, int) or value <= 0:
                    frida_errors.append(f"Frida evidence records no {key.split('_')[0]} calls")
    else:
        frida_errors.append("no paired Frida report")

    runner_configured = bool(verification.get("behavioral_runner"))
    base_ok = not errors and not build_errors and not semantic_errors
    # The default gate validates the saved behavioural report. The strict gate
    # runs the configured runner live and then ignores the saved file, so a
    # record with a runner can be strict-clean while its saved file is stale.
    saved_backed = base_ok and not behavioral_errors
    strict_backed = base_ok and (not behavioral_errors or runner_configured)

    return {
        "address": target["address"],
        "name": target["name"],
        "status": status,
        "implementation_digest_matches": not errors,
        "record_errors": errors,
        "build_errors": build_errors,
        "behavioral_errors": behavioral_errors,
        "semantic_errors": semantic_errors,
        "frida_errors": frida_errors,
        "runner_configured": runner_configured,
        "saved_evidence_backed": saved_backed,
        "strict_evidence_backed": strict_backed,
        "end_to_end_backed": strict_backed and not frida_errors,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", type=pathlib.Path, default=None)
    args = parser.parse_args()

    targets = json.loads(MVP.read_text())
    manifest = json.loads(MANIFEST.read_text())
    records = {r["address"].lower(): r for r in manifest.get("natives") or []}

    rows = []
    for target in targets:
        record = records.get(target["address"].lower())
        if record is None:
            rows.append({"address": target["address"], "name": target["name"],
                         "status": None, "record_errors": ["no manifest record"],
                         "saved_evidence_backed": False,
                         "strict_evidence_backed": False,
                         "end_to_end_backed": False,
                         "build_errors": [], "behavioral_errors": [],
                         "semantic_errors": [], "frida_errors": [],
                         "implementation_digest_matches": False})
            continue
        rows.append(audit_record(record, target))

    labelled = [r for r in rows if r["status"] in {"VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE"}]
    saved_ok = [r for r in rows if r["saved_evidence_backed"]]
    strict_ok = [r for r in rows if r["strict_evidence_backed"]]
    end_to_end_ok = [r for r in rows if r["end_to_end_backed"]]

    print(f"MVP natives: {len(rows)}")
    print(f"  labelled VERIFIED: {len(labelled)}")
    print(f"  saved evidence accepted (default gate definition): {len(saved_ok)}")
    print(f"  runner configured, so the strict gate would run it live: {len(strict_ok)}"
          "  (upper bound: whether the runner supports each address is only proven by running it)")
    print(f"  end-to-end evidence (paired Frida with calls both sides): {len(end_to_end_ok)}")
    failing = [r for r in rows if not r["saved_evidence_backed"]]
    if failing:
        grouped: dict[str, int] = {}
        for row in failing:
            for error in (row["record_errors"] + row["build_errors"] +
                          row["behavioral_errors"] + row["semantic_errors"]):
                key = re.sub(r"^0x[0-9A-Fa-f]{8}:?\s*", "", error)
                key = key.split(" in docs/")[0][:90]
                grouped[key] = grouped.get(key, 0) + 1
        print(f"\n  {len(failing)} records are not backed; most common problems:")
        for message, count in sorted(grouped.items(), key=lambda kv: -kv[1])[:12]:
            print(f"    {count:3}  {message}")

    if args.json:
        args.json.write_text(json.dumps({
            "summary": {
                "total": len(rows),
                "labelled_verified": len(labelled),
                "saved_evidence_backed": len(saved_ok),
                "strict_evidence_backed": len(strict_ok),
                "end_to_end_backed": len(end_to_end_ok),
                "records_with_problems": len(failing),
            },
            "natives": rows}, indent=1) + "\n")
        print(f"\nwrote {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
