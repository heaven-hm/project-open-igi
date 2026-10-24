#!/usr/bin/env python3
"""Classify every naming divergence in IGINatives_v4.json against Binary Ninja.

Semantic divergences are not cosmetic. Before renaming in Binary Ninja we need to
know whether the catalog name or the Binary Ninja name is supported by evidence in
the binary itself (error strings, called functions, return values).
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SURVEY = ROOT / "assets" / "bn_survey_v4.json"
V4 = ROOT / "assets" / "IGINatives_v4.json"
CALLER = ROOT / "scripts" / "bn_mcp_call.py"


def norm(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def call_text(tool, args, timeout=300):
    p = subprocess.run([sys.executable, str(CALLER), tool, json.dumps(args)],
                       capture_output=True, text=True, timeout=timeout)
    return p.stdout


def decompile(addr):
    text = call_text("bn_function_decompile", {"function": addr})
    m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
    return m.group(1) if m else text


def callees(addr):
    text = call_text("bn_function_callees", {"function": addr})
    return re.findall(r"\| (0x[0-9a-f]+) \(x86\)", text)


def main():
    rows = json.load(open(SURVEY))
    v4 = {n["address"]: n for n in json.load(open(V4))["Natives"]}
    out = []
    for r in rows:
        if r["nameMatch"] or norm(r["v4Name"]) == norm(r["bnName"]):
            continue
        kind = "cosmetic" if norm(r["v4Name"]) == norm(r["bnName"]) else "semantic"
        entry = {"address": r["address"], "v4Name": r["v4Name"],
                 "bnName": r["bnName"], "kind": kind}
        if kind == "semantic":
            body = decompile(r["address"])
            entry["decompile"] = body[:1400]
            entry["callees"] = [c for c in callees(r["address"])]
            entry["evidenceStrings"] = re.findall(r'"([^"]{3,})"', body)[:6]
        out.append(entry)
        print(f"{r['address']} {kind:<9} v4={r['v4Name']:<36} bn={r['bnName']}")
    (ROOT / "assets" / "naming_conflicts.json").write_text(json.dumps(out, indent=2))
    sem = sum(1 for e in out if e["kind"] == "semantic")
    print(f"\ncosmetic={len(out)-sem} semantic={sem} total={len(out)}")


if __name__ == "__main__":
    main()
