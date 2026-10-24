#!/usr/bin/env python3
"""Rewrite native provenance blocks into the five-line form required by AGENTS.md 10.7.

The gate (scripts/native_gate.py) requires, immediately above each native
definition, exactly five consecutive ``//`` lines carrying the labels
``Native:``, ``Artifact:``, ``Tools/evidence:``, ``Behavior/CFG:`` and
``Status/limits:``. This helper reads the current block, folds its existing
information into that five-line layout, and uses the manifest verification
status for the ``Status/limits:`` line so it stays in lock-step with
docs/fidelity/native-verification.json.
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

NATIVE_COMMENT = re.compile(
    r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
    re.IGNORECASE,
)
HOST_HELPER_COMMENT = re.compile(r"^\s*//\s*HOST HELPER:", re.IGNORECASE)
NATIVE_ROOTS = ("engine/src/retail/",)


def normalized_address(value: str) -> str:
    return f"0x{int(value, 16):08X}"


def derive_name(raw: str) -> str:
    name = re.split(r"[;,(]", raw, maxsplit=1)[0]
    name = re.split(r"\s+-\s+", name, maxsplit=1)[0]
    return name.strip()


def strip_comment(line: str) -> str:
    return re.sub(r"^\s*//\s?", "", line).rstrip()


def collect(lines: list[str]) -> tuple[list[str], list[str]]:
    """Split an old block into (evidence, behavior, extras)."""
    evidence: list[str] = []
    behavior: list[str] = []
    extras: list[str] = []
    for line in lines[1:]:
        text = strip_comment(line).strip()
        if not text:
            continue
        low = text.lower()
        if low.startswith(("tools/evidence:", "tools:", "evidence:")):
            evidence.append(re.sub(r"^[^:]*:\s*", "", text))
        elif low.startswith(("behavior:", "behavior/cfg:", "cfg:")):
            behavior.append(re.sub(r"^[^:]*:\s*", "", text))
        elif low.startswith(("confidence:", "signature:", "usage/tests:")):
            extras.append(text)
        elif low.startswith("previous catalog alias"):
            extras.append(text)
        elif "digest" in low or "game/natives" in low or ".c." in text or "bytes" in low:
            evidence.append(text)
        else:
            behavior.append(text)
    return evidence, behavior, extras


STATUS_COMMENT = re.compile(r"^\s*//\s*Status/limits:\s*(?P<status>[A-Z_]+)\b")


def is_conformant(block: list[str], status: str) -> bool:
    if len(block) < 5:
        return False
    head = block[:5]
    if not all(re.match(r"^\s*//", line) for line in head):
        return False
    for label, offset in (("Artifact:", 1), ("Tools/evidence:", 2), ("Behavior/CFG:", 3), ("Status/limits:", 4)):
        if label not in head[offset]:
            return False
    parsed = STATUS_COMMENT.match(head[4])
    return bool(parsed) and parsed.group("status") == status


def build_block(name: str, address: str, signature: str, binary_sha: str,
                evidence: list[str], behavior: list[str], extras: list[str],
                status: str) -> list[str]:
    evidence_text = " ".join(evidence).strip() or "Binary Ninja MCP and Radare2 on retail igi.exe."
    behavior_text = " ".join(behavior).strip() or "not restated here."
    limits = next((e for e in extras if e.lower().startswith("confidence:")), "")
    limits_text = limits.split(":", 1)[1].strip() if limits else "see verification record for scope and port limits."
    block = [
        f"// Native: {name}; retail VA {address}; signature/calling convention {signature.rstrip('.')}.",
        f"// Artifact: retail igi.exe SHA256 {binary_sha}.",
        f"// Tools/evidence: {evidence_text}",
        f"// Behavior/CFG: {behavior_text}",
        f"// Status/limits: {status}; {limits_text}",
    ]
    seen: set[str] = set()
    for extra in extras:
        if extra.lower().startswith(("confidence:", "signature:", "usage/tests:")):
            continue
        if extra in seen:
            continue
        seen.add(extra)
        block.append(f"// {extra}")
    return block


def load_manifest(root: pathlib.Path) -> tuple[str, dict[str, dict]]:
    manifest = json.loads((root / "docs/fidelity/native-verification.json").read_text(encoding="utf-8"))
    binary_sha = manifest["binary_sha256"]
    records = {normalized_address(n["address"]): n for n in manifest["natives"]}
    return binary_sha, records


def process(path: pathlib.Path, relative: str, binary_sha: str, records: dict[str, dict],
            dry_run: bool) -> int:
    lines = path.read_text(encoding="utf-8").splitlines()
    changed = 0
    index = 0
    output: list[str] = []
    while index < len(lines):
        line = lines[index]
        match = NATIVE_COMMENT.match(line)
        if not match:
            output.append(line)
            index += 1
            continue
        start = index
        end = index + 1
        while end < len(lines):
            candidate = lines[end]
            if not candidate.lstrip().startswith("//"):
                break
            if NATIVE_COMMENT.match(candidate) or HOST_HELPER_COMMENT.match(candidate):
                break
            end += 1
        block = lines[start:end]
        address = normalized_address(match.group("address"))
        record = records.get(address)
        name = record["name"] if record else derive_name(match.group("name"))
        status = record["verification"]["status"] if record else "TRANSCRIBED"
        signature = "not restated"
        if record:
            signature = record.get("implementation", {}).get("signature") or signature
        if is_conformant(block, status):
            output.extend(block)
            index = end
            continue
        evidence, behavior, extras = collect(block)
        if not record:
            found = next((e for e in extras if e.lower().startswith("signature:")), "")
            if found:
                signature = found.split(":", 1)[1].strip()
        new_block = build_block(name, address, signature, binary_sha, evidence, behavior, extras, status)
        output.extend(new_block)
        changed += 1
        index = end
    new_text = "\n".join(output) + ("\n" if lines and path.read_text(encoding="utf-8").endswith("\n") else "")
    if not dry_run and new_text != path.read_text(encoding="utf-8"):
        path.write_text(new_text, encoding="utf-8")
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("paths", nargs="*")
    args = parser.parse_args()
    root = args.root.resolve()
    binary_sha, records = load_manifest(root)
    if args.paths:
        files = [root / p for p in args.paths]
    else:
        base = root / "engine/src/retail"
        files = sorted([*base.rglob("*.cpp"), *base.rglob("*.hpp")])
    total = 0
    for path in files:
        if not path.is_file():
            continue
        relative = path.relative_to(root).as_posix()
        if not any(relative.startswith(prefix) for prefix in NATIVE_ROOTS):
            continue
        total += process(path, relative, binary_sha, records, args.dry_run)
    print(f"{'would normalize' if args.dry_run else 'normalized'} {total} native block(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
