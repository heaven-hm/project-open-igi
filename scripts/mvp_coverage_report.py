#!/usr/bin/env python3
"""Emit a diagnostic, hash-bound inventory for the configured MVP targets.

This report is not a verification gate. It does not infer end-to-end parity,
modify native statuses, or authorize a build. Candidate symbol matching is
name-based only and cannot prove that a retail address is implemented at the
same address or that a runtime probe exercised the function.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any, Callable, Sequence

ROOT = Path(__file__).resolve().parent.parent
MANIFEST_REL = Path("docs/fidelity/native-verification.json")
TARGETS_REL = Path("scripts/mvp_native_targets.json")
EVIDENCE_REL = Path("docs/fidelity/evidence")
ADDRESS_RE = re.compile(r"^(?:0x)?([0-9a-fA-F]{1,8})$")
SHA256_RE = re.compile(r"^[0-9a-fA-F]{64}$")
TEXT_SYMBOL_TYPES = {"T", "t", "W", "w"}
SUPPORTED_EVIDENCE_SCHEMA_VERSION = 1
SUPPORTED_MANIFEST_SCHEMA_VERSION = 1

# This is a reviewed port naming difference, not a general fuzzy-match rule.
SYMBOL_ALIASES = {"Retail_Main": ("Main",)}
DIAGNOSTIC_REPORT_KIND = "diagnostic-mvp-native-coverage"
EVIDENCE_TYPES = {
    "semantic_review": "semantic-review-report",
    "build": "build-report",
    "behavioral": "behavioral-test-report",
    "runtime_tests": "behavioral-test-report",
}


class ReportError(ValueError):
    """Input cannot support a trustworthy diagnostic report."""


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def normalize_address(value: Any) -> str:
    if not isinstance(value, str):
        raise ReportError(f"address must be a string, got {type(value).__name__}")
    match = ADDRESS_RE.fullmatch(value.strip())
    if not match:
        raise ReportError(f"invalid 32-bit hex address: {value!r}")
    number = int(match.group(1), 16)
    return f"0x{number:08X}"


def load_json(path: Path, description: str) -> tuple[Any, bytes]:
    try:
        raw = path.read_bytes()
    except OSError as error:
        raise ReportError(f"cannot read {description} {path}: {error}") from error
    try:
        return json.loads(raw), raw
    except (UnicodeDecodeError, json.JSONDecodeError) as error:
        raise ReportError(f"invalid JSON in {description} {path}: {error}") from error


def _inside(root: Path, path: Path) -> bool:
    try:
        path.relative_to(root)
        return True
    except ValueError:
        return False


def parse_nm_symbols(output: str) -> list[str]:
    """Parse defined text symbols from GNU nm -C output, retaining full names."""
    symbols: set[str] = set()
    for line in output.splitlines():
        parts = line.split(None, 2)
        if len(parts) == 3 and parts[1] in TEXT_SYMBOL_TYPES:
            symbols.add(parts[2].strip())
    if not symbols:
        raise ReportError("nm produced no defined text symbols for the candidate")
    return sorted(symbols)


def candidate_symbols(
    executable: Path,
    nm_executable: str = "i686-w64-mingw32-nm",
    run: Callable[..., Any] = subprocess.run,
) -> list[str]:
    executable = executable.resolve()
    if not executable.is_file():
        raise ReportError(f"candidate executable is missing or not a file: {executable}")
    try:
        result = run(
            [nm_executable, "-C", str(executable)],
            capture_output=True,
            text=True,
            timeout=120,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        raise ReportError(f"could not inspect candidate symbols with {nm_executable}: {error}") from error
    if result.returncode != 0:
        detail = (result.stderr or result.stdout or "no diagnostic").strip()
        raise ReportError(f"nm failed with exit code {result.returncode}: {detail}")
    return parse_nm_symbols(result.stdout)


def _symbol_bases(symbol: str) -> set[str]:
    """Return a demangled name and its prefixes before each argument list."""
    bases = {symbol.strip()}
    for index, character in enumerate(symbol):
        if character == "(":
            bases.add(symbol[:index].rstrip())
    return bases


def matching_symbols(name: str, symbols: Sequence[str]) -> list[str]:
    accepted_names = {name, *SYMBOL_ALIASES.get(name, ())}
    matches: set[str] = set()
    for symbol in symbols:
        for base in _symbol_bases(symbol):
            if any(base == item or base.endswith("::" + item) for item in accepted_names):
                matches.add(symbol)
                break
    return sorted(matches)


def _safe_evidence_path(root: Path, value: Any) -> Path:
    if not isinstance(value, str) or not value.strip():
        raise ReportError("evidence_path is missing")
    relative = Path(value)
    if relative.is_absolute() or not _inside(EVIDENCE_REL, relative):
        raise ReportError("evidence_path must be under docs/fidelity/evidence/")
    path = (root / relative).resolve()
    evidence_root = (root / EVIDENCE_REL).resolve()
    if not _inside(evidence_root, path):
        raise ReportError("evidence_path resolves outside docs/fidelity/evidence/")
    return path


def _artifact_pass_state(field: str, ref: dict[str, Any], artifact: dict[str, Any]) -> tuple[str, str | None]:
    """Validate only report formats explicitly understood; unknown stays unknown."""
    expected_kind = EVIDENCE_TYPES.get(field)
    if expected_kind is None:
        return "unknown", "unrecognized evidence field; integrity checked only"
    schema_version = artifact.get("schema_version")
    if type(schema_version) is not int or schema_version != SUPPORTED_EVIDENCE_SCHEMA_VERSION:
        return "invalid", f"unsupported evidence schema_version {schema_version!r}; expected {SUPPORTED_EVIDENCE_SCHEMA_VERSION}"
    if artifact.get("kind") != expected_kind:
        return "invalid", f"expected kind {expected_kind!r}, got {artifact.get('kind')!r}"
    if artifact.get("result") != "passed":
        return "failed", "evidence result is not 'passed'"
    if field == "semantic_review" and artifact.get("reviewed") is not True:
        return "failed", "semantic report does not record reviewed=true"
    if field == "build" and artifact.get("executed") is not True:
        return "failed", "build report does not record executed=true"
    if field in {"behavioral", "runtime_tests"}:
        total = artifact.get("tests_total")
        passed = artifact.get("tests_passed")
        failed = artifact.get("tests_failed")
        if (artifact.get("executed") is not True or type(total) is not int
                or type(passed) is not int or type(failed) is not int
                or total <= 0 or passed != total or failed != 0):
            return "failed", "behavioral result/case counts are incomplete or inconsistent"
        if ref.get("status") not in (None, "PASS", "passed"):
            return "failed", f"manifest evidence status is {ref.get('status')!r}"
    return "pass", None


def validate_evidence_ref(
    root: Path,
    field: str,
    ref: Any,
    *,
    address: str,
    binary_sha256: Any,
    implementation_sha256: Any,
    implementation_is_current: bool | None,
) -> dict[str, Any]:
    if not isinstance(ref, dict):
        return {"state": "unknown", "reason": "no structured evidence reference"}
    try:
        evidence_path = _safe_evidence_path(root, ref.get("evidence_path"))
    except ReportError as error:
        return {"state": "invalid", "reason": str(error)}
    expected_hash = ref.get("evidence_sha256")
    if not isinstance(expected_hash, str) or not SHA256_RE.fullmatch(expected_hash):
        return {"state": "invalid", "reason": "evidence_sha256 is missing or malformed"}
    try:
        raw = evidence_path.read_bytes()
    except OSError as error:
        return {"state": "invalid", "reason": f"evidence file cannot be read: {error}"}
    actual_hash = sha256_bytes(raw)
    if actual_hash.lower() != expected_hash.lower():
        return {"state": "stale", "reason": "evidence file SHA-256 differs from manifest reference", "actual_sha256": actual_hash}
    try:
        artifact = json.loads(raw)
    except (UnicodeDecodeError, json.JSONDecodeError) as error:
        return {"state": "invalid", "reason": f"evidence is not valid JSON: {error}", "actual_sha256": actual_hash}
    if not isinstance(artifact, dict):
        return {"state": "invalid", "reason": "evidence JSON root is not an object", "actual_sha256": actual_hash}

    artifact_address = artifact.get("address")
    if artifact_address is not None:
        try:
            if normalize_address(artifact_address) != address:
                return {"state": "stale", "reason": "evidence address does not match target", "actual_sha256": actual_hash}
        except ReportError:
            return {"state": "invalid", "reason": "evidence has malformed address", "actual_sha256": actual_hash}
    elif field in EVIDENCE_TYPES:
        return {"state": "unknown", "reason": "evidence has no address binding", "actual_sha256": actual_hash}

    artifact_binary = artifact.get("binary_sha256")
    if binary_sha256 and artifact_binary:
        if str(artifact_binary).lower() != str(binary_sha256).lower():
            return {"state": "stale", "reason": "evidence retail binary hash differs from manifest", "actual_sha256": actual_hash}
    else:
        return {"state": "unknown", "reason": "retail binary binding is missing", "actual_sha256": actual_hash}

    artifact_implementation = artifact.get("implementation_sha256")
    if implementation_sha256 and artifact_implementation:
        if str(artifact_implementation).lower() != str(implementation_sha256).lower():
            return {"state": "stale", "reason": "evidence implementation hash differs from manifest", "actual_sha256": actual_hash}
    else:
        return {"state": "unknown", "reason": "implementation hash binding is missing", "actual_sha256": actual_hash}
    if implementation_is_current is not True:
        return {"state": "stale", "reason": "manifest implementation hash does not match source file", "actual_sha256": actual_hash}

    state, reason = _artifact_pass_state(field, ref, artifact)
    result = {"state": state, "actual_sha256": actual_hash, "kind": artifact.get("kind"), "result": artifact.get("result")}
    if reason:
        result["reason"] = reason
    return result


def _load_inventory(root: Path) -> tuple[list[dict[str, Any]], dict[str, dict[str, Any]], str, str, str | None]:
    targets_value, target_bytes = load_json(root / TARGETS_REL, "MVP target list")
    manifest, manifest_bytes = load_json(root / MANIFEST_REL, "native manifest")
    if not isinstance(targets_value, list) or not isinstance(manifest, dict) or not isinstance(manifest.get("natives"), list):
        raise ReportError("target list or manifest has an unexpected top-level shape")
    version = manifest.get("schema_version")
    if type(version) is not int or version != SUPPORTED_MANIFEST_SCHEMA_VERSION:
        raise ReportError(f"unsupported manifest schema_version {version!r}; expected {SUPPORTED_MANIFEST_SCHEMA_VERSION}")

    targets: list[dict[str, Any]] = []
    target_addresses: set[str] = set()
    for index, target in enumerate(targets_value):
        if not isinstance(target, dict) or not isinstance(target.get("name"), str):
            raise ReportError(f"target[{index}] must contain string name and address")
        address = normalize_address(target.get("address"))
        if address in target_addresses:
            raise ReportError(f"duplicate address in target list: {address}")
        target_addresses.add(address)
        targets.append({**target, "address": address})

    records: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(manifest["natives"]):
        if not isinstance(record, dict):
            raise ReportError(f"manifest native[{index}] is not an object")
        address = normalize_address(record.get("address"))
        if address in records:
            raise ReportError(f"duplicate address in manifest: {address}")
        records[address] = record

    missing = sorted(target_addresses - records.keys())
    if missing:
        raise ReportError("missing manifest addresses: " + ", ".join(missing))
    binary_sha256 = manifest.get("binary_sha256")
    if not isinstance(binary_sha256, str) or not SHA256_RE.fullmatch(binary_sha256):
        binary_sha256 = None
    return (
        sorted(targets, key=lambda row: int(row["address"], 16)),
        records,
        sha256_bytes(target_bytes),
        sha256_bytes(manifest_bytes),
        binary_sha256,
    )


def _source_binding(root: Path, implementation: Any) -> dict[str, Any]:
    if not isinstance(implementation, dict):
        return {"state": "unknown", "reason": "implementation record missing"}
    relative = implementation.get("path")
    claimed = implementation.get("sha256")
    if not isinstance(relative, str) or not relative.strip():
        return {"state": "unknown", "reason": "implementation path missing"}
    path = Path(relative)
    if path.is_absolute():
        return {"state": "invalid", "reason": "implementation path must be repository-relative"}
    resolved = (root / path).resolve()
    if not _inside(root, resolved):
        return {"state": "invalid", "reason": "implementation path resolves outside repository"}
    try:
        raw = resolved.read_bytes()
    except OSError as error:
        return {"state": "missing", "reason": f"implementation file cannot be read: {error}"}
    actual = sha256_bytes(raw)
    if not isinstance(claimed, str) or not SHA256_RE.fullmatch(claimed):
        return {"state": "unknown", "path": relative, "actual_sha256": actual, "reason": "manifest source hash missing or malformed"}
    state = "matches" if actual.lower() == claimed.lower() else "stale"
    return {"state": state, "path": relative, "manifest_sha256": claimed, "actual_sha256": actual}


def _trace_observations(root: Path, trace_arg: str | None, retail_hash: Any) -> dict[str, Any]:
    if not trace_arg:
        return {"state": "unknown", "reason": "no retail trace supplied", "observed_addresses": []}
    path = Path(trace_arg)
    if not path.is_absolute():
        path = root / path
    path = path.resolve()
    if not _inside(root, path):
        return {"state": "invalid", "reason": "retail trace must be inside repository", "observed_addresses": []}
    try:
        trace, raw = load_json(path, "retail reachability trace")
    except ReportError as error:
        return {"state": "invalid", "reason": str(error), "observed_addresses": []}
    if not isinstance(trace, dict) or not isinstance(trace.get("observed_order"), list):
        return {"state": "unknown", "reason": "trace lacks a recognized observed_order array", "observed_addresses": [], "sha256": sha256_bytes(raw)}
    if not retail_hash or str(trace.get("binary_sha256", "")).lower() != str(retail_hash).lower():
        return {"state": "stale", "reason": "trace is not bound to the manifest retail binary", "observed_addresses": [], "sha256": sha256_bytes(raw)}
    observed: set[str] = set()
    try:
        for value in trace["observed_order"]:
            observed.add(normalize_address(value))
    except ReportError as error:
        return {"state": "invalid", "reason": str(error), "observed_addresses": [], "sha256": sha256_bytes(raw)}
    return {
        "state": "bound",
        "path": path.relative_to(root).as_posix(),
        "sha256": sha256_bytes(raw),
        "binary_sha256": retail_hash,
        "observed_addresses": sorted(observed, key=lambda value: int(value, 16)),
    }


def build_report(
    root: Path,
    candidate: Path,
    *,
    nm_executable: str = "i686-w64-mingw32-nm",
    retail_trace: str | None = None,
    run: Callable[..., Any] = subprocess.run,
) -> dict[str, Any]:
    root = root.resolve()
    candidate = candidate.resolve()
    targets, records, target_digest, manifest_digest, retail_hash = _load_inventory(root)
    symbols = candidate_symbols(candidate, nm_executable, run)
    try:
        candidate_digest = sha256_bytes(candidate.read_bytes())
    except OSError as error:
        raise ReportError(f"cannot hash candidate {candidate}: {error}") from error

    manifest_path = root / MANIFEST_REL
    target_path = root / TARGETS_REL
    for forbidden in (manifest_path.resolve(), target_path.resolve(), (root / EVIDENCE_REL).resolve()):
        if candidate == forbidden or _inside(forbidden, candidate):
            raise ReportError("candidate cannot be a manifest, target list, or evidence file")

    trace = _trace_observations(root, retail_trace, retail_hash)
    trace_addresses = set(trace.get("observed_addresses", [])) if trace.get("state") == "bound" else set()

    rows = []
    for target in targets:
        address = target["address"]
        record = records[address]
        verification = record.get("verification") if isinstance(record.get("verification"), dict) else {}
        implementation = record.get("implementation")
        source = _source_binding(root, implementation)
        catalog_name = record.get("name")
        names_to_check = {target["name"]}
        if isinstance(catalog_name, str) and catalog_name:
            names_to_check.add(catalog_name)
        matches_by_name = {name: matching_symbols(name, symbols) for name in sorted(names_to_check)}
        matches = sorted({symbol for found in matches_by_name.values() for symbol in found})
        symbol_state = "not_found" if not matches else ("unique_name_match" if len(matches) == 1 else "ambiguous_name_match")
        record_binary = record.get("binary_sha256")
        binary_binding = (
            "unknown" if not retail_hash or not isinstance(record_binary, str)
            else "matches_manifest" if record_binary.lower() == retail_hash.lower()
            else "mismatch"
        )
        evidence = {
            field: validate_evidence_ref(
                root,
                field,
                verification.get(field),
                address=address,
                binary_sha256=retail_hash,
                implementation_sha256=implementation.get("sha256") if isinstance(implementation, dict) else None,
                implementation_is_current=source.get("state") == "matches",
            )
            for field in ("semantic_review", "build", "behavioral", "runtime_tests", "frida_runtime")
        }
        rows.append({
            "address": address,
            "name": target["name"],
            "catalog_name": catalog_name,
            "catalog_name_matches_target": catalog_name == target["name"],
            "category": target.get("category"),
            "catalog_status": verification.get("status"),
            "manifest_binary_binding": binary_binding,
            "implementation_source": source,
            "candidate_symbol_name_match": {
                "state": symbol_state,
                "matches": matches,
                "by_name": matches_by_name,
            },
            "retail_trace_observation": (
                "observed" if address in trace_addresses else "not_observed"
            ) if trace.get("state") == "bound" else "unknown",
            "evidence_references": evidence,
        })

    state_counts: dict[str, int] = {}
    for row in rows:
        state = row["candidate_symbol_name_match"]["state"]
        state_counts[state] = state_counts.get(state, 0) + 1
    evidence_counts: dict[str, int] = {}
    for row in rows:
        for result in row["evidence_references"].values():
            state = result["state"]
            evidence_counts[state] = evidence_counts.get(state, 0) + 1

    return {
        "schema_version": 1,
        "kind": DIAGNOSTIC_REPORT_KIND,
        "diagnostic_only": True,
        "acceptance_claim": False,
        "status_mutation": False,
        "limitations": [
            "Name-based candidate symbol matches do not establish retail address identity or runtime behavior.",
            "Catalog status is reported as recorded, not independently accepted by this report.",
            "Unknown evidence schemas remain unknown; this report is not a substitute for the strict native gate.",
            "Candidate source/build receipt linkage is not inferred from symbol presence.",
        ],
        "inputs": {
            "manifest_sha256": manifest_digest,
            "target_list_sha256": target_digest,
            "retail_binary_sha256": retail_hash,
            "candidate_path": candidate.relative_to(root).as_posix() if _inside(root, candidate) else candidate.name,
            "candidate_sha256": candidate_digest,
            "nm_executable": nm_executable,
            "retail_trace": trace,
        },
        "summary": {
            "target_count": len(rows),
            "candidate_symbol_name_matches": state_counts.get("unique_name_match", 0),
            "ambiguous_candidate_symbol_name_matches": state_counts.get("ambiguous_name_match", 0),
            "candidate_symbol_name_not_found": state_counts.get("not_found", 0),
            "retail_trace_observed": sum(row["retail_trace_observation"] == "observed" for row in rows),
            "catalog_status_counts": _catalog_status_counts(rows),
            "evidence_reference_states": evidence_counts,
        },
        "natives": rows,
    }


def _catalog_status_counts(rows: Sequence[dict[str, Any]]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for row in rows:
        status = row.get("catalog_status")
        label = status if isinstance(status, str) and status else "unknown"
        counts[label] = counts.get(label, 0) + 1
    return dict(sorted(counts.items()))


def _git_metadata_paths(root: Path) -> set[Path]:
    marker = root / ".git"
    protected = {marker.resolve()}
    if marker.is_dir():
        return protected
    if not marker.is_file():
        return protected
    try:
        line = marker.read_text(encoding="utf-8").strip()
    except OSError as error:
        raise ReportError(f"cannot read worktree metadata pointer: {error}") from error
    if not line.startswith("gitdir:"):
        raise ReportError("unrecognized .git worktree metadata pointer")
    git_dir = Path(line.partition(":")[2].strip())
    if not git_dir.is_absolute():
        git_dir = root / git_dir
    git_dir = git_dir.resolve()
    protected.add(git_dir)
    common_file = git_dir / "commondir"
    if common_file.is_file():
        try:
            common_dir = Path(common_file.read_text(encoding="utf-8").strip())
        except OSError as error:
            raise ReportError(f"cannot read shared Git metadata pointer: {error}") from error
        if not common_dir.is_absolute():
            common_dir = git_dir / common_dir
        protected.add(common_dir.resolve())
    return protected


def _safe_output_path(root: Path, value: str, candidate: Path) -> Path:
    root = root.resolve()
    output = Path(value)
    if not output.is_absolute():
        output = root / output
    output = output.resolve()
    if any(output == path or _inside(path, output) for path in _git_metadata_paths(root)):
        raise ReportError("output path cannot be inside Git or worktree metadata")
    protected = {
        (root / MANIFEST_REL).resolve(),
        (root / TARGETS_REL).resolve(),
        (root / EVIDENCE_REL).resolve(),
        Path(__file__).resolve(),
        Path(__file__).with_name("test_mvp_coverage_report.py").resolve(),
        candidate.resolve(),
    }
    manifest, _ = load_json(root / MANIFEST_REL, "native manifest")
    for record in manifest.get("natives", []) if isinstance(manifest, dict) else []:
        implementation = record.get("implementation") if isinstance(record, dict) else None
        source = implementation.get("path") if isinstance(implementation, dict) else None
        if isinstance(source, str) and source:
            path = Path(source)
            if not path.is_absolute():
                protected.add((root / path).resolve())
    if any(output == path or _inside(path, output) for path in protected):
        raise ReportError("output path cannot overwrite candidate, source, tests, manifest, targets, or evidence")

    if _inside(root, output) and (root / ".git").exists():
        relative = output.relative_to(root).as_posix()
        try:
            tracked = subprocess.run(
                ["git", "-C", str(root), "ls-files", "--error-unmatch", "--", relative],
                capture_output=True, text=True, timeout=10, check=False,
            )
        except (OSError, subprocess.TimeoutExpired) as error:
            raise ReportError(f"could not verify whether output path is tracked: {error}") from error
        if tracked.returncode == 0:
            raise ReportError("output path is tracked; refusing to overwrite an input")
        if tracked.returncode != 1:
            raise ReportError("could not verify whether output path is tracked")

    if output.exists():
        try:
            existing, _ = load_json(output, "existing output")
        except ReportError as error:
            raise ReportError("output exists and is not a previous diagnostic report") from error
        if not (isinstance(existing, dict)
                and existing.get("kind") == DIAGNOSTIC_REPORT_KIND
                and existing.get("diagnostic_only") is True):
            raise ReportError("output exists and is not a previous diagnostic report")
    return output


def main(argv: Sequence[str] | None = None, *, root: Path = ROOT) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", action="append", required=True,
                        help="one explicit candidate executable; may be supplied exactly once")
    parser.add_argument("--nm", default="i686-w64-mingw32-nm", help="GNU nm executable")
    parser.add_argument("--retail-trace", help="optional repository-relative hash-bound trace with observed_order")
    parser.add_argument("--out", default="build/diagnostic/mvp-coverage.json", help="diagnostic JSON output path")
    args = parser.parse_args(argv)
    if len(args.candidate) != 1:
        parser.error("provide exactly one --candidate")
    root = root.resolve()
    try:
        candidate = Path(args.candidate[0]) if Path(args.candidate[0]).is_absolute() else root / args.candidate[0]
        report = build_report(
            root,
            candidate,
            nm_executable=args.nm,
            retail_trace=args.retail_trace,
        )
        output = _safe_output_path(root, args.out, candidate)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    except ReportError as error:
        print(f"mvp_coverage_report: error: {error}", file=sys.stderr)
        return 2
    except OSError as error:
        print(f"mvp_coverage_report: error: {error}", file=sys.stderr)
        return 2

    summary = report["summary"]
    print(f"MVP targets: {summary['target_count']}")
    print(f"Unique symbol-name matches: {summary['candidate_symbol_name_matches']}")
    print(f"Ambiguous symbol-name matches: {summary['ambiguous_candidate_symbol_name_matches']}")
    print(f"Evidence reference states: {json.dumps(summary['evidence_reference_states'], sort_keys=True)}")
    print(f"wrote diagnostic report {output}")
    print("Diagnostic only: no end-to-end verification or acceptance claim is made.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
