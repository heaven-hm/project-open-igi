#!/usr/bin/env python3
"""Refresh the manually testable igi1.exe in the game folder from the latest build.

The game folder copy is what a human launches (`igi.exe window levelN` for retail,
`igi1.exe window levelN` for the port). Two candidate builds exist and they are not
interchangeable:

  * build-win32-no-verify-gate/igi1.exe  - the ungated gameplay build. This is the
    binary the capture scripts default to and the only one suitable for manual
    testing; it loads retail lightmaps (log line "lightmaps=1").
  * build-win32-verify-noinline/igi1.exe - the verification build used for native
    gate work. It runs with lightmaps disabled ("lightmaps=0"), so the window shows
    unlit/black terrain and looks broken even though the process runs.

Copying the verification build into the game folder therefore produces a black
screen. This script always installs the ungated build, keeps the previous binary as
igi1.exe.prev-<sha8>, and refuses to run while a game session is live (the retail
mutex allows only one instance).
"""

from __future__ import annotations

import argparse
import hashlib
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import native_frida_diff as nfd  # noqa: E402  (repo helper for GAME_DIR)

DEFAULT_SOURCE = ROOT / "build-win32-no-verify-gate" / "igi1.exe"
GAME_BINARY = "igi1.exe"
LIVE_PROCESSES = ("igi.exe", "igi1.exe", "open-igi.exe")


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def live_sessions() -> list[str]:
    running: list[str] = []
    for name in LIVE_PROCESSES:
        result = subprocess.run(["pgrep", "-f", name], capture_output=True, text=True)
        if result.returncode == 0 and result.stdout.strip():
            running.append(name)
    return running


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE,
                        help="candidate build to install (default: the ungated build)")
    parser.add_argument("--game-dir", type=Path, default=Path(nfd.vnb.GAME_DIR),
                        help="folder holding the manually launched binaries")
    parser.add_argument("--force", action="store_true",
                        help="install even if a game session appears to be running")
    args = parser.parse_args()

    source = args.source.resolve()
    if not source.is_file():
        print(f"refresh: missing build {source}", file=sys.stderr)
        return 2
    if any("verify-noinline" in part for part in source.parts):
        print("refresh: refusing to install the verification build; it renders with "
              "lightmaps disabled and shows a black window. Use the ungated build.",
              file=sys.stderr)
        return 2

    running = live_sessions()
    if running and not args.force:
        print(f"refresh: game session already running ({', '.join(sorted(set(running)))}); "
              "close it first (the retail mutex allows one instance)", file=sys.stderr)
        return 3

    target = args.game_dir / GAME_BINARY
    if target.is_file():
        current = digest(target)
        if current == digest(source):
            print(f"refresh: {target} already matches {source.name} "
                  f"({current[:16]})")
            return 0
        backup = target.with_name(f"{GAME_BINARY}.prev-{current[:8]}")
        shutil.copy2(target, backup)
        print(f"refresh: kept previous binary as {backup.name}")

    shutil.copy2(source, target)
    installed = digest(target)
    expected = digest(source)
    if installed != expected:
        print("refresh: copy verification failed", file=sys.stderr)
        return 4
    print(f"refresh: installed {source} -> {target} sha256={installed}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
