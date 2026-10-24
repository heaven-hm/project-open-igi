#!/usr/bin/env bash
# Ungated Windows x86 build of the diagnostic executable, copied to ~/Downloads/IGI.
# Diagnostic only; keep the normal build cache and release gate independent.
set -euo pipefail
cd "$(dirname "$0")"
DEST="$HOME/Downloads/IGI"
BUILD_DIR=build-win32-no-verify-gate
cmake -S . -B "$BUILD_DIR" -DIGI_TESTING_MUTEX=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DBUILD_TESTING=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tail -5
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
