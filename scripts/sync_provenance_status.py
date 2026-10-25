#!/usr/bin/env python3
"""Make each linked native's `Status/limits:` line agree with its manifest status.

The gate compares the status token in a native's five-line provenance block with the status recorded for that address
in docs/fidelity/native-verification.json. Label drift is a real defect: a block that still says TRANSCRIBED while the
manifest says BUILD_VERIFIED (or the reverse) is two contradicting claims about the same function.

This tool copies the manifest status into the comment only for files that are actually in the build target, because
BUILD_VERIFIED means "the object is part of igi1.exe" and nothing else. Files not in the target are left alone and
reported, so an unlinked transcription cannot acquire a linked status by accident.

Usage:
    python3 scripts/sync_provenance_status.py --check
    python3 scripts/sync_provenance_status.py --write
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
CMAKE = ROOT / "CMakeLists.txt"
VA = re.compile(r"retail VA (0x[0-9A-Fa-f]{8})")
STATUS = re.compile(r"^(\s*//\s*Status/limits:\s*)([A-Z_]+)(.*)$")


def linked_sources() -> set[str]:
    text = CMAKE.read_text(encoding="utf-8")
    return {match.group(0).strip() for match in re.finditer(r"engine/src/retail/[^\s\)]+\.cpp", text)}


def manifest_status() -> dict[str, str]:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    return {str(record["address"]).lower(): (record.get("verification") or {}).get("status", "")
            for record in manifest["natives"]}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()

    linked = linked_sources()
    statuses = manifest_status()
    changed = skipped = 0
    for path in sorted(ROOT.glob("engine/src/retail/**/native_*.cpp")):
        relative = path.relative_to(ROOT).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        lines = text.splitlines()
        header = next((line for line in lines if "retail VA" in line and line.lstrip().startswith("//")), None)
        if header is None:
            continue
        match = VA.search(header)
        if not match:
            continue
        address = match.group(1).lower()
        wanted = statuses.get(address)
        if not wanted:
            continue
        if relative not in linked:
            if wanted == "BUILD_VERIFIED":
                skipped += 1
                print(f"SKIP {relative}: manifest says BUILD_VERIFIED but the file is not in the build target")
            continue
        for index, line in enumerate(lines):
            found = STATUS.match(line)
            if found is None:
                continue
            if found.group(2) == wanted:
                break
            new_line = f"{found.group(1)}{wanted}{found.group(3)}"
            lines[index] = new_line
            changed += 1
            print(f"{'would sync' if not args.write else 'synced'} {relative}: {found.group(2)} -> {wanted}")
            if args.write:
                path.write_text("\n".join(lines) + ("\n" if text.endswith("\n") else ""), encoding="utf-8")
            break
    print(f"{changed} status line(s) {'synced' if args.write else 'to sync'}, {skipped} skipped as unlinked")
    return 0


if __name__ == "__main__":
    sys.exit(main())
