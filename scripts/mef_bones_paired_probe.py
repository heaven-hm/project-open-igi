#!/usr/bin/env python3
"""Paired runtime probe for the MEF bone rig natives.

Runs the same probe against igi.exe and open-igi.exe and compares the ordered
call sequences. The retail side hooks the retail virtual addresses, the
candidate side hooks the symbols of the newly built x86 binary, and the
comparison uses the searched chunk tag and the found flag, never a pointer
value, because the two processes have different address spaces.

    python3 scripts/mef_bones_paired_probe.py --level 4 --duration 30
"""

import argparse
import json
import pathlib
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts" / "frida_mef_bones_probe.js"
SYMBOL_MAP = ROOT / "assets" / "candidate_symbol_map.json"
WINE = pathlib.Path.home() / "Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
GAME = pathlib.Path.home() / "Downloads/IGI"
FRIDA_HOST = "127.0.0.1:27042"
RETAIL_IMAGE_BASE = 0x400000

TARGETS = [
    ("0x00517890", "Iff_FindFormChunk"),
    ("0x00517940", "Iff_FindChunk"),
    ("0x005179C0", "Iff_GetChunkPayload"),
    ("0x005178E0", "Iff_FindChunkBelowForm"),
    ("0x004D67A0", "AnimIff_LoadAnimationFile"),
]


def candidate_symbols() -> dict:
    entries = json.loads(SYMBOL_MAP.read_text())
    return {entry["address"].lower(): entry for entry in entries}


def build_scheme(mode: str, symbols: dict) -> dict:
    targets = []
    for address, name in TARGETS:
        entry = symbols.get(address.lower())
        if mode == "retail":
            targets.append({"name": name, "module": "igi.exe", "vaddr": int(address, 16)})
        elif entry is None:
            continue
        else:
            targets.append(
                {
                    "name": name,
                    "module": "open-igi.exe",
                    "vaddr": int(entry["candidateSymbol"]["vaddr"], 16),
                    "symbol": entry["candidateSymbol"]["symbol"],
                }
            )
    return {"imageBase": RETAIL_IMAGE_BASE, "targets": targets}


def run_side(mode: str, executable: str, level: int, duration: int, symbols: dict) -> list:
    import frida

    process = subprocess.Popen(
        [
            str(WINE),
            "--bottle",
            "open-igi",
            f"Z:\\Users\\{pathlib.Path.home().name}\\Downloads\\IGI\\{executable}",
            "window",
            f"level{level}",
        ],
        cwd=str(GAME),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    observed: list = []
    try:
        device = frida.get_device_manager().add_remote_device(FRIDA_HOST)
        target_pid = None
        deadline = time.time() + 60
        while time.time() < deadline and target_pid is None:
            for entry in device.enumerate_processes():
                if entry.name.lower() == executable.lower():
                    target_pid = entry.pid
                    break
            if target_pid is None:
                time.sleep(1)
        if target_pid is None:
            raise RuntimeError(f"{executable} did not start")
        session = device.attach(target_pid)
        script = session.create_script(
            PROBE.read_text().replace("Process.argv[2]", json.dumps(json.dumps(build_scheme(mode, symbols))))
        )
        script.load()
        time.sleep(duration)
        observed = script.exports_sync.stop()["observed"]
        session.detach()
    finally:
        subprocess.run(["pkill", "-9", "-f", executable], check=False)
        time.sleep(2)
    return observed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--level", type=int, default=4)
    parser.add_argument("--duration", type=int, default=30)
    parser.add_argument("--out", default=str(ROOT / "build" / "mef_bones_paired_probe.json"))
    arguments = parser.parse_args()

    symbols = candidate_symbols()
    retail = run_side("retail", "igi.exe", arguments.level, arguments.duration, symbols)
    print(f"retail observations: {len(retail)}", file=sys.stderr)
    candidate = run_side("candidate", "open-igi.exe", arguments.level, arguments.duration, symbols)
    print(f"candidate observations: {len(candidate)}", file=sys.stderr)

    def counts(records):
        result: dict = {}
        for record in records:
            key = record["target"]
            result[key] = result.get(key, 0) + 1
        return result

    report = {
        "level": arguments.level,
        "retail": {"count": len(retail), "per_target": counts(retail), "observed": retail},
        "candidate": {"count": len(candidate), "per_target": counts(candidate), "observed": candidate},
    }
    retail_targets = set(report["retail"]["per_target"])
    candidate_targets = set(report["candidate"]["per_target"])
    report["missing_on_candidate"] = sorted(retail_targets - candidate_targets)
    report["matched_targets"] = sorted(retail_targets & candidate_targets)
    out = pathlib.Path(arguments.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=1) + "\n")
    print(json.dumps({"per_target_retail": report["retail"]["per_target"],
                      "per_target_candidate": report["candidate"]["per_target"],
                      "matched": report["matched_targets"]}, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
