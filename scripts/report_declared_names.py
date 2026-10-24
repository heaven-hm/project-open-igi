#!/usr/bin/env python3
"""Report GameNatives files whose declared function name is not the canonical one.

Renaming a GameNatives file is only half the job: the declaration inside it
still names the old function, so the translation unit defines the wrong symbol.
This reports every file whose declared name disagrees with the canonical name
for that address.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GN = ROOT / "scripts" / "GameNatives"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
DECL = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
BANNED = {"if", "for", "while", "switch", "return", "sizeof", "catch", "defined"}


def main() -> None:
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    # the canonical names in use, so an old name is only rewritten when it is
    # not itself the canonical name of some other address
    canonical = {n["name"] for n in master.values()}

    rows = []
    for path in sorted(GN.rglob("*")):
        if not path.is_file() or path.suffix not in (".cpp", ".hpp"):
            continue
        m = FILENAME.match(path.name)
        if not m:
            continue
        addr = int(m.group(1), 16)
        entry = master.get(addr)
        if not entry:
            continue
        want = entry["name"]
        file_name = m.group(2)
        text = path.read_text(encoding="utf-8", errors="replace")
        names = {d.group("name") for d in DECL.finditer(text)
                 if d.group("name") not in BANNED}
        # the file's own subject is the function it declares
        declared = {n for n in names if n == file_name or n == want}
        stale = {n for n in names
                 if n not in canonical and n != want and n in text}
        if want not in names:
            rows.append({"path": str(path.relative_to(ROOT)), "address": f"0x{addr:08X}",
                         "canonical": want, "fileNameSubject": file_name,
                         "declared": sorted(names), "status": "WRONG_NAME",
                         "suspects": sorted(stale)})
        elif file_name != want:
            rows.append({"path": str(path.relative_to(ROOT)), "address": f"0x{addr:08X}",
                         "canonical": want, "fileNameSubject": file_name,
                         "declared": sorted(names), "status": "FILENAME_NAME_MISMATCH",
                         "suspects": sorted(stale)})

    dest = ROOT / "docs" / "fidelity" / "evidence" / "gamenatives-declared-names.json"
    dest.write_text(json.dumps({
        "checkedFiles": len(list(GN.rglob("*.cpp"))) + len(list(GN.rglob("*.hpp"))),
        "issues": rows,
    }, indent=2) + "\n")
    wrong = [r for r in rows if r["status"] == "WRONG_NAME"]
    print(f"files with a declaration that is not the canonical name: {len(wrong)}")
    print(f"files whose filename subject differs from the canonical name: "
          f"{len(rows) - len(wrong)}")
    for r in wrong[:15]:
        print(f"  {r['address']} {str(r['canonical']):<32} file={r['fileNameSubject']:<32} "
              f"declares={r['declared'][:3]}")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
