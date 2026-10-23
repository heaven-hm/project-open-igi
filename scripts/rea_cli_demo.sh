#!/usr/bin/env bash
set -euo pipefail

# -------------------------------------------------------------------------
# 0️⃣  Make sure the REA server is running (spawn it in background if needed)
# -------------------------------------------------------------------------
if ! pgrep -f "node external/rea/scripts/rea.mjs" > /dev/null; then
  echo "Starting REA server…"
  (cd external/rea && npx rea mcp --http --port 8000 &)   # background daemon
  if ! command -v kuna >/dev/null 2>&1; then
    echo "Kuna binary not found in PATH. Install it (see README) and ensure it's accessible."
    exit 1
  fi
  export REA_KUNA_COMMAND=$(which kuna)
  echo "→ waiting a few seconds for the server to become ready…"
  sleep 3
fi

# -------------------------------------------------------------------------
# 1️⃣  List all REA commands (quick sanity check)
# -------------------------------------------------------------------------
node external/rea/scripts/rea.mjs providers

# -------------------------------------------------------------------------
# 2️⃣  Run a concrete REA tool – example: disassemble a function
# -------------------------------------------------------------------------
# Usage: ./scripts/rea_cli_demo.sh <address> [binary_path]
#   <address>    Hex address of the function to disassemble (e.g., 0x401050)
#   [binary_path] Path to the binary; defaults to the IGI executable.
ADDRESS="${1:-}"
BINARY="${2:-/Users/haseeb-mir/Downloads/IGI/igi.exe}"
if [[ -z "$ADDRESS" ]]; then
  echo "Usage: $0 <address> [binary_path]"
  exit 1
fi
node external/rea/scripts/rea.mjs disassemble \
  --binary "$BINARY" \
  --address "$ADDRESS" \
  --output json | jq '.'   # pretty‑print the JSON response
