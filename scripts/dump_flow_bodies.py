#!/usr/bin/env python3
"""Dump the retail body of the flow engine and task-type natives.

The transcription is written from what the retail binary does, so the bodies are
collected here in one place: the function entry, its blocks, its callers and its
callees.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

FLOWS = [
    "0x00401FC0", "0x00402260", "0x00402780", "0x004027E0", "0x004027F0",
    "0x00402800", "0x004028A0", "0x00402930", "0x00402C40",
    "0x00401900", "0x00401CF0",
]


def main() -> None:
    bn = BN()
    out = {}
    for address in FLOWS:
        text = bn.call("bn_function_decompile", {"function": address})
        m = re.search(r"````bn-pseudoc\n(.*?)\n````", text, re.S)
        body = m.group(1) if m else text
        info = bn.call("bn_function_info", {"function": address})
        callers = bn.call("bn_function_callers", {"function": address})
        callees = bn.call("bn_function_callees", {"function": address})
        out[address] = {
            "name": bn.header(info, "name"),
            "body": body,
            "callers": [line.split("|")[1].strip()
                        for line in callers.splitlines() if line.startswith("| 0x")],
            "callees": re.findall(r"\| (0x[0-9a-f]+) \(x86\)", callees),
        }
        print("=" * 90)
        print(f"{address}  {out[address]['name']}")
        print(f"  callers: {out[address]['callers'][:6]}")
        print(f"  callees: {out[address]['callees'][:8]}")
        print(re.sub(r"\n\s*\n", "\n", body)[:1500])

    dest = ROOT / "assets" / "flow_retail_bodies.json"
    dest.write_text(json.dumps(out, indent=2) + "\n")
    print(f"\nwritten {dest}")


if __name__ == "__main__":
    main()
