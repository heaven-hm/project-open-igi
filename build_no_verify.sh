#!/usr/bin/env bash
# Ungated Windows x86 build of open-igi.exe, copied to ~/Downloads/IGI.
# Diagnostic only; keep the normal build cache and release gate independent.
set -euo pipefail
cd "$(dirname "$0")"
DEST="$HOME/Downloads/IGI"
BUILD_DIR=build-win32-no-verify-gate
cmake -S . -B "$BUILD_DIR" -DIGI_ENABLE_NATIVE_GATE=OFF -DIGI_FULL_PRODUCTION_GATE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DBUILD_TESTING=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tail -5
cmake --build "$BUILD_DIR" -j8 2>&1 | tail -15
ls -lh "$BUILD_DIR/engine/platform/windows/open-igi.exe"
if [ -f "$DEST/open-igi.exe" ]; then
  cp -f "$DEST/open-igi.exe" "$DEST/open-igi.exe.prev-before-ungated-build"
fi
cp -f "$BUILD_DIR/engine/platform/windows/open-igi.exe" "$DEST/open-igi.exe"
ls -lh "$DEST/open-igi.exe"
echo "Diagnostic build copied; this is not a verified release."
