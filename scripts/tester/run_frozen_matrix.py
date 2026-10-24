#!/usr/bin/env python3
"""Run the paired retail-first / candidate-second Frida matrix for one frozen candidate.

Every hook spec is (retail VA, probe name, candidate VA). Both sides use the SAME
probe file (probe_<retailVA>.js) and the SAME driver layout, so the resulting
reports are directly comparable. Candidate runs are refused by the driver when the
deployed executable differs from --sha.

Usage: run_frozen_matrix.py --sha <hex> --run-dir build/tester/<run> [--only G,M]
"""
from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GAME_DIR = Path("/Users/haseeb-mir/Downloads/IGI")
RETAIL = GAME_DIR / "igi.exe"
CANDIDATE = GAME_DIR / "igi1.exe"

# Results from build/tester/run.../runtime; retail VA -> candidate VA resolved by nm on
# the frozen build (16f151a8). Keep this map in lock-step with --sha.
CAND = {
    0x0048F0C0: 0x00416C80, 0x0048F0D0: 0x00416CA0, 0x0048F0F0: 0x00417D10,
    0x0048F100: 0x00416470, 0x0048F120: 0x00416A00, 0x0048F130: 0x00416A10,
    0x0048F140: 0x00417B70, 0x0048F1A0: 0x004163B0, 0x0048F1C0: 0x004164A0,
    0x0048F1E0: 0x004163D0, 0x0048F200: 0x00416C70, 0x0048F210: 0x00416A50,
    0x0048F240: 0x004163E0, 0x0048F260: 0x00416400, 0x0048F280: 0x00416A20,
    0x0048F290: 0x00416A30, 0x0048F2A0: 0x00416A40, 0x0048F2B0: 0x00416A60,
    0x0048F300: 0x00416A70, 0x0048F350: 0x00416990, 0x0048F360: 0x00416650,
    0x0048F920: 0x004169A0, 0x0048F930: 0x004169B0, 0x0048F950: 0x004169D0,
    0x0048F960: 0x004169E0, 0x004A5A0C: 0x004164C0, 0x004C1790: 0x00417F50,
    0x004C17C0: 0x00417FA0, 0x004C1800: 0x00417F20, 0x00401000: 0x00417EB0,
    0x00402930: 0x00595B58,
}

def hook(va: int, name: str, candidate_side: bool) -> list[str]:
    probe = ROOT / "build" / "tester" / RUN_NAME / "probes" / f"probe_0x{va:08X}.js"
    if candidate_side:
        return ["--hook", f"{probe}:0x{va:08X}:{name}:0x{CAND[va]:08X}"]
    return ["--hook", f"{probe}:0x{va:08X}:{name}"]

