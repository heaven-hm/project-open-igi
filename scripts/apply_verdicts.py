#!/usr/bin/env python3
"""Apply hand-adjudicated, evidence-backed verdicts for the GameNatives divergences.

Each verdict below was reached by reading the decompiled body of the retail
function in igi.exe (via Binary Ninja) plus any retail string literals and the
surrounding library block. The evidence is recorded per entry so the decision is
auditable rather than asserted.
"""

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# address -> (winner, evidence)
#   "bn"  Binary Ninja name is correct; GameNatives filename must be renamed
#   "gn"  GameNatives name is correct; Binary Ninja must be renamed
#   None  unresolved; leave both untouched until more evidence exists
VERDICTS = {
    # -- retail string states the name outright -------------------------------
    "0x403c10": ("bn", "retail string 'Config_SetContentControlPassword' is referenced inside the function"),
    "0x407d60": ("gn", "retail strings 'SetActiveMaterial'/'DefineQMaterial'/'DefineGameMaterial' name the material subsystem"),
    "0x411240": ("gn", "retail string 'earcandy' identifies the earcandy/heartbeat routine"),
    "0x411f20": ("bn", "retail string 'wire_slide_1' is the wire-slide task name"),
    "0x413a90": ("bn", "retail string 'WeaponType_Open(): Couldn\\'t load script: %s' names the function"),
    "0x4b3c50": ("bn", "retail string 'Atan2 undefined in origo' is the ToEulerAngles failure path; Mat33_ToEulerAngles is the coherent block name"),
    "0x4d14e0": ("bn", "retail strings 'Should have allocated data '/'Illegal alloc info' are terrain octree allocation assertions"),

    # -- thunks: the name must agree with the tail-call target ----------------
    "0x4e94d0": ("bn", "single tail-jump to Terrain_SampleHeightmap_2; 'Sound_Invoke2' is a caller-side alias"),
    "0x52bff0": ("bn", "single tail-jump to Script_ParseEnumReal32_5; RenderLight_Thunk is a caller-side alias"),
    "0x531470": ("bn", "single tail-jump to Render_DrawLightmapMesh_13; RenderLight_Thunk is a caller-side alias"),

    # -- Vec3 library block: bodies are unambiguous vector math ---------------
    "0x4b2c70": ("bn", "ray/plane projection using Vec3 helpers; Sys_PopStack_10 is an unrelated Ghidra guess"),
    "0x4b3100": ("bn", "scales a 3-double vector by 1/Vec3_Length; Sys_FreeMemory_11 is unrelated"),
    "0x4b3130": ("bn", "returns sqrt(x*x+y*y+z*z) over a 3-double vector; Sys_OpenFileStream_11 is unrelated"),
    "0x4b32b0": ("bn", "returns sqrt of squared component differences of two vectors; Sys_DispatchOpcode_11 is unrelated"),
    "0x4b32f0": ("bn", "squared distance between two 3-double vectors; Sys_PushStack_11 is unrelated"),
    "0x4b3320": ("bn", "tests all three doubles against 0.0 -> Vec3_IsZero; Sys_PopStack_11 is unrelated"),

    # -- Mat33 library block: bodies write 3x3 rotation matrices --------------
    "0x4b34f0": ("bn", "writes a 3x3 post-rotation about X; Sys_AllocateMemory_12 is unrelated"),
    "0x4b3640": ("bn", "writes a 3x3 post-rotation about Y; Sys_FreeMemory_12 is unrelated"),
    "0x4b3790": ("bn", "writes a 3x3 post-rotation about Z; Sys_OpenFileStream_12 is unrelated"),
    "0x4b38e0": ("bn", "composes RotateZ/RotateY into a 3x3 -> Mat33_FromEulerAngles; Sys_ReadFileBlock_12 is unrelated"),
    "0x4b3b60": ("bn", "writes a 3x3 rotation matrix using sin/cos of one angle; Sys_CloseFileStream_12 is unrelated"),
    "0x4b3ba0": ("bn", "3x3 rotation about Y; Sys_ExecuteBytecode_12 is unrelated"),
    "0x4b3be0": ("bn", "3x3 rotation about Z; Sys_DispatchOpcode_12 is unrelated"),
    "0x4b3c20": ("bn", "scales a 3x3 matrix; Sys_PushStack_12 is unrelated"),
    "0x4b3ef0": ("bn", "builds a 3x3 from an axis and angle; Sys_PopStack_12 is unrelated"),

    # -- camera block: bodies are camera update/draw --------------------------
    "0x482a40": ("bn", "first-person view matrix update (Mat33_RotateX on the view struct); FX_ExplosionSpawn_3 is unrelated"),
    "0x482c30": ("bn", "third-person camera with view-distance clamping; FX_FragmentSimulate_3 is unrelated"),
    "0x483390": ("bn", "sniper-scope camera zoom/aim comparison on +0x110/+0x114; FX_ExplosionSpawn_4 is unrelated"),
    "0x483d80": ("bn", "binocular camera update gated on a flag bit; FX_DebrisSpawn_4 is unrelated"),
    "0x484450": ("bn", "binocular overlay draw using Mat33_FromEulerAngles; FX_ExplosionSpawn_5 is unrelated"),
    "0x4868d0": ("bn", "clears a status list; FX_TrailUpdate_7 is unrelated"),

    # -- human / hit-resolution block ----------------------------------------
    "0x489c60": ("bn", "resolves a hit zone from bone-transformed coordinates; FX_000011 is unrelated"),
    "0x489e10": ("bn", "reads a limb damage record from the human struct; FX_FragmentSimulate_14 is unrelated"),
    "0x489f00": ("bn", "resets/clears a mesh cache entry; FX_ExplosionSpawn_15 is unrelated"),
    "0x48a340": ("bn", "transforms a bone to world space via the model matrix; FX_ExplosionSpawn_16 is unrelated"),

    # -- console / timer / misc blocks ---------------------------------------
    "0x487180": ("bn", "returns a single status-screen global -> StatusScreen_Get; FX_SmokeEmit_8 is unrelated"),
    "0x485750": ("bn", "enqueues a status message; StatusMessage_Process was the caller-side label"),
    "0x4869b0": ("bn", "loads STATUSSCREEN:*.res/.spr resources -> StatusScreen_OnLoad"),
    "0x486f30": ("bn", "handles a status-screen event with a 'Time Left' string -> StatusScreen_OnEvent"),
    "0x490370": ("bn", "returns GetTickCount() - g_tick_base -> Timer_Now; Display_RestoreDevice is unrelated"),
    "0x495cc0": ("bn", "commits a timer value; Display_SetGammaRamp_9 is unrelated"),
    "0x4e6030": ("bn", "stores a limit global and commits it -> SoundSys_SetFrequency; SoundSys_SetMasterVolume_4 is unrelated"),
    "0x4e7530": ("bn", "returns a status-screen event; Screen_OnEvent is unrelated"),
    "0x4e76e0": ("bn", "String_VFormat then Console_AppendText -> Console_VPrint; SoundSys_* is unrelated"),
    "0x4e7710": ("bn", "appends formatted text to the console buffer -> Console_AppendText"),
    "0x4e7c00": ("bn", "prints to console -> Console_Print; SoundSys_SetMasterVolume_11 is unrelated"),
    "0x460c80": ("bn", "gated debug printf using Console_Print -> Human_DebugLog; AI_CheckHearing_18 is unrelated"),
    "0x463150": ("bn", "tests the active-player flag -> Human_IsActivePlayer; AI_CheckHearing_19 is unrelated"),
    "0x4b7030": ("bn", "frees a heap entry; Sys_PushStack_18 is unrelated"),
    "0x4b12f0": ("bn", "registers a script namespace; Sys_FreeMemory_4 is unrelated"),
    "0x4b22f0": ("bn", "clears a 36-byte struct; Sys_CheckHeap_7 is unrelated"),
    "0x4c1800": ("bn", "allocates an 8-byte-header int array -> Mem_Alloc8; DynObj_LookupHashItem_4 is unrelated"),
    "0x4d7020": ("bn", "returns a bone node by index; Render_SkinBones_11 is unrelated"),
    "0x4f2060": ("bn", "indexed table lookup -> Table_GetAt; Script_ParseEnumString_12 is unrelated"),
    "0x475df0": ("bn", "arms a mine (proxy_arm); Mine_Update is the caller-side label"),
    "0x4798c0": ("bn", "gun reload path; Gun_MagFill is unrelated"),
    "0x477d10": ("gn", "ejects the magazine; Weapon_EjectMagazine_5 matches the body"),
    "0x47c3d0": ("gn", "gunshot processing/cleanup; GunShot_Process matches the body"),
    "0x4404c0": ("gn", "ladder update; Ladder_Update matches the body"),
    "0x401530": ("gn", "links a QTask event handler with no context; QTask_TriggerEvent is the retail role"),
    "0x4012a0": ("gn", "handles QTask input; QTask_HandleInput matches the retail role"),
    "0x4109a0": ("gn", "movement debug strings ('Delta move pos X/Y/Z') name the human run update"),
    "0x410f70": ("gn", "stamina update; HumanPlayer_UpdateStamina matches the body"),
    "0x410370": ("gn", "loads human player parameters; HumanPlayer_LoadParameters matches the body"),
    "0x4108c0": ("gn", "human player process; HumanPlayer_Invoke8 is a placeholder label"),
    "0x45de20": ("gn", "human player input setup; HumanPlayerInput_Setup matches the block"),
    "0x45df50": ("gn", "human player input process; HumanPlayerInput_Process matches the block"),
    "0x45e060": ("gn", "AI check sector; AI_CheckSector_16 matches the block"),
    "0x4415c0": ("gn", "camera cleanup; SCamera_Cleanup matches the block"),
    "0x4a5daa": ("bn", "decompiled body is the MSVC CRT __abnormal_termination (compares ExceptionList->Handler against the math exception handler); MathSys_ProcessStep is a wrong label"),
    "0x4a7fa0": ("bn", "decompiled body is the MSVC CRT __allmul 64-bit multiply helper; MathSys_Invoke12 is a wrong label"),
}


