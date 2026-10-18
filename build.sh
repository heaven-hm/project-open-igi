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
export RE_AGENT_ROOT="${RE_AGENT_ROOT:-/Users/haseeb-mir/Documents/Code/re-agent}"
export IGI_ANGR_PYTHON="${IGI_ANGR_PYTHON:-$ROOT_DIR/.venv-angr/bin/python}"
export REA_MCP_SERVER="${REA_MCP_SERVER:-rea}"
export IGI_PROBE_BIN="${IGI_PROBE_BIN:-}"
export FRIDA_BIN="${FRIDA_BIN:-$(command -v frida || echo "$HOME/.local/bin/frida")}"
export FRIDA_HOST="${FRIDA_HOST:-127.0.0.1:27042}"
export FRIDA_TARGET="${FRIDA_TARGET:-open-igi.exe}"

REFERENCED_ONLY=1
FULL_PRODUCTION=0
RUN_FRIDA=0
CHECK_FRIDA=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --full-production)
            FULL_PRODUCTION=1
            REFERENCED_ONLY=0
            shift
            ;;
        --frida)
            RUN_FRIDA=1
            shift
            ;;
        --frida-check)
            CHECK_FRIDA=1
            shift
            ;;
        *)
            echo "!! Unknown build option: $1" >&2
            echo "Usage: $0 [--full-production] [--frida] [--frida-check]" >&2
            exit 2
            ;;
    esac
done

toolchain_path_error=0
for required_path in "$IGI_RETAIL_EXE" "$IGI_GAME_NATIVES_DIR" "$BINARY_NINJA_APP" "$GHIDRA_INSTALL_DIR" "$RE_AGENT_ROOT" "$IGI_ANGR_PYTHON"; do
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
echo "    angr Python:    $IGI_ANGR_PYTHON"
echo "    REA MCP:        $REA_MCP_SERVER"
echo "    IGI-Probe:      ${IGI_PROBE_BIN:-not configured}"
echo "    Frida:          $FRIDA_BIN (host: $FRIDA_HOST, target: $FRIDA_TARGET)"

if [[ "$CHECK_FRIDA" -eq 1 ]]; then
    echo "==> Checking Frida remote connection ($FRIDA_HOST -> $FRIDA_TARGET)..."
    if [[ ! -x "$FRIDA_BIN" ]]; then
        echo "!! Frida executable not found at: $FRIDA_BIN" >&2
        exit 1
    fi
    FRIDA_PS_BIN="$(dirname "$FRIDA_BIN")/frida-ps"
    if [[ ! -x "$FRIDA_PS_BIN" ]]; then
        FRIDA_PS_BIN="$(command -v frida-ps || true)"
    fi
    if [[ -n "$FRIDA_PS_BIN" && -x "$FRIDA_PS_BIN" ]]; then
        if ! "$FRIDA_PS_BIN" -H "$FRIDA_HOST" >/dev/null 2>&1; then
            echo "!! Cannot reach Frida server at $FRIDA_HOST. Is frida-server-x86.exe running in Wine/CrossOver?" >&2
            exit 1
        fi
        echo "    frida-server at $FRIDA_HOST is reachable."
        if "$FRIDA_PS_BIN" -H "$FRIDA_HOST" | grep -w "$FRIDA_TARGET" >/dev/null 2>&1; then
            echo "    Target '$FRIDA_TARGET' is active in process list. Verifying attachment..."
            "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET" --eval 'console.log("Attached to " + Process.enumerateModules()[0].name);' -q
            echo "==> Frida connection and attachment verified."
        else
            echo "    Notice: frida-server is active, but '$FRIDA_TARGET' is not currently running."
            echo "    Start '$FRIDA_TARGET' in CrossOver to attach."
        fi
    else
        "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET" --eval 'console.log("Attached to " + Process.enumerateModules()[0].name);' -q
    fi
    exit 0
