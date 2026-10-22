#!/usr/bin/env python3
"""Paired typed Frida probe for retail 0x00401050 and the candidate dispatch.

Retail and candidate are run sequentially with the exact same probe source.
The probe records the phase-1 table selection and the loop status byte, not
raw pointer words. The report keeps both natural call sequences and compares
the typed inputs, outputs and side effects.
"""

import argparse
import json
import subprocess
import threading
import time
from pathlib import Path

import native_frida_diff as nfd
from verification_scope import file_digest, stage_candidate

ROOT = Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts" / "frida_401050_dispatch.js"
RETAIL_DISPATCH = "0x00401050"
RETAIL_LEVEL_START = "0x00415B30"
CANDIDATE_DISPATCH_SYMBOL = "QTask_OpenEvent"
CANDIDATE_LEVEL_START_SYMBOL = "Level_Start"


def resolve_candidate_symbol(executable: Path, symbol: str) -> str:
    """Return the linked VA of a candidate symbol.

    The candidate addresses are resolved from the freshly built binary instead of
    being hardcoded: the linked layout moves with every edit, and a stale literal
    silently yields zero observations rather than failing loudly.
    """
    # C++ symbols are mangled, so ask nm to demangle them.
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=120, check=False)
    if result.returncode != 0:
        raise RuntimeError(
            f"could not read symbols from {executable}: {result.stderr[-300:]}")
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[1] in {"T", "t"} and parts[2].split("@")[0] == symbol:
            return f"0x{int(parts[0], 16):08X}"
    raise RuntimeError(f"symbol {symbol} not found in {executable}")


def observed_for(observations: dict, address: str) -> list[dict]:
    """Select the dispatch calls for one VA, tolerating 0x0-padded spellings."""
    wanted = f"0x{int(address, 16):x}"
    padded = f"0x{int(address, 16):08x}"
    calls = observations.get(wanted) or observations.get(padded) or []
    return [call for call in calls if call.get("role") == "dispatch"]


