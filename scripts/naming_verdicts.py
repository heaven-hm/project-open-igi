#!/usr/bin/env python3
"""Correlate naming conflicts across v4, Binary Ninja and GameNatives.

For every address whose v4 name disagrees with Binary Ninja, report whether
scripts/GameNatives independently carries the v4 name, the BN name, a third
name, or nothing. Retail error strings inside the function are the tie-breaker.
"""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def norm(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    conflicts = json.load(open(ROOT / "assets" / "naming_conflicts.json"))
    gn = json.load(open(ROOT / "assets" / "gamenatives_index.json"))

    rows = []
    for c in conflicts:
        a = c["address"].upper().lstrip("0X").lstrip("0")
        gnames = list(gn.get(a, {}).keys())
        v4n, bnn = c["v4Name"], c["bnName"]
        if any(norm(g) == norm(v4n) for g in gnames):
            verdict = "v4_supported"
        elif any(norm(g) == norm(bnn) for g in gnames):
            verdict = "bn_supported"
        elif gnames:
            verdict = "both_unconfirmed"
        else:
            verdict = "no_gamenatives"
        rows.append({**c, "gamenatives": gnames, "verdict": verdict})

    (ROOT / "assets" / "naming_verdicts.json").write_text(json.dumps(rows, indent=2))
    counts = {}
    for r in rows:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
    print(counts)
    for r in rows:
        print(f"\n{r['address']}  [{r['verdict']}]")
        print(f"   v4 : {r['v4Name']}")
        print(f"   bn : {r['bnName']}")
        if r["gamenatives"]:
            print(f"   gn : {r['gamenatives']}")
        if r.get("evidenceStrings"):
            print(f"   str: {r['evidenceStrings']}")


if __name__ == "__main__":
    main()
