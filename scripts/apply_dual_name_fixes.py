#!/usr/bin/env python3
"""Apply the 44 adjudicated dual-name renames across Binary Ninja and GameNatives.

For every address that carried two conflicting filenames, the retail body decided
which name is correct. The curated GameNatives name won in all 44 cases; the
Ghidra auto-name carried by Binary Ninja and by VERIFICATION.md was wrong.

This renames the Binary Ninja symbol and the stale " 2" GameNatives file, and
repairs the #include inside that file so it points at the real header.
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


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    dual = json.load(open(ROOT / "assets" / "dual_name_evidence.json"))
    index = json.load(open(ROOT / "assets" / "gamenatives_index.json"))
    bn = BN()

    applied = []
    for e in dual:
        addr = e["address"]
        bn_name = e["bnName"]
        curated = [n for n in e["names"] if nz(n) != nz(bn_name)]
        if len(curated) != 1:
            print(f"SKIP {addr}: ambiguous candidates {e['names']}")
            continue
        correct = curated[0]

        # ---- Binary Ninja ------------------------------------------------
        ok, msg = bn.rename(addr, correct)
        print(f"BN  {addr}: {bn_name} -> {correct} {'ok' if ok else 'FAIL ' + msg}")

        # ---- GameNatives stale duplicate file ------------------------------
        key = f"{int(addr, 16):08X}"
        idx_key = f"{int(addr, 16):X}"
        stale_files = [ROOT / p for p in index.get(idx_key, {}).get(bn_name, [])]
        for p in stale_files:
            if not p.exists():
                continue
            m = PAT.match(p.name)
            if not m:
                continue
            disamb = m.group(3) or ""
            ext = m.group(4)
            new = p.with_name(f"{key}-{correct}{disamb}{ext}")
            if new == p:
                continue
            if new.exists():
                print(f"GN  {p.name}: SKIP (target exists)")
                continue
            text = p.read_text()
            primary = [q for q in index.get(idx_key, {}).get(correct, [])
                       if q.endswith(".hpp")]
            if primary:
                want = Path(primary[0]).name
                text = re.sub(r'#include\s+"[^"]*"', f'#include "{want}"', text, count=1)
            text = re.sub(r"provenance: [A-Za-z0-9_]+ - 0x",
                          f"provenance: {correct} - 0x", text, count=1)
            p.write_text(text)
            p.rename(new)
            print(f"GN  {p.name} -> {new.name}")

        applied.append({"address": addr, "bnFrom": bn_name, "bnTo": correct,
                        "bnRenameOk": ok})

    bn.call("bn_analysis_update_and_wait", {})
    dest = ROOT / "assets" / "dual_name_applied.json"
    dest.write_text(json.dumps(applied, indent=2))
    ok = sum(1 for a in applied if a["bnRenameOk"])
    print(f"\napplied {len(applied)} ({ok} BN renames ok) -> {dest}")


if __name__ == "__main__":
    main()
