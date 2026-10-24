#!/usr/bin/env python3
"""Alias-table spawn-suspended retail observation.

Retail only. Spawns IGI.EXE suspended with the game directory as cwd, verifies
cwd and image base, installs all hooks before resume, runs startup + level 1 for
a bounded interval, snapshots creator records and populator/lookup state, then
kills the owned PID.

Targets:
  - 0x004B19C0 record creator and 0x004B1A90 walker record layout
  - 0x004B12F0 populator of 0x00942330 and its caller chain
  - 0x004B1510 caller of 0x004B1620 and the immediate use of its return value

Raw evidence is written under docs/fidelity/evidence/ (repo-visible); the probe
script is copied alongside it. Nothing is committed here.
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
PROBE = ROOT / "scripts" / "tester" / "frida_qfile_alias_probe.js"
GAME_PROCESS_NAMES = {"igi.exe", "igi1.exe", "open-igi.exe"}
REQUESTED_CWD = "Z:\\Users\\haseeb-mir\\Downloads\\IGI"
EXPECTED_HOOKS = 3

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
    parser.add_argument("--seconds", type=float, default=32.0)
    parser.add_argument("--host", default="127.0.0.1:27042")
    parser.add_argument("--out", default=None)
    args = parser.parse_args()

    retail_sha = sha256(RETAIL)
    if retail_sha != EXPECTED_RETAIL_SHA256:
        raise SystemExit(f"retail hash mismatch: {retail_sha}")
    probe_sha = sha256(PROBE)
    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    out = pathlib.Path(args.out) if args.out else ROOT / "docs" / "fidelity" / "evidence" / f"qfile-alias-{stamp}"
    out.mkdir(parents=True, exist_ok=False)
    (out / "probe.js").write_bytes(PROBE.read_bytes())

    record = {
        "kind": "retail-startup-native-observation",
        "schema": 1,
        "round": 218,
        "captured_utc": stamp,
        "retail": str(RETAIL),
        "retail_sha256": retail_sha,
        "probe": str(PROBE),
        "probe_sha256": probe_sha,
        "arguments": ["window", f"level{args.level}"],
        "requested_cwd": REQUESTED_CWD,
        "frida_python": frida.__version__,
        "result": "failed",
        "hook_addresses": [
            "0x004B11A0 QFile_DeviceIndex",
            "0x004B12F0 QFile_RegisterAlias",
            "0x004B1020 QFile_AliasResolve",
        ],
        "limitations": [
            "Retail only; no candidate runtime evidence exists because the candidate fails closed at 0x00402930.",
            "This is not a paired differential and does not prove candidate parity.",
            "Hooks are installed before resume; very early calls could still be missed.",
            "Table snapshots are raw reads of 0x00942330..0x009435B0; fields are reported exactly as found.",
            "The fixed buffer at 0x00943A18 is read up to 0x100 bytes; its static extent is not proven, so a boundary hit would only be reported as an observed maximum.",
            "The game is launched as a normal startup + level-1 load and left stationary; no movement input is sent.",
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

        pid = device.spawn([
            str(RETAIL).replace("/", "\\").replace("\\Users", "Z:\\Users", 1),
            "window",
            f"level{args.level}",
        ], cwd=REQUESTED_CWD)
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
        if len(record["ready"].get("installed", [])) != EXPECTED_HOOKS:
            raise RuntimeError(f"expected {EXPECTED_HOOKS} installed hooks, got {record['ready']!r}")

        record["pre_resume_snapshot"] = script.exports_sync.snapshot()
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
