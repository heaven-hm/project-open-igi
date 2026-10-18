#!/usr/bin/env bash
set -euo pipefail

# Launch the Windows x86 DirectDraw/Direct3D executable after retail verification.
# --status is read-only; --build runs the enforced build; --direct selects build output.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RETAIL_DIR="${IGI_GAME_PATH:-${IGI_DIR:-/Users/haseeb-mir/Downloads/IGI}}"
RETAIL_DIR="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$RETAIL_DIR")"
BUILD_BIN="$SCRIPT_DIR/build-win32/engine/platform/windows/open-igi.exe"
DEPLOYED_BIN="$RETAIL_DIR/open-igi.exe"
MODE="deployed"
args=()
for argument in "$@"; do
    case "$argument" in
        --build) MODE="build" ;;
        --direct) MODE="direct" ;;
        --status) MODE="status" ;;
        *) args+=("$argument") ;;
    esac
done

if [[ "$MODE" == "status" ]]; then
    echo "Branch: $(git -C "$SCRIPT_DIR" branch --show-current)"
    echo "Game directory: $RETAIL_DIR"
    for binary in "$BUILD_BIN" "$DEPLOYED_BIN"; do
        if [[ -f "$binary" ]]; then
            echo "Present: $binary"
        else
            echo "Missing: $binary"
        fi
    done
    echo "Launch requires the full production VERIFIED gate (CFG >= 0.85)."
    exit 0
fi

if [[ "$MODE" == "build" ]]; then
    exec "$SCRIPT_DIR/build.sh"
fi

python3 "$SCRIPT_DIR/scripts/native_gate.py" --root "$SCRIPT_DIR" \
    --binary "${IGI_RETAIL_EXE:-$RETAIL_DIR/igi.exe}" \
    --production-tree --threshold VERIFIED --cfg-threshold 0.85 \
    --report build/native-launch-verification.json || {
        echo "Launch BLOCKED: production native verification failed." >&2
        exit 1
    }

binary="$DEPLOYED_BIN"
if [[ "$MODE" == "direct" || ! -f "$binary" ]]; then
    binary="$BUILD_BIN"
fi
if [[ ! -f "$binary" ]]; then
    echo "Missing Windows executable: $binary. Run ./build.sh first." >&2
    exit 1
fi
python3 "$SCRIPT_DIR/scripts/build_attestation.py" check --root "$SCRIPT_DIR" \
    --gate-report "$SCRIPT_DIR/build/native-launch-verification.json" \
    --binary "$binary" --receipt "$binary.native-verification.json"
cd "$RETAIL_DIR"
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) exec "$binary" "${args[@]+"${args[@]}"}" ;;
esac

wine_bin="${IGI_WINE_BIN:-$(command -v wine || true)}"
if [[ -z "$wine_bin" || ! -x "$wine_bin" ]]; then
    echo "Set IGI_WINE_BIN to the Wine/CrossOver launcher for Windows x86." >&2
    exit 1
fi
if [[ -n "${IGI_CX_BOTTLE:-}" ]]; then
    exec "$wine_bin" --bottle "$IGI_CX_BOTTLE" "$binary" "${args[@]+"${args[@]}"}"
fi
exec "$wine_bin" "$binary" "${args[@]+"${args[@]}"}"
