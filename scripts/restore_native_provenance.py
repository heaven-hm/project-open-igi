#!/usr/bin/env python3
"""Restore the five-line `// Native:` / `// HOST HELPER:` provenance the pinned gate requires.

The merged tree carries several informal marker styles instead of the strict
`// Native: <name> ... VA 0x<addr>` block (e.g. `// 0x0048F1A0  App_ContextSetDebugged`
or `// Model_GetAttachment() - 0x004D0CF0 - ...`). This tool converts a marker that
names a retail address into the gate's five-line form, and adds an explicit
HOST HELPER block to a file that has no address-qualified entry at all.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent

MARKER_ADDR_NAME = re.compile(r"^\s*//\s*0x([0-9A-Fa-f]{8})\s+([A-Za-z_]\w*)")
MARKER_NAME_ADDR = re.compile(r"^\s*//\s*(?:Original IGI\.EXE\s+)?([A-Za-z_]\w*)\s*\(?\s*\)?\s*[-–]\s*0x([0-9A-Fa-f]{8})")
MARKER_ADDR_ONLY = re.compile(r"^\s*//\s*0x([0-9A-Fa-f]{8})\s*$")
MARKER_NAME_AT_ADDR = re.compile(r"\b([A-Za-z_]\w*)\s+(?:at\s+)?0x([0-9A-Fa-f]{8})\b")
FUNCTION_DEF = re.compile(r"^(?:[A-Za-z_][\w:<>,\s*&]*\s)?\b([A-Za-z_]\w*)\s*\(")
HOST_HELPER_MARK = re.compile(r"^\s*//\s*HOST HELPER:", re.IGNORECASE)


def sha256(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1 << 20), b""):
            hasher.update(block)
    return hasher.hexdigest()


def block(name: str, address: str, binary_sha: str, note: str) -> list[str]:
    return [
        f"// Native: {name}; retail VA {address}; signature/calling convention not restated.",
        f"// Artifact: retail igi.exe SHA256 {binary_sha}.",
        f"// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.",
        f"// Behavior/CFG: {note or 'restored provenance; see verification record.'}",
        "// Status/limits: TRANSCRIBED; see verification record for scope and port limits.",
    ]


def host_helper_block(binary_sha: str) -> list[str]:
    return [
        "// HOST HELPER: host-side module for the open-igi port; no retail parity is claimed.",
        f"// Artifact: retail igi.exe SHA256 {binary_sha} (constants referenced where noted).",
        "// Tools/evidence: host port module; no address-qualified retail entry is assigned here.",
        "// Behavior/CFG: host implementation; retail control flow is not asserted for this file.",
        "// Status/limits: HOST HELPER; excluded from native parity claims.",
    ]


def game_native_addresses() -> set[str]:
    addresses = set()
    for path in (ROOT / "scripts/GameNatives").rglob("*.cpp"):
        match = re.match(r"(?i)^([0-9A-Fa-f]{8})-", path.name)
        if match:
            addresses.add(f"0x{match.group(1).upper()}")
    return addresses


def next_function_name(lines: list[str], start: int, window: int = 40) -> str | None:
    for index in range(start + 1, min(start + 1 + window, len(lines))):
        match = FUNCTION_DEF.match(lines[index])
        if match:
            return match.group(1)
    return None


def process(path: pathlib.Path, relative: str, binary_sha: str, game_natives: set[str],
            dry_run: bool) -> int:
    lines = path.read_text(encoding="utf-8").splitlines()
    if any(HOST_HELPER_MARK.match(line) or line.strip().startswith("// Native:") for line in lines):
        return 0
    edits: list[tuple[int, int, list[str]]] = []
    for index, line in enumerate(lines):
        m1 = MARKER_ADDR_NAME.match(line)
        m2 = None if m1 else MARKER_NAME_ADDR.match(line)
        m3 = None if (m1 or m2) else MARKER_ADDR_ONLY.match(line)
        if m1:
            address, _ = m1.group(1).upper(), m1.group(2)
        elif m2:
            address = m2.group(2).upper()
        elif m3:
            address = m3.group(1).upper()
        else:
            continue
        if f"0x{address}" not in game_natives:
            continue
        name = next_function_name(lines, index)
        if name is None:
            continue
        edits.append((index, index + 1, block(name, f"0x{address}", binary_sha, line.lstrip("/ ").strip())))
    output: list[str] = []
    if not edits:
        output = list(lines)
        insert_at = 0
        for index, line in enumerate(lines):
            if line.startswith("#include"):
                insert_at = index + 1
        output[insert_at:insert_at] = [""] + host_helper_block(binary_sha)
        if not dry_run:
            path.write_text("\n".join(output) + "\n", encoding="utf-8")
        return 1
    cursor = 0
    for start, end, new_lines in edits:
        output.extend(lines[cursor:start])
        output.extend(new_lines)
        cursor = end
    output.extend(lines[cursor:])
    if not dry_run:
        path.write_text("\n".join(output) + "\n", encoding="utf-8")
    return len(edits)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("paths", nargs="+")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    manifest = json.loads((ROOT / "docs/fidelity/native-verification.json").read_text(encoding="utf-8"))
    binary_sha = manifest["binary_sha256"]
    game_natives = game_native_addresses()
    total = 0
    for entry in args.paths:
        path = (ROOT / entry) if not pathlib.Path(entry).is_absolute() else pathlib.Path(entry)
        if not path.is_file():
            continue
        count = process(path, path.relative_to(ROOT).as_posix(), binary_sha, game_natives, args.dry_run)
        total += count
        print(f"{path.relative_to(ROOT).as_posix()}: {count} provenance block(s)")
    print(f"{'would restore' if args.dry_run else 'restored'} {total} block(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
