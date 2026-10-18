#!/usr/bin/env python3
"""Compare three frame accessors using retail and freshly compiled PE32 code.

The private worker imports angr only in the isolated analysis interpreter.
This does not exercise QTask_OpenEvent or the game process.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import subprocess
import sys
import tempfile

TARGETS = {
    "0x00402880": "Game_GetContext",
    "0x004028A0": "Flow_SetState",
    "0x0048F480": "Loop_SetStatusByte",
}
CONTEXT = 0x60002000
STOP = 0x13370000


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def worker(binary, candidate, address):
    import angr

    original = angr.Project(str(binary), auto_load_libs=False)
    rebuilt = angr.Project(str(candidate), auto_load_libs=False)
    for project in (original, rebuilt):
        if project.arch.name != "X86" or project.arch.bits != 32:
            raise RuntimeError("both inputs must be x86 PE32")
    if original.loader.main_object.mapped_base != 0x400000:
        raise RuntimeError("unexpected retail image base")

    def symbol(name):
        result = rebuilt.loader.find_symbol(name)
        if result is None:
            raise RuntimeError(f"candidate symbol missing: {name}")
        return result.rebased_addr

    candidate_entry = symbol(TARGETS[address])
    candidate_pointer = symbol("DAT_00567C8C")
    candidate_status = symbol("DAT_005C8DE8")
    values = (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF,
              0x60002000, 0x12345678, 0xA5A5A5A5)
    if address == "0x0048F480":
        values = range(256)

    def execute(project, entry, pointer_global, status_global, value):
        pointer = value if address == "0x00402880" else CONTEXT
        arguments = [] if address == "0x00402880" else [value]
        state = project.factory.call_state(
            entry, *arguments, ret_addr=STOP,
            add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                         angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
        state.memory.store(pointer_global, pointer.to_bytes(4, "little"))
        state.memory.store(status_global, b"\xA5")
        canaries = bytes((i * 17 + 3) & 255 for i in range(64))
        state.memory.store(CONTEXT, canaries)
        stack = state.solver.eval(state.regs.esp)
        effects = []

        def observe_write(current):
            location = current.solver.eval(current.inspect.mem_write_address)
            length = current.solver.eval(current.inspect.mem_write_length)
            if stack - 0x10000 <= location < stack + 0x10000:
                return
            if CONTEXT <= location < CONTEXT + 64:
                name = f"context+0x{location - CONTEXT:X}"
            elif location == pointer_global:
                name = "context-pointer-global"
            elif location == status_global:
                name = "loop-status-global"
            else:
                name = f"unexpected:0x{location:08X}"
            content = current.solver.eval(
                current.memory.load(location, length), cast_to=bytes).hex()
            effects.append({"location": name, "bytes": content})

        state.inspect.b("mem_write", when=angr.BP_AFTER, action=observe_write)
        manager = project.factory.simgr(state)
        manager.explore(find=STOP, num_find=1, n=1000)
        if len(manager.found) != 1 or manager.errored or manager.active or manager.unconstrained:
            raise RuntimeError(f"incomplete execution at 0x{entry:08X}")
        final = manager.found[0]
        before = int.from_bytes(canaries, "big")
        after = final.solver.eval(final.memory.load(CONTEXT, 64))
        return {
            # EAX is not an observable result of the two void setters.
            "return_value": final.solver.eval(final.regs.eax) if address == "0x00402880" else 0,
            "state_delta": {
                "context_before": before.to_bytes(64, "big").hex(),
                "context_after": after.to_bytes(64, "big").hex(),
                "context_pointer": final.solver.eval(final.memory.load(pointer_global, 4, endness=project.arch.memory_endness)),
                "loop_status": final.solver.eval(final.memory.load(status_global, 1)),
            },
            "side_effects": effects,
        }

    cases = []
    for value in values:
        reference = execute(original, int(address, 16), 0x567C8C, 0x5C8DE8, value)
        observed = execute(rebuilt, candidate_entry, candidate_pointer, candidate_status, value)
        cases.append({"id": f"value-{value:08X}", "input": {"value": value},
                      "categories": ["boundary" if value in (0, 1, 255, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF) else "nominal"],
                      "reference_output": reference, "candidate_output": observed})
    return {"cases": cases, "angr_version": angr.__version__}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True, choices=TARGETS)
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--implementation", type=pathlib.Path)
    parser.add_argument("--binary-sha256")
    parser.add_argument("--implementation-sha256")
    parser.add_argument("--root", type=pathlib.Path)
    parser.add_argument("--worker-candidate", type=pathlib.Path, help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.worker_candidate:
        print(json.dumps(worker(args.binary, args.worker_candidate, args.address)))
        return 0
    if not args.root or not args.implementation:
        parser.error("--root and --implementation are required")
    root, source, binary = args.root.resolve(), args.implementation.resolve(), args.binary.resolve()
    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise RuntimeError("binary or implementation hash changed")
    runner_hash = digest(pathlib.Path(__file__).resolve())
    analysis_python = os.environ.get("IGI_ANGR_PYTHON") or str(pathlib.Path(
        os.environ.get("RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/auto-re-agent")) / ".venv/bin/python")
    with tempfile.TemporaryDirectory(prefix="igi-frame-diff-") as temp:
        stub, candidate = pathlib.Path(temp) / "unused_dispatch_table.cpp", pathlib.Path(temp) / "candidate.dll"
        stub.write_text('#include "igi/retail/core/frame_natives.hpp"\nextern "C" { igi::retail::core::FrameDispatchCallback DAT_00A970E0[1] = {}; }\n')
        command = ["i686-w64-mingw32-g++", "-std=c++20", "-O0", "-shared",
                   "-Wl,--export-all-symbols", f"-I{root / 'engine/include'}", str(source), str(stub), "-o", str(candidate)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=60)
        candidate_hash = digest(candidate)
        result = subprocess.run([analysis_python, str(pathlib.Path(__file__).resolve()),
                                 "--address", args.address, "--binary", str(binary),
                                 "--worker-candidate", str(candidate)],
                                check=True, capture_output=True, text=True, timeout=240)
        output = json.loads(result.stdout)
    cases = output["cases"]
    expected_values = range(256) if args.address == "0x0048F480" else (
        0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x60002000, 0x12345678, 0xA5A5A5A5)
    if [case["id"] for case in cases] != [f"value-{value:08X}" for value in expected_values]:
        raise RuntimeError("worker returned an incomplete or unexpected fixture set")
    if (digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256
            or digest(pathlib.Path(__file__).resolve()) != runner_hash):
        raise RuntimeError("binary, implementation or runner changed during comparison")
    failed = sum(c["reference_output"] != c["candidate_output"] for c in cases)
    report = {
        "schema_version": 1, "kind": "behavioral-test-report",
        "result": "passed" if not failed else "failed", "executed": True,
        "oracle": "retail-binary-execution", "address": args.address,
        "binary_sha256": args.binary_sha256, "implementation_sha256": args.implementation_sha256,
        "method": "Retail and freshly compiled PE32 x86 native entries executed with angr VEX; measured return, context bytes, global values and non-stack memory writes.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases), "tests_passed": len(cases) - failed,
        "tests_failed": failed, "cases": cases,
        "runner": {"path": "scripts/frame_diff_runner.py", "sha256": runner_hash},
        "retail_execution": {"tool": "angr", "version": output["angr_version"],
                             "engine": "VEX", "candidate_sha256": candidate_hash,
                             "candidate_build_command": command},
        "limitations": ["Concrete emulator fixtures, not a running retail process or a proof of all inputs.",
                        "Stack writes are excluded; original and candidate stack frame layouts can differ.",
                        "Void setter EAX values are intentionally ignored.",
                        "QTask_OpenEvent and its dispatch table are not executed."],
    }
    print(json.dumps(report))
    return 1 if failed else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, subprocess.SubprocessError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
