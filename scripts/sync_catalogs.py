#!/usr/bin/env python3
"""Synchronise the native catalogs with the verified retail evidence.

Writes:
  assets/IGINatives_v4.json              - corrected names and full signatures
  assets/IGINatives_All_GameNatives.json - master catalog over every GameNatives
                                          address with its retail signature and
                                          the three-tool verification result

Names come from the adjudicated verdicts; signatures come from Binary Ninja's
analysis of the retail function.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
SURVEY = ROOT / "assets" / "gamenatives_bn_survey.json"
THREE = ROOT / "assets" / "gamenatives_three_way.json"
VERDICTS = ROOT / "assets" / "naming_verdicts.json"
ANGR = ROOT / "assets" / "gamenatives_angr.json"
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

# v4 names that survived adjudication: the retail body supports the v4 label
# where Binary Ninja and the manifest carried a Ghidra auto-name.
V4_KEEPS = {
    "0x004109A0": "Human_RunUpdate",       # 'Delta move pos X/Y/Z' movement debug strings
    "0x00460C80": "Human_printf",          # printf-style console log gated on the active player
    "0x00490370": "Timer_Read",            # returns GetTickCount() - g_tick_base
    "0x004C1800": "QTaskList_New",         # allocates the {count,capacity,items} QTaskList layout
}


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def to_sig(name: str, bn_proto: str) -> str:
    """Build a C signature from a name and a Binary Ninja prototype definition."""
    if not bn_proto:
        return f"void {name}(void);"
    m = re.match(r"^(.*?)\((.*)\)\s*$", bn_proto, re.S)
    if not m:
        return f"{bn_proto} {name}(void);"
    ret, args = m.group(1).strip(), m.group(2).strip()
    if args in ("", "void"):
        return f"{ret} {name}(void);"
    parts = []
    for i, a in enumerate(args.split(",")):
        a = a.strip()
        toks = a.split()
        ptr = "".join(t for t in toks if t == "*")
        base = [t for t in toks if t != "*"][0]
        parts.append(f"{base}{ptr} param_{i + 1}")
    return f"{ret} {name}({', '.join(parts)});"


def main() -> None:
    survey = {int(r["address"], 16): r for r in json.load(open(SURVEY))}
    three = {int(e["address"], 16): e for e in json.load(open(THREE))["entries"]}
    angr = json.load(open(ANGR))["addresses"]
    verdicts = {v["address"].lower(): v for v in json.load(open(VERDICTS))}

    # ---------------- v4 ------------------------------------------------
    doc = json.loads(V4.read_text())
    final = {v["address"].lower(): v
             for v in json.load(open(ROOT / "assets" / "v4_final_verdicts.json"))}
    changed = []
    for entry in doc["Natives"]:
        addr = entry["address"]
        row = survey.get(int(addr, 16))
        old_name = entry["name"]
        new_name = old_name

        fv = final.get(addr.lower())
        # The catalog's own name is the authority: the Binary Ninja database is
        # being brought in line with it, so it must not push its older
        # decompiler labels back into the catalog.
        if entry.get("name"):
            new_name = entry["name"]
        elif fv:
            new_name = fv["finalName"]
        elif row and row.get("bnName"):
            new_name = row["bnName"]

        if row and row["bnName"] and row["bnPrototype"]:
            entry["signature"] = to_sig(new_name, row["bnPrototype"])
            entry["bnPrototype"] = row["bnPrototype"]
        entry["name"] = new_name
        entry["bnName"] = row["bnName"] if row else None
        if fv:
            entry["namingEvidence"] = fv["evidence"]
            entry["namingWinner"] = fv["winner"]
        if new_name != old_name:
            changed.append({"address": addr, "from": old_name, "to": new_name})

    doc["Natives"] = sorted(doc["Natives"], key=lambda e: int(e["address"], 16))
    doc["totalVerifiedCount"] = len(doc["Natives"])
    doc["binarySha256"] = RETAIL_SHA256
    V4.write_text(json.dumps(doc, indent=2) + "\n")
    print(f"v4: {len(doc['Natives'])} natives, {len(changed)} names corrected")

    # ---------------- master catalog -------------------------------------
    # The canonical name wins over the decompiler label, so the master catalog
    # carries the same name the confirmed catalog and the manifest carry.
    canonical = {int(k, 16): v["name"] for k, v in
                 ((e["address"], e) for e in doc["Natives"])}
    manifest_names = {int(str(r["address"]), 16): r.get("name")
                      for r in json.loads(
                          (ROOT / "docs" / "fidelity" / "native-verification.json").read_text())["natives"]}
    master = []
    for addr_int, row in sorted(survey.items()):
        addr = f"0x{addr_int:08X}"
        t = three.get(addr_int, {})
        a = angr.get(f"0x{addr_int:08X}", {})
        gn = row["gnNames"]
        name = (canonical.get(addr_int) or manifest_names.get(addr_int)
                or row["bnName"])
        master.append({
            "address": addr,
            "name": name,
            "signature": to_sig(name, row["bnPrototype"]),
            "bnPrototype": row["bnPrototype"],
            "bnName": row["bnName"],
            "callingConvention": "cdecl",
            "blocks": row["blocks"],
            "callers": row["callers"],
            "callees": row["callees"],
            "bnFunctionStart": bool(row["bnStart"]),
            "angrEntry": bool(a.get("is_function_entry")),
            "r2Entry": bool(t.get("r2Entry")),
            "threeWayConfirmed": bool(t.get("confirmed")),
            "gameNativesNames": gn,
        })

    dest = ROOT / "assets" / "IGINatives_All_GameNatives.json"
    dest.write_text(json.dumps({
        "schemaVersion": 1,
        "binary": "IGI.EXE (Retail v1.0 Europe)",
        "binarySha256": RETAIL_SHA256,
        "imageBase": "0x00400000",
        "totalNatives": len(master),
        "verification": {
            "methods": ["binaryninja-mcp", "radare2", "angr-cfgfast"],
            "threeWayConfirmed": sum(1 for m in master if m["threeWayConfirmed"]),
            "note": "threeWayConfirmed requires all three tools to agree the "
                    "address is a function entry; CFG/block agreement is "
                    "structural evidence only and does not prove behavioural "
                    "equivalence",
        },
        "Natives": master,
    }, indent=2) + "\n")
    print(f"master: {len(master)} natives -> {dest}")


if __name__ == "__main__":
    main()
