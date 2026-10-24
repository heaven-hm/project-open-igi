#!/usr/bin/env python3
"""Extract the verified native table from scripts/GameNatives/VERIFICATION.md.

The manifest records address -> canonical name as verified against Binary Ninja.
This is an independent naming source alongside Binary Ninja itself and the
GameNatives filenames, so disagreements between the three are detectable.
"""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD = ROOT / "scripts" / "GameNatives" / "VERIFICATION.md"
ROW = re.compile(
    r"^\|\s*`(0x[0-9A-Fa-f]{6,8})`\s*\|\s*`([^`]+)`\s*\|\s*([A-Z-]+)\s*\|\s*([A-Z-]+)\s*\|")


def main() -> None:
    rows = []
    for ln in MD.read_text().splitlines():
        m = ROW.match(ln.strip())
        if m:
            rows.append({
                "address": m.group(1).upper(),
                "canonical": m.group(2),
                "gate": m.group(3),
                "bnCheck": m.group(4),
            })
    dest = ROOT / "assets" / "gamenatives_manifest_names.json"
    dest.write_text(json.dumps(rows, indent=2))
    print(f"rows parsed: {len(rows)}")
    gates = {}
    for r in rows:
        gates[r["gate"]] = gates.get(r["gate"], 0) + 1
    print("gate:", gates)
    print(f"written {dest}")


if __name__ == "__main__":
    main()
