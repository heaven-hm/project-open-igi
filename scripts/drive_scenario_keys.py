#!/usr/bin/env python3
"""Attach the synthetic keyboard driver to an image while a capture is running.

The paired capture owns the game start; this helper only attaches as a second
Frida session and injects keys through SendInput (see frida_drive_keys.js). It
exists so scenario work does not have to fork the observation probe, whose hash
is recorded in every capture.

  python3 scripts/drive_scenario_keys.py --targets igi.exe igi1.exe --duration 55
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import native_frida_diff as nfd

ROOT = Path(__file__).resolve().parent.parent
DRIVER = ROOT / "scripts" / "frida_drive_keys.js"

CHILD = r"""
import json, os, sys, time
from pathlib import Path

import frida

targets = [name.lower() for name in json.loads(sys.argv[1])]
source = Path(sys.argv[2]).read_text()
duration = float(sys.argv[3])
delay = float(sys.argv[4])
patience = float(sys.argv[5])

device = frida.get_device_manager().add_remote_device(
    os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))
deadline = time.time() + patience
attached = []
while time.time() < deadline and len(attached) < len(targets):
    for process in device.enumerate_processes():
        name = process.name.lower()
        if name in targets and name not in attached:
            try:
                session = device.attach(process.pid)
                script = session.create_script(source)
                script.load()
            except Exception as error:  # noqa: BLE001 - report, do not crash the capture
                print(f"attach {name} failed: {error}")
                continue
            attached.append(name)
            print(f"driving {name} pid={process.pid}", flush=True)
            time.sleep(delay + duration)
            try:
                session.detach()
            except Exception:  # noqa: BLE001 - the image may already have exited
                pass
    time.sleep(1.0)
print(json.dumps({"driven": attached}), flush=True)
"""


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--targets", nargs="+", default=["igi.exe", "igi1.exe"])
    parser.add_argument("--duration", type=float, default=55.0,
                        help="seconds of key driving after the start delay")
    parser.add_argument("--delay", type=float, default=12.0,
                        help="seconds to wait after attach so the level can load")
    parser.add_argument("--patience", type=float, default=240.0,
                        help="seconds to watch for the target processes")
    args = parser.parse_args()

    child = ROOT / "build" / "frida_drive_child.py"
    child.parent.mkdir(parents=True, exist_ok=True)
    child.write_text(CHILD)
    result = subprocess.run(
        [nfd.FRIDA_PYTHON, str(child), json.dumps(args.targets), str(DRIVER),
         str(args.duration), str(args.delay), str(args.patience)],
        capture_output=True, text=True, encoding="utf-8")
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr[-800:])
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
