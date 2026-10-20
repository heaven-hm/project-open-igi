#!/usr/bin/env python3
"""Group gate diagnostics and optionally rerun exact natives; never attest runtime parity."""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import signal
import shutil
import subprocess
import sys
import tempfile
import time

ADDRESS = re.compile(r"0x[0-9a-fA-F]{8}\b")


def address(value: str) -> str:
    try:
        number = int(value, 16)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("expected a hexadecimal native address") from exc
    if not 0 <= number <= 0xFFFFFFFF:
        raise argparse.ArgumentTypeError("address must fit 32 bits")
    return f"0x{number:08X}"


def group_report(report: dict, selected: set[str] | None = None) -> dict:
    groups: dict[tuple[str, str | None], dict] = {}
    for record in report.get("checked_natives", []):
        native = address(record["address"])
        key = (record["path"], native)
        item = groups.setdefault(key, {"path": key[0], "address": native, "records": [], "failures": []})
        item["records"].append(record)
        item["failures"].extend(record.get("errors", []))
    uncovered = []
    for failure in report.get("failures", []):
        match = ADDRESS.search(failure)
        native = address(match.group()) if match else None
        # Source-prefixed diagnostics can also contain a line number before the address.
        prefix = failure.split(":", 1)[0]
        source = prefix if prefix.endswith((".cpp", ".hpp", ".h", ".cc", ".cxx")) else None
        matches = [item for (path, va), item in groups.items()
                   if (source is None or source == path) and native is not None and va == native]
        if matches:
            for item in matches:
                item["failures"].append(failure)
        elif source is not None:
            item = groups.setdefault((source, native), {"path": source, "address": native,
                                                       "records": [], "failures": []})
            item["failures"].append(failure)
        else:
            uncovered.append(failure)
    items = sorted(groups.values(), key=lambda item: (not item["path"].startswith("engine/"),
                                                      item["path"], item["address"] or ""))
    for item in items:
        item["failures"] = list(dict.fromkeys(item["failures"]))
    filtered = [item for item in items if not selected or item["address"] in selected]
    return {"schema_version": 1, "scope": "diagnostic queue; no runtime or release acceptance",
            "report_result": report.get("result"),
            "checked_record_count": len(report.get("checked_natives", [])),
            "group_count": len(items), "selected_group_count": len(filtered),
            "unique_address_count": len({item["address"] for item in items if item["address"]}),
            "unassigned_failures": uncovered,
            "unmatched_addresses": sorted((selected or set()) - {item["address"] for item in items}),
            "groups": filtered}


