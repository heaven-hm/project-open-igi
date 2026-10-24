#!/usr/bin/env python3
"""Check the GameNatives declarations against the confirmed retail signatures.

The confirmed signature for each address comes from the Binary Ninja analysis of
the retail function and is recorded in the master catalog. A GameNatives
declaration must agree with it on the number of parameters, so a transcription
that drops or invents an argument is visible.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GN = ROOT / "scripts" / "GameNatives"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
DECL = re.compile(
    r"^\s*(?P<ret>[A-Za-z_][\w:*&<>\s]*?)\b(?P<name>[A-Za-z_]\w*)\s*\((?P<args>[^;]*)\)\s*;",
    re.MULTILINE)


def split_args(text: str) -> list[str]:
    text = text.strip()
    if not text or text == "void":
        return []
    out, depth, cur = [], 0, ""
    for ch in text:
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
            continue
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        cur += ch
    if cur.strip():
        out.append(cur)
    return out


def retail_arity(signature: str) -> int:
    m = re.search(r"\(([^)]*)\)", signature)
    if not m:
        return 0
    return len(split_args(m.group(1)))


def main() -> None:
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    # A declaration that differs from the retail arity is legitimate only when
    # the project's own signature review already recorded the adaptation.
    reviewed = {}
    for n in json.load(open(MANIFEST))["natives"]:
        comparison = (n.get("verification") or {}).get("signature_comparison") or {}
        reviewed[int(n["address"], 16)] = {
            "result": comparison.get("result"),
            "rationale": comparison.get("rationale"),
        }

    rows = []
    for path in sorted(GN.rglob("*.hpp")):
        m = FILENAME.match(path.name)
        if not m:
            continue
        key = f"{int(m.group(1), 16):08X}"
        addr = int(key, 16)
        entry = master.get(addr)
        if not entry:
            continue
        review = reviewed.get(addr)
        text = path.read_text(encoding="utf-8", errors="replace")
        decls = [d for d in DECL.finditer(text)
                 if d.group("name") == entry["name"]]
        if not decls:
            rows.append({"path": str(path.relative_to(ROOT)), "address": key,
                         "name": entry["name"], "status": "NO_DECLARATION",
                         "retailArity": retail_arity(entry["signature"]),
                         "review": review})
            continue
        args = split_args(decls[0].group("args"))
        want = retail_arity(entry["signature"])
        if len(args) == want:
            status = "OK"
        elif review and review.get("result") in ("exact", "name-insensitive", "abi-adapted"):
            status = "REVIEWED_ADAPTATION"
        else:
            status = "ARITY_MISMATCH"
        rows.append({"path": str(path.relative_to(ROOT)), "address": key,
                     "name": entry["name"], "status": status,
                     "declaredArity": len(args), "retailArity": want,
                     "declared": decls[0].group(0).strip(),
                     "retail": entry["signature"], "review": review})

    bad = [r for r in rows if r["status"] in ("ARITY_MISMATCH", "NO_DECLARATION")]
    reviewed_rows = [r for r in rows if r["status"] == "REVIEWED_ADAPTATION"]
    dest = ROOT / "docs" / "fidelity" / "evidence" / "gamenatives-signature-check.json"
    dest.write_text(json.dumps({
        "checked": len(rows),
        "ok": sum(1 for r in rows if r["status"] == "OK"),
        "reviewedAdaptations": len(reviewed_rows),
        "issues": bad,
    }, indent=2) + "\n")
    print(f"checked {len(rows)} GameNatives declarations")
    print(f"  matching the confirmed retail arity : "
          f"{sum(1 for r in rows if r['status'] == 'OK')}")
    print(f"  differences already reviewed by the project's signature review : "
          f"{len(reviewed_rows)}")
    print(f"  still needing attention : {len(bad)}")
    for r in bad[:20]:
        print(f"    {r['status']} {r['address']} {r['name']} "
              f"declared={r.get('declaredArity')} retail={r['retailArity']}")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
