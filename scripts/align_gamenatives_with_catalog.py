#!/usr/bin/env python3
"""Align the GameNatives tree with the confirmed catalog.

Every GameNatives file is filed under the confirmed name for its address, and
the declaration inside it is renamed to match, so the tree, the catalog, the
Binary Ninja database and the engine all carry one name per address.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")


def main() -> None:
    apply = "--apply" in sys.argv
    catalog = {int(e["address"], 16): e["name"]
               for e in json.load(open(V4))["Natives"]}
    index = json.loads((ROOT / "assets" / "gamenatives_index.json").read_text())

    renamed, conflicts = [], []
    for idx_key, names in sorted(index.items()):
        addr = int(idx_key, 16)
        want = catalog.get(addr)
        if want is None:
            continue
        for old_name, files in list(names.items()):
            if old_name == want:
                continue
            for rel in files:
                path = ROOT / rel
                if not path.exists():
                    continue
                m = FILENAME.match(path.name)
                if not m:
                    continue
                new = path.with_name(f"{addr:08X}-{want}{m.group(3) or ''}{m.group(4)}")
                if new == path:
                    continue
                if new.exists():
                    conflicts.append({"path": rel, "target": new.name})
                    continue
                text = path.read_text(encoding="utf-8", errors="replace")
                text = re.sub(rf"\b{re.escape(old_name)}\b", want, text)
                if apply:
                    path.write_text(text, encoding="utf-8")
                    path.rename(new)
                renamed.append({"from": rel, "to": str(new.relative_to(ROOT))})

    print(f"renamed: {len(renamed)}  conflicts: {len(conflicts)}")
    for r in renamed[:12]:
        print(f"  {r['from']} -> {r['to']}")
    for c in conflicts[:8]:
        print(f"  CONFLICT {c['path']} -> {c['target']}")
    if apply and renamed:
        print("re-indexing ...")
        import subprocess
        subprocess.run([sys.executable, str(ROOT / "scripts" / "gamenatives_index.py")],
                       capture_output=True, text=True)


if __name__ == "__main__":
    main()
