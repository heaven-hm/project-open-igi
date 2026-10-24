#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${IGI_CLEAN_BUILD_DIR:-$ROOT_DIR/build-win32-no-verify-gate}"
if [[ "${1:-}" != "--diagnostic" ]]; then
    exec python3 "$ROOT_DIR/scripts/accept_changed_feature.py" "$@"
fi
shift
if [[ "$#" -ne 0 ]]; then
    echo "Usage: ./build.sh [--probe path --level N] | --diagnostic" >&2
    exit 2
fi
CXX="${IGI_WIN32_CXX:-i686-w64-mingw32-g++}"
if ! command -v "$CXX" >/dev/null 2>&1; then
    echo "Windows x86 compiler not found: $CXX" >&2
    exit 1
fi
echo "==> Building Windows x86 igi1.exe diagnostic shell (no retail natives verified)."
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR"
echo "==> Diagnostic placeholder only; no game, parity claim, acceptance, or deployment."
