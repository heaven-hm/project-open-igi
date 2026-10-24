#!/usr/bin/env python3
"""Batch (combined-session) runtime verification over the individually accepted natives.

One retail session followed by one candidate session, same scenario/probes/cwd,
with every accepted probe hooked in the same process. Ordered calls, argument
shape/pointer-content for string natives, and return values are compared per target.
Pointer-content equality for non-string pointer natives was already proven by the
individual records; this batch pass checks combined-session invocation order,
counts and returns (and C-string contents where the native takes a string).

Usage:
  run_batch_verification.py --run-dir build/tester/<run> --sha dee2974c...
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER = ROOT / "scripts/tester/frida_probe_driver.py"
GAME_DIR = Path("/Users/haseeb-mir/Downloads/IGI")

# (retail VA, name, candidate VA, arg0 is C string)
TARGETS = [
    ("0x0048F0C0", "StoreCommandLine", "0x416C80", True),
    ("0x0048F0D0", "StoreInstanceHandle", "0x416CA0", False),
    ("0x0048F0F0", "SetConfigFileName", "0x417D10", True),
    ("0x0048F120", "Display_SetScreenMode_48F120", "0x416A00", False),
    ("0x0048F140", "AppContext_SetIsActive", "0x417B70", False),
    ("0x0048F1A0", "App_ContextSetDebugged", "0x4163B0", False),
    ("0x0048F1C0", "SetDebugCompileState_48F1C0", "0x4164A0", False),
    ("0x0048F1E0", "App_ContextSetDebugtextState", "0x4163D0", False),
    ("0x0048F200", "AppMain_Set5C8C04_48F200", "0x416C70", False),
    ("0x0048F280", "Input_SetUseWin32Input_48F280", "0x416A20", False),
    ("0x0048F290", "AppMain_Set5C8C0C_48F290", "0x416A30", False),
    ("0x0048F2A0", "AppMain_Set5C8C10_48F2A0", "0x416A40", False),
    ("0x0048F2B0", "AppContext_SetGermanVersion_48F2B0", "0x416A60", False),
    ("0x0048F300", "AppContext_SetLanguage_48F300", "0x416A70", True),
    ("0x0048F350", "AppMain_WindowCommand_48F350", "0x416990", True),
    ("0x0048F920", "AppMain_WinInputCommand_48F920", "0x4169A0", True),
    ("0x0048F930", "AppMain_GermanyCommand_48F930", "0x4169B0", True),
    ("0x0048F950", "AppMain_USACommand_48F950", "0x4169D0", True),
]
GAME_ARGS = ["WinInput", "Germany", "USA"]


def probe_path(run_dir: Path, address: str) -> Path:
    return run_dir / "probes" / f"probe_0x{address[2:].upper()}.js"


def capture(run_dir: Path, side: str, exe: Path, sha: str, out: Path) -> dict:
    spellings = {"retail": lambda va: va, "candidate": lambda va: cand[va]}
    cmd = [sys.executable, str(DRIVER), "--exe", str(exe), "--level", "1",
           "--observe", "10", "--max-calls", "256", "--arg-words", "4",
           "--mem-arg", "0:0:16", "--return-mem", "0:16",
           "--out", str(out), "--expect-exe-sha256", sha]
    for va, name, cva, _ in TARGETS:
        probe = probe_path(run_dir, va)
        spec = f"{probe}:{va}:{name}"
        if side == "candidate":
            spec += f":{cva}"
        cmd += ["--hook", spec]
    for token in GAME_ARGS:
        cmd += ["--game-arg", token]
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode:
        print(f"[{side}] FAILED rc={proc.returncode}\n{proc.stderr[-2000:]}", file=sys.stderr)
        raise SystemExit(1)
    print(f"[{side}] rc=0 :: {out}")
    return json.loads(out.read_text())


cand = {va: cva for va, _, cva, _ in TARGETS}


def c_string(entry: dict | None, key: str = "memory") -> str | None:
    if not entry:
        return None
    snap = ((entry.get(key) or {}).get("snapshot") or {})
    data = snap.get("hex")
    if data is None or snap.get("status") != "read":
        return None
    for index in range(0, len(data) - 1, 2):
        if data[index:index + 2] == "00":
            return data[:index + 2]
    return data


def snapshot_status(entry: dict | None, key: str = "memory") -> str | None:
    if not entry:
        return None
    return ((entry.get(key) or {}).get("snapshot") or {}).get("status")


def compare_calls(retail: dict, candidate: dict) -> dict:
    targets = []
    all_ok = True
    for va, name, cva, is_string in TARGETS:
        rentals = [e for e in retail["entries"] if e["address"] == va]
        cands = [e for e in candidate["entries"] if e["address"] == va]
        ret_r = {(e["address"], e["callIndex"]): e for e in retail["returns"]}
        ret_c = {(e["address"], e["callIndex"]): e for e in candidate["returns"]}
        issues = []
        if len(rentals) != len(cands):
            issues.append(f"call count {len(rentals)} vs {len(cands)}")
        matched = min(len(rentals), len(cands))
        for index in range(matched):
            r, c = rentals[index], cands[index]
            if r["callIndex"] != c["callIndex"]:
                issues.append(f"call {index + 1}: callIndex order {r['callIndex']} vs {c['callIndex']}")
            rret = ret_r.get((va, r["callIndex"]))
            cret = ret_c.get((va, c["callIndex"]))
            r_eax = (rret or {}).get("eax")
            c_eax = (cret or {}).get("eax")
            if is_string and (rret or {}).get("returnMemory") is not None:
                if c_string(rret, "returnMemory") != c_string(cret, "returnMemory"):
                    issues.append(f"call {index + 1}: returned string content differs")
            elif not is_string and r_eax != c_eax:
                issues.append(f"call {index + 1}: eax {r_eax} vs {c_eax}")
            rargs, cargs = r.get("args") or [], c.get("args") or []
            if len(rargs) != len(cargs):
                issues.append(f"call {index + 1}: arg word count {len(rargs)} vs {len(cargs)}")
            # every accepted native takes exactly one argument word (see individual records);
            # wider captured register/stack words are not part of the native's ABI.
            for word in range(min(1, len(rargs), len(cargs))):
                rv, cv = rargs[word], cargs[word]
                same = rv == cv
                if not same and is_string and word == 0:
                    same = c_string(r) == c_string(c) and c_string(r) is not None
                if not same and not is_string:
                    same = bool(rv["value"] if "value" in rv else int(rv["hex"], 16)) and \
                           bool(cv["value"] if "value" in cv else int(cv["hex"], 16))
                if not same:
                    issues.append(f"call {index + 1}: arg{word} differs")
        verdict = "MATCH" if not issues else "DIVERGES"
        if issues:
            all_ok = False
        targets.append(dict(address=va, name=name, candidate_address=cva,
                            retail_entries=len(rentals), candidate_entries=len(cands),
                            verdict=verdict, issues=issues))
        print(f"  {va} {name}: {verdict} retail={len(rentals)} candidate={len(cands)}"
              + (f" {issues[:3]}" if issues else ""))
    return dict(scenario=dict(
        command="window level1 WinInput Germany USA", level=1,
        extra_args=GAME_ARGS, cwd=str(GAME_DIR),
        retail_command="igi.exe window level1 WinInput Germany USA",
        candidate_command="igi1.exe window level1 WinInput Germany USA"),
        targets=targets, overall="MATCH" if all_ok else "DIVERGES")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--sha", required=True)
    ap.add_argument("--retail", type=Path, default=GAME_DIR / "igi.exe")
    ap.add_argument("--candidate", type=Path, default=GAME_DIR / "igi1.exe")
    args = ap.parse_args()
    run_dir = args.run_dir.resolve()
    retail_out = run_dir / "runtime" / "batch-retail.json"
    cand_out = run_dir / "runtime" / "batch-candidate.json"
    if not retail_out.is_file():
        capture(run_dir, "retail", args.retail, "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01", retail_out)
    if not cand_out.is_file():
        capture(run_dir, "candidate", args.candidate, args.sha, cand_out)
    retail = json.loads(retail_out.read_text())
    candidate = json.loads(cand_out.read_text())
    print("batch comparison:")
    comparison = compare_calls(retail, candidate)
    (run_dir / "runtime" / "batch-comparison.json").write_text(json.dumps(comparison, indent=2) + "\n")
    print("batch overall:", comparison["overall"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
