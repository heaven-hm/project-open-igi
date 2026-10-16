#!/usr/bin/env bash
set -euo pipefail

# Run Project I.G.I. — launches the app this repository builds.
#
# Usage:
#   ./run.sh                                  # launch with defaults
#   ./run.sh --mission 2 --graphics high      # pass engine arguments
#   ./run.sh --build                          # build first (no deploy)
#   ./run.sh --direct                         # run the binary directly
#   ./run.sh --status                         # print build/app state only

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RETAIL_DIR="${IGI_GAME_PATH:-/Users/haseeb-mir/Downloads/IGI}"
BUILD_APP="${SCRIPT_DIR}/build/engine/igi.app"
BUILD_BIN="${BUILD_APP}/Contents/MacOS/igi"
RETAIL_APP="${RETAIL_DIR}/igi.app"

MODE="retail"
args=()
for argument in "$@"; do
    case "${argument}" in
        --build)  MODE="build" ;;
        --direct) MODE="direct" ;;
        --status) MODE="status" ;;
        *)        args+=("${argument}") ;;
    esac
done

if [ "${MODE}" = "build" ]; then
    exec "${SCRIPT_DIR}/build.sh" --no-deploy
fi

if [ "${MODE}" = "status" ]; then
    echo "Branch:      $(git -C "${SCRIPT_DIR}" branch --show-current)"
    echo "Retail dir:  ${RETAIL_DIR}"
    if [ -d "${RETAIL_APP}" ]; then
        echo "Deployed:    ${RETAIL_APP} ($(stat -f '%Sm' "${RETAIL_APP}"))"
        codesign --verify --deep --strict "${RETAIL_APP}" 2>&1 && \
            echo "Signature:   valid" || echo "Signature:   invalid"
    else
        echo "Deployed:    none"
    fi
    if [ -x "${BUILD_BIN}" ]; then
        echo "Built:       ${BUILD_BIN} ($(stat -f '%Sm' "${BUILD_BIN}"))"
    else
        echo "Built:       none (run ./build.sh)"
    fi
    exit 0
fi

if [ "${MODE}" = "direct" ]; then
    if [ ! -x "${BUILD_BIN}" ]; then
        echo "[ERROR] Not built: ${BUILD_BIN}. Run ./build.sh first." >&2
        exit 1
    fi
    cd "${RETAIL_DIR}"
    exec "${BUILD_BIN}" --game "${RETAIL_DIR}" "${args[@]+"${args[@]}"}"
fi

if [ -d "${RETAIL_APP}" ]; then
    echo "Launching ${RETAIL_APP} ${args[*]+${args[*]}}"
    exec open -n "${RETAIL_APP}" --args "${args[@]+"${args[@]}"}"
fi

if [ -x "${BUILD_BIN}" ]; then
    echo "[WARNING] ${RETAIL_APP} not deployed; running build output directly."
    cd "${RETAIL_DIR}"
    exec "${BUILD_BIN}" --game "${RETAIL_DIR}" "${args[@]+"${args[@]}"}"
fi

echo "[ERROR] No app found. Run ./build.sh first." >&2
exit 1
