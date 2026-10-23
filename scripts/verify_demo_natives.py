#!/usr/bin/env python3
"""Verification runner for Project Open-IGI core demo native methods.

Covers the essential engine demo subsystems:
  - 3D Models & MEF
  - MTP (Model Texture Package)
  - QVM (Quake Virtual Machine / Script execution)
  - RES (Resource management & cache)
  - config.qvm (Retail game configuration & display options)
  - humanplayer.qvm (Player entity, movement, inputs & camera)
  - Terrain (Heightmap, tilemap, LOD quadtree, contour)
  - TEX (Texture loaders, mipmap upload, palettes, modifiers)
  - CTR / Camera (Camera transforms, viewblock, aspect ratio)
  - ILFF & IFF (Chunk navigation, container headers, chunk payload)
  - Flow Engine (Game init, per-frame dispatch, loop status, flow state)

Verifies each native against:
  1. angr CFGFast exact entry recovery on retail igi.exe
  2. Decompiler / Radare2 CFG similarity (>= 0.85)
  3. Manifest status & implementation source validity via scripts/native_gate.py
"""

from __future__ import annotations

import argparse
from collections import Counter
import json
import os
import pathlib
import subprocess
import sys
import tempfile
import time
from typing import Any

ROOT = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_BINARY = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
DEFAULT_MANIFEST = ROOT / "docs/fidelity/native-verification.json"
ANGR_HELPER = ROOT / "scripts/angr_analyze.py"
NATIVE_GATE = ROOT / "scripts/native_gate.py"

