#!/usr/bin/env python3
"""Verify the implemented natives: static analysis plus both runtime sides.

The retail game and the reconstruction both take the IGIMUTEX mutex, so they
cannot run at the same time. The verification therefore runs in three passes:

  1. static     - native_gate.py per native against its implementation file
  2. retail     - one Frida session against igi.exe, hooking every retail VA
  3. candidate  - one Frida session against open-igi.exe, hooking every
                   candidate symbol

This legacy script collects reachability diagnostics only. Natural call counts
do not compare behavior and never authorize native acceptance. Use build.sh and
check_frida_differential.py with a static/semantic gate report for acceptance.
"""

import argparse
import ast
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FRIDA = str(Path.home() / ".local" / "bin" / "frida")
FRIDA_HOST = "127.0.0.1:27042"
WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"
GAME_DIR = "/Users/haseeb-mir/Downloads/IGI"
RETAIL_EXE = "Z:\\Users\\haseeb-mir\\Downloads\\IGI\\igi.exe"
CANDIDATE_EXE = "Z:\\Users\\haseeb-mir\\Downloads\\IGI\\open-igi.exe"

PROBE = """
'use strict';
const targets = __TARGETS__;
const counts = {};
const failures = [];
for (const target of targets) {
  counts[target.name] = {va: target.va, hits: 0, first: []};
  try {
    Interceptor.attach(ptr(target.va), {
      onEnter(args) {
        const rec = counts[target.name];
        rec.hits++;
        this.rec = null;
        if (rec.first.length < 2) {
          const words = [];
          for (let i = 0; i < 4; i++) words.push(args[i].toString());
          this.rec = {args: words};
          rec.first.push(this.rec);
        }
      },
      onLeave(retval) { if (this.rec) this.rec.returnValue = retval.toString(); }
    });
  } catch (e) { failures.push({name: target.name, va: target.va, error: String(e)}); }
}
send({type: 'probe_hooks', installed: Object.keys(counts).length, failed: failures.length, failures});
// A repeating snapshot is used rather than a single delayed one, so a snapshot
// is captured even if the agent's event loop is busy.
setInterval(() => {
  const called = {};
  for (const name of Object.keys(counts)) if (counts[name].hits) called[name] = counts[name];
  send({type: 'probe_snapshot', called});
}, 2000);
"""


def extract_payload(line: str):
    marker = "'payload': "
    i = line.find(marker)
    if i < 0:
        return None
    i += len(marker)
    if i >= len(line) or line[i] != "{":
        return None
    depth, in_str, quote = 0, False, ""
    for j in range(i, len(line)):
        c = line[j]
        if in_str:
            if c == quote and line[j - 1] != "\\":
                in_str = False
            continue
        if c in "'\"":
            in_str, quote = True, c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                snippet = line[i:j + 1]
                # the Frida CLI prints Python reprs, so literal_eval comes first
                for loader in (ast.literal_eval, json.loads):
                    try:
                        return loader(snippet)
                    except (ValueError, SyntaxError, json.JSONDecodeError):
                        continue
                return None
    return None


def frida_probe(process_name: str, targets: list[dict], duration: int,
                pid: int | None = None) -> dict:
    if not targets:
        return {"result": "no_targets"}
    source = PROBE.replace("__TARGETS__", json.dumps(targets))
    script = ROOT / "build" / "frida_batch_probe.js"
    script.parent.mkdir(parents=True, exist_ok=True)
    script.write_text(source)
    # a pid is used when one is known: several copies of the game can be
    # running, and the name would then be ambiguous
    selector = ["-p", str(pid)] if pid else ["-n", process_name]
    # the CLI buffers its output, so it writes to a file and exits on its own
    # timer; a killed process would lose the buffered snapshots
    log = ROOT / "build" / f"frida_batch_{process_name}_{pid or 'name'}.log"
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open("w") as handle:
        frida = subprocess.Popen(
            [FRIDA, "-H", FRIDA_HOST, *selector, "-l", str(script),
             "--no-auto-reload", "-q", "-t", str(duration + 5)],
            stdout=handle, stderr=subprocess.STDOUT, text=True)
    try:
        frida.wait(timeout=duration + 40)
    except subprocess.TimeoutExpired:
        frida.kill()
    stdout = log.read_text(errors="replace")
    stderr = ""
    observed, installed, failures = {}, 0, []
    for line in stdout.splitlines():
        payload = extract_payload(line)
        if not payload:
            continue
        if payload.get("type") == "probe_hooks":
            installed = payload.get("installed", 0)
            failures = payload.get("failures", [])
        elif payload.get("type") == "probe_snapshot":
            observed = payload.get("called", {})
    if not observed and not installed:
        return {"process": process_name, "result": "no_output",
                "stdout": stdout[-300:], "stderr": stderr[-300:]}
    return {"process": process_name, "installed": installed, "failures": failures,
            "hits": {k: v.get("hits") for k, v in observed.items()},
            "first": {k: v.get("first", [])[:1] for k, v in observed.items()}}


