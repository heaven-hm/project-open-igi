#!/usr/bin/env python3
"""Paired retail/candidate runtime observation for a small list of natives.

Runs the SAME probe against retail igi.exe and against a freshly built candidate
executable, one process at a time (both acquire IGIMUTEX), and writes a
differential record: per native, whether each side reached it, call counts, and
the first observed argument words and return values.

This exists because the per-native acceptance loop needs both sides observed
with the same script. When the candidate cannot reach a native, that is recorded
as "not reached" with the reason, never as a pass.

Usage:
  python3 scripts/tester/capture_paired_native.py \
      --candidate build-win32-no-verify-gate/igi1.exe \
      --vas 0x004B0E70 0x004B12F0 0x004B0F90 --level 1
"""

import argparse
import datetime
import hashlib
import json
import pathlib
import sys
import time

import frida

EXPECTED_RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
BASE = 0x00400000
GAME_DIR = pathlib.Path("/Users/haseeb-mir/Downloads/IGI")
RETAIL = GAME_DIR / "igi.exe"
ROOT = pathlib.Path(__file__).resolve().parents[2]
REQUESTED_CWD = "Z:\\Users\\haseeb-mir\\Downloads\\IGI"
GAME_PROCESS_NAMES = {"igi.exe", "igi1.exe", "open-igi.exe"}

CWD_JS = r"""
const cwdBuffer = Memory.alloc(2048);
const cwdFn = new NativeFunction(
    Process.getModuleByName('kernel32.dll').getExportByName('GetCurrentDirectoryW'),
    'uint32', ['uint32', 'pointer'], 'stdcall');
const cwdLen = cwdFn(1024, cwdBuffer);
if (cwdLen === 0 || cwdLen >= 1024) throw new Error('cwd query failed');
send({type: 'cwd', value: cwdBuffer.readUtf16String(cwdLen)});
"""

