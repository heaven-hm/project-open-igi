#!/usr/bin/env python3
"""Compile one candidate translation unit for the retail x86 target and record it.

Produces a ``build-report`` evidence artifact that ``scripts/native_gate.py``
accepts for ranks >= BUILD_VERIFIED.  The report is only written when the
compiler actually succeeds, so it can never be hand-faked.

    python3 scripts/native_build_evidence.py --root . --address 0x004B26B0 \
        --implementation engine/src/retail/texture/texture_upload_natives.cpp
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess

BINARY_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
COMPILER = "i686-w64-mingw32-g++"


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default=".")
    parser.add_argument("--address", required=True)
    parser.add_argument("--implementation", required=True)
    parser.add_argument("--output", help="defaults to docs/fidelity/evidence/<addr>-build.json")
    parser.add_argument("--compiler", default=COMPILER)
    parser.add_argument("--output-object", default="/tmp/native-x86.o")
    parser.add_argument("--source", help="optional extra TU list (comma separated)")
    args = parser.parse_args()

    root = pathlib.Path(args.root).resolve()
    implementation = root / args.implementation
    if not implementation.is_file():
        raise SystemExit("missing implementation: %s" % implementation)

    sources = [args.implementation]
    if args.source:
        sources.extend(item.strip() for item in args.source.split(",") if item.strip())

    compile_args = [args.compiler, "-std=c++20", "-I", "engine/include", "-c"]
    compile_args.extend(sources)
    compile_args.extend(["-o", args.output_object])
    completed = subprocess.run(compile_args, cwd=root, capture_output=True, text=True)
    if completed.returncode != 0:
        raise SystemExit(
            "x86 compile failed (%d):\n%s\n%s"
            % (completed.returncode, completed.stdout, completed.stderr)
        )
    if not pathlib.Path(args.output_object).is_file():
        raise SystemExit("compiler reported success but produced no object")

    address = "0x%08X" % int(args.address, 16)
    output = pathlib.Path(
        args.output or "docs/fidelity/evidence/%s-build.json" % address
    )
    if not output.is_absolute():
        output = root / output

    report = {
        "schema_version": 1,
        "kind": "build-report",
        "result": "passed",
        "address": address,
        "binary_sha256": BINARY_SHA256,
        "implementation_sha256": sha256(implementation),
        "executed": True,
        "command": " ".join(compile_args),
        "target": "Windows x86 (Intel 80386 COFF object)",
        "note": "Candidate translation unit compiled successfully for the retail x86 architecture.",
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=1) + "\n")
    try:
        label = str(output.relative_to(root))
    except ValueError:
        label = str(output)
    print("wrote %s (%s)" % (label, sha256(output)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
