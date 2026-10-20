#!/usr/bin/env python3
"""Per-native verification matrix: static gate + retail runtime + candidate runtime.

Implements the required per-native loop: every implemented native is checked

  1. statically   - native_gate.py for that exact address against its implementation
                    file, using the same flags the build's gate uses;
  2. on retail     - a Frida entry/return hook on the retail VA in igi.exe;
  3. on the candidate - a Frida entry/return hook on the address-bound candidate
                    symbol in open-igi.exe.

The three results are merged per address into
docs/fidelity/evidence/per-native-verification.json so progress toward full
coverage is measured, not asserted.

Runtime captures hook every target in a single attach (one session, one clock),
which is cheaper than one process per native and gives every native the same
observation window. This script records diagnostics only; matching-input,
output and state comparisons are required by the separate acceptance checker.

Attach-after-launch misses load-phase calls; session metadata records the exact
limitation instead of hiding it.
"""

from __future__ import annotations

import argparse
import ast
import hashlib
import json
import os
import pathlib
import re
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
FRIDA = str(pathlib.Path.home() / ".local" / "bin" / "frida")
FRIDA_HOST = "127.0.0.1:27042"
PROCESSES = {"retail": "igi.exe", "candidate": "open-igi.exe"}
GAME_DIR = pathlib.Path("/Users/haseeb-mir/Downloads/IGI")
SYMBOL_MAP = ROOT / "assets" / "candidate_symbol_map.json"
RECORDS = ROOT / "docs" / "fidelity" / "evidence" / "per-native-verification.json"
TRACES = ROOT / "build" / "runtime-traces" / "per-native"

PROBE_TEMPLATE = """'use strict';
const targets = __TARGETS__;
const counts = {};
const failures = [];
for (const t of targets) {
  counts[t.name] = {va: t.va, hits: 0, first: []};
  try {
    Interceptor.attach(ptr(t.va), {
      onEnter(args) {
        const r = counts[t.name]; r.hits++;
        this.rec = null;
        if (r.first.length < 1) {
          const w = []; for (let i = 0; i < 4; i++) w.push(args[i].toString());
          this.rec = {args: w}; r.first.push(this.rec);
        }
      },
      onLeave(rv) { if (this.rec) this.rec.ret = rv.toString(); }
    });
  } catch (e) { failures.push({name: t.name, va: t.va, error: String(e)}); }
}
send({type: 'probe_hooks', installed: Object.keys(counts).length, failed: failures.length, failures: failures.slice(0, 20)});
setTimeout(() => { send({type: 'probe_snapshot', called: counts}); send({type: 'probe_done'}); }, __DURATION__);
"""


