#!/usr/bin/env python3
"""Measure how far each MVP native actually is from end-to-end verification.

The binding requirement is that a native may only be called verified when it
passes the CFG threshold, the angr structural analysis, and a Frida paired
probe that observes the same behaviour in retail and in the candidate. That
last pillar needs the native to exist in the candidate binary, so a native
whose symbol is absent cannot be verified at all, no matter what the static
evidence says.

This script reports, per MVP target, the honest state of each pillar:
  * implementation file present in this worktree
  * candidate symbol present in the freshly built candidate executable
  * semantic review / build / behavioural / Frida evidence recorded
  * retail reachability observed in a real level run
It writes a machine-readable report and prints a summary. It does not change
any verification status.
"""

from __future__ import annotations

import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
TARGETS = ROOT / "scripts" / "mvp_native_targets.json"
EVIDENCE = ROOT / "docs" / "fidelity" / "evidence"
DEFAULT_CANDIDATES = (
    ROOT / "build-win32-no-verify-gate" / "igi1.exe",
    Path("/Users/haseeb-mir/Downloads/IGI/igi1.exe"),
)
RETAIL_TRACE = ROOT / "build" / "runtime-traces" / "ds-level1-retail.json"
PAIR_CAPTURE = ROOT / "docs" / "fidelity" / "evidence" / "mvp-frida-pair-level1.json"


def candidate_symbols(executable: Path) -> set[str]:
    """Every defined text symbol, demangled, without the @-suffix."""
    result = subprocess.run(
        ["i686-w64-mingw32-nm", "-C", str(executable)],
        capture_output=True, text=True, timeout=600, check=False)
    symbols = set()
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[1] in {"T", "t"}:
            symbols.add(parts[2].split("@")[0])
    return symbols


def base_spellings(symbol: str) -> list[str]:
    """The symbol without its argument list, at every '(' boundary.

    A demangled symbol looks like `igi::retail::mem::Mem_InitHeapState()` or
    `(anonymous namespace)::Foo(int)`, so a plain endswith() comparison never
    matches. Truncating at each '(' yields the names the module is defined as.
    """
    out = [symbol]
    for index, character in enumerate(symbol):
        if character == "(":
            out.append(symbol[:index].rstrip())
    return out


def matches(name: str, symbols: set[str]) -> bool:
    """True when the native name is linked, allowing a C++ namespace prefix."""
    suffix = "::" + name
    for symbol in symbols:
        for base in base_spellings(symbol):
            if base == name or base.endswith(suffix):
                return True
    return False


def retail_reachability() -> dict[str, dict]:
    """Per-address observations from the recorded retail level run, if any."""
    if not RETAIL_TRACE.exists():
        return {}
    try:
        trace = json.loads(RETAIL_TRACE.read_text())
    except ValueError:
        return {}
    out = {}
    # A reachability run records the addresses that were entered, in first-hit
    # order, plus the per-address hit records. It does not record call bodies.
    names = {}
    for hit in (trace.get("probe") or {}).get("hits") or []:
        address = str(hit.get("address", "")).lower()
        if address:
            names[address] = hit.get("name")
    for position, address in enumerate(trace.get("observed_order") or []):
        address = str(address).lower()
        out[address] = {"first_hit_position": position, "name": names.get(address)}
    return out


