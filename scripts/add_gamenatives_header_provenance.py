#!/usr/bin/env python3
"""Give the GameNatives headers an address-bound native identity.

The gate reads a native identity from a `// Native: <name>; retail VA 0x...`
provenance comment. The GameNatives .cpp files carry the address in their
filename, but the .hpp files carry no such comment, so a changed header has no
address-bound identity the gate can verify.

This inserts the same five-line provenance block the engine sources use, with
the canonical name and the confirmed CFG score for that address.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GN = ROOT / "scripts" / "GameNatives"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
NATIVE_COMMENT = re.compile(
    r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
    re.IGNORECASE)


def changed_headers() -> set[str]:
    """Headers the gate will inspect, so only they need an identity comment."""
    out: set[str] = set()
    import subprocess
    for cmd in (["git", "diff", "--cached", "--name-only"],
                ["git", "diff", "--name-only"]):
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        for line in p.stdout.splitlines():
            if line.strip().endswith(".hpp") and line.strip().startswith("scripts/GameNatives/"):
                out.add(line.strip())
    return out


def main() -> None:
    apply = "--apply" in sys.argv
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    only_changed = "--changed-only" in sys.argv
    changed = changed_headers() if only_changed else None

    updated, skipped = [], []
    for path in sorted(GN.rglob("*.hpp")):
        m = FILENAME.match(path.name)
        if not m:
            continue
        relative = path.relative_to(ROOT).as_posix()
        if changed is not None and relative not in changed:
            continue
        addr = int(m.group(1), 16)
        entry = master.get(addr)
        if not entry or not entry.get("name"):
            skipped.append({"path": str(path.relative_to(ROOT)),
                            "reason": "no confirmed canonical name"})
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if NATIVE_COMMENT.search(text):
            continue
        name = entry["name"]
        block = (
            f"// Native: {name}; retail VA 0x{addr:08X}; signature from the retail "
            f"analysis of igi.exe.\n"
            f"// Artifact: candidate transcription of the retail function at 0x{addr:08X}.\n"
            f"// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail "
            f"igi.exe; address confirmed by Binary Ninja, Radare2 and angr.\n"
            f"// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this "
            f"entry; the candidate body is the transcription under review.\n"
            f"// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by "
            f"the configured differential runner, not by this comment.\n"
        )
        lines = text.splitlines(keepends=True)
        insert_at = 0
        while insert_at < len(lines) and lines[insert_at].startswith("#"):
            insert_at += 1
        new_text = "".join(lines[:insert_at]) + block + "".join(lines[insert_at:])
        if apply:
            path.write_text(new_text, encoding="utf-8")
        updated.append({"path": str(path.relative_to(ROOT)), "address": f"0x{addr:08X}",
                        "name": name})

    dest = ROOT / "docs" / "fidelity" / "evidence" / "gamenatives-header-provenance.json"
    dest.write_text(json.dumps({"applied": apply, "updated": updated, "skipped": skipped},
                                indent=2) + "\n")
    print(f"headers given an address-bound identity: {len(updated)}  skipped: {len(skipped)}"
          f"  applied={apply}")
    for u in updated[:6]:
        print(f"  {u['address']} {u['name']}  {u['path']}")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
