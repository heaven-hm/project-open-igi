#!/usr/bin/env python3
"""Survey every scripts/GameNatives address against Binary Ninja in one pass.

Produces a full map: retail address -> GameNatives name(s) -> Binary Ninja
name/prototype -> callers/callees. This is the input for bulk renaming and for
the divergence audit; it is derived from igi.exe, not from any catalog.
"""

import json
import re
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

GN_INDEX = ROOT / "assets" / "gamenatives_index.json"
OUT = ROOT / "assets" / "gamenatives_bn_survey.json"


def norm(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    gn = json.load(open(GN_INDEX))
    bn = BN()
    rows = []
    t0 = time.time()
    for i, (addr, names) in enumerate(sorted(gn.items()), 1):
        full = "0x" + addr
        info = bn.info(full)
        rows.append({
            "address": full,
            "key": addr,
            "gnNames": list(names),
            "gnFiles": [f for v in names.values() for f in v],
            "bnName": info["name"],
            "bnStart": info["start"],
            "bnPrototype": info["prototype"],
            "blocks": info["blocks"],
            "callers": info["callers"],
            "callees": info["callees"],
        })
        if i % 500 == 0:
            print(f"  {i}/{len(gn)}  {time.time()-t0:.0f}s", flush=True)

    OUT.write_text(json.dumps(rows, indent=2))
    total = len(rows)
    resolved = [r for r in rows if r["bnStart"]]
    match = [r for r in resolved if any(norm(g) == norm(r["bnName"]) for g in r["gnNames"])]
    div = [r for r in resolved if not any(norm(g) == norm(r["bnName"]) for g in r["gnNames"])]
    multi = [r for r in resolved if len(r["gnNames"]) > 1]
    orphan = [r for r in resolved if r["callers"] == 0 and r["callees"] == 0]

    print(f"\nsurveyed            : {total}")
    print(f"BN function start   : {len(resolved)}")
    print(f"not a BN function   : {total-len(resolved)} "
          f"{[r['address'] for r in rows if not r['bnStart']][:20]}")
    print(f"name matches BN     : {len(match)}")
    print(f"name diverges       : {len(div)}")
    print(f"GameNatives has >1 name: {len(multi)}")
    print(f"no callers/callees  : {len(orphan)}")
    print(f"elapsed             : {time.time()-t0:.0f}s")
    print(f"written             : {OUT}")


if __name__ == "__main__":
    main()
