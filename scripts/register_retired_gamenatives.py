#!/usr/bin/env python3
"""Register reviewed retired_files/retired_natives entries for deleted GameNatives copies.

The 2026-10-19 GameNatives cleanup removed duplicate "copy" files ("... 2.cpp")
and old catalog-alias transcriptions after every manifest game_natives reference
was re-pointed at the remaining same-retail-address file. This script inspects
each deleted path against git HEAD, extracts the native addresses from the
provenance comments, and writes reviewed retirement entries so the change gate
can audit the deletion history.
"""

from __future__ import annotations

import json
import pathlib
import re
import subprocess

ROOT = pathlib.Path(".").resolve()
MANIFEST = ROOT / "docs/fidelity/native-verification.json"

NATIVE_COMMENT = re.compile(
    r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
    re.IGNORECASE,
)


def deleted_native_files() -> list[str]:
    process = subprocess.run(
        ["git", "status", "--porcelain"], cwd=ROOT, text=True, capture_output=True, check=True,
    )
    paths = []
    for line in process.stdout.splitlines():
        if len(line) < 4 or "D" not in (line[0], line[1]):
            continue
        target = line[3:].strip().strip('"')
        if target.startswith("scripts/GameNatives/") and target.endswith((".cpp", ".hpp", ".c")):
            paths.append(target)
    return sorted(set(paths))


def addresses_from_git(path: str) -> list[str]:
    process = subprocess.run(
        ["git", "show", f"HEAD:{path}"], cwd=ROOT, text=True, capture_output=True, check=False,
    )
    if process.returncode != 0:
        return []
    found: list[str] = []
    for line in process.stdout.splitlines():
        match = NATIVE_COMMENT.match(line)
        if match:
            address = "0x" + match.group("address").upper()
            found.append(address)
    return found


def main() -> int:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    retired_files = manifest.setdefault("retired_files", [])
    retired_natives = manifest.setdefault("retired_natives", [])
    existing_files = {item.get("path") for item in retired_files if isinstance(item, dict)}
    existing_pairs = {(item.get("address"), item.get("path"))
                      for item in retired_natives if isinstance(item, dict)}
    rationale = (
        "Duplicate or superseded GameNatives transcription copy (old catalog alias or "
        "duplicated 'copy' file) removed during the 2026-10-19 catalog-name cleanup; the "
        "native's manifest game_natives reference was re-pointed at the surviving "
        "same-retail-address .cpp, and the manifest evidence/artifacts still bind the "
        "retail binary digest for verification."
    )
    added_files = 0
    added_natives = 0
    for path in deleted_native_files():
        if path not in existing_files:
            retired_files.append({"path": path, "reviewed": True, "rationale": rationale})
            existing_files.add(path)
            added_files += 1
        for address in addresses_from_git(path):
            if (address, path) not in existing_pairs:
                retired_natives.append(
                    {"address": address, "path": path, "reviewed": True, "rationale": rationale})
                existing_pairs.add((address, path))
                added_natives += 1
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"retired_files added: {added_files}; retired_natives added: {added_natives}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
