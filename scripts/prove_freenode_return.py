#!/usr/bin/env python3
"""Retail-only angr experiment: what does FreeNode (0x004B5C90) return on the swap path?

Chains, inside one carried state: pool init (0x004B5E70) -> Resource_CreateNode x3
(0x004B59F0) -> Resource_FreeNode on the middle node. Prints eax per call.
"""
from __future__ import annotations

import json
import os
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
STOP = 0x13370000
NAME_BASE = 0x5C00000


def worker(binary: str) -> dict:
    import angr

    sys.path.insert(0, str(ROOT / "scripts"))
    from angr_abi_execute import ALLOCATED_POINTER, _hook_allocator

    project = angr.Project(binary, auto_load_libs=False)
    _hook_allocator(project, "HeapAlloc", True)

    events: list[tuple] = []

    class FreeData(angr.SimProcedure):
        def run(self, data):
            events.append(("freedata", self.state.solver.eval(data)))
            return 1

    project.hook(0x004B1A30, FreeData(cc=angr.calling_conventions.SimCCCdecl(project.arch),
                                      prototype="int f(void*)"), replace=True)

    def call(state, entry: int, args: list[int]):
        work = state.copy()
        stack = work.solver.eval(work.regs.esp) - 0x400
        work.memory.store(stack, STOP.to_bytes(4, "little"))
        for index, value in enumerate(args):
            work.memory.store(stack + 4 + index * 4, value.to_bytes(4, "little"))
        work.regs.esp = stack
        work.regs.ip = entry
        manager = project.factory.simgr(work)
        manager.explore(find=STOP, num_find=1, n=200000)
        if not manager.found:
            raise RuntimeError(f"call to 0x{entry:08X} did not return: {[str(x.error) for x in manager.errored][:2]}")
        return manager.found[0]

    options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
               angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
    state = project.factory.call_state(0x004B5E70, ret_addr=STOP, add_options=options)

    names = {}
    for index in range(3):
        target = NAME_BASE + index * 0x40
        text = f"LOCAL:HARNESS/FREENODE.{index}\x00".encode()
        state.memory.store(target, text)
        names[index] = target

    meta = 0x5C00100
    data = 0x5C00200

    state = call(state, 0x004B5E70, [])

    def globals_snapshot(st):
        return {
            "pool": st.solver.eval(st.memory.load(0x943E20, 4, endness="Iend_LE")),
            "table": st.solver.eval(st.memory.load(0x943E24, 4, endness="Iend_LE")),
            "count": st.solver.eval(st.memory.load(0x943E28, 4, endness="Iend_LE")),
            "stride": st.solver.eval(st.memory.load(0x943E30, 4, endness="Iend_LE")),
            "first": st.solver.eval(st.memory.load(0x943E38, 4, endness="Iend_LE")),
            "tail": st.solver.eval(st.memory.load(0x943E40, 4, endness="Iend_LE")),
        }

    nodes = []
    for index in range(3):
        state = call(state, 0x004B59F0, [names[index], 0, data + index * 0x10, 8 + index, 2, meta])
        node = state.solver.eval(state.regs.eax)
        active = state.solver.eval(state.memory.load(node + 0x244, 4, endness="Iend_LE"))
        nodes.append(node)
        print(f"CreateNode[{index}] -> node=0x{node:08X} active_idx={active} count={globals_snapshot(state)['count']}")

    results = {"nodes": [f"0x{n:08X}" for n in nodes], "globals_after_setup": globals_snapshot(state)}

    for label, target in (("middle(n1)", nodes[1]), ("last(n2)", nodes[2]), ("first(n0)", nodes[0])):
        before = globals_snapshot(state)
        state = call(state, 0x004B5C90, [target])
        ret = state.solver.eval(state.regs.eax)
        after = globals_snapshot(state)
        mapped = "count" if ret == after["count"] else (f"node_ptr(0x{ret:08X})" if ret in nodes else f"other(0x{ret:08X})")
        moved = state.solver.eval(state.memory.load(nodes[2] + 0x244, 4, endness="Iend_LE"))
        print(f"FreeNode({label} @0x{target:08X}) ret={mapped} count {before['count']}->{after['count']} n2.active_idx->{moved}")
        results[label] = {"ret_raw": f"0x{ret:08X}", "ret_kind": mapped,
                          "count_before": before["count"], "count_after": after["count"]}

    results["events"] = [list(e) for e in events]
    results["alloc_pointer"] = ALLOCATED_POINTER
    return results


def main() -> int:
    binary = str(pathlib.Path(os.environ.get("IGI_RETAIL_EXE", "/Users/haseeb-mir/Downloads/IGI/igi.exe")))
    try:
        import angr  # noqa: F401
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(ROOT / ".venv-angr/bin/python"))
        return subprocess.call([interpreter, __file__])
    print(json.dumps(worker(binary), indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
