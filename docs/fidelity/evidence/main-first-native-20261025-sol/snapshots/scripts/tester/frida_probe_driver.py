#!/usr/bin/env python3
"""Bounded Frida call-capture driver for one native in either IGI executable.

The same probe file is injected into retail `igi.exe` and the candidate
`igi1.exe`. The driver spawns the game suspended with an explicit game-data
working directory, installs the per-native hook before the first instruction
runs, resumes, observes for a bounded interval, then terminates only the process
it spawned. A late-attach mode exists but is marked as missing startup calls.

Never touches the other side's evidence, never launches a second game while one
is present, and never kills a process it does not own.

Usage (spawn, retail):
  python3 scripts/tester/frida_probe_driver.py \
    --exe /Users/haseeb-mir/Downloads/IGI/igi.exe \
    --probe build/tester/<run>/probes/probe_0x004C1800.js \
    --target 0x004C1800 --name QTaskList_New --level 1 --observe 20 \
    --out build/tester/<run>/runtime/retail-0x004C1800.json \
    --raw build/tester/<run>/runtime/retail-0x004C1800.jsonl \
    --expect-exe-sha256 <sha>

Output kinds: result is one of
  observed            at least one entry was captured
  unobserved          capture completed, native never called in the interval
  hook_failed / blocked / failed   the capture is NOT evidence; read `error`
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
GAME_DIR = Path("/Users/haseeb-mir/Downloads/IGI")
FRIDA_PYTHON = str(Path.home() / "Library" / "Application Support" / "pipx" /
                   "venvs" / "frida-tools" / "bin" / "python")
GAME_NAMES = ("igi.exe", "open-igi.exe", "igi1.exe")


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def win_path(path: Path) -> str:
    return "Z:\\" + str(Path(path).resolve())[1:].replace("/", "\\")


def pe_preferred_base(path: Path) -> int:
    raw = path.read_bytes()
    pe = struct.unpack_from("<I", raw, 0x3C)[0]
    if raw[pe:pe + 4] != b"PE\0\0":
        raise ValueError("not a PE image")
    magic = struct.unpack_from("<H", raw, pe + 24)[0]
    if magic != 0x10B:
        raise ValueError("expected PE32 image")
    return struct.unpack_from("<I", raw, pe + 24 + 28)[0]


def game_process_names(device) -> list[str]:
    names = []
    for process in device.enumerate_processes():
        low = process.name.lower()
        if low in GAME_NAMES or low.startswith("open-igi-probe-"):
            names.append(process.name)
    return names


def frida_module():
    try:
        import frida  # noqa: F401
        return frida
    except ImportError:
        if Path(sys.executable).resolve() == Path(FRIDA_PYTHON).resolve():
            raise
        os.execv(FRIDA_PYTHON, [FRIDA_PYTHON, str(Path(__file__).resolve()), *sys.argv[1:]])


def parse_watch(text: str):
    parts = text.split(":")
    if len(parts) != 3:
        raise argparse.ArgumentTypeError("watch must be ADDRESS:OFFSET:LENGTH")
    address, offset, length = (int(part, 0) for part in parts)
    if not 0 <= offset <= 4096 or not 1 <= length <= 4096:
        raise argparse.ArgumentTypeError("watch offset 0..4096, length 1..4096")
    return {"address": address, "offset": offset, "length": length}


def parse_return_mem(text: str | None):
    if not text or text.lower() == "none":
        return None
    parts = text.split(":")
    if len(parts) != 2:
        raise argparse.ArgumentTypeError("return-mem must be OFFSET:LENGTH or none")
    offset, length = (int(part, 0) for part in parts)
    if offset < 0:
        raise argparse.ArgumentTypeError("return-mem offset must be >= 0")
    if not 1 <= length <= 4096:
        raise argparse.ArgumentTypeError("return-mem length must be 1..4096")
    return {"offset": offset, "length": length}


def parse_mem_arg(text: str | None):
    if not text or text.lower() == "none":
        return None
    parts = text.split(":")
    if len(parts) != 3:
        raise argparse.ArgumentTypeError("mem-arg must be INDEX:OFFSET:LENGTH or none")
    index, offset, length = (int(part, 0) for part in parts)
    if not 0 <= index <= 15:
        raise argparse.ArgumentTypeError("mem-arg index must be 0..15")
    if offset < 0:
        raise argparse.ArgumentTypeError("mem-arg offset must be >= 0")
    if not 1 <= length <= 4096:
        raise argparse.ArgumentTypeError("mem-arg length must be 1..4096")
    return {"index": index, "offset": offset, "length": length}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--exe", type=Path, required=True, help="igi.exe or igi1.exe")
    ap.add_argument("--probe", type=Path, help="single probe file (with --target/--name)")
    ap.add_argument("--target", help="retail VA of the probe's native")
    ap.add_argument("--name")
    ap.add_argument("--hook", action="append", default=[], metavar="PROBE:TARGET:NAME",
                    help="hook to install for this session; repeatable. Either --hook or --probe is required")
    ap.add_argument("--level", type=int, default=1, choices=range(1, 15))
    ap.add_argument("--observe", type=float, default=20.0, help="observation seconds after resume")
    ap.add_argument("--out", type=Path, required=True)
    ap.add_argument("--raw", type=Path, help="JSONL of every Frida message (default: beside --out)")
    ap.add_argument("--mode", choices=("spawn", "attach"), default="spawn")
    ap.add_argument("--game-dir", type=Path, default=GAME_DIR)
    ap.add_argument("--expect-exe-sha256", help="refuse to capture when the executable differs")
    ap.add_argument("--arg-words", type=int, default=4)
    ap.add_argument("--native-size", type=lambda value: int(value, 0),
                    help="Reviewed native extent in bytes for probes that require an exact caller range")
    ap.add_argument("--mem-arg", type=parse_mem_arg, default=None,
                    help="INDEX:OFFSET:LENGTH pointer argument to snapshot, or none")
    ap.add_argument("--watch", action="append", type=parse_watch, default=[],
                    metavar="ADDRESS:OFFSET:LENGTH",
                    help="absolute retail VA to snapshot at entry and return; repeatable")
    ap.add_argument("--return-mem", type=parse_return_mem, default=None,
                    help="OFFSET:LENGTH bounded snapshot of the pointer in EAX on return, or none")
    ap.add_argument("--max-calls", type=int, default=256)
    ap.add_argument("--process-name", help="attach-mode target process name (default: executable name)")
    ap.add_argument("--probe-input-json", type=Path,
                    help="Diagnostic probe input recorded from retail; passed unchanged as layout.evidenceInput")
    ap.add_argument('--diagnostic-resume-before-hook', action='store_true',
                    help='Replay diagnostics only: resume before RPC; earlier startup calls remain unobserved')
    ap.add_argument('--stream-probe-cases', action='store_true',
                    help='Send recorded cases in ordered batches instead of one large setup RPC')
    ap.add_argument("--game-arg", action="append", default=[],
                    help="extra command-line token appended after 'window level<N>' (e.g. WinInput)")
    args = ap.parse_args()
    if args.diagnostic_resume_before_hook and args.mode != 'spawn':
        ap.error('diagnostic early resume requires owned spawn mode')
    if args.native_size is not None and not 1 <= args.native_size <= 65536:
        ap.error('--native-size must be 1..65536 bytes')

    args.out.parent.mkdir(parents=True, exist_ok=True)
    raw_path = args.raw or args.out.with_suffix(".jsonl")
    if not args.exe.is_file():
        raise SystemExit(f"executable missing: {args.exe}")
    hook_specs = []
    for item in args.hook:
        parts = item.split(":")
        if len(parts) not in (3, 4):
            raise SystemExit(f"--hook must be PROBE:TARGET:NAME[:SIDE_ADDRESS]: {item}")
        hook_specs.append(dict(probe=Path(parts[0]), target=parts[1], name=parts[2],
                               side_target=parts[3] if len(parts) == 4 else parts[1]))
    if args.probe is not None:
        if not (args.target and args.name):
            raise SystemExit("--probe requires --target and --name")
        hook_specs.append(dict(probe=args.probe, target=args.target, name=args.name,
                               side_target=args.target))
    if not hook_specs:
        raise SystemExit("at least one --probe/--target/--name or --hook is required")
    for spec in hook_specs:
        if not spec["probe"].is_file():
            raise SystemExit(f"probe missing: {spec['probe']}")
    if not (2.0 <= args.observe <= 300.0):
        raise SystemExit("observe must be 2..300 seconds")
    args.max_calls = max(1, min(65536, args.max_calls))
    return_mem = args.return_mem

    exe_sha = sha256(args.exe)
    evidence_input = None
    evidence_input_identity = None
    replay_cases = None
    if args.probe_input_json:
        if args.probe_input_json.stat().st_size > 8 * 1024 * 1024:
            raise SystemExit('probe input exceeds 8 MiB')
        evidence_input = json.loads(args.probe_input_json.read_text())
        evidence_input_identity = dict(path=str(args.probe_input_json.resolve()),
                                       sha256=sha256(args.probe_input_json))
    if args.stream_probe_cases:
        if not isinstance(evidence_input, dict) or not isinstance(evidence_input.get('cases'), list):
            raise SystemExit('streaming requires a probe-input JSON object with cases')
        replay_cases = evidence_input['cases']
        if not 1 <= len(replay_cases) <= 65536:
            raise SystemExit('streamed case count must be 1..65536')
        evidence_input = {k: v for k, v in evidence_input.items() if k != 'cases'}
        evidence_input.update(caseCount=len(replay_cases), casesStreamed=True)
    if args.expect_exe_sha256 and exe_sha != args.expect_exe_sha256.lower():
        print(f"BLOCKED: executable sha256 {exe_sha} != expected {args.expect_exe_sha256}", file=sys.stderr)
        return 1
    preferred_base = pe_preferred_base(args.exe)

    frida = frida_module()
    config_path = ROOT / "scripts/igi-workflow.json"
    config = json.loads(config_path.read_text()) if config_path.is_file() else {}
    host = os.environ.get("FRIDA_HOST", config.get("frida_host", "127.0.0.1:27042"))

    report = dict(
        kind="igi-tester-native-probe",
        schema_version=1,
        result="blocked",
        verified=False,
        captured_at=datetime.now(timezone.utc).isoformat(),
        targets=[dict(address=spec["target"], name=spec["name"],
                      probe=str(spec["probe"].resolve()),
                      probe_sha256=sha256(spec["probe"])) for spec in hook_specs],
        executable=dict(path=str(args.exe.resolve()), sha256=exe_sha,
                        preferred_base=f"0x{preferred_base:08X}", module=args.exe.name),
        probe_template_sha256=(sha256(Path(__file__).parent / "frida_native_probe_template.js")
                               if (Path(__file__).parent / "frida_native_probe_template.js").is_file() else None),
        environment=dict(
            frida_host=host,
            frida_client=getattr(frida, "__version__", "unknown"),
            game_directory=str(args.game_dir.resolve()),
            level=args.level,
            arguments=["window", f"level{args.level}", *args.game_arg],
            scenario=dict(observe_seconds=args.observe, scripted_actions="none (idle observation)"),
        ),
        arguments=dict(arg_words=args.arg_words, native_size=args.native_size, mem_arg=args.mem_arg,
                       return_mem=return_mem, watch=args.watch, max_calls=args.max_calls),
        hooks=[],
        evidence_input=evidence_input_identity,
        entries=[],
        returns=[],
        messages=0,
        summary=None,
        limitations=[
            "Finite idle observation interval only; no scripted gameplay actions.",
            "Only the process this driver spawned is terminated; attach mode never terminates the target.",
            "A pointer snapshot is read only after Process.findRangeByAddress proves the range readable; "
            "unknown pointer contents remain unobserved rather than guessed.",
            "Observed calls are not proof of behavioral parity by themselves; a typed comparator is required.",
        ],
    )
    if args.mode == "attach":
        report["limitations"].append(
            "Late attach: calls executed before attachment (including startup) are unobserved.")

    observations: list[dict] = []
    failures: list[dict] = []
    pid = None
    device = None
    raw_stream = raw_path.open("w")

    def on_message(message, data):
        raw_stream.write(json.dumps({"monotonic": time.time(), "message": message,
                                     "data": (data.hex() if data is not None else None)}) + "\n")
        raw_stream.flush()
        report["messages"] += 1
        if message.get("type") == "error":
            failures.append({"kind": "script-error", "message": message})
            return
        payload = message.get("payload") or {}
        kind = payload.get("type")
        if kind in ("observed", "entry", "return"):
            observations.append(payload)
            is_entry = kind == "entry" or (kind == "observed" and payload.get("phase") == "entry")
            (report["entries"] if is_entry else report["returns"]).append(payload)
        elif kind == "hook_failed":
            failures.append({"kind": "hook_failed", "message": payload})
        elif kind == "overflow":
            failures.append({"kind": "call-overflow", "message": payload})
        elif kind is not None:
            # Typed registrar/chain events (registration, chain_call, ...) stay in
            # observed order so a gate comparator can consume them.
            report.setdefault("events", []).append(payload)

    try:
        device = frida.get_device_manager().add_remote_device(host)
        present = game_process_names(device)
        if present:
            raise RuntimeError(f"another game process is already running: {present}; refusing to overlap captures")
        if args.mode == "spawn":
            spawn_args = [win_path(args.exe), "window", f"level{args.level}", *args.game_arg]
            pid = device.spawn(spawn_args, cwd=win_path(args.game_dir))
            report["capture"] = dict(mode="spawn-suspended", owned_pid=pid,
                                     arguments=spawn_args, cwd=win_path(args.game_dir))
            args.out.with_suffix('.owned.json').write_text(json.dumps(dict(
                kind='igi-owned-capture-start', pid=pid, executable=report['executable'],
                capture=report['capture'], captured_at=report['captured_at']), indent=2) + '\n')
            print(f'DEBUG attaching owned PID {pid}', flush=True)
            session = device.attach(pid)
            print('DEBUG attached', flush=True)
        else:
            name = args.process_name or args.exe.name
            pid = device.get_process(name).pid
            report["capture"] = dict(mode="attach-late", pid=pid, process_name=name)
            session = device.attach(pid)
        detach_meta: dict = {}

        def on_detached(*callback_args):
            reason = callback_args[0] if callback_args else None
            crash = callback_args[1] if len(callback_args) > 1 else None
            detach_meta.update(dict(reason=str(reason), at=time.time(),
                                    crash=(str(crash) if crash else None)))
        session.on("detached", on_detached)
        scripts = []
        resumed = False
        for spec in hook_specs:
            script = session.create_script(spec["probe"].read_text())
            script.on("message", on_message)
            print('DEBUG script load start', flush=True)
            script.load()
            print('DEBUG script load done', flush=True)
            layout = dict(preferredBase=preferred_base, argWords=args.arg_words,
                          memArg=args.mem_arg, maxCalls=args.max_calls,
                          returnMem=return_mem, watch=args.watch,
                          moduleName=args.exe.name, nativeSize=args.native_size)
            layout['evidenceInput'] = evidence_input
            if args.diagnostic_resume_before_hook and not resumed:
                device.resume(pid)
                resumed = True
                report['capture']['diagnostic_resume_before_hook'] = True
                report['limitations'].append(
                    'Diagnostic early resume: startup before hook installation is unobserved; '
                    'this mode cannot establish startup-call parity.')
            print('DEBUG typed hook RPC start', flush=True)
            hook = script.exports_sync.hook(spec["side_target"], layout)
            print('DEBUG typed hook RPC done', flush=True)
            hook["name"] = spec["name"]
            hook["retailVA"] = spec["target"]
            hook["sideAddress"] = spec["side_target"]
            report["hooks"].append(hook)
            scripts.append(script)
            if hook.get('pendingReplay'):
                total = hook.get('replayTotal')
                if not isinstance(total, int) or not 1 <= total <= 65536:
                    raise ValueError('invalid diagnostic replay count')
                previous = 0
                for _ in range(256):
                    progress = (script.exports_sync.replaynext(replay_cases[previous:previous+256])
                                if replay_cases is not None else script.exports_sync.replaynext())
                    completed = progress.get('completed')
                    if (progress.get('total') != total or not isinstance(completed, int) or
                            not previous < completed <= total):
                        raise ValueError('invalid or stalled diagnostic replay progress')
                    previous = completed
                    report.setdefault('replay_progress', []).append(progress)
                    print(f'DEBUG recorded-input replay {completed}/{total}', flush=True)
                    if progress.get('done') is True:
                        if completed != total:
                            raise ValueError('diagnostic replay stopped before all inputs')
                        break
                else:
                    raise ValueError('diagnostic replay did not finish within 256 bounded batches')
        if args.mode == "spawn" and not resumed:
            device.resume(pid)
        observe_started = time.time()
        observe_deadline = observe_started + args.observe
        exited_at = None
        while time.time() < observe_deadline:
            time.sleep(min(0.5, max(0.0, observe_deadline - time.time())))
            try:
                if not any(proc.pid == pid for proc in device.enumerate_processes()):
                    exited_at = time.time()
                    break
            except Exception:
                pass
        if detach_meta:
            report.setdefault("capture", {})["session_detach"] = detach_meta
        if exited_at is not None:
            report.setdefault("capture", {})["exited_before_observe_end"] = dict(
                at=exited_at, elapsed_seconds=round(exited_at - observe_started, 3))
        else:
            report.setdefault("capture", {})["exited_before_observe_end"] = None
        for index, script in enumerate(scripts):
            print(f"DEBUG summary {index} start", flush=True)
            try:
                report.setdefault("summaries", []).append(script.exports_sync.summary())
                print(f"DEBUG summary {index} done", flush=True)
            except Exception as error:  # summary is supplementary
                report.setdefault("summary_errors", []).append(
                    {"probe": report["targets"][index]["probe"], "error": f"{type(error).__name__}: {error}"})
            try:
                print(f"DEBUG detach {index} start", flush=True)
                script.exports_sync.detach()
                print(f"DEBUG detach {index} done", flush=True)
            except Exception:
                pass
        print("DEBUG session detach start", flush=True)
        # The main-prefix probe keeps forty import/native hooks on the running
        # game. Frida 17 occasionally never returns from session.detach() once
        # the observed process is idle in its message loop. Cleanup is not
        # evidence, so bound the wait and continue; the spawned process is
        # terminated in the finally block either way.
        import threading
        detacher = threading.Thread(target=session.detach, daemon=True)
        detacher.start()
        detacher.join(5.0)
        print("DEBUG session detach done" if not detacher.is_alive() else
              "DEBUG session detach timed out; process will be terminated", flush=True)
        by_target = {}
        for payload in observations:
            address = payload.get("address")
            bucket = by_target.setdefault(address, dict(name=None, entries=0, returns=0))
            if payload.get("phase") == "entry":
                bucket["entries"] += 1
            else:
                bucket["returns"] += 1
        for target in report["targets"]:
            bucket = by_target.get(target["address"], dict(entries=0, returns=0))
            target["observations"] = bucket
            target["retailName"] = target["name"]
        entry_count = len(report["entries"])
        report["counts"] = dict(
            entry=len(report["entries"]),
            return_=len(report["returns"]),
            unmatched_entries=entry_count - len(report["returns"]),
        )
        report["result"] = "observed" if entry_count else "unobserved"
        report["limitations"].append(
            "Result 'unobserved' means only that this capture did not reach the native; it is not a pass or fail.")
        if failures:
            report["failures"] = failures
        return_code = 0
    except Exception as error:
        report["result"] = "failed"
        report["error"] = f"{type(error).__name__}: {error}"
        report["failures"] = failures
        return_code = 1
    finally:
        raw_stream.close()
        if device is not None and pid is not None:
            try:
                alive = [p.name for p in device.enumerate_processes() if p.pid == pid]
                report.setdefault("capture", {})["alive_at_end"] = bool(alive)
            except Exception as error:
                report.setdefault("cleanup_errors", []).append(f"liveness: {error}")
        if device is not None and pid is not None and args.mode == "spawn":
            try:
                device.kill(pid)
            except Exception as error:
                report.setdefault("cleanup_errors", []).append(f"kill: {error}")
            deadline = time.time() + 20
            while time.time() < deadline:
                try:
                    remaining = [p.name for p in device.enumerate_processes()
                                 if p.pid == pid]
                except Exception as error:
                    report.setdefault("cleanup_errors", []).append(f"enumerate: {error}")
                    break
                if not remaining:
                    report["cleanup"] = dict(owned_process_exited=True)
                    break
                time.sleep(0.5)
            else:
                report.setdefault("cleanup_errors", []).append(
                    "owned process still listed after 20s; not killed by another route")
        args.out.write_text(json.dumps(report, indent=2) + "\n")
        first = report["targets"][0] if report["targets"] else {"address": "?", "name": "?"}
        print(f"{report['result']}: {first['address']} {first['name']} entries={len(report['entries'])} "
              f"returns={len(report['returns'])}")
        print(f"report: {args.out}")
    return return_code


if __name__ == "__main__":
    raise SystemExit(main())
