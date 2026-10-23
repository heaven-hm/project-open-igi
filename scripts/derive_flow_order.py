#!/usr/bin/env python3
"""Derive the retail call-flow order from `main` for the MVP native set.

The captured runtime flow (scripts/frida_flow_order.js) is shallow, so this
derives the order statically from the retail executable:

1. Radare2's full direct call graph (`agCd`) is walked breadth-first from
   retail `main` (0x0048F530).
2. Entries not reached directly are then linked through their code/data
   cross-references: when a function that is already on the flow references
   the entry (virtual table, dispatch table or indirect call site), the entry
   is attached to that function and the walk continues.

The report records depth, parent, whether the edge was direct or via a
cross-reference, and lists entries that remain unreachable. Indirect
references are ordering evidence only: they do not prove the call happens at
run time, which is what the paired runtime capture is for.

Usage:
    python3 scripts/derive_flow_order.py --targets scripts/mvp_native_targets.json
"""

from __future__ import annotations

import argparse
from bisect import bisect_right
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile

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


def function_ranges(binary: Path) -> tuple[list[int], list[tuple[int, str]]]:
    """Sorted function starts and (end, function address) for containing lookup."""
    process = subprocess.run(
        ["r2", "-2", "-q", "-c", "aaa; aflj", str(binary)],
        capture_output=True, text=True, timeout=7200, check=False,
    )
    text = process.stdout
    start, end = text.find("["), text.rfind("]")
    functions = json.loads(text[start:end + 1]) if start >= 0 and end > start else []
    entries = []
    for item in functions:
        if not isinstance(item, dict) or "addr" not in item:
            continue
        low = item.get("minaddr", item.get("min"))
        high = item.get("maxaddr", item.get("max"))
        if low is None or high is None:
            continue
        entries.append((int(low), int(high), normalise(hex(item["addr"]))))
    entries.sort()
    starts = [entry[0] for entry in entries]
    return starts, [(entry[1], entry[2]) for entry in entries]


def containing(starts: list[int], ends: list[tuple[int, str]], address: int) -> str | None:
    index = bisect_right(starts, address) - 1
    if index < 0:
        return None
    end, function = ends[index]
    return function if address <= end else None


def xref_edges(binary: Path, addresses: list[str],
               starts: list[int], ends: list[tuple[int, str]]) -> dict[str, set[str]]:
    """Edges from the function containing a code/data reference to each entry."""
    if not addresses:
        return {}
    parts = ["aaa"]
    for index, address in enumerate(addresses):
        parts += [f"s {address}", "axtj", f"?e XREFMARK{index}"]
    with tempfile.NamedTemporaryFile("w", suffix=".r2", delete=False) as handle:
        handle.write("\n".join(parts) + "\n")
        script_path = handle.name
    try:
        process = subprocess.run(
            ["r2", "-2", "-q", "-i", script_path, str(binary)],
            capture_output=True, text=True, timeout=7200, check=False,
        )
    finally:
        Path(script_path).unlink(missing_ok=True)
    segments = re.split(r"XREFMARK(\d+)", process.stdout)
    edges: dict[str, set[str]] = {}
    for position in range(1, len(segments) - 1, 2):
        index, body = int(segments[position]), segments[position + 1]
        start, end = body.find("["), body.rfind("]")
        if start < 0 or end <= start:
            continue
        try:
            references = json.loads(body[start:end + 1])
        except json.JSONDecodeError:
            continue
        target = addresses[index]
        for reference in references:
            raw = reference.get("from") if isinstance(reference, dict) else None
            try:
                if isinstance(raw, int):
                    source = raw
                elif isinstance(raw, str):
                    source = int(raw, 0)
                else:
                    source = None
            except ValueError:
                source = None
            if source is None:
                continue
            container = containing(starts, ends, source)
            if container and container != target:
                edges.setdefault(container.lower(), set()).add(target.lower())
    return edges


def walk(edges: dict[str, set[str]], entry: str):
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
    return depth, parent


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--targets", type=Path, default=ROOT / "scripts" / "mvp_native_targets.json")
    parser.add_argument("--binary", type=Path, default=RETAIL)
    parser.add_argument("--report", type=Path, default=ROOT / "build" / "retail-flow-order-derived.json")
    parser.add_argument("--no-xrefs", action="store_true", help="skip cross-reference ordering")
    parser.add_argument("--runtime", type=Path,
                        default=ROOT / "build" / "runtime-traces" / "mvp-runtime-flow-75.json",
                        help="optional retail runtime capture to merge as observed order")
    args = parser.parse_args()

    binary = args.binary.resolve()
    if not binary.is_file():
        raise SystemExit(f"retail executable not found: {binary}")
    targets = json.loads(args.targets.read_text())
    names = {f"0x{int(entry['address'], 16):08x}": entry.get("name", "") for entry in targets}
    target_addresses = set(names)
    entry = f"0x{MAIN:08x}"

    direct = call_graph(binary)
    depth, parent = walk(direct, entry)
    print(f"direct call graph: {len(direct)} nodes, "
          f"{sum(len(v) for v in direct.values())} edges, {len(depth)} reachable functions")

    edge_kind: dict[str, str] = {address: "direct" for address in depth}
    if not args.no_xrefs:
        unresolved = sorted(target_addresses - set(depth))
        starts, ends = function_ranges(binary)
        indirect = xref_edges(binary, unresolved, starts, ends)
        merged = {source: set(targets_) for source, targets_ in direct.items()}
        for source, targets_ in indirect.items():
            merged.setdefault(source, set()).update(targets_)
        depth, parent = walk(merged, entry)
        for address in depth:
            edge_kind.setdefault(address, "cross-reference")
        print(f"with cross-references: {len(indirect)} referencing functions, "
              f"{len(depth)} reachable functions")

    runtime_order: dict[str, int] = {}
    if args.runtime and args.runtime.is_file():
        capture = json.loads(args.runtime.read_text())
        for hit in capture.get("ordered", []):
            address = hit["address"].lower()
            runtime_order.setdefault(address, hit["order"])
        print(f"runtime capture: {len(runtime_order)} observed entries from {args.runtime.name}")

    def sort_key(address: str):
        if address in runtime_order:
            return (0, runtime_order[address], address)
        return (1, depth.get(address, 10**6), address)

    ordered = sorted(target_addresses & (set(depth) | set(runtime_order)), key=sort_key)
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
        "runtime_observed": len(runtime_order),
        "ordered": [
            {"order": index, "address": normalise(address), "name": names[address],
             "depth": depth[address],
             "parent": normalise(parent[address]) if parent[address] else None,
             "edge": "runtime" if address in runtime_order else edge_kind.get(address, "cross-reference"),
             "runtime_order": runtime_order.get(address)}
            for index, address in enumerate(ordered, start=1)
        ],
        "unreachable_addresses": [normalise(a) for a in unreachable],
        "runtime_capture": str(args.runtime) if args.runtime and args.runtime.is_file() else None,
        "limitations": [
            "Cross-reference edges record that a function references the entry (dispatch table, "
            "vtable or indirect call site); they order the entry but do not prove the call occurs.",
            "Entries with no direct or referencing caller remain unordered and are listed separately.",
            "The order is a graph traversal from retail main, not an execution trace.",
        ],
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(f"reachable MVP natives: {len(ordered)}/{len(target_addresses)}; "
          f"still unreachable {len(unreachable)}; report {args.report}")
    for item in report["ordered"][:10]:
        print(f"  {item['order']:3d} d{item['depth']} {item['edge'][:7]} {item['address']} {item['name']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
