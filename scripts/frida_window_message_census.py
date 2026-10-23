#!/usr/bin/env python3
"""Census the window messages the candidate receives.

The input path has to be fixed from what the window actually receives, so
this spawns the candidate, counts the messages that reach its message loop
and writes the census out.
"""
import argparse
import json
import os
import pathlib
from pathlib import Path
import subprocess
import sys
from pathlib import Path
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts/frida_window_message_census.js"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--candidate", type=pathlib.Path,
                        default=pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi1.exe"))
    parser.add_argument("--out", type=pathlib.Path,
                        default=ROOT / "build/runtime-traces/window-message-census.json")
    arguments = parser.parse_args()
    try:
        import frida
    except ImportError:
        python = Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python"
        os.execv(str(python), [str(python), str(Path(__file__).resolve()), *sys.argv[1:]])
    device = frida.get_device_manager().add_remote_device("127.0.0.1:27042")
    spawned = device.spawn([str(arguments.candidate), "window", "level1"],
                           cwd=str(arguments.candidate.parent))
    session = device.attach(spawned)
    messages = []
    script = session.create_script(PROBE.read_text())
    script.on("message", lambda message, data: messages.append(message))
    script.load()
    device.resume(spawned)
    time.sleep(12)
    try:
        device.kill(spawned)
    except Exception:
        pass
    census = {}
    for message in messages:
        if message.get("type") == "send":
            payload = message.get("payload", {})
            if isinstance(payload, dict) and payload.get("kind") == "window-message-census":
                census = payload
    arguments.out.parent.mkdir(parents=True, exist_ok=True)
    arguments.out.write_text(json.dumps(census, indent=2) + "\n")
    print(json.dumps(census.get("counts", {}), indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
