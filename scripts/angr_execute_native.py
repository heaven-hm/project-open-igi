#!/usr/bin/env python3
"""Concrete angr execution helper for selected retail-native differential tests."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys


ADDRESS = "0x004B1E90"
RETURN_SENTINEL = 0x13370000
STACK_BASE = 0x70000000
DESTINATION_BASE = 0x71000000
PATH_BASE = 0x71010000
DESTINATION_OFFSET = 32
DESTINATION_SIZE = 128


def execute(project, case: dict[str, object]) -> dict[str, object]:
    import angr

    path = case.get("path")
    max_len = case.get("max_len")
    null_destination = case.get("null_destination")
    if not isinstance(path, str) or not path or "\0" in path:
        raise ValueError("case path must be a non-empty NUL-free string")
    if not isinstance(max_len, int) or isinstance(max_len, bool) or max_len < 0:
        raise ValueError("case max_len must be a non-negative integer")
    if not isinstance(null_destination, bool):
        raise ValueError("case null_destination must be a boolean")
    if not null_destination and max_len > DESTINATION_SIZE - DESTINATION_OFFSET:
        raise ValueError("case destination length exceeds the mapped buffer")

    path_bytes = path.encode("ascii") + b"\0"
    destination = 0 if null_destination else DESTINATION_BASE + DESTINATION_OFFSET
    before = bytes([0xA5]) * DESTINATION_SIZE
    state = project.factory.blank_state(
        addr=int(ADDRESS, 16),
        add_options={
            angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
            angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS,
        },
    )
    stack = STACK_BASE + 0x8000
    for address, value in (
        (stack, RETURN_SENTINEL),
        (stack + 4, destination),
        (stack + 8, max_len),
        (stack + 12, PATH_BASE),
    ):
        state.memory.store(address, value.to_bytes(4, "little"))
    state.memory.store(PATH_BASE, path_bytes)
    if not null_destination:
        state.memory.store(DESTINATION_BASE, before)
    state.regs.esp = stack

    manager = project.factory.simgr(state)
    manager.explore(find=RETURN_SENTINEL, num_find=1, n=20000)
    if not manager.found:
        reasons = [str(item.error) for item in manager.errored]
        raise RuntimeError(
            f"retail execution did not return for {case.get('id')}: "
            f"active={len(manager.active)}, deadended={len(manager.deadended)}, errors={reasons[:2]}"
        )
    final = manager.found[0]
    return_offset = final.solver.eval(final.regs.eax) - PATH_BASE
    if return_offset < 0 or return_offset > len(path_bytes) - 1:
        raise RuntimeError(f"retail returned an unexpected path pointer offset {return_offset}")
    if null_destination:
        after = before
        side_effects: list[str] = []
    else:
        after = final.solver.eval(
            final.memory.load(DESTINATION_BASE, DESTINATION_SIZE), cast_to=bytes
        )
        side_effects = ["write:destination-buffer"]
    changed = [i for i, (old, new) in enumerate(zip(before, after)) if old != new]
    return {
        "id": case.get("id"),
        "return_offset": return_offset,
        "changed_offsets": changed,
        "side_effects": side_effects,
        "destination_hex": after.hex(),
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--address", required=True)
    args = parser.parse_args()
    address = "0x" + args.address[-8:].upper()
    if address != ADDRESS:
        raise ValueError(f"unsupported angr execution address {address}")
    if not args.binary.is_file():
        raise FileNotFoundError(args.binary)

    cases = json.load(sys.stdin)
    if not isinstance(cases, list) or len(cases) < 8:
        raise ValueError("at least eight case descriptions are required")

    import angr

    project = angr.Project(str(args.binary.resolve()), auto_load_libs=False)
    if project.arch.name != "X86" or project.arch.bits != 32:
        raise RuntimeError(f"expected retail PE32 x86, got {project.arch.name}/{project.arch.bits}")
    observations = [execute(project, case) for case in cases]
    print(json.dumps({
        "tool": "angr",
        "angr_version": angr.__version__,
        "engine": "VEX",
        "analysis": "concrete execution of retail function and in-binary callees",
        "binary_path": str(args.binary.resolve()),
        "function_entry": ADDRESS,
        "cases": observations,
    }, separators=(",", ":"), sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
