#!/usr/bin/env python3
"""Dump the retail bodies for the natives whose canonical name differs from the
project manifest, so the two naming sources can be adjudicated against the
binary rather than against each other."""

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
    sur = {int(r["address"], 16): r
           for r in json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}
    mv = json.load(open(ROOT / "docs" / "fidelity" / "native-verification.json"))["natives"]
    bn = BN()

    rows = []
    for n in mv:
        a = int(n["address"], 16)
        cur = sur.get(a, {}).get("bnName")
        if not cur or nz(cur) == nz(n["name"]):
            continue
        text = bn.call("bn_function_decompile", {"function": n["address"]})
        m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
        body = m.group(1) if m else ""
        rows.append({
            "address": n["address"],
            "manifestName": n["name"],
            "canonicalName": cur,
            "implementation": n.get("implementation", {}).get("path"),
            "gameNatives": n.get("game_natives", {}).get("path"),
            "strings": re.findall(r'"([^"]{3,})"', body)[:8],
            "body": re.sub(r"\n\s*\n", "\n", body)[:1200],
        })
    dest = ROOT / "assets" / "manifest_name_bodies.json"
    dest.write_text(json.dumps(rows, indent=2))
    print(f"wrote {dest} entries={len(rows)}\n")
    for r in rows:
        print("=" * 88)
        print(f"{r['address']}  manifest={r['manifestName']}   canonical={r['canonicalName']}")
        print(f"   impl: {r['implementation']}")
        if r["strings"]:
            print(f"   strings: {r['strings'][:5]}")
        print(r["body"][:430])


if __name__ == "__main__":
    main()
