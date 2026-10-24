#!/usr/bin/env python3
"""Compare the Binary Ninja names against the authoritative name sources.

The authoritative sources are:
  * the IGIPatch native list, verified by a human,
  * the native name evidence file, which records where each name came from,
  * the confirmed catalogs, which carry the project's canonical naming.

Only a name that the decompiler invented is a candidate for renaming; a name
that already matches an authoritative source is left alone.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

REF = Path("/Users/haseeb-mir/Documents/Code/project-igi-natives/assets")
EVIDENCE = REF / "IGI1-Native-Name-Evidence.json"
IGIPATCH = REF / "IGIPathNatives.txt"
CATALOG_V4 = REF / "IGINatives_v4.json"
CATALOG_CLEAN = REF / "IGINatives_100_Percent_Clean.json"


def nz(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", str(value).lower())


def load_sources():
    sources: dict[int, dict] = {}

    def note(address: int, name: str, origin: str, evidence=None):
        entry = sources.setdefault(address, {"candidates": [], "origins": set()})
        if name and all(nz(n["name"]) != nz(name) for n in entry["candidates"]):
            entry["candidates"].append({"name": name, "origin": origin})
        entry["origins"].add(origin)

    for line in IGIPATCH.read_text().splitlines():
        m = re.match(r"^\s*\d+\s+(\S+)\s+([0-9A-Fa-f]{6,8})\s*$", line)
        if m:
            note(int(m.group(2), 16), m.group(1), "igipatch-human")

    evidence = json.loads(EVIDENCE.read_text())
    for item in evidence["Natives"]:
        name = item.get("canonicalName") or item.get("catalogName")
        if not name:
            continue
        note(int(item["address"]), name, f"evidence:{item.get('source')}",
             item.get("retailEvidence") or item.get("analysisEvidence"))

    for path in (CATALOG_V4, CATALOG_CLEAN):
        for item in json.loads(path.read_text())["Natives"]:
            if not item.get("name"):
                continue
            raw = item["address"]
            address = int(raw, 16) if isinstance(raw, str) else int(raw)
            note(address, item["name"], f"catalog:{path.name}")

    return sources


def main() -> None:
    sources = load_sources()
    bn = BN()

    rows = []
    for address in sorted(sources):
        text = bn.call("bn_function_info", {"function": f"0x{address:08X}"})
        current = bn.header(text, "name")
        if not current:
            continue
        entry = sources[address]
        chosen = None
        for candidate in entry["candidates"]:
            if nz(candidate["name"]) == nz(current):
                continue
            chosen = candidate
            break
        rows.append({
            "address": f"0x{address:08X}",
            "current": current,
            "candidates": entry["candidates"],
            "origins": sorted(entry["origins"]),
            "renameTo": chosen["name"] if chosen else None,
        })

    dest = ROOT / "assets" / "bn_name_audit.json"
    dest.write_text(json.dumps(rows, indent=2) + "\n")
    renamable = [r for r in rows if r["renameTo"]]
    print(f"addresses with an authoritative name: {len(rows)}")
    print(f"already matching an authoritative name: {len(rows) - len(renamable)}")
    print(f"candidates for renaming: {len(renamable)}")
    for r in renamable[:40]:
        print(f"  {r['address']}  {r['current']:<40} -> {r['renameTo']:<40} "
              f"[{','.join(r['origins'][:2])}]")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
