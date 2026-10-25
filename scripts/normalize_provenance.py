#!/usr/bin/env python3
"""Normalise native provenance blocks to the five consecutive lines the gate parses.

scripts/native_gate.py:check_provenance_comment requires exactly five consecutive `//` lines starting at
`// Native:` and expects `Artifact:` / `Tools/evidence:` / `Behavior/CFG:` / `Status/limits:` at offsets 1-4.
Wrapped block text (a continuation line such as `//   (28 bytes, 3 blocks ...)`) makes the block longer than five
lines, so the gate reports missing labels even though the information is present.

This tool rewrites only the block's line structure: it joins each labelled section back onto one line, keeps the
text verbatim, and leaves everything outside the block untouched. It never invents or drops a sentence, and it
reports every file whose block it could not parse instead of guessing.

Usage:
    python3 scripts/normalize_provenance.py --check          # list non-conforming blocks
    python3 scripts/normalize_provenance.py --write          # rewrite them in place
    python3 scripts/normalize_provenance.py --check engine/src/retail/core/native_00467d90.cpp
"""

from __future__ import annotations

import argparse
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
LABELS = ("Artifact:", "Tools/evidence:", "Behavior/CFG:", "Status/limits:")
NATIVE_LINE = re.compile(r"^\s*//\s*Native:")
STATUS_WORD = re.compile(r"^\s*//\s*Status/limits:\s*([A-Z_]+)\b")


def block_end(lines: list[str], start: int) -> int:
    """Index of the first line after the provenance block, i.e. first non-comment line."""
    index = start
    while index < len(lines) and lines[index].lstrip().startswith("//"):
        index += 1
    return index


def split_sections(lines: list[str]) -> list[str] | None:
    """Return the block's text joined per label, or None when the block is not a five-part block."""
    sections: list[list[str]] = [[lines[0]]]
    for line in lines[1:]:
        stripped = re.sub(r"^\s*//\s?", "", line).rstrip()
        if any(stripped.startswith(label) for label in LABELS):
            sections.append([stripped])
        else:
            if len(sections) == 0:
                return None
            sections[-1].append(stripped)
    if len(sections) != 5:
        return None
    joined = []
    for index, parts in enumerate(sections):
        text = " ".join(part.strip() for part in parts if part.strip())
        if index == 0:
            text = re.sub(r"^//\s*Native:\s*", "Native: ", text)
        joined.append("// " + text)
    return joined


def check_file(path: pathlib.Path) -> tuple[bool, str]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    for index, line in enumerate(lines):
        if not NATIVE_LINE.match(line):
            continue
        end = block_end(lines, index)
        block = lines[index:end]
        if len(block) == 5 and all(label in block[offset] for label, offset in
                                   (("Artifact:", 1), ("Tools/evidence:", 2), ("Behavior/CFG:", 3), ("Status/limits:", 4))):
            return True, "conforming"
        replacement = split_sections(block)
        if replacement is None:
            return False, f"unparsable block at line {index + 1}"
        return False, f"{len(block)} lines -> 5 lines at line {index + 1}"
    return False, "no provenance block"


def squash(text: str) -> str:
    """Every character except the comment markers and whitespace, in order.

    Used to prove a rewrite only changed line structure: joining wrapped lines must not add, drop or reorder a
    single character of the provenance text.
    """
    joined = "\n".join(re.sub(r"^\s*//\s?", "", line) for line in text.splitlines())
    return re.sub(r"\s+", "", joined)


def rewrite_file(path: pathlib.Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    for index, line in enumerate(lines):
        if not NATIVE_LINE.match(line):
            continue
        end = block_end(lines, index)
        block = lines[index:end]
        replacement = split_sections(block)
        if replacement is None:
            return False
        new_lines = lines[:index] + replacement + lines[end:]
        if squash("\n".join(replacement)) != squash("\n".join(block)):
            print(f"REFUSING {path}: rewritten text differs from the original beyond whitespace")
            return False
        path.write_text("\n".join(new_lines) + ("\n" if text.endswith("\n") else ""), encoding="utf-8")
        return True
    return False


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("paths", nargs="*", help="files to inspect (default: every engine/src/retail native)")
    parser.add_argument("--write", action="store_true", help="rewrite non-conforming blocks")
    parser.add_argument("--check", action="store_true", help="only report (default behaviour)")
    args = parser.parse_args()

    if args.paths:
        paths = [pathlib.Path(item) for item in args.paths]
    else:
        paths = sorted(ROOT.glob("engine/src/retail/**/native_*.cpp"))

    bad = 0
    for path in paths:
        ok, detail = check_file(path)
        if ok:
            continue
        bad += 1
        if args.write:
            changed = rewrite_file(path)
            print(f"{'rewritten' if changed else 'FAILED   '} {path.relative_to(ROOT) if path.is_relative_to(ROOT) else path}: {detail}")
        else:
            print(f"{path.relative_to(ROOT) if path.is_relative_to(ROOT) else path}: {detail}")
    print(f"{bad} non-conforming block(s) of {len(paths)} file(s)")
    return 0 if (bad == 0 or args.write) else 1


if __name__ == "__main__":
    sys.exit(main())
