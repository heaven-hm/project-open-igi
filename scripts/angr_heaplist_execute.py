#!/usr/bin/env python3
"""Compare retail and candidate HeapList_Insert in 32-bit angr/VEX."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
from typing import Any

STOP = 0x13370000
ARENA = 0x50000000


def init_arena(state: Any, count: int) -> list[int]:
    """Build the retail list header and a chain of ``count`` existing nodes."""
    header = ARENA
    incoming = ARENA + 0x100
    existing = [ARENA + 0x200 + index * 0x20 for index in range(count)]
    state.memory.store(header, (header + 4).to_bytes(4, "little"))
    state.memory.store(header + 4, (existing[0] if existing else 0).to_bytes(4, "little"))
    state.memory.store(header + 8, (existing[-1] if existing else header).to_bytes(4, "little"))
    for index, node in enumerate(existing):
        next_node = existing[index + 1] if index + 1 < count else 0
        prev_node = existing[index - 1] if index else header
        state.memory.store(node, next_node.to_bytes(4, "little"))
        state.memory.store(node + 4, prev_node.to_bytes(4, "little"))
    state.memory.store(incoming, b"\xa5" * 8)
    return [header, incoming, *existing]


def init_remove_arena(state: Any, count: int, index: int) -> tuple[int, list[int]]:
    """Build a valid retail intrusive-list layout and return the selected node."""
    header = ARENA
    nodes = [ARENA + 0x200 + item * 0x20 for item in range(count)]
    state.memory.store(header, (header + 4).to_bytes(4, "little"))
    state.memory.store(header + 4, (nodes[0] if nodes else header + 4).to_bytes(4, "little"))
    state.memory.store(header + 8, (nodes[-1] if nodes else header).to_bytes(4, "little"))
    for item, node in enumerate(nodes):
        next_slot = nodes[item + 1] if item + 1 < count else header + 4
        previous_slot = nodes[item - 1] if item else header
        state.memory.store(node, next_slot.to_bytes(4, "little"))
        state.memory.store(node + 4, previous_slot.to_bytes(4, "little"))
    return nodes[index], [header, *nodes]


def run(project: Any, entry: int, count: int, *, remove_index: int | None = None) -> dict[str, Any]:
    import angr

    if remove_index is None:
        args = [ARENA, ARENA + 0x100]
    else:
        args = [ARENA + 0x200 + remove_index * 0x20]
    state = project.factory.call_state(
        entry,
        *args,
        ret_addr=STOP,
        add_options={
            angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
            angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS,
        },
    )
    addresses = init_arena(state, count) if remove_index is None else init_remove_arena(state, count, remove_index)[1]
    manager = project.factory.simgr(state)
    manager.explore(find=STOP, num_find=1, n=10000)
    if not manager.found:
        details = [str(item.error) for item in manager.errored]
        raise RuntimeError(f"HeapList_Insert failed to return: {details[:2]}")
    final = manager.found[0]
    memory = {
        f"0x{address:08X}": final.solver.eval(
            final.memory.load(address, 12 if address == ARENA else 8), cast_to=bytes
        ).hex()
        for address in addresses
    }
    return {
        "return_value": final.solver.eval(final.regs.eax),
        "state_delta": memory,
        "side_effects": [],
    }


def execute(binary: pathlib.Path, candidate: pathlib.Path, cases: list[dict[str, Any]]) -> dict[str, Any]:
    import angr

    retail = angr.Project(str(binary.resolve()), auto_load_libs=False)
    rebuilt = angr.Project(str(candidate.resolve()), auto_load_libs=False)
    if retail.arch.name != "X86" or retail.arch.bits != 32:
        raise RuntimeError("retail binary did not load as PE32 x86")
    if rebuilt.arch.name != "X86" or rebuilt.arch.bits != 32:
        raise RuntimeError("candidate did not load as PE32 x86")
    insert_symbol = rebuilt.loader.find_symbol("NativeInsert")
    remove_symbol = rebuilt.loader.find_symbol("NativeRemove")
    if insert_symbol is None or remove_symbol is None:
        raise RuntimeError("candidate DLL must export NativeInsert and NativeRemove")

    observations = []
    for case in cases:
        operation = case["operation"]
        if operation == "insert":
            reference = run(retail, 0x004AF8F0, int(case["existing_nodes"]))
            reconstructed = run(rebuilt, insert_symbol.rebased_addr, int(case["existing_nodes"]))
        elif operation == "remove":
            index = int(case["remove_index"])
            reference = run(retail, 0x004AF960, int(case["existing_nodes"]), remove_index=index)
            reconstructed = run(rebuilt, remove_symbol.rebased_addr, int(case["existing_nodes"]), remove_index=index)
        else:
            raise ValueError(f"unsupported list operation {operation}")
        observations.append({
            "id": case["id"],
            "input": {"args": ["context", "node"] if operation == "insert" else ["node"],
                      "initial_state": {"existing_nodes": case["existing_nodes"],
                                        **({"remove_index": case["remove_index"]} if operation == "remove" else {})}},
            "categories": case["categories"],
            "reference_output": reference,
            "candidate_output": reconstructed,
        })
    return {
        "tool": "angr",
        "version": angr.__version__,
        "engine": "VEX",
        "analysis": "Retail PE32 function and freshly compiled candidate PE32 DLL executed against identical list layouts",
        "function_entry": "0x004AF8F0",
        "cases": observations,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--candidate", type=pathlib.Path, required=True)
    args = parser.parse_args()
    cases = json.load(sys.stdin)
    if not isinstance(cases, list) or len(cases) < 8:
        raise ValueError("at least eight list-layout cases are required")
    print(json.dumps(execute(args.binary, args.candidate, cases), separators=(",", ":"), sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
