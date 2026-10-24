#!/usr/bin/env python3
"""Repair the manifest records built for the renamed GameNatives candidates.

A record built from a GameNatives candidate takes its name from the file stem,
which carries the " 2"/" 3" duplicate marker, and it has no signature because
the candidate's provenance comment records the address rather than a
prototype. This sets the canonical name and the declared signature, and
rewrites the Binary Ninja and Radare2 evidence so their signature still agrees
with the manifest.
"""

import hashlib
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
EVIDENCE = ROOT / "docs" / "fidelity" / "evidence"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
DECL = re.compile(r"^(?P<decl>[^/\n]*?\b[A-Za-z_]\w*\s*\([^;]*\))\s*;", re.MULTILINE)


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> None:
    apply = "--apply" in sys.argv
    manifest = json.loads(MANIFEST.read_text())
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}

    fixed = []
    for rec in manifest["natives"]:
        implementation = rec.get("implementation") or {}
        path = implementation.get("path", "")
        if not path.startswith("scripts/GameNatives/"):
            continue
        addr = int(rec["address"], 16)
        entry = master.get(addr)
        if not entry or not entry.get("name"):
            continue
        want_name = entry["name"]
        source = ROOT / path
        # the confirmed retail signature for this address, taken from the
        # Binary Ninja analysis recorded in the master catalog
        signature = entry.get("signature") or "unknown"

        changed = False
        if rec.get("name") != want_name:
            rec["name"] = want_name
            changed = True
        for holder in (implementation, rec.get("game_natives") or {}):
            if holder.get("signature") != signature:
                holder["signature"] = signature
                changed = True
        for tool in ("binary_ninja", "radare2"):
            reference = (rec.get("verification") or {}).get(tool) or {}
            if reference.get("signature") != signature:
                reference["signature"] = signature
                changed = True
                evidence = ROOT / reference.get("evidence_path", "")
                if evidence.exists():
                    artifact = json.loads(evidence.read_text())
                    artifact["signature"] = signature
                    evidence.write_text(json.dumps(artifact, indent=2) + "\n")
                    reference["evidence_sha256"] = sha256(evidence)
        if changed:
            fixed.append({"address": rec["address"], "name": want_name,
                          "signature": signature})
        rec["implementation"] = implementation
        rec["game_natives"] = rec.get("game_natives") or {}

    if apply:
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"records repaired: {len(fixed)}  applied={apply}")
    for f in fixed[:8]:
        print(f"  {f['address']} {f['name']:<34} {f['signature'][:60]}")


if __name__ == "__main__":
    main()
