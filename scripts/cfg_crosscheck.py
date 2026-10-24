#!/usr/bin/env python3
"""Compare radare2 and Binary Ninja basic-block counts per native.

CFG score = 1 - |bn_blocks - r2_blocks| / max(bn_blocks, r2_blocks).
A score of 1.0 means both tools independently agree on the block count.
"""

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
CALLER = ROOT / "scripts" / "bn_mcp_call.py"


def call_text(tool: str, args: dict) -> str:
    p = subprocess.run(
        [sys.executable, str(CALLER), tool, json.dumps(args)],
        capture_output=True, text=True, timeout=300,
    )
    return p.stdout


def bn_blocks(addr: str):
    text = call_text("bn_function_basic_blocks", {"function": addr})
    for ln in text.splitlines():
        if ln.startswith("count:"):
            return int(ln.split(":", 1)[1].strip())
    return None


def r2_blocks(addr: str):
    p = subprocess.run(
        ["r2", "-q", "-c", f"s {addr}; af; afb", "/Users/haseeb-mir/Downloads/IGI/igi.exe"],
        capture_output=True, text=True, timeout=120,
    )
    return sum(1 for ln in p.stdout.splitlines() if ln.split() and ln.split()[0].lower().startswith("0x"))


def main() -> None:
    natives = json.loads(V4.read_text())["Natives"]
    rows = []
    for n in natives:
        addr, name = n["address"], n["name"]
        b, r = bn_blocks(addr), r2_blocks(addr)
        if b is None or r is None:
            continue
        mx = max(b, r) or 1
        score = round(1.0 - abs(b - r) / mx, 4)
        rows.append({"address": addr, "name": name, "bnBlocks": b, "r2Blocks": r,
                     "cfgScore": score, "claimed": n.get("cfgScore")})
        print(f"{addr} {name:<38} bn={b:<4} r2={r:<4} score={score:<7} claimed={n.get('cfgScore')}")

    out = ROOT / "assets" / "cfg_crosscheck.json"
    out.write_text(json.dumps(rows, indent=2))
    agree = [r for r in rows if r["bnBlocks"] == r["r2Blocks"]]
    gate = [r for r in rows if r["cfgScore"] >= 0.85]
    print(f"\nchecked={len(rows)} exactBlockMatch={len(agree)} gate>=0.85={len(gate)}")
    overclaim = [r for r in rows if r["cfgScore"] < r["claimed"]]
    print(f"entries where measured < claimed: {len(overclaim)}")
    for r in overclaim[:15]:
        print(f"   {r['address']} {r['name']:<36} measured={r['cfgScore']} claimed={r['claimed']}")


if __name__ == "__main__":
    main()
