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
CANDIDATE_DISPATCH = "0x0042C630"   # QTask_OpenEvent
CANDIDATE_LEVEL_START = "0x0042C8A0"  # Level_Start


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

    print("launching retail igi.exe and OpenIGI side by side ...", flush=True)
    retail_process = nfd.vnb.start_game(
        "Z:" + str(retail).replace("/", "\\"), args.level,
        game_directory=retail.resolve().parent)
    try:
        with stage_candidate(candidate, retail.resolve().parent) as staged:
            # Both games share one asset directory; the candidate keeps its
            # stage log apart so the retail log stays a retail-only record.
            nfd.os.environ["IGI_SCENE_LOG"] = "open-igi-scene.log"
            # The candidate's linked FX_Igimutex uses OPENIGIMUTEX_TEST, so it
            # can run beside retail's IGIMUTEX.
            candidate_process = nfd.vnb.start_game(
                "Z:" + str(staged).replace("/", "\\"), args.level,
                game_directory=retail.resolve().parent)
            try:
                retail_result: dict = {}
                candidate_result: dict = {}
                threads = [
                    threading.Thread(target=attach,
                                     args=("igi.exe", [RETAIL_DISPATCH,
                                                       RETAIL_LEVEL_START],
                                           retail_result)),
                    threading.Thread(target=attach,
                                     args=(staged.name.lower(),
                                           [CANDIDATE_DISPATCH,
                                            CANDIDATE_LEVEL_START],
                                           candidate_result)),
                ]
                for thread in threads:
                    thread.start()
                for thread in threads:
                    thread.join()
            finally:
                nfd.vnb.stop_game(candidate_process)
    finally:
        nfd.vnb.stop_game(retail_process)
        for name in ("igi.exe", "open-igi.exe"):
            for pid in nfd.frida_list().get(name, []):
                subprocess.run([frida_kill, "-H", frida_host, str(pid)],
                               capture_output=True, text=True, timeout=30)

    for result in (retail_result, candidate_result):
        if result.get("error"):
            raise RuntimeError(result["error"])
    remaining = {name: nfd.frida_list().get(name, [])
                 for name in ("igi.exe", "open-igi.exe")}
    if any(remaining.values()):
        raise RuntimeError(f"game processes remain alive: {remaining}")

    retail_obs = retail_result["observed"]
    candidate_obs = candidate_result["observed"]
    retail_dispatch = observed_for(retail_obs, RETAIL_DISPATCH)
    candidate_dispatch = observed_for(candidate_obs, CANDIDATE_DISPATCH)
    comparison = typed_pair(retail_dispatch, candidate_dispatch)
    report = {
        "tool": "frida",
        "captureMode": "concurrent",
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
            "Both games run at once with different mutex names; timing and frame counts still differ per game.",
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
