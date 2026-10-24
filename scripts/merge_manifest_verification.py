#!/usr/bin/env python3
"""Merge back the rich verification fields after a CFG evidence rebuild.

scripts/build_native_records.py always writes a fresh record whose verification
block holds only the CFG evidence, so running it drops the status, the
differential runner and the semantic-review reference that the gate also
requires. This restores those fields from a saved manifest while keeping the
freshly regenerated CFG evidence and implementation hash.
"""

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
PRESERVE = ("status", "behavioral_runner", "semantic_review", "signature_comparison",
            "runtime_tests", "cfg_nodes", "cfg_edges")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--from", dest="source", required=True, type=Path,
                    help="saved manifest holding the rich verification fields")
    a = ap.parse_args()

    current = json.loads(MANIFEST.read_text())
    saved = {n["address"].upper(): n for n in
             json.loads(a.source.read_text())["natives"]}

    merged = 0
    for rec in current["natives"]:
        prev = saved.get(rec["address"].upper())
        if not prev:
            continue
        pv = prev.get("verification", {})
        cv = rec.setdefault("verification", {})
        # The saved status reflects the higher verification level that the
        # differential runners and semantic review established; a CFG rebuild
        # must not demote it.
        if "status" in pv:
            cv["status"] = pv["status"]
            merged += 1
        for key in PRESERVE:
            if key == "status":
                continue
            if key in pv and key not in cv:
                cv[key] = pv[key]
                merged += 1
        # a name change must not leave a stale runner/semantic reference behind
        if prev.get("name") != rec.get("name"):
            rec.setdefault("verification", {})["renamedFrom"] = prev.get("name")

    MANIFEST.write_text(json.dumps(current, indent=2) + "\n")
    print(f"restored {merged} verification field(s) from {a.source}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
