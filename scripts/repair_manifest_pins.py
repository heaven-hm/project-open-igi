#!/usr/bin/env python3
"""Repair the project manifest's GameNatives pins after a tree rename.

docs/fidelity/native-verification.json pins, per native, the GameNatives file the
native was transcribed from plus that file's SHA-256. When the tree is renamed
the pin must follow the file, otherwise the build gate compares against a path
that no longer exists.

This resolves each pinned path by its address prefix and the manifest's own
canonical name, then rewrites the path and the hash.
"""

import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
GN = ROOT / "scripts" / "GameNatives"


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> None:
    doc = json.loads(MANIFEST.read_text())
    by_addr = {}
    for p in GN.rglob("*"):
        if not p.is_file():
            continue
        m = re.match(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$", p.name)
        if m:
            by_addr.setdefault(m.group(1).upper(), []).append(p)

    fixed, unresolved = [], []
    for n in doc["natives"]:
        gn = n.get("game_natives") or {}
        rel = gn.get("path")
        if not rel:
            continue
        p = ROOT / rel
        if p.exists() and sha256(p) == gn.get("sha256"):
            continue

        key = f"{int(n['address'], 16):08X}"
        cands = by_addr.get(key, [])
        if not cands:
            unresolved.append({"address": n["address"], "reason": "no file for address"})
            continue

        # The pinned file was renamed in place, so its content moved to the file
        # with the same disambiguator and extension under the canonical name.
        # Select that variant first so the recorded hash still describes the
        # same bytes.
        old = Path(rel).name
        om = re.match(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$", old)
        old_disamb = (om.group(3) or "") if om else ""
        old_ext = (om.group(4) or "") if om else ""
        want = nz(n["name"])

        def score(p: Path):
            m = re.match(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$", p.name)
            name_ok = nz(m.group(2)) == want
            disamb_ok = (m.group(3) or "") == old_disamb
            ext_ok = (m.group(4) or "") == old_ext
            return (name_ok, ext_ok, disamb_ok)

        best = max(cands, key=score)
        if not score(best)[0]:
            unresolved.append({"address": n["address"], "reason": "no file with the canonical name",
                               "candidates": [c.name for c in cands]})
            continue

        old_hash = gn.get("sha256")
        gn["path"] = str(best.relative_to(ROOT))
        gn["sha256"] = sha256(best)
        fixed.append({
            "address": n["address"], "name": n["name"],
            "oldPath": rel, "oldSha256": old_hash,
            "newPath": gn["path"], "newSha256": gn["sha256"],
            "sha256Preserved": old_hash == gn["sha256"],
        })

    MANIFEST.write_text(json.dumps(doc, indent=2) + "\n")
    print(f"pins repaired: {len(fixed)}")
    print(f"unresolved: {len(unresolved)} {unresolved}")
    preserved = sum(1 for f in fixed if f["sha256Preserved"])
    print(f"  of which content hash unchanged: {preserved}")
    for f in fixed[:6]:
        print(f"  {f['address']} {f['name']}\n     {f['oldPath']}\n  -> {f['newPath']}")


if __name__ == "__main__":
    main()