def main() -> None:
    ev = {e["address"].lower(): e
          for e in json.load(open(ROOT / "assets" / "gamenatives_divergence_evidence.json"))}
    rows = []
    for addr, (winner, why) in VERDICTS.items():
        e = ev.get(addr)
        if not e:
            print(f"WARNING: {addr} not in divergence evidence")
            continue
        rows.append({
            "address": e["address"],
            "gnName": e["gnNames"][0],
            "bnName": e["bnName"],
            "winner": winner,
            "correctName": e["bnName"].strip('"') if winner == "bn" else e["gnNames"][0],
            "evidence": why,
            "strings": e["strings"],
        })

    covered = {r["address"].lower() for r in rows}
    unresolved = [e for a, e in ev.items() if a not in covered]
    dest = ROOT / "assets" / "gamenatives_verdicts.json"
    dest.write_text(json.dumps({"adjudicated": rows,
                                "unresolved": [{"address": e["address"],
                                                "gnName": e["gnNames"][0],
                                                "bnName": e["bnName"]}
                                               for e in unresolved]}, indent=2))

    to_bn = [r for r in rows if r["winner"] == "gn"]
    to_gn = [r for r in rows if r["winner"] == "bn"]
    print(f"adjudicated : {len(rows)}  (bn wins {len(to_gn)}, gn wins {len(to_bn)})")
    print(f"unresolved  : {len(unresolved)} {[e['address'] for e in unresolved]}")
    print(f"written     : {dest}")


if __name__ == "__main__":
    main()
