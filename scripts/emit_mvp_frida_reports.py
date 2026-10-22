#!/usr/bin/env python3
"""Turn a paired MVP capture into per-native frida-runtime evidence reports.

Only natives whose capture actually matched are emitted: both images must have
observed the function and both must have produced the same set of return-value
classes. Nothing is inferred for a native that failed either condition.

Each report states its basis in the report itself, so the evidence cannot be
read as more than it is: the native was reached in the running retail game and
in the running candidate, and the two returned the same class of value. Per-call
arguments are recorded but not compared, because the two runs are timed
independently.

With --write the reports are written under docs/fidelity/evidence and the
manifest gains a frida_runtime record for each emitted native. The manifest
implementation digest is taken from the file on disk and must still equal the
manifest's recorded digest, so a stale record is skipped rather than endorsed.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
EVIDENCE_DIR = ROOT / "docs" / "fidelity" / "evidence"


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def report_path(address: str) -> Path:
    return EVIDENCE_DIR / f"{address.lower()}-frida-runtime.json"


def archived_capture(capture_path: Path) -> Path:
    """Keep the paired capture beside the reports it supports.

    build/ is not versioned, so a report that cites a build artifact would lose
    its evidence on a clean checkout. The capture is archived under
    docs/fidelity/evidence/ and the reports point at the archived copy.
    """
    EVIDENCE_DIR.mkdir(parents=True, exist_ok=True)
    target = EVIDENCE_DIR / f"mvp-frida-pair-{capture_path.stem.split('-')[-1]}.json"
    target.write_text(capture_path.read_text())
    return target


def build_report(entry: dict, capture: dict, implementation_sha: str,
                 capture_path: Path) -> dict:
    comparison = entry["comparison"]
    return {
        "schema_version": 1,
        "kind": "frida-runtime-report",
        "result": "passed",
        "address": entry["address"].lower(),
        "binary_sha256": capture["retailSha256"],
        "implementation_sha256": implementation_sha,
        "candidate_executable_sha256": capture["candidateSha256"],
        "retail_observed_calls": entry["retail_observed_calls"],
        "candidate_observed_calls": entry["candidate_observed_calls"],
        "matched_calls": min(entry["retail_observed_calls"],
                             entry["candidate_observed_calls"]),
        "probe_sha256": capture["probeSha256"],
        "captureMode": capture["captureMode"],
        "level": capture["level"],
        "native_name": entry["name"],
        "addresses": {
            "retail_va": entry["retail_va"],
            "candidate_va": entry["candidate_va"],
        },
        "comparison": {
            "basis": comparison["basis"],
            "retail_ret_classes": comparison["retail_ret_classes"],
            "candidate_ret_classes": comparison["candidate_ret_classes"],
            "ret_classes_match": comparison["ret_classes_match"],
            "arguments_compared": False,
            "note": comparison["note"],
        },
        "evidence": {
            "paired_capture": str(capture_path.relative_to(ROOT)),
            "paired_capture_sha256": digest(capture_path),
        },
        "limitations": [
            "Retail and candidate are captured sequentially because both hold a "
            "single-instance mutex; the two runs attach at different points of level "
            "load, so call indices and call counts do not correspond.",
            "The probe classifies arguments and return values as null, a small exact "
            "value, or a pointer. Values behind pointers are not read, so this does "
            "not evaluate the data a native computes.",
            "This report establishes that the native was reached in both images and "
            "returned the same class of value for the configured level run. It does "
            "not establish behavioural parity or any side effect.",
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture", type=Path, required=True)
    parser.add_argument("--manifest", type=Path, default=MANIFEST)
    parser.add_argument("--write", action="store_true",
                        help="write the reports and rebind the manifest")
    parser.add_argument("--only", nargs="*", default=[],
                        help="limit to these addresses")
    args = parser.parse_args()

    capture_path = args.capture.resolve()
    capture = json.loads(capture_path.read_text())
    archived = archived_capture(capture_path) if args.write else capture_path
    manifest = json.loads(args.manifest.read_text())
    records = {record["address"].lower(): record for record in manifest["natives"]}
    wanted = {address.lower() for address in args.only}

    emitted, skipped = [], []
    for entry in capture["natives"]:
        address = entry["address"].lower()
        if wanted and address not in wanted:
            continue
        if entry["result"] != "match":
            skipped.append((entry["address"], entry["name"], entry["result"]))
            continue
        record = records.get(address)
        if not record:
            skipped.append((entry["address"], entry["name"], "no manifest record"))
            continue
        implementation = record.get("implementation") or {}
        impl_rel = implementation.get("path")
        impl_path = ROOT / impl_rel if impl_rel else None
        if not impl_path or not impl_path.exists():
            skipped.append((entry["address"], entry["name"], "implementation missing"))
            continue
        actual = digest(impl_path)
        if actual != implementation.get("sha256"):
            skipped.append((entry["address"], entry["name"],
                            "implementation digest is stale in the manifest"))
            continue
        report = build_report(entry, capture, actual, archived)
        path = report_path(entry["address"])
        emitted.append((entry, report, path))

    print(f"matched entries: {len(emitted)}")
    for entry, _report, path in emitted:
        print(f"  {entry['address']}  {entry['name']:34} "
              f"retail={entry['retail_observed_calls']:4} "
              f"cand={entry['candidate_observed_calls']:4}  -> {path.name}")
    print(f"skipped: {len(skipped)}")
    for address, name, reason in skipped[:40]:
        print(f"  {address}  {name:34} {reason}")

    if not args.write:
        print("\ndry run: nothing written")
        return 0

    for _entry, report, path in emitted:
        path.write_text(json.dumps(report, indent=1) + "\n")
        record = records[report["address"].lower()]
        record.setdefault("verification", {})["frida_runtime"] = {
            "status": "PASS",
            "evidence_path": str(path.relative_to(ROOT)),
            "evidence_sha256": digest(path),
        }
    args.manifest.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"\nwrote {len(emitted)} report(s) and rebound the manifest")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
