#!/usr/bin/env python3
"""Refresh only the Radare2 CFG section of existing static capture records.

When the r2 invocation or parsing is corrected, the BN/angr parts of a capture
do not need re-querying. This re-runs r2 for each recorded address, parses the
CFG with the current normalization, and recomputes cfg_cross_check and the
evidence-vs-live comparison against the repository evidence artifacts.
"""
from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load_module():
    spec = importlib.util.spec_from_file_location("spc", ROOT / "scripts/tester/static_probe_capture.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--address", action="append", default=[])
    args = ap.parse_args()
    spc = load_module()
    files = sorted((args.run_dir / "static").glob("*-static.json"))
    for path in files:
        record = json.loads(path.read_text())
        address = record["address"]
        if args.address and address.lower() not in {item.lower() for item in args.address}:
            continue
        retail = Path(record["retail"]["path"])
        command = ["r2", "-q", "-2", "-c",
                   f"af @ {address}; agfj @ {address}; q", str(retail)]
        r2 = spc.run(command, timeout=180)
        stdout = r2.get("stdout", "")
        record["radare2"] = dict(command=command, exit_code=r2.get("exit_code"),
                                 stderr=r2.get("stderr", ""),
                                 stdout_sha256=hashlib.sha256(stdout.encode()).hexdigest(),
                                 stdout_prefix=stdout[:2000])
        try:
            record["radare2"]["cfg"] = spc.parse_r2_graph(stdout, address)
        except (ValueError, KeyError, TypeError) as error:
            record["radare2"]["cfg_error"] = str(error)
        if record.get("binary_ninja", {}).get("cfg") and record["radare2"].get("cfg"):
            record["cfg_cross_check"] = spc.graph_similarity(
                record["binary_ninja"]["cfg"], record["radare2"]["cfg"])
        if record.get("evidence_cfg", {}).get("radare2") and record["radare2"].get("cfg"):
            evidence = record["evidence_cfg"]["radare2"]
            record["evidence_vs_live_r2"] = spc.graph_similarity(
                dict(nodes=evidence["nodes"], edges=evidence.get("edges", [])),
                record["radare2"]["cfg"])
        path.write_text(json.dumps(record, indent=2) + "\n")
        cross = record.get("cfg_cross_check") or {}
        live = record.get("evidence_vs_live_r2") or {}
        print(f"{address}: r2_nodes={len(record['radare2'].get('cfg', {}).get('nodes', []))} "
              f"cross={cross.get('node_jaccard')}/{cross.get('edge_jaccard')} "
              f"evidence_r2={live.get('node_jaccard')}/{live.get('edge_jaccard')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
