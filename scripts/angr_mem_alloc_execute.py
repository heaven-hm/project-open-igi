#!/usr/bin/env python3
"""Run retail and candidate Mem_Alloc with matching x86 heap boundary models."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
from typing import Any

from angr_abi_execute import ALLOCATED_POINTER, STOP, _hook_allocator, _procedure

RETAIL_ENTRY = 0x004B0C60
RETAIL_GLOBALS = {
    "alloc_count": 0x00942300,
    "heap_init": 0x009422FC,
    "head": 0x00A960F0,
    "sentinel": 0x00A960F4,
    "tail": 0x00A960F8,
    "state": 0x00A960E0,
    "qhash": 0x009422FD,
    "new_handler": 0x00936254,
    "new_sentinel": 0x00936250,
    "heap_handle": 0x00C32428,
    "heap_mode": 0x00C3242C,
    "pool_threshold": 0x00C321FC,
    "bucket_bound": 0x005466D4,
}


def _run(project: Any, entry: int, size: int, alignment: int,
         *, retail: bool, observations_symbol: int | None = None) -> dict[str, Any]:
    import angr

    _hook_allocator(project, "HeapAlloc" if retail else "malloc", True)
    state = project.factory.call_state(
        entry, size, alignment, ret_addr=STOP,
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS},
    )
    if retail:
        for name, address in RETAIL_GLOBALS.items():
            width = 1 if name in {"heap_init", "qhash"} else 4
            state.memory.store(address, b"\x00" * width)
    manager = project.factory.simgr(state)
    manager.explore(find=STOP, num_find=1, n=50000)
    if not manager.found:
        reasons = [str(item.error) for item in manager.errored]
        raise RuntimeError(f"Mem_Alloc did not return: {reasons[:2]}")
    final = manager.found[0]
    result = final.solver.eval(final.regs.eax)
    allocation_calls = list(final.globals.get("allocation_calls", []))
    raw = ALLOCATED_POINTER
    if retail:
        global_values = {
            name: final.solver.eval(final.memory.load(
                address, 1 if name in {"heap_init", "qhash"} else 4, endness="Iend_LE"))
            for name, address in RETAIL_GLOBALS.items()
        }
        head_address = RETAIL_GLOBALS["head"]
        sentinel_address = RETAIL_GLOBALS["sentinel"]
        tail_address = RETAIL_GLOBALS["tail"]
    else:
        if observations_symbol is None:
            raise RuntimeError("candidate observations export is missing")
        observation_values = [
            final.solver.eval(final.memory.load(observations_symbol + index * 4, 4, endness="Iend_LE"))
            for index in range(11)
        ]
        global_values = dict(zip(
            ("result", "alloc_count", "heap_init", "head", "sentinel", "tail", "state", "qhash",
             "head_address", "sentinel_address", "tail_address"), observation_values,
        ))
        head_address = global_values["head_address"]
        sentinel_address = global_values["sentinel_address"]
        tail_address = global_values["tail_address"]

    returned = result if retail else global_values["result"]
    header = [final.solver.eval(final.memory.load(raw + offset, 4, endness="Iend_LE")) for offset in (0, 4, 8, 12, 16)]
    back_pointer_address = (returned - 4) & 0xFFFFFFFC
    back_pointer = final.solver.eval(final.memory.load(back_pointer_address, 4, endness="Iend_LE"))
    payload_alignment = returned & (alignment - 1)
    header_relations = {
            "next_is_sentinel_slot": header[0] == sentinel_address,
            "previous_is_head_slot": header[1] == head_address,
            "payload_pointer": header[2],
            "total_bytes": header[3],
            "user_bytes": header[4],
            "back_pointer_targets_raw_block": back_pointer == raw,
            "payload_alignment_remainder": payload_alignment,
        }
    heap_state = {
            key: global_values[key]
            for key in ("alloc_count", "heap_init", "head", "sentinel", "tail", "state", "qhash")
        }
    return {
        "return_value": returned,
        "state_delta": {"heap_state": heap_state, "header_relations": header_relations},
        "side_effects": {"allocation_requests": allocation_calls},
    }


def execute(binary: pathlib.Path, candidate: pathlib.Path, cases: list[dict[str, Any]]) -> dict[str, Any]:
    import angr

    retail = angr.Project(str(binary.resolve()), auto_load_libs=False)
    rebuilt = angr.Project(str(candidate.resolve()), auto_load_libs=False)
    if retail.arch.name != "X86" or retail.arch.bits != 32 or rebuilt.arch.name != "X86" or rebuilt.arch.bits != 32:
        raise RuntimeError("both retail and candidate binaries must be PE32 x86")
    entry = rebuilt.loader.find_symbol("NativeEntry")
    observations = rebuilt.loader.find_symbol("NativeObservations")
    if entry is None or observations is None:
        raise RuntimeError("candidate DLL is missing NativeEntry/NativeObservations exports")
    results = []
    for case in cases:
        size, alignment = int(case["size"]), int(case["alignment"])
        reference = _run(retail, RETAIL_ENTRY, size, alignment, retail=True)
        candidate_output = _run(rebuilt, entry.rebased_addr, size, alignment,
                                retail=False, observations_symbol=observations.rebased_addr)
        results.append({
            "id": case["id"],
            "input": {"args": [size, alignment], "initial_state": {"heap_mode": 0, "allocation_succeeds": True}},
            "categories": case["categories"],
            "reference_output": reference,
            "candidate_output": candidate_output,
        })
    return {"tool": "angr", "version": angr.__version__, "engine": "VEX",
            "analysis": "Retail PE32 Mem_Alloc and freshly compiled x86 candidate executed with identical deterministic allocation addresses",
            "function_entry": f"0x{RETAIL_ENTRY:08X}", "cases": results}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--candidate", type=pathlib.Path, required=True)
    args = parser.parse_args()
    cases = json.load(sys.stdin)
    if not isinstance(cases, list) or len(cases) < 8:
        raise ValueError("at least eight Mem_Alloc cases are required")
    print(json.dumps(execute(args.binary, args.candidate, cases), separators=(",", ":"), sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