def digest(path: Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            hasher.update(block)
    return hasher.hexdigest()


def tool_identities(root: Path) -> dict:
    """Conservative superset of tools used by the existing gate/runners."""
    commands = {"gate_python": sys.executable, "python3": "python3", "c++": "c++",
                "clang++": "clang++", "mingw": "i686-w64-mingw32-g++", "r2": "r2",
                "CC": os.environ.get("CC", "cc"), "CXX": os.environ.get("CXX", "c++"),
                "angr_python": os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python")),
                "legacy_angr_python": str(Path(os.environ.get(
                    "RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent")) / ".venv/bin/python")}
    if os.environ.get("IGI_UNICORN_PYTHON"):
        commands["unicorn_python"] = os.environ["IGI_UNICORN_PYTHON"]
    result = {}
    for name, command in commands.items():
        resolved = shutil.which(os.path.expanduser(command))
        if resolved is None:
            result[name] = {"command": command, "resolved": None}
            continue
        path = Path(resolved).resolve()
        result[name] = {"command": command, "resolved": str(path), "sha256": digest(path)}
    return result


def fingerprint(root: Path, binary: Path, manifest: Path, report: Path) -> str:
    # Include reference sources/evidence as well as all scripts and engine inputs.
    paths = {binary.resolve(), manifest.resolve(), report.resolve(), Path(__file__).resolve()}
    for folder in ("engine", "scripts", "docs/fidelity", "assets"):
        for path in (root / folder).rglob("*"):
            if path.is_file() and "__pycache__" not in path.parts:
                paths.add(path.resolve())
    hasher = hashlib.sha256()
    for path in sorted(paths):
        hasher.update(str(path).encode() + b"\0" + digest(path).encode() + b"\0")
    # Changed interpreter/tool configuration invalidates saved diagnostic runs as well.
    environment = {key: value for key, value in os.environ.items()
                   if key.startswith(("IGI_", "FRIDA_", "REA_")) or key in ("PATH", "PYTHONPATH", "CC", "CXX", "CFLAGS", "CXXFLAGS", "CPPFLAGS", "LDFLAGS")}
    hasher.update(json.dumps(environment, sort_keys=True).encode())
    hasher.update(json.dumps(tool_identities(root), sort_keys=True).encode())
    return hasher.hexdigest()


def atomic_json(path: Path, value: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, delete=False) as stream:
            temporary = Path(stream.name)
            json.dump(value, stream, indent=2)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def run_queue(args, queue: dict, report_path: Path) -> int:
    root, binary, manifest = args.root.resolve(), args.binary.resolve(), args.manifest.resolve()
    if digest(binary) != args.report_data.get("binary_sha256"):
        raise ValueError("gate report retail SHA-256 does not match --binary")
    if queue.get("unmatched_addresses"):
        raise ValueError("requested addresses absent from gate report: " + ", ".join(queue["unmatched_addresses"]))
    identities = tool_identities(root)
    reuse_allowed = all(item.get("sha256") for item in identities.values())
    if args.resume and not reuse_allowed:
        print("Tool identity incomplete; checkpoint reuse disabled.", file=sys.stderr)
    initial = fingerprint(root, binary, manifest, report_path)
    checkpoint = args.output / "checkpoint.json"
    state = {"schema_version": 1, "fingerprint": initial, "scope": queue["scope"], "runs": {}}
    if args.resume and reuse_allowed and checkpoint.exists():
        previous = json.loads(checkpoint.read_text())
        if previous.get("fingerprint") == initial:
            state = previous
        else:
            print("Inputs changed; previous diagnostic results will not be reused.", file=sys.stderr)
    failed = bool(queue["unassigned_failures"])
    for item in queue["groups"]:
        native, source = item["address"], item["path"]
        if native is None:
            failed = True
            print(f"BLOCKED {source}: no address-bound identity", file=sys.stderr)
            continue
        candidate = (root / source).resolve()
        if not candidate.is_relative_to(root) or not candidate.is_file():
            raise ValueError(f"invalid or missing source path: {source}")
        if fingerprint(root, binary, manifest, report_path) != initial:
            raise ValueError("inputs changed during queue execution; rerun with the current gate report")
        key = f"{source}:{native}"
        saved = state["runs"].get(key)
        if reuse_allowed and saved and saved.get("completed") and all(
                Path(saved[k]).is_file() and digest(Path(saved[k])) == saved[k + "_sha256"]
                for k in ("log", "report")):
            failed |= saved["returncode"] != 0
            print(f"REUSED {native} {source}: exit {saved['returncode']}")
            continue
        stem = native + "-" + hashlib.sha256(source.encode()).hexdigest()[:12]
        output_report, log = args.output / (stem + ".json"), args.output / (stem + ".log")
        output_report.unlink(missing_ok=True)
        command = [sys.executable, str(root / "scripts/native_gate.py"), "--root", str(root),
                   "--file", source, "--address", native, "--binary", str(binary),
                   "--manifest", str(manifest), "--threshold", "VERIFIED", "--cfg-threshold", "0.85",
                   "--require-semantic-evidence", "--require-behavioral-tests", "--report", str(output_report)]
        started = time.time()
        with log.open("w") as stream:
            process = subprocess.Popen(command, cwd=root, stdout=stream, stderr=subprocess.STDOUT,
                                       start_new_session=True)
            try:
                code = process.wait(timeout=args.timeout)
            except (subprocess.TimeoutExpired, KeyboardInterrupt):
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
                stream.write("\nQueue stopped this owned gate process group.\n")
                raise
        unchanged = fingerprint(root, binary, manifest, report_path) == initial
        saved = {"completed": unchanged and output_report.is_file(), "returncode": code,
                 "elapsed_seconds": round(time.time() - started, 2), "command": command,
                 "log": str(log), "log_sha256": digest(log), "report": str(output_report),
                 "report_sha256": digest(output_report) if output_report.is_file() else None}
        state["runs"][key] = saved
        atomic_json(checkpoint, state)
        print(f"CHECKED {native} {source}: exit {code}", flush=True)
        failed |= code != 0 or not saved["completed"]
        if not unchanged:
            raise ValueError("inputs changed during native check; result is not reusable")
    return int(failed)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument("--gate-report", type=Path, default=Path("build/native-verification.json"))
    parser.add_argument("--address", action="append", type=address)
    parser.add_argument("--run", action="store_true", help="Run strict focused gate checks sequentially")
    parser.add_argument("--resume", action="store_true", help="Reuse only unchanged completed diagnostic checks")
    parser.add_argument("--timeout", type=float, default=600)
    parser.add_argument("--output", type=Path, default=Path("build/native-verification-queue"))
    parser.add_argument("--binary", type=Path, default=Path("/Users/haseeb-mir/Downloads/IGI/igi.exe"))
    parser.add_argument("--manifest", type=Path, default=Path("docs/fidelity/native-verification.json"))
    args = parser.parse_args()
    args.root = args.root.resolve()
    for name in ("gate_report", "output", "manifest", "binary"):
        value = getattr(args, name)
        setattr(args, name, (args.root / value).resolve())
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error("--timeout must be finite and positive")
    try:
        args.report_data = json.loads(args.gate_report.read_text())
        queue = group_report(args.report_data, set(args.address or []))
        atomic_json(args.output / "queue.json", queue)
        if args.run:
            if not queue["groups"]:
                raise ValueError("empty queue cannot establish acceptance")
            return run_queue(args, queue, args.gate_report)
        print(json.dumps(queue, indent=2))
        return int(bool(queue["unmatched_addresses"]))
    except (OSError, ValueError, KeyError, subprocess.TimeoutExpired) as exc:
        print(f"native_verification_queue: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
