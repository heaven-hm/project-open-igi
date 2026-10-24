#!/usr/bin/env python3
"""Pull decompile + caller evidence for the GameNatives divergences, in bulk."""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

SURVEY = ROOT / "assets" / "gamenatives_bn_survey.json"
OUT = ROOT / "assets" / "gamenatives_divergence_evidence.json"


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    rows = json.load(open(SURVEY))
    div = [r for r in rows if r["bnStart"]
           and not any(nz(g) == nz(r["bnName"]) for g in r["gnNames"])]
    bn = BN()
    out = []
    for i, r in enumerate(div, 1):
        text = bn.call("bn_function_decompile", {"function": r["address"]})
        m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
        body = m.group(1) if m else ""
        callers = bn.call("bn_function_callers", {"function": r["address"]})
        caller_names = re.findall(r"\| (0x[0-9a-f]+) \(x86\)", callers)
        out.append({
            **r,
            "strings": re.findall(r'"([^"]{3,})"', body)[:8],
            "callerAddrs": caller_names[:10],
            "body": body[:1800],
        })
        if i % 15 == 0:
            print(f"  {i}/{len(div)}", flush=True)
    OUT.write_text(json.dumps(out, indent=2))
    print(f"wrote {OUT}  entries={len(out)}")


if __name__ == "__main__":
    main()
