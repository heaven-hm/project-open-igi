#!/usr/bin/env bash
# Terrain worktree build: Windows 32-bit Release only, no CFG/native verification.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

# Remote Frida Configuration for CrossOver/Wine
export FRIDA_HOST="${FRIDA_HOST:-127.0.0.1:27042}"
export FRIDA_TARGET="${FRIDA_TARGET:-open-igi.exe}"

# Handle --frida-check flag
if [[ "${1:-}" == "--frida-check" ]]; then
    echo "==> Checking remote Frida agent on $FRIDA_HOST..."
    if command -v frida-ps >/dev/null 2>&1; then
        frida-ps -H "$FRIDA_HOST" || {
            echo "!! Failed to connect to frida-server on $FRIDA_HOST"
            exit 1
        }
    else
        echo "frida-ps not installed on host machine."
    fi
    exit 0
fi

# Native analysis toolchain (informational only — gate is disabled in this worktree).
export IGI_RETAIL_EXE="${IGI_RETAIL_EXE:-/Users/haseeb-mir/Downloads/IGI/igi.exe}"
export IGI_GAME_NATIVES_DIR="${IGI_GAME_NATIVES_DIR:-$ROOT_DIR/scripts/GameNatives}"
export BINARY_NINJA_APP="${BINARY_NINJA_APP:-/Applications/Binary Ninja.app}"
export GHIDRA_INSTALL_DIR="${GHIDRA_INSTALL_DIR:-/opt/homebrew/opt/ghidra/libexec}"
export RE_AGENT_ROOT="${RE_AGENT_ROOT:-/Users/haseeb-mir/Documents/Code/re-agent}"
export IGI_ANGR_PYTHON="${IGI_ANGR_PYTHON:-$ROOT_DIR/.venv-angr/bin/python}"

echo "==> Terrain worktree: Windows 32-bit Release build (verification OFF)"
echo "    retail igi.exe:   $IGI_RETAIL_EXE"
echo "    GameNatives:      $IGI_GAME_NATIVES_DIR"
echo "    RE-Agent:         $RE_AGENT_ROOT"

echo "==> [1/3] Configuring 32-bit Windows Release (build-win32/)..."
cmake -S . -B build-win32 \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF

echo "==> [2/3] Building open-igi.exe..."
cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

IGI_DEST_DIR="${IGI_DIR:-/Users/haseeb-mir/Downloads/IGI}"
echo "==> [3/3] Copying open-igi.exe to $IGI_DEST_DIR/open-igi.exe..."
cp -f build-win32/engine/platform/windows/open-igi.exe "$IGI_DEST_DIR/open-igi.exe"
ls -lh "$IGI_DEST_DIR/open-igi.exe"

echo "==> Done. open-igi.exe deployed to $IGI_DEST_DIR"

if [[ "${1:-}" == "--frida" ]]; then
    echo "==> Attaching Frida CLI to $FRIDA_TARGET at $FRIDA_HOST..."
    exec frida -H "$FRIDA_HOST" -n "$FRIDA_TARGET"
fi