def find_pid(name: str) -> int | None:
    """The pid of the most recently started copy of the game.

    Several copies can be present, and only the newest one is the instance the
    caller just launched, so the highest pid is used.
    """
    try:
        listing = subprocess.run(
            [str(Path.home() / ".local" / "bin" / "frida-ps"), "-H", FRIDA_HOST],
            capture_output=True, text=True, timeout=60)
    except Exception:
        return None
    pids = []
    for line in listing.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 2 and parts[1].lower() == name.lower():
            try:
                pids.append(int(parts[0]))
            except ValueError:
                continue
    return max(pids) if pids else None


def kill_stale(name: str) -> None:
    """Close any older copies so only the instance under test remains."""
    try:
        listing = subprocess.run(
            [str(Path.home() / ".local" / "bin" / "frida-ps"), "-H", FRIDA_HOST],
            capture_output=True, text=True, timeout=60)
    except Exception:
        return
    pids = []
    for line in listing.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 2 and parts[1].lower() == name.lower():
            try:
                pids.append(int(parts[0]))
            except ValueError:
                continue
    for pid in pids:
        subprocess.run([str(Path.home() / ".local" / "bin" / "frida-kill"),
                        "-H", FRIDA_HOST, str(pid)], capture_output=True)
    if pids:
        time.sleep(2)


def start_game(windows_exe: str, level: int, mutex_name: str | None = None,
               game_directory: Path | None = None) -> subprocess.Popen:
    environment = dict(os.environ)
    if mutex_name is not None:
        environment["IGI_MUTEX_NAME"] = mutex_name
    return subprocess.Popen(
        ["env", "LC_ALL=C", "LANG=C", WINE, "--bottle", BOTTLE, windows_exe,
         "window", f"level{level}"],
        cwd=game_directory or GAME_DIR, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        env=environment)


def stop_game(process: subprocess.Popen) -> None:
    process.terminate()
    try:
        process.wait(timeout=10)
    except subprocess.TimeoutExpired:
        process.kill()
    time.sleep(2)