fi
if [[ "$REFERENCED_ONLY" -eq 1 ]]; then
    echo "==> [0/4] Discovering and verifying statically referenced runtime-path native candidates..."
    REFERENCED_OUTPUT="$(python3 "$ROOT_DIR/scripts/discover_referenced_natives.py" --root "$ROOT_DIR")"
    REFERENCED_ADDRESSES=()
    while IFS= read -r address; do
        [[ -z "$address" ]] || REFERENCED_ADDRESSES+=("$address")
    done <<< "$REFERENCED_OUTPUT"
    if [[ "${#REFERENCED_ADDRESSES[@]}" -eq 0 ]]; then
        echo "!! No runtime-path native candidates were discovered; refusing to build." >&2
        exit 1
    fi
    REFERENCED_GATE_ARGS=(--root "$ROOT_DIR" --tree engine/src/retail
        --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
        --report build/native-referenced-verification.json)
    for address in "${REFERENCED_ADDRESSES[@]}"; do
        REFERENCED_GATE_ARGS+=(--address "$address")
    done
    echo "    discovered ${#REFERENCED_ADDRESSES[@]} statically referenced native candidate(s)"
    python3 "$ROOT_DIR/scripts/native_gate.py" "${REFERENCED_GATE_ARGS[@]}"
    echo "==> [1/4] Configuring and building host engine & test suite (build/)..."
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    echo "==> [2/4] Running CTest suite..."
    ctest --test-dir build --output-on-failure
    if command -v i686-w64-mingw32-g++ >/dev/null 2>&1; then
        echo "==> [3/4] Configuring and building the Windows x86 development executable..."
        cmake -S . -B build-win32 -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DBUILD_TESTING=OFF
        cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
        echo "==> Development executable built; release attestation/deployment remains disabled."
        IGI_DEST_DIR="${IGI_DIR:-/Users/haseeb-mir/Downloads/IGI}"
        if [[ -d "$IGI_DEST_DIR" && -f build-win32/engine/platform/windows/open-igi.exe ]]; then
            echo "==> [Dev Sync] Copying open-igi.exe to $IGI_DEST_DIR/open-igi.exe..."
            cp -f build-win32/engine/platform/windows/open-igi.exe "$IGI_DEST_DIR/open-igi.exe"
        fi
    fi
    if [[ "$RUN_FRIDA" -eq 1 ]]; then
        echo "==> Attaching Frida: $FRIDA_BIN -H $FRIDA_HOST -n $FRIDA_TARGET..."
        "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET"
    fi
    exit 0
fi
echo "==> [0/4] Verifying all production native transcriptions against retail igi.exe..."
python3 "$ROOT_DIR/scripts/build_attestation.py" capture --root "$ROOT_DIR" \
    --output "$ROOT_DIR/build/native-verification-inputs.json"
python3 "$ROOT_DIR/scripts/native_gate.py" --root "$ROOT_DIR" --production-tree \
    --threshold VERIFIED --cfg-threshold 0.85 --report build/native-verification.json || {
  echo "!! Build BLOCKED: production native code lacks matching binary-bound evidence."
  echo "!! A five-line comment alone cannot authorize compilation or deployment."
  exit 1
}

echo "==> [1/4] Configuring and building host engine & test suite (build/)..."
python3 "$ROOT_DIR/scripts/build_attestation.py" snapshot --root "$ROOT_DIR" \
    --input-snapshot "$ROOT_DIR/build/native-verification-inputs.json" \
    --gate-report "$ROOT_DIR/build/native-verification.json" \
    --output "$ROOT_DIR/build/native-build-snapshot.json"
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
    python3 "$ROOT_DIR/scripts/build_attestation.py" configure --root "$ROOT_DIR" \
        --snapshot "$ROOT_DIR/build/native-build-snapshot.json" \
        --output "$ROOT_DIR/build/native-build-snapshot.json"
    cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    python3 "$ROOT_DIR/scripts/build_attestation.py" seal --root "$ROOT_DIR" \
        --snapshot "$ROOT_DIR/build/native-build-snapshot.json" \
        --binary "$ROOT_DIR/build-win32/engine/platform/windows/open-igi.exe" \
        --output "$ROOT_DIR/build-win32/engine/platform/windows/open-igi.exe.native-verification.json"

    IGI_DEST_DIR="${IGI_DIR:-/Users/haseeb-mir/Downloads/IGI}"
    if [[ -d "$IGI_DEST_DIR" ]]; then
        echo "==> [4/4] Copying open-igi.exe to $IGI_DEST_DIR/open-igi.exe..."
        cp -f build-win32/engine/platform/windows/open-igi.exe "$IGI_DEST_DIR/open-igi.exe"
        cp -f build-win32/engine/platform/windows/open-igi.exe.native-verification.json \
            "$IGI_DEST_DIR/open-igi.exe.native-verification.json"
        ls -lh "$IGI_DEST_DIR/open-igi.exe"
    else
        echo "==> [4/4] Skipping copy: $IGI_DEST_DIR does not exist."
    fi
else
    echo "==> [3/4] i686-w64-mingw32-g++ not found; skipping 32-bit Windows build."
fi

echo "==> Build and verification complete."

if [[ "$RUN_FRIDA" -eq 1 ]]; then
    echo "==> Attaching Frida: $FRIDA_BIN -H $FRIDA_HOST -n $FRIDA_TARGET..."
    "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET"
fi
