#!/usr/bin/env python3
"""Merge a startup (spawn-mode) capture into the canonical paired capture.

The canonical trace is produced by attaching to an image that is already running, so nothing called during startup
can appear in it. The spawn-mode capture installs hooks before the entry point and therefore observes startup code,
but a fresh retail run also re-observes the whole demo with run-to-run variance. Neither file alone is the best
evidence, so this merge keeps the canonical retail observation for every native it already observed (holding that
side fixed) and takes the spawn-phase retail observation only for natives the canonical run never saw. The candidate
side always comes from the newest capture, because a rebuild changes the image the candidate observations belong to.

The verdict is recomputed with the harness's own comparison rules, including the return-comparability analysis, so a
merged capture is exactly as strict as a captured one.
"""
from __future__ import annotations

import argparse
import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))


def load(path: pathlib.Path) -> dict:
    return json.loads(path.read_text())


def aggregate(entry: dict, side: str) -> dict:
    comparison = entry.get("comparison") or {}
    return {
        "hits": entry.get(f"{side}_observed_calls") or 0,
        "ret_classes": comparison.get(f"{side}_ret_classes") or [],
        "arg_patterns": comparison.get(f"{side}_argument_class_patterns") or [],
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--canonical", required=True, type=pathlib.Path)
    parser.add_argument("--startup", required=True, type=pathlib.Path,
                        help="spawn-mode capture whose retail side supplies natives the canonical run missed")
    parser.add_argument("--candidate", required=True, type=pathlib.Path,
                        help="capture of the current candidate build; supplies every candidate observation")
    parser.add_argument("--out", required=True, type=pathlib.Path)
    args = parser.parse_args()

    import frida_mvp_batch as harness

    canonical = load(args.canonical)
    startup = load(args.startup)
    candidate = load(args.candidate)
    if candidate.get("retailSha256") != canonical.get("retailSha256"):
        raise SystemExit("candidate capture was made against a different retail image")
    startup_by = {n["address"]: n for n in startup["natives"]}
    candidate_by = {n["address"]: n for n in candidate["natives"]}

    comparability = candidate.get("returnComparability") or startup.get("returnComparability") or {}
    merged = json.loads(json.dumps(canonical))
    counts = {"matched": 0, "mismatched": 0, "inconclusive": 0}
    added = []
    for entry in merged["natives"]:
        address = entry["address"]
        retail = aggregate(entry, "retail")
        source = "canonical"
        if not retail["hits"]:
            fresh = startup_by.get(address)
            if fresh is not None:
                retail = aggregate(fresh, "retail")
                source = "startup"
                if retail["hits"]:
                    added.append(address)
        newest = candidate_by.get(address)
        observed = aggregate(newest, "candidate") if newest is not None else {"hits": 0, "ret_classes": [],
                                                                            "arg_patterns": []}
        entry["retail_observed_calls"] = retail["hits"]
        entry["candidate_observed_calls"] = observed["hits"]
        entry["comparison"] = harness.compare(retail, observed, comparability.get(entry["retail_va"]))
        entry["comparison"]["retail_observation_source"] = source
        entry["result"] = ("match" if entry["comparison"]["match"]
                           else "inconclusive"
                           if not retail["hits"] or not observed["hits"]
                           else "mismatch")
        counts[{"match": "matched", "mismatch": "mismatched"}.get(entry["result"], "inconclusive")] += 1

    merged["candidate"] = candidate.get("candidate")
    merged["candidateSha256"] = candidate.get("candidateSha256")
    merged["returnComparability"] = comparability
    merged["capture"] = {"retail": dict(canonical.get("capture", {}).get("retail") or {},
                                        reused_from=str(args.canonical)),
                         "candidate": candidate.get("capture", {}).get("candidate")}
    merged["summary"].update({
        "retail_observed": sum(1 for n in merged["natives"] if n["retail_observed_calls"]),
        "candidate_observed": sum(1 for n in merged["natives"] if n["candidate_observed_calls"]),
        **counts,
    })
    merged.setdefault("limitations", []).append(
        "Retail observations come from the canonical capture where it has any, and from the spawn-mode startup "
        "capture only for natives the canonical run never reached; the candidate side is always the newest image.")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(merged, indent=1) + "\n")
    print(json.dumps({"startup_retail_added": len(added), "startup_retail_addresses": added,
                      **merged["summary"]}, indent=1))
    print(f"wrote {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
