#!/usr/bin/env python3
"""Parse a Frida level probe log into structured runtime evidence."""

import ast
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def extract_payload(line: str):
    """Pull the {'type': ..., ...} payload dict out of a Frida message line."""
    marker = "'payload': "
    i = line.find(marker)
    if i < 0:
        return None
    i += len(marker)
    if i >= len(line) or line[i] != "{":
        return None
    depth = 0
    in_str = False
    quote = ""
    for j in range(i, len(line)):
        c = line[j]
        if in_str:
            if c == quote and line[j - 1] != "\\":
                in_str = False
            continue
        if c in "'\"":
            in_str = True
            quote = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                try:
                    return ast.literal_eval(line[i:j + 1])
                except (ValueError, SyntaxError):
                    return None
    return None


def main() -> None:
    log = Path(sys.argv[1])
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else None
    text = log.read_text(errors="replace")

    start = None
    hooks = None
    latest = {}
    for line in text.splitlines():
        if "probe_start" in line:
            start = extract_payload(line) or start
        elif "probe_hooks" in line:
            hooks = extract_payload(line) or hooks
        elif "probe_snapshot" in line:
            snap = extract_payload(line)
            if snap:
                latest = snap.get("called", {})

    doc = {
        "tool": "frida",
        "probe": "scripts/frida_level_probe.js",
        "log": str(log),
        "module": (start or {}).get("module"),
        "base": (start or {}).get("base"),
        "retailAddressMode": (start or {}).get("retailAddressMode"),
        "targetCount": (start or {}).get("targetCount"),
        "hooksInstalled": (hooks or {}).get("installed"),
        "hooksFailed": (hooks or {}).get("failed"),
        "observed": {
            name: {
                "va": info["va"],
                "hits": info["hits"],
                "firstCalls": info.get("first", []),
            }
            for name, info in sorted(latest.items())
        },
        "observedCount": len(latest),
        "limitations": [
            "Frida attached after the process started, so startup and QVM-load "
            "calls are not observed",
            "argument words and EAX are recorded without dereferencing; the ABI "
            "of each native is not assumed",
            "hit counts and return values are runtime observations of the retail "
            "binary only; they are not a comparison against the reconstruction",
        ],
    }
    dest = out or (ROOT / "docs" / "fidelity" / "evidence" / "retail-level4-frida.json")
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(doc, indent=2) + "\n")
    print(f"observed natives: {doc['observedCount']}")
    print(f"hooks: {doc['hooksInstalled']} installed / {doc['hooksFailed']} failed")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
