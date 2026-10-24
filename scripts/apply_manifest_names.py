#!/usr/bin/env python3
"""Apply the project manifest's canonical names to Binary Ninja and GameNatives.

The project manifest (docs/fidelity/native-verification.json) already carries a
curated name for every implemented native, backed by a full evidence chain. For
31 of those addresses the Binary Ninja database still held a Ghidra auto-name
that the retail body contradicts.

This aligns the binary, the GameNatives filenames and the manifest on one name
per address. Where retail registers the function under a literal string that
differs from the project alias, the literal is recorded rather than lost.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

GN = ROOT / "scripts" / "GameNatives"
PAT = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
# retail registers these two under a literal string; the project aliases them
RETAIL_REGISTERED = {
    "0x004c6670": "CreateTerrainMaterial",
    "0x004c6760": "CreateTerrainTileMap",
}


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    sur = {int(r["address"], 16): r
           for r in json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}
    mv = json.load(open(ROOT / "docs" / "fidelity" / "native-verification.json"))["natives"]
    bn = BN()

    applied = []
    for n in mv:
        a = int(n["address"], 16)
        cur = sur.get(a, {}).get("bnName")
        if not cur or nz(cur) == nz(n["name"]):
            continue
        final = n["name"]
        ok, msg = bn.rename(n["address"], final)
        print(f"BN  {n['address']}: {cur} -> {final} {'ok' if ok else 'FAIL ' + msg}")

        key = f"{a:08X}"
        for p in list(GN.rglob(f"{key}-*")):
            m = PAT.match(p.name)
            if not m:
                continue
            new = p.with_name(f"{key}-{final}{m.group(3) or ''}{m.group(4)}")
            if new == p or new.exists():
                continue
            text = p.read_text()
            text = re.sub(r"provenance: [A-Za-z0-9_]+ - 0x",
                          f"provenance: {final} - 0x", text, count=1)
            p.write_text(text)
            p.rename(new)
            print(f"GN  {p.name} -> {new.name}")

        applied.append({
            "address": n["address"], "from": cur, "to": final,
            "bnRenameOk": ok,
            "retailRegisteredName": RETAIL_REGISTERED.get(n["address"].lower()),
            "implementation": n.get("implementation", {}).get("path"),
        })

    bn.call("bn_analysis_update_and_wait", {})
    dest = ROOT / "assets" / "manifest_names_applied.json"
    dest.write_text(json.dumps(applied, indent=2))
    print(f"\napplied {len(applied)} ({sum(1 for a in applied if a['bnRenameOk'])} ok) -> {dest}")


if __name__ == "__main__":
    main()
