#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

# Retail analysis toolchain.  These defaults identify the exact local tools
# used by the native gate; callers may override a path through the matching
# environment variable without changing the checked-in build procedure.
export IGI_RETAIL_EXE="${IGI_RETAIL_EXE:-/Users/haseeb-mir/Downloads/IGI/igi.exe}"
export IGI_GAME_NATIVES_DIR="${IGI_GAME_NATIVES_DIR:-$ROOT_DIR/scripts/GameNatives}"
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

REFERENCED_ONLY=0
RUN_FRIDA=0
CHECK_FRIDA=0
RUNTIME_TRACE=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --full-production)
            REFERENCED_ONLY=0
            shift
            ;;
        --development)
            REFERENCED_ONLY=1
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
        --runtime-trace)
            if [[ $# -lt 2 ]]; then
                echo "!! --runtime-trace requires a complete IGI-Probe JSON trace path." >&2
                exit 2
            fi
            RUNTIME_TRACE="$2"
            REFERENCED_ONLY=1
            shift 2
            ;;
        *)
            echo "!! Unknown build option: $1" >&2
            echo "Usage: $0 [--development] [--runtime-trace TRACE.json] [--full-production] [--frida] [--frida-check]" >&2
            exit 2
            ;;
    esac
done

if [[ -n "$RUNTIME_TRACE" && "$REFERENCED_ONLY" -ne 1 ]]; then
    echo "!! --runtime-trace cannot be combined with --full-production." >&2
    exit 2
fi

BN_MCP_AVAILABLE=0
if python3 -c 'from urllib.parse import urlparse; import socket,sys; u=urlparse(sys.argv[1]); s=socket.create_connection((u.hostname, u.port or (443 if u.scheme == "https" else 80)), timeout=1); s.close()' "$BINARY_NINJA_MCP_URL" >/dev/null 2>&1; then
    BN_MCP_AVAILABLE=1
fi

toolchain_path_error=0
for required_path in "$IGI_RETAIL_EXE" "$IGI_GAME_NATIVES_DIR" "$RE_AGENT_ROOT" "$IGI_ANGR_PYTHON"; do
    if [[ ! -e "$required_path" ]]; then
        echo "!! Missing required native-analysis path: $required_path" >&2
        toolchain_path_error=1
    fi
done
if [[ "$BN_MCP_AVAILABLE" -eq 0 && ! -e "$GHIDRA_INSTALL_DIR" ]]; then
    echo "!! Binary Ninja MCP is unavailable and Ghidra fallback is missing: $GHIDRA_INSTALL_DIR" >&2
    toolchain_path_error=1
fi
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
echo "    Decompiler policy: Binary Ninja evidence preferred per native; Ghidra fallback only when unavailable"
if [[ "$BN_MCP_AVAILABLE" -eq 1 ]]; then
    echo "    Binary Ninja MCP: reachable from build shell ($BINARY_NINJA_MCP_URL)"
else
    echo "    Binary Ninja MCP: not reachable from build shell ($BINARY_NINJA_MCP_URL)"
fi
echo "    Ghidra fallback: $GHIDRA_MCP_URL ($GHIDRA_INSTALL_DIR)"
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
    REFERENCED_GATE_ARGS=(--root "$ROOT_DIR"
        --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
        --report build/native-referenced-verification.json)
    if [[ -n "$RUNTIME_TRACE" ]]; then
        echo "==> [0/4] Verifying complete IGI-Probe observed runtime native candidates..."
        REFERENCED_GATE_ARGS+=(--runtime-trace "$RUNTIME_TRACE")
    else
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
        REFERENCED_GATE_ARGS+=(--tree engine/src/retail)
        for address in "${REFERENCED_ADDRESSES[@]}"; do
            REFERENCED_GATE_ARGS+=(--address "$address")
        done
        echo "    discovered ${#REFERENCED_ADDRESSES[@]} statically referenced native candidate(s)"
        python3 "$ROOT_DIR/scripts/generate_frida_level_probe.py" --root "$ROOT_DIR" \
            --output "$ROOT_DIR/build/frida_level_probe.generated.js"
    fi
    python3 "$ROOT_DIR/scripts/native_gate.py" "${REFERENCED_GATE_ARGS[@]}"
    mkdir -p "$ROOT_DIR/build" "$ROOT_DIR/build-win32"
    touch "$ROOT_DIR/build/igi_native_verification.stamp" "$ROOT_DIR/build-win32/igi_native_verification.stamp"
    echo "==> [1/4] Configuring and building host engine & test suite (build/)..."
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    echo "==> [2/4] Running CTest suite, including terrain-mask and player-motion regressions..."
    ctest --test-dir build --output-on-failure
    if command -v i686-w64-mingw32-g++ >/dev/null 2>&1; then
        echo "==> [3/4] Configuring and building the Windows x86 development executable..."
        cmake -S . -B build-win32 -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DBUILD_TESTING=OFF
        cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
        echo "==> Development executable built; release attestation/deployment remains disabled."
        echo "    Development builds are not copied to the game directory; use the full-production gate for release deployment."
    fi
    if [[ "$RUN_FRIDA" -eq 1 ]]; then
        if [[ "$FRIDA_TARGET" == "igi.exe" ]]; then
            FRIDA_PROBE="$ROOT_DIR/build/frida_level_probe.generated.js"
            python3 "$ROOT_DIR/scripts/generate_frida_level_probe.py" --root "$ROOT_DIR" --output "$FRIDA_PROBE"
            echo "==> Attaching retail Frida probe: $FRIDA_BIN -H $FRIDA_HOST -n $FRIDA_TARGET -l $FRIDA_PROBE..."
            "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET" -l "$FRIDA_PROBE"
        else
            echo "==> Attaching Frida to $FRIDA_TARGET. Retail-VA entry hooks are not applied to OpenIGI."
            "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET"
        fi
    fi
    exit 0
fi
echo "==> [0/4] Verifying changed natives: static CFG, source semantics and behavioral evidence..."
python3 "$ROOT_DIR/scripts/build_attestation.py" capture --root "$ROOT_DIR" \
    --output "$ROOT_DIR/build/native-verification-inputs.json"
python3 "$ROOT_DIR/scripts/native_gate.py" --root "$ROOT_DIR" --changed \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85 --require-semantic-evidence \
    --report build/native-verification.json || {
  echo "!! Build BLOCKED: production native code lacks matching binary-bound evidence."
  echo "!! A five-line comment alone cannot authorize compilation or deployment."
  exit 1
}

mkdir -p "$ROOT_DIR/build" "$ROOT_DIR/build-win32"
touch "$ROOT_DIR/build/igi_native_verification.stamp" "$ROOT_DIR/build-win32/igi_native_verification.stamp"

echo "==> [1/4] Configuring and building host engine & test suite (build/)..."
python3 "$ROOT_DIR/scripts/build_attestation.py" snapshot --root "$ROOT_DIR" \
    --input-snapshot "$ROOT_DIR/build/native-verification-inputs.json" \
    --gate-report "$ROOT_DIR/build/native-verification.json" \
    --output "$ROOT_DIR/build/native-build-snapshot.json"
cmake -S . -B build -DIGI_FULL_PRODUCTION_GATE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

if [[ -f build/compile_commands.json ]]; then
    cp -f build/compile_commands.json ./compile_commands.json
fi

echo "==> [2/4] Running CTest suite, including terrain-mask and player-motion regressions..."
ctest --test-dir build --output-on-failure

if command -v i686-w64-mingw32-g++ >/dev/null 2>&1; then
    echo "==> [3/4] Configuring and building 32-bit Windows executable (build-win32/)..."
    cmake -S . -B build-win32 -DIGI_FULL_PRODUCTION_GATE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
        -DBUILD_TESTING=OFF
    python3 "$ROOT_DIR/scripts/build_attestation.py" configure --root "$ROOT_DIR" \
        --snapshot "$ROOT_DIR/build/native-build-snapshot.json" \
        --output "$ROOT_DIR/build/native-build-snapshot.json"
    cmake --build build-win32 -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    # Headless runtime verification: one small Frida probe per native, against the
    # retail game and the newly compiled candidate, before sealing or deployment.
    if [[ "${IGI_SKIP_RUNTIME_VERIFICATION:-0}" == "1" ]]; then
      echo "!! Build BLOCKED: IGI_SKIP_RUNTIME_VERIFICATION cannot bypass the Frida acceptance gate." >&2
      exit 1
    fi
    echo "==> Headless runtime verification (Frida, retail igi.exe and open-igi.exe)..."
    python3 "$ROOT_DIR/scripts/native_frida_diff.py" \
        --symbols "$ROOT_DIR/assets/candidate_symbol_map.json" \
        --gate-report "$ROOT_DIR/build/native-verification.json" \
        --candidate-exe "$ROOT_DIR/build-win32/engine/platform/windows/open-igi.exe" \
        --probe "${IGI_NATIVE_PROBE:-$ROOT_DIR/scripts/frida_native_diff.js}" \
        --level "${IGI_VERIFY_LEVEL:-4}" \
        --observe "${IGI_VERIFY_OBSERVE:-6}" \
        --out "$ROOT_DIR/build/native-frida-differential.json" || {
      echo "!! Build BLOCKED: the headless runtime verification did not complete." >&2
      exit 1
    }
    python3 "$ROOT_DIR/scripts/check_frida_differential.py" \
        --report "$ROOT_DIR/build/native-frida-differential.json" \
        --gate-report "$ROOT_DIR/build/native-verification.json" \
        --candidate-exe "$ROOT_DIR/build-win32/engine/platform/windows/open-igi.exe" || {
      echo "!! Build BLOCKED: runtime parity is incomplete or unproven." >&2
      exit 1
    }
    python3 "$ROOT_DIR/scripts/build_attestation.py" seal --root "$ROOT_DIR" \
        --snapshot "$ROOT_DIR/build/native-build-snapshot.json" \
        --binary "$ROOT_DIR/build-win32/engine/platform/windows/open-igi.exe" \
        --runtime-report "$ROOT_DIR/build/native-frida-differential.json" \
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
    echo "!! Build BLOCKED: Windows x86 compiler is required for candidate runtime verification." >&2
    exit 1
fi

echo "==> Build complete for the verified changed-native scope; no whole-game equivalence claim."

if [[ "$RUN_FRIDA" -eq 1 ]]; then
    echo "==> Attaching Frida: $FRIDA_BIN -H $FRIDA_HOST -n $FRIDA_TARGET..."
    "$FRIDA_BIN" -H "$FRIDA_HOST" -n "$FRIDA_TARGET"
fi
