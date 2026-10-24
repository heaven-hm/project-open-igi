"""Autonomous reverse-engineering harness (self-contained, REA-disciplined).

Static + runtime + game-natives discovery/decompilation against a retail binary,
behind a fail-closed scope gate, with a REA-style evidence ledger.

This package is NOT the RE-Agent CLI (auto-re-agent / re-agent) and NOT the REA
MCP server (reverse-engineer-anything). It coordinates local tools (radare2,
ghidra-cli, frida, capstone, pefile) and records evidence with the project's
fail-closed discipline. See docs/AUTONOMOUS_RE_HARNESS.md.
"""

__version__ = "0.1.0"
SCHEMA_VERSION = 1