TARGET_METHODS: list[dict[str, str]] = [
    {"va": "0x0048F530", "name": "Main", "category": "Flow Engine"},
    {"va": "0x00401000", "name": "QTask_CreateTask", "category": "Flow Engine"},
    {"va": "0x00401050", "name": "QTask_OpenEvent", "category": "Flow Engine"},
    {"va": "0x00402930", "name": "QTask_RegisterClasses", "category": "Flow Engine"},
    {"va": "0x00402800", "name": "Flow_CreateHandler", "category": "Flow Engine"},
    {"va": "0x00402260", "name": "Flow_RunHandler", "category": "Flow Engine"},
    {"va": "0x00402880", "name": "Game_GetContext", "category": "Flow Engine"},
    {"va": "0x004028A0", "name": "Flow_SetState", "category": "Flow Engine"},
    {"va": "0x0048F480", "name": "Loop_SetStatusByte", "category": "Flow Engine"},
    {"va": "0x00404590", "name": "Config_GetActiveGraphicOptions", "category": "config.qvm"},
    {"va": "0x00404F00", "name": "Config_GraphicOptionsSetPerformanceFlags", "category": "config.qvm"},
    {"va": "0x00404F60", "name": "Config_GraphicOptionsGetPerformanceFlags", "category": "config.qvm"},
    {"va": "0x00405530", "name": "Config_GraphicOptionsGetPerfLevelFromFlags", "category": "config.qvm"},
    {"va": "0x004055C0", "name": "Config_GraphicOptionsGetPerfFlagsFromLevel", "category": "config.qvm"},
    {"va": "0x00405620", "name": "Config_GraphicOptionsNumPerfLevels", "category": "config.qvm"},
    {"va": "0x00405850", "name": "Config_Read", "category": "config.qvm"},
    {"va": "0x004037C0", "name": "GOInRemap", "category": "humanplayer.qvm"},
    {"va": "0x00410070", "name": "DefineHumanPlayerGeneral", "category": "humanplayer.qvm"},
    {"va": "0x004137A0", "name": "HumanPlayer_LoadParameters", "category": "humanplayer.qvm"},
    {"va": "0x00463180", "name": "Human_IsCrouched", "category": "humanplayer.qvm"},
    {"va": "0x00465820", "name": "Human_CameraEyePosition", "category": "humanplayer.qvm"},
    {"va": "0x004659E0", "name": "Human_TaskViewReset", "category": "humanplayer.qvm"},
    {"va": "0x004ED5C0", "name": "InputPort_Process", "category": "humanplayer.qvm"},
    {"va": "0x0048F1D0", "name": "Script_GetDebugCompileState", "category": "QVM"},
    {"va": "0x0048F340", "name": "Script_GetForcePrecompiledState", "category": "QVM"},
    {"va": "0x004B1E90", "name": "String_FindExtension", "category": "QVM"},
    {"va": "0x004B5B60", "name": "Resource_FileExists", "category": "QVM"},
    {"va": "0x004B7E10", "name": "Qvm_ReportError", "category": "QVM"},
    {"va": "0x004B80B0", "name": "Qvm_Load", "category": "QVM"},
    {"va": "0x004B83D0", "name": "Qvm_Cleanup", "category": "QVM"},
    {"va": "0x004B8A10", "name": "Qvm_SetGlobalState", "category": "QVM"},
    {"va": "0x004BCFD0", "name": "Qvm_ExecuteInstructions", "category": "QVM"},
    {"va": "0x004BD3D0", "name": "Qvm_SetOpcodeHandler", "category": "QVM"},
    {"va": "0x004BDA00", "name": "Qvm_PushInteger", "category": "QVM"},
    {"va": "0x00491D40", "name": "Display_ComputeScreenAspect", "category": "CTR / Camera"},
    {"va": "0x00497D70", "name": "Transcontext_BuildViewBlock", "category": "CTR / Camera"},
    {"va": "0x004A2600", "name": "Camera_ResetMatrix", "category": "CTR / Camera"},
    {"va": "0x004B7450", "name": "Matrix_InvalidateSlot", "category": "CTR / Camera"},
    {"va": "0x004B7470", "name": "Ilff_GetNextChunk", "category": "ILFF & IFF"},
    {"va": "0x004B7490", "name": "Ilff_GetFirstChunk", "category": "ILFF & IFF"},
    {"va": "0x004B74D0", "name": "Ilff_GetChunkPayload", "category": "ILFF & IFF"},
    {"va": "0x004B7540", "name": "Ilff_ReadContainerHeader", "category": "ILFF & IFF"},
    {"va": "0x004B7570", "name": "Ilff_GetFormType", "category": "ILFF & IFF"},
    {"va": "0x004B7580", "name": "Ilff_GetChunkTag", "category": "ILFF & IFF"},
    {"va": "0x004B7590", "name": "Ilff_FindChunk", "category": "ILFF & IFF"},
    {"va": "0x004CEAA0", "name": "Ilff_FindFormChunk", "category": "ILFF & IFF"},
    {"va": "0x004AAD10", "name": "Resource_MatchPrefix", "category": "RES"},
    {"va": "0x004AD73D", "name": "String_ToUpperChar", "category": "RES"},
    {"va": "0x004B0D40", "name": "QHash_ValueSet", "category": "RES"},
    {"va": "0x004B1690", "name": "Device_CloseStream", "category": "RES"},
    {"va": "0x004B1780", "name": "Device_GetStreamPosition", "category": "RES"},
    {"va": "0x004B17F0", "name": "Device_SeekStream", "category": "RES"},
    {"va": "0x004B59F0", "name": "Resource_CreateNode", "category": "RES"},
    {"va": "0x004B5AF0", "name": "Resource_CanonicalizePath", "category": "RES"},
    {"va": "0x004B5B30", "name": "Resource_CalculatePathHash", "category": "RES"},
    {"va": "0x004B5C90", "name": "Resource_FreeNode", "category": "RES"},
    {"va": "0x004B5F00", "name": "Resource_Load", "category": "RES"},
    {"va": "0x004B5F80", "name": "Resource_RegisterBuffer", "category": "RES"},
    {"va": "0x004B60F0", "name": "Resource_RegisterIlffContainer", "category": "RES"},
    {"va": "0x004B6380", "name": "Resource_Unload", "category": "RES"},
    {"va": "0x004CF300", "name": "DynObj_LookupHashItem", "category": "3D Models & MEF"},
    {"va": "0x004CF330", "name": "DynObj_LoadPackageMeshes", "category": "3D Models & MEF"},
    {"va": "0x004CF390", "name": "DynObj_LoadModelMesh", "category": "3D Models & MEF"},
    {"va": "0x004CF630", "name": "DynObj_LoadPackageLovs", "category": "3D Models & MEF"},
    {"va": "0x004CF910", "name": "DynObj_LoadPackageAnimations", "category": "3D Models & MEF"},
    {"va": "0x004D0CF0", "name": "Model_GetAttachment", "category": "3D Models & MEF"},
    {"va": "0x004D0D10", "name": "Model_GetAttachmentCount", "category": "3D Models & MEF"},
    {"va": "0x004EDB60", "name": "LODSettings_LoadParameters", "category": "3D Models & MEF"},
    {"va": "0x004CF080", "name": "DynObj_LoadModelTexturePackage", "category": "MTP"},
    {"va": "0x004A26C0", "name": "Camera_UpdateFogPalette", "category": "TEX"},
    {"va": "0x004B0F80", "name": "Texture_GetActiveBackendId", "category": "TEX"},
    {"va": "0x004B23B0", "name": "Texture_UploadMipmapPacket", "category": "TEX"},
    {"va": "0x004B24C0", "name": "Texture_LoadLoopTextureFile", "category": "TEX"},
    {"va": "0x004B26A0", "name": "Texture_EnableUpload", "category": "TEX"},
    {"va": "0x004B26B0", "name": "Texture_DisableUpload", "category": "TEX"},
    {"va": "0x004B7CF0", "name": "Texture_UploadPalettePacket", "category": "TEX"},
    {"va": "0x004CF790", "name": "DynObj_LoadModelTextures", "category": "TEX"},
    {"va": "0x004CF870", "name": "DynObj_LoadPackagePalettes", "category": "TEX"},
    {"va": "0x00505E40", "name": "TextureModifier_Push", "category": "TEX"},
    {"va": "0x004C5F00", "name": "Terrain_LoadTexturesAndLightmaps", "category": "Terrain"},
    {"va": "0x004C6240", "name": "Terrain_Load", "category": "Terrain"},
    {"va": "0x004C6670", "name": "Terrain_CreateMaterial", "category": "Terrain"},
    {"va": "0x004C6760", "name": "Terrain_CreateTileMap", "category": "Terrain"},
    {"va": "0x004CEA80", "name": "Render_GetActiveBackendId", "category": "Terrain"},
    {"va": "0x004D14E0", "name": "Terrain_TraverseContourQuadtree", "category": "Terrain"},
    {"va": "0x004F82B0", "name": "TerrainLightMap_PushEntry", "category": "Terrain"},
    {"va": "0x00506F00", "name": "HeightMap_Enter", "category": "Terrain"},
    {"va": "0x00510010", "name": "Terrain_DecodeChunkCommands", "category": "Terrain"},
    {"va": "0x00512C30", "name": "Terrain_Draw", "category": "Terrain"},
    {"va": "0x005175A0", "name": "Terrain_LoadTexturePackage", "category": "Terrain"},
    {"va": "0x004A53B3", "name": "sprintf", "category": "Memory & System"},
    {"va": "0x004A5702", "name": "String_VFormat", "category": "Memory & System"},
    {"va": "0x004AA670", "name": "Mem_AllocChunk", "category": "Memory & System"},
    {"va": "0x004AA682", "name": "Mem_AllocWithRetry", "category": "Memory & System"},
    {"va": "0x004AA6AE", "name": "Mem_AllocateRawBlock", "category": "Memory & System"},
    {"va": "0x004AD722", "name": "Mem_InvokeNewHandler", "category": "Memory & System"},
    {"va": "0x004AF7B0", "name": "Error_Show", "category": "Memory & System"},
    {"va": "0x004AF8F0", "name": "HeapList_Insert", "category": "Memory & System"},
    {"va": "0x004AF960", "name": "HeapList_Remove", "category": "Memory & System"},
    {"va": "0x004B0BA0", "name": "Mem_InitHeapState", "category": "Memory & System"},
    {"va": "0x004B0C60", "name": "Mem_Alloc", "category": "Memory & System"},
    {"va": "0x0048F140", "name": "AppContext_SetIsActive", "category": "Flow Engine"},
    {"va": "0x0048F170", "name": "AppContext_GetIsActive", "category": "Flow Engine"},
    {"va": "0x0048F1B0", "name": "App_ContextGetDebugged", "category": "Flow Engine"},
    {"va": "0x0048F230", "name": "App_ContextGetLightmapsUsed", "category": "Flow Engine"},
    {"va": "0x0048F250", "name": "App_ContextGetTerrainLightmapsUsed", "category": "Flow Engine"},
    {"va": "0x0048F260", "name": "AppContext_SetTerrainLightmapsUsed", "category": "Flow Engine"},
    {"va": "0x00498460", "name": "FP_InitExtendedPrecision", "category": "Flow Engine"},
    {"va": "0x00497E70", "name": "TransContext_SetActiveTransContext", "category": "CTR / Camera"},
    {"va": "0x004A7157", "name": "FP_SetDefaultPrecision", "category": "CTR / Camera"},
    {"va": "0x004AC620", "name": "FP_MergeControlWord", "category": "CTR / Camera"},
    {"va": "0x004AC655", "name": "FP_MergeControlWordNoDenormal", "category": "CTR / Camera"},
    {"va": "0x004AC66B", "name": "FP_ControlWordToFlags", "category": "CTR / Camera"},
    {"va": "0x004AC6FD", "name": "FP_FlagsToControlWord", "category": "CTR / Camera"},
    {"va": "0x004F1160", "name": "Script_GetParseMode", "category": "QVM"},
    {"va": "0x004D67A0", "name": "AnimIff_LoadAnimationFile", "category": "MTP"},
    {"va": "0x00517890", "name": "Iff_FindFormChunk", "category": "MTP"},
    {"va": "0x005178E0", "name": "Iff_FindChunkBelowForm", "category": "MTP"},
    {"va": "0x00517940", "name": "Iff_FindChunk", "category": "MTP"},
    {"va": "0x005179C0", "name": "Iff_GetChunkPayload", "category": "MTP"},
]