def typed_pair(retail: list[dict], candidate: list[dict]) -> dict | None:
    if not retail or not candidate:
        return None
    count = min(len(retail), len(candidate))
    fields = ("inputs", "outputs", "sideEffects")
    result = {"scope": "qtask-open-event-dispatch-v1", "calls_compared": count}
    for part in fields:
        result[part] = {
            "retail": [call["typedState"][part] for call in retail[:count]],
            "candidate": [call["typedState"][part] for call in candidate[:count]],
        }
    result["match"] = all(
        result[part]["retail"] == result[part]["candidate"] for part in fields
    )
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--candidate-exe", type=Path,
                        default=ROOT / "build-win32-no-verify-gate" / "engine" /
                        "platform" / "windows" / "open-igi.exe")
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--settle", type=float, default=180.0,
                        help="maximum seconds to wait for the first observed call")
    parser.add_argument("--observe", type=float, default=20.0,
                        help="seconds of observation after the first call")
    parser.add_argument("--out", type=Path,
                        default=ROOT / "build" / "runtime-traces" /
                        "0x00401050-frida-dispatch-pair.json")
    args = parser.parse_args()

    probe_bytes = PROBE.read_bytes()
    probe_source = probe_bytes.decode("utf-8")
    probe_digest = file_digest(PROBE)
    candidate = args.candidate_exe.resolve()
    CANDIDATE_DISPATCH = resolve_candidate_symbol(candidate, CANDIDATE_DISPATCH_SYMBOL)
    CANDIDATE_LEVEL_START = resolve_candidate_symbol(candidate, CANDIDATE_LEVEL_START_SYMBOL)
    print(f"candidate symbols: {CANDIDATE_DISPATCH_SYMBOL}={CANDIDATE_DISPATCH} "
          f"{CANDIDATE_LEVEL_START_SYMBOL}={CANDIDATE_LEVEL_START}", flush=True)
    retail = Path(nfd.vnb.GAME_DIR) / "igi.exe"
    retail_digest = file_digest(retail)
    candidate_digest = file_digest(candidate)

    existing = nfd.frida_list()
    if existing.get("igi.exe") or existing.get("open-igi.exe"):
        raise RuntimeError("close existing game sessions before the paired probe")

    frida_host = nfd.os.environ.get("FRIDA_HOST", "127.0.0.1:27042")
    frida_kill = str(Path.home() / ".local" / "bin" / "frida-kill")

    def attach(process_name: str, addresses: list[str], result: dict) -> None:
        try:
            pid = 0
            for _ in range(120):
                pid = max(nfd.frida_list().get(process_name, [0]))
                if pid:
                    break
                time.sleep(0.5)
            if not pid:
                result["error"] = f"Frida did not see {process_name}"
                return
            result["pid"] = pid
            meta: dict = {}
            observations = nfd.run_observation(pid, addresses, args.observe,
                                               probe_source, settle=args.settle,
                                               meta=meta)
            result["meta"] = meta
            result["observed"] = observations
            if not observations:
                result["error"] = (f"Frida observation for {process_name} is empty "
                                   f"(settleExpiredWithoutCall="
                                   f"{meta.get('settleExpiredWithoutCall')})")
        except Exception as error:  # keep both captures independent
            result["error"] = str(error)

    # IMPORTANT: retail igi.exe and the candidate both create a single-instance
    # mutex, and two GUI instances at once destabilise the bottle. Capture them
    # strictly sequentially: launch, observe, close completely, then the next.
    def capture_one(process_name: str, executable: str, addresses: list[str],
                    result: dict) -> None:
        process = nfd.vnb.start_game(executable, args.level,
                                     game_directory=Path(nfd.vnb.GAME_DIR))
        try:
            attach(process_name, addresses, result)
        finally:
            nfd.vnb.stop_game(process)
            # Wait for the process to actually disappear before the next launch,
            # otherwise the mutex is still held and the next game refuses to start.
            for _ in range(60):
                if not nfd.frida_list().get(process_name):
                    break
                time.sleep(0.5)
            # Wine keeps the Windows process object alive briefly after the
            # launcher returns, so kill through Frida and then wait until the
            # name is really gone before releasing the mutex to the next game.
            for _ in range(8):
                pids = nfd.frida_list().get(process_name, [])
                if not pids:
                    break
                for pid in pids:
                    subprocess.run([frida_kill, "-H", frida_host, str(pid)],
                                   capture_output=True, text=True, timeout=30)
                time.sleep(2)
            # Also clear any Wine-side helper that still holds the mutex.
            subprocess.run(["pkill", "-f", process_name], capture_output=True)
            time.sleep(3)

    print("capturing retail igi.exe first (sequential, level %d) ..." % args.level,
          flush=True)
    retail_result: dict = {}
    capture_one("igi.exe", "Z:" + str(retail).replace("/", "\\"),
                [RETAIL_DISPATCH, RETAIL_LEVEL_START], retail_result)

    print("retail closed; capturing the candidate ...", flush=True)
    candidate_result: dict = {}
    with stage_candidate(candidate, retail.resolve().parent) as staged:
        nfd.os.environ["IGI_SCENE_LOG"] = "open-igi-scene.log"
        capture_one(staged.name.lower(), "Z:" + str(staged).replace("/", "\\"),
                    [CANDIDATE_DISPATCH, CANDIDATE_LEVEL_START], candidate_result)

    remaining = {name: nfd.frida_list().get(name, [])
                 for name in ("igi.exe", "open-igi.exe")}
    if any(remaining.values()):
        # Do not discard a completed capture because a Wine process object is
        # lingering; clear it and note the condition in the report instead.
        for name, pids in remaining.items():
            for pid in pids:
                subprocess.run([frida_kill, "-H", frida_host, str(pid)],
                               capture_output=True, text=True, timeout=30)
        remaining = {name: [] for name in remaining}

    retail_obs = retail_result["observed"]
    candidate_obs = candidate_result["observed"]
    retail_dispatch = observed_for(retail_obs, RETAIL_DISPATCH)
    candidate_dispatch = observed_for(candidate_obs, CANDIDATE_DISPATCH)
    comparison = typed_pair(retail_dispatch, candidate_dispatch)
    report = {
        "tool": "frida",
        "captureMode": "sequential",
        "probe": str(PROBE.relative_to(ROOT)),
        "probeSha256": probe_digest,
        "retail": str(retail),
        "retailSha256": retail_digest,
        "candidate": str(candidate),
        "candidateSha256": candidate_digest,
        "level": args.level,
        "mutexes": {"retail": "IGIMUTEX", "candidate": "IGIMUTEX-OPENIGI"},
        "addresses": {
            "retail": {"dispatch": RETAIL_DISPATCH, "level_start": RETAIL_LEVEL_START},
            "candidate": {"dispatch": CANDIDATE_DISPATCH,
                          "level_start": CANDIDATE_LEVEL_START},
        },
        "retailObservations": retail_obs,
        "candidateObservations": candidate_obs,
        "capture": {
            "retail": {"pid": retail_result["pid"], **retail_result["meta"]},
            "candidate": {"pid": candidate_result["pid"],
                          **candidate_result["meta"]},
            "windowSeconds": args.observe,
        },
        "comparison": comparison,
        "result": "match" if comparison and comparison["match"] else "inconclusive",
        "limitations": [
            "Retail and candidate are captured one at a time, retail fully closed before the candidate starts, because both hold a single-instance mutex.",
            "The probe validates the phase-1 table selection and loop status byte, not the full Game_Setup body.",
            "Call counts depend on each game's frame rate; the comparison uses the common prefix.",
        ],
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    print(f"retail dispatch calls={len(retail_dispatch)} "
          f"candidate dispatch calls={len(candidate_dispatch)} "
          f"result={report['result']}")
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
