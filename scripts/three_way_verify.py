#!/usr/bin/env python3
"""Three-way verification of every GameNatives address.

  Binary Ninja  - decompiles the address as an analysed function
  Radare2       - af; afij finds a function at the address
  angr          - CFGFast recovers a function whose entry is the address

An address is CONFIRMED only when all three agree it is a function entry.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
IGI = "/Users/haseeb-mir/Downloads/IGI/igi.exe"
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
CHUNK = 400


def r2_batch(addrs):
    """Return {addr_lower: {name,size,nbbs}} for addresses r2 resolves to a function."""
    found = {}
    for i in range(0, len(addrs), CHUNK):
        chunk = addrs[i:i + CHUNK]
        # radare2's `af` at one address can create a function that swallows the
        # next address, so the function table is cleared before each analysis.
        script = ";".join(f"s {a};af-*;af;afij" for a in chunk)
        p = subprocess.run(
            ["r2", "-q", "-e", "bin.relocs.apply=true", "-c", script, IGI],
            capture_output=True, text=True, timeout=1800)
        # radare2 prints one afij array per seek on its own line; parse them
        # per line so a bracket inside a function name cannot break the scan.
        for ln in p.stdout.splitlines():
            ln = ln.strip()
            if not (ln.startswith("[") and ln.endswith("]")):
                continue
            try:
                data = json.loads(ln)
            except json.JSONDecodeError:
                continue
            if not data:
                continue
            d = data[0]
            found[f"0x{d['addr']:08x}"] = {"name": d.get("name"),
                                           "size": d.get("size"),
                                           "nbbs": d.get("nbbs")}
        print(f"  r2 {min(i + CHUNK, len(addrs))}/{len(addrs)} "
              f"resolved={len(found)}", flush=True)
    return found


def main() -> None:
    rows = json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))
    angr = json.load(open(ROOT / "assets" / "gamenatives_angr.json"))["addresses"]

    addrs = [r["address"] for r in rows]
    r2 = r2_batch(addrs)

    out = []
    confirmed = 0
    for r in rows:
        a = r["address"]
        key = f"0x{int(a, 16):08X}"
        bn_start = (r["bnStart"] or "").strip().strip('"')
        bn_ok = bool(bn_start) and bn_start.lower() == a.lower()
        ag = angr.get(key, {})
        angr_ok = bool(ag.get("is_function_entry"))
        ri = r2.get(f"0x{int(a, 16):08x}")
        r2_ok = ri is not None
        agree = bn_ok and angr_ok and r2_ok
        if agree:
            confirmed += 1
        out.append({
            "address": a, "gnNames": r["gnNames"], "bnName": r["bnName"],
            "bnEntry": bn_ok, "angrEntry": angr_ok, "r2Entry": r2_ok,
            "r2Name": (ri or {}).get("name"),
            "angrBlocks": ag.get("block_count"), "bnBlocks": r["blocks"],
            "confirmed": agree,
        })

    dest = ROOT / "assets" / "gamenatives_three_way.json"
    dest.write_text(json.dumps({
        "binary": IGI, "binary_sha256": RETAIL_SHA256,
        "methods": ["binaryninja", "radare2", "angr"],
        "checked": len(out), "confirmed": confirmed,
        "entries": out,
    }, indent=2))

    bad = [o for o in out if not o["confirmed"]]
    print(f"\nchecked   : {len(out)}")
    print(f"confirmed : {confirmed}  (all three agree on function entry)")
    print(f"not confirmed: {len(bad)}")
    for o in bad[:25]:
        print(f"   {o['address']} bn={o['bnEntry']} angr={o['angrEntry']} r2={o['r2Entry']} "
              f"gn={o['gnNames'][0]}")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
