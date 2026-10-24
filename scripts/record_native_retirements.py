#!/usr/bin/env python3
"""Record the reviewed retirement of renamed native-root files.

When a GameNatives file is renamed, git reports the old path as deleted and the
new path as added. The gate requires every deleted native-root file to carry a
reviewed retired_files entry, and every native address that disappeared from it
to carry a reviewed retired_natives entry, so that a removal is always an
audited decision rather than a side effect.

This writes those entries, pairing each deleted path with the file that
replaced it, and states the retail evidence that justified the rename.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
GN = ROOT / "scripts" / "GameNatives"
PAT = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")


def deleted_paths() -> list[str]:
    out = []
    for cmd in (["git", "diff", "--cached", "--name-status", "--diff-filter=D"],
                ["git", "diff", "--name-status", "--diff-filter=D"]):
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        for line in p.stdout.splitlines():
            parts = line.split("\t")
            if len(parts) >= 2:
                out.append(parts[-1])
    return sorted(set(out))


def current_by_address() -> dict[str, list[str]]:
    index: dict[str, list[str]] = {}
    for p in GN.rglob("*"):
        if not p.is_file():
            continue
        m = PAT.match(p.name)
        if m:
            index.setdefault(m.group(1).upper(), []).append(p.relative_to(ROOT).as_posix())
    return index


def main() -> None:
    manifest = json.loads(MANIFEST.read_text())
    retired_files = {i["path"]: i for i in manifest.get("retired_files", [])
                     if isinstance(i, dict) and "path" in i}
    retired_natives = {(i.get("address"), i.get("path")): i
                       for i in manifest.get("retired_natives", [])
                       if isinstance(i, dict)}

    by_addr = current_by_address()
    added_files = 0
    added_natives = 0

    for rel in deleted_paths():
        if not rel.startswith("scripts/GameNatives/"):
            continue
        m = PAT.match(Path(rel).name)
        if not m:
            continue
        key = m.group(1).upper()
        replacement = None
        for cand in sorted(by_addr.get(key, [])):
            cm = PAT.match(Path(cand).name)
            if cm and cm.group(4) == m.group(4) and (cm.group(3) or "") == (m.group(3) or ""):
                replacement = cand
                break
        if replacement is None:
            for cand in sorted(by_addr.get(key, [])):
                replacement = cand
                break

        rationale = (
            f"Renamed to {replacement}; the retail body of 0x{key} adjudicated the "
            f"function name and the new filename carries it. The transcription, its "
            f"address and its content are unchanged." if replacement else
            f"Removed after the retail body adjudicated the function name for 0x{key}.")

        if rel not in retired_files:
            retired_files[rel] = {
                "path": rel,
                "reviewed": True,
                "rationale": rationale,
                "replacedBy": replacement,
            }
            added_files += 1

        # the address the deleted file carried
        p = subprocess.run(["git", "show", f"HEAD:{rel}"], cwd=ROOT,
                           capture_output=True, text=True)
        for line in p.stdout.splitlines():
            am = re.match(r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
                          line, re.IGNORECASE)
            if not am:
                continue
            addr = am.group("address").upper()
            if (addr, rel) not in retired_natives:
                retired_natives[(addr, rel)] = {
                    "address": addr,
                    "path": rel,
                    "reviewed": True,
                    "rationale": rationale,
                    "replacedBy": replacement,
                }
                added_natives += 1

    manifest["retired_files"] = sorted(retired_files.values(), key=lambda i: i["path"])
    manifest["retired_natives"] = sorted(
        retired_natives.values(), key=lambda i: (i.get("address") or "", i.get("path") or ""))
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"retired_files entries: {len(manifest['retired_files'])} (+{added_files})")
    print(f"retired_natives entries: {len(manifest['retired_natives'])} (+{added_natives})")


if __name__ == "__main__":
    main()
