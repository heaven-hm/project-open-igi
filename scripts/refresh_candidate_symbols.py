#!/usr/bin/env python3
"""Refresh the candidate symbol map from the freshly built candidate.

The map records the address each native has in the Windows x86 candidate, and
the paired Frida probes hook the candidate at that address. A rebuilt candidate
moves the symbols, so the map has to be refreshed before a paired comparison or
the probe watches an address the candidate never calls.
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import subprocess
import sys
from verification_scope import validate_native_symbol_binding

ROOT = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_MAP = ROOT / "assets" / "candidate_symbol_map.json"
DEFAULT_CANDIDATE = ROOT / "build-win32-no-verify-gate/igi1.exe"


def candidate_symbols(candidate: pathlib.Path) -> dict[str, int]:
    """The candidate's exported symbols, keyed by their mangled name."""
    process = subprocess.run(
        ["i686-w64-mingw32-nm", "--defined-only", str(candidate)],
        capture_output=True, text=True, check=True, timeout=120,
    )
    symbols: dict[str, int] = {}
    for line in process.stdout.splitlines():
        match = re.match(r"^([0-9a-fA-F]+)\s+[A-Za-z]\s+(\S+)$", line.strip())
        if match:
            symbols[match.group(2)] = int(match.group(1), 16)
    return symbols


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--map", type=pathlib.Path, default=DEFAULT_MAP)
    parser.add_argument("--candidate", type=pathlib.Path, default=DEFAULT_CANDIDATE)
    parser.add_argument("--report", action="store_true", help="list every change")
    args = parser.parse_args()

    if not args.candidate.is_file():
        raise SystemExit(f"candidate not found: {args.candidate}")
    symbols = candidate_symbols(args.candidate)
    data = json.loads(args.map.read_text())
    entries = data if isinstance(data, list) else data.get("natives", [])
    changed = 0
    for entry in entries:
        validate_native_symbol_binding(entry)
        candidate_symbol = entry.get("candidateSymbol")
        if not isinstance(candidate_symbol, dict):
            continue
        name = candidate_symbol.get("symbol")
        if not name or name not in symbols:
            continue
        address = f"0x{symbols[name]:08X}"
        if candidate_symbol.get("vaddr") != address:
            if args.report:
                print(f"{entry.get('address')} {name}: {candidate_symbol.get('vaddr')} -> {address}")
            candidate_symbol["vaddr"] = address
            changed += 1
    args.map.write_text(json.dumps(data, indent=2) + "\n")
    print(f"{changed} candidate address(es) refreshed in {args.map}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
