#!/usr/bin/env python3
"""Report which retail routines in an address range can be transcribed right now.

A routine is transcribable when every direct call target it makes already has a
transcribed implementation (a provenance block naming that retail VA). The tool
only reads the retail image through radare2 and the engine sources for landed
VAs; it never writes to the tree, never changes a manifest, and never promotes a
status.

Usage:
    python3 scripts/linkable_frontier.py --start 0x4AA000 --end 0x4B1000
    python3 scripts/linkable_frontier.py --start 0x4F1000 --end 0x4F1B00 --max-missing 3
"""

from __future__ import annotations

import argparse
import pathlib
import re
import subprocess
import sys

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
DEFAULT_IMAGE = pathlib.Path.home() / "Downloads" / "IGI" / "igi.exe"
R2 = pathlib.Path("/opt/homebrew/bin/r2")

INSTR_RE = re.compile(r"\s*[\|`.:@]*\s*0x([0-9a-f]{8})\s+([0-9a-f]{2,})\s+(.*)")
CALL_RE = re.compile(r"call (?:fcn\.|sym\.|0x)([0-9a-f]{8}|[A-Za-z_.?$][\w.?$@]*)")
VA_RE = re.compile(r"VA 0x([0-9A-Fa-f]{8})")
DIRECT_CALL_RE = re.compile(r"call (?:fcn\.)?0x([0-9a-f]{8})")


def landed_addresses(root: pathlib.Path) -> set[int]:
    landed: set[int] = set()
    for path in (root / "engine" / "src").rglob("*.cpp"):
        for match in VA_RE.finditer(path.read_text(errors="ignore")):
            landed.add(int(match.group(1), 16))
    return landed


def disassemble(image: pathlib.Path, start: int, end: int) -> list[tuple[int, str]]:
    count = max(64, int((end - start) / 2) + 64)
    out = subprocess.run(
        [str(R2), "-q", "-e", "scr.color=0", "-c", f"pd {count} @ {hex(start)}", "-c", "q", str(image)],
        capture_output=True,
        text=True,
        check=True,
    ).stdout.splitlines()
    instructions: list[tuple[int, str]] = []
    for line in out:
        match = INSTR_RE.match(line)
        if not match:
            continue
        address = int(match.group(1), 16)
        if address >= end:
            continue
        instructions.append((address, match.group(3).strip()))
    return instructions


def split_functions(instructions: list[tuple[int, str]]) -> list[list[tuple[int, str]]]:
    """Split on 'ret' followed by alignment nops, the boundary style this image uses.

    A trailing run without padding is kept as one body; if padding never appears
    the split simply does not happen, which is reported as an unsplit tail rather
    than guessed at.
    """
    functions: list[list[tuple[int, str]]] = []
    current: list[tuple[int, str]] = []
    for index, entry in enumerate(instructions):
        current.append(entry)
        if not entry[1].startswith("ret"):
            continue
        lookahead = index + 1
        while lookahead < len(instructions) and instructions[lookahead][1].startswith("nop"):
            lookahead += 1
        if lookahead > index + 1:
            functions.append(current)
            current = []
    if current:
        functions.append(current)
    return functions


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--start", required=True)
    parser.add_argument("--end", required=True)
    parser.add_argument("--image", type=pathlib.Path, default=DEFAULT_IMAGE)
    parser.add_argument("--root", type=pathlib.Path, default=REPO_ROOT)
    parser.add_argument("--max-missing", type=int, default=0, help="print routines with at most this many missing callees")
    parser.add_argument("--limit", type=int, default=40)
    parser.add_argument("--show-missing", action="store_true")
    args = parser.parse_args()

    start = int(args.start, 16)
    end = int(args.end, 16)
    if end <= start:
        parser.error("--end must be greater than --start")
    landed = landed_addresses(args.root)
    instructions = disassemble(args.image, start, end)
    functions = split_functions(instructions)

    rows = []
    for body in functions:
        calls = sorted({int(m.group(1), 16) for _, asm in body for m in [DIRECT_CALL_RE.search(asm)] if m})
        missing = [call for call in calls if call not in landed]
        rows.append((len(missing), body[0][0], len(body), body[-1][0], missing))
    rows.sort()

    print(f"landed VAs known: {len(landed)}")
    print(f"instructions scanned: {len(instructions)} in {len(functions)} bodies (0x{start:x}-0x{end:x})")
    shown = 0
    for missing_count, first, count, last, missing in rows:
        if missing_count > args.max_missing:
            continue
        tail = ""
        if args.show_missing and missing:
            tail = " missing=" + ",".join(hex(value) for value in missing)
        print(f"0x{first:08x}-0x{last:08x} instrs={count} missing={missing_count}{tail}")
        shown += 1
        if shown >= args.limit:
            break
    if shown == 0:
        print("no routine in range meets the missing-callee limit")
    return 0


if __name__ == "__main__":
    sys.exit(main())
