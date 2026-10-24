#!/usr/bin/env python3
"""Extend the confirmed catalog with the IGIPatch entries it does not yet cover.

Six confirmed natives are missing from the catalog, and the retail data symbols
the IGIPatch project identifies are recorded as a separate section because they
are globals rather than functions.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
SCRIPTS_COPY = ROOT / "scripts" / "IGINatives_v4.json"
IGIPATCH = ROOT / "assets" / "igipatch_confirmed_natives.json"
DATA = ROOT / "assets" / "igipatch_retail_data_symbols.json"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

# the IGIPatch entries that are retail globals rather than functions
DATA_SYMBOL_NAMES = {
    "Game_ptGame", "Flow_ptFlow", "Game_iMissionID", "Cursor_nMouseX", "Cursor_nMouseY",
}


def to_sig(name: str, prototype: str) -> str:
    if not prototype:
        return f"void {name}(void);"
    m = re.match(r"^(.*?)\((.*)\)\s*$", prototype, re.S)
    if not m:
        return f"{prototype} {name}(void);"
    ret, args = m.group(1).strip(), m.group(2).strip()
    if args in ("", "void"):
        return f"{ret} {name}(void);"
    parts = []
    for i, a in enumerate(args.split(",")):
        toks = a.strip().split()
        ptr = "".join(t for t in toks if t == "*")
        base = [t for t in toks if t != "*"][0]
        parts.append(f"{base}{ptr} param_{i + 1}")
    return f"{ret} {name}({', '.join(parts)});"


def main() -> None:
    apply = "--apply" in sys.argv
    doc = json.loads(V4.read_text())
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    known = {int(e["address"], 16) for e in doc["Natives"]}

    added = []
    for entry in json.load(open(IGIPATCH))["natives"]:
        addr = int(entry["address"], 16)
        if addr in known or entry["name"] in DATA_SYMBOL_NAMES:
            continue
        m = master.get(addr, {})
        doc["Natives"].append({
            "address": entry["address"],
            "name": entry["name"],
            "signature": to_sig(entry["name"], m.get("bnPrototype", "")),
            "bnPrototype": m.get("bnPrototype", ""),
            "bnName": m.get("name"),
            "callingConvention": "cdecl",
            "namingSource": "IGIPatch confirmed native list",
        })
        added.append(entry["address"])

    doc["Natives"] = sorted(doc["Natives"], key=lambda e: int(e["address"], 16))
    doc["totalVerifiedCount"] = len(doc["Natives"])
    doc["dataSymbols"] = json.load(open(DATA))["symbols"]
    doc["binarySha256"] = RETAIL_SHA256

    print(f"catalog entries: {len(doc['Natives'])} (added {len(added)}: {added})")
    print(f"data symbols recorded: {len(doc['dataSymbols'])}")

    if apply:
        text = json.dumps(doc, indent=2) + "\n"
        V4.write_text(text)
        SCRIPTS_COPY.write_text(text)
        print(f"written {V4}")
        print(f"written {SCRIPTS_COPY}")


if __name__ == "__main__":
    main()
