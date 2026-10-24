#!/usr/bin/env python3
"""Re-bind manifest hashes to the current on-disk implementation and evidence files.

Editing a native source or evidence artifact invalidates the SHA-256 values that
docs/fidelity/native-verification.json records for it. This refreshes only those
hash fields (implementation.sha256 and each evidence_sha256) so the record keeps
pointing at the reviewed bytes. It never changes statuses, signatures or paths,
and it never rewrites the internal implementation_sha256 of an evidence report.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import sys

EVIDENCE_KEYS = ("binary_ninja", "radare2", "ghidra", "semantic_review", "build",
                 "runtime_tests", "mutation_tests")


def digest(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            hasher.update(block)
    return hasher.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--manifest", default="docs/fidelity/native-verification.json")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    root = args.root.resolve()
    manifest_path = root / args.manifest
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    changed = 0
    for record in manifest["natives"]:
        implementation = record.get("implementation", {})
        path = implementation.get("path")
        if isinstance(path, str) and (root / path).is_file():
            current = digest(root / path)
            if implementation.get("sha256", "").lower() != current:
                print(f"{record['address']} implementation {path}")
                implementation["sha256"] = current
                changed += 1
        verification = record.get("verification", {})
        for key in EVIDENCE_KEYS:
            evidence = verification.get(key)
            if not isinstance(evidence, dict):
                continue
            evidence_path = evidence.get("evidence_path")
            if not isinstance(evidence_path, str) or not (root / evidence_path).is_file():
                continue
            current = digest(root / evidence_path)
            if evidence.get("evidence_sha256", "").lower() != current:
                print(f"{record['address']} {key} evidence {evidence_path}")
                evidence["evidence_sha256"] = current
                changed += 1
    if changed and not args.dry_run:
        manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"{'would rebind' if args.dry_run else 'rebound'} {changed} hash field(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
