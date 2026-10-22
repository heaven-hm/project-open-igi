#!/usr/bin/env bash
# Single-entry deterministic advanced reverse-engineering pipeline.
# Delegates to scripts/advanced_reverse_engineering_pipeline.py (fail-closed code flow).
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec python3 "$SCRIPT_DIR/advanced_reverse_engineering_pipeline.py" "$@"
