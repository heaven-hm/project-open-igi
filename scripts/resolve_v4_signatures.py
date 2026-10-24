#!/usr/bin/env python3
"""Resolve Ghidra 'undefinedN' placeholders in IGINatives_v4.json using Binary Ninja.

Binary Ninja's IL analysis produces a concrete prototype for every parameter and
return value. This maps those concrete types back onto the catalog signature by
position, so no parameter is left as undefinedN. The catalog's own name, return
type and parameter names are preserved unless the placeholder is the only source
of the type.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
CALLER = ROOT / "scripts" / "bn_mcp_call.py"
BN_TO_C = {
    "int8_t": "int8_t", "uint8_t": "uint8_t",
    "int16_t": "int16_t", "uint16_t": "uint16_t",
    "int32_t": "int32_t", "uint32_t": "uint32_t",
    "int64_t": "int64_t", "uint64_t": "uint64_t",
    "bool": "bool", "char": "char", "void": "void",
    "float": "float", "double": "double",
    "int32_t*": "int32_t *", "void*": "void *",
}
UNDEF_WIDTH = {"undefined": 4, "undefined1": 1, "undefined2": 2, "undefined4": 4,
               "undefined8": 8, "undefined16": 16}


def bn_prototype(addr: str) -> str:
    p = subprocess.run(
        [sys.executable, str(CALLER), "bn_function_prototype_get",
         json.dumps({"function": addr})],
        capture_output=True, text=True, timeout=300,
    )
    try:
        data = json.loads(p.stdout)
        return data["prototype"]["definition"]
    except Exception:
        return ""


def split_args(defn: str):
    """Split a BN definition 'ret(args)' into (return_type, [param_types])."""
    m = re.match(r"^(.*?)\((.*)\)\s*$", defn, re.S)
    if not m:
        return defn.strip(), []
    ret = m.group(1).strip()
    params = []
    depth = 0
    cur = ""
    for ch in m.group(2):
        if ch == "," and depth == 0:
            params.append(cur)
            cur = ""
            continue
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        cur += ch
    if cur.strip():
        params.append(cur)
    types = []
    for prm in params:
        prm = prm.strip()
        if prm in ("void", "..."):
            types.append(prm)
            continue
        toks = prm.split()
        ptr = "".join(t for t in toks if t == "*")
        base = [t for t in toks if t != "*"][0]
        types.append(BN_TO_C.get(base, base) + (" " + ptr if ptr else ""))
    return ret, types


def resolve(entry: dict) -> dict:
    sig = entry["signature"]
    if "undefined" not in sig:
        return entry
    defn = bn_prototype(entry["address"])
    if not defn:
        entry["resolutionError"] = "no Binary Ninja prototype available"
        return entry
    ret, params = split_args(defn)

    m = re.match(r"^(.*?\()([^)]*)(\);?)$", sig, re.S)
    if not m:
        entry["resolutionError"] = f"unparsable signature: {sig}"
        return entry
    head, body, tail = m.group(1), m.group(2), m.group(3)

    arg_texts = []
    depth = 0
    cur = ""
    for ch in body:
        if ch == "," and depth == 0:
            arg_texts.append(cur)
            cur = ""
            continue
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        cur += ch
    if cur.strip():
        arg_texts.append(cur)

    head = re.sub(r"undefined\d*\b", lambda _m: ret, head, count=1)
    out_args = []
    for i, at in enumerate(arg_texts):
        bn_t = params[i] if i < len(params) else None
        if "undefined" in at:
            if bn_t is None or bn_t == "void":
                bn_t = {"undefined": "uint32_t", "undefined1": "uint8_t",
                        "undefined2": "uint16_t", "undefined4": "uint32_t",
                        "undefined8": "uint64_t"}[
                    re.search(r"undefined\d*", at).group(0)]
            # keep the original parameter name, swap only the type
            names = at.split()
            nametok = names[-1] if names and names[-1].startswith("param") else ""
            out_args.append(f"{bn_t.strip()} {nametok}".strip())
        else:
            out_args.append(at.strip())
    entry["signature"] = head + ", ".join(out_args) + tail
    entry["bnPrototype"] = defn
    entry["resolvedBy"] = "binaryninja"
    return entry


def main() -> None:
    doc = json.loads(V4.read_text())
    changed = []
    for entry in doc["Natives"]:
        before = entry["signature"]
        entry = resolve(entry)
        if entry["signature"] != before:
            changed.append((entry["address"], before, entry["signature"]))
    V4.write_text(json.dumps(doc, indent=2) + "\n")
    print(f"resolved={len(changed)}")
    for a, b, c in changed:
        print(f"  {a}\n    - {b}\n    + {c}")


if __name__ == "__main__":
    main()
