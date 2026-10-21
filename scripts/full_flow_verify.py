#!/usr/bin/env python3
"""
full_flow_verify.py – Comprehensive script to trace the full native call flow
from the game entry point(s) through to leaf functions and compare it against
the expected flow derived from the retail IGI binary.

The script:
  1. Starts from a set of known entry addresses (e.g. QTask_OpenEvent).
  2. For each address it runs `scripts/native_gate.py` with the same flags the
     user has been using to gather evidence. The output (JSON) is expected to
     contain at least:
        - "address"
        - "name"
        - "callers" (list of VA strings)
        - "callees" (list of VA strings)
  3. Builds a directed call graph (VA -> callee VA).
  4. Walks the graph to produce an ordered list of functions from start to
     termination (topological sort – cycles are reported).
  5. Generates a report JSON file summarizing:
        * total functions discovered
        * any functions whose analysis failed
        * mismatches between discovered edges and the expected edges (if a
          reference manifest is provided)
        * a simple linear flow preview for human inspection.

Usage (run from repo root):
    python3 scripts/full_flow_verify.py

The script assumes `scripts/native_gate.py` is present and functional and that
the necessary analysis tools (Binary Ninja, radare2, angr) are configured per
project rules.
"""
import json
import subprocess
import os
from pathlib import Path
from collections import defaultdict, deque

# ---------------------------------------------------------------------------
# Configuration – entry points and optional reference manifest.
# ---------------------------------------------------------------------------
ENTRY_POINTS = [
    {"name": "QTask_OpenEvent", "va": "0x00401050"},
    {"name": "Game_GetContext", "va": "0x00402880"},
    {"name": "Flow_SetState", "va": "0x004028A0"},
]

# Optional reference manifest of expected call edges (VA -> [callee VA]).
REFERENCE_MANIFEST_PATH = None  # e.g. "engine/reference_flow.json"

# ---------------------------------------------------------------------------
def run_native_gate(address: str) -> dict:
    cmd = ["python3", "scripts/native_gate.py", "--root", ".",
           "--address", address,
           "--threshold", "VERIFIED",
           "--cfg-threshold", "0.85"]
    try:
        completed = subprocess.run(cmd, capture_output=True, text=True, check=False)
        if completed.returncode != 0:
            return {"success": False, "error": completed.stderr.strip()}
        try:
            data = json.loads(completed.stdout)
            return {"success": True, "data": data}
        except json.JSONDecodeError:
            return {"success": False, "error": "Invalid JSON output"}
    except Exception as e:
        return {"success": False, "error": str(e)}

# ---------------------------------------------------------------------------
def build_call_graph():
    graph = defaultdict(set)   # caller -> set(callee)
    visited = set()
    queue = deque([ep["va"] for ep in ENTRY_POINTS])
    details = {}
    failures = []
    while queue:
        va = queue.popleft()
        if va in visited:
            continue
        visited.add(va)
        res = run_native_gate(va)
        if not res["success"]:
            failures.append({"address": va, "error": res.get("error")})
            continue
        data = res["data"]
        name = data.get("name", "<unknown>")
        callees = data.get("callees", [])
        details[va] = {"name": name, "callees": callees}
        for callee in callees:
            graph[va].add(callee)
            if callee not in visited:
                queue.append(callee)
    return graph, details, failures, visited

# ---------------------------------------------------------------------------
def topological_sort(graph, start_nodes):
    indegree = defaultdict(int)
    for src, dsts in graph.items():
        for dst in dsts:
            indegree[dst] += 1
    zero = deque([n for n in start_nodes if indegree[n] == 0])
    order = []
    while zero:
        node = zero.popleft()
        order.append(node)
        for nxt in graph.get(node, []):
            indegree[nxt] -= 1
            if indegree[nxt] == 0:
                zero.append(nxt)
    remaining = set(graph.keys()) | set(indegree.keys())
    cycles = list(remaining - set(order))
    return order, cycles

def main():
    os.chdir(Path(__file__).resolve().parents[1])  # repo root
    graph, details, failures, visited = build_call_graph()
    start_vas = [ep["va"] for ep in ENTRY_POINTS]
    flow_order, cycles = topological_sort(graph, start_vas)
    report = {
        "total_functions": len(visited),
        "entry_points": start_vas,
        "graph_edges": {k: list(v) for k, v in graph.items()},
        "function_details": details,
        "failures": failures,
        "flow_order": flow_order,
        "cycles": cycles,
    }
    if REFERENCE_MANIFEST_PATH:
        try:
            with open(REFERENCE_MANIFEST_PATH) as f:
                reference = json.load(f)
            mismatches = []
            for caller, callees in graph.items():
                expected = set(reference.get(caller, []))
                actual = set(callees)
                if expected != actual:
                    mismatches.append({"caller": caller, "expected": list(expected), "actual": list(actual)})
            report["reference_mismatches"] = mismatches
        except Exception as e:
            report["reference_load_error"] = str(e)
    out_path = Path("scripts/full_flow_report.json")
    with open(out_path, "w") as f:
        json.dump(report, f, indent=2)
    print(f"Full flow verification complete. Report saved to {out_path}")
    if cycles:
        print("⚠️ Detected cycles in the call graph – manual review required.")
    if failures:
        print(f"⚠️ {len(failures)} functions failed analysis – see report.")

if __name__ == "__main__":
    main()
