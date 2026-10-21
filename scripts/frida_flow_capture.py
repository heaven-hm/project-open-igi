#!/usr/bin/env python3
"""Capture the retail first-call order for the MVP targets.

Spawns retail IGI.EXE suspended, installs an entry hook on every MVP target
before the primary thread resumes (so load-phase calls are observed), then
records the first hit of each address in execution order.

The result orders targets that static analysis cannot reach (virtual dispatch,
QVM task dispatch). It is an observation of one scripted Level 1 run, not a
proof that every path always executes.

Usage:
    python3 scripts/frida_flow_capture.py --targets scripts/mvp_native_targets.json
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import sys
import time

ROOT = Path(__file__).resolve().parent.parent
GAME_DIR = Path("/Users/haseeb-mir/Downloads/IGI")
RETAIL = GAME_DIR / "igi.exe"

PROBE = r"""
'use strict';
let hits = []; let sequence = 0; let seen = {}; let installed = 0; let failed = [];
rpc.exports.start = function(moduleName, targets) {
  const module = Process.getModuleByName(moduleName);
  const base = ptr('0x00400000');
  for (const item of targets) {
    try {
      const target = module.base.add(ptr(item.address).sub(base));
      const address = item.address, name = item.name;
      Interceptor.attach(target, {
        onEnter() {
          if (seen[address]) return;
          seen[address] = true;
          const hit = {address: address, name: name, sequence: sequence++, at: Date.now()};
          hits.push(hit);
          send({type: 'hit', hit: hit});
        }
      });
      installed++;
    } catch (error) {
      failed.push({address: item.address, error: String(error)});
    }
  }
  return {installed: installed, failed: failed.length};
};
rpc.exports.results = function() {
  return {hits: hits, installed: installed, failed: failed};
};
rpc.exports.key = function(code) {
  const user32 = Process.getModuleByName('user32.dll');
  const findWindow = new NativeFunction(user32.getExportByName('FindWindowW'), 'pointer', ['pointer', 'pointer'], 'stdcall');
  const enumWindows = new NativeFunction(user32.getExportByName('EnumWindows'), 'int', ['pointer', 'pointer'], 'stdcall');
  const getWindowText = new NativeFunction(user32.getExportByName('GetWindowTextW'), 'int', ['pointer', 'pointer', 'int'], 'stdcall');
  const getWindowPid = new NativeFunction(user32.getExportByName('GetWindowThreadProcessId'), 'uint32', ['pointer', 'pointer'], 'stdcall');
  const postMessage = new NativeFunction(user32.getExportByName('PostMessageW'), 'int', ['pointer', 'uint32', 'uint32', 'pointer'], 'stdcall');
  const pidBuffer = Memory.alloc(4); const textBuffer = Memory.alloc(512);
  let window = ptr(0);
  const callback = new NativeCallback((candidate, data) => {
    pidBuffer.writeU32(0); getWindowPid(candidate, pidBuffer);
    if (pidBuffer.readU32() !== Process.id) return 1;
    textBuffer.writeU16(0); getWindowText(candidate, textBuffer, 256);
    if (textBuffer.readUtf16String() === 'IGI') { window = candidate; return 0; }
    return 1;
  }, 'int', ['pointer', 'pointer'], 'stdcall');
  enumWindows(callback, ptr(0));
  if (window.isNull()) return {sent: false};
  const down = postMessage(window, 0x0100, code, ptr(0));
  const up = postMessage(window, 0x0101, code, ptr(0));
  return {sent: down !== 0 && up !== 0, code: code};
};
"""


def win_path(path: Path) -> str:
    return "Z:" + str(path)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--targets", type=Path, default=ROOT / "scripts" / "mvp_native_targets.json")
    parser.add_argument("--binary", type=Path, default=RETAIL)
    parser.add_argument("--attach-pid", type=int, default=None,
                        help="Attach to an already running retail process instead of spawning.")
    parser.add_argument("--seconds", type=float, default=30.0)
    parser.add_argument("--start-key", action="store_true", help="send Space then Enter to start the level")
    parser.add_argument("--report", type=Path, default=ROOT / "build" / "runtime-traces" / "mvp-runtime-flow.json")
    args = parser.parse_args()

    try:
        import frida
    except ImportError:
        python = Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python"
        os.execv(str(python), [str(python), str(Path(__file__).resolve()), *sys.argv[1:]])

    targets = [
        {"address": f"0x{int(entry['address'], 16):08X}", "name": entry.get("name", "")}
        for entry in json.loads(args.targets.read_text())
    ]
    binary = args.binary.resolve()
    device = frida.get_device_manager().add_remote_device(os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))

    report = {
        "schema_version": 1,
        "kind": "retail-runtime-flow",
        "binary_sha256": hashlib.sha256(binary.read_bytes()).hexdigest(),
        "targets": len(targets),
        "seconds": args.seconds,
        "result": "incomplete",
    }
    pid = None
    session = None
    try:
        if args.attach_pid:
            pid = args.attach_pid
            session = device.attach(pid)
        else:
            pid = device.spawn([win_path(binary), "window", "level1"], cwd=win_path(GAME_DIR))
            session = device.attach(pid)
        script = session.create_script(PROBE)
        script.load()
        streamed = []
        script.on("message", lambda message, data: streamed.append(message["payload"]["hit"])
                  if message.get("type") == "send" and isinstance(message.get("payload"), dict)
                  and message["payload"].get("type") == "hit" else None)
        report["install"] = script.exports_sync.start(binary.name, targets)
        if not args.attach_pid:
            device.resume(pid)
        deadline = time.monotonic() + args.seconds
        pressed = []
        while time.monotonic() < deadline:
            elapsed = args.seconds - (deadline - time.monotonic())
            if args.start_key and 10 <= elapsed < 11 and not pressed:
                pressed.append({"at": 10, "space": script.exports_sync.key(0x20)})
            if args.start_key and 20 <= elapsed < 21 and len(pressed) == 1:
                pressed.append({"at": 20, "enter": script.exports_sync.key(0x0D)})
            time.sleep(1)
        report["keys"] = pressed
        try:
            report["probe"] = script.exports_sync.results()
        except Exception as error:  # the game may exit during the window
            report["probe"] = {"hits": streamed, "streamed": True, "rpc_error": repr(error)}
        report["result"] = "captured"
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

    hits = report.get("probe", {}).get("hits", [])
    report["observed"] = len(hits)
    report["ordered"] = [{"order": i + 1, "address": h["address"], "name": h["name"]}
                         for i, h in enumerate(hits)]
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(f"installed {report['install']['installed']} hooks; observed {len(hits)} distinct entries; "
          f"report {args.report}")
    for item in report["ordered"][:12]:
        print(f"  {item['order']:3d} {item['address']} {item['name']}")
    return int(report["result"] != "captured")


if __name__ == "__main__":
    raise SystemExit(main())
