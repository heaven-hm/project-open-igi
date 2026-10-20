"""Shared scope binding for static/semantic and paired runtime verification."""
import hashlib
import json
from pathlib import Path
import contextlib
import shutil
import tempfile
import re


def file_digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def address(value):
    number = int(value, 16)
    if not 0 < number <= 0xFFFFFFFF:
        raise ValueError("native address must be a nonzero x86 VA")
    return f"0x{number:08X}"


def load_scope(path):
    report = json.loads(Path(path).read_text())
    if (report.get("result") != "passed" or report.get("failures") != []
            or report.get("threshold") != "VERIFIED"
            or not report.get("semantic_review_required")
            or not report.get("behavioral_tests_required")
            or not 0.85 <= float(report.get("cfg_threshold", 0)) <= 1):
        raise ValueError("static, semantic and behavioral native gate must pass first")
    natives = report.get("checked_natives", [])
    if not natives or len(natives) != report.get("checked_native_count"):
        raise ValueError("no complete changed-native scope; nothing is authorized as verified")
    if any(n.get("errors") for n in natives):
        raise ValueError("native scope contains failed evidence checks")
    contracts = {}
    for native in natives:
        va, contract = address(native["address"]), native.get("runtime_contract")
        if va in contracts and contract != contracts[va]:
            raise ValueError(f"conflicting reviewed runtime contracts for {va}")
        contracts[va] = contract
    return {address(n["address"]) for n in natives}, file_digest(path), report


def pair_typed_sequences(retail_calls, candidate_calls):
    if (not isinstance(retail_calls, list) or not isinstance(candidate_calls, list)
            or not retail_calls or not candidate_calls
            or any(not isinstance(c, dict) for c in retail_calls + candidate_calls)):
        return None
    states = [c.get("typedState") for c in retail_calls + candidate_calls]
    if any(not isinstance(state, dict) for state in states):
        return None
    scope = states[0].get("scope")
    if not isinstance(scope, str) or not scope.strip() or any(s.get("scope") != scope for s in states):
        return None
    result = {"scope": scope}
    for part in ("inputs", "outputs", "sideEffects"):
        if any(part not in state or state[part] is None for state in states):
            return None
        result[part] = {
            "retail": [c["typedState"][part] for c in retail_calls],
            "candidate": [c["typedState"][part] for c in candidate_calls],
        }
    return result


@contextlib.contextmanager
def stage_candidate(candidate, game_directory):
    """Use a distinct test filename beside the retail assets; never deploy over the game."""
    with tempfile.NamedTemporaryFile(prefix="open-igi-probe-", suffix=".exe",
                                     dir=game_directory, delete=False) as temporary:
        destination = Path(temporary.name)
    try:
        shutil.copyfile(candidate, destination)
        if file_digest(candidate) != file_digest(destination):
            raise ValueError("staged candidate executable differs from compiled bytes")
        yield destination
    finally:
        destination.unlink(missing_ok=True)


def validate_runtime_contract(record, contract, probe_digest):
    """Require observables explicitly authorized by the source-semantic review."""
    comparison = record.get("comparison", {})
    if (not isinstance(probe_digest, str) or re.fullmatch(r"[0-9a-f]{64}", probe_digest) is None
            or not isinstance(contract, dict) or contract.get("scope") != comparison.get("scope")
            or contract.get("probe_sha256") != probe_digest
            or record.get("semanticComparator") != probe_digest):
        raise ValueError("runtime comparison lacks a matching reviewed scope/probe contract")
    requirements = contract.get("required_observables")
    if not isinstance(requirements, dict):
        raise ValueError("runtime contract does not declare required observables")
    counts = []
    for part, empty_flag in (("inputs", "no_arguments"), ("outputs", "no_return"),
                             ("sideEffects", "no_side_effects")):
        fields = requirements.get(part)
        if (not isinstance(fields, list) or any(not isinstance(k, str) or not k for k in fields)
                or (not fields and contract.get(empty_flag) is not True)):
            raise ValueError(f"runtime contract must review required {part} or its absence")
        for side in ("retail", "candidate"):
            values = comparison.get(part, {}).get(side)
            if not isinstance(values, list) or not values:
                raise ValueError("runtime comparison must contain the complete nonempty call sequence")
            counts.append(len(values))
            for observation in values:
                if (not isinstance(observation, dict)
                        or any(k not in observation or observation[k] is None for k in fields)):
                    raise ValueError(f"runtime comparison omits reviewed {part} observables")
    if len(set(counts)) != 1:
        raise ValueError("runtime call sequences differ in length")
