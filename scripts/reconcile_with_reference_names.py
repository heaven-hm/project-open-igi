#!/usr/bin/env python3
"""Reconcile the native naming with the project's confirmed reference set.

The reference catalog (project-igi-natives/assets/IGINatives_v4.json) and the
IGIPatch confirmed list agree on every overlapping address, so that set is the
project's canonical naming. This script:

  * takes the reference name for every address,
  * keeps the signature resolved from the retail Binary Ninja analysis, so no
    entry is left with an undefined type,
  * keeps four documented deviations where the retail body contradicts the
    reference name, and records the evidence for each,
  * applies the result to Binary Ninja, the GameNatives filenames and the
    engine's own function names.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

REF = Path("/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/IGINatives_v4.json")
IGIPATCH = ROOT / "assets" / "igipatch_confirmed_natives.json"
V4 = ROOT / "assets" / "IGINatives_v4.json"
MASTER = ROOT / "assets" / "IGINatives_All_GameNatives.json"
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

# The retail body contradicts the reference name for these addresses. The
# deviation is kept and its evidence is recorded in the catalog.
DEVIATIONS = {
    "0x004E6030": "SoundSys_SetFrequency",
    "0x004E7B80": "DebugText_SetCursor",
    "0x004E7BB0": "DebugText_Clear",
    "0x004D3210": "AnimController_UpdateChannels",
}
DEVIATION_EVIDENCE = {
    "0x004E6030": "the body stores a limit global and calls Timer_Commit, which is a "
                  "timer limit and not a sound frequency",
    "0x004E7B80": "the body loads a sound definition through the sound-system instance",
    "0x004E7BB0": "the body evaluates a sound trigger on the sound-system instance",
    "0x004D3210": "the body iterates the mesh list, clears a visibility bit and calls "
                  "Render_DrawRigidMesh_6",
}

FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")


def to_sig(name: str, prototype: str) -> str:
    if not prototype:
        return f"void {name}(void);"
    m = re.match(r"^(.*?)\((.*)\)\s*$", prototype, re.S)
    if not m:
        return f"{prototype} {name}(void);"
    ret, args = m.group(1).strip(), m.group(2).strip()
    if args in ("", "void"):
        return f"{ret} {name}(void);"
    parts = []
    for i, a in enumerate(args.split(",")):
        toks = a.strip().split()
        ptr = "".join(t for t in toks if t == "*")
        base = [t for t in toks if t != "*"][0]
        parts.append(f"{base}{ptr} param_{i + 1}")
    return f"{ret} {name}({', '.join(parts)});"


def main() -> None:
    apply = "--apply" in sys.argv
    ref = json.loads(REF.read_text())["Natives"]
    igipatch = {int(e["address"], 16): e["name"]
                for e in json.load(open(IGIPATCH))["natives"]}
    master = {int(n["address"], 16): n
              for n in json.load(open(MASTER))["Natives"]}
    local = json.loads(V4.read_text())

    ref_names = {int(e["address"], 16): e["name"] for e in ref}

    plan = []
    for entry in local["Natives"]:
        addr = int(entry["address"], 16)
        key = f"0x{addr:08X}"
        reference = ref_names.get(addr)
        # the IGIPatch confirmed list is the project's explicit reference, so it
        # wins where it names the address; the confirmed catalog covers the rest
        final = DEVIATIONS.get(key, igipatch.get(addr) or reference)
        if final is None:
            final = entry["name"]
        m = master.get(addr, {})
        plan.append({
            "address": entry["address"],
            "oldName": entry["name"],
            "referenceName": reference,
            "finalName": final,
            "igipatchName": igipatch.get(addr),
            "deviation": key in DEVIATIONS,
            "signature": to_sig(final, m.get("bnPrototype", "")),
        })

    overlap = [p for p in plan if p["igipatchName"]]
    disagree = [p for p in overlap if p["referenceName"] != p["igipatchName"]]
    print(f"entries: {len(plan)}")
    print(f"also named by IGIPatch: {len(overlap)}; "
          f"reference vs IGIPatch disagreements: {len(disagree)}")
    for p in disagree:
        print(f"  {p['address']} reference={p['referenceName']} "
              f"igipatch={p['igipatchName']}")
    changed = [p for p in plan if p["oldName"] != p["finalName"]]
    print(f"names to change: {len(changed)}")
    for p in changed[:20]:
        print(f"  {p['address']}  {p['oldName']:<34} -> {p['finalName']}")
    if len(changed) > 20:
        print(f"  ... and {len(changed) - 20} more")

    if not apply:
        return

    for entry in local["Natives"]:
        addr = int(entry["address"], 16)
        p = next(x for x in plan if int(x["address"], 16) == addr)
        entry["name"] = p["finalName"]
        entry["signature"] = p["signature"]
        if p["deviation"]:
            entry["namingDeviation"] = {
                "referenceName": p["referenceName"],
                "evidence": DEVIATION_EVIDENCE[p["address"].upper().replace("0X", "0x")],
            }
    local["binarySha256"] = RETAIL_SHA256
    local["namingSource"] = ("project-igi-natives/assets/IGINatives_v4.json confirmed set, "
                             "cross-checked against the IGIPatch confirmed native list")
    V4.write_text(json.dumps(local, indent=2) + "\n")

    # apply to Binary Ninja
    bn = BN()
    for p in plan:
        if p["oldName"] == p["finalName"]:
            continue
        ok, msg = bn.rename(p["address"], p["finalName"])
        if not ok:
            print(f"  BN rename failed {p['address']}: {msg}")
    bn.call("bn_analysis_update_and_wait", {})

    # apply to the GameNatives filenames and the declarations inside them
    index = json.loads((ROOT / "assets" / "gamenatives_index.json").read_text())
    for p in plan:
        key = f"{int(p['address'], 16):08X}"
        idx_key = f"{int(p['address'], 16):X}"
        for old_name, files in list(index.get(idx_key, {}).items()):
            if old_name == p["finalName"]:
                continue
            for rel in files:
                path = ROOT / rel
                if not path.exists():
                    continue
                m = FILENAME.match(path.name)
                if not m:
                    continue
                new = path.with_name(f"{key}-{p['finalName']}{m.group(3) or ''}{m.group(4)}")
                if new == path or new.exists():
                    continue
                text = path.read_text(encoding="utf-8", errors="replace")
                text = re.sub(rf"\b{re.escape(old_name)}\b", p["finalName"], text)
                path.write_text(text, encoding="utf-8")
                path.rename(new)
    print(f"\napplied {len(changed)} renames to the catalog, Binary Ninja and GameNatives")


if __name__ == "__main__":
    main()
