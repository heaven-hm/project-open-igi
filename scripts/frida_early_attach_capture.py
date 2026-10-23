#!/usr/bin/env python3
"""Attach to retail igi.exe as early as possible to observe startup natives.

Frida spawn and spawn-gating are unavailable against the CrossOver/Wine
retail image on this host, so this launches the game normally and polls for
the process, attaching on the first sighting. It records how long after launch
the attach succeeded, so a trace that missed startup is marked as such instead
of being presented as pre-resume evidence.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
GAME_DIR = Path("/Users/haseeb-mir/Downloads/IGI")
RETAIL = GAME_DIR / "igi.exe"
WINE = Path("/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine")
BOTTLE = "open-igi"

PROBE = r"""
'use strict';
var hits = [];
var seen = {};
var targets = {};
rpc.exports.start = function (moduleName, list) {
  var image = Process.getModuleByName(moduleName);
  var peOffset = image.base.add(0x3c).readU32();
  var preferred = image.base.add(peOffset + 0x34).readU32();
  list.forEach(function (entry) {
    var va = ptr(entry.address);
    var actual = image.base.add(va.sub(ptr(preferred)));
    targets[entry.address] = entry.name;
    try {
      Interceptor.attach(actual, {
        onEnter: function () {
          if (!seen[entry.address]) {
            seen[entry.address] = true;
            if (hits.length < 5000) hits.push({ address: entry.address, name: entry.name, at: Date.now() });
          }
        }
      });
    } catch (e) {}
  });
  Interceptor.flush();
  return { installed: Object.keys(targets).length };
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
rpc.exports.results = function () { return { hits: hits }; };
"""


def win_path(path: Path) -> str:
    return "Z:\\" + str(path.resolve())[1:].replace("/", "\\")


def main() -> int:
    try:
        import frida
    except ImportError:
        python = Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python"
        os.execv(str(python), [str(python), str(Path(__file__).resolve()), *sys.argv[1:]])

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--targets", type=Path, default=ROOT / "scripts/mvp_native_targets.json")
    parser.add_argument("--seconds", type=float, default=45.0)
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--keys", default="",
                        help="Comma-separated VK codes to post after hooks install, "
                             "e.g. 0x1B for Escape to open the menu.")
    parser.add_argument("--key-interval", type=float, default=2.5)
    parser.add_argument("--report", type=Path, default=ROOT / "build/runtime-traces/early.json")
    args = parser.parse_args()

    targets = [
        {"address": f"0x{int(e['address'], 16):08X}", "name": e.get("name", "")}
        for e in json.loads(args.targets.read_text())
    ]
    device = frida.get_device_manager().add_remote_device(
        os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))

    report = {
        "schema_version": 1,
        "kind": "retail-early-attach-flow",
        "binary_sha256": hashlib.sha256(RETAIL.read_bytes()).hexdigest(),
        "launch": {"command": f"igi.exe window level{args.level}", "cwd": str(GAME_DIR)},
        "targets": len(targets),
        "result": "incomplete",
    }

    env = dict(os.environ)
    env["LC_ALL"] = "C"
    env["LANG"] = "C"
    proc = subprocess.Popen(
        [str(WINE), "--bottle", BOTTLE, "--cx-app", win_path(RETAIL), "window", f"level{args.level}"],
        cwd=str(GAME_DIR), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, env=env)

    started = time.monotonic()
    session = None
    script = None
    pid = None
    try:
        # Poll fast: the goal is to beat the startup natives.
        # Several igi.exe entries can exist in the Wine process tree, and the
        # short-lived stubs carry no running game loop. Prefer a candidate whose
        # image hosts the retail module AND that keeps running across polls.
        best = None
        while time.monotonic() - started < 60 and pid is None:
            alive = []
            for candidate in device.enumerate_processes():
                if candidate.name.lower() != "igi.exe":
                    continue
                try:
                    probe = device.attach(candidate.pid)
                    mod = probe.create_script(
                        'rpc.exports.m=function(){var m=Process.enumerateModules()'
                        '.filter(function(x){return /igi\\.exe$/i.test(x.name)&&x.size>1000000;});'
                        'return m.length?m[0].size:0;};')
                    mod.load()
                    size = mod.exports_sync.m()
                    mod.unload()
                    probe.detach()
                except Exception:
                    size = 0
                if size:
                    alive.append(candidate.pid)
            if alive:
                best = alive
                pid = alive[0]
                break
            time.sleep(0.1)
        report["candidate_pids"] = best
        attach_delay = time.monotonic() - started
        report["attach_delay_seconds"] = round(attach_delay, 3)
        report["frida_pid"] = pid
        if pid is None:
            report["result"] = "failed"
            report["error"] = "igi.exe never appeared to frida"
            return 1
        # Wine can expose short-lived igi.exe stubs before the real game image
        # is mapped; require the retail image to be a loaded module with the
        # expected size before hooking, otherwise retry the next candidate.
        session = device.attach(pid)
        probe_mod = session.create_script(
            'rpc.exports.m=function(){var m=Process.enumerateModules()'
            '.filter(function(x){return /igi\\.exe$/i.test(x.name);});'
            'return m.length?{name:m[0].name,size:m[0].size,base:m[0].base.toString()}:null;};')
        probe_mod.load()
        info = probe_mod.exports_sync.m()
        probe_mod.unload()
        report["attached_module"] = info
        if not info or info.get("size", 0) < 1_000_000:
            session.detach()
            report["result"] = "failed"
            report["error"] = f"attached process lacks the retail image: {info}"
            return 1
        report["attach_completed_at"] = round(time.monotonic() - started, 3)
        script = session.create_script(PROBE)
        script.load()
        # The process can appear before igi.exe is mapped; retry until the
        # image exists so hooks are installed against the real preferred base.
        install_error = None
        for _ in range(200):
            try:
                report["install"] = script.exports_sync.start(RETAIL.name, targets)
                break
            except Exception as error:
                install_error = repr(error)
                time.sleep(0.05)
        if report.get("install") is None:
            raise RuntimeError(f"image never became hookable: {install_error}")
        # Hooks installed after the level is already running will not see that
        # level's already-executed calls. Restart the level so the instrumented
        # code runs again from its beginning.
        # The retail main window is titled "IGI" only after QTask_CreateTask has
        # run, so wait until a key can actually be delivered instead of posting
        # into nothing at a fixed early time.
        restarts = []
        while time.monotonic() - started < min(args.seconds, 40):
            try:
                result = script.exports_sync.key(0x1B)
            except Exception as error:
                result = {"error": repr(error)}
            restarts.append({"at": round(time.monotonic() - started, 1), "escape": result})
            if result.get("sent"):
                break
            time.sleep(1.0)
        report["restart_keys"] = restarts

        # Drive the menu sequence so screens the level never opens (options,
        # graphics settings) still execute their natives while hooks are live.
        sequence = [int(key, 0) for key in args.keys.split(",") if key.strip()]
        pressed = []
        for key in sequence:
            time.sleep(args.key_interval)
            try:
                sent = script.exports_sync.key(key)
            except Exception as error:
                sent = {"error": repr(error)}
            pressed.append({"key": hex(key), "at": round(time.monotonic() - started, 1),
                            "result": sent})
        report["menu_keys"] = pressed

        deadline = time.monotonic() + args.seconds
        while time.monotonic() < deadline:
            time.sleep(0.5)
        report["probe"] = script.exports_sync.results()
        report["result"] = "captured"
    except Exception as error:
        report["result"] = "failed"
        report["error"] = repr(error)
    finally:
        if session is not None:
            try:
                session.detach()
            except Exception:
                pass
        try:
            proc.terminate()
        except Exception:
            pass
        hits = (report.get("probe") or {}).get("hits") or []
        report["observed"] = len({h["address"] for h in hits})
        seen = []
        for hit in hits:
            if hit["address"] not in seen:
                seen.append(hit["address"])
        report["observed_order"] = seen
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + "\n")
        print(f"result={report['result']} attach_delay={report.get('attach_delay_seconds')}s "
              f"installed={(report.get('install') or {}).get('installed')} observed={report['observed']}")
    return 0 if report["result"] == "captured" else 1


if __name__ == "__main__":
    raise SystemExit(main())
