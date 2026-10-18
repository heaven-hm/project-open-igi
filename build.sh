#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "==> [0/4] Verifying changed native transcriptions against retail igi.exe..."
python3 "$ROOT_DIR/scripts/native_gate.py" --root "$ROOT_DIR" --changed \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85 || {
  echo "!! Build BLOCKED: changed native code lacks matching binary-bound evidence."
  echo "!! A five-line comment alone cannot authorize compilation or deployment."
  exit 1
}

echo "==> [1/4] Configuring and building host engine & test suite (build/)..."
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

if [[ -f build/compile_commands.json ]]; then
    cp -f build/compile_commands.json ./compile_commands.json
fi

echo "==> [2/4] Running CTest suite..."
ctest --test-dir build --output-on-failure

if command -v i686-w64-mingw32-g++ >/dev/null 2>&1; then
    echo "==> [3/4] Configuring and building 32-bit Windows executable (build-win32/)..."
    cmake -S . -B build-win32 \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
        -DBUILD_TESTING=OFF
    cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

    IGI_DEST_DIR="${IGI_DIR:-/Users/haseeb-mir/Downloads/IGI}"
    if [[ -d "$IGI_DEST_DIR" ]]; then
        echo "==> [4/4] Copying open-igi.exe to $IGI_DEST_DIR/open-igi.exe..."
        cp -f build-win32/engine/platform/windows/open-igi.exe "$IGI_DEST_DIR/open-igi.exe"
        ls -lh "$IGI_DEST_DIR/open-igi.exe"
    else
        echo "==> [4/4] Skipping copy: $IGI_DEST_DIR does not exist."
    fi
else
    echo "==> [3/4] i686-w64-mingw32-g++ not found; skipping 32-bit Windows build."
fi

echo "==> Build and verification complete."
