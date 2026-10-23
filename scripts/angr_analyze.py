#!/usr/bin/env python3
"""Emit bounded, machine-readable angr CFG evidence for retail IGI functions.

This helper owns angr imports and binary analysis so native_gate.py remains an
orchestrator. CFGFast is approximate structural evidence; it does not establish
semantic or behavioral equivalence.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import sys
from typing import Any


def sha256(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def address_value(value: str) -> int:
    return int(value, 0)


def analyze(binary: pathlib.Path, addresses: list[int]) -> dict[str, Any]:
    try:
        import angr
    except ImportError as error:
        return {
            "schema_version": 1,
            "kind": "angr-analysis",
            "result": "inconclusive",
            "reason": f"angr is unavailable in {sys.executable}: {error}",
            "binary_path": str(binary),
            "binary_sha256": sha256(binary),
            "functions": {},
        }

    project = angr.Project(str(binary), auto_load_libs=False)
    cfg = project.analyses.CFGFast(normalize=True, data_references=True)
    auto_functions = {address: cfg.kb.functions.get(address) for address in addresses}
    baseline_unresolved = {
        address: bool(function.has_unresolved_jumps) if function is not None else None
        for address, function in auto_functions.items()
    }
    # Snapshot the first graph's structure now. A second CFGFast run on the same
    # project releases the first graph's blocks, and angr's function objects
    # only hold a weak reference to them, so reading the graph afterwards
    # raises "weakly-referenced object no longer exists".
    def snapshot(function) -> dict[str, Any] | None:
        if function is None or function.addr is None:
            return None
        nodes = sorted({int(node.addr) for node in function.graph.nodes()
                        if getattr(node, "addr", None) is not None})
        edges = []
        for source, target, data in function.graph.edges(data=True):
            source_addr = getattr(source, "addr", None)
            target_addr = getattr(target, "addr", None)
            if source_addr is None or target_addr is None:
                continue
            edges.append({
                "source": f"0x{int(source_addr):08X}",
                "target": f"0x{int(target_addr):08X}",
                "kind": str(data.get("jumpkind", "unknown")),
            })
        return {"nodes": nodes, "edges": edges, "unresolved": bool(function.has_unresolved_jumps)}

    auto_snapshots = {address: snapshot(function) for address, function in auto_functions.items()}
    initialized_cfg = None
    initialization_model = None
    if 0x004AF7B0 in addresses and baseline_unresolved.get(0x004AF7B0):
        # Retail installs its exit callback in Error subsystem initialization.
        # Validate the initializer, setter and tail-dispatch bytes before using
        # that concrete state. This is a scoped analysis assumption, not a claim
        # that every possible caller-provided callback has been recovered.
        initializer = project.loader.memory.load(0x00498900, 10)
        setter = project.loader.memory.load(0x004AF8A0, 10)
        dispatch = project.loader.memory.load(0x004AF7FD, 12)
        if (initializer[:1] == b"\x68" and initializer[5:6] == b"\xe8"
                and setter == bytes.fromhex("8b442404a36c629300c3")
                and dispatch == bytes.fromhex("a16c62930085c07402ffe0c3")):
            target = int.from_bytes(initializer[1:5], "little")
            callee = 0x0049890A + int.from_bytes(initializer[6:10], "little", signed=True)
            if callee == 0x004AF8A0 and cfg.kb.functions.get(target) is not None:
                initialization_model = {
                    "scope": "Retail error subsystem after initializer 0x00498900",
                    "global": "0x0093626C", "target": f"0x{target:08X}",
                    "initializer": "0x00498900", "setter": "0x004AF8A0",
                    "initializer_bytes": initializer.hex(), "setter_bytes": setter.hex(),
                    "dispatch_bytes": dispatch.hex(),
                    "limitation": "Arbitrary replacement callbacks and uninitialized state remain outside this CFG model",
                }
                # A new project keeps the baseline unresolved-jump knowledge
                # from contaminating this independently initialized analysis.
                initialized_project = angr.Project(str(binary), auto_load_libs=False)
                initialized_project.loader.memory.store(0x0093626C, target.to_bytes(4, "little"))
                initialized_cfg = initialized_project.analyses.CFGFast(normalize=True, data_references=True)
    seed_addresses = [
        address for address, function in auto_functions.items()
        if function is None or function.addr != address
    ]
    seeded_cfg = None
    if seed_addresses:
        # The caller supplies addresses from native provenance. These seeds are
        # not accepted as independent boundary proof: native_gate must also
        # validate the address against the independent BN/Radare2 artifacts.
        seeded_cfg = project.analyses.CFGFast(
            normalize=True, data_references=True, function_starts=seed_addresses
        )
    functions: dict[str, Any] = {}
    for address in addresses:
        key = f"0x{address:08X}"
        function = auto_functions.get(address)
        recovery_mode = "automatic"
        if address == 0x004AF7B0 and initialized_cfg is not None:
            function = initialized_cfg.kb.functions.get(address)
            recovery_mode = "retail_initializer_callback_model"
        if (function is None or function.addr != address) and seeded_cfg is not None:
            function = seeded_cfg.kb.functions.get(address)
            recovery_mode = "seeded_from_native_address"
        if function is None or function.addr != address:
            functions[key] = {"result": "inconclusive", "reason": "CFGFast did not recover an exact function entry"}
            continue

        snapshot_data = auto_snapshots.get(address) if recovery_mode == "automatic" else None
        if snapshot_data is not None:
            nodes = snapshot_data["nodes"]
            edges = snapshot_data["edges"]
            unresolved = snapshot_data["unresolved"]
        else:
            nodes = sorted({int(node.addr) for node in function.graph.nodes()
                            if getattr(node, "addr", None) is not None})
            edges = []
            for source, target, data in function.graph.edges(data=True):
                source_addr = getattr(source, "addr", None)
                target_addr = getattr(target, "addr", None)
                if source_addr is None or target_addr is None:
                    continue
                edges.append({"source": f"0x{int(source_addr):08X}",
                              "target": f"0x{int(target_addr):08X}",
                              "kind": str(data.get("jumpkind", "unknown"))})
            unresolved = bool(function.has_unresolved_jumps)
        function_result = "pass" if nodes and not unresolved else "inconclusive"
        functions[key] = {
            "result": function_result,
            "reason": "CFGFast left unresolved jumps in this function" if unresolved else None,
            "recovery_mode": recovery_mode,
            "function_entry": f"0x{int(function.addr):08X}",
            "name": function.name,
            "block_count": len(nodes),
            "nodes": [f"0x{node:08X}" for node in nodes],
            "edges": sorted(edges, key=lambda edge: (edge["source"], edge["target"], edge["kind"])),
            "returning": bool(function.returning),
            "has_unresolved_jumps": unresolved,
            "baseline_has_unresolved_jumps": baseline_unresolved.get(address),
            "initialization_model": initialization_model if address == 0x004AF7B0 else None,
        }

    results = [item.get("result") for item in functions.values()]
    overall = "fail" if "fail" in results else "inconclusive" if "inconclusive" in results else "pass"
    artifact = {
        "schema_version": 1,
        "kind": "angr-analysis",
        "result": overall,
        "tool": "angr",
        "angr_version": angr.__version__,
        "analysis": "CFGFast",
        "analysis_options": {
            "normalize": True,
            "data_references": True,
            "automatic_first": True,
            "seeded_address_count": len(seed_addresses),
        },
        "binary_path": str(binary),
        "binary_sha256": sha256(binary),
        "architecture": project.arch.name,
        "bits": project.arch.bits,
        "mapped_base": f"0x{int(project.loader.main_object.mapped_base):X}",
        "functions": functions,
        "limitations": ["CFGFast is approximate static analysis", "This result does not prove semantic or behavioral equivalence"],
    }
    # When one address is analyzed, its function's facts are also reported at
    # the top level, so a caller does not have to reach into the function map.
    if len(functions) == 1:
        only = next(iter(functions.values()))
        artifact["function_entry"] = only.get("function_entry")
        artifact["has_unresolved_jumps"] = only.get("has_unresolved_jumps")
        artifact["reason"] = only.get("reason")
    return artifact


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--address", required=True, action="append", type=address_value)
    args = parser.parse_args()
    try:
        if not args.binary.is_file():
            raise OSError(f"retail executable not found: {args.binary}")
        result = analyze(args.binary.resolve(), sorted(set(args.address)))
    except Exception as error:  # Emit structured failure for the gate, never a fabricated pass.
        result = {
            "schema_version": 1,
            "kind": "angr-analysis",
            "result": "fail",
            "reason": f"{type(error).__name__}: {error}",
            "binary_path": str(args.binary),
            "binary_sha256": sha256(args.binary) if args.binary.is_file() else None,
            "functions": {},
        }
    print(json.dumps(result, sort_keys=True))
    return 0 if result["result"] == "pass" else 2 if result["result"] == "inconclusive" else 1


if __name__ == "__main__":
    raise SystemExit(main())
