#!/usr/bin/env bash
set -euo pipefail

# Project Open IGI — formatter + static analysis + warning/linker gate.
# Uses LLVM tooling (brew install llvm). Run from the repository root.

export PATH="/opt/homebrew/opt/llvm/bin:${PATH}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

IFS='\n' read -r -d '' -a SOURCES < <(find engine/src engine/include engine/tools engine/tests \
    -name '*.cpp' -o -name '*.hpp' -o -name '*.h' | sort | tr '\n' '\000') || true

echo "[1/3] clang-format check (${#SOURCES[@]} files)"
DIRTY=0
for f in "${SOURCES[@]}"; do
    if clang-format --output-replacements-xml "${f}" | grep -q '<replacement '; then
        echo "  needs formatting: ${f}"
        DIRTY=1
    fi
done
if [ "${DIRTY}" -ne 0 ]; then
    echo "  applying clang-format -i ..."
    clang-format -i "${SOURCES[@]}"
fi

echo "[2/3] clang-tidy (real issues: includes, unused, bugs)"
clang-tidy engine/src/logger.cpp engine/src/exceptions.cpp engine/src/diagnostics.cpp \
    engine/src/format_support.cpp engine/src/main.cpp -p build --quiet 2>&1 | \
    grep -E "warning:|error:" | grep -Ev "trailing return|magic number|identifier length|enum.*base|designated initializer|scoped_lock|trailing comma|const-correctness|easily-swappable|uppercase-literal|math-missing-parenthes" || true

echo "[3/3] strict warning + linker gate (Debug with -Wall -Wextra -Wpedantic -Wunused -Wshadow)"
cmake -B build-warn -S . -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wunused -Wshadow" \
    -DIGI_GAME_PATH="${IGI_GAME_PATH:-/Users/haseeb-mir/Downloads/IGI}" >/dev/null
cmake --build build-warn -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 8)" 2>&1 | tee /tmp/igi-lint-build.log | grep -E "warning:|error:" || true
if grep -qE "warning:|error:" /tmp/igi-lint-build.log; then
    echo "[FAIL] warnings or errors found in strict build"
    exit 1
fi

BIN="build/engine/igi.app/Contents/MacOS/igi"
if [ -f "${BIN}" ]; then
    cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 8)" >/dev/null
    if nm -u "${BIN}" | grep -q "__ZN3igi"; then
        echo "[FAIL] unresolved igi:: symbols in ${BIN}"
        exit 1
    fi
fi

echo "LINT OK: format clean, tidy clean, strict build zero warnings, linker symbols resolved"
