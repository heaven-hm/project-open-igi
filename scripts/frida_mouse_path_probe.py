#!/usr/bin/env python3
"""Probe the retail's mouse path: which Win32 API carries the mouse counts.

The retail imports DINPUT.dll, so the counts may come from a DirectInput
device rather than from WM_INPUT. This probe spawns igi.exe, watches the
cursor and raw-input entry points, and reports which ones the retail calls
while the mouse is moving.
"""
import argparse
import json
import os
import pathlib
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts/frida_mouse_path_probe.js"
RETAIL = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
WINE = str(pathlib.Path.home() / "Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine")
BOTTLE = "project-igi"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", type=pathlib.Path,
                        default=ROOT / "build/runtime-traces/mouse-path-probe.json")
    arguments = parser.parse_args()
    try:
        import frida
    except ImportError:
        python = pathlib.Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python"
        os.execv(str(python), [str(python), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])
    device = frida.get_device_manager().add_remote_device("127.0.0.1:27042")
    spawned = device.spawn([str(RETAIL)], cwd=str(RETAIL.parent))
    session = device.attach(spawned)
    messages = []
    script = session.create_script(PROBE.read_text())
    script.on("message", lambda message, data: messages.append(message))
    script.load()
    device.resume(spawned)
    time.sleep(14)
    try:
        device.kill(spawned)
    except Exception:
        pass
    observed = []
    for message in messages:
        if message.get("type") == "send":
            payload = message.get("payload", {})
            if isinstance(payload, dict) and payload.get("kind") == "mouse-path-probe":
                observed = payload.get("observed", [])
    arguments.out.parent.mkdir(parents=True, exist_ok=True)
    arguments.out.write_text(json.dumps({"observed": observed}, indent=2) + "\n")
    counts = {}
    for entry in observed:
        counts[entry.get("api")] = counts.get(entry.get("api"), 0) + 1
    print(json.dumps(counts, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
