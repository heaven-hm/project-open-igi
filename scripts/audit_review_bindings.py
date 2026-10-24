#!/usr/bin/env python3
"""Audit (and optionally repair) semantic-review bindings in the manifest.

A code-backed record should point at a semantic review. Two failure modes exist:
  1. a review file exists under docs/fidelity/evidence/ but the manifest does not reference it;
  2. no review file exists at all, so the record has code but no review evidence.

This script reports both. With --bind it links the files it finds (case 1 only). Case 2 is never
invented here: writing a review after the fact, from the implementation rather than from the
retail instructions, would be a fabricated review, so those records are only reported.

Usage: python3 scripts/audit_review_bindings.py [--bind]
"""
from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
EVIDENCE = ROOT / "docs/fidelity/evidence"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--bind", action="store_true", help="link review files that already exist")
    options = parser.parse_args()

    manifest = json.loads(MANIFEST.read_text())
    implemented = {int(path.name[7:13], 16) for path in (ROOT / "engine/src/retail").glob("*/native_*.cpp")}

    unbound = []
    bindable = []
    for row in manifest.get("natives", []):
        try:
            address = int(row["address"], 16)
        except (KeyError, ValueError):
            continue
        if address not in implemented:
            continue
        if ((row.get("verification") or {}).get("semantic_review") or {}).get("evidence_path"):
            continue
        # Reviews live both at the evidence root and in dated subdirectories, and the
        # filename may carry the address in either case form, so search recursively
        # and match on the parsed address instead of on the filename spelling.
        matches = []
        for candidate in EVIDENCE.rglob("*-semantic-review*.json"):
            found = re.search(r"0x([0-9a-fA-F]{6,8})", candidate.name)
            if found and int(found.group(1), 16) == address:
                matches.append(candidate)
        matches.sort()
        unbound.append(row)
        if matches:
            bindable.append((row, matches[0]))

    print(f"code-backed records without a bound review: {len(unbound)}")
    print(f"  review file exists, bindable:             {len(bindable)}")
    print(f"  no review file anywhere:                  {len(unbound) - len(bindable)}")

    if options.bind and bindable:
        for row, path in bindable:
            row["verification"]["semantic_review"] = {
                "evidence_path": path.relative_to(ROOT).as_posix(),
                "evidence_sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
            }
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"bound {len(bindable)} review file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
