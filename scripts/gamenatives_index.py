#!/usr/bin/env python3
"""Index scripts/GameNatives by retail address.

GameNatives files are named <ADDRESS>-<Name>.<ext>, so the filename is an
independent record of which address each transcription claims to implement.
"""

import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GN = ROOT / "scripts" / "GameNatives"
PAT = re.compile(r"^([0-9A-Fa-f]{6,8})-(.+?)(\s+\d+)?\.(cpp|hpp|h|cc|c)$", re.I)


def main() -> None:
    index = defaultdict(lambda: defaultdict(list))
    for p in GN.rglob("*"):
        if not p.is_file():
            continue
        m = PAT.match(p.name)
        if not m:
            continue
        addr = m.group(1).upper().lstrip("0") or "0"
        name = m.group(2)
        index[addr][name].append(str(p.relative_to(ROOT)))

    out = {a: {n: sorted(v) for n, v in names.items()} for a, names in sorted(index.items())}
    dest = ROOT / "assets" / "gamenatives_index.json"
    dest.write_text(json.dumps(out, indent=2))
    print(f"distinct addresses: {len(out)}")
    print(f"written: {dest}")

    multi = {a: n for a, n in out.items() if len(n) > 1}
    print(f"addresses with >1 distinct name: {len(multi)}")
    for a, names in list(multi.items())[:15]:
        print(f"  0x{a}: {list(names)}")


if __name__ == "__main__":
    main()