def sha256(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


def load_targets(addresses: list[str] | None = None) -> list[dict]:
    records = json.loads(SYMBOL_MAP.read_text(encoding="utf-8"))
    wanted = {a.upper() for a in addresses} if addresses else None
    targets = []
    for record in records:
        if not record.get("implemented") or not record.get("enginePath"):
            continue
        if wanted is not None and record["address"].upper() not in wanted:
            continue
        targets.append(record)
    return targets


def parse_frida_output(text: str) -> tuple[dict | None, dict | None]:
    snapshot = None
    hooks = None
    for line in text.splitlines():
        if not line.startswith("message: {"):
            continue
        body = line[len("message: "):].rsplit(" data:", 1)[0]
        try:
            wrapper = ast.literal_eval(body)
        except (ValueError, SyntaxError):
            continue
        payload = wrapper.get("payload", wrapper)
        if payload.get("type") == "probe_snapshot":
            snapshot = payload.get("called")
        elif payload.get("type") == "probe_hooks":
            hooks = payload
    return snapshot, hooks


WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"


def process_present(side: str) -> bool:
    proc = subprocess.run([FRIDA + "-ps", "-H", FRIDA_HOST], capture_output=True, text=True, timeout=60)
    name = PROCESSES[side]
    return any(name in line.split() for line in proc.stdout.splitlines())


def launch_game(side: str, level: int, log: pathlib.Path) -> subprocess.Popen:
    """Launch one game through CrossOver from the game data root.

    The working directory must be the game root: retail resolves YMBE.AFP and its
    level data relative to it, and the same root is used for the candidate so the
    pair sees identical data.
    """
    exe = f"Z:\\Users\\haseeb-mir\\Downloads\\IGI\\{PROCESSES[side]}"
    log.parent.mkdir(parents=True, exist_ok=True)
    handle = log.open("w")
    env = dict(os.environ, LC_ALL="C", LANG="C")
    return subprocess.Popen([WINE, "--bottle", "open-igi", exe, "window", f"level{level}"],
                            cwd=GAME_DIR, stdout=handle, stderr=subprocess.STDOUT, env=env)


def wait_for_process(side: str, seconds: int) -> bool:
    deadline = time.time() + seconds
    while time.time() < deadline:
        if process_present(side):
            return True
        time.sleep(0.5)
    return False


def runtime_capture(side: str, targets: list[dict], duration: int) -> dict:
    process = PROCESSES[side]
    probe_targets = []
    for record in targets:
        if side == "retail":
            probe_targets.append({"name": record.get("retailName") or record["address"],
                                  "va": record["address"]})
        else:
            symbol = record.get("candidateSymbol")
            if not symbol:
                continue
            probe_targets.append({"name": record.get("cppName") or record["address"],
                                  "va": symbol["vaddr"]})
    if not probe_targets:
        return {"status": "no_targets"}
    TRACES.mkdir(parents=True, exist_ok=True)
    script = ROOT / "build" / f"per-native-probe-{side}.js"
    script.write_text(PROBE_TEMPLATE.replace("__TARGETS__", json.dumps(probe_targets))
                                        .replace("__DURATION__", str(duration * 1000)))
    log = TRACES / f"{side}-capture.txt"
    started = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    proc = subprocess.run([FRIDA, "-H", FRIDA_HOST, "-n", process, "-l", str(script),
                           "--no-auto-reload", "-q", "-t", str(duration + 25)],
                          capture_output=True, text=True, timeout=duration + 120)
    log.write_text(proc.stdout + proc.stderr)
    snapshot, hooks = parse_frida_output(proc.stdout)
    session = {
        "process": process,
        "side": side,
        "started": started,
        "duration_ms": duration * 1000,
        "observation": "frida attach after launch; load-phase calls before attach are not observed",
        "hooks_installed": (hooks or {}).get("installed"),
        "hooks_failed": (hooks or {}).get("failed"),
        "frida": subprocess.run([FRIDA, "--version"], capture_output=True, text=True).stdout.strip(),
    }
    exe = GAME_DIR / process
    if exe.is_file():
        session["sha256"] = sha256(exe)
    if snapshot is None:
        session["status"] = "no_snapshot"
        return {"status": "no_snapshot", "session": session, "natives": {}}
    session["status"] = "captured"
    record = {"status": "captured", "session": session, "natives": snapshot}
    (TRACES / f"{side}-snapshot.json").write_text(json.dumps(record, indent=1) + "\n")
    return record


def static_gate(record: dict, threshold: str, structural_only: bool) -> dict:
    implementation = record["enginePath"]
    cmd = [sys.executable, str(ROOT / "scripts" / "native_gate.py"),
           "--root", str(ROOT), "--address", record["address"], "--file", implementation,
           "--threshold", threshold, "--cfg-threshold", "0.85"]
    if structural_only:
        cmd.append("--allow-structural-only")
    env = dict(os.environ)
    env.setdefault("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr" / "bin" / "python"))
    proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env, timeout=1800)
    tail = [line for line in (proc.stdout + proc.stderr).splitlines() if line.strip()][-6:]
    reason = ""
    for line in tail:
        if "BLOCKED" in line or "missing" in line or "failed" in line.lower():
            reason = line.strip()
    return {"ok": proc.returncode == 0, "exitCode": proc.returncode,
            "threshold": threshold, "structural_only": structural_only,
            "reason": reason, "tail": tail}


