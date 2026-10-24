#!/usr/bin/env python3
"""Dump retail bodies for the native addresses whose v4 label still differs."""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    v4 = json.load(open(ROOT / "assets" / "IGINatives_v4.json"))["Natives"]
    sur = {int(r["address"], 16): r
           for r in json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}
    bn = BN()
    rows = []
    for e in v4:
        r = sur.get(int(e["address"], 16))
        if not r or nz(r["bnName"]) == nz(e["name"]):
            continue
        text = bn.call("bn_function_decompile", {"function": e["address"]})
        m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
        body = m.group(1) if m else ""
        rows.append({
            "address": e["address"], "v4Name": e["name"], "bnName": r["bnName"],
            "strings": re.findall(r'"([^"]{3,})"', body)[:8],
            "body": re.sub(r"\n\s*\n", "\n", body)[:1500],
        })
    dest = ROOT / "assets" / "v4_divergence_bodies.json"
    dest.write_text(json.dumps(rows, indent=2))
    print(f"wrote {dest} entries={len(rows)}\n")
    for r in rows:
        print("=" * 88)
        print(f"{r['address']}  v4={r['v4Name']}   bn={r['bnName']}")
        if r["strings"]:
            print("   strings:", r["strings"][:5])
        print(r["body"][:700])


if __name__ == "__main__":
    main()
