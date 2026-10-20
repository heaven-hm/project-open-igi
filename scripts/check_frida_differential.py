#!/usr/bin/env python3
"""Check the headless runtime verification result before the build proceeds.

A native passes only for its recorded matching inputs, outputs and side effects.
Attachment, entry counts and matching EAX values alone cannot establish parity.
"""

import argparse
import json
from pathlib import Path
from verification_scope import address, file_digest, load_scope, validate_runtime_contract, pair_typed_sequences


def behavior_matches(record):
    """Recompute typed comparisons instead of trusting pass/status booleans."""
    comparison = record.get("comparison")
    if (not isinstance(comparison, dict) or not isinstance(comparison.get("scope"), str)
            or not comparison["scope"].strip()):
        return False
    for part in ("inputs", "outputs", "sideEffects"):
        values = comparison.get(part)
        if (not isinstance(values, dict) or "retail" not in values or "candidate" not in values
                or values["retail"] is None or values["candidate"] is None
                or values["retail"] != values["candidate"]):
            return False
        try:
            # Python equality alone conflates True/1 and 1/1.0; exact typed
            # observations must also preserve their JSON value types.
            if json.dumps(values["retail"], sort_keys=True, allow_nan=False) != json.dumps(
                    values["candidate"], sort_keys=True, allow_nan=False):
                return False
        except (ValueError, TypeError):
            return False
    return True


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--report", required=True)
    ap.add_argument("--gate-report", type=Path,
                    help="Require complete runtime coverage of this static/semantic scope")
    ap.add_argument("--candidate-exe", type=Path,
                    help="Require runtime evidence for these exact executable bytes")
    ap.add_argument("--allow-unobserved", action="store_true",
                    help="do not fail for natives the retail game did not call")
    a = ap.parse_args()
    # Missing observations are not approvals. Exploratory callers may opt in
    # to omissions; release verification leaves this false.
    allow_unobserved = a.allow_unobserved

    path = Path(a.report)
    if not path.exists():
        print(f"native_gate: BLOCKED: runtime verification report {path} is missing",
              file=__import__("sys").stderr)
        return 1
    try:
        doc = json.loads(path.read_text())
        if not isinstance(doc, dict):
            raise ValueError("runtime report must be an object")
        if not isinstance(doc.get("results"), list) or any(not isinstance(r, dict) for r in doc["results"]):
            raise ValueError("runtime results must be an array of native records")
        if a.gate_report:
            if a.allow_unobserved:
                raise ValueError("required native scope cannot allow unobserved methods")
            required, digest, gate = load_scope(a.gate_report)
            actual = [address(r["address"]) for r in doc.get("results", [])]
            if len(actual) != len(set(actual)) or set(actual) != required:
                raise ValueError("runtime results must cover every required native exactly once")
            if doc.get("staticGateSha256") != digest or doc.get("binarySha256") != gate.get("binary_sha256"):
                raise ValueError("runtime evidence is not bound to the current static/semantic report and retail image")
            contracts = {address(n["address"]): n.get("runtime_contract") for n in gate["checked_natives"]}
            for row in doc["results"]:
                recomputed = pair_typed_sequences(row.get("retailCalls", []), row.get("candidateCalls", []))
                if recomputed is None or recomputed != row.get("comparison"):
                    raise ValueError("typed comparison does not match the complete captured call sequences")
                validate_runtime_contract(row, contracts[address(row["address"])], doc.get("probeSha256"))
        if a.candidate_exe and doc.get("candidateSha256") != file_digest(a.candidate_exe):
            raise ValueError("runtime evidence targets a different candidate executable")
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"native_gate: BLOCKED: {error}", file=__import__("sys").stderr)
        return 1
    results = doc.get("results", [])
    if not isinstance(results, list) or not results:
        print("native_gate: BLOCKED: runtime report has no native results",
              file=__import__("sys").stderr)
        return 1

    observed = [r for r in results if r.get("retailCalled")]
    unobserved = [r for r in results if not r.get("retailCalled")]
    # the candidate's implementation must be present and callable
    unreachable = [r for r in observed if not r.get("candidateReachable")]
    not_wired = [r for r in observed if not r.get("candidateCalled")]
    # a native called on both sides returns a comparable value only when the
    # result is a scalar; a pointer lives in each side's own address space
    scalar_mismatch = [r for r in observed if r.get("candidateCalled")
                       and r.get("scalarReturn") and not r.get("semanticComparator")
                       and not r.get("valueMatch")]
    # Entry counts and raw registers do not prove same-input behavior. Pointer
    # arguments/results also require a semantic comparator over pointed-to state.
    incomparable = [r for r in observed if r.get("candidateCalled") and not (behavior_matches(r) and
        r.get("argumentsMatch") is True and
        ((r.get("scalarReturn") is True and r.get("valueMatch") is True
          and r.get("sideEffectsMatch") is True) or
         (r.get("semanticComparator") and r.get("semanticMatch") is True))
    )]

    print(f"runtime verification: {len(results)} natives, {len(observed)} observed in "
          f"the retail game, {len(unreachable)} without a candidate implementation, "
          f"{len(scalar_mismatch)} scalar mismatches")
    for r in unreachable[:20]:
        print(f"  NO IMPLEMENTATION {r['address']} {r['name']}")
    for r in scalar_mismatch[:20]:
        print(f"  VALUE DIFFERS {r['address']} {r['name']}: retail={r.get('retailReturn')} "
              f"candidate={r.get('candidateReturn')}")
    if not_wired:
        print(f"  {len(not_wired)} natives the retail game calls are transcribed in the "
              f"reconstruction but the host never invokes them")
    for r in incomparable[:20]:
        print(f"  INCONCLUSIVE {r['address']} {r['name']}: no verified matching-input "
              "and behavioral-output comparison is recorded")
    if unobserved:
        print(f"  {len(unobserved)} natives were not called by the retail game in "
              f"this scenario and carry no observed input")

    if unreachable:
        print("native_gate: BLOCKED: a native the retail game called has no "
              "implementation in the candidate", file=__import__("sys").stderr)
        return 1
    if scalar_mismatch:
        print("native_gate: BLOCKED: a native called on both sides returned a "
              "different value", file=__import__("sys").stderr)
        return 1
    if not_wired:
        print("native_gate: BLOCKED: retail-used natives were not reached in OpenIGI",
              file=__import__("sys").stderr)
        return 1
    if incomparable:
        print("native_gate: BLOCKED: Frida calls were observed without a verified "
              "matching-input behavioral comparison", file=__import__("sys").stderr)
        return 1
    if unobserved and not allow_unobserved:
        print("native_gate: BLOCKED: a native was not observed in the retail game",
              file=__import__("sys").stderr)
        return 1
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
