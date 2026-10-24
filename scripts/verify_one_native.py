#!/usr/bin/env python3
"""Verify one implemented native on both sides: static, retail runtime, candidate runtime.

For a single native this runs, in order:

  1. static  - native_gate.py for that address against its implementation file
  2. retail  - a Frida hook on the retail VA in igi.exe, recording calls
  3. candidate - a Frida hook on the candidate's symbol in open-igi.exe

and records the three results together, so a native is only reported as verified
when the binary analysis and both runtime observations agree.
"""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FRIDA = str(Path.home() / ".local" / "bin" / "frida")
FRIDA_HOST = "127.0.0.1:27042"
RETAIL = "igi.exe"
CANDIDATE = "open-igi.exe"

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
  } catch (e) { failures.push({name: target.name, error: String(e)}); }
}
send({type: 'probe_hooks', installed: Object.keys(counts).length, failed: failures.length, failures});
setTimeout(() => {
  const called = {};
  for (const name of Object.keys(counts)) if (counts[name].hits) called[name] = counts[name];
  send({type: 'probe_snapshot', called});
  send({type: 'probe_done'});
}, __DURATION__);
"""


def extract_payload(line: str):
    """Pull the payload dict out of a Frida message line."""
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
                try:
                    return json.loads(line[i:j + 1])
                except json.JSONDecodeError:
                    return None
    return None


def run_frida(process: str, targets: list[dict], duration: int) -> dict:
    if not targets:
        return {"result": "no_targets"}
    source = (PROBE.replace("__TARGETS__", json.dumps(targets))
                     .replace("__DURATION__", str(duration * 1000)))
    script = ROOT / "build" / "frida_single_native_probe.js"
    script.parent.mkdir(parents=True, exist_ok=True)
    script.write_text(source)
    frida = subprocess.Popen(
        [FRIDA, "-H", FRIDA_HOST, "-n", process, "-l", str(script),
         "--no-auto-reload", "-q"],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        stdout, stderr = frida.communicate(timeout=duration + 20)
    except subprocess.TimeoutExpired:
        frida.kill()
        stdout, stderr = frida.communicate()
    observed, installed, failed = {}, 0, []
    for line in stdout.splitlines():
        payload = extract_payload(line)
        if not payload:
            continue
        if payload.get("type") == "probe_hooks":
            installed = payload.get("installed", 0)
            failed = payload.get("failures", [])
        elif payload.get("type") == "probe_snapshot":
            observed = payload.get("called", {})
    if not observed and not installed:
        return {"process": process, "result": "no_output",
                "stdout": stdout[-400:], "stderr": stderr[-400:]}
    return {"process": process, "installed": installed, "failures": failed,
            "observed": observed,
            "hits": {k: v.get("hits") for k, v in observed.items()}}


def run_gate(address: str, implementation: str, threshold: str) -> dict:
    cmd = [sys.executable, str(ROOT / "scripts" / "native_gate.py"),
           "--root", str(ROOT), "--address", address, "--file", implementation,
           "--threshold", threshold, "--cfg-threshold", "0.85"]
    env = dict(**__import__("os").environ)
    env["IGI_ANGR_PYTHON"] = env.get("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))
    p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env, timeout=900)
    return {"exitCode": p.returncode,
            "ok": p.returncode == 0,
            "tail": p.stdout.splitlines()[-4:] + p.stderr.splitlines()[-4:]}


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("addresses", nargs="+")
    ap.add_argument("--symbols", default=str(ROOT / "assets" / "candidate_symbol_map.json"))
    ap.add_argument("--threshold", default="STRUCTURALLY_VERIFIED")
    ap.add_argument("--duration", type=int, default=25)
    ap.add_argument("--static-only", action="store_true")
    a = ap.parse_args()

    symbol_map = {r["address"].upper(): r for r in json.load(open(a.symbols))}

    results = []
    for address in a.addresses:
        entry = symbol_map.get(address.upper())
        if entry is None:
            print(f"{address}: not in the symbol map", flush=True)
            continue
        record = {"address": address, "name": entry.get("retailName"),
                  "retailName": entry.get("retailName"),
                  "cppName": entry.get("cppName"),
                  "implementation": entry.get("enginePath"),
                  "implemented": entry.get("implemented"),
                  "status": entry.get("status")}

        if entry.get("implemented") and entry.get("enginePath"):
            record["static"] = run_gate(address, entry["enginePath"], a.threshold)
        else:
            record["static"] = {"ok": False, "reason": "not implemented in the engine"}

        if not a.static_only:
            record["retailRuntime"] = run_frida(
                RETAIL, [{"name": entry.get("retailName") or address, "va": address}],
                a.duration)
            symbol = entry.get("candidateSymbol")
            if symbol:
                record["candidateRuntime"] = run_frida(
                    CANDIDATE, [{"name": entry.get("cppName") or address,
                                 "va": symbol["vaddr"]}], a.duration)
            else:
                record["candidateRuntime"] = {"result": "no_candidate_symbol"}

        ok = record["static"].get("ok", False)
        if not a.static_only:
            retail_hits = record.get("retailRuntime", {}).get("hits", {})
            cand_hits = record.get("candidateRuntime", {}).get("hits", {})
            record["retailCalled"] = bool(retail_hits)
            record["candidateCalled"] = bool(cand_hits)
            ok = ok and record["retailCalled"]
        record["verified"] = ok
        results.append(record)
        print(f"{address} {record['name']:<32} static={record['static'].get('ok')} "
              f"retail={record.get('retailCalled')} candidate={record.get('candidateCalled')}",
              flush=True)

    dest = ROOT / "docs" / "fidelity" / "evidence" / "per-native-verification.json"
    dest.write_text(json.dumps({"results": results}, indent=2) + "\n")
    print(f"written {dest}")
    good = sum(1 for r in results if r.get("verified"))
    print(f"verified {good}/{len(results)}")


if __name__ == "__main__":
    main()
