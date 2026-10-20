#!/usr/bin/env python3
"""CMake bridge for retail IGI native verification.

Executes the exact same verification gate logic as build.sh:
- In development mode: discovers referenced natives, generates Frida probe targets,
  and runs native_gate.py at STRUCTURALLY_VERIFIED with CFG >= 0.85.
- In acceptance mode: runs native_gate.py --changed at VERIFIED.
"""

from __future__ import annotations

import argparse
import os
import math
import shutil
import pathlib
import subprocess
import sys

from native_gate import STATUS_RANK


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--binary", type=pathlib.Path, default=None)
    parser.add_argument("--development", action="store_true", default=True,
                        help="Run development gate on referenced natives (default).")
    parser.add_argument("--full-production", action="store_true",
                        help="Run strict changed-native gate at VERIFIED (legacy option name).")
    parser.add_argument("--threshold", choices=STATUS_RANK, default=None)
    parser.add_argument("--cfg-threshold", type=float, default=0.85)
    parser.add_argument("--report", type=pathlib.Path, default=None)

    args = parser.parse_args()
    root = args.root.resolve()
    minimum = "VERIFIED" if args.full_production else "STRUCTURALLY_VERIFIED"
    threshold = args.threshold or minimum
    if STATUS_RANK[threshold] < STATUS_RANK[minimum]:
        parser.error(f"this mode requires status >= {minimum}")
    if not math.isfinite(args.cfg_threshold) or not 0.85 <= args.cfg_threshold <= 1.0:
        parser.error("CFG threshold must be finite and between 0.85 and 1.0")

    # Determine binary path
    binary_path = args.binary
    if binary_path is None:
        env_binary = os.environ.get("IGI_RETAIL_EXE")
        if env_binary:
            binary_path = pathlib.Path(env_binary)
        else:
            binary_path = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")

    if not binary_path.is_file():
        print(f"!! [CMake Native Gate] Retail executable not found at: {binary_path}", file=sys.stderr)
        print("   Set IGI_RETAIL_EXE environment variable or -DIGI_RETAIL_EXE=<path>", file=sys.stderr)
        return 1

    os.environ["IGI_RETAIL_EXE"] = str(binary_path.resolve())

    # Set toolchain defaults matching build.sh
    # Note: Do NOT .resolve() .venv-angr/bin/python, because resolving the symlink breaks virtualenv prefix detection.
    if "IGI_GAME_NATIVES_DIR" not in os.environ:
        os.environ["IGI_GAME_NATIVES_DIR"] = str((root / "scripts/GameNatives").resolve())
    if "IGI_ANGR_PYTHON" not in os.environ:
        os.environ["IGI_ANGR_PYTHON"] = str(root / ".venv-angr/bin/python")
    if "BINARY_NINJA_MCP_URL" not in os.environ:
        os.environ["BINARY_NINJA_MCP_URL"] = "http://127.0.0.1:24642/mcp"
    if "GHIDRA_INSTALL_DIR" not in os.environ:
        os.environ["GHIDRA_INSTALL_DIR"] = "/opt/homebrew/opt/ghidra/libexec"
    if "GHIDRA_MCP_URL" not in os.environ:
        os.environ["GHIDRA_MCP_URL"] = "http://127.0.0.1:8771"
    if "RADARE2_BIN" not in os.environ:
        r2 = shutil.which("r2")
        os.environ["RADARE2_BIN"] = r2 or "/opt/homebrew/bin/r2"

    is_production = args.full_production

    build_dir = root / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    if is_production:
        print("==> [CMake Native Gate] Running changed-native verification gate (threshold=VERIFIED)...")
        attest_script = root / "scripts/build_attestation.py"
        if not attest_script.is_file():
            print(f"!! [CMake Native Gate] Missing script: {attest_script}", file=sys.stderr)
            return 1
        inputs_json = build_dir / "native-cmake-verification-inputs.json"
        capture = subprocess.run([sys.executable, str(attest_script), "capture", "--root", str(root),
                                  "--output", str(inputs_json)])
        if capture.returncode != 0:
            return capture.returncode

        gate_args = [
            sys.executable,
            str(root / "scripts/native_gate.py"),
            "--root", str(root),
            "--changed",
            "--require-semantic-evidence", "--require-behavioral-tests",
            "--threshold", threshold,
            "--cfg-threshold", str(args.cfg_threshold),
            "--report", str(args.report or (build_dir / "native-cmake-verification.json")),
        ]
    else:
        print("==> [CMake Native Gate] Discovering and verifying statically referenced runtime native candidates...")
        discover_script = root / "scripts/discover_referenced_natives.py"
        if not discover_script.is_file():
            print(f"!! [CMake Native Gate] Missing script: {discover_script}", file=sys.stderr)
            return 1

        proc = subprocess.run([sys.executable, str(discover_script), "--root", str(root)],
                              capture_output=True, text=True)
        if proc.returncode != 0:
            print(f"!! [CMake Native Gate] Discovery failed: {proc.stderr}", file=sys.stderr)
            return 1

        addresses = [line.strip() for line in proc.stdout.splitlines() if line.strip()]
        if not addresses:
            print("!! [CMake Native Gate] No runtime-path native candidates discovered.", file=sys.stderr)
            return 1

        print(f"    Discovered {len(addresses)} statically referenced native candidate(s)")

        # Generate Frida target probe matching build.sh
        frida_gen = root / "scripts/generate_frida_level_probe.py"
        if not frida_gen.is_file():
            print(f"!! [CMake Native Gate] Missing script: {frida_gen}", file=sys.stderr)
            return 1
        frida_out = build_dir / "frida_level_probe.generated.js"
        generated = subprocess.run([sys.executable, str(frida_gen), "--root", str(root),
                                    "--output", str(frida_out)])
        if generated.returncode != 0:
            return generated.returncode

        gate_args = [
            sys.executable,
            str(root / "scripts/native_gate.py"),
            "--root", str(root),
            "--threshold", threshold,
            "--cfg-threshold", str(args.cfg_threshold),
            "--tree", "engine/src/retail",
            "--report", str(args.report or (build_dir / "native-referenced-verification.json")),
        ]
        for addr in addresses:
            gate_args.extend(["--address", addr])

    # Run the native gate with the current environment
    result = subprocess.run(gate_args, env=os.environ)
    if result.returncode == 0:
        print("==> [CMake Native Gate] PASS: Native verification successfully satisfied.")
    else:
        print(f"!! [CMake Native Gate] BLOCKED: Native verification failed with exit code {result.returncode}",
              file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
