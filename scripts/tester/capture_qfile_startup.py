#!/usr/bin/env python3
"""Spawn-suspended retail observation of five landed qfile/sys natives.

This is a retail-only probe. It refuses to run if any game process is already
visible through Frida, spawns retail IGI.EXE suspended with the game directory
as cwd, verifies cwd and image base, installs the hooks before resume, then lets
startup and the level-1 load run for a bounded interval.

It never attaches to the candidate. Raw evidence is written under
docs/fidelity/evidence/ (repo-visible), not build/.
"""

import argparse
import datetime
import hashlib
import json
import pathlib
import sys
import time
import traceback

import frida

EXPECTED_RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
GAME_DIR = pathlib.Path("/Users/haseeb-mir/Downloads/IGI")
RETAIL = GAME_DIR / "igi.exe"
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROBE = ROOT / "scripts" / "tester" / "frida_qfile_startup_probe.js"
GAME_PROCESS_NAMES = {"igi.exe", "igi1.exe", "open-igi.exe"}
REQUESTED_CWD = "Z:\\Users\\haseeb-mir\\Downloads\\IGI"
GAME_NAMES = ("igi.exe", "open-igi.exe", "igi1.exe")

CWD_JS = r"""
const cwdBuffer = Memory.alloc(2048);
const cwdFn = new NativeFunction(
    Process.getModuleByName('kernel32.dll').getExportByName('GetCurrentDirectoryW'),
    'uint32', ['uint32', 'pointer'], 'stdcall');
const cwdLen = cwdFn(1024, cwdBuffer);
if (cwdLen === 0 || cwdLen >= 1024) throw new Error('cwd query failed');
send({type: 'cwd', value: cwdBuffer.readUtf16String(cwdLen)});
"""


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def game_processes(device):
    try:
        return [p for p in device.enumerate_processes() if p.name.lower() in GAME_PROCESS_NAMES]
    except Exception as error:
        return [f"<enumerate failed: {error}>"]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--seconds", type=float, default=35.0)
    parser.add_argument("--host", default="127.0.0.1:27042")
    parser.add_argument("--out", default=None)
    args = parser.parse_args()

    retail_sha = sha256(RETAIL)
    if retail_sha != EXPECTED_RETAIL_SHA256:
        raise SystemExit(f"retail hash mismatch: {retail_sha}")
    probe_sha = sha256(PROBE)
    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    out = pathlib.Path(args.out) if args.out else ROOT / "docs" / "fidelity" / "evidence" / f"qfile-startup-{stamp}"
    out.mkdir(parents=True, exist_ok=False)
    (out / "probe.js").write_bytes(PROBE.read_bytes())

    record = {
        "kind": "retail-startup-native-observation",
        "schema": 1,
        "captured_utc": stamp,
        "retail": str(RETAIL),
        "retail_sha256": retail_sha,
        "probe": str(PROBE),
        "probe_sha256": probe_sha,
        "arguments": ["window", f"level{args.level}"],
        "requested_cwd": REQUESTED_CWD,
        "frida_python": frida.__version__,
        "result": "failed",
        "limitations": [
            "Retail only; no candidate runtime evidence exists because the candidate throws at 0x00402930.",
            "This is not a paired differential and does not prove candidate parity.",
            "Hooks are installed before resume but late thread creation can still miss very early thread-local calls.",
            "The game is launched as a normal startup + level-1 load and left stationary; no movement input is sent.",
            "Table reads are raw reads of known retail addresses; undefined record fields are not interpreted.",
        ],
    }
    pid = None
    session = None
    messages = []

    def on_message(message, data):
        messages.append(message)
        with (out / "frida.jsonl").open("a") as handle:
            handle.write(json.dumps(message) + "\n")

    try:
        device = frida.get_device_manager().add_remote_device(args.host)
        existing = game_processes(device)
        if existing:
            raise RuntimeError(f"refusing concurrent game: {existing!r}")
        record["game_processes_before"] = []
        record["frida_server_processes"] = [p.name for p in device.enumerate_processes() if "frida-server" in p.name.lower()]

        pid = device.spawn([str(RETAIL).replace("/", "\\").replace("\\Users", "Z:\\Users", 1), "window", f"level{args.level}"], cwd=REQUESTED_CWD)
        record["pid"] = pid
        session = device.attach(pid)
        script = session.create_script(CWD_JS + PROBE.read_text())
        script.on("message", on_message)
        script.load()
        record["hooks_loaded"] = True

        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            if any(m.get("type") == "send" and m.get("payload", {}).get("type") == "cwd" for m in messages):
                break
            time.sleep(0.1)
        cwd_message = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "cwd"), None)
        if cwd_message is None:
            raise RuntimeError("no observed working directory before resume")
        observed_cwd = cwd_message["payload"]["value"]
        record["observed_cwd"] = observed_cwd
        if observed_cwd.lower().rstrip("\\") != REQUESTED_CWD.lower().rstrip("\\"):
            raise RuntimeError(f"cwd mismatch: {observed_cwd!r}")

        module_message = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "module"), None)
        if module_message is None:
            raise RuntimeError("no module report from probe")
        record["module"] = module_message["payload"]
        if record["module"].get("base", "").lower() != "0x400000":
            raise RuntimeError(f"unexpected image base: {record['module']!r}")

        ready = next((m for m in messages if m.get("type") == "send" and m.get("payload", {}).get("type") == "ready"), None)
        if ready is None:
            raise RuntimeError("probe did not report ready")
        record["ready"] = ready["payload"]
        if record["ready"].get("failures"):
            raise RuntimeError(f"hook failures: {record['ready']['failures']!r}")
        if len(record["ready"].get("installed", [])) != 5:
            raise RuntimeError(f"expected 5 installed hooks, got {record['ready']!r}")

        pre_resume = script.exports_sync.snapshot()
        record["pre_resume_snapshot"] = pre_resume
        record["resumed_after_cwd_hooks_and_module"] = True
        device.resume(pid)

        time.sleep(max(1.0, args.seconds))
        record["post_capture_snapshot"] = script.exports_sync.snapshot()
        record["result"] = "captured"
    except Exception as error:
        record["result"] = "failed"
        record["error"] = str(error)
        record["traceback"] = traceback.format_exc()
        print(f"Capture failed: {error}", file=sys.stderr)
    finally:
        if pid is not None:
            try:
                device.kill(pid)
                record["owned_process_terminated_after_capture"] = True
            except Exception as error:
                record["cleanup_error"] = str(error)
                try:
                    device.kill(pid)
                except Exception:
                    pass
        if session is not None:
            try:
                session.detach()
            except Exception:
                pass
        try:
            record["game_processes_after"] = [p.name for p in game_processes(device)] if 'device' in locals() else []
        except Exception as error:
            record["game_processes_after_error"] = str(error)
        record["frida_messages"] = len(messages)
        (out / "capture.json").write_text(json.dumps(record, indent=2) + "\n")
        print(str(out), flush=True)

    raise SystemExit(0 if record["result"] == "captured" else 1)


if __name__ == "__main__":
    main()
