#!/usr/bin/env python3
"""Survey Binary Ninja prototype state for every native in IGINatives_v4.json."""

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
V4 = ROOT / "assets" / "IGINatives_v4.json"
CALLER = ROOT / "scripts" / "bn_mcp_call.py"


def call(tool: str, args: dict) -> dict:
    p = subprocess.run(
        [sys.executable, str(CALLER), tool, json.dumps(args)],
        capture_output=True,
        text=True,
        timeout=300,
    )
    out = p.stdout.strip()
    if not out:
        return {"_error": p.stderr.strip()[:200]}
    try:
        return json.loads(out)
    except json.JSONDecodeError:
        return {"_error": out[:200]}


def main() -> None:
    natives = json.loads(V4.read_text())["Natives"]
    rows = []
    for n in natives:
        addr = n["address"]
        info = call("bn_function_info", {"function": addr})
        name = ""
        if "text" in info:
            for line in info["text"].splitlines():
                if line.startswith("name:"):
                    name = line.split(":", 1)[1].strip().strip('"')
                    break
        proto = call("bn_function_prototype_get", {"function": addr})
        definition = ""
        if "prototype" in proto:
            definition = proto["prototype"].get("definition", "")
            bnname = proto["prototype"].get("function", {}).get("name", "")
            if bnname:
                name = bnname
        rows.append(
            {
                "address": addr,
                "v4Name": n["name"],
                "bnName": name,
                "v4Sig": n["signature"],
                "bnDef": definition,
                "nameMatch": name == n["name"],
                "sigMatch": definition.replace(" ", "") == n["signature"].replace(" ", ""),
            }
        )
    out = ROOT / "assets" / "bn_survey_v4.json"
    out.write_text(json.dumps(rows, indent=2))
    nm = sum(r["nameMatch"] for r in rows)
    sm = sum(r["sigMatch"] for r in rows)
    print(f"surveyed={len(rows)} nameMatch={nm} sigMatch={sm}")
    print(f"written: {out}")
    mism = [r for r in rows if not r["nameMatch"]][:15]
    for r in mism:
        print(f"  {r['address']} v4={r['v4Name']} bn={r['bnName']}")


if __name__ == "__main__":
    main()
