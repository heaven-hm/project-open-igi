#!/usr/bin/env python3
"""Refresh the manifest snapshot inside existing static captures.

`static_probe_capture.py` embeds the relevant manifest record (implementation
path, recorded sha256, current sha256, status) at capture time. When the Lead
refreshes docs/fidelity/native-verification.json (for example after a shared
translation unit changes), old captures would keep asserting the stale digest
and the individual record's static axis would fail for the wrong reason.

This re-reads the current manifest for the capture's address, recomputes the
implementation digest from the working tree, and rewrites only the `manifest`
block, recording a `manifest_rebind` history entry. Retail static evidence
(BN/r2/angr) is never touched.
"""
from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
from pathlib import Path


def canonical(value: str) -> str:
    return f"0x{int(value, 0):08x}"


def digests(path: Path) -> str | None:
    return hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--manifest", type=Path,
                    default=Path(__file__).resolve().parents[2] / "docs/fidelity/native-verification.json")
    args = ap.parse_args()

    manifest = json.loads(args.manifest.read_text())
    by_address = {canonical(entry["address"]): entry for entry in manifest.get("natives", [])
                  if entry.get("address")}
    stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    updated = []
    for path in sorted((args.run_dir / "static").glob("*-static.json")):
        doc = json.loads(path.read_text())
        address = canonical(doc["address"])
        entry = by_address.get(address)
        if not entry:
            continue
        implementation = entry.get("implementation") or {}
        source = implementation.get("path")
        recorded = implementation.get("sha256")
        now = digests(args.manifest.parents[2] / source) if source else None
        status = (entry.get("verification") or {}).get("status") or entry.get("status")
        doc["manifest"] = dict(
            name=entry.get("name"),
            status=status,
            implementation=source,
            implementation_sha256_now=now,
            implementation_sha256_recorded=recorded,
            implementation_current=bool(now and recorded == now),
            game_natives=(entry.get("game_natives") or {}).get("path"),
        )
        doc.setdefault("manifest_rebind", []).append(dict(
            stamp=stamp, status=status, implementation=source,
            recorded=recorded, current=now, matches=bool(now and recorded == now),
            note="manifest snapshot re-read from docs/fidelity/native-verification.json",
        ))
        path.write_text(json.dumps(doc, indent=2) + "\n")
        updated.append((address, doc["manifest"]["implementation_current"]))

    for address, ok in updated:
        print(f"{address} implementation_matches_manifest={ok}")
    print(f"{len(updated)} captures refreshed from {args.manifest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
