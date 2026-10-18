#!/usr/bin/env bash
# Verify the native verification gate, build script, and run script are unmodified
# against the committed trust anchor (docs/fidelity/gate-integrity.sha256).
# Any coding agent that edits the gate/build to fake a pass will fail this check.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

pin="docs/fidelity/gate-integrity.sha256"
if [[ ! -f "$pin" ]]; then
    echo "!! gate integrity anchor missing: $pin" >&2
    exit 1
fi

if command -v shasum >/dev/null 2>&1; then
    shasum -a 256 -c "$pin"
else
    sha256sum -c "$pin"
fi
