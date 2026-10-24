#!/usr/bin/env python3
"""Verify the IGIPatch confirmed natives against the retail binary and record it.

The IGIPatch list is human-verified, so this records the independent evidence
for it: the three-tool function-entry check (Binary Ninja, Radare2, angr) and
the runtime observation from a Frida probe on the retail igi.exe.

The result is written next to the catalog so the two live together.
"""

import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import importlib.util  # noqa: E402

spec = importlib.util.spec_from_file_location(
    "vnb", ROOT / "scripts" / "verify_natives_batch.py")
vnb = importlib.util.module_from_spec(spec)
spec.loader.exec_module(vnb)

IGIPATCH = ROOT / "assets" / "igipatch_confirmed_natives.json"
THREE_WAY = ROOT / "assets" / "gamenatives_three_way.json"
ANGR = ROOT / "assets" / "gamenatives_angr.json"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
V4 = ROOT / "assets" / "IGINatives_v4.json"
OUT = ROOT / "scripts" / "IGIPatch_natives_frida_verification.json"


def main() -> None:
    apply = "--apply" in sys.argv
    duration = 25
    for arg in sys.argv[1:]:
        if arg.startswith("--duration="):
            duration = int(arg.split("=", 1)[1])

    igipatch = json.load(open(IGIPATCH))["natives"]
    three = {int(e["address"], 16): e
             for e in json.load(open(THREE_WAY))["entries"]}
    angr = json.load(open(ANGR))["addresses"]
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    v4 = {int(n["address"], 16): n
          for n in json.load(open(V4))["Natives"]}

    rows = []
    for entry in igipatch:
        addr = int(entry["address"], 16)
        key = f"0x{addr:08X}"
        t = three.get(addr, {})
        a = angr.get(key, {})
        m = master.get(addr, {})
        rows.append({
            "address": entry["address"],
            "name": entry["name"],
            "inV4Catalog": addr in v4,
            "v4Name": v4.get(addr, {}).get("name"),
            "signature": v4.get(addr, {}).get("signature"),
            "retailSignature": m.get("signature"),
            "binaryNinjaEntry": bool(t.get("bnEntry")),
            "radare2Entry": bool(t.get("r2Entry")),
            "angrEntry": bool(a.get("is_function_entry")),
            "threeWayConfirmed": bool(t.get("confirmed")),
        })

    print(f"IGIPatch natives: {len(rows)}")
    vnb.kill_stale("igi.exe")
    vnb.kill_stale("open-igi.exe")
    print("starting retail igi.exe ...", flush=True)
    retail = vnb.start_game(vnb.RETAIL_EXE, 4)
    # the level load takes a while before the per-frame loop starts, so the
    # probe is attached only once the game is running
    time.sleep(45)
    pid = vnb.find_pid("igi.exe")
    # a known per-frame native is probed as a control, so a run with no
    # observations can be told apart from a probe that never attached
    control = {"name": "CONTROL_Game_GetContext", "va": "0x00402880"}
    targets = [{"name": r["name"], "va": r["address"]} for r in rows] + [control]
    probe = vnb.frida_probe("igi.exe", targets, duration, pid=pid)
    print(f"  probe: pid={pid} installed={probe.get('installed')} "
          f"observed={len(probe.get('hits', {}))}", flush=True)
    vnb.stop_game(retail)

    hits = probe.get("hits", {})
    first = probe.get("first", {})
    control_hits = hits.get(control["name"])
    print(f"  control native hits: {control_hits}")
    for r in rows:
        r["retailHits"] = hits.get(r["name"])
        r["retailFirstCall"] = first.get(r["name"], [None])[0] if first.get(r["name"]) else None
        r["retailCalled"] = r["retailHits"] is not None

    observed = sum(1 for r in rows if r["retailCalled"])
    confirmed = sum(1 for r in rows if r["threeWayConfirmed"])
    in_v4 = sum(1 for r in rows if r["inV4Catalog"])
    named = sum(1 for r in rows if r["v4Name"] == r["name"])

    doc = {
        "source": "IGIPatch confirmed native list",
        "note": "The IGIPatch list is human-verified. This file records the "
                "independent evidence for it: the three-tool function-entry check "
                "and the retail runtime observation.",
        "binary": "IGI.EXE (Retail v1.0 Europe)",
        "binarySha256": vnb.RETAIL_SHA256 if hasattr(vnb, "RETAIL_SHA256") else
                        "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01",
        "level": 4,
        "probe": {
            "tool": "frida",
            "process": "igi.exe",
            "durationSeconds": duration,
            "hooksInstalled": probe.get("installed"),
            "hooksFailed": probe.get("failures"),
            "controlNative": {
                "name": control["name"],
                "va": control["va"],
                "hits": control_hits,
                "note": "a per-frame native probed alongside, to show the probe was live",
            },
        },
        "summary": {
            "natives": len(rows),
            "confirmedFunctionEntryByAllThreeTools": confirmed,
            "presentInV4Catalog": in_v4,
            "v4NameMatchesIgipatchName": named,
            "observedCalledInRetailRuntime": observed,
        },
        "natives": rows,
    }
    if apply:
        OUT.write_text(json.dumps(doc, indent=2) + "\n")
        print(f"written {OUT}")
    print(json.dumps(doc["summary"], indent=2))


if __name__ == "__main__":
    main()