PROBE_TEMPLATE = r"""
const modules = Process.enumerateModules();
const exe = modules.find(m => /igi/i.test(m.name)) || modules[0];
const base = exe.base;
send({type: 'module', name: exe.name, base: base.toString()});
const targets = %(targets)s;
const counts = {};
const samples = {};
const failures = [];
for (const va of targets) {
  try {
    Interceptor.attach(base.add(va - 0x400000), {
      onEnter(args) {
        const key = va;
        counts[key] = (counts[key] || 0) + 1;
        if (!samples[key]) {
          samples[key] = [args[0].toString(), args[1].toString(), args[2].toString()];
          send({type: 'first_call', va: key, args: samples[key]});
        }
      },
      onLeave(retval) {
        send({type: 'return', va: va, value: retval.toString()});
      }
    });
  } catch (e) {
    failures.push(String(va) + ': ' + e.message);
  }
}
send({type: 'ready', installed: targets.length - failures.length, failures: failures, targets: targets});
rpc.exports = {
  snapshot() {
    return {counts: counts, samples: samples};
  }
};
"""


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run_side(device, label, executable, arguments, targets, seconds, record):
    side = {"executable": str(executable), "sha256": sha256(executable), "arguments": arguments,
            "frida_python": frida.__version__, "result": "failed"}
    record[label] = side
    pid = None
    session = None
    messages = []
    detached = {}

    existing = [p.name for p in device.enumerate_processes() if p.name.lower() in GAME_PROCESS_NAMES]
    if existing:
        side["error"] = f"refusing concurrent game: {existing!r}"
        return side

    try:
        windows_path = str(executable).replace("/", "\\")
        if windows_path.startswith("\\Users"):
            windows_path = "Z:" + windows_path
        elif windows_path.startswith("Users"):
            windows_path = "Z:\\" + windows_path
        pid = device.spawn([windows_path] + arguments, cwd=REQUESTED_CWD)
        side["pid"] = pid
        session = device.attach(pid)
        session.on("detached", lambda reason, crash=None: detached.setdefault("reason", str(reason)))
        js = CWD_JS + (PROBE_TEMPLATE % {"targets": json.dumps([int(t, 16) for t in targets])})
        script = session.create_script(js)
        script.on("message", lambda m, d: messages.append(m))
        script.load()

        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            if any(m.get("type") == "send" and m.get("payload", {}).get("type") == "cwd" for m in messages):
                break
            time.sleep(0.1)
        cwd_msg = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "cwd"), None)
        side["observed_cwd"] = cwd_msg["payload"]["value"] if cwd_msg else None
        ready = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "ready"), None)
        side["ready"] = ready["payload"] if ready else None
        if ready and ready["payload"].get("failures"):
            side["hook_failures"] = ready["payload"]["failures"]
        side["hooks_before_resume"] = bool(ready and ready["payload"]["installed"] == len(targets))

        module = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "module"), None)
        side["module"] = module["payload"] if module else None

        device.resume(pid)
        side["resumed"] = True
        time.sleep(seconds)

        if "reason" in detached:
            side["detached"] = detached["reason"]
        try:
            snapshot = script.exports_sync.snapshot()
            side["counts"] = snapshot["counts"]
            side["samples"] = snapshot["samples"]
        except Exception as error:
            side["snapshot_error"] = str(error)
        calls = sum((side.get("counts") or {}).values())
        side["result"] = "reached" if calls else ("exited" if "reason" in detached else "not_reached")
    except Exception as error:
        side["error"] = f"{type(error).__name__}: {error}"
    finally:
        if pid is not None:
            try:
                device.kill(pid)
            except Exception:
                pass
        if session is not None:
            try:
                session.detach()
            except Exception:
                pass
        time.sleep(1.0)
        side["game_processes_after"] = [p.name for p in device.enumerate_processes() if p.name.lower() in GAME_PROCESS_NAMES]
    return side


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--candidate", default="build-win32-no-verify-gate/igi1.exe")
    parser.add_argument("--vas", nargs="+", required=True)
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--seconds-retail", type=float, default=30.0)
    parser.add_argument("--seconds-candidate", type=float, default=20.0)
    parser.add_argument("--host", default="127.0.0.1:27042")
    parser.add_argument("--out", default=None)
    args = parser.parse_args()

    retail_sha = sha256(RETAIL)
    if retail_sha != EXPECTED_RETAIL_SHA256:
        raise SystemExit(f"retail hash mismatch: {retail_sha}")
    candidate = (ROOT / args.candidate).resolve()
    if not candidate.is_file():
        raise SystemExit(f"candidate missing: {candidate}")

    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    out = pathlib.Path(args.out) if args.out else ROOT / "docs" / "fidelity" / "evidence" / f"paired-natives-{stamp}"
    out.mkdir(parents=True, exist_ok=True)
    (out / "runner.py").write_bytes(pathlib.Path(__file__).read_bytes())

    record = {
        "kind": "paired-native-runtime-differential",
        "schema": 1,
        "captured_utc": stamp,
        "natives": args.vas,
        "level": args.level,
        "retail_sha256": retail_sha,
        "candidate_path": str(candidate),
        "candidate_sha256": sha256(candidate),
        "requested_cwd": REQUESTED_CWD,
        "order": "retail first, candidate second, never concurrent",
        "limitations": [
            "Hooks capture entry argument words and return values only; they do not interpret pointed-to memory.",
            "A side that exits before reaching a native is recorded as not reached, never as agreeing.",
            "Retail and candidate are separate processes; pointer values are not compared across them.",
        ],
    }
    device = frida.get_device_manager().add_remote_device(args.host)

    run_side(device, "retail", RETAIL, ["window", f"level{args.level}"], args.vas, args.seconds_retail, record)
    run_side(device, "candidate", candidate, ["window", f"level{args.level}"], args.vas, args.seconds_candidate, record)

    comparison = {}
    for va in args.vas:
        retail = record["retail"].get("counts", {}).get(str(int(va, 16)), 0)
        cand_side = record["candidate"]
        cand = (cand_side.get("counts") or {}).get(str(int(va, 16)), 0)
        comparison[va] = {
            "retail_calls": retail,
            "candidate_calls": cand,
            "candidate_result": cand_side.get("result"),
            "verdict": ("both_reached" if retail and cand else
                        "retail_only" if retail else
                        "candidate_only" if cand else "neither_reached"),
        }
    record["comparison"] = comparison
    (out / "paired.json").write_text(json.dumps(record, indent=2) + "\n")
    print(json.dumps({"out": str(out), "comparison": comparison,
                      "candidate_result": record["candidate"].get("result"),
                      "candidate_detached": record["candidate"].get("detached")}, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
