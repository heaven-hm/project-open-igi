#!/usr/bin/env python3
"""Execute retail and x86 candidate allocator natives with angr/VEX.

The heap APIs are replaced with deterministic boundary procedures. The helper
compares returns, request sizes, six mapped globals, and a 64-byte synthetic
allocation guard under shared deterministic allocation addresses. Fixture inputs
provide the before-state; final global and guard values are read from execution.
"""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
from typing import Any


RETAIL_ENTRIES = {
    "0x004AA670": (0x004AA670, "chunk"),
    "0x004AA682": (0x004AA682, "retry"),
    "0x004AA6AE": (0x004AA6AE, "raw"),
}
STOP = 0x13370000
ALLOCATED_POINTER = 0x60000000
GLOBAL_ADDRESSES = (0x00936250, 0x00936254, 0x00C3242C,
                    0x00C32428, 0x00C321FC, 0x005466D4)
GUARD_BYTES = bytes(range(64))


def _procedure(angr: Any, kind: str, allocation_succeeds: bool):
    if kind == "HeapAlloc":
        class Allocator(angr.SimProcedure):
            num_args = 3

            def run(self, heap, flags, size):
                return self._allocate(size)
    else:
        class Allocator(angr.SimProcedure):
            num_args = 1

            def run(self, size):
                return self._allocate(size)

    def _allocate(self, size):
        size = self.state.solver.eval(size)
        calls = list(self.state.globals.get("allocation_calls", []))
        calls.append(size)
        self.state.globals["allocation_calls"] = calls
        return ALLOCATED_POINTER if allocation_succeeds else 0

    Allocator._allocate = _allocate
    return Allocator


def _hook_allocator(project: Any, kind: str, allocation_succeeds: bool) -> None:
    import angr
    from angr.sim_type import SimTypeFunction, SimTypeInt

    symbol = project.loader.find_symbol(kind)
    if symbol is None:
        raise RuntimeError(f"angr could not resolve the {kind} boundary in {project.filename}")
    procedure_type = _procedure(angr, kind, allocation_succeeds)
    proc = procedure_type()
    proc.cc = (
        angr.calling_conventions.SimCCStdcall(project.arch)
        if kind == "HeapAlloc"
        else angr.calling_conventions.SimCCCdecl(project.arch)
    )
    proc.prototype = SimTypeFunction(
        [SimTypeInt(False, 32)] * proc.num_args, SimTypeInt(False, 32)
    ).with_arch(project.arch)
    project.hook(symbol.rebased_addr, proc, replace=True)


def _run(project: Any, entry: int, args: list[int], kind: str,
         allocation_succeeds: bool, retail_state: dict[str, int] | None = None,
         observed_addresses: dict[str, int] | None = None,
         initial_globals: dict[str, int] | None = None) -> dict[str, Any]:
    import angr

    _hook_allocator(project, kind, allocation_succeeds)
    state = project.factory.call_state(
        entry,
        *args,
        ret_addr=STOP,
        add_options={
            angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
            angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS,
        },
    )
    if retail_state:
        for address, value in retail_state.items():
            state.memory.store(int(address, 16), value.to_bytes(4, "little"))
    state.memory.store(ALLOCATED_POINTER, GUARD_BYTES)
    manager = project.factory.simgr(state)
    manager.explore(find=STOP, num_find=1, n=100000)
    if not manager.found:
        reasons = [str(item.error) for item in manager.errored]
        raise RuntimeError(
            f"{project.filename} function 0x{entry:08X} failed to return: "
            f"active={len(manager.active)}, errors={reasons[:2]}"
        )
    final = manager.found[0]
    result = final.solver.eval(final.regs.eax)
    return {
        "return_value": result,
        "return_class": "null" if result == 0 else "allocated",
        "requested_allocation_sizes": list(final.globals.get("allocation_calls", [])),
        "state_delta": {
            key: {"before": initial_globals[key],
                  "after": final.solver.eval(final.memory.load(location, 4, endness=project.arch.memory_endness))}
            for key, location in (observed_addresses or {}).items()
        },
        "side_effects": {
            "allocation_requests": list(final.globals.get("allocation_calls", [])),
            "allocation_guard_before": GUARD_BYTES.hex(),
            "allocation_guard_after": final.solver.eval(
                final.memory.load(ALLOCATED_POINTER, len(GUARD_BYTES)), cast_to=bytes).hex(),
        },
    }


