#!/usr/bin/env bash
# Ungated Windows x86 build of the diagnostic executable, copied to ~/Downloads/IGI.
# Diagnostic only; keep the normal build cache and release gate independent.
# STRICT: agent-driven native increments must not use this ungated build. Use
# ./build.sh (strict changed-native gate + paired runtime acceptance) or
# ./build.sh --diagnostic (diagnostic shell only). This script requires explicit
# owner opt-in via IGI_ALLOW_UNGATED_BUILD=1 and is refused by default.
set -euo pipefail
if [[ "${IGI_ALLOW_UNGATED_BUILD:-0}" != "1" ]]; then
  echo "build_no_verify.sh: BLOCKED (ungated builds are disabled by default). Use ./build.sh or ./build.sh --diagnostic." >&2
  exit 1
fi
cd "$(dirname "$0")"
DEST="$HOME/Downloads/IGI"
BUILD_DIR=build-win32-no-verify-gate
cmake -S . -B "$BUILD_DIR" -DIGI_ENABLE_NATIVE_GATE=OFF -DIGI_FULL_PRODUCTION_GATE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DBUILD_TESTING=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tail -5
cmake --build "$BUILD_DIR" -j8 2>&1 | tail -15

# The CMake target is "igi" with OUTPUT_NAME "igi1", so the executable lands in
# the build root. Older layouts put it under the platform directory.
CANDIDATE="$BUILD_DIR/igi1.exe"
if [ ! -f "$CANDIDATE" ]; then
  CANDIDATE="$BUILD_DIR/engine/platform/windows/open-igi.exe"
fi
if [ ! -f "$CANDIDATE" ]; then
  echo "No executable was produced in $BUILD_DIR" >&2
  exit 1
fi
ls -lh "$CANDIDATE"

DEPLOYED="$DEST/$(basename "$CANDIDATE")"
if [ -f "$DEPLOYED" ]; then
  cp -f "$DEPLOYED" "$DEPLOYED.prev-before-ungated-build"
fi
cp -f "$CANDIDATE" "$DEPLOYED"
ls -lh "$DEPLOYED"
echo "Diagnostic build copied; this is not a verified release."