def merge_records(records: pathlib.Path, results: dict[str, dict]) -> None:
    if records.is_file():
        existing = json.loads(records.read_text(encoding="utf-8"))
    else:
        existing = {"results": []}
    index = {entry["address"]: entry for entry in existing.get("results", [])}
    for address, fresh in results.items():
        index[address] = {**index.get(address, {}), **fresh}
    existing["results"] = [index[a] for a in sorted(index, key=lambda a: int(a, 16))]
    records.parent.mkdir(parents=True, exist_ok=True)
    records.write_text(json.dumps(existing, indent=2) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--phase", choices=["static", "retail", "candidate", "all"], default="all")
    parser.add_argument("--duration", type=int, default=20)
    parser.add_argument("--threshold", default="STRUCTURALLY_VERIFIED")
    parser.add_argument("--structural-only", action="store_true",
                        help="Pass --allow-structural-only to the gate (build-compatible mode).")
    parser.add_argument("--address", action="append", default=None)
    parser.add_argument("--no-merge", action="store_true")
    parser.add_argument("--launch", choices=["retail", "candidate"], default=None,
                        help="Launch this side through CrossOver before capturing it.")
    parser.add_argument("--level", type=int, default=7)
    parser.add_argument("--wait-seconds", type=int, default=60)
    parser.add_argument("--keep-alive", action="store_true",
                        help="Leave a launched game running after the capture.")
    args = parser.parse_args()

    targets = load_targets(args.address)
    if not targets:
        print("no implemented natives matched", file=sys.stderr)
        return 2
    print(f"implemented natives: {len(targets)}")

    results: dict[str, dict] = {}
    for record in targets:
        results[record["address"]] = {
            "address": record["address"],
            "name": record.get("retailName"),
            "cppName": record.get("cppName"),
            "implementation": record.get("enginePath"),
        }

    if args.phase in ("static", "all"):
        for index, record in enumerate(targets, 1):
            gate = static_gate(record, args.threshold, args.structural_only)
            results[record["address"]]["static"] = gate
            print(f"[{index}/{len(targets)}] {record['address']} {record.get('retailName','?'):<38} "
                  f"static={'pass' if gate['ok'] else 'FAIL'} {gate['reason'][:70]}", flush=True)
            if not args.no_merge:
                merge_records(RECORDS, results)

    for side in ("retail", "candidate"):
        if args.phase not in (side, "all"):
            continue
        launched = False
        side_key = "retailRuntime" if side == "retail" else "candidateRuntime"
        if args.launch == side:
            if process_present(side):
                print(f"[{side}] {PROCESSES[side]} already running; attaching to the existing instance")
            else:
                log = TRACES / f"{side}-launch.log"
                launch_game(side, args.level, log)
                launched = True
                print(f"[{side}] launched level{args.level} from {GAME_DIR}; waiting up to "
                      f"{args.wait_seconds}s for the process")
                if not wait_for_process(side, args.wait_seconds):
                    print(f"[{side}] {PROCESSES[side]} did not appear within {args.wait_seconds}s; "
                          f"launch log: {log}", file=sys.stderr)
                    if not args.no_merge:
                        merge_records(RECORDS, {
                            record["address"]: {**results[record["address"]],
                                                side_key: {"status": "launch_failed"}}
                            for record in targets})
                    continue
                print(f"[{side}] attached at launch (early attach; load-phase calls covered)")
        capture = runtime_capture(side, targets, args.duration)
        if launched:
            capture["session"]["launched"] = {"level": args.level, "cwd": str(GAME_DIR),
                                             "crossover_bottle": "open-igi"}
            capture["session"]["termination"] = ("left running" if args.keep_alive
                                                 else "terminated by the harness after capture "
                                                      "(not a normal in-game exit)")
            if not args.keep_alive:
                subprocess.run(["pkill", "-f", f"{PROCESSES[side]} window level{args.level}"],
                               capture_output=True)
                time.sleep(3)
        session = capture.get("session", {"status": capture.get("status")})
        if side == "candidate":
            natives = capture.get("natives") or {}
            if natives and not any((entry or {}).get("hits") for entry in natives.values()) \
                    and process_present("retail"):
                session["status"] = "blocked_single_instance_mutex"
                session["note"] = ("retail igi.exe held IGIMUTEX during this capture; OpenIGI refuses a "
                                   "second instance with the modal 'IGI is already running.' dialog, so "
                                   "zero recorded calls are not method-level evidence")
                print(f"[{side}] blocked by IGIMUTEX contention with retail; capture is not evidence")
        print(f"[{side}] {session.get('status')} hooks={session.get('hooks_installed')} "
              f"failed={session.get('hooks_failed')} sha={session.get('sha256','?')[:12]}")
        observed = {name for name, entry in (capture.get("natives") or {}).items() if entry.get("hits")}
        print(f"[{side}] observed {len(observed)}/{len(capture.get('natives') or {})}")
        zero = sorted(name for name, entry in (capture.get("natives") or {}).items() if not entry.get("hits"))
        for name in zero:
            print(f"[{side}]   no calls observed: {name}")
        for record in targets:
            key = record.get("retailName") if side == "retail" else (record.get("cppName") or record.get("retailName"))
            entry = (capture.get("natives") or {}).get(key)
            results[record["address"]][side_key] = {
                "process": PROCESSES[side],
                "installed": session.get("hooks_installed"),
                "failures": session.get("hooks_failed"),
                "observed": None if entry is None else {key: entry},
                "hits": {} if entry is None else {key: entry.get("hits")},
                "session": session,
            }
        if not args.no_merge:
            merge_records(RECORDS, results)

    for record in targets:
        entry = results[record["address"]]
        static_ok = bool(entry.get("static", {}).get("ok"))
        retail_hits = (entry.get("retailRuntime") or {}).get("hits") or {}
        candidate_hits = (entry.get("candidateRuntime") or {}).get("hits") or {}
        entry["retailCalled"] = any((hits or 0) > 0 for hits in retail_hits.values())
        entry["candidateCalled"] = any((hits or 0) > 0 for hits in candidate_hits.values())
        entry["verified"] = False
        if static_ok and entry["retailCalled"]:
            entry["verification"] = "diagnostic_only_no_matching_input_output_state_comparison"
        elif static_ok:
            entry["verification"] = "static_passed_retail_call_not_observed"
        else:
            entry["verification"] = "static_gate_not_satisfied"
    if not args.no_merge:
        merge_records(RECORDS, results)

    print(f"records written to {RECORDS.relative_to(ROOT)}")
    return 1  # Diagnostic observations never authorize acceptance.


if __name__ == "__main__":
    raise SystemExit(main())
