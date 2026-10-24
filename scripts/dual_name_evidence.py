#!/usr/bin/env python3
"""Adjudicate the 44 GameNatives addresses that carry two conflicting filenames.

For these addresses one filename is a Ghidra auto-generated name (recognisable by
its trailing disambiguator counter) and the other is a hand-assigned retail
name. Binary Ninja currently carries whichever name was seeded into it, so the
two sources must be compared against the retail body before anything is renamed.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def strip_counter(s):
    return re.sub(r"_\d+$", "", s)


def main() -> None:
    index = json.load(open(ROOT / "assets" / "gamenatives_index.json"))
    survey = {int(r["address"], 16): r
              for r in json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}
    bn = BN()

    rows = []
    for key, names in sorted(index.items()):
        if len(names) < 2:
            continue
        row = survey.get(int(key, 16))
        if not row:
            continue
        addr = row["address"]
        text = bn.call("bn_function_decompile", {"function": addr})
        m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
        body = m.group(1) if m else ""
        strings = re.findall(r'"([^"]{3,})"', body)[:8]
        rows.append({
            "address": addr,
            "names": list(names),
            "bnName": row["bnName"],
            "strings": strings,
            "body": body[:1200],
        })

    dest = ROOT / "assets" / "dual_name_evidence.json"
    dest.write_text(json.dumps(rows, indent=2))
    print(f"wrote {dest}  entries={len(rows)}\n")
    for r in rows:
        print("=" * 78)
        print(r["address"], " bn=", r["bnName"])
        for n in r["names"]:
            tag = "  (auto)" if re.search(r"_\d+$", n) else ""
            mark = "*" if nz(n) == nz(r["bnName"]) else " "
            print(f"  {mark} {n}{tag}")
        if r["strings"]:
            print("   strings:", r["strings"][:5])


if __name__ == "__main__":
    main()
