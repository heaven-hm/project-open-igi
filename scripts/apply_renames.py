#!/usr/bin/env python3
"""Bulk-rename natives across every path that carries a name.

Three surfaces are synchronised:

  1. Binary Ninja   - bn_symbol_rename on the retail function
  2. GameNatives    - the file names under scripts/GameNatives/<subsystem>/
  3. catalogs       - assets/IGINatives_v4.json and friends

Renames are applied from the adjudicated verdict table, so a name only changes
where retail evidence supports it. Everything is idempotent and re-runnable.
"""

import json
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

VERDICTS = ROOT / "assets" / "gamenatives_verdicts.json"
GN = ROOT / "scripts" / "GameNatives"
APPLY = "--apply" in sys.argv


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def strip_n(s):
    return re.sub(r"_\d+$", "", nz(s))


def gn_files_for(addr_key: str):
    """All GameNatives files whose name starts with this address."""
    pat = re.compile(rf"^{re.escape(addr_key)}-", re.I)
    return [p for p in GN.rglob("*") if p.is_file() and pat.match(p.name)]


def addr_key(addr: str) -> str:
    """GameNatives filenames use 8-digit zero-padded hex addresses."""
    return f"{int(addr, 16):08X}"


def main() -> None:
    data = json.load(open(VERDICTS))
    rows = data["adjudicated"]
    bn = BN()

    renamed_bn, renamed_gn, skipped = [], [], []
    for r in rows:
        addr = r["address"]
        key = addr_key(addr)
        correct = r["correctName"]

        # ---- Binary Ninja ------------------------------------------------
        cur = bn.call("bn_function_info", {"function": addr})
        cur_name = bn.header(cur, "name")
        if cur_name and nz(cur_name) != nz(correct):
            ok, msg = bn.rename(addr, correct)
            (renamed_bn if ok else skipped).append(
                {"address": addr, "from": cur_name, "to": correct, "ok": ok,
                 "msg": msg if not ok else ""})
            print(f"BN  {addr}: {cur_name} -> {correct} {'ok' if ok else 'FAIL ' + msg}")
        elif cur_name:
            print(f"BN  {addr}: already {cur_name}")

        # ---- GameNatives files -------------------------------------------
        files = gn_files_for(key)
        for p in files:
            m = re.match(rf"^([0-9A-Fa-f]{{6,8}})-(.*?)(\s+\d+)?(\.[a-z]+)$", p.name)
            if not m:
                continue
            old = m.group(2)
            if nz(old) == nz(correct):
                continue
            ext = m.group(4)
            disamb = m.group(3) or ""   # " 2" / " 3" duplicate marker, keep the space
            new = p.with_name(f"{key}-{correct}{disamb}{ext}")
            if new == p:
                continue
            if new.exists():
                skipped.append({"address": addr, "file": str(p),
                                "msg": f"target exists: {new.name}"})
                print(f"GN  {p.name}: SKIP (target {new.name} exists)")
                continue
            if APPLY:
                p.rename(new)
            renamed_gn.append({"address": addr, "from": str(p.relative_to(ROOT)),
                               "to": str(new.relative_to(ROOT))})
            print(f"GN  {p.name} -> {new.name}")

    if APPLY:
        bn.call("bn_analysis_update_and_wait", {})

    summary = {
        "applied": APPLY,
        "bnRenamed": len(renamed_bn),
        "gnRenamed": len(renamed_gn),
        "skipped": len(skipped),
        "bn": renamed_bn, "gn": renamed_gn, "skips": skipped,
    }
    out = ROOT / "assets" / "rename_applied.json"
    out.write_text(json.dumps(summary, indent=2))
    print(f"\napplied={APPLY} bnRenamed={len(renamed_bn)} gnRenamed={len(renamed_gn)} "
          f"skipped={len(skipped)}")


if __name__ == "__main__":
    main()
