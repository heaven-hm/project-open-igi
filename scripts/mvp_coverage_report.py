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
# The release build inlines most natives, so the same measurement is repeated
# against the non-inlined verification build; both are recorded, neither is a
# verdict on its own.
VERIFY_CAPTURE = ROOT / "docs" / "fidelity" / "evidence" / "mvp-frida-pair-recompar.json"


def candidate_symbols(executable: Path) -> dict[str, int]:
    """Every defined text symbol that has an address, keyed by demangled name."""
    result = subprocess.run(
        ["i686-w64-mingw32-nm", "-C", str(executable)],
        capture_output=True, text=True, timeout=600, check=False)
    symbols: dict[str, int] = {}
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[1] in {"T", "t"}:
            symbols.setdefault(parts[2].split("@")[0], int(parts[0], 16))
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


def direct_call_sites(executable: Path) -> dict[int, int]:
    """Count direct call/jump sites per target address in the built executable.

    A native whose call sites were all inlined has no site left, so an entry
    hook on its address can never fire even though the code executes. This is
    the difference between "not executed" and "not observable", and it matters
    before any runtime claim is made.
    """
    result = subprocess.run(
        ["i686-w64-mingw32-objdump", "-d", str(executable)],
        capture_output=True, text=True, timeout=1800, check=False)
    calls: dict[int, int] = {}
    # objdump lines look like "  41f95b:\tff 14 95 ...\tcall   41f900 <sym>", so
    # the mnemonic is the third tab-separated field, not the first token.
    for line in result.stdout.splitlines():
        fields = line.split("\t")
        if len(fields) < 3:
            continue
        instruction = fields[2].strip().split()
        if not instruction or instruction[0] not in {"call", "jmp"}:
            continue
        # The mnemonic and its operand share one tab field:
        # "call   496f80 <_fflush>" or "call   *0x69e608".
        operands = instruction[1:]
        if not operands and len(fields) > 3:
            operands = fields[3].split()
        if not operands:
            continue
        target = operands[0].rstrip(",")
        try:
            value = int(target, 16)
        except ValueError:
            continue
        calls[value] = calls.get(value, 0) + 1
    return calls


def resolve(name: str, symbols: dict[str, int]) -> int | None:
    """The linked address of one native, allowing a C++ namespace prefix."""
    suffix = "::" + name
    for symbol, address in symbols.items():
        for base in base_spellings(symbol):
            if base == name or base.endswith(suffix):
                return address
    return None


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


def paired_runtime(capture_path: Path | None = None) -> dict[str, dict]:
    """Per-native retail/candidate call counts from a paired level-1 capture."""
    capture_path = capture_path or PAIR_CAPTURE
    if not capture_path.exists():
        return {}
    try:
        capture = json.loads(capture_path.read_text())
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
    verification = paired_runtime(VERIFY_CAPTURE) if VERIFY_CAPTURE.exists() else {}

    executables = args.candidate or [path for path in DEFAULT_CANDIDATES if path.exists()]
    symbol_sets = [(path, candidate_symbols(path)) for path in executables]
    call_sites = {}
    for path in executables:
        call_sites[path] = direct_call_sites(path)

    rows = []
    for target in targets:
        address = target["address"].lower()
        name = target["name"]
        record = records.get(address) or {}
        record_verification = record.get("verification") or {}
        implementation = record.get("implementation") or {}
        impl_path = implementation.get("path")
        impl_exists = bool(impl_path) and (ROOT / impl_path).exists()

        linked = {}
        sites = 0
        for path, symbols in symbol_sets:
            linked_address = resolve(name, symbols)
            linked[str(path)] = linked_address is not None
            if linked_address is not None:
                sites = max(sites, call_sites[path].get(linked_address, 0))

        pillars = {
            "implementation_present": impl_exists,
            "candidate_symbol_linked": any(linked.values()),
            "semantic_review": bool(record_verification.get("semantic_review")),
            "build": bool(record_verification.get("build")),
            "behavioral": bool(record_verification.get("behavioral")
                               or record_verification.get("runtime_tests")),
            "frida_runtime": bool(record_verification.get("frida_runtime")),
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
            "manifest_status": record_verification.get("status"),
            "pillars": pillars,
            "end_to_end_possible": end_to_end_possible,
            "frida_observable_end_to_end": pillars["frida_runtime"]
            and pillars["candidate_symbol_linked"],
            "blockers": blockers,
            "candidate_symbol_in": {k: v for k, v in linked.items()},
            "candidate_direct_call_sites": sites,
            "hook_observable": bool(sites),
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
        "hook_observable": sum(1 for r in rows if r["hook_observable"]),
        "hook_observable_and_retail_reached": sum(
            1 for r in rows if r["hook_observable"]
            and r["runtime"]["retail_calls"]),
        "runtime_observed_both": sum(
            1 for r in rows if r["runtime"]["retail_calls"]
            and r["runtime"]["candidate_calls"]),
        "runtime_return_class_matched": sum(
            1 for r in rows if r["runtime"]["paired_result"] == "match"),
        "verification_build_observed_candidate": sum(
            1 for r in rows if (verification.get(r["address"].lower()) or {}).get("candidate")),
        "verification_build_matched": sum(
            1 for r in rows
            if (verification.get(r["address"].lower()) or {}).get("result") == "match"),
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
    print("\nHook-observable natives still missing paired Frida evidence:")
    for row in rows:
        if row["hook_observable"] and not row["frida_observable_end_to_end"]:
            print(f"  {row['address']}  {row['name']}")
    print("\nInlined natives that need a non-inlined verification build:")
    inlined = [row for row in rows
               if row["pillars"]["candidate_symbol_linked"] and not row["hook_observable"]]
    print(f"  {len(inlined)} of {len(rows)}")
    print("\nNatives with no implementation in the candidate:")
    missing = [row for row in rows if not row["pillars"]["candidate_symbol_linked"]]
    print(f"  {len(missing)} of {total}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
