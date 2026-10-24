#!/usr/bin/env python3
# Execute retail igi.exe and an x86 candidate DLL for the WeaponGun_Invoke5 natives with angr/VEX.
# Retail sprintf (0x004A53B3) and the 0x00A96AE0 handler table are replaced by deterministic
# boundary procedures that record their calls. Weapon_ComputeAccuracy_5 (0x00477BF0) runs as
# real retail code reading the word at 0x005BE3C6.
from __future__ import annotations

import argparse
import json
import pathlib
import sys

ENTRIES = {"0x00473AB0": 0x00473AB0, "0x00472E10": 0x00472E10,
           "0x004734D0": 0x004734D0, "0x00475460": 0x00475460}
STOP = 0x13370000
STUB_BASE = 0x13380000
TABLE = 0x00A96AE0
ACCURACY = 0x005BE3C6
SPRINTF = 0x004A53B3
BUF = 0x61000000
BUF_SIZE = 0x300
SENT = {"ebx": 0x11111111, "esi": 0x22222222, "edi": 0x33333333, "ebp": 0x44444444}
INDEXES = [0, 1, 2, 3, 7, 0x100, 0xFFFF]
ARG2S = [0, 1, 0x7FFFFFFF, 0xFFFFFFFF]


def make_procs(angr):
    class Sprintf(angr.SimProcedure):
        num_args = 2

        def run(self, buf, fmt):
            addr = self.state.solver.eval(fmt)
            text = self.state.solver.eval(self.state.memory.load(addr, 64), cast_to=bytes)
            text = text.split(bytes(1))[0]
            dest = self.state.solver.eval(buf)
            self.state.memory.store(dest, text + bytes(1))
            ev = list(self.state.globals.get("events", []))
            ev.append(["sprintf", dest - BUF, text.decode("latin1")])
            self.state.globals["events"] = ev
            return len(text)

    class Handler(angr.SimProcedure):
        num_args = 2
        slot = 0

        def run(self, a1, a2):
            ev = list(self.state.globals.get("events", []))
            ev.append(["handler", self.slot, self.state.solver.eval(a1) - BUF,
                       self.state.solver.eval(a2)])
            self.state.globals["events"] = ev
            return 0xDEAD0000 + self.slot

    return Sprintf, Handler


def hook_all(project, sprintf_addr):
    import angr
    from angr.sim_type import SimTypeFunction, SimTypeInt
    sprintf_cls, handler_cls = make_procs(angr)
    cdecl = angr.calling_conventions.SimCCCdecl(project.arch)
    proto = SimTypeFunction([SimTypeInt(False, 32)] * 2, SimTypeInt(False, 32)).with_arch(project.arch)
    proc = sprintf_cls()
    proc.cc = cdecl
    proc.prototype = proto
    project.hook(sprintf_addr, proc, replace=True)
    for k in range(len(INDEXES)):
        cls = type("Handler%d" % k, (handler_cls,), {"slot": k})
        proc = cls()
        proc.cc = cdecl
        proc.prototype = proto
        project.hook(STUB_BASE + 0x10 * k, proc, replace=True)


def run_case(project, entry, table, accuracy, idx, word, arg2):
    import angr
    state = project.factory.call_state(
        entry, BUF, arg2, ret_addr=STOP,
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    for name, value in SENT.items():
        setattr(state.regs, name, value)
    state.regs.eax = 0xA5A50000
    pattern = bytes((i * 7 + 3) & 0xFF for i in range(BUF_SIZE))
    pattern = pattern[:0x1B0] + (0xCAFEBABE).to_bytes(4, "little") + pattern[0x1B4:]
    state.memory.store(BUF, pattern)
    little = project.arch.memory_endness
    for k, i in enumerate(INDEXES):
        state.memory.store(table + 4 * i, state.solver.BVV(STUB_BASE + 0x10 * k, 32), endness=little)
    state.memory.store(accuracy, state.solver.BVV(word, 16), endness=little)
    esp0 = state.solver.eval(state.regs.esp)
    manager = project.factory.simgr(state)
    manager.explore(find=STOP, num_find=1, n=20000)
    if not manager.found:
        raise RuntimeError("function 0x%08X did not return: %s" % (entry, [str(e.error) for e in manager.errored][:2]))
    final = manager.found[0]
    ev = lambda r: final.solver.eval(getattr(final.regs, r))
    return {
        "return_value": ev("eax"),
        "esp_delta": ev("esp") - esp0,
        "preserved": {name: ev(name) for name in SENT},
        "buffer": final.solver.eval(final.memory.load(BUF, BUF_SIZE), cast_to=bytes).hex(),
        "events": [list(e) for e in final.globals.get("events", [])],
    }


def cases():
    return [{"id": "idx%04x-arg%08x" % (i, a), "index": i, "arg2": a}
            for i in INDEXES for a in ARG2S]


def execute(binary, candidate, address):
    import angr
    key = "0x" + address[-8:].upper()
    if key not in ENTRIES:
        raise ValueError("unsupported native " + key)
    retail = angr.Project(str(binary.resolve()), auto_load_libs=False)
    cand = angr.Project(str(candidate.resolve()), auto_load_libs=False)
    for p in (retail, cand):
        if p.arch.name != "X86" or p.arch.bits != 32:
            raise RuntimeError("both binaries must load as PE32 x86")
    entry_symbol = cand.loader.find_symbol("NativeEntry")
    table_symbol = cand.loader.find_symbol("g_a96ae0")
    word_symbol = cand.loader.find_symbol("g_5be3c6")
    sprintf_symbol = cand.loader.find_symbol("sprintf")
    if not (entry_symbol and table_symbol and word_symbol and sprintf_symbol):
        raise RuntimeError("candidate DLL must export NativeEntry, g_a96ae0, g_5be3c6 and import sprintf")
    hook_all(retail, SPRINTF)
    hook_all(cand, sprintf_symbol.rebased_addr)
    out = []
    for case in cases():
        word = case["index"]
        ref = run_case(retail, ENTRIES[key], TABLE, ACCURACY, case["index"], word, case["arg2"])
        got = run_case(cand, entry_symbol.rebased_addr, table_symbol.rebased_addr,
                       word_symbol.rebased_addr, case["index"], word, case["arg2"])
        out.append({"id": case["id"], "input": case, "reference_output": ref, "candidate_output": got})
    block = retail.factory.block(ENTRIES[key])
    return {"tool": "angr", "version": angr.__version__, "engine": "VEX",
            "analysis": "retail PE32 igi.exe and x86 candidate DLL executed under angr with sprintf and handler-table boundary procedures",
            "binary_path": str(binary.resolve()), "candidate_path": str(candidate.resolve()),
            "function_entry": key, "entry_block": [str(i) for i in block.capstone.insns],
            "cases": out}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--candidate", type=pathlib.Path, required=True)
    parser.add_argument("--address", required=True)
    args = parser.parse_args()
    print(json.dumps(execute(args.binary, args.candidate, args.address), separators=(",", ":"), sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
