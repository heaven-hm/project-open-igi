#!/usr/bin/env python3
"""
verify_flow.py – Smart script to verify native function flow against retail IGI.

This script reads a list of target native functions (address & name) and executes
`native_gate.py` for each one with the appropriate flags to collect analysis
(evidence, CFG, angr, etc.). It then aggregates the results and prints a concise
report highlighting which functions passed the verification thresholds and which
did not.

Usage (run from the repository root):
    python3 scripts/verify_flow.py

The script assumes that `scripts/native_gate.py` exists and that the required
tools (Binary Ninja, radare2, angr) are installed and configured per the project
rules.
"""
import json
import subprocess
import os
from pathlib import Path

# Add REA repository to Python path
import sys
import os
REA_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'external', 'rea'))
if REA_PATH not in sys.path:
    sys.path.insert(0, REA_PATH)

# ---------------------------------------------------------------------------
# Load target native methods from the demo verification script.
# This ensures that all demo methods are verified without manually duplicating the list.
# ---------------------------------------------------------------------------
from scripts.verify_demo_natives import TARGET_METHODS as TARGETS

def run_native_gate(address: str) -> dict:
    """Run native_gate.py for a single address and capture its JSON output."""
    cmd = ["python3", "scripts/native_gate.py", "--root", ".",
           "--address", address,
           "--threshold", "VERIFIED",
           "--cfg-threshold", "0.85"]
    try:
        completed = subprocess.run(cmd, capture_output=True, text=True, check=False)
        if completed.returncode != 0:
            return {"success": False, "error": completed.stderr.strip()}
        try:
            return {"success": True, "output": json.loads(completed.stdout)}
        except json.JSONDecodeError:
            return {"success": True, "output": completed.stdout.strip()}
    except Exception as e:
        return {"success": False, "error": str(e)}

def main():
    os.chdir(Path(__file__).resolve().parents[1])  # repo root
    report = []
    print(f"Starting verification of {len(TARGETS)} native functions...")
    for entry in TARGETS:
        name, addr = entry["name"], entry["va"]
        print(f"\nVerifying {name} @ {addr}...")
        result = run_native_gate(addr)
        status = "PASS" if result["success"] else "FAIL"
        report.append({"name": name, "address": addr, "status": status, "details": result})
        print(f"{name}: {status}")

    summary = {
        "total": len(report),
        "passed": sum(1 for r in report if r["status"] == "PASS"),
        "failed": sum(1 for r in report if r["status"] == "FAIL"),
        "details": report,
    }
    with open("scripts/flow_verification_report.json", "w") as f:
        json.dump(summary, f, indent=2)
    print("\nReport written to scripts/flow_verification_report.json")

if __name__ == "__main__":
    main()