def run_gate(address: str, implementation: str, threshold: str) -> dict:
    env = dict(os.environ)
    env["IGI_ANGR_PYTHON"] = env.get("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))
    cmd = [sys.executable, str(ROOT / "scripts" / "native_gate.py"),
           "--root", str(ROOT), "--address", address, "--file", implementation,
           "--threshold", threshold, "--cfg-threshold", "0.85"]
    try:
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                           env=env, timeout=900)
    except subprocess.TimeoutExpired:
        return {"ok": False, "reason": "gate timed out"}
    return {"ok": p.returncode == 0, "exitCode": p.returncode,
            "tail": (p.stdout.splitlines()[-3:] + p.stderr.splitlines()[-3:])}


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--symbols", default=str(ROOT / "assets" / "candidate_symbol_map.json"))
    ap.add_argument("--threshold", default="STRUCTURALLY_VERIFIED")
    ap.add_argument("--duration", type=int, default=20)
    ap.add_argument("--level", type=int, default=4)
    ap.add_argument("--skip-static", action="store_true")
    ap.add_argument("--skip-runtime", action="store_true")
    ap.add_argument("--only-implemented", action="store_true", default=True)
    a = ap.parse_args()

    symbol_map = json.load(open(a.symbols))
    rows = [r for r in symbol_map if r.get("implemented")]
    print(f"natives with an engine implementation: {len(rows)}", flush=True)

    results = {r["address"]: {"address": r["address"], "name": r["retailName"],
                               "cppName": r["cppName"],
                               "implementation": r["enginePath"],
                               "status": r["status"]} for r in rows}

    if not a.skip_static:
        for i, r in enumerate(rows, 1):
            res = results[r["address"]]
            res["static"] = run_gate(r["address"], r["enginePath"], a.threshold)
            print(f"  static {i}/{len(rows)} {r['address']} {r['retailName']:<30} "
                  f"ok={res['static']['ok']}", flush=True)

    if not a.skip_runtime:
        retail_targets = [{"name": r["retailName"], "va": r["address"]} for r in rows]
        print("starting retail igi.exe ...", flush=True)
        retail = start_game(RETAIL_EXE, a.level)
        time.sleep(10)
        retail_pid = find_pid("igi.exe")
        res = frida_probe("igi.exe", retail_targets, a.duration, pid=retail_pid)
        print(f"  retail probe: pid={retail_pid} installed={res.get('installed')} "
              f"observed={len(res.get('hits', {}))}", flush=True)
        for r in rows:
            hits = res.get("hits", {}).get(r["retailName"])
            results[r["address"]]["retailHits"] = hits
            results[r["address"]]["retailFirst"] = res.get("first", {}).get(r["retailName"])

        cand_rows = [r for r in rows if r.get("candidateSymbol")]
        cand_targets = [{"name": r["cppName"], "va": r["candidateSymbol"]["vaddr"]}
                        for r in cand_rows]
        # the candidate takes a separate mutex so it can run beside the retail
        # game, which holds IGIMUTEX itself
        print(f"starting open-igi.exe for {len(cand_targets)} candidate symbols ...",
              flush=True)
        candidate = start_game(CANDIDATE_EXE, a.level, mutex_name="IGIMUTEX-OPENIGI")
        time.sleep(10)
        candidate_pid = find_pid("open-igi.exe")
        cres = frida_probe("open-igi.exe", cand_targets, a.duration, pid=candidate_pid)
        print(f"  candidate probe: pid={candidate_pid} installed={cres.get('installed')} "
              f"observed={len(cres.get('hits', {}))}", flush=True)
        for r in cand_rows:
            results[r["address"]]["candidateHits"] = cres.get("hits", {}).get(r["cppName"])
            results[r["address"]]["candidateFirst"] = cres.get("first", {}).get(r["cppName"])

        stop_game(retail)
        stop_game(candidate)

    for res in results.values():
        static_ok = res.get("static", {}).get("ok", False)
        retail_ok = bool(res.get("retailHits"))
        cand_ok = bool(res.get("candidateHits"))
        res["retailCalled"] = retail_ok
        res["candidateCalled"] = cand_ok
        res["reachabilityObserved"] = bool(static_ok and retail_ok and cand_ok)
        res["verified"] = False
        res["verification"] = "diagnostic_only_no_matching_input_output_state_comparison"

    verified = sum(1 for r in results.values() if r["verified"])
    dest = ROOT / "docs" / "fidelity" / "evidence" / "per-native-verification.json"
    dest.write_text(json.dumps({
        "retail": "igi.exe",
        "candidate": "open-igi.exe",
        "level": a.level,
        "threshold": a.threshold,
        "results": list(results.values()),
    }, indent=2) + "\n")
    print(f"\nverified {verified}/{len(results)} -> {dest}")
    for res in sorted(results.values(), key=lambda r: r["address"]):
        if not res["verified"]:
            print(f"  {res['address']} {res['name']:<32} static={res.get('static',{}).get('ok')} "
                  f"retail={res.get('retailHits')} candidate={res.get('candidateHits')}")
    raise SystemExit("Diagnostic capture complete; behavioral acceptance is not established")


if __name__ == "__main__":
    main()
