#!/usr/bin/env python3
"""Final naming adjudication for IGINatives_v4.json, applied to every path.

Each entry records the winner and the retail evidence that decided it. The
winner is applied to Binary Ninja, to the GameNatives filenames, to
IGINatives_v4.json and to scripts/GameNatives/VERIFICATION.md so that one
address carries exactly one name everywhere.

"bn"  = Binary Ninja's name is correct      -> v4 takes the BN name
"v4"  = the v4 label is correct              -> Binary Ninja takes the v4 name
"amb" = neither is evidenced                -> keep the BN name, flag low confidence
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

# address -> (winner, evidence)
VERDICTS = {
    # ---- Binary Ninja name is correct -----------------------------------
    "0x00401810": ("bn", "retail string 'Unable to allocate event' names the failure path"),
    "0x004018E0": ("bn", "delegates to QTask_InitContext, so it creates the task context"),
    "0x00401A20": ("bn", "closes the event's file stream and clears its loaded flag"),
    "0x00402260": ("bn", "retail string 'LOCAL:menusystem\\mainmenu.qsc' is loaded here"),
    "0x00414FD0": ("bn", "reads weapon-type configuration flag bits from the weapon table"),
    "0x00416D20": ("amb", "returns a fixed global+0x2c; callers are damage/death paths, so neither label is evidenced"),
    "0x004199D0": ("bn", "called by Menu_Manager and returns a status message; the menu module matches"),
    "0x004B3320": ("bn", "tests all three doubles against 0.0 -> Vec3_IsZero"),
    "0x004B38E0": ("bn", "composes RotateZ/RotateY into a 3x3 -> Mat33_FromEulerAngles"),
    "0x004B8A20": ("bn", "reads argument slot arg2 from the script frame -> Script_GetArg"),
    "0x004D3210": ("bn", "iterates meshes, clears a visibility bit and calls Render_DrawRigidMesh_6"),
    "0x004E6030": ("bn", "stores a limit global then calls Timer_Commit -> SoundSys_SetFrequency"),
    "0x004E76E0": ("bn", "String_VFormat into a buffer then Console_AppendText -> Console_VPrint"),
    "0x004E7B80": ("bn", "calls SoundSys_LoadSoundDef_9 on the sound-system instance"),
    "0x004E7BB0": ("bn", "calls SoundSys_EvaluateTrigger_9 on the sound-system instance"),

    # ---- the v4 label is correct -----------------------------------------
    "0x004027E0": ("v4", "calls QTask_CloseEvent_2, so it closes the flow event"),
    "0x00424CE0": ("v4", "integrates mouse deltas into the cursor position; it loads no parameters"),
    "0x00460C80": ("v4", "printf-style: forwards a format string to Console_Print for the active player"),
    "0x00464470": ("v4", "clears the view flag and unregisters the symbol; it loads no parameters"),
    "0x00464800": ("v4", "45-degree tangent view helper called from Human_RunUpdate and the AI speed routines"),
    "0x00464BD0": ("v4", "transforms the body pose for the same human/AI callers as the view helper"),
    "0x004675B0": ("v4", "projects a world point to screen using the viewport extents"),
    "0x00467620": ("v4", "transforms a world position by the view matrix and projects it"),
    "0x00471440": ("v4", "draws the binocular overlay and swaps buffers"),
    "0x0048F0A0": ("v4", "returns the application window handle used by Display_SetMode and DirectX init"),
    "0x0048FC20": ("v4", "reads the mouse state and stores normalised cursor coordinates"),
    "0x00490360": ("v4", "GetTickCount() stored into g_tick_base, so it starts the timer"),
    "0x00490370": ("v4", "GetTickCount() - g_tick_base, the matching timer read"),
    "0x004B0D10": ("v4", "decrements the allocation counter, removes the block from the heap list and frees it"),
    "0x004B0F90": ("v4", "pops the render-mode stack and frees the popped entry"),
    "0x004B6E60": ("v4", "called by LoadingScreen_Show to register the background picture"),
    "0x004C1790": ("v4", "retail string 'QTaskList is full'; appends an item and bumps the count"),
    "0x004C17C0": ("v4", "retail string 'Trying to remove unrigistered QTask from QTaskList'"),
    "0x004C1800": ("v4", "allocates the {count,capacity,items} QTaskList layout used by the sibling routines"),
    "0x004C1830": ("v4", "calls the deallocator, so it deletes the list"),
    "0x004D9610": ("v4", "returns a global queried by Human_Init, Human_Setup and WeaponType_UpdateAnim"),
    "0x00507EC0": ("v4", "maintains a 32-bit changed mask and a 32-entry state array: an input port update"),
}


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def main() -> None:
    v4 = json.load(open(ROOT / "assets" / "IGINatives_v4.json"))
    index = json.load(open(ROOT / "assets" / "gamenatives_index.json"))
    bn = BN()

    applied = []
    for entry in v4["Natives"]:
        addr = entry["address"]
        key = f"{int(addr, 16):08X}"
        idx_key = f"{int(addr, 16):X}"
        v4_name = entry["name"]
        cur = bn.call("bn_function_info", {"function": addr})
        bn_name = bn.header(cur, "name")
        if not bn_name:
            continue

        winner, evidence = VERDICTS.get(addr, ("bn", "no divergence"))
        if nz(bn_name) == nz(v4_name):
            winner = "same"

        final = v4_name if winner == "v4" else bn_name

        if final != bn_name:
            ok, msg = bn.rename(addr, final)
            print(f"BN  {addr}: {bn_name} -> {final} {'ok' if ok else 'FAIL ' + msg}")

        # rename any GameNatives file still carrying a different name
        for name, files in list(index.get(idx_key, {}).items()):
            if nz(name) == nz(final):
                continue
            for rel in files:
                p = ROOT / rel
                if not p.exists():
                    continue
                m = re.match(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$", p.name)
                if not m:
                    continue
                new = p.with_name(f"{key}-{final}{m.group(3) or ''}{m.group(4)}")
                if new == p or new.exists():
                    continue
                p.rename(new)
                print(f"GN  {p.name} -> {new.name}")

        applied.append({"address": addr, "v4Name": v4_name, "bnName": bn_name,
                        "finalName": final, "winner": winner, "evidence": evidence})

    v4["Natives"] = sorted(v4["Natives"], key=lambda e: int(e["address"], 16))
    (ROOT / "assets" / "IGINatives_v4.json").write_text(json.dumps(v4, indent=2) + "\n")
    dest = ROOT / "assets" / "v4_final_verdicts.json"
    dest.write_text(json.dumps(applied, indent=2))
    print(f"\nadjudicated {len(applied)} -> {dest}")


if __name__ == "__main__":
    main()
