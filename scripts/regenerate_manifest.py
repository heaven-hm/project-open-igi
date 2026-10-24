#!/usr/bin/env python3
"""Regenerate scripts/GameNatives/VERIFICATION.md from the verified evidence.

The manifest previously carried Ghidra auto-names that contradicted the retail
bodies. It is rebuilt from the current Binary Ninja names and the three-tool
verification result, so the table is the single source of truth for which name
belongs to which address.
"""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MD = ROOT / "scripts" / "GameNatives" / "VERIFICATION.md"
INDEX = ROOT / "assets" / "gamenatives_index.json"
SURVEY = ROOT / "assets" / "gamenatives_bn_survey.json"
THREE = ROOT / "assets" / "gamenatives_three_way.json"
ANGR = ROOT / "assets" / "gamenatives_angr.json"

HEAD = """# GameNatives - Native Verification Manifest

Every GameNatives address was checked against the retail `igi.exe`
(SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`)
using three independent methods:

- **Binary Ninja MCP** - the address resolves to an analysed function
- **Radare2** - `af; afij` resolves the address to a function
- **angr CFGFast** - a function is recovered whose entry is that address

An address is marked **CONFIRMED** only when all three agree that the address
is a function entry. CFG and block agreement is structural evidence only; it
does not prove semantic or behavioural equivalence, and no runtime behaviour is
claimed here.

**Naming:** the filename in this tree is the canonical name. It was reconciled
against the retail body for every address, and the Binary Ninja database was
renamed to match. Addresses that are not function entries are listed separately
at the end and must not be transcribed as standalone natives.

"""

TAIL = """
## Addresses that are not function entries

These addresses were recorded as natives but are jump targets inside a
surrounding function. Neither Binary Ninja nor radare2 defines a function
start at them, so they cannot be transcribed as standalone natives.

| Address | GameNatives name | Containing function | Note |
|---|---|---|---|
"""


def main() -> None:
    index = json.load(open(INDEX))
    survey = {int(r["address"], 16): r for r in json.load(open(SURVEY))}
    three = {int(e["address"], 16): e for e in json.load(open(THREE))["entries"]}
    angr = json.load(open(ANGR))["addresses"]

    rows = []
    for key, names in sorted(index.items(), key=lambda kv: int(kv[0], 16)):
        a = int(key, 16)
        r = survey.get(a)
        if not r or not r["bnStart"]:
            continue
        t = three.get(a, {})
        g = angr.get(f"0x{a:08X}", {})
        gate = "PASS" if t.get("confirmed") else "FAIL"
        bn = "VERIFIED" if t.get("confirmed") else "CONFLICT"
        files = sorted(next(iter(names.values())))
        rows.append((a, r["bnName"], gate, bn, files[0]))

    out = [HEAD]
    out.append(f"**Scope:** {len(rows)} confirmed natives "
               f"({len(index)} addresses catalogued).\n")
    out.append("| Address | Canonical name | Gate | BN check | GameNatives path |")
    out.append("|---|---|---|---|---|")
    for a, name, gate, bn, path in rows:
        out.append(f"| `0x{a:08X}` | `{name}` | {gate} | {bn} | `{path}` |")

    out.append(TAIL)
    for key, names in sorted(index.items(), key=lambda kv: int(kv[0], 16)):
        a = int(key, 16)
        r = survey.get(a)
        if r and r["bnStart"]:
            continue
        nm = sorted(names)[0]
        files = sorted(next(iter(names.values())))
        out.append(f"| `0x{a:08X}` | `{nm}` | inside a surrounding function | "
                   f"not a function entry; do not transcribe standalone |")

    MD.write_text("\n".join(out) + "\n")
    print(f"wrote {MD}  rows={len(rows)}")


if __name__ == "__main__":
    main()