def execute(binary: pathlib.Path, candidate: pathlib.Path, address: str,
            cases: list[dict[str, Any]]) -> dict[str, Any]:
    import angr

    key = "0x" + address[-8:].upper()
    if key not in RETAIL_ENTRIES:
        raise ValueError(f"unsupported allocator native {key}")
    retail_entry, operation = RETAIL_ENTRIES[key]
    retail_project = angr.Project(str(binary.resolve()), auto_load_libs=False)
    candidate_project = angr.Project(str(candidate.resolve()), auto_load_libs=False)
    if retail_project.arch.name != "X86" or retail_project.arch.bits != 32:
        raise RuntimeError("retail binary must load as PE32 x86")
    if candidate_project.arch.name != "X86" or candidate_project.arch.bits != 32:
        raise RuntimeError("candidate binary must load as PE32 x86")
    candidate_symbol = candidate_project.loader.find_symbol("NativeEntry")
    if candidate_symbol is None:
        raise RuntimeError("candidate PE does not export NativeEntry")

    getter = candidate_project.loader.find_symbol("NativeGlobalAddress")
    if getter is None:
        raise RuntimeError("candidate PE does not export NativeGlobalAddress")
    candidate_globals = {}
    for index, retail_address in enumerate(GLOBAL_ADDRESSES):
        state = candidate_project.factory.call_state(
            getter.rebased_addr, index, ret_addr=STOP,
            add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                         angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
        manager = candidate_project.factory.simgr(state)
        manager.explore(find=STOP, num_find=1, n=1000)
        if not manager.found:
            raise RuntimeError("candidate global-address adapter did not return")
        location = manager.found[0].solver.eval(manager.found[0].regs.eax)
        if not location or not candidate_project.loader.main_object.contains_addr(location):
            raise RuntimeError("candidate global-address adapter returned invalid storage")
        candidate_globals[f"0x{retail_address:08X}"] = location

    observations = []
    for case in cases:
        size = int(case["size"])
        mode = int(case.get("mode", 0))
        threshold = int(case.get("pool_threshold", 0))
        pool_bound = int(case.get("bucket_bound", 0))
        sentinel = int(case.get("sentinel", 0))
        succeeds = bool(case.get("allocation_succeeds", True))
        if operation == "raw":
            retail_args = [size]
        elif operation == "retry":
            retail_args = [size, sentinel]
        else:
            retail_args = [size]
        globals_state = {
            "0x00C3242C": mode,
            "0x00C321FC": threshold,
            "0x005466D4": pool_bound,
            "0x00936250": sentinel,
            "0x00936254": 0,
            "0x00C32428": 0,
        }
        reference = _run(
            retail_project,
            retail_entry,
            retail_args,
            "HeapAlloc",
            succeeds,
            globals_state,
            {key: int(key, 16) for key in globals_state},
            globals_state,
        )
        candidate_output = _run(
            candidate_project,
            candidate_symbol.rebased_addr,
            [int(key, 16), size, sentinel, mode, threshold, pool_bound],
            "malloc",
            succeeds,
            observed_addresses=candidate_globals,
            initial_globals=globals_state,
        )
        observations.append({
            "id": str(case["id"]),
            "input": case,
            "reference_output": reference,
            "candidate_output": candidate_output,
        })
    return {
        "tool": "angr",
        "version": angr.__version__,
        "engine": "VEX",
        "analysis": "retail x86 and x86 candidate execution with deterministic heap-boundary procedures",
        "binary_path": str(binary.resolve()),
        "candidate_path": str(candidate.resolve()),
        "function_entry": key,
        "cases": observations,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--candidate", type=pathlib.Path, required=True)
    parser.add_argument("--address", required=True)
    args = parser.parse_args()
    cases = json.load(sys.stdin)
    if not isinstance(cases, list) or len(cases) < 8:
        raise ValueError("at least eight allocator cases are required")
    print(json.dumps(execute(args.binary, args.candidate, args.address, cases),
                     separators=(",", ":"), sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
