#!/usr/bin/env python3
"""Apply a verified-native patch to docs/fidelity/native-verification.json.

Shared-worktree safe: the manifest is read, patched and replaced under an
exclusive ``flock`` so concurrent agents cannot clobber each other's records.
Every referenced evidence path is hashed here, so a patch can never bind a
stale SHA-256 by hand.

Patch format (JSON object)::

    {
      "address": "0x004B26B0",
      "status": "VERIFIED",
      "implementation_signature": "void __cdecl ...",   // optional override
      "signature_comparison": { ... },                  // merged object
      "limitations": [ "..." ],                         // replaces the list
      "behavioral_runner": { "path": "...", "args": [ ... ], "timeout_seconds": 360 },
      "evidence": {
        "build": "docs/fidelity/evidence/0x004B26B0-build.json",
        "semantic_review": "docs/fidelity/evidence/0x004B26B0-semantic-review.json",
        "behavioral": "docs/fidelity/evidence/0x004B26B0-behavioral.json"
      },
      "runtime_tests": { "status": "PASS", "oracle": "retail-binary-execution" },
      "extra": { "cfg_similarity": ... }                // merchant keys into verification
    }

``runtime_tests`` is enriched with the behavioral evidence path/SHA-256 and the
current runner SHA-256 automatically.  ``extra`` is merged last, so it can
override any verification field.
"""

from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
import pathlib
import sys
import tempfile

MANIFEST = pathlib.Path("docs/fidelity/native-verification.json")
RUNNER = pathlib.Path("scripts/native_unicorn_runner.py")


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def normalize_address(value: str) -> str:
    return "0x%08X" % int(value, 16)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default=".")
    parser.add_argument("--patch", required=True, help="patch JSON file")
    parser.add_argument("--manifest", default=str(MANIFEST))
    args = parser.parse_args()

    root = pathlib.Path(args.root).resolve()
    manifest_path = root / args.manifest
    patch = json.loads(pathlib.Path(args.patch).read_text())
    address = normalize_address(patch["address"])
    evidence = patch.get("evidence", {})

    resolved_evidence: dict[str, dict[str, str]] = {}
    for key, rel in evidence.items():
        path = root / rel
        if not path.is_file():
            raise SystemExit("missing evidence file for %s: %s" % (key, path))
        resolved_evidence[key] = {"evidence_path": rel, "evidence_sha256": sha256(path)}

    lock_path = manifest_path.with_suffix(manifest_path.suffix + ".lock")
    with open(lock_path, "w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        manifest = json.loads(manifest_path.read_text())
        record = None
        for native in manifest["natives"]:
            if normalize_address(native["address"]) == address:
                record = native
                break
        if record is None:
            raise SystemExit("address not present in manifest: %s" % address)

        verification = record.setdefault("verification", {})
        implementation_path = root / record["implementation"]["path"]
        if not implementation_path.is_file():
            raise SystemExit("missing implementation: %s" % implementation_path)
        record["implementation"]["sha256"] = sha256(implementation_path)

        if "implementation_signature" in patch:
            record["implementation"]["signature"] = patch["implementation_signature"]

        if "status" in patch:
            verification["status"] = patch["status"]
        if "signature_comparison" in patch:
            merged = dict(verification.get("signature_comparison") or {})
            merged.update(patch["signature_comparison"])
            verification["signature_comparison"] = merged
        if "limitations" in patch:
            verification["limitations"] = list(patch["limitations"])
        if "behavioral_runner" in patch:
            verification["behavioral_runner"] = patch["behavioral_runner"]

        for key, value in resolved_evidence.items():
            verification[key] = value

        if "runtime_tests" in patch:
            runtime_tests = dict(patch["runtime_tests"])
            behavioral = resolved_evidence.get("behavioral")
            if behavioral is None:
                raise SystemExit("runtime_tests patch needs behavioral evidence")
            report = json.loads((root / behavioral["evidence_path"]).read_text())
            runtime_tests.setdefault("evidence_path", behavioral["evidence_path"])
            runtime_tests.setdefault("evidence_sha256", behavioral["evidence_sha256"])
            runtime_tests.setdefault("tests_passed", report.get("tests_passed"))
            runtime_tests.setdefault("tests_total", report.get("tests_total"))
            runtime_tests["runner_sha256"] = sha256(root / RUNNER)
            verification["runtime_tests"] = runtime_tests

        verification.update(patch.get("extra") or {})

        handle, tmp_name = tempfile.mkstemp(dir=str(manifest_path.parent), prefix=".manifest-")
        with os.fdopen(handle, "w") as stream:
            json.dump(manifest, stream, indent=2)
            stream.write("\n")
        os.replace(tmp_name, manifest_path)

    print("patched %s -> status %s" % (address, verification.get("status")))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
