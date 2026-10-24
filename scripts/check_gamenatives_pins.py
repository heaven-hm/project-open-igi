#!/usr/bin/env python3
"""Check the project manifest's pinned GameNatives paths and hashes.

docs/fidelity/native-verification.json records, for every implemented native, the
GameNatives source file it was transcribed from together with that file's
SHA-256. Renaming a GameNatives file invalidates the pin, so this must be
re-checked whenever the tree is renamed.
"""

import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> None:
    doc = json.loads(MANIFEST.read_text())
    rows = []
    for n in doc["natives"]:
        gn = n.get("game_natives") or {}
        rel = gn.get("path")
        if not rel:
            continue
        p = ROOT / rel
        exists = p.exists()
        digest = sha256(p) if exists else None
        rows.append({
            "address": n["address"],
            "name": n["name"],
            "path": rel,
            "exists": exists,
            "expected": gn.get("sha256"),
            "actual": digest,
            "hashOk": bool(digest) and digest == gn.get("sha256"),
        })

    missing = [r for r in rows if not r["exists"]]
    mismatch = [r for r in rows if r["exists"] and not r["hashOk"]]
    ok = [r for r in rows if r["exists"] and r["hashOk"]]

    print(f"pinned GameNatives references : {len(rows)}")
    print(f"  file present and hash matches: {len(ok)}")
    print(f"  file present, hash differs  : {len(mismatch)}")
    print(f"  file missing                : {len(missing)}")
    for r in missing:
        print(f"    MISSING {r['address']} {r['name']}: {r['path']}")
    for r in mismatch[:10]:
        print(f"    CHANGED {r['address']} {r['name']}: {r['path']}")

    dest = ROOT / "docs" / "fidelity" / "evidence" / "gamenatives-pin-check.json"
    dest.write_text(json.dumps({
        "checked": len(rows), "ok": len(ok),
        "missing": missing, "hashMismatch": mismatch,
    }, indent=2) + "\n")
    print(f"written {dest}")
    return 1 if missing or mismatch else 0


if __name__ == "__main__":
    raise SystemExit(main())
