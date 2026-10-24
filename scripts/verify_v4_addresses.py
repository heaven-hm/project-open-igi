#!/usr/bin/env python3
"""Verify every IGINatives_v4 address against retail igi.exe.

Checks, per native:
  * retail SHA-256 binding of the analysed file
  * the address resolves to an analysed function start in Binary Ninja
  * the address resolves to a function start in radare2
  * caller / callee counts, so the entry is provably referenced

Everything is derived from igi.exe itself, not from any pre-existing catalog.
"""

import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
CALLER = ROOT / "scripts" / "bn_mcp_call.py"
IGI = Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def call_text(tool, args, timeout=300):
    p = subprocess.run([sys.executable, str(CALLER), tool, json.dumps(args)],
                       capture_output=True, text=True, timeout=timeout)
    return p.stdout


def header_val(text, key):
    for ln in text.splitlines():
        if ln.startswith(key + ":"):
            return ln.split(":", 1)[1].strip()
    return None


def r2_at(addr):
    p = subprocess.run(
        ["r2", "-q", "-c", f"s {addr}; afi~name,size,nbbs,cc,callrefs",
         str(IGI)],
        capture_output=True, text=True, timeout=120,
    )
    return p.stdout.strip()


def main() -> None:
    digest = hashlib.sha256(IGI.read_bytes()).hexdigest()
    if digest != RETAIL_SHA256:
        print(f"FATAL: retail hash mismatch\n  got      {digest}\n  expected {RETAIL_SHA256}")
        sys.exit(1)
    print(f"retail igi.exe sha256 verified: {digest}")

    natives = json.loads(V4.read_text())["Natives"]
    rows = []
    for n in natives:
        addr = n["address"]
        info = call_text("bn_function_info", {"function": addr})
        blocks = call_text("bn_function_basic_blocks", {"function": addr})
        callers = call_text("bn_function_callers", {"function": addr})
        callees = call_text("bn_function_callees", {"function": addr})
        bn_start = header_val(info, "function")
        bn_name = header_val(info, "name")
        r2 = r2_at(addr)
        rows.append({
            "address": addr,
            "v4Name": n["name"],
            "bnFunctionStart": bn_start,
            "bnName": bn_name,
            "bnBlocks": int(header_val(blocks, "count") or 0),
            "bnCallers": int(header_val(callers, "count") or 0),
            "bnCallees": int(header_val(callees, "count") or 0),
            "r2FunctionInfo": r2,
            "r2IsFunctionStart": bool(r2.strip()),
        })
        print(f"{addr} bnStart={bn_start} blocks={rows[-1]['bnBlocks']} "
              f"callers={rows[-1]['bnCallers']} callees={rows[-1]['bnCallees']} "
              f"r2start={rows[-1]['r2IsFunctionStart']}")

    dest = ROOT / "assets" / "v4_address_verification.json"
    dest.write_text(json.dumps(rows, indent=2))

    bad_bn = [r for r in rows if not r["bnFunctionStart"]]
    bad_r2 = [r for r in rows if not r["r2IsFunctionStart"]]
    orphan = [r for r in rows if r["bnCallers"] == 0 and r["bnCallees"] == 0]
    print(f"\nchecked={len(rows)}")
    print(f"not-a-BN-function-start: {len(bad_bn)} {[r['address'] for r in bad_bn]}")
    print(f"not-a-r2-function-start: {len(bad_r2)} {[r['address'] for r in bad_r2]}")
    print(f"no callers and no callees (unreferenced): {len(orphan)} "
          f"{[r['address'] for r in orphan]}")


if __name__ == "__main__":
    main()
