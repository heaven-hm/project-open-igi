#!/usr/bin/env python3
"""Rewrite GameNatives declarations to the canonical native name.

Renaming a GameNatives file leaves the declaration inside it naming the old
function, so the translation unit would define the wrong symbol and would
include a header that no longer exists. This rewrites, for each address:

  * the function definition / declaration line,
  * the #include of its own header,
  * the provenance comment and the address header comment.

The old subject is taken from the file's own provenance comment, which records
the function the file transcribes. When that old name is also the canonical
name of a different address, only the definition, include and comments are
rewritten, so a genuine call to the other native is left alone.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GN = ROOT / "scripts" / "GameNatives"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
PROVENANCE = re.compile(r"^\s*//\s*IGI native provenance:\s*(?P<name>[A-Za-z_]\w*)\s*-\s*0x(?P<addr>[0-9A-Fa-f]{8})")
HEADER_COMMENT = re.compile(r"^/\* (?P<addr>[0-9A-Fa-f]{8}) (?P<name>[A-Za-z_]\w*) \|")
DEFINITION = re.compile(r"^(?P<head>[^/\n]*?\b)(?P<name>[A-Za-z_]\w*)\s*\(", re.MULTILINE)


def main() -> None:
    apply = "--apply" in sys.argv
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    canonical = {n["name"] for n in master.values() if n.get("name")}
    # the current header file for each address
    headers: dict[int, list[Path]] = {}
    for path in GN.rglob("*.hpp"):
        m = FILENAME.match(path.name)
        if m:
            headers.setdefault(int(m.group(1), 16), []).append(path)

    fixed, skipped = [], []
    for path in sorted(GN.rglob("*")):
        if not path.is_file() or path.suffix not in (".cpp", ".hpp"):
            continue
        m = FILENAME.match(path.name)
        if not m:
            continue
        addr = int(m.group(1), 16)
        entry = master.get(addr)
        want = entry.get("name") if entry else None
        if not want:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")

        prov = PROVENANCE.search(text)
        # the stale subject is whichever of the provenance name, the declared
        # name and the filename subject is not the canonical name
        declared = DEFINITION.search(text)
        candidates = []
        if prov:
            candidates.append(prov.group("name"))
        if declared:
            candidates.append(declared.group("name"))
        candidates.append(m.group(2))
        subject = next((c for c in candidates if c != want), None)
        if subject is None or subject == want:
            continue

        shared = subject in canonical
        new_text = text
        # 1. the provenance comment
        new_text = re.sub(rf"(IGI native provenance:\s*){re.escape(subject)}(\s*-\s*0x)",
                          rf"\g<1>{want}\g<2>", new_text)
        # 2. the address header comment
        new_text = re.sub(rf"(/\* {m.group(1)} ){re.escape(subject)}( \|)",
                          rf"\g<1>{want}\g<2>", new_text)
        # 3. the include of its own header
        own = [h for h in headers.get(addr, [])
               if (m.group(3) or "") in h.name or h.name.endswith(f"{want}.hpp")]
        if own:
            chosen = own[0].name
            new_text = re.sub(r'#include\s+"[^"]+\.hpp"',
                              f'#include "{chosen}"', new_text, count=1)
        # 4. the definition / declaration line
        def swap(match):
            if match.group("name") != subject:
                return match.group(0)
            return match.group("head") + want + "("
        new_text = DEFINITION.sub(swap, new_text, count=1)
        # 5. call sites, only when the old name is unambiguous
        if not shared:
            new_text = re.sub(rf"\b{re.escape(subject)}\b(?=\s*\()", want, new_text)

        if new_text == text:
            skipped.append({"path": str(path.relative_to(ROOT)),
                            "reason": f"{subject} is also the canonical name of another address"
                                      if shared else "nothing to rewrite"})
            continue
        if apply:
            path.write_text(new_text, encoding="utf-8")
        fixed.append({"path": str(path.relative_to(ROOT)),
                      "address": f"0x{addr:08X}", "from": subject, "to": want,
                      "sharedName": shared})

    dest = ROOT / "docs" / "fidelity" / "evidence" / "gamenatives-name-rewrite.json"
    dest.write_text(json.dumps({"applied": apply, "fixed": fixed, "skipped": skipped},
                                indent=2) + "\n")
    print(f"files rewritten: {len(fixed)}  skipped: {len(skipped)}  applied={apply}")
    for f in fixed[:10]:
        print(f"  {f['address']} {f['from']} -> {f['to']}  {f['path']}")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
