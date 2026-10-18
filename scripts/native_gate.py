#!/usr/bin/env python3
"""Fail closed when changed IGI native code lacks binary-bound evidence.

Evidence contract
-----------------
Binary Ninja + Radare2 are the required primary analysis tools. Ghidra is an
optional backup/corroboration tool and cannot replace either primary artifact.
Each primary disassembly artifact is normalized JSON::

    {
      "schema_version": 1,
      "kind": "native-disassembly",
      "tool": "binary_ninja",          // or "radare2"
      "binary_sha256": "<retail exe sha256>",
      "function_entry": "0x004C6670",
      "signature": "...",
      "cfg": {
        "nodes": [{"address": "0x004C6670"}, ...],
        "edges": [
          {"source": "0x004C6670", "target": "0x004C66A0", "kind": "fallthrough"}
        ]
      }
    }

CFG edges must be intraprocedural edges between basic-block start addresses.
The gate compares graph nodes/edges; matching aggregate counts alone is not
accepted as CFG similarity.

Logic correctness is checked through differential test observations. A
behavioral-test-report must include at least eight cases, test inputs, and the
actual observable outputs from both the retail reference and the reconstructed
implementation. The gate compares those outputs; it does not demand identical
source code, so different algorithms that produce the same observations pass.
The test runner must execute both implementations and include return values,
state/memory changes, and relevant side effects in its normalized observations.
No generic static checker can guarantee logic equivalence for arbitrary C++.
Strict mode launches the configured per-native runner on every gate invocation,
so it cannot pass on a stale behavioral report. The runner must dispatch the
retail native and the current compiled reconstruction for equivalent inputs.
If no runner is configured or any observable differs, the gate blocks.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import os
import pathlib
import re
import subprocess
import sys
from typing import Any


# VERIFIED_WITH_DOCUMENTED_SCOPE is a qualification of VERIFIED, not a higher
# assurance level. Its extra scope-document requirement is checked separately.
STATUS_RANK = {
    "ANALYZED": 0,
    "TRANSCRIBED": 1,
    "INCONCLUSIVE": 1,
    "STRUCTURALLY_VERIFIED": 2,
    "BUILD_VERIFIED": 3,
    "RUNTIME_TESTED": 4,
    "VERIFIED": 5,
    "VERIFIED_WITH_DOCUMENTED_SCOPE": 5,
}

NATIVE_COMMENT = re.compile(
    r"^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b",
    re.IGNORECASE,
)
HOST_HELPER_COMMENT = re.compile(r"^\s*//\s*HOST HELPER:", re.IGNORECASE)
STATUS_COMMENT = re.compile(r"^\s*//\s*Status/limits:\s*(?P<status>[A-Z_]+)\b")
GAME_NATIVE_ADDRESS = re.compile(r"(?i)^(?P<address>[0-9a-f]{8})-")
NATIVE_ROOTS = ("engine/src/retail/", "scripts/GameNatives/")
NATIVE_SOURCE_SUFFIXES = {".cpp", ".cc", ".cxx", ".c++", ".h", ".hh", ".hpp", ".hxx", ".inl"}
EVIDENCE_ROOT = "docs/fidelity/evidence/"
DEFAULT_BINARY = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
DEFAULT_MANIFEST = pathlib.Path("docs/fidelity/native-verification.json")

ACCEPTED_SIGNATURE_RESULTS = {"exact", "name-insensitive", "abi-adapted"}
PRIMARY_ANALYSIS_TOOLS = ("binary_ninja", "radare2")
BACKUP_ANALYSIS_TOOLS = ("ghidra",)
MINIMUM_BEHAVIORAL_CASES = 8
REQUIRED_BEHAVIORAL_COVERAGE = {"nominal", "boundary"}
# Prevent a report from making incorrect outputs pass with an absurd tolerance.
MAX_ABSOLUTE_FLOAT_TOLERANCE = 1e-3
MAX_RELATIVE_FLOAT_TOLERANCE = 1e-4
EDGE_KIND_ALIASES = {
    "fallthrough": "fallthrough",
    "fallthroughedge": "fallthrough",
    "ft": "fallthrough",
    "sequential": "fallthrough",
    "true": "conditional",
    "false": "conditional",
    "truebranch": "conditional",
    "falsebranch": "conditional",
    "unconditionalbranch": "unconditional",
    "indirectbranch": "unconditional",
    "functionreturn": "return",
    "return": "return",
    "call": "call",
    "calldestination": "call",
    "cond": "conditional",
    "conditional": "conditional",
    "conditionalbranch": "conditional",
    "conditionaljump": "conditional",
    "jcc": "conditional",
    "branch": "conditional",
    "jump": "unconditional",
    "unconditional": "unconditional",
    "unconditionaljump": "unconditional",
    "jmp": "unconditional",
    "goto": "unconditional",
    "switch": "switch",
    "switchcase": "switch",
    "exception": "exceptional",
    "exceptional": "exceptional",
    "other": "other",
}


class GateError(Exception):
    """Expected validation failure."""


def sha256(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def normalize_address(value: Any) -> str:
    """Normalize an x86 virtual address to 0x plus eight uppercase hex digits."""
    if isinstance(value, bool):
        raise ValueError("boolean is not an address")
    if isinstance(value, int):
        number = value
    elif isinstance(value, str):
        text = value.strip()
        if text.lower().startswith("0x"):
            number = int(text, 16)
        elif re.fullmatch(r"[0-9a-fA-F]{1,8}", text):
            # Address-like strings in this project are hexadecimal, including
            # strings without the 0x prefix.
            number = int(text, 16)
        else:
            raise ValueError(f"invalid hexadecimal address: {value!r}")
    else:
        raise ValueError(f"invalid address type: {type(value).__name__}")
    if number < 0 or number > 0xFFFFFFFF:
        raise ValueError(f"address is outside the x86 address range: {value!r}")
    return f"0x{number:08X}"


def normalized_signature(value: str) -> str:
    return re.sub(r"\s+", " ", value.strip())


def is_under(path: pathlib.Path, parent: pathlib.Path) -> bool:
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def resolve_repo_path(root: pathlib.Path, relative: str, *, must_exist: bool = True) -> pathlib.Path:
    """Resolve a manifest path while preventing absolute paths and traversal."""
    if not isinstance(relative, str) or not relative.strip():
        raise GateError("repository-relative path is missing")
    raw = pathlib.PurePosixPath(relative)
    if raw.is_absolute() or ".." in raw.parts:
        raise GateError(f"unsafe repository-relative path: {relative!r}")
    candidate = (root / pathlib.Path(relative)).resolve(strict=must_exist)
    if not is_under(candidate, root.resolve()):
        raise GateError(f"path escapes repository root: {relative!r}")
    if must_exist and not candidate.is_file():
        raise GateError(f"expected a file at {relative!r}")
    return candidate


def _is_native_source_name(entry: str) -> bool:
    suffix = pathlib.PurePosixPath(entry).suffix.lower()
    return suffix in NATIVE_SOURCE_SUFFIXES and any(entry.startswith(prefix) for prefix in NATIVE_ROOTS)


def _run_git(root: pathlib.Path, args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(args, cwd=root, capture_output=True, text=True, check=False)


def _git_paths(root: pathlib.Path, args: list[str]) -> list[str]:
    result = _run_git(root, args)
    if result.returncode != 0:
        raise GateError(f"Could not enumerate changed files: {' '.join(args)}: {result.stderr.strip()}")
    return [line for line in result.stdout.splitlines() if line]


def _has_unstaged_changes(root: pathlib.Path, relative: str) -> bool:
    """True when a path selected from the index also differs in the worktree."""
    result = _run_git(root, ["git", "diff", "--quiet", "--", relative])
    if result.returncode == 0:
        return False
    if result.returncode == 1:
        return True
    raise GateError(f"Could not check staged/worktree consistency for {relative}: {result.stderr.strip()}")


def _deleted_native_paths(root: pathlib.Path, include_unstaged: bool) -> set[str]:
    commands = [["git", "diff", "--cached", "--name-status", "--diff-filter=D"]]
    if include_unstaged:
        commands.append(["git", "diff", "--name-status", "--diff-filter=D"])
    deleted: set[str] = set()
    for command in commands:
        for line in _git_paths(root, command):
            pieces = line.split("\t", 1)
            if len(pieces) == 2 and _is_native_source_name(pieces[1]):
                deleted.add(pieces[1])
    return deleted


def changed_paths(
    root: pathlib.Path,
    mode: str,
    scan_root: pathlib.Path | None = None,
) -> tuple[list[pathlib.Path], list[str]]:
    """Return existing source files to inspect and deleted native source paths."""
    if mode == "tree":
        base = (root / scan_root).resolve() if scan_root and not scan_root.is_absolute() else (scan_root or root).resolve()
        if not base.exists():
            raise GateError(f"Native scan path does not exist: {base}")
        found: set[pathlib.Path] = set()
        for native_root in NATIVE_ROOTS:
            candidate_root = (root / native_root).resolve()
            if not is_under(candidate_root, base) and not is_under(base, candidate_root):
                continue
            scan = base if is_under(base, candidate_root) else candidate_root
            if scan.is_file() and scan.suffix.lower() in NATIVE_SOURCE_SUFFIXES:
                found.add(scan.resolve())
            elif scan.is_dir():
                found.update(
                    path.resolve() for path in scan.rglob("*")
                    if path.is_file() and path.suffix.lower() in NATIVE_SOURCE_SUFFIXES
                )
        return sorted(found), []

    if mode == "production-tree":
        native_root = root / "engine/src/retail"
        files = native_root.rglob("*") if native_root.exists() else []
        return sorted(
            path.resolve() for path in files
            if path.is_file() and path.suffix.lower() in NATIVE_SOURCE_SUFFIXES
        ), []

    commands: list[list[str]] = []
    if mode in ("staged", "changed"):
        commands.append(["git", "diff", "--cached", "--name-only", "--diff-filter=ACMRT"])
    if mode == "changed":
        commands.append(["git", "diff", "--name-only", "--diff-filter=ACMRT"])
        commands.append(["git", "ls-files", "--others", "--exclude-standard"])

    found: set[pathlib.Path] = set()
    for command in commands:
        for entry in _git_paths(root, command):
            if not _is_native_source_name(entry):
                continue
            candidate = root / pathlib.Path(entry)
            if candidate.is_file():
                found.add(candidate.resolve())

    deleted = sorted(_deleted_native_paths(root, include_unstaged=(mode == "changed")))
    return sorted(found), deleted


def implementation_natives(path: pathlib.Path, relative: str) -> list[dict[str, Any]]:
    if relative.startswith("scripts/GameNatives/") and path.suffix.lower() == ".cpp":
        match = GAME_NATIVE_ADDRESS.match(path.name)
        if not match:
            raise GateError(f"Address-qualified GameNatives filename required: {relative}")
        native_name = path.stem[match.end():].strip(" _-")
        if not native_name:
            raise GateError(f"GameNatives filename must include a name after its address: {relative}")
        return [{
            "name": native_name,
            "address": normalize_address(match.group("address")),
            "line": 1,
            "comment_lines": [],
            "is_canonical_source": True,
        }]

    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    natives: list[dict[str, Any]] = []
    for index, line in enumerate(lines):
        match = NATIVE_COMMENT.match(line)
        if match:
            natives.append({
                "name": match.group("name").strip(),
                "address": normalize_address(match.group("address")),
                "line": index + 1,
                "comment_lines": lines[index:index + 5],
                "is_canonical_source": False,
            })
    return natives


def source_addresses_from_text(text: str) -> set[str]:
    addresses: set[str] = set()
    for line in text.splitlines():
        match = NATIVE_COMMENT.match(line)
        if match:
            addresses.add(normalize_address(match.group("address")))
    return addresses


def retired_native_is_reviewed(manifest: dict[str, Any], address: str, path: str) -> bool:
    retired = manifest.get("retired_natives", [])
    if not isinstance(retired, list):
        return False
    for item in retired:
        if not isinstance(item, dict):
            continue
        try:
            same_address = normalize_address(item.get("address")) == address
        except (TypeError, ValueError):
            same_address = False
        if (same_address and item.get("path") == path and item.get("reviewed") is True
                and isinstance(item.get("rationale"), str) and item["rationale"].strip()):
            return True
    return False


def retired_file_is_reviewed(manifest: dict[str, Any], path: str) -> bool:
    retired = manifest.get("retired_files", [])
    if not isinstance(retired, list):
        return False
    return any(
        isinstance(item, dict) and item.get("path") == path and item.get("reviewed") is True
        and isinstance(item.get("rationale"), str) and item["rationale"].strip()
        for item in retired
    )


def removed_native_addresses(root: pathlib.Path, path: pathlib.Path, relative: str) -> set[str]:
    if not relative.startswith("engine/src/retail/"):
        return set()
    original = _run_git(root, ["git", "show", f"HEAD:{relative}"])
    if original.returncode != 0:
        return set()
    old = source_addresses_from_text(original.stdout)
    new = source_addresses_from_text(path.read_text(encoding="utf-8", errors="replace"))
    return old - new


def check_provenance_comment(native: dict[str, Any], relative: str) -> list[str]:
    if native.get("is_canonical_source"):
        return []
    lines = native.get("comment_lines", [])
    errors: list[str] = []
    address = native["address"]
    if len(lines) != 5:
        return [f"{relative}:{native['line']} {address}: expected five consecutive provenance lines"]
    if not all(re.match(r"^\s*//", line) for line in lines):
        return [f"{relative}:{native['line']} {address}: provenance block must contain five comment lines"]
    for label, offset in (("Artifact:", 1), ("Tools/evidence:", 2),
                          ("Behavior/CFG:", 3), ("Status/limits:", 4)):
        if label not in lines[offset]:
            errors.append(f"{relative}:{native['line']} {address}: missing {label} line")
    status = STATUS_COMMENT.match(lines[4])
    native["comment_status"] = status.group("status") if status else "MISSING_STATUS"
    return errors


def check_host_helper_comments(path: pathlib.Path, relative: str) -> tuple[bool, list[str]]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    found = False
    errors: list[str] = []
    for index, line in enumerate(lines):
        if not HOST_HELPER_COMMENT.match(line):
            continue
        found = True
        block = lines[index:index + 5]
        if len(block) != 5 or not all(re.match(r"^\s*//", item) for item in block):
            errors.append(f"{relative}:{index + 1}: host helper needs five consecutive provenance lines")
            continue
        for label, offset in (("Artifact:", 1), ("Tools/evidence:", 2),
                              ("Behavior/CFG:", 3), ("Status/limits:", 4)):
            if label not in block[offset]:
                errors.append(f"{relative}:{index + 1}: host helper comment is missing {label}")
    return found, errors


def _normal_edge_kind(value: Any) -> str:
    if not isinstance(value, str) or not value.strip():
        raise GateError("CFG edge is missing a non-empty kind")
    token = re.sub(r"[^a-z0-9]", "", value.lower())
    return EDGE_KIND_ALIASES.get(token, "other")


def parse_cfg_graph(artifact: dict[str, Any], label: str) -> dict[str, set[Any]]:
    cfg = artifact.get("cfg")
    if not isinstance(cfg, dict):
        cfg = artifact.get("control_flow_graph")
    if not isinstance(cfg, dict):
        raise GateError(f"{label} evidence has no cfg object")
    raw_nodes, raw_edges = cfg.get("nodes"), cfg.get("edges")
    if not isinstance(raw_nodes, list) or not raw_nodes:
        raise GateError(f"{label} CFG must contain a non-empty nodes array")
    if not isinstance(raw_edges, list):
        raise GateError(f"{label} CFG must contain an edges array")

    nodes: set[str] = set()
    for index, item in enumerate(raw_nodes):
        raw_address = item.get("address") if isinstance(item, dict) else item
        try:
            address = normalize_address(raw_address)
        except (ValueError, TypeError) as error:
            raise GateError(f"{label} CFG node {index} has an invalid address: {error}") from error
        if address in nodes:
            raise GateError(f"{label} CFG contains duplicate node address {address}")
        nodes.add(address)

    edges: set[tuple[str, str, str]] = set()
    for index, item in enumerate(raw_edges):
        if not isinstance(item, dict):
            raise GateError(f"{label} CFG edge {index} must be an object")
        source = item.get("source", item.get("from"))
        target = item.get("target", item.get("to"))
        try:
            source_address = normalize_address(source)
            target_address = normalize_address(target)
            kind = _normal_edge_kind(item.get("kind", item.get("type")))
        except (ValueError, TypeError, GateError) as error:
            raise GateError(f"{label} CFG edge {index} is invalid: {error}") from error
        if source_address not in nodes or target_address not in nodes:
            raise GateError(
                f"{label} CFG edge {index} endpoints must both be intraprocedural CFG node addresses"
            )
        edges.add((source_address, target_address, kind))
    return {"nodes": nodes, "edges": edges}


def cfg_score(ghidra_graph: dict[str, set[Any]], radare2_graph: dict[str, set[Any]]) -> float:
    """Compare actual CFG node/edge identity, not aggregate counters."""
    def jaccard(left: set[Any], right: set[Any]) -> float:
        union = left | right
        return 1.0 if not union else len(left & right) / len(union)

    node_score = jaccard(ghidra_graph["nodes"], radare2_graph["nodes"])
    edge_score = jaccard(ghidra_graph["edges"], radare2_graph["edges"])
    # Edges carry more control-flow information than block counts.
    return 0.35 * node_score + 0.65 * edge_score


def load_manifest(path: pathlib.Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise GateError(f"Cannot read native verification manifest {path}: {error}") from error
    if not isinstance(value, dict) or value.get("schema_version") != 1:
        raise GateError(f"{path} must be a JSON object with schema_version=1")
    records = value.get("natives")
    if not isinstance(records, list):
        raise GateError(f"{path} must have a natives array")
    seen: set[str] = set()
    for index, record in enumerate(records):
        if not isinstance(record, dict) or not isinstance(record.get("address"), str):
            raise GateError(f"{path} natives[{index}] must be an object with a string address")
        try:
            address = normalize_address(record["address"])
        except (ValueError, TypeError) as error:
            raise GateError(f"{path} natives[{index}] has an invalid x86 address: {error}") from error
        if address in seen:
            raise GateError(f"{path} contains duplicate native address {address}")
        seen.add(address)
    return value


def index_manifest(manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {normalize_address(record["address"]): record for record in manifest["natives"]}


def check_file_hash(
    root: pathlib.Path,
    relative: Any,
    expected_digest: Any,
    label: str,
    errors: list[str],
    *,
    required_prefix: str | None = None,
) -> tuple[pathlib.Path | None, str | None]:
    if not isinstance(relative, str) or not relative.strip():
        errors.append(f"{label}: evidence/source path is missing")
        return None, None
    if required_prefix and not relative.startswith(required_prefix):
        errors.append(f"{label}: path must be under {required_prefix}")
        return None, None
    try:
        path = resolve_repo_path(root, relative)
    except GateError as error:
        errors.append(f"{label}: {error}")
        return None, None
    actual = sha256(path)
    if not isinstance(expected_digest, str) or expected_digest.lower() != actual:
        errors.append(f"{label}: SHA-256 does not match {relative}")
        return path, actual
    return path, actual


def load_json_artifact(path: pathlib.Path, label: str, errors: list[str]) -> dict[str, Any] | None:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        errors.append(f"{label}: evidence artifact must be valid JSON: {error}")
        return None
    if not isinstance(value, dict):
        errors.append(f"{label}: JSON evidence artifact must contain an object")
        return None
    return value


def validate_disassembly_artifact(
    artifact: dict[str, Any] | None,
    tool_name: str,
    native_address: str,
    binary_digest: str,
    manifest_signature: str,
    label: str,
    errors: list[str],
) -> dict[str, set[Any]] | None:
    if artifact is None:
        return None
    if artifact.get("schema_version") != 1 or artifact.get("kind") != "native-disassembly":
        errors.append(f"{label}: expected schema_version=1 and kind='native-disassembly'")
    artifact_tool = str(artifact.get("tool", "")).strip().lower()
    accepted_by_tool = {
        "binary_ninja": {"binary_ninja", "binary ninja", "binaryninja", "bn"},
        "radare2": {"radare2", "radare", "r2"},
        "ghidra": {"ghidra"},
    }
    accepted_tools = accepted_by_tool.get(tool_name, {tool_name})
    if artifact_tool not in accepted_tools:
        errors.append(f"{label}: tool must identify {tool_name}; got {artifact_tool!r}")
    if not isinstance(artifact.get("binary_sha256"), str) or artifact["binary_sha256"].lower() != binary_digest:
        errors.append(f"{label}: artifact is not bound to the selected retail executable SHA-256")
    try:
        artifact_entry = normalize_address(artifact.get("function_entry"))
        if artifact_entry != native_address:
            errors.append(f"{label}: function entry {artifact_entry} does not match native {native_address}")
    except (ValueError, TypeError) as error:
        errors.append(f"{label}: invalid/missing function_entry: {error}")
        return None
    artifact_signature = artifact.get("signature")
    if not isinstance(artifact_signature, str) or not artifact_signature.strip():
        errors.append(f"{label}: artifact is missing function signature")
    elif normalized_signature(artifact_signature) != normalized_signature(manifest_signature):
        errors.append(f"{label}: artifact signature differs from the manifest evidence signature")
    try:
        graph = parse_cfg_graph(artifact, label)
    except GateError as error:
        errors.append(str(error))
        return None
    if native_address not in graph["nodes"]:
        errors.append(f"{label}: CFG nodes do not include function entry {native_address}")
        return None
    return graph


def _is_number(value: Any) -> bool:
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def _compare_json_values(
    reference: Any,
    candidate: Any,
    path: str,
    abs_tol: float,
    rel_tol: float,
) -> list[str]:
    """Compare normalized observable values, allowing only declared float noise."""
    if isinstance(reference, bool) or isinstance(candidate, bool):
        return [] if type(reference) is type(candidate) and reference == candidate else [
            f"{path}: expected {reference!r}, got {candidate!r}"
        ]
    if reference is None or candidate is None:
        return [] if reference is None and candidate is None else [
            f"{path}: expected {reference!r}, got {candidate!r}"
        ]
    if isinstance(reference, dict) or isinstance(candidate, dict):
        if not isinstance(reference, dict) or not isinstance(candidate, dict):
            return [f"{path}: observable types differ ({type(reference).__name__} vs {type(candidate).__name__})"]
        failures: list[str] = []
        ref_keys, cand_keys = set(reference), set(candidate)
        for key in sorted(ref_keys - cand_keys):
            failures.append(f"{path}.{key}: candidate omitted observable")
        for key in sorted(cand_keys - ref_keys):
            failures.append(f"{path}.{key}: candidate added an unnormalized observable")
        for key in sorted(ref_keys & cand_keys):
            failures.extend(_compare_json_values(reference[key], candidate[key], f"{path}.{key}", abs_tol, rel_tol))
        return failures
    if isinstance(reference, list) or isinstance(candidate, list):
        if not isinstance(reference, list) or not isinstance(candidate, list):
            return [f"{path}: observable types differ ({type(reference).__name__} vs {type(candidate).__name__})"]
        if len(reference) != len(candidate):
            return [f"{path}: expected {len(reference)} entries, got {len(candidate)}"]
        failures = []
        for index, (ref_item, cand_item) in enumerate(zip(reference, candidate)):
            failures.extend(_compare_json_values(ref_item, cand_item, f"{path}[{index}]", abs_tol, rel_tol))
        return failures
    if _is_number(reference) and _is_number(candidate):
        # Integer observables (IDs, addresses, counts, indexes) must match exactly.
        if isinstance(reference, int) and isinstance(candidate, int):
            return [] if reference == candidate else [f"{path}: expected {reference!r}, got {candidate!r}"]
        if math.isclose(float(reference), float(candidate), rel_tol=rel_tol, abs_tol=abs_tol):
            return []
        return [f"{path}: expected {reference!r}, got {candidate!r} (abs_tol={abs_tol}, rel_tol={rel_tol})"]
    if type(reference) is not type(candidate) or reference != candidate:
        return [f"{path}: expected {reference!r}, got {candidate!r}"]
    return []


def validate_behavioral_observations(
    root: pathlib.Path, artifact: dict[str, Any], native_address: str, errors: list[str]
) -> None:
    """Require case-by-case retail-vs-candidate observations, not asserted counts."""
    if artifact.get("executed") is not True:
        errors.append(f"{native_address} behavioral report must record executed=true")
    if artifact.get("oracle") != "retail-binary-execution":
        errors.append(f"{native_address} behavioral report must identify oracle='retail-binary-execution'")

    runner = artifact.get("runner")
    if not isinstance(runner, dict):
        errors.append(f"{native_address} behavioral report must identify the test runner")
    else:
        runner_path = runner.get("path")
        runner_digest = runner.get("sha256")
        if not isinstance(runner_path, str) or not runner_path.strip():
            errors.append(f"{native_address} behavioral report is missing runner.path")
        elif not isinstance(runner_digest, str) or not re.fullmatch(r"[0-9a-fA-F]{64}", runner_digest):
            errors.append(f"{native_address} behavioral report is missing a valid runner SHA-256")
        else:
            try:
                runner_file = resolve_repo_path(root, runner_path)
            except GateError as error:
                errors.append(f"{native_address} behavioral runner: {error}")
            else:
                if sha256(runner_file).lower() != runner_digest.lower():
                    errors.append(f"{native_address} behavioral runner hash does not match {runner_path}")
        if not isinstance(runner.get("command"), list) or not runner["command"] or not all(
            isinstance(item, str) and item.strip() for item in runner["command"]
        ):
            errors.append(f"{native_address} behavioral report must record the executed runner command as an argv array")

    tolerance = artifact.get("float_tolerance", {})
    if not isinstance(tolerance, dict):
        errors.append(f"{native_address} float_tolerance must be an object")
        tolerance = {}
    abs_tol = tolerance.get("absolute", 0.0)
    rel_tol = tolerance.get("relative", 0.0)
    for label, value, maximum in (
        ("absolute", abs_tol, MAX_ABSOLUTE_FLOAT_TOLERANCE),
        ("relative", rel_tol, MAX_RELATIVE_FLOAT_TOLERANCE),
    ):
        if not _is_number(value) or not math.isfinite(float(value)) or value < 0:
            errors.append(f"{native_address} float_tolerance.{label} must be a finite non-negative number")
        elif value > maximum:
            errors.append(
                f"{native_address} float_tolerance.{label}={value} exceeds the fail-safe maximum {maximum}"
            )
    if not _is_number(abs_tol) or not math.isfinite(float(abs_tol)) or abs_tol < 0 or abs_tol > MAX_ABSOLUTE_FLOAT_TOLERANCE:
        abs_tol = 0.0
    if not _is_number(rel_tol) or not math.isfinite(float(rel_tol)) or rel_tol < 0 or rel_tol > MAX_RELATIVE_FLOAT_TOLERANCE:
        rel_tol = 0.0

    observable_fields = artifact.get("observable_fields")
    if not isinstance(observable_fields, list) or not observable_fields or not all(
        isinstance(item, str) and item.strip() for item in observable_fields
    ):
        errors.append(f"{native_address} behavioral report needs a non-empty observable_fields array")
        observable_fields = []
    else:
        if len(set(observable_fields)) != len(observable_fields):
            errors.append(f"{native_address} behavioral report has duplicate observable_fields")
        # Return state and side effects are the generic core. Functions may add
        # function-specific fields in addition to these normalized observations.
        for required in ("return_value", "state_delta", "side_effects"):
            if required not in observable_fields:
                errors.append(f"{native_address} behavioral report must include {required!r} in observable_fields")

    cases = artifact.get("cases")
    if not isinstance(cases, list):
        errors.append(f"{native_address} behavioral report must include case-by-case observations in cases[]")
        return
    if len(cases) < MINIMUM_BEHAVIORAL_CASES:
        errors.append(
            f"{native_address} behavioral report has {len(cases)} case(s); at least {MINIMUM_BEHAVIORAL_CASES} are required"
        )
    case_ids: set[str] = set()
    input_fingerprints: set[str] = set()
    observed_categories: set[str] = set()
    passed = 0
    failed = 0
    mismatches: list[str] = []
    for index, case in enumerate(cases):
        prefix = f"{native_address} case[{index}]"
        if not isinstance(case, dict):
            errors.append(f"{prefix} must be an object")
            failed += 1
            continue
        case_id = case.get("id")
        if not isinstance(case_id, str) or not case_id.strip():
            errors.append(f"{prefix} is missing a non-empty id")
            failed += 1
            continue
        if case_id in case_ids:
            errors.append(f"{prefix} duplicates case id {case_id!r}")
            failed += 1
            continue
        case_ids.add(case_id)
        case_input = case.get("input")
        if not isinstance(case_input, dict) or not case_input:
            errors.append(f"{prefix} input must be a non-empty object containing arguments and/or initial state")
            failed += 1
            continue
        try:
            input_fingerprint = json.dumps(case_input, sort_keys=True, separators=(",", ":"), allow_nan=False)
        except (TypeError, ValueError) as error:
            errors.append(f"{prefix} input is not canonical JSON: {error}")
            failed += 1
            continue
        if input_fingerprint in input_fingerprints:
            errors.append(f"{prefix} duplicates an earlier test input; vary args, state, or deterministic seed")
            failed += 1
            continue
        input_fingerprints.add(input_fingerprint)
        categories = case.get("categories", [])
        if not isinstance(categories, list) or not all(isinstance(item, str) for item in categories):
            errors.append(f"{prefix} categories must be an array of strings")
        else:
            observed_categories.update(item.lower() for item in categories)
        reference = case.get("reference_output")
        candidate = case.get("candidate_output")
        if not isinstance(reference, dict) or not isinstance(candidate, dict):
            errors.append(f"{prefix} must have reference_output and candidate_output objects")
            failed += 1
            continue
        missing_reference = set(observable_fields) - set(reference)
        missing_candidate = set(observable_fields) - set(candidate)
        if missing_reference or missing_candidate:
            if missing_reference:
                errors.append(f"{prefix} reference_output omits observables: {sorted(missing_reference)}")
            if missing_candidate:
                errors.append(f"{prefix} candidate_output omits observables: {sorted(missing_candidate)}")
            failed += 1
            continue
        case_failures = _compare_json_values(reference, candidate, f"case[{case_id}].output", float(abs_tol), float(rel_tol))
        if case_failures:
            failed += 1
            mismatches.extend(case_failures[:10])
        else:
            passed += 1

    missing_categories = REQUIRED_BEHAVIORAL_COVERAGE - observed_categories
    if missing_categories:
        errors.append(f"{native_address} behavioral report is missing coverage categories: {sorted(missing_categories)}")
    if passed + failed != len(cases):
        errors.append(f"{native_address} behavioral report has inconsistent evaluated case counts")
    if failed > 0 or mismatches:
        errors.append(
            f"{native_address} behavioral comparison failed: {failed} failing case(s) out of {len(cases)}"
        )
        errors.extend(f"{native_address} mismatch: {mismatch}" for mismatch in mismatches[:20])
    reported_total, reported_passed, reported_failed = (
        artifact.get("tests_total"), artifact.get("tests_passed"), artifact.get("tests_failed")
    )
    expected_counts = (len(cases), passed, failed)
    reported_counts = (reported_total, reported_passed, reported_failed)
    if reported_counts != expected_counts:
        errors.append(
            f"{native_address} reported test counts {reported_counts!r} do not match recomputed counts {expected_counts!r}"
        )
    if artifact.get("result") != ("passed" if failed == 0 and not mismatches and len(cases) >= MINIMUM_BEHAVIORAL_CASES and not missing_categories else "failed"):
        errors.append(f"{native_address} behavioral report result does not match the recomputed comparison result")


def validate_report_artifact(
    root: pathlib.Path,
    report_ref: Any,
    *,
    report_name: str,
    expected_kind: str,
    native_address: str,
    binary_digest: str,
    implementation_digest: str,
    errors: list[str],
) -> dict[str, Any] | None:
    if not isinstance(report_ref, dict):
        errors.append(f"{native_address}: missing {report_name} report reference")
        return None
    path, _ = check_file_hash(
        root,
        report_ref.get("evidence_path"),
        report_ref.get("evidence_sha256"),
        f"{native_address} {report_name}",
        errors,
        required_prefix=EVIDENCE_ROOT,
    )
    if path is None:
        return None
    artifact = load_json_artifact(path, f"{native_address} {report_name}", errors)
    if artifact is None:
        return None
    if artifact.get("schema_version") != 1 or artifact.get("kind") != expected_kind:
        errors.append(f"{native_address} {report_name}: expected schema_version=1 and kind={expected_kind!r}")
    if artifact.get("result") != "passed":
        errors.append(f"{native_address} {report_name}: result must be 'passed'")
    if not isinstance(artifact.get("binary_sha256"), str) or artifact["binary_sha256"].lower() != binary_digest:
        errors.append(f"{native_address} {report_name}: report is not bound to the selected retail binary")
    if not isinstance(artifact.get("implementation_sha256"), str) or artifact["implementation_sha256"].lower() != implementation_digest:
        errors.append(f"{native_address} {report_name}: report is not bound to the current implementation source")
    try:
        report_address = normalize_address(artifact.get("address"))
        if report_address != native_address:
            errors.append(f"{native_address} {report_name}: report address does not match the native")
    except (ValueError, TypeError) as error:
        errors.append(f"{native_address} {report_name}: invalid/missing address: {error}")

    if expected_kind == "build-report":
        if artifact.get("executed") is not True or not isinstance(artifact.get("command"), str) or not artifact["command"].strip():
            errors.append(f"{native_address} build report must record executed=true and the build command")
    elif expected_kind == "behavioral-test-report":
        validate_behavioral_observations(root, artifact, native_address, errors)
    elif expected_kind == "semantic-review-report":
        if artifact.get("reviewed") is not True:
            errors.append(f"{native_address} semantic review must record reviewed=true")
        reviewer = artifact.get("reviewed_by")
        if not isinstance(reviewer, str) or not reviewer.strip():
            errors.append(f"{native_address} semantic review must identify the reviewer")
        required_checks = {
            "function_identity", "argument_mapping", "constants_and_scaling",
            "memory_layout_and_access", "calls_and_side_effects", "return_and_error_paths",
            "floating_point_semantics",
        }
        raw_checks = artifact.get("checks")
        if not isinstance(raw_checks, list):
            errors.append(f"{native_address} semantic review must contain a checks array")
        else:
            checks: dict[str, dict[str, Any]] = {}
            for item in raw_checks:
                if isinstance(item, dict) and isinstance(item.get("name"), str):
                    checks[item["name"]] = item
            for check_name in sorted(required_checks):
                item = checks.get(check_name)
                if not item or item.get("result") not in {"passed", "not_applicable"}:
                    errors.append(f"{native_address} semantic review is missing a passed/not_applicable {check_name} check")
                elif item.get("result") == "not_applicable" and not (
                    isinstance(item.get("rationale"), str) and item["rationale"].strip()
                ):
                    errors.append(f"{native_address} semantic review: {check_name}=not_applicable needs a rationale")
    elif expected_kind == "mutation-test-report":
        if artifact.get("executed") is not True:
            errors.append(f"{native_address} mutation report must record executed=true")
        runner = artifact.get("runner")
        if not isinstance(runner, dict):
            errors.append(f"{native_address} mutation report must identify its mutation-test runner")
        else:
            runner_path = runner.get("path")
            runner_digest = runner.get("sha256")
            if not isinstance(runner_path, str) or not runner_path.strip():
                errors.append(f"{native_address} mutation report is missing runner.path")
            elif not isinstance(runner_digest, str) or not re.fullmatch(r"[0-9a-fA-F]{64}", runner_digest):
                errors.append(f"{native_address} mutation report is missing a valid runner SHA-256")
            else:
                try:
                    runner_file = resolve_repo_path(root, runner_path)
                except GateError as error:
                    errors.append(f"{native_address} mutation runner: {error}")
                else:
                    if sha256(runner_file).lower() != runner_digest.lower():
                        errors.append(f"{native_address} mutation runner hash does not match {runner_path}")
            if not isinstance(runner.get("command"), list) or not runner["command"] or not all(
                isinstance(item, str) and item.strip() for item in runner["command"]
            ):
                errors.append(f"{native_address} mutation report must record runner.command as an argv array")
        mutations = artifact.get("mutations")
        if not isinstance(mutations, list) or not mutations:
            errors.append(f"{native_address} mutation report must include the individual mutation outcomes")
            mutations = []
        mutation_ids: set[str] = set()
        detected_count = 0
        for index, mutation in enumerate(mutations):
            label = f"{native_address} mutation[{index}]"
            if not isinstance(mutation, dict):
                errors.append(f"{label} must be an object")
                continue
            mutation_id = mutation.get("id")
            if not isinstance(mutation_id, str) or not mutation_id.strip() or mutation_id in mutation_ids:
                errors.append(f"{label} needs a unique non-empty id")
                continue
            mutation_ids.add(mutation_id)
            if not isinstance(mutation.get("fault"), str) or not mutation["fault"].strip():
                errors.append(f"{label} must describe the seeded fault")
            if mutation.get("baseline_result") != "passed":
                errors.append(f"{label} must show the unmutated baseline passes")
            if mutation.get("mutant_result") != "failed":
                errors.append(f"{label} must show the mutated implementation fails")
            case_ids = mutation.get("detected_by_cases")
            detected = mutation.get("detected") is True and isinstance(case_ids, list) and bool(case_ids)
            if not detected:
                errors.append(f"{label} was not demonstrated as detected by at least one test case")
            else:
                detected_count += 1
        total, detected = artifact.get("mutations_total"), artifact.get("mutations_detected")
        if any(not isinstance(number, int) or isinstance(number, bool) or number < 0 for number in (total, detected)):
            errors.append(f"{native_address} mutation report requires non-negative integer mutation counts")
        elif total != len(mutations) or detected != detected_count or total <= 0 or detected != total:
            errors.append(
                f"{native_address} mutation counts disagree with per-mutation evidence or some seeded faults escaped detection"
            )
    return artifact


def validate_status_evidence(
    root: pathlib.Path,
    record: dict[str, Any],
    native_address: str,
    binary_digest: str,
    implementation_digest: str,
    status: str,
    require_semantic: bool,
    require_behavioral: bool,
    require_mutation: bool,
    errors: list[str],
) -> None:
    verification = record.get("verification")
    if not isinstance(verification, dict):
        return
    rank = STATUS_RANK[status]
    if rank >= STATUS_RANK["BUILD_VERIFIED"]:
        validate_report_artifact(
            root, verification.get("build"), report_name="build", expected_kind="build-report",
            native_address=native_address, binary_digest=binary_digest,
            implementation_digest=implementation_digest, errors=errors,
        )
    # In strict mode verify_record runs the configured behavioral runner live.
    # A saved report alone is not sufficient because it could be stale.
    if not require_behavioral and rank >= STATUS_RANK["RUNTIME_TESTED"]:
        validate_report_artifact(
            root, verification.get("runtime_tests"), report_name="behavioral tests",
            expected_kind="behavioral-test-report", native_address=native_address,
            binary_digest=binary_digest, implementation_digest=implementation_digest, errors=errors,
        )
    if rank >= STATUS_RANK["VERIFIED"] or require_semantic:
        validate_report_artifact(
            root, verification.get("semantic_review"), report_name="semantic review",
            expected_kind="semantic-review-report", native_address=native_address,
            binary_digest=binary_digest, implementation_digest=implementation_digest, errors=errors,
        )
    if require_mutation:
        validate_report_artifact(
            root, verification.get("mutation_tests"), report_name="mutation tests",
            expected_kind="mutation-test-report", native_address=native_address,
            binary_digest=binary_digest, implementation_digest=implementation_digest, errors=errors,
        )

    if status == "VERIFIED_WITH_DOCUMENTED_SCOPE":
        scope = verification.get("scope")
        if not isinstance(scope, dict):
            errors.append(f"{native_address}: VERIFIED_WITH_DOCUMENTED_SCOPE requires a scope object")
        else:
            path, _ = check_file_hash(
                root, scope.get("path"), scope.get("sha256"),
                f"{native_address} verification scope", errors,
                required_prefix="docs/fidelity/scopes/",
            )
            if path is not None and not path.read_text(encoding="utf-8", errors="replace").strip():
                errors.append(f"{native_address}: verification scope document is empty")


def status_meets_threshold(status: str, minimum_status: str) -> bool:
    if minimum_status == "VERIFIED_WITH_DOCUMENTED_SCOPE":
        return status == "VERIFIED_WITH_DOCUMENTED_SCOPE"
    return STATUS_RANK[status] >= STATUS_RANK[minimum_status]


def run_live_behavioral_runner(
    root: pathlib.Path,
    runner_config: Any,
    native_address: str,
    binary_path: pathlib.Path,
    binary_digest: str,
    implementation: pathlib.Path,
    implementation_digest: str,
    errors: list[str],
) -> dict[str, Any] | None:
    """Run the project's native-specific retail-vs-candidate harness now."""
    if not isinstance(runner_config, dict):
        errors.append(
            f"{native_address}: missing verification.behavioral_runner; strict mode needs a live differential-test harness"
        )
        return None
    runner_rel = runner_config.get("path")
    try:
        runner_path = resolve_repo_path(root, runner_rel)
    except GateError as error:
        errors.append(f"{native_address}: behavioral runner: {error}")
        return None

    raw_args = runner_config.get("args")
    if not isinstance(raw_args, list) or not all(isinstance(item, str) for item in raw_args):
        errors.append(f"{native_address}: behavioral_runner.args must be an array of argument strings")
        return None
    substitutions = {
        "address": native_address,
        "binary": str(binary_path.resolve()),
        "binary_sha256": binary_digest,
        "implementation": str(implementation.resolve()),
        "implementation_sha256": implementation_digest,
        "root": str(root.resolve()),
    }
    used_placeholders: set[str] = set()
    expanded_args: list[str] = []
    for arg in raw_args:
        unknown = set(re.findall(r"\{([a-z0-9_]+)\}", arg)) - set(substitutions)
        if unknown:
            errors.append(f"{native_address}: behavioral runner argument has unknown placeholders: {sorted(unknown)}")
            return None
        used_placeholders.update(re.findall(r"\{([a-z0-9_]+)\}", arg))
        expanded_args.append(re.sub(r"\{([a-z0-9_]+)\}", lambda match: substitutions[match.group(1)], arg))
    required_placeholders = {"address", "binary", "implementation", "binary_sha256", "implementation_sha256"}
    missing_placeholders = required_placeholders - used_placeholders
    if missing_placeholders:
        errors.append(
            f"{native_address}: behavioral_runner.args must pass current context placeholders: {sorted(missing_placeholders)}"
        )
        return None

    timeout = runner_config.get("timeout_seconds", 120)
    if not isinstance(timeout, int) or isinstance(timeout, bool) or not 1 <= timeout <= 1800:
        errors.append(f"{native_address}: behavioral_runner.timeout_seconds must be an integer from 1 to 1800")
        return None
    command = [str(runner_path), *expanded_args]
    try:
        process = subprocess.run(
            command,
            cwd=root,
            capture_output=True,
            text=True,
            check=False,
            timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        errors.append(f"{native_address}: could not execute behavioral runner: {error}")
        return None
    if process.returncode != 0:
        stderr = process.stderr.strip()[-2000:]
        errors.append(
            f"{native_address}: behavioral runner exited with code {process.returncode}; stderr={stderr!r}"
        )
        return None
    try:
        artifact = json.loads(process.stdout)
    except json.JSONDecodeError as error:
        errors.append(
            f"{native_address}: behavioral runner stdout must be one JSON report (no banner/log text): {error}"
        )
        return None
    if not isinstance(artifact, dict):
        errors.append(f"{native_address}: behavioral runner must return a JSON object")
        return None
    if artifact.get("schema_version") != 1 or artifact.get("kind") != "behavioral-test-report":
        errors.append(f"{native_address}: live runner must return schema_version=1, kind='behavioral-test-report'")
    if artifact.get("binary_sha256") != binary_digest:
        errors.append(f"{native_address}: live report is not bound to the selected retail binary SHA-256")
    if artifact.get("implementation_sha256") != implementation_digest:
        errors.append(f"{native_address}: live report is not bound to the current implementation SHA-256")
    try:
        if normalize_address(artifact.get("address")) != native_address:
            errors.append(f"{native_address}: live behavioral report address does not match requested native")
    except (TypeError, ValueError) as error:
        errors.append(f"{native_address}: live behavioral report has invalid address: {error}")
    # Stamp runner identity from what this process actually launched. Do not trust
    # the report itself to self-assert which runner executed it.
    artifact["runner"] = {
        "path": runner_path.relative_to(root.resolve()).as_posix(),
        "sha256": sha256(runner_path),
        "command": [runner_path.relative_to(root.resolve()).as_posix(), *expanded_args],
    }
    validate_behavioral_observations(root, artifact, native_address, errors)
    return artifact


def run_angr_analysis(
    root: pathlib.Path, binary: pathlib.Path, binary_digest: str, addresses: list[str],
) -> dict[str, Any]:
    """Run the isolated CFG helper once; failures remain explicit gate errors."""
    helper = root / "scripts/angr_analyze.py"
    if not addresses:
        return {"functions": {}}
    if not helper.is_file():
        return {"error": "scripts/angr_analyze.py is missing", "functions": {}}
    re_agent = pathlib.Path(os.environ.get(
        "RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/auto-re-agent"))
    interpreter = os.environ.get("IGI_ANGR_PYTHON", str(re_agent / ".venv/bin/python"))
    command = [interpreter, str(helper), "--binary", str(binary.resolve())]
    for address in addresses:
        command.extend(["--address", address])
    try:
        helper_digest = sha256(helper)
        process = subprocess.run(command, cwd=root, text=True, capture_output=True,
                                 check=False, timeout=180)
        if sha256(helper) != helper_digest:
            raise ValueError("angr helper changed during analysis")
        artifact = json.loads(process.stdout)
        if not isinstance(artifact, dict):
            raise ValueError("helper output is not an object")
        if process.returncode not in (0, 2):
            raise ValueError(artifact.get("reason") or process.stderr[-1000:] or
                             f"helper exited {process.returncode}")
        if (artifact.get("schema_version") != 1 or artifact.get("kind") != "angr-analysis"
                or artifact.get("tool") != "angr" or not artifact.get("angr_version")):
            raise ValueError("helper did not identify a supported angr evidence report")
        if artifact.get("binary_sha256") != binary_digest:
            raise ValueError("helper binary digest differs from selected retail executable")
        if pathlib.Path(artifact.get("binary_path", "")).resolve() != binary.resolve():
            raise ValueError("helper loaded a different binary path")
        if artifact.get("bits") != 32 or int(artifact.get("mapped_base", "0"), 0) != 0x400000:
            raise ValueError("helper did not load the retail x86 image at 0x00400000")
        if not isinstance(artifact.get("functions"), dict):
            raise ValueError("helper function results are missing")
        artifact["helper_sha256"] = helper_digest
        artifact["command"] = command
        artifact["stderr"] = process.stderr[-1000:]
        return artifact
    except (OSError, subprocess.TimeoutExpired, ValueError, TypeError) as error:
        return {"error": str(error), "functions": {}}


def verify_record(
    root: pathlib.Path,
    binary_path: pathlib.Path,
    binary_digest: str,
    manifest_records: dict[str, dict[str, Any]],
    native: dict[str, Any],
    implementation: pathlib.Path,
    relative: str,
    minimum_status: str,
    minimum_cfg: float,
    require_semantic: bool,
    require_behavioral: bool,
    require_mutation: bool,
    angr_analysis: dict[str, Any],
    warnings: list[str],
) -> tuple[list[str], dict[str, Any]]:
    address = native["address"]
    errors = check_provenance_comment(native, relative)
    record = manifest_records.get(address)
    details: dict[str, Any] = {"path": relative, "address": address, "errors": errors, "cfg_similarity": None}
    if record is None:
        errors.append(f"{relative}:{native['line']} {address}: no binary-bound verification record")
        return errors, details

    parsed_name = re.split(r"[;,]|\s+-\s+", native["name"], maxsplit=1)[0].strip()
    if record.get("name") != parsed_name:
        errors.append(f"{address}: native name does not match its verification record (source={parsed_name!r})")

    implementation_record = record.get("implementation", {})
    if not isinstance(implementation_record, dict):
        implementation_record = {}
    if implementation_record.get("path") != relative:
        errors.append(f"{address}: implementation path does not match {relative}")
    implementation_digest = sha256(implementation)
    angr_function = angr_analysis.get("functions", {}).get(address, {})
    if angr_analysis.get("error"):
        errors.append(f"{address}: angr analysis failed: {angr_analysis['error']}")
    elif (not isinstance(angr_function, dict) or angr_function.get("result") != "pass"
          or angr_function.get("function_entry") != address or not angr_function.get("nodes")
          or angr_function.get("has_unresolved_jumps") is not False):
        reason = angr_function.get("reason", "no exact recovered function CFG") if isinstance(angr_function, dict) else "invalid function result"
        errors.append(f"{address}: angr CFGFast result is inconclusive or failed: {reason}")
    details["angr"] = {
        **(angr_function if isinstance(angr_function, dict) else {}),
        "helper_sha256": angr_analysis.get("helper_sha256"),
        "implementation_sha256": implementation_digest,
        "binary_sha256": binary_digest,
        "version": angr_analysis.get("angr_version"),
        "limitations": angr_analysis.get("limitations", []),
    }
    if not isinstance(implementation_record.get("sha256"), str) or implementation_record["sha256"].lower() != implementation_digest:
        errors.append(f"{address}: implementation changed since its evidence record was reviewed")

    reference = record.get("game_natives", {})
    if not isinstance(reference, dict):
        reference = {}
    reference_path = reference.get("path", "")
    if not isinstance(reference_path, str) or not reference_path.startswith("scripts/GameNatives/"):
        errors.append(f"{address}: canonical GameNatives source path is missing")
    else:
        source, _ = check_file_hash(
            root, reference_path, reference.get("sha256"), f"{address} GameNatives source",
            errors, required_prefix="scripts/GameNatives/",
        )
        if source is not None:
            source_name = source.stem
            ref_match = GAME_NATIVE_ADDRESS.match(source.name)
            if not ref_match:
                errors.append(f"{address}: canonical GameNatives filename is not address-qualified")
            elif normalize_address(ref_match.group("address")) != address:
                errors.append(f"{address}: canonical GameNatives filename address does not match the record")
            if isinstance(reference.get("signature"), str) and reference["signature"].strip():
                pass
            else:
                errors.append(f"{address}: missing GameNatives function signature")

    verification = record.get("verification", {})
    if not isinstance(verification, dict):
        verification = {}
    status = verification.get("status")
    if status not in STATUS_RANK:
        errors.append(f"{address}: unknown or missing verification status {status!r}")
        return errors, details
    details["status"] = status
    if status == "INCONCLUSIVE":
        limitation = verification.get("inconclusive", {})
        reason = limitation.get("reason") if isinstance(limitation, dict) else None
        errors.append(f"{address}: INCONCLUSIVE — {reason or 'missing explanation of unverified behavior'}")
        # This status always blocks; a fresh comparison can gather evidence but
        # cannot silently promote it or waive the release threshold.
        if isinstance(verification.get("behavioral_runner"), dict):
            live = run_live_behavioral_runner(
                root, verification["behavioral_runner"], address, binary_path,
                binary_digest, implementation, implementation_digest, errors)
            if live is not None:
                details["behavioral_tests"] = {
                    key: live.get(key) for key in
                    ("result", "tests_total", "tests_passed", "tests_failed")}
        return errors, details
    if not status_meets_threshold(status, minimum_status):
        errors.append(f"{address}: status {status} is below required {minimum_status}")
    if not native.get("is_canonical_source") and native.get("comment_status") != status:
        errors.append(f"{address}: five-line comment status does not match manifest status {status}")

    signatures: dict[str, str] = {}
    tool_artifacts: dict[str, dict[str, Any] | None] = {}
    tool_graphs: dict[str, dict[str, set[Any]] | None] = {}
    # Binary Ninja + Radare2 are the primary independent analyses. Ghidra is
    # optional backup evidence; its presence never substitutes for either.
    tools_to_check = list(PRIMARY_ANALYSIS_TOOLS)
    ghidra_config = verification.get("ghidra")
    # Ghidra is optional backup evidence. Only inspect it when the manifest
    # actually configures at least one evidence field; any backup problems are
    # reported as warnings and do not block the required Binary Ninja + r2 pair.
    ghidra_configured = isinstance(ghidra_config, dict) and any(
        isinstance(ghidra_config.get(key), str) and ghidra_config[key].strip()
        for key in ("evidence_id", "evidence_path", "evidence_sha256", "signature")
    )
    if ghidra_configured:
        tools_to_check.extend(BACKUP_ANALYSIS_TOOLS)
    for tool_name in tools_to_check:
        evidence_errors = warnings if tool_name in BACKUP_ANALYSIS_TOOLS else errors
        evidence = verification.get(tool_name, {})
        if not isinstance(evidence, dict):
            evidence = {}
        evidence_id = evidence.get("evidence_id")
        if not isinstance(evidence_id, str) or not evidence_id.strip():
            evidence_errors.append(f"{address}: missing {tool_name} evidence ID")
        evidence_path, _ = check_file_hash(
            root, evidence.get("evidence_path"), evidence.get("evidence_sha256"),
            f"{address} {tool_name} evidence", evidence_errors, required_prefix=EVIDENCE_ROOT,
        )
        signature = evidence.get("signature")
        if not isinstance(signature, str) or not signature.strip():
            evidence_errors.append(f"{address}: missing {tool_name} function signature")
        else:
            signatures[tool_name] = signature
        tool_artifacts[tool_name] = None
        tool_graphs[tool_name] = None
        if evidence_path is not None and STATUS_RANK[status] >= STATUS_RANK["STRUCTURALLY_VERIFIED"]:
            artifact = load_json_artifact(evidence_path, f"{address} {tool_name}", evidence_errors)
            tool_artifacts[tool_name] = artifact
            tool_graphs[tool_name] = validate_disassembly_artifact(
                artifact, tool_name, address, binary_digest,
                signature if isinstance(signature, str) else "", f"{address} {tool_name}", evidence_errors,
            )

    for label, value in (("implementation", implementation_record.get("signature")),
                          ("GameNatives", reference.get("signature"))):
        if not isinstance(value, str) or not value.strip():
            errors.append(f"{address}: missing {label} function signature")

    signature_comparison = verification.get("signature_comparison", {})
    if not isinstance(signature_comparison, dict):
        signature_comparison = {}
    result = signature_comparison.get("result")
    if signature_comparison.get("reviewed") is not True or result not in ACCEPTED_SIGNATURE_RESULTS:
        errors.append(f"{address}: function symbol/signature comparison has not passed review")
    elif result != "exact":
        if not isinstance(signature_comparison.get("rationale"), str) or not signature_comparison["rationale"].strip():
            errors.append(f"{address}: adapted signature comparison requires a written rationale")
    if len(signatures) >= 2 and "binary_ninja" in signatures and "radare2" in signatures and normalized_signature(signatures["binary_ninja"]) != normalized_signature(signatures["radare2"]):
        if result not in {"name-insensitive", "abi-adapted"}:
            errors.append(f"{address}: Binary Ninja/Radare2 signatures disagree without a reviewed adaptation")
        if not isinstance(signature_comparison.get("rationale"), str) or not signature_comparison["rationale"].strip():
            errors.append(f"{address}: Binary Ninja/Radare2 signature disagreement requires a written rationale")
    if "ghidra" in signatures:
        primary_signature = signatures.get("binary_ninja")
        if primary_signature and normalized_signature(primary_signature) != normalized_signature(signatures["ghidra"]):
            backup = verification.get("ghidra", {})
            if backup.get("comparison_reviewed") is not True or not isinstance(backup.get("comparison_rationale"), str) or not backup["comparison_rationale"].strip():
                warnings.append(f"{address}: Ghidra backup signature disagreement requires a reviewed rationale")

    # Only an actual CFG graph comparison from the primary pair satisfies structural verification.
    if STATUS_RANK[status] >= STATUS_RANK["STRUCTURALLY_VERIFIED"]:
        if tool_graphs.get("binary_ninja") is not None and tool_graphs.get("radare2") is not None:
            score = cfg_score(tool_graphs["binary_ninja"], tool_graphs["radare2"])
            details["cfg_similarity"] = score
            recorded_score = verification.get("cfg_similarity")
            if not isinstance(recorded_score, (int, float)) or isinstance(recorded_score, bool) or not math.isfinite(float(recorded_score)) or abs(float(recorded_score) - score) > 1e-9:
                errors.append(f"{address}: recorded CFG score does not match recomputed graph score {score:.3f}")
            if score < minimum_cfg:
                errors.append(f"{address}: Binary Ninja/Radare2 CFG graph similarity {score:.3f} is below required {minimum_cfg:.2f}")
            if tool_graphs.get("ghidra") is not None:
                backup_score_bn = cfg_score(tool_graphs["binary_ninja"], tool_graphs["ghidra"])
                backup_score_r2 = cfg_score(tool_graphs["radare2"], tool_graphs["ghidra"])
                details["ghidra_backup_cfg_similarity"] = {
                    "binary_ninja": backup_score_bn,
                    "radare2": backup_score_r2,
                }
                # Ghidra is diagnostic backup evidence. It flags an unexpected
                # disagreement, but cannot upgrade a failed primary comparison.
                if min(backup_score_bn, backup_score_r2) < minimum_cfg:
                    backup = verification.get("ghidra", {})
                    if not (isinstance(backup, dict) and backup.get("comparison_reviewed") is True
                            and isinstance(backup.get("comparison_rationale"), str)
                            and backup["comparison_rationale"].strip()):
                        warnings.append(
                            f"{address}: Ghidra backup CFG disagrees with a primary tool; reviewed rationale required"
                        )
        else:
            errors.append(f"{address}: structural status requires valid CFG graph artifacts from Binary Ninja and Radare2")

    validate_status_evidence(
        root, record, address, binary_digest, implementation_digest, status,
        require_semantic, require_behavioral, require_mutation, errors,
    )
    if require_behavioral:
        live_report = run_live_behavioral_runner(
            root,
            verification.get("behavioral_runner"),
            address,
            binary_path,
            binary_digest,
            implementation,
            implementation_digest,
            errors,
        )
        if live_report is not None:
            details["behavioral_tests"] = {
                "result": live_report.get("result"),
                "tests_total": live_report.get("tests_total"),
                "tests_passed": live_report.get("tests_passed"),
                "tests_failed": live_report.get("tests_failed"),
                "runner": live_report.get("runner", {}).get("path"),
            }
    details["errors"] = errors
    return errors, details


def write_report(path_arg: pathlib.Path, root: pathlib.Path, report: dict[str, Any]) -> None:
    path = path_arg if path_arg.is_absolute() else root / path_arg
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Gate IGI native code using Binary Ninja + Radare2, with optional Ghidra backup and retail differential tests."
    )
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--staged", action="store_true", help="Check staged native files only (pre-commit).")
    parser.add_argument("--changed", action="store_true", help="Check staged, unstaged, and untracked native files.")
    parser.add_argument(
        "--tree", nargs="?", const=pathlib.Path("."), type=pathlib.Path,
        help="Check native C++ source/header files under both native roots, optionally limited to a path.",
    )
    parser.add_argument("--production-tree", action="store_true",
                        help="Check every production C++ source/header under engine/src/retail, regardless of Git status.")
    parser.add_argument("--binary", type=pathlib.Path, default=DEFAULT_BINARY)
    parser.add_argument("--manifest", type=pathlib.Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--threshold", choices=STATUS_RANK, default="STRUCTURALLY_VERIFIED")
    parser.add_argument("--cfg-threshold", type=float, default=0.85)
    parser.add_argument("--require-semantic-evidence", dest="require_semantic_evidence", action="store_true",
                        help="Require a passed source-semantic review for every changed native (default).")
    parser.add_argument("--skip-semantic-review", dest="require_semantic_evidence", action="store_false",
                        help="Temporarily disable the semantic-review requirement; not recommended for release builds.")
    behavior_group = parser.add_mutually_exclusive_group()
    behavior_group.add_argument("--require-behavioral-tests", dest="require_behavioral_tests", action="store_true",
                                help="Require retail-vs-candidate output comparisons (default).")
    behavior_group.add_argument("--allow-structural-only", dest="require_behavioral_tests", action="store_false",
                                help="Allow structural-only checks; cannot detect wrong calculations and is not release-safe.")
    parser.add_argument("--require-mutation-tests", action="store_true",
                        help="Require mutation testing to prove the behavioral suite catches seeded logic faults.")
    parser.set_defaults(require_semantic_evidence=True, require_behavioral_tests=True)
    parser.add_argument("--report", type=pathlib.Path,
                        help="Write a machine-readable JSON report (relative to --root unless absolute).")
    args = parser.parse_args()

    if not math.isfinite(args.cfg_threshold) or not 0.0 <= args.cfg_threshold <= 1.0:
        parser.error("--cfg-threshold must be a finite number between 0.0 and 1.0")
    selected_modes = sum((args.staged, args.changed, args.tree is not None, args.production_tree))
    if selected_modes > 1:
        parser.error("choose only one of --staged, --changed, --tree, or --production-tree")

    root = args.root.resolve()
    if args.tree is not None:
        mode = "tree"
        scan_root = args.tree
    elif args.production_tree:
        mode = "production-tree"
        scan_root = None
    elif args.changed:
        mode = "changed"
        scan_root = None
    else:
        # --staged is the default behavior when no mode is explicitly chosen.
        mode = "staged"
        scan_root = None

    report: dict[str, Any] = {
        "schema_version": 1,
        "result": "blocked",
        "root": str(root),
        "mode": mode,
        "threshold": args.threshold,
        "cfg_threshold": args.cfg_threshold,
        "primary_analysis_tools": list(PRIMARY_ANALYSIS_TOOLS),
        "backup_analysis_tools": list(BACKUP_ANALYSIS_TOOLS),
        "behavioral_tests_required": args.require_behavioral_tests,
        "semantic_review_required": args.require_semantic_evidence,
        "checked_files": [],
        "checked_natives": [],
        "deleted_files": [],
        "failures": [],
        "warnings": [],
    }
    try:
        files, deleted = changed_paths(root, mode, scan_root)
        manifest_path = args.manifest if args.manifest.is_absolute() else root / args.manifest
        manifest = load_manifest(manifest_path)
        manifest_records = index_manifest(manifest)
        binary_path = args.binary if args.binary.is_absolute() else root / args.binary
        if files and not binary_path.is_file():
            raise GateError(f"Retail executable is missing: {binary_path}; native evidence cannot be verified")
        binary_digest = sha256(binary_path) if binary_path.is_file() else ""
        expected_digest = manifest.get("binary_sha256", "")
        if not isinstance(expected_digest, str):
            expected_digest = ""
        if files and (not expected_digest or expected_digest.lower() != binary_digest):
            raise GateError(
                f"Manifest target SHA-256 does not match {binary_path}; expected {expected_digest or '<missing>'}, "
                f"observed {binary_digest}"
            )
    except (GateError, OSError) as error:
        message = str(error)
        print(f"native_gate: BLOCKED: {message}", file=sys.stderr)
        report["failures"].append(message)
        if args.report:
            try:
                write_report(args.report, root, report)
            except OSError as report_error:
                print(f"native_gate: could not write report: {report_error}", file=sys.stderr)
        return 1

    report["binary_sha256"] = binary_digest
    report["checked_files"] = [str(path.relative_to(root).as_posix()) for path in files]
    report["deleted_files"] = deleted
    failures: list[str] = []
    warnings: list[str] = []
    addresses: set[str] = set()
    for path in files:
        try:
            addresses.update(native["address"] for native in implementation_natives(
                path, path.relative_to(root).as_posix()))
        except (GateError, OSError, ValueError):
            # The normal per-file pass below reports the precise source error.
            continue
    angr_analysis = run_angr_analysis(root, binary_path, binary_digest, sorted(addresses))
    report["angr_analysis"] = angr_analysis
    report["supplemental_analysis_tools"] = ["angr"]
    if addresses:
        recovered = sum(item.get("result") == "pass" for item in
                        angr_analysis.get("functions", {}).values() if isinstance(item, dict))
        print(f"native_gate: angr CFGFast {recovered}/{len(addresses)} exact entries recovered (structural evidence only)")

    for deleted_path in deleted:
        original = _run_git(root, ["git", "show", f"HEAD:{deleted_path}"])
        if original.returncode != 0:
            failures.append(f"{deleted_path}: deleted native source could not be inspected against HEAD")
            continue
        old_addresses = source_addresses_from_text(original.stdout)
        if not retired_file_is_reviewed(manifest, deleted_path):
            failures.append(f"{deleted_path}: deletion of a native-root .cpp requires a reviewed retired_files entry")
        for address in sorted(old_addresses):
            if not retired_native_is_reviewed(manifest, address, deleted_path):
                failures.append(f"{deleted_path}: removed native {address} requires a reviewed retired_natives entry")

    checked = 0
    details_by_key: set[tuple[str, str]] = set()
    for implementation in files:
        try:
            relative = implementation.relative_to(root).as_posix()
        except ValueError:
            failures.append(f"{implementation}: resolved source is outside repository root")
            continue
        if mode == "staged":
            try:
                if _has_unstaged_changes(root, relative):
                    failures.append(
                        f"{relative}: staged-only verification refused because the worktree differs from the index; "
                        "stage or discard the remaining edits before running the gate"
                    )
            except GateError as error:
                failures.append(str(error))
        for address in sorted(removed_native_addresses(root, implementation, relative)):
            if not retired_native_is_reviewed(manifest, address, relative):
                failures.append(f"{relative}: removing Native provenance for {address} requires a reviewed retired_natives entry")

        try:
            natives = implementation_natives(implementation, relative)
        except GateError as error:
            failures.append(str(error))
            continue
        has_host_helpers, helper_errors = check_host_helper_comments(implementation, relative)
        failures.extend(helper_errors)
        if not natives:
            if any(relative.startswith(prefix) for prefix in NATIVE_ROOTS) and not has_host_helpers:
                failures.append(f"{relative}: changed native source/header has no Native or HOST HELPER provenance")
            continue
        for native in natives:
            checked += 1
            native_errors, details = verify_record(
                root,
                binary_path,
                binary_digest,
                manifest_records,
                native,
                implementation,
                relative,
                args.threshold,
                args.cfg_threshold,
                args.require_semantic_evidence,
                args.require_behavioral_tests,
                args.require_mutation_tests,
                angr_analysis,
                warnings,
            )
            failures.extend(native_errors)
            report["checked_natives"].append(details)
            details_by_key.add((relative, native["address"]))

    report["checked_native_count"] = checked
    report["failures"] = failures
    report["warnings"] = warnings
    if warnings:
        for warning in warnings:
            print(f"native_gate: WARNING: {warning}")
    if failures:
        print(
            f"native_gate: BLOCKED {checked} changed native(s) "
            f"(status>={args.threshold}, CFG>={args.cfg_threshold:.2f}, "
            f"behavioral-tests={'required' if args.require_behavioral_tests else 'disabled'})"
        )
        for failure in failures:
            print(f"  - {failure}")
        print("Comments and manifest status labels alone do not authorize compilation.")
        exit_code = 1
    else:
        report["result"] = "passed"
        print(
            f"native_gate: OK — {checked} changed native(s) passed required "
            f"binary/source/evidence checks (status>={args.threshold}, CFG>={args.cfg_threshold:.2f}, "
            f"behavioral-tests={'required' if args.require_behavioral_tests else 'disabled'})"
        )
        exit_code = 0

    if args.report:
        try:
            write_report(args.report, root, report)
        except OSError as error:
            print(f"native_gate: BLOCKED: could not write verification report: {error}", file=sys.stderr)
            return 1
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
