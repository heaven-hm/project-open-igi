#!/usr/bin/env python3
"""Derive the retail call-flow order from `main` for the MVP native set.

The captured runtime flow (scripts/frida_flow_order.js) is shallow, so this
derives the order statically from the retail executable: Radare2 analyses the
image once and emits its full call graph (`agCd`), which is walked
breadth-first from retail `main` (0x0048F530). Each MVP entry that is
statically reachable is reported with its depth and parent, and the rest are
listed as reached only indirectly (virtual dispatch / QVM task dispatch).

Usage:
    python3 scripts/derive_flow_order.py --targets scripts/mvp_native_targets.json
"""

from __future__ import annotations

import argparse
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parent.parent
RETAIL = Path.home() / "Downloads/IGI/igi.exe"
MAIN = 0x0048F530


def normalise(address: str) -> str:
    return f"0x{int(address, 16):08X}"


def call_graph(binary: Path) -> dict[str, set[str]]:
    process = subprocess.run(
        ["r2", "-2", "-q", "-c", "aaa; agCd", str(binary)],
        capture_output=True, text=True, timeout=7200, check=False,
    )
    edges: dict[str, set[str]] = {}
    for source, target in re.findall(r'"(0x[0-9a-fA-F]+)"\s*->\s*"(0x[0-9a-fA-F]+)"', process.stdout):
        edges.setdefault(source.lower(), set()).add(target.lower())
    return edges


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--targets", type=Path, default=ROOT / "scripts" / "mvp_native_targets.json")
    parser.add_argument("--binary", type=Path, default=RETAIL)
    parser.add_argument("--report", type=Path, default=ROOT / "build" / "retail-flow-order-derived.json")
    args = parser.parse_args()

    binary = args.binary.resolve()
    if not binary.is_file():
        raise SystemExit(f"retail executable not found: {binary}")
    targets = json.loads(args.targets.read_text())
    names = {f"0x{int(entry['address'], 16):08x}": entry.get("name", "") for entry in targets}
    target_addresses = set(names)
    edges = call_graph(binary)
    print(f"call graph: {len(edges)} nodes with edges, "
          f"{sum(len(v) for v in edges.values())} direct call edges")

    entry = f"0x{MAIN:08x}"
    parent: dict[str, str | None] = {entry: None}
    depth: dict[str, int] = {entry: 0}
    queue = deque([entry])
    while queue:
        current = queue.popleft()
        for callee in edges.get(current, ()):
            if callee in depth:
                continue
            depth[callee] = depth[current] + 1
            parent[callee] = current
            queue.append(callee)

    ordered = sorted(target_addresses & set(depth), key=lambda a: (depth[a], a))
    unreachable = sorted(target_addresses - set(depth))
    report = {
        "schema_version": 1,
        "kind": "retail-flow-order",
        "binary_sha256": hashlib.sha256(binary.read_bytes()).hexdigest(),
        "entry": normalise(hex(MAIN)),
        "targets": len(target_addresses),
        "reachable": len(ordered),
        "unreachable": len(unreachable),
        "reachable_functions": len(depth),
        "ordered": [
            {"order": index, "address": normalise(address), "name": names[address],
             "depth": depth[address], "parent": normalise(parent[address]) if parent[address] else None}
            for index, address in enumerate(ordered, start=1)
        ],
        "unreachable_addresses": [normalise(a) for a in unreachable],
        "limitations": [
            "Direct call edges only; virtual dispatch and QVM task dispatch are not traversed, "
            "so entries reached only through them appear as unreachable.",
            "The order is a breadth-first call graph from retail main, not an execution trace.",
        ],
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(f"reachable MVP natives: {len(ordered)}/{len(target_addresses)}; "
          f"indirect-only {len(unreachable)}; reachable functions {len(depth)}; report {args.report}")
    for item in report["ordered"][:10]:
        print(f"  {item['order']:3d} d{item['depth']} {item['address']} {item['name']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
