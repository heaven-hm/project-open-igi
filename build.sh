#!/usr/bin/env bash
set -euo pipefail

# Project I.G.I. Native Port — build, test and deploy igi.app
#
# Source of truth: this repository, branch feature/gameplay-prototype.
# The engine under engine/ is the native IGI engine (retail MEF/TEX/QVM/GRAPH
# loading, skeletal scene rendering, AI/navigation, collision, gameplay).
#
# Usage:
#   ./build.sh                 build + test + deploy to ~/Downloads/IGI/igi.app
#   ./build.sh --no-deploy     build + test only

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
RETAIL_DIR="${IGI_GAME_PATH:-/Users/haseeb-mir/Downloads/IGI}"

DEPLOY=1
for argument in "$@"; do
    case "${argument}" in
        --no-deploy) DEPLOY=0 ;;
        *) echo "Unknown option: ${argument}" >&2; exit 2 ;;
    esac
done

echo "========================================================"
echo " Project Open I.G.I. — Build"
echo " Source: ${SCRIPT_DIR}"
echo " Build:  ${BUILD_DIR}"
echo " Retail: ${RETAIL_DIR}"
echo "========================================================"

# 1. Configure (Release)
if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "[1/4] Configuring CMake (Release)..."
    cmake -B "${BUILD_DIR}" -S "${SCRIPT_DIR}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DIGI_GAME_PATH="${RETAIL_DIR}"
else
    echo "[1/4] Reusing existing CMake configuration in ${BUILD_DIR}"
    cmake -B "${BUILD_DIR}" -S "${SCRIPT_DIR}" -DIGI_GAME_PATH="${RETAIL_DIR}"
fi

# 2. Build everything (engine, probes, app bundle)
echo "[2/4] Building..."
cmake --build "${BUILD_DIR}" -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 8)"

APP="${BUILD_DIR}/engine/igi.app"
if [ ! -d "${APP}" ]; then
    echo "[ERROR] Expected app bundle not found: ${APP}" >&2
    exit 1
fi

# 3. Functional test suite (retail parity gate is separate: see engine/VERIFICATION.md)
echo "[3/4] Running test suite (ctest, excluding retail parity gate)..."
if ! ctest --test-dir "${BUILD_DIR}" -LE retail --output-on-failure; then
    echo "[ERROR] Test suite failed." >&2
    exit 1
fi

# 4. Deploy to the retail install
if [ "${DEPLOY}" -eq 1 ] && [ -d "${RETAIL_DIR}" ]; then
    echo "[4/4] Deploying igi.app to ${RETAIL_DIR}..."
    rm -rf "${RETAIL_DIR}/igi.app"
    cp -R "${APP}" "${RETAIL_DIR}/igi.app"
    xattr -cr "${RETAIL_DIR}/igi.app" 2>/dev/null || true
    # Ad-hoc sign the copied bundle so macOS launches it without Gatekeeper
    # complaints about the untarred copy.
    codesign --force --deep --sign - "${RETAIL_DIR}/igi.app" 2>/dev/null || true
    echo "Deployed: ${RETAIL_DIR}/igi.app"
elif [ "${DEPLOY}" -eq 1 ]; then
    echo "[4/4] Retail directory ${RETAIL_DIR} not found; skipping deploy."
else
    echo "[4/4] Deploy skipped (--no-deploy)."
fi

echo "========================================================"
echo " Build & Test Succeeded"
echo " Run:  ./run.sh --mission 1 --graphics high"
echo "========================================================"
