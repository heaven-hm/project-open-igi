#!/usr/bin/env python3
"""Record the retail TaskType_Register (0x00401900) call order.

Launches retail igi.exe at one level, attaches as early as the process
appears, and records every registration in call order: class id, name,
object size, parent class and flags. The ordered id -> name map is what
the port must replay so class ids and handler-table slots match retail.

Registration happens during startup, so this runner attaches the moment
Frida first sees the process. A late attach misses early registrations;
that outcome is reported as settleExpiredWithoutCall instead of writing
an empty or fabricated map.
"""

import argparse
import hashlib
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import native_frida_diff as nfd

ROOT = Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts" / "frida_register_trace.js"
REGISTER_VA = "0x00401900"

CHILD = r'''
import json, sys, time
import frida

pid = int(sys.argv[1])
address = sys.argv[2]
script_source = sys.stdin.buffer.read().decode("utf-8")
result_path = sys.argv[4]
observe = float(sys.argv[5])
settle = float(sys.argv[6])

device = frida.get_device_manager().add_remote_device(
    __import__("os").environ.get("FRIDA_HOST", "127.0.0.1:27042"))
session = device.attach(pid)
script = session.create_script(script_source)
registrations = []
diagnostics = []

def on_message(message, data):
    payload = message.get("payload") or {}
    if message.get("type") == "error":
        diagnostics.append(message)
    elif payload.get("type") == "register":
        registrations.append(payload["entry"])

script.on("message", on_message)
script.load()
script.exports_sync.hook(address)
started = time.time()
last_hook_at = started
first_at = None
deadline = last_hook_at + max(0.0, settle)
while time.time() < deadline:
    if registrations:
        first_at = time.time()
        break
    time.sleep(0.1)
if registrations:
    time.sleep(observe)
session.detach()
json.dump({
    "registrations": registrations,
    "diagnostics": diagnostics,
    "firstRegistrationLatencySeconds":
        None if first_at is None else round(first_at - started, 3),
    "settleExpiredWithoutRegistration": not registrations,
}, open(result_path, "w"))
'''


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--level", type=int, default=1)
    ap.add_argument("--settle", type=float, default=120.0,
                    help="maximum seconds to wait for the first registration")
    ap.add_argument("--observe", type=float, default=8.0,
                    help="seconds to keep recording after the first registration")
    ap.add_argument("--out", type=Path,
                    default=ROOT / "build" / "runtime-traces" /
                    "retail-register-order.json")
    args = ap.parse_args()

    probe_bytes = PROBE.read_bytes()
    probe_source = probe_bytes.decode("utf-8")
    probe_digest = hashlib.sha256(probe_bytes).hexdigest()

    retail = Path(nfd.vnb.GAME_DIR) / "igi.exe"
    retail_digest = nfd.file_digest(retail)
    if nfd.frida_list().get("igi.exe"):
        raise RuntimeError("close existing retail sessions before this capture")

    print("launching retail igi.exe ...", flush=True)
    process = nfd.vnb.start_game(
        "Z:" + str(retail.resolve()).replace("/", "\\"), args.level,
        game_directory=retail.resolve().parent)
    try:
        pid = 0
        for _ in range(240):
            pid = max(nfd.frida_list().get("igi.exe", [0]))
            if pid:
                break
            time.sleep(0.1)
        if not pid:
            raise RuntimeError("Frida did not see igi.exe after launch")
        print(f"retail pid={pid}; attaching and hooking {REGISTER_VA}", flush=True)

        child = ROOT / "build" / "frida_register_child.py"
        child.write_text(CHILD)
        out = ROOT / "build" / "frida_register_order.json"
        out.unlink(missing_ok=True)
        cmd = [nfd.FRIDA_PYTHON, str(child), str(pid), REGISTER_VA, "-", str(out),
               str(args.observe), str(args.settle)]
        p = subprocess.run(cmd, input=probe_bytes.decode("utf-8"),
                           capture_output=True, text=True, encoding="utf-8",
                           timeout=args.settle + args.observe + 180)
        if p.returncode != 0 or not out.exists():
            raise RuntimeError(
                "registration probe failed: "
                f"{(p.stderr or p.stdout or '')[-600:]}")
        envelope = json.loads(out.read_text())
        out.unlink()
    finally:
        nfd.vnb.stop_game(process)
        for pid in nfd.frida_list().get("igi.exe", []):
            subprocess.run([str(Path.home() / ".local" / "bin" / "frida-kill"),
                            "-H", "127.0.0.1:27042", str(pid)],
                           capture_output=True, text=True, timeout=30)

    registrations = envelope["registrations"]
    print(f"registrations captured: {len(registrations)}")
    for entry in registrations[:40]:
        print(f"  id={entry.get('classId'):<4} size={entry.get('objectSize'):<6} "
              f"parent={entry.get('parent'):<4} flags={entry.get('flags'):<3} "
              f"name={entry.get('name')}")
    if envelope.get("settleExpiredWithoutRegistration"):
        print("settle expired with NO registrations captured; the attach was "
              "too late for startup registration")
    if envelope.get("diagnostics"):
        print(f"probe diagnostics: {json.dumps(envelope['diagnostics'])[:400]}")

    report = {
        "tool": "frida",
        "probe": str(PROBE.relative_to(ROOT)),
        "probeSha256": probe_digest,
        "retail": str(retail),
        "retailSha256": retail_digest,
        "level": args.level,
        "hookedAddress": REGISTER_VA,
        "capture": envelope,
        "limitations": [
            "Attach happens as soon as Frida first lists the process; "
            "registrations before that moment are not observed.",
            "settleExpiredWithoutRegistration means the attach missed the "
            "startup registration burst entirely.",
        ],
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
