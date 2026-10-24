#!/usr/bin/env python3
"""Independently re-derive radare2 CFG facts for natives in IGINatives_v4.json.

The catalog claims cfgScore 1.0 for every entry. This re-runs radare2 from
scratch so the claim can be checked against an independent tool.
"""

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
IGI = Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def r2(addr: str) -> dict:
    cmd = [
        "r2", "-q", "-e", "bin.relocs.apply=true", "-c",
        f"s {addr}; af; afb; aflc",
        str(IGI),
    ]
    p = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
    text = p.stdout.strip()
    blocks = []
    for line in text.splitlines():
        parts = line.split()
        if parts and parts[0].lower().startswith("0x"):
            blocks.append(parts[0])
    count = None
    for line in reversed(text.splitlines()):
        parts = line.split()
        if len(parts) == 1 and parts[0].isdigit():
            count = int(parts[0])
    return {"addr": addr, "blocks": blocks, "n": len(blocks), "size": count, "raw": text[-300:]}


def main() -> None:
    natives = json.loads(V4.read_text())["Natives"]
    which = sys.argv[1:] or [n["address"] for n in natives]
    out = []
    for n in natives:
        if n["address"] not in which:
            continue
        info = r2(n["address"])
        info["name"] = n["name"]
        info["claimedCfg"] = n.get("cfgScore")
        out.append(info)
        print(f"{info['addr']} {n['name']:<40} r2blocks={info['n']} claimed={info['claimedCfg']}")
    (ROOT / "assets" / "r2_cfg_check.json").write_text(json.dumps(out, indent=2))


if __name__ == "__main__":
    main()
