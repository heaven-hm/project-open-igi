#!/usr/bin/env python3
"""Fix stale GameNatives filenames that disagree with their own contents.

44 addresses carry two filenames for the same retail function. In each case the
duplicate (" 2") file holds the same transcription body as the primary file but
kept a stale filename and a stale #include, so the name on disk contradicts the
name inside the file and the name in Binary Ninja.

This renames the stale file to the Binary Ninja name, repairs the #include to
point at the real header, and refreshes the provenance comment.
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
    index = json.load(open(ROOT / "assets" / "gamenatives_index.json"))
    survey = {int(r["address"], 16): r
              for r in json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}
    bn = BN()

    fixed = []
    for key, names in sorted(index.items()):
        if len(names) < 2:
            continue
        row = survey.get(int(key, 16))
        if not row or not row["bnStart"]:
            continue
        addr = row["address"]
        correct = row["bnName"]
        # The primary file already carries the correct name; the stale one does not.
        stale = [n for n in names if nz(n) != nz(correct)]
        if not stale:
            continue
        stale_name = stale[0]
        files = [GN / p for p in index[key][stale_name]]
        for p in files:
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
                print(f"SKIP {p.name}: {new.name} exists")
                continue
            text = p.read_text()
            # repair the include to point at the real header next to the primary file
            primary = [q for q in index[key].get(correct, [])
                       if q.endswith(".hpp")]
            if primary:
                want = Path(primary[0]).name
                text = re.sub(r'#include\s+"[^"]*"', f'#include "{want}"', text, count=1)
            # refresh the provenance comment to the correct name
            text = re.sub(r"provenance: [A-Za-z0-9_]+ - 0x",
                          f"provenance: {correct} - 0x", text, count=1)
            p.write_text(text)
            p.rename(new)
            fixed.append({"address": addr, "from": p.name, "to": new.name,
                          "correctName": correct})
            print(f"{p.name} -> {new.name}")

    dest = ROOT / "assets" / "stale_name_fixes.json"
    dest.write_text(json.dumps(fixed, indent=2))
    print(f"\nfixed {len(fixed)} stale files -> {dest}")


if __name__ == "__main__":
    main()