def paired_runtime() -> dict[str, dict]:
    """Per-native retail/candidate call counts from the paired level-1 capture."""
    if not PAIR_CAPTURE.exists():
        return {}
    try:
        capture = json.loads(PAIR_CAPTURE.read_text())
    except ValueError:
        return {}
    out = {}
    for entry in capture.get("natives") or []:
        out[entry["address"].lower()] = {
            "retail": entry.get("retail_observed_calls", 0),
            "candidate": entry.get("candidate_observed_calls", 0),
            "result": entry.get("result"),
        }
    return out


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", type=Path, action="append",
                        help="candidate executable to inspect (default: both known builds)")
    parser.add_argument("--out", type=Path,
                        default=ROOT / "docs" / "fidelity" / "mvp-coverage.json")
    args = parser.parse_args()

    targets = json.loads(TARGETS.read_text())
    manifest = json.loads(MANIFEST.read_text())
    records = {record["address"].lower(): record for record in manifest["natives"]}
    reachable = retail_reachability()
    paired = paired_runtime()

    executables = args.candidate or [path for path in DEFAULT_CANDIDATES if path.exists()]
    symbol_sets = [(path, candidate_symbols(path)) for path in executables]

    rows = []
    for target in targets:
        address = target["address"].lower()
        name = target["name"]
        record = records.get(address) or {}
        verification = record.get("verification") or {}
        implementation = record.get("implementation") or {}
        impl_path = implementation.get("path")
        impl_exists = bool(impl_path) and (ROOT / impl_path).exists()

        linked = {}
        for path, symbols in symbol_sets:
            linked[str(path)] = matches(name, symbols)

        pillars = {
            "implementation_present": impl_exists,
            "candidate_symbol_linked": any(linked.values()),
            "semantic_review": bool(verification.get("semantic_review")),
            "build": bool(verification.get("build")),
            "behavioral": bool(verification.get("behavioral")
                               or verification.get("runtime_tests")),
            "frida_runtime": bool(verification.get("frida_runtime")),
            "retail_reachable": address in reachable,
        }
        # End-to-end verification is only possible when the function exists in
        # both images; everything else is a static claim.
        end_to_end_possible = pillars["candidate_symbol_linked"] and impl_exists
        blockers = []
        if not impl_exists:
            blockers.append("no implementation file in this worktree")
        if not pillars["candidate_symbol_linked"]:
            blockers.append("no symbol in the candidate executable")
        if not pillars["frida_runtime"]:
            blockers.append("no paired Frida runtime evidence")
        if not pillars["behavioral"]:
            blockers.append("no behavioural test evidence")
        runtime = paired.get(address) or {}
        rows.append({
            "runtime": {
                "retail_calls": runtime.get("retail", 0),
                "candidate_calls": runtime.get("candidate", 0),
                "paired_result": runtime.get("result"),
            },
            "address": target["address"],
            "name": name,
            "category": target.get("category"),
            "manifest_status": verification.get("status"),
            "pillars": pillars,
            "end_to_end_possible": end_to_end_possible,
            "frida_observable_end_to_end": pillars["frida_runtime"]
            and pillars["candidate_symbol_linked"],
            "blockers": blockers,
            "candidate_symbol_in": {k: v for k, v in linked.items()},
        })

    total = len(rows)
    summary = {
        "total": total,
        "implementation_present": sum(r["pillars"]["implementation_present"] for r in rows),
        "candidate_symbol_linked": sum(r["pillars"]["candidate_symbol_linked"] for r in rows),
        "behavioral_evidence": sum(r["pillars"]["behavioral"] for r in rows),
        "frida_evidence": sum(r["pillars"]["frida_runtime"] for r in rows),
        "retail_reachable": sum(r["pillars"]["retail_reachable"] for r in rows),
        "end_to_end_possible": sum(r["end_to_end_possible"] for r in rows),
        # Evidence presence, not a verdict: a native can have paired Frida
        # evidence while another pillar (behavioural, semantic) still blocks it.
        "paired_frida_evidence": sum(r["frida_observable_end_to_end"] for r in rows),
        "marked_verified_in_manifest": sum(
            1 for r in rows if r["manifest_status"] == "VERIFIED"),
        "runtime_observed_retail": sum(
            1 for r in rows if r["runtime"]["retail_calls"]),
        "runtime_observed_candidate": sum(
            1 for r in rows if r["runtime"]["candidate_calls"]),
        "runtime_observed_both": sum(
            1 for r in rows if r["runtime"]["retail_calls"]
            and r["runtime"]["candidate_calls"]),
        "runtime_return_class_matched": sum(
            1 for r in rows if r["runtime"]["paired_result"] == "match"),
    }
    report = {
        "schema_version": 1,
        "kind": "mvp-coverage-report",
        "requirement": "A native is only verified end to end when the CFG threshold, "
                       "angr structural analysis and a Frida paired probe all pass; the "
                       "probe needs the function to exist in both images.",
        "candidates": [str(path) for path, _ in symbol_sets],
        "retail_trace": str(RETAIL_TRACE.relative_to(ROOT)) if RETAIL_TRACE.exists() else None,
        "summary": summary,
        "natives": rows,
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")

    print(f"MVP natives: {total}")
    for key, value in summary.items():
        if key != "total":
            print(f"  {key:30} {value}")
    print(f"\nwrote {args.out.relative_to(ROOT)}")
    print("\nNatives that can be Frida-verified today (linked + evidence missing):")
    for row in rows:
        if row["end_to_end_possible"] and not row["frida_observable_end_to_end"]:
            print(f"  {row['address']}  {row['name']}")
    print("\nNatives with no implementation in the candidate:")
    missing = [row for row in rows if not row["pillars"]["candidate_symbol_linked"]]
    print(f"  {len(missing)} of {total}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
