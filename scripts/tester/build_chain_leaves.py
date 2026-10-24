#!/usr/bin/env python3
"""Inventory the direct callees of 0x5183D0 (74) and 0x508FA0 (43) and classify leaves.

Reads the two BN callee tables already captured under /tmp, asks BN for each unique
callee's name/size/range and callees, and marks a target as a leaf when it calls no
retail native that lacks an engine implementation. Saves one raw JSON per address so
the leaf packets can be captured address by address without repeating MCP work.
"""
from __future__ import annotations

import json
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build/tester/20261024T005700Z/registrar-leaves"
OUT.mkdir(parents=True, exist_ok=True)
(OUT / "bn").mkdir(exist_ok=True)

HELPER = ROOT / "scripts/bn_mcp_call.py"
ROW = re.compile(r"\|\s*0x[0-9a-fA-F]+\s*\|\s*x86\s*\|\s*(0x[0-9a-fA-F]+)\s*\|\s*(.*?)\s*\|")


def mcp(tool: str, payload: dict) -> str:
    proc = subprocess.run([sys.executable, str(HELPER), tool, json.dumps(payload)],
                          capture_output=True, text=True, cwd=ROOT)
    if proc.returncode != 0:
        raise RuntimeError(f"{tool} {payload} failed: {proc.stderr[-500:]}")
    return proc.stdout


def parse_callees(text: str) -> list[dict]:
    rows = []
    for line in text.splitlines():
        m = ROW.search(line)
        if m:
            rows.append(dict(address=int(m.group(1), 16), name=m.group(2).split(" (")[0]))
    return rows


def parse_info(text: str) -> dict:
    name = None; size = None; start = None
    for line in text.splitlines():
        if "| Name |" in line:
            name = line.split("|")[2].strip()
        if "| Start |" in line:
            start = line.split("|")[2].strip()
        m = re.search(r"Ranges \| 0x([0-9a-fA-F]+)-0x([0-9a-fA-F]+) \((\d+) bytes\)", line)
        if m:
            size = int(m.group(3))
    return dict(name=name, start=start, size=size)


def main() -> int:
    manifest = json.loads((ROOT / "docs/fidelity/native-verification.json").read_text())
    implemented = {int(n["address"], 16): n["name"] for n in manifest["natives"]
                   if n.get("implementation")}
    targets = {}
    for parent, path in (("0x5183d0", pathlib.Path("/tmp/callees_5183d0.json")),
                         ("0x508fa0", pathlib.Path("/tmp/callees_508fa0.json"))):
        rows = parse_callees(path.read_text())
        targets.setdefault(parent, rows)
    all_targets = sorted({row["address"] for rows in targets.values() for row in rows})
    uniq = [a for a in all_targets if 0x401000 <= a < 0x600000]
    external = [hex(a) for a in all_targets if not (0x401000 <= a < 0x600000)]
    inventory = dict(parents={k: [r["address"] for r in v] for k, v in targets.items()},
                     unique=[hex(a) for a in uniq], external=external, records={})
    for address in uniq:
        key = hex(address).lower().replace("0x", "0x")
        info_path = OUT / "bn" / f"{key}-info.txt"
        callees_path = OUT / "bn" / f"{key}-callees.txt"
        if not info_path.exists():
            info_path.write_text(mcp("bn_function_info", {"function": key}))
        if not callees_path.exists():
            callees_path.write_text(mcp("bn_function_callees", {"function": key}))
        info = parse_info(info_path.read_text())
        callees = parse_callees(callees_path.read_text())
        unresolved = [c for c in callees if c["address"] not in implemented
                      and 0x401000 <= c["address"] < 0x600000]
        record = dict(address=key, name=info["name"], size=info["size"],
                      callees=callees, unresolved_callees=unresolved,
                      implemented=implemented.get(address), leaf=len(unresolved) == 0,
                      parent=[k for k in targets if any(r["address"] == address for r in targets[k])])
        inventory["records"][key] = record
        (OUT / "bn" / f"{key}-record.json").write_text(json.dumps(record, indent=2) + "\n")
        print(f"{key} size={info['size']} name={info['name']} callees={len(callees)} "
              f"unresolved={len(unresolved)} leaf={record['leaf']} impl={bool(record['implemented'])}")
    (OUT / "inventory.json").write_text(json.dumps(inventory, indent=2) + "\n")
    leaves = {k: v for k, v in inventory["records"].items() if v["leaf"]}
    (OUT / "leaf-plan.json").write_text(json.dumps(dict(leaves=list(leaves)), indent=2) + "\n")
    print(f"unique={len(uniq)} leaves={len(leaves)} "
          f"already_implemented={sum(1 for v in inventory['records'].values() if v['implemented'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
