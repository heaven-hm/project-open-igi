#!/usr/bin/env python3
"""Differential replay of Transcontext_BuildViewBlock (0x00497D70) and Display_ComputeScreenAspect (0x00491D40).

Both retail bodies are branch-free. The aspect factor reads the display mode (width at +4, height at +8) and leaves
(4 * height) / (3 * width) in x87 st0 as the double return value. The view-block builder copies and transposes a
40-byte block, copies three camera float64 coordinates, writes fovX at +0x40, then derives the vertical and screen
FOVs plus the block origin from the registered screen block (retail 0x00BCABA0, fields at +0x18..+0x2C) and returns 1.

Retail reaches the display mode through Display_GetActiveMode (0x00491CF0), which the harness hooks to the retail
display object (0x00C28B40); the candidate reads its host mirrors instead, so the harness installs the equivalent
state on each side. Floating-point results are compared as raw IEEE bit patterns: strict, and no tolerance choice.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import logging
import os
import pathlib
import re
import struct
import subprocess
import sys

STOP_ADDRESS = 0x13370000
CONTEXT_ADDRESS = 0x6000
VIEW_ADDRESS = 0x6400
CAMERA_ADDRESS = 0x6500
RETAIL_DISPLAY = 0x00C28B40
RETAIL_SCREEN_BLOCK = 0x00BCABA0
RETAIL_GET_ACTIVE_MODE = 0x00491CF0
RETAIL_ASPECT = 0x00491D40
RETAIL_BUILD_VIEW_BLOCK = 0x00497D70

ASPECT_CASES = (
    (640, 480), (800, 600), (1024, 768), (1600, 1200),
    (1280, 1024), (320, 200), (1920, 1080), (400, 300),
)

# (origin_x, origin_y, width, height, fov_x, view_seed, camera_scale)
BUILD_CASES = (
    (0.0, 0.0, 640.0, 480.0, 1.0, 1, 1.0),
    (10.0, 20.0, 800.0, 600.0, 0.5, 2, 2.0),
    (-32.0, 48.0, 1024.0, 768.0, 1.25, 3, 0.5),
    (0.0, 0.0, 1280.0, 1024.0, 0.75, 4, 4.0),
    (5.0, 7.0, 1600.0, 1200.0, 0.25, 6, 3.0),
    (12.5, -7.5, 400.0, 300.0, 1.5, 8, 2.5),
    (100.0, -100.0, 320.0, 200.0, 2.0, 5, -1.0),
    (-1.0, 1.0, 1920.0, 1080.0, 3.0, 7, 0.25),
)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def bits(value: float, size: int) -> str:
    return struct.pack("<f" if size == 4 else "<d", value).hex()


def candidate_symbols(executable: pathlib.Path) -> dict[str, int]:
    wanted = {
        "aspect": "Display_ComputeScreenAspect",
        "build": "Transcontext_BuildViewBlock",
        "display_mode": "g_display_mode",
        "screen_block": "g_screen_block",
    }
    result = subprocess.run(["i686-w64-mingw32-nm", "-C", str(executable)],
                            capture_output=True, text=True, timeout=600, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"could not read candidate symbols: {result.stderr[-300:]}")
    found: dict[str, int] = {}
    for parts in (line.split(maxsplit=2) for line in result.stdout.splitlines()):
        if len(parts) < 3:
            continue
        base = re.sub(r"\([^()]*\)$", "", parts[2].strip())
        for label, name in wanted.items():
            if label in found:
                continue
            if base == name or base.endswith("::" + name) or base.endswith(name):
                found[label] = int(parts[0], 16)
    missing = set(wanted) - set(found)
    if missing:
        raise RuntimeError(f"candidate build does not expose {sorted(missing)}")
    return found


def install_display_mode(state, address: int, width: int, height: int) -> None:
    state.memory.store(address + 4, struct.pack("<i", width))
    state.memory.store(address + 8, struct.pack("<i", height))


def install_screen_block(state, base: int, fields: tuple[float, ...]) -> None:
    # +0x18 originX, +0x1C originY, +0x20 width, +0x24 height, +0x28 halfWidth, +0x2C halfHeight.
    for index, value in enumerate(fields):
        state.memory.store(base + 0x18 + index * 4, struct.pack("<f", value))


def read_context(state, endness) -> dict:
    words = [state.solver.eval(state.memory.load(CONTEXT_ADDRESS + i * 4, 4, endness=endness)) for i in range(10)]
    output = {"block_words": [f"0x{value:08X}" for value in words], "camera_bits": []}
    for index in range(3):
        raw = state.solver.eval(state.memory.load(CONTEXT_ADDRESS + 0x28 + index * 8, 8, endness=endness))
        output["camera_bits"].append(f"0x{raw:016X}")
    for name, offset in (("fov_x", 0x40), ("view_fov_y", 0x44), ("screen_fov_x", 0x48),
                         ("screen_fov_y", 0x4C), ("origin_x", 0x50), ("origin_y", 0x54)):
        raw = state.solver.eval(state.memory.load(CONTEXT_ADDRESS + offset, 4, endness=endness))
        output[name] = f"0x{raw:08X}"
    output["flags"] = [state.solver.eval(state.memory.load(CONTEXT_ADDRESS + offset, 4, endness=endness))
                       for offset in (0x58, 0x60)]
    return output


def make_hook():
    import angr

    class GetActiveMode(angr.SimProcedure):
        def run(self):
            return RETAIL_DISPLAY

    return GetActiveMode()


def run_aspect(project, entry: int, width: int, height: int, candidate: bool, symbols: dict) -> dict:
    import angr

    state = project.factory.call_state(
        entry, ret_addr=STOP_ADDRESS, prototype="double f(void)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    if candidate:
        install_display_mode(state, symbols["display_mode"], width, height)
    else:
        install_display_mode(state, RETAIL_DISPLAY, width, height)
        project.hook(RETAIL_GET_ACTIVE_MODE, make_hook())
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=600)
    if not manager.found:
        raise RuntimeError(f"no completed execution of 0x{entry:08X}")
    return {"return_value": f"0x{manager.found[0].solver.eval(manager.found[0].regs.st0):016X}",
            "state_delta": {}, "side_effects": []}


def run_build(project, entry: int, case: tuple, candidate: bool, symbols: dict) -> dict:
    import angr

    endness = project.arch.memory_endness
    origin_x, origin_y, width, height, fov_x, view_seed, camera_scale = case
    state = project.factory.call_state(
        entry, CONTEXT_ADDRESS, VIEW_ADDRESS, CAMERA_ADDRESS, fov_x, ret_addr=STOP_ADDRESS,
        prototype="int f(void*, void const*, void const*, float)",
        cc=angr.calling_conventions.SimCCCdecl(project.arch),
        add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                     angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
    view = struct.pack("<10I", *[(view_seed * 0x01010101 + index) & 0xFFFFFFFF for index in range(10)])
    camera = struct.pack("<3d", 1.5 * view_seed * camera_scale, -2.5 * camera_scale, 3.5 * view_seed)
    state.memory.store(VIEW_ADDRESS, view)
    state.memory.store(CAMERA_ADDRESS, camera)
    state.memory.store(CONTEXT_ADDRESS, bytes(0x100))
    fields = (origin_x, origin_y, width, height, width * 0.5, height * 0.5)
    if candidate:
        install_screen_block(state, symbols["screen_block"] - 0x18, fields)
        install_display_mode(state, symbols["display_mode"], int(width), int(height))
    else:
        install_screen_block(state, RETAIL_SCREEN_BLOCK, fields)
        install_display_mode(state, RETAIL_DISPLAY, int(width), int(height))
        project.hook(RETAIL_GET_ACTIVE_MODE, make_hook())
    manager = project.factory.simgr(state)
    manager.explore(find=STOP_ADDRESS, n=600)
    if not manager.found:
        raise RuntimeError(f"no completed execution of 0x{entry:08X}")
    final = manager.found[0]
    return {"return_value": final.solver.eval(final.regs.eax),
            "state_delta": read_context(final, endness), "side_effects": ["write:context"]}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--candidate", type=pathlib.Path)
    args = parser.parse_args()

    address = "0x" + args.address[-8:].upper()
    if address not in ("0x00497D70", "0x00491D40"):
        raise SystemExit(f"unsupported native address: {args.address}")
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")
    candidate = (args.candidate or root / "build-win32-verify-noinline" / "igi1.exe").resolve()
    if not candidate.is_file():
        raise SystemExit(f"candidate build is missing: {candidate}")
    try:
        logging.getLogger("angr").setLevel(logging.ERROR)
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])

    symbols = candidate_symbols(candidate)
    is_aspect = address == "0x00491D40"
    specs = ASPECT_CASES if is_aspect else BUILD_CASES
    cases = []
    passed = failed = 0
    for spec in specs:
        if is_aspect:
            width, height = spec
            retail_project = angr.Project(str(binary), auto_load_libs=False)
            candidate_project = angr.Project(str(candidate), auto_load_libs=False)
            reference = run_aspect(retail_project, RETAIL_ASPECT, width, height, False, symbols)
            observed = run_aspect(candidate_project, symbols["aspect"], width, height, True, symbols)
            case_id = f"aspect-{width}x{height}"
            inputs = {"width": width, "height": height}
        else:
            retail_project = angr.Project(str(binary), auto_load_libs=False)
            candidate_project = angr.Project(str(candidate), auto_load_libs=False)
            reference = run_build(retail_project, RETAIL_BUILD_VIEW_BLOCK, spec, False, symbols)
            observed = run_build(candidate_project, symbols["build"], spec, True, symbols)
            case_id = "build-{:g}-{:g}-{:g}x{:g}-fov{:g}".format(*spec[:5])
            inputs = {"origin_x": spec[0], "origin_y": spec[1], "width": spec[2],
                      "height": spec[3], "fov_x": spec[4]}
        matches = reference == observed
        if not matches:
            print(f"mismatch {case_id}: reference={reference} candidate={observed}", file=sys.stderr)
        passed, failed = (passed + 1, failed) if matches else (passed, failed + 1)
        if is_aspect:
            categories = ["nominal"] if spec[0] * 3 == spec[1] * 4 else ["boundary"]
        else:
            categories = ["boundary"] if spec[0] < 0 or spec[1] < 0 else ["nominal"]
        cases.append({"id": case_id,
                      "input": {"args": [], "initial_state": inputs},
                      "categories": categories,
                      "reference_output": reference,
                      "candidate_output": observed})

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 and passed == len(cases) else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": ("Retail and the compiled candidate run the same body over identical display-mode and screen-block "
                   "state; the aspect factor is compared as the raw x87 st0 bit pattern and the view block as the raw "
                   "bits of every field the body writes, so no floating-point tolerance is involved."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "retail_execution": {"tool": "angr", "engine": "VEX",
                             "version": getattr(angr, "__version__", "unknown"),
                             "function_entry": address,
                             "binary_sha256": args.binary_sha256,
                             "candidate_sha256": digest(candidate)},
        "cases": cases,
    }
    json.dump(report, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if report["result"] == "passed" else 1


if __name__ == "__main__":
    raise SystemExit(main())
