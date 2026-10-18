#!/usr/bin/env python3
"""Bind a verified Windows build to its source snapshot and executable bytes."""
from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import struct
import sys


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def sources(root):
    paths = {root / p for p in (
        "CMakeLists.txt", "build.sh", "run.sh",
        "docs/fidelity/native-verification.json", "docs/fidelity/gate-integrity.sha256")}
    for base in (root / "engine", root / "scripts"):
        for path in base.rglob("*"):
            if path.is_file() and (path.name == "CMakeLists.txt" or path.suffix in {
                    ".cpp", ".hpp", ".h", ".c", ".cc", ".cxx", ".cmake", ".py"}):
                paths.add(path)
    return {str(p.relative_to(root)): digest(p) for p in sorted(paths)}


def load(path):
    value = json.loads(path.read_text())
    if not isinstance(value, dict):
        raise ValueError(f"not a JSON object: {path}")
    return value


def build_inputs(root):
    base = root / "build-win32"
    cache = base / "CMakeCache.txt"
    paths = {cache}
    paths.update(base.glob("CMakeFiles/*/CMakeCXXCompiler.cmake"))
    paths.update(base.rglob("flags.make"))
    paths.update(base.rglob("link.txt"))
    if (base / "compile_commands.json").is_file():
        paths.add(base / "compile_commands.json")
    compiler = None
    for line in cache.read_text().splitlines():
        if line.startswith("CMAKE_CXX_COMPILER:"):
            compiler = pathlib.Path(line.split("=", 1)[1].strip('"'))
            break
    if compiler is None or not compiler.is_file():
        raise ValueError("configured Windows compiler executable is unavailable")
    return {"configuration": {str(p.relative_to(root)): digest(p) for p in sorted(paths)},
            "compiler": {"path": str(compiler.resolve()), "sha256": digest(compiler)}}


def gate_report(root, path):
    report = load(path)
    if (report.get("result") != "passed" or report.get("mode") != "production-tree"
            or report.get("threshold") != "VERIFIED" or report.get("failures") != []
            or not report.get("checked_native_count", 0)
            or not report.get("semantic_review_required")
            or not report.get("behavioral_tests_required")
            or not 0.85 <= float(report.get("cfg_threshold", 0)) <= 1.0):
        raise ValueError("full production VERIFIED evidence is required")
    natives = report.get("checked_natives", [])
    if len(natives) != report["checked_native_count"]:
        raise ValueError("incomplete native verification report")
    for native in natives:
        path = (root / native["path"]).resolve()
        path.relative_to(root)
        if native.get("errors") or native.get("angr", {}).get("implementation_sha256") != digest(path):
            raise ValueError(f"native evidence is stale: {native['path']}")
    return report


def require_x86_pe(binary):
    data = binary.read_bytes()
    if len(data) < 64 or data[:2] != b"MZ":
        raise ValueError("not a Windows PE executable")
    offset = struct.unpack_from("<I", data, 0x3C)[0]
    if (len(data) < offset + 26 or data[offset:offset + 4] != b"PE\0\0"
            or struct.unpack_from("<H", data, offset + 4)[0] != 0x14C
            or struct.unpack_from("<H", data, offset + 24)[0] != 0x10B):
        raise ValueError("executable must be Windows PE32 Intel x86")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("capture", "snapshot", "configure", "seal", "check"))
    parser.add_argument("--root", type=pathlib.Path, required=True)
    parser.add_argument("--gate-report", type=pathlib.Path)
    parser.add_argument("--snapshot", type=pathlib.Path)
    parser.add_argument("--input-snapshot", type=pathlib.Path)
    parser.add_argument("--receipt", type=pathlib.Path)
    parser.add_argument("--binary", type=pathlib.Path)
    parser.add_argument("--output", type=pathlib.Path)
    args = parser.parse_args()
    root = args.root.resolve()
    if args.action == "capture":
        if not args.output:
            parser.error("capture requires --output")
        result = {"schema_version": 1, "kind": "verification-input-snapshot", "sources": sources(root)}
    elif args.action == "snapshot":
        if not args.gate_report or not args.output or not args.input_snapshot:
            parser.error("snapshot requires --gate-report, --input-snapshot and --output")
        captured = load(args.input_snapshot)
        if (captured.get("schema_version") != 1 or captured.get("kind") != "verification-input-snapshot"
                or captured.get("sources") != sources(root)):
            raise ValueError("verification inputs changed while the gate was running")
        report = gate_report(root, args.gate_report)
        result = {"schema_version": 1, "kind": "native-build-snapshot",
                  "sources": sources(root), "retail_sha256": report["binary_sha256"],
                  "gate_report": str(args.gate_report.resolve()),
                  "gate_report_sha256": digest(args.gate_report)}
    elif args.action == "configure":
        if not args.snapshot or not args.output:
            parser.error("configure requires --snapshot and --output")
        result = load(args.snapshot)
        if (result.get("schema_version") != 1 or result.get("kind") != "native-build-snapshot"
                or sources(root) != result.get("sources")):
            raise ValueError("source changed before Windows compilation")
        result["build_inputs"] = build_inputs(root)
    else:
        if not args.binary:
            parser.error("--binary is required")
        require_x86_pe(args.binary)
        if args.action == "seal":
            if not args.snapshot or not args.output:
                parser.error("seal requires --snapshot and --output")
            result = load(args.snapshot)
            if result.get("schema_version") != 1 or result.get("kind") != "native-build-snapshot":
                raise ValueError("invalid pre-build snapshot")
            report_path = pathlib.Path(result["gate_report"])
            gate_report(root, report_path)
            if digest(report_path) != result["gate_report_sha256"]:
                raise ValueError("gate evidence changed during build")
            if sources(root) != result["sources"]:
                raise ValueError("source/build-script files changed during build")
            if build_inputs(root) != result.get("build_inputs"):
                raise ValueError("Windows compiler/configuration changed during build")
            result.update(kind="verified-windows-build", binary_sha256=digest(args.binary))
        else:
            if not args.receipt or not args.gate_report:
                parser.error("check requires --receipt and --gate-report")
            result = load(args.receipt)
            report = gate_report(root, args.gate_report)
            if (result.get("schema_version") != 1 or result.get("kind") != "verified-windows-build"
                    or result.get("binary_sha256") != digest(args.binary)
                    or result.get("retail_sha256") != report["binary_sha256"]
                    or result.get("sources") != sources(root)
                    or result.get("build_inputs") != build_inputs(root)):
                raise ValueError("executable or build receipt is stale; run ./build.sh")
            print("Windows x86 executable matches its verified build snapshot.")
            return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError, TypeError, struct.error) as error:
        print(f"build_attestation: BLOCKED: {error}", file=sys.stderr)
        raise SystemExit(1)