SCENARIOS = [
    # id, retail args words, mem-arg, return-mem, max calls, game args, hooks
    ("globals", 1, "0:0:32", "0:32", 1024, [],
     [(0x0048F280, "Input_SetUseWin32Input_48F280"), (0x0048F290, "AppMain_Set5C8C0C_48F290"),
      (0x0048F2A0, "AppMain_Set5C8C10_48F2A0"), (0x0048F210, "Game_SetStartupMode_48F210"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0"), (0x0048F300, "AppContext_SetLanguage_48F300"),
      (0x0048F200, "AppMain_Set5C8C04_48F200"), (0x0048F130, "Display_GetScreenMode_48F130"),
      (0x0048F120, "Display_SetScreenMode_48F120")]),
    ("methods", 1, "0:0:32", "0:32", 64, [],
     [(0x0048F0C0, "StoreCommandLine"), (0x0048F0D0, "StoreInstanceHandle"),
      (0x0048F0F0, "SetConfigFileName"), (0x0048F100, "SetAppTitle_48F100"),
      (0x0048F140, "AppContext_SetIsActive"), (0x0048F1A0, "App_ContextSetDebugged"),
      (0x0048F1C0, "SetDebugCompileState_48F1C0"), (0x0048F1E0, "App_ContextSetDebugtextState"),
      (0x0048F240, "App_ContextSetLightmapsUsed"), (0x0048F260, "AppContext_SetTerrainLightmapsUsed")]),
    ("parser", 1, "0:0:32", "0:32", 256, [],
     [(0x0048F360, "AppMain_ParseCmdLineArgs_48F360"), (0x004A5A0C, "AppMain_TokenizeDelimited_4A5A0C"),
      (0x0048F120, "Display_SetScreenMode_48F120"), (0x0048F280, "Input_SetUseWin32Input_48F280"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0"), (0x0048F300, "AppContext_SetLanguage_48F300")]),
    ("trio", 2, "0:0:8", "0:8", 2048, [],
     [(0x004C1800, "QTaskList_New"), (0x004C1790, "QTaskList_Register"), (0x004C17C0, "QTaskList_Remove")]),
    ("window", 1, "0:0:16", "0:32", 64, [],
     [(0x0048F350, "AppMain_WindowHandler_48F350"), (0x0048F120, "Display_SetScreenMode_48F120"),
      (0x0048F280, "Input_SetUseWin32Input_48F280"), (0x0048F300, "AppContext_SetLanguage_48F300"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0")]),
    ("germany", 1, "0:0:16", "0:32", 64, ["Germany"],
     [(0x0048F350, "AppMain_WindowHandler_48F350"), (0x0048F920, "AppMain_WinInputHandler_48F920"),
      (0x0048F930, "AppMain_GermanyHandler_48F930"), (0x0048F950, "AppMain_USAHandler_48F950"),
      (0x0048F280, "Input_SetUseWin32Input_48F280"), (0x0048F300, "AppContext_SetLanguage_48F300"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0")]),
    ("wininput", 1, "0:0:16", "0:32", 64, ["WinInput"],
     [(0x0048F350, "AppMain_WindowHandler_48F350"), (0x0048F920, "AppMain_WinInputHandler_48F920"),
      (0x0048F930, "AppMain_GermanyHandler_48F930"), (0x0048F950, "AppMain_USAHandler_48F950"),
      (0x0048F280, "Input_SetUseWin32Input_48F280"), (0x0048F300, "AppContext_SetLanguage_48F300"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0")]),
    ("usa", 1, "0:0:16", "0:32", 64, ["USA"],
     [(0x0048F350, "AppMain_WindowHandler_48F350"), (0x0048F920, "AppMain_WinInputHandler_48F920"),
      (0x0048F930, "AppMain_GermanyHandler_48F930"), (0x0048F950, "AppMain_USAHandler_48F950"),
      (0x0048F280, "Input_SetUseWin32Input_48F280"), (0x0048F300, "AppContext_SetLanguage_48F300"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0")]),
    ("hardcoregerman", 1, "0:0:16", "0:32", 64, ["HardcoreGerman"],
     [(0x0048F960, "AppMain_HardcoreGermanHandler_48F960"),
      (0x0048F2B0, "AppContext_SetGermanVersion_48F2B0")]),
    ("qtask", 0, None, None, 64, [],
     [(0x00401000, "QTask_CreateTask"), (0x00402930, "QTask_RegisterClasses")]),
]


def run_one(label: str, exe: Path, scenario, expect_sha: str | None, observe: float,
            run_dir: Path) -> None:
    candidate_side = label == "candidate"
    sid, args_words, mem_arg, return_mem, max_calls, game_args, hooks = scenario
    out = run_dir / "runtime" / f"{label}-{sid}.json"
    raw = out.with_suffix(".jsonl")
    if out.is_file():
        print(f"skip existing {out.name}")
        return
    cmd = [sys.executable, str(ROOT / "scripts/tester/frida_probe_driver.py"),
           "--exe", str(exe), "--level", "1", "--observe", str(observe), "--mode", "spawn",
           "--max-calls", str(max_calls), "--arg-words", str(args_words)]
    if mem_arg:
        cmd += ["--mem-arg", mem_arg]
    if return_mem:
        cmd += ["--return-mem", return_mem]
    if expect_sha:
        cmd += ["--expect-exe-sha256", expect_sha]
    for token in game_args:
        cmd += ["--game-arg", token]
    for va, name in hooks:
        cmd += hook(va, name, candidate_side)
    cmd += ["--out", str(out), "--raw", str(raw)]
    started = time.time()
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=240)
    elapsed = round(time.time() - started, 1)
    print(f"[{label}-{sid}] rc={proc.returncode} {elapsed}s :: {(proc.stdout or '').strip().splitlines()[-1] if proc.stdout else ''}"
          + (f" :: STDERR {proc.stderr.strip()[:200]}" if proc.returncode else ""))
    if proc.returncode != 0:
        raise SystemExit(f"capture failed for {label}-{sid}")


def main() -> int:
    global RUN_NAME
    ap = argparse.ArgumentParser()
    ap.add_argument("--sha", required=True)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--only", default="")
    args = ap.parse_args()
    RUN_NAME = args.run_dir.name
    wanted = {item.strip() for item in args.only.split(",") if item.strip()}
    run_dir = args.run_dir.resolve()
    (run_dir / "runtime").mkdir(parents=True, exist_ok=True)
    for scenario in SCENARIOS:
        if wanted and scenario[0] not in wanted:
            continue
        run_one("retail", RETAIL, scenario, None, 10.0, run_dir)
        run_one("candidate", CANDIDATE, scenario, args.sha, 15.0, run_dir)
    print("matrix complete")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
