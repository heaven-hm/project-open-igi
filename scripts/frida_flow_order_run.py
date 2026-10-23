#!/usr/bin/env python3
"""Record the retail flow call order once, for comparison with the port.

Instrumentation only. One game process is launched, the flow entry points are
hooked, and their natural enter/leave order is written as evidence.
"""

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import native_frida_diff as nfd  # noqa: E402

PROBE = ROOT / "scripts" / "frida_flow_order.js"
FLOW_ADDRESSES = {
    "0x00415530": "Game_Init",
    "0x00415b30": "Level_Start",
    "0x00401050": "QTask_OpenEvent",
    "0x004010a0": "QTask_OpenEvent_2",
    "0x00402260": "Flow_Phase1Handler",
    "0x00416260": "Game_Phase1Handler",
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--level", type=int, default=1)
    parser.add_argument("--settle", type=float, default=120.0)
    parser.add_argument("--observe", type=float, default=10.0)
    parser.add_argument("--out", type=Path,
                        default=ROOT / "build" / "runtime-traces" /
                        "retail-flow-order.json")
    args = parser.parse_args()

    retail = Path(nfd.vnb.GAME_DIR) / "igi.exe"
    existing = nfd.frida_list()
    if existing.get("igi.exe"):
        raise RuntimeError("close the running igi.exe before the flow trace")
    if existing.get("open-igi.exe"):
        raise RuntimeError("close the running open-igi.exe before the flow trace")

    process = nfd.vnb.start_game("Z:" + str(retail).replace("/", "\\"),
                                 args.level, game_directory=retail.resolve().parent)
    events: list[dict] = []
    try:
        pid = 0
        for _ in range(120):
            pid = max(nfd.frida_list().get("igi.exe", [0]))
            if pid:
                break
            time.sleep(0.5)
        if not pid:
            raise RuntimeError("Frida did not see igi.exe")

        device = frida.get_device_manager().add_remote_device(
            nfd.os.environ.get("FRIDA_HOST", "127.0.0.1:27042"))
        session = device.attach(pid)
        script = session.create_script(PROBE.read_text())
        script.on("message", lambda message, data: events.append(message.get("payload") or message))
        script.load()
        for address in FLOW_ADDRESSES:
            script.exports_sync.hook(address)
        script.exports_sync.start()

        deadline = time.time() + args.settle
        while time.time() < deadline:
            if any(e.get("address") == "0x401050" for e in events):
                break
            time.sleep(0.2)
        time.sleep(args.observe)
        script.exports_sync.stop()
        time.sleep(0.5)
        session.detach()
    finally:
        nfd.vnb.stop_game(process)
        host = nfd.os.environ.get("FRIDA_HOST", "127.0.0.1:27042")
        for pid in nfd.frida_list().get("igi.exe", []):
            subprocess.run([str(Path.home() / ".local" / "bin" / "frida-kill"),
                            "-H", host, str(pid)], capture_output=True, text=True,
                           timeout=30)
        if nfd.frida_list().get("igi.exe"):
            raise RuntimeError("igi.exe remains alive")

    flow = [e for e in events if e.get("type") == "flow"]
    report = {
        "tool": "frida",
        "probe": str(PROBE.relative_to(ROOT)),
        "retail": str(retail),
        "level": args.level,
        "addresses": FLOW_ADDRESSES,
        "eventCount": len(flow),
        "events": flow,
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    print(f"captured {len(flow)} flow events -> {args.out}")
    seen = {}
    for event in flow:
        seen.setdefault(event["address"], {"enter": 0, "leave": 0})
        if event["phase"] in seen[event["address"]]:
            seen[event["address"]][event["phase"]] += 1
    for address, name in FLOW_ADDRESSES.items():
        counts = seen.get(address, {"enter": 0, "leave": 0})
        print(f"  {name} {address}: enter={counts['enter']} leave={counts['leave']}")


if __name__ == "__main__":
    main()
