#!/usr/bin/env bash
# Windows x86 build of the diagnostic executable with inlining and interprocedural
# constant propagation disabled.
#
# Why this exists: the shipped candidate is a Release build that inlines the
# retail sources across translation units, so 54 of the 100 linked MVP natives
# keep no call site at all. Their bodies still execute, but a Frida entry hook
# on their address can never fire, so their end-to-end reachability cannot be
# observed in that binary. Disabling inlining does not change what the program
# computes; it only gives each native a real call site to observe.
#
# This build is a verification instrument, not a release. Any evidence captured
# against it must record this binary's digest and state that the only build
# difference is the disabled optimisation.
set -euo pipefail
cd "$(dirname "$0")"
BUILD_DIR=build-win32-verify-noinline
cmake -S . -B "$BUILD_DIR" \
    -DIGI_ENABLE_NATIVE_GATE=OFF \
    -DIGI_FULL_PRODUCTION_GATE=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
    -DCMAKE_CXX_FLAGS="-fno-inline -fno-inline-functions-called-once -fno-inline-small-functions -fno-ipa-cp -fno-ipa-sra" \
    -DBUILD_TESTING=OFF \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tail -5
cmake --build "$BUILD_DIR" -j8 2>&1 | tail -15

CANDIDATE="$BUILD_DIR/igi1.exe"
if [ ! -f "$CANDIDATE" ]; then
  CANDIDATE="$BUILD_DIR/engine/platform/windows/open-igi.exe"
fi
if [ ! -f "$CANDIDATE" ]; then
  echo "No executable was produced in $BUILD_DIR" >&2
  exit 1
fi
ls -lh "$CANDIDATE"
echo "Verification build ready (inlining disabled); not a release."
