#!/usr/bin/env python3
"""Find Binary Ninja functions whose name is a decompiler artefact.

A decompiler artefact is an auto-generated label: a family name plus a
disambiguating counter (Ghidra's "Family_Action_17"), or a generic "sub_" name.
Those labels sit on real retail functions and hide what they are.

This scans every function in the active view and reports the ones that an
authoritative source - the IGIPatch list, the native name evidence file or the
confirmed catalogs - names differently.
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
CATALOGS = (REF / "IGINatives_v4.json", REF / "IGINatives_100_Percent_Clean.json")

# Ghidra's auto-generated shape: Family_Action_17 / Family_17 / sub_401000
ARTEFACT = re.compile(r"^(?:sub_[0-9a-fA-F]+|[A-Z][A-Za-z0-9]*_[A-Za-z0-9_]*_\d+|"
                      r"[A-Z][A-Za-z0-9]*_\d+)$")
# a name the decompiler took from an unrelated string
STRINGY = re.compile(r"^(?:Sys|Camera|FX|MathSys|Display|Input|Resource|SoundSys|"
                     r"Human|HumanPlayer|HumanView|Game|Level|Config|QTask|"
                     r"Flow|Texture|Terrain|DynObj|Mem|Weapon|Gun|Script|Render|"
                     r"Anim|Model|Object|AI|Cursor|Mouse|ListBox|Break)[A-Za-z0-9]*_\d+$")


def nz(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", str(value).lower())


def load_sources():
    sources: dict[int, list[dict]] = {}

    def note(address: int, name: str, origin: str):
        entry = sources.setdefault(address, [])
        if name and all(nz(item["name"]) != nz(name) for item in entry):
            entry.append({"name": name, "origin": origin})

    for line in IGIPATCH.read_text().splitlines():
        m = re.match(r"^\s*\d+\s+(\S+)\s+([0-9A-Fa-f]{6,8})\s*$", line)
        if m:
            note(int(m.group(2), 16), m.group(1), "igipatch-human")

    for item in json.loads(EVIDENCE.read_text())["Natives"]:
        name = item.get("canonicalName") or item.get("catalogName")
        if name:
            note(int(item["address"]), name, f"evidence:{item.get('source')}")

    for path in CATALOGS:
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

    # every function in the active view
    functions = []
    offset = 0
    while True:
        text = bn.call("bn_function_list", {"offset": offset})
        if not text:
            break
        rows = [line for line in text.splitlines() if line.startswith("| 0x")]
        for line in rows:
            cells = [cell.strip() for cell in line.strip("|").split("|")]
            if len(cells) < 3:
                continue
            address, name = cells[0], cells[2]
            if not address.lower().startswith("0x"):
                continue
            functions.append((int(address, 16), name.strip('"')))
        if not rows:
            break
        offset += len(rows)
        if len(rows) < 100:
            break

    print(f"functions in the view: {len(functions)}")

    artefacts = []
    for address, name in functions:
        if not (ARTEFACT.match(name) or STRINGY.match(name)):
            continue
        candidates = sources.get(address, [])
        authoritative = [c for c in candidates if nz(c["name"]) != nz(name)]
        if not authoritative:
            continue
        # prefer the strongest source
        order = {"igipatch-human": 0, "evidence:retail-string": 1,
                 "evidence:retail-string-context": 1, "catalog:": 3}
        authoritative.sort(key=lambda c: order.get(c["origin"], 9))
        artefacts.append({
            "address": f"0x{address:08X}",
            "current": name,
            "renameTo": authoritative[0]["name"],
            "origin": authoritative[0]["origin"],
            "alternatives": [c["name"] for c in authoritative[1:]],
        })

    dest = ROOT / "assets" / "bn_name_artefacts.json"
    dest.write_text(json.dumps(artefacts, indent=2) + "\n")
    print(f"decompiler-artefact names with an authoritative name: {len(artefacts)}")
    for item in artefacts[:40]:
        print(f"  {item['address']}  {item['current']:<40} -> {item['renameTo']:<38} "
              f"[{item['origin']}]")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
