#!/usr/bin/env python3
"""Map every implemented native to its engine function and candidate symbol.

A native is implemented when the engine declares a function for its address.
The C++ name can differ from the retail label, so the engine source is searched
for the declaration, and the resulting symbol is looked up in open-igi.exe.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OPEN_IGI = ROOT / "build-win32" / "engine" / "platform" / "windows" / "open-igi.exe"
if not OPEN_IGI.exists():
    OPEN_IGI = Path("/Users/haseeb-mir/Downloads/IGI/open-igi.exe")
R2 = "/opt/homebrew/bin/rabin2"

DECL = re.compile(
    r"^\s*(?:\[\[[^\]]*\]\]\s*)?(?:extern\s+\"C\"\s*)?"
    r"(?P<ret>[A-Za-z_][\w:*&<>\s]*?)\b(?P<name>[A-Za-z_]\w*)\s*\((?P<args>[^;{]*)\)\s*;",
    re.MULTILINE)
NATIVE_COMMENT = re.compile(
    r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
    re.IGNORECASE)


def main() -> None:
    manifest = json.loads((ROOT / "docs" / "fidelity" / "native-verification.json").read_text())
    records = {f"0x{int(n['address'], 16):08X}": n for n in manifest["natives"]}

    # the engine's own native comments give address -> C++ declaration
    engine_decls: dict[str, dict] = {}
    for path in sorted((ROOT / "engine" / "src" / "retail").rglob("*.cpp")):
        text = path.read_text(encoding="utf-8", errors="replace")
        for index, line in enumerate(text.splitlines()):
            m = NATIVE_COMMENT.match(line)
            if not m:
                continue
            address = f"0x{int(m.group('address'), 16):08X}"
            label = m.group("name").strip().rstrip(";").strip()
            label = re.sub(r"\s+retail$", "", label, flags=re.IGNORECASE).strip()
            block = text.splitlines()[index:index + 40]
            # the C++ function normally carries the retail name
            found = None
            for candidate in block:
                if re.search(rf"\b{re.escape(label)}\s*\(", candidate):
                    found = candidate
                    break
            if found is None:
                # the declaration can sit further away when the comment block is long
                for candidate in text.splitlines()[index:]:
                    if re.search(rf"\b{re.escape(label)}\s*\(", candidate):
                        found = candidate
                        break
            if found is None:
                for candidate in block:
                    d = DECL.match(candidate)
                    if d and d.group("name") != "Native":
                        found = candidate
                        label = d.group("name")
                        break
            if found is not None:
                engine_decls[address] = {
                    "cppName": label,
                    "path": str(path.relative_to(ROOT)),
                    "declaration": found.strip(),
                }

    # nm lists the local text symbols the verification build keeps; rabin2
    # only reports the global ones. A PE symbol for an extern "C" entry keeps
    # a leading underscore, so both spellings are accepted below.
    out = subprocess.run(["nm", str(OPEN_IGI)], capture_output=True, text=True).stdout
    demangled = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) != 3 or parts[1] not in ("T", "t", "W"):
            continue
        try:
            vaddr = int(parts[0], 16)
        except ValueError:
            continue
        raw = parts[2]
        pretty = subprocess.run(["c++filt", raw], capture_output=True,
                                text=True).stdout.strip()
        demangled.setdefault(raw, {"vaddr": f"0x{vaddr:08X}", "symbol": raw,
                                    "demangled": pretty})

    rows = []
    for address, rec in sorted(records.items()):
        decl = engine_decls.get(address)
        candidate = None
        if decl:
            wanted = decl["cppName"]
            for raw, info in demangled.items():
                if raw == wanted or raw == "_" + wanted:
                    candidate = info
                    break
                pretty = info["demangled"]
                if re.search(rf"[:(]\s*{re.escape(wanted)}\s*\(", pretty) or \
                        re.search(rf"[:(]\s*{re.escape(wanted)}\b", pretty):
                    candidate = info
                    break
        rows.append({
            "address": address,
            "retailName": rec.get("name", ""),
            "implemented": decl is not None,
            "cppName": decl["cppName"] if decl else None,
            "enginePath": decl["path"] if decl else None,
            "candidateSymbol": candidate,
            "status": rec.get("verification", {}).get("status"),
        })

    dest = ROOT / "assets" / "candidate_symbol_map.json"
    dest.write_text(json.dumps(rows, indent=2) + "\n")
    implemented = [r for r in rows if r["implemented"]]
    hooked = [r for r in implemented if r["candidateSymbol"]]
    print(f"manifest natives        : {len(rows)}")
    print(f"implemented in the engine: {len(implemented)}")
    print(f"with a candidate symbol : {len(hooked)}")
    for r in implemented[:10]:
        sym = r["candidateSymbol"]
        print(f"  {r['address']} {r['retailName']:<30} cpp={r['cppName']:<28} "
              f"sym={sym['vaddr'] if sym else 'NOT FOUND'}")


if __name__ == "__main__":
    main()