def load_manifest_records(manifest_path: pathlib.Path) -> dict[int, dict[str, Any]]:
    if not manifest_path.is_file():
        return {}
    try:
        with manifest_path.open("r", encoding="utf-8") as f:
            data = json.load(f)
        natives = data.get("natives", [])
        return {int(n["address"], 16): n for n in natives if "address" in n}
    except Exception as err:
        print(f"Warning: could not read manifest {manifest_path}: {err}", file=sys.stderr)
        return {}


def run_angr_batch(binary: pathlib.Path, addresses: list[str]) -> dict[str, Any]:
    """Run angr CFGFast recovery on retail binary once for all selected addresses."""
    interpreter = os.environ.get("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))
    if not pathlib.Path(interpreter).is_file():
        interpreter = sys.executable

    command = [interpreter, str(ANGR_HELPER), "--binary", str(binary)]
    for addr in addresses:
        command.extend(["--address", addr])

    try:
        proc = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False, timeout=300)
        if proc.stdout.strip():
            return json.loads(proc.stdout)
    except Exception as err:
        return {"error": str(err), "functions": {}}
    return {"functions": {}}


def run_gate_batch(addresses: list[str], threshold: str, cfg_threshold: float, skip_semantic: bool = False, *, report_path: pathlib.Path | None = None) -> dict[str, tuple[bool, str]]:
    """Run native_gate.py in batch mode for multiple addresses using JSON report."""
    if not addresses:
        return {}

    env = dict(os.environ)
    env["IGI_ANGR_PYTHON"] = env.get("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))

    if report_path is None:
        with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tf:
            report_file = tf.name
    else:
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_file = str(report_path)

    cmd = [
        sys.executable,
        str(NATIVE_GATE),
        "--root", str(ROOT),
        # The gate addresses a native inside a file, so the batch selects the
        # tree the demo natives live in.
        "--tree", "engine/src/retail",
        "--threshold", threshold,
        "--cfg-threshold", str(cfg_threshold),
        "--report", report_file
    ]
    strict_acceptance = threshold.upper() in {"VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE"}
    if strict_acceptance:
        cmd.append("--require-behavioral-tests")
        if not skip_semantic:
            cmd.append("--require-semantic-evidence")
    else:
        cmd.append("--allow-structural-only")
    if skip_semantic and not strict_acceptance:
        cmd.append("--skip-semantic-review")

    for addr in addresses:
        cmd.extend(["--address", addr])

    results: dict[str, tuple[bool, str]] = {}
    try:
        proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env, timeout=600)
        pathlib.Path(report_file + ".stdout.log").write_text(proc.stdout)
        pathlib.Path(report_file + ".stderr.log").write_text(proc.stderr)
        report_data = {}
        if os.path.isfile(report_file):
            try:
                with open(report_file, "r", encoding="utf-8") as rf:
                    report_data = json.load(rf)
            except Exception:
                pass

        failures = report_data.get("failures", [])
        checked_natives = {cn.get("address"): cn for cn in report_data.get("checked_natives", [])}

        global_failures = [failure for failure in failures if not any(
            f"0x{int(addr, 16):08X}".lower() in failure.lower() for addr in addresses
        )]
        for addr in addresses:
            norm_addr = f"0x{int(addr, 16):08X}"
            # Check if address had failures
            addr_failures = [f for f in failures if norm_addr.lower() in f.lower() or addr.lower() in f.lower()]
            if norm_addr in checked_natives or addr in checked_natives:
                cn = checked_natives.get(norm_addr) or checked_natives.get(addr)
                errors = [*global_failures, *addr_failures, *cn.get("errors", [])]
                if errors:
                    results[addr] = (False, errors[0])
                elif proc.returncode != 0 and not failures:
                    results[addr] = (False, "Gate exited nonzero without complete error evidence")
                else:
                    results[addr] = (True, "checks passed")
            else:
                if addr_failures:
                    results[addr] = (False, addr_failures[0])
                elif global_failures:
                    results[addr] = (False, global_failures[0])
                elif proc.returncode == 0:
                    results[addr] = (False, "Native missing from gate report; not validated")
                else:
                    results[addr] = (False, f"Not validated (exit code {proc.returncode})")

    except subprocess.TimeoutExpired:
        for addr in addresses:
            results[addr] = (False, "TIMEOUT (600s)")
    except Exception as err:
        for addr in addresses:
            results[addr] = (False, f"EXEC_ERROR: {err}")

    return results


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Verify core demo natives (3D models, MEF, MTP, QVM, RES, config.qvm, humanplayer.qvm, Terrain, TEX, CTR, ILFF, Flow Engine) against retail igi.exe."
    )
    parser.add_argument("--root", type=pathlib.Path, default=ROOT, help="Project root directory.")
    parser.add_argument("--binary", type=pathlib.Path, default=DEFAULT_BINARY, help="Path to retail igi.exe.")
    parser.add_argument("--manifest", type=pathlib.Path, default=DEFAULT_MANIFEST, help="Path to native-verification.json.")
    parser.add_argument("--category", choices=[
        "all", "3d", "mef", "mtp", "qvm", "res", "config", "player", "terrain", "tex", "ctr", "camera", "ilff", "iff", "flow", "mem"
    ], default="all", help="Filter targets by subsystem/category.")
    parser.add_argument("--address", action="append", help="Filter target by specific address (repeatable).")
    parser.add_argument("--threshold", default="VERIFIED", help="Minimum gate status (default: VERIFIED).")
    parser.add_argument("--cfg-threshold", type=float, default=0.85, help="Minimum CFG similarity score (default: 0.85).")
    parser.add_argument("--skip-semantic", action="store_true", help="Skip semantic review check during structural verification.")
    parser.add_argument("--skip-gate", action="store_true", help="Run only angr CFGFast without full gate.")
    parser.add_argument("--report", type=pathlib.Path, help="Save machine-readable JSON report.")
    args = parser.parse_args()

    manifest_records = load_manifest_records(args.manifest)

    # Filter targets
    targets = TARGET_METHODS
    if args.address:
        requested_addrs = {f"0x{int(a, 16):08X}".upper() for a in args.address}
        targets = [t for t in targets if f"0x{int(t['va'], 16):08X}".upper() in requested_addrs]

    if args.category != "all":
        cat_key = args.category.lower()
        filtered = []
        for t in targets:
            c = t["category"].lower()
            if cat_key == "3d" or cat_key == "mef":
                if "model" in c or "mef" in c:
                    filtered.append(t)
            elif cat_key == "mtp" and "mtp" in c:
                filtered.append(t)
            elif cat_key == "qvm" and "qvm" in c:
                filtered.append(t)
            elif cat_key == "res" and "res" in c:
                filtered.append(t)
            elif cat_key == "config" and "config" in c:
                filtered.append(t)
            elif cat_key == "player" and "humanplayer" in c:
                filtered.append(t)
            elif cat_key == "terrain" and "terrain" in c:
                filtered.append(t)
            elif cat_key == "tex" and "tex" in c:
                filtered.append(t)
            elif (cat_key == "ctr" or cat_key == "camera") and "ctr" in c:
                filtered.append(t)
            elif (cat_key == "ilff" or cat_key == "iff") and "ilff" in c:
                filtered.append(t)
            elif cat_key == "flow" and "flow" in c:
                filtered.append(t)
            elif cat_key == "mem" and "mem" in c:
                filtered.append(t)
        targets = filtered

    if not targets:
        print("No target methods selected for verification.")
        return 0

    print(f"================================================================================")
    print(f"Project Open-IGI Demo Natives Verification Runner")
    print(f"Target count: {len(targets)} methods")
    print(f"Threshold:    {args.threshold} (CFG >= {args.cfg_threshold})")
    print(f"Binary:       {args.binary}")
    print(f"================================================================================\n")

    # Step 1: Batch angr CFGFast recovery
    print(f"[*] Step 1/2: Recovering angr CFGFast image entries for {len(targets)} targets...")
    start_angr = time.time()
    angr_data = run_angr_batch(args.binary, [t["va"] for t in targets])
    angr_funcs = {
        key.lower(): value
        for key, value in angr_data.get("functions", {}).items()
    }
    angr_time = time.time() - start_angr
    recovered_angr = sum(
        1 for va in [t["va"] for t in targets]
        if angr_funcs.get(f"0x{int(va, 16):08x}", {}).get("result") == "pass"
    )
    print(f"    angr CFGFast recovered: {recovered_angr}/{len(targets)} function entries in {angr_time:.2f}s\n")

    # Step 2: Gate verification
    print(f"[*] Step 2/2: Verifying CFG similarity and manifest records...")
    manifest_targets = [t for t in targets if int(t["va"], 16) in manifest_records]
    unbound_targets = [t for t in targets if int(t["va"], 16) not in manifest_records]

    gate_results: dict[str, tuple[bool, str]] = {}
    engine_targets = []
    for target in manifest_targets:
        record = manifest_records[int(target["va"], 16)]
        implementation = record.get("implementation", {}).get("path", "")
        if implementation.startswith("engine/src/retail/") and (ROOT / implementation).is_file():
            engine_targets.append(target)
        else:
            gate_results[target["va"]] = (False, "BLOCKED: reference-only or missing engine implementation: " + implementation)
    if not args.skip_gate and engine_targets:
        print(f"    Running batch gate verification for {len(engine_targets)} engine-bound targets...")
        gate_results.update(run_gate_batch(
            [t["va"] for t in engine_targets],
            args.threshold,
            args.cfg_threshold,
            args.skip_semantic,
            report_path=args.report.with_suffix(".gate.json") if args.report else None,
        ))

    results = []
    passed_count = 0
    blocked_count = 0

    for idx, target in enumerate(targets, 1):
        va = target["va"]
        name = target["name"]
        cat = target["category"]
        int_va = int(va, 16)
        manifest_rec = manifest_records.get(int_va)

        addr_key = f"0x{int_va:08x}".lower()
        angr_entry = angr_funcs.get(addr_key, {})
        angr_recovered = angr_entry.get("result") == "pass"
        angr_blocks = angr_entry.get("block_count", 0) if angr_recovered else 0

        manifest_status = manifest_rec.get("verification", {}).get("status", "NO_MANIFEST") if manifest_rec else "NO_MANIFEST"
        cfg_score = manifest_rec.get("verification", {}).get("cfg_similarity", 0.0) if manifest_rec else 0.0

        if args.skip_gate:
            gate_ok = angr_recovered and (cfg_score >= args.cfg_threshold or manifest_status != "NO_MANIFEST")
            summary = "ANGR_CFG_ONLY"
        else:
            if manifest_rec:
                gate_ok, summary = gate_results.get(va, (False, "No gate result"))
                if not angr_recovered:
                    gate_ok = False
                    summary = (
                        "BLOCKED: angr CFGFast did not recover a resolved exact entry: "
                        + str(angr_entry.get("reason", "missing result"))
                    )
            else:
                gate_ok = False
                summary = "BLOCKED: Candidate in GameNatives; unrecorded in manifest"

        if gate_ok:
            passed_count += 1
            status_tag = "PASS"
        else:
            blocked_count += 1
            status_tag = "FAIL"

        result_row = {
            "address": va,
            "name": name,
            "category": cat,
            "angr_recovered": angr_recovered,
            "angr_blocks": angr_blocks,
            "manifest_status": manifest_status,
            "cfg_similarity": cfg_score,
            "gate_passed": gate_ok,
            "gate_summary": summary,
        }
        results.append(result_row)

        print(f"[{idx:2d}/{len(targets):2d}] {va} | {name:<36} | {cat:<15} | CFG:{cfg_score:4.2f} | angr:{'Y' if angr_recovered else 'N'} | {status_tag} ({summary})")

    print(f"\n================================================================================")
    print(f"Verification Results Summary")
    print(f"================================================================================")
    print(f"Total targets:          {len(targets)}")
    status_counts = Counter(row["manifest_status"] for row in results)
    accepted_count = sum(
        status_counts[status]
        for status in ("VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE")
    )
    print(f"Required gate passed:   {passed_count}")
    print(f"Required gate blocked:  {blocked_count}")
    print(f"angr entries recovered: {recovered_angr}/{len(targets)}")
    print(f"Manifest VERIFIED labels: {accepted_count}/{len(targets)}")
    print("Manifest statuses:      " + ", ".join(
        f"{status}={count}" for status, count in sorted(status_counts.items())
    ))
    if accepted_count != len(targets):
        print(
            "NOTE: CFG/angr recovery is structural evidence only; methods below VERIFIED "
            "or without passing behavioral evidence remain blocked."
        )

    if args.report:
        report_data = {
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            "total": len(targets),
            "passed": passed_count,
            "blocked": blocked_count,
            "threshold": args.threshold,
            "cfg_threshold": args.cfg_threshold,
            "accepted": passed_count if not args.skip_gate and args.threshold.upper() in {"VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE"} else 0,
            "manifest_verified_labels": accepted_count,
            "gate_report": str(args.report.with_suffix(".gate.json")) if not args.skip_gate else None,
            "manifest_status_counts": dict(sorted(status_counts.items())),
            "results": results,
        }
        with open(args.report, "w", encoding="utf-8") as f:
            json.dump(report_data, f, indent=2)
        print(f"Saved JSON report to: {args.report}")

    return 0 if blocked_count == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
