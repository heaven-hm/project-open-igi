#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

# Retail analysis toolchain.  These defaults identify the exact local tools
# used by the native gate; callers may override a path through the matching
# environment variable without changing the checked-in build procedure.
export IGI_RETAIL_EXE="${IGI_RETAIL_EXE:-/Users/haseeb-mir/Downloads/IGI/igi.exe}"
export IGI_GAME_NATIVES_DIR="${IGI_GAME_NATIVES_DIR:-$ROOT_DIR/scripts/GameNatives}"
export BINARY_NINJA_APP="${BINARY_NINJA_APP:-/Applications/Binary Ninja.app}"
export BINARY_NINJA_MCP_URL="${BINARY_NINJA_MCP_URL:-http://127.0.0.1:24642/mcp}"
export GHIDRA_INSTALL_DIR="${GHIDRA_INSTALL_DIR:-/opt/homebrew/opt/ghidra/libexec}"
export GHIDRA_MCP_URL="${GHIDRA_MCP_URL:-http://127.0.0.1:8771}"
export RADARE2_BIN="${RADARE2_BIN:-$(command -v r2 || true)}"
export RE_AGENT_ROOT="${RE_AGENT_ROOT:-/Users/haseeb-mir/Documents/Code/auto-re-agent}"
export REA_MCP_SERVER="${REA_MCP_SERVER:-rea}"
export IGI_PROBE_BIN="${IGI_PROBE_BIN:-}"

toolchain_path_error=0
for required_path in "$IGI_RETAIL_EXE" "$IGI_GAME_NATIVES_DIR" "$BINARY_NINJA_APP" "$GHIDRA_INSTALL_DIR" "$RE_AGENT_ROOT"; do
    if [[ ! -e "$required_path" ]]; then
        echo "!! Missing required native-analysis path: $required_path" >&2
        toolchain_path_error=1
    fi
done
if [[ -z "$RADARE2_BIN" || ! -x "$RADARE2_BIN" ]]; then
    echo "!! Radare2 executable is unavailable: ${RADARE2_BIN:-not found}" >&2
    toolchain_path_error=1
fi
if [[ "$toolchain_path_error" -ne 0 ]]; then
    exit 1
fi

echo "==> Native analysis toolchain"
echo "    retail igi.exe: $IGI_RETAIL_EXE"
echo "    GameNatives:    $IGI_GAME_NATIVES_DIR"
echo "    Binary Ninja:   $BINARY_NINJA_APP"
echo "    Binary Ninja MCP: $BINARY_NINJA_MCP_URL"
echo "    Ghidra:         $GHIDRA_INSTALL_DIR"
echo "    Ghidra MCP:     $GHIDRA_MCP_URL"
echo "    Radare2:        $RADARE2_BIN"
echo "    RE-Agent:       $RE_AGENT_ROOT"
echo "    REA MCP:        $REA_MCP_SERVER"
echo "    IGI-Probe:      ${IGI_PROBE_BIN:-not configured}"

echo "==> [0/4] Verifying all production native transcriptions against retail igi.exe..."
python3 "$ROOT_DIR/scripts/native_gate.py" --root "$ROOT_DIR" --production-tree \
    --threshold VERIFIED --cfg-threshold 0.85 || {
  echo "!! Build BLOCKED: production native code lacks matching binary-bound evidence."
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
