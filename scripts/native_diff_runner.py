#!/usr/bin/env python3
"""Compare selected reconstructed natives with retail x86 ESIL execution.

App Context setters are checked over their full byte input domain. Model
attachment accessors execute both the retail body and a freshly compiled x86
candidate object in Radare2 ESIL, using identical synthetic x86 memory layouts.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile


NATIVES = {
    "0x0048F1A0": ("App_ContextSetDebugged", "debugged", "0x005C8BF8"),
    "0x0048F1E0": ("App_ContextSetDebugtextState", "debug_text_state", "0x005C8BF4"),
    "0x0048F240": ("App_ContextSetLightmapsUsed", "lightmaps_used", "0x005C8BE4"),
    "0x0048F260": ("App_ContextSetTerrainLightmapsUsed", "terrain_lightmaps_used", "0x005C8BE8"),
}

GETTERS = {
    "0x0048F1B0": ("App_ContextGetDebugged", "debugged", "0x005C8BF8"),
    "0x0048F230": ("App_ContextGetLightmapsUsed", "lightmaps_used", "0x005C8BE4"),
    "0x0048F250": ("App_ContextGetTerrainLightmapsUsed", "terrain_lightmaps_used", "0x005C8BE8"),
}

CONTEXT_ACCESSOR_ADDRESS = "0x00402880"
CONTEXT_STATE_SETTER_ADDRESS = "0x004028A0"
LOOP_STATUS_SETTER_ADDRESS = "0x0048F480"
STRING_FIND_EXTENSION_ADDRESS = "0x004B1E90"
CONTEXT_GLOBAL_ADDRESS = "0x00567c8c"
MODEL_GET_ATTACHMENT_ADDRESS = "0x004D0CF0"
MODEL_GET_ATTACHMENT_COUNT_ADDRESS = "0x004D0D10"
MODEL_RECORD = 0x60000000
RESOURCE_RECORD = 0x60001000
MESH_HEADER = 0x60002000
ATTACHMENT_BASE = 0x60003000


def digest(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def signed32(value: int) -> int:
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


def run_string_find_extension(args: argparse.Namespace, binary: pathlib.Path,
                              implementation: pathlib.Path, root: pathlib.Path) -> int:
    """Compare the retail function (including its retail copy callee) with this candidate."""
    cases_to_run = [
        ("extension-one-byte-dst", "model.mef", 1, False, "boundary"),
        ("extension-two-byte-dst", "model.mef", 2, False, "boundary"),
        ("extension-short-dst", "model.mef", 5, False, "boundary"),
        ("extension-full-dst", "model.mef", 16, False, "nominal"),
        ("no-extension", "model", 12, False, "edge"),
        ("last-of-multiple-dots", "archive.tar.gz", 16, False, "nominal"),
        ("leading-dot", ".hidden", 12, False, "edge"),
        ("trailing-dot", "model.", 8, False, "edge"),
        ("directory-dot-search", "folder.with.dot/model", 24, False, "edge"),
        ("null-destination", "unit.qvm", 0, True, "boundary"),
        ("small-file-dst", "x.qvm", 3, False, "boundary"),
        ("longer-extension", "terrain.texture.package", 32, False, "nominal"),
    ]
    # The same interpreter the gate uses: the project's own angr environment,
    # then the re-agent checkout, then whatever IGI_ANGR_PYTHON names.
    angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
    if not angr_python.is_file():
        angr_python = pathlib.Path(__file__).resolve().parent.parent / ".venv-angr/bin/python"
    if not angr_python.is_file():
        re_agent = pathlib.Path(os.environ.get("RE_AGENT_ROOT", "/Users/haseeb-mir/Documents/Code/re-agent"))
        angr_python = re_agent / ".venv/bin/python"
    if not angr_python.is_file():
        raise RuntimeError("String_FindExtension requires IGI_ANGR_PYTHON with angr installed")

    harness = r'''#include "igi/retail/script/qvm_native_loader.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
int main(int argc, char** argv) {
    if (argc != 4) return 2;
    const std::string path = argv[1];
    const int length = std::atoi(argv[2]);
    const bool null_destination = std::atoi(argv[3]) != 0;
    char storage[128];
    for (char& byte : storage) byte = static_cast<char>(0xa5);
    char* destination = null_destination ? nullptr : storage + 32;
    char* result = igi::retail::script::String_FindExtension(destination, length,
                                                              const_cast<char*>(path.c_str()));
    std::printf("%td ", result - path.c_str());
    for (unsigned char byte : storage) std::printf("%02x", byte);
    std::printf("\n");
    return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="igi-string-extension-") as temp:
        tempdir = pathlib.Path(temp)
        harness_path = tempdir / "candidate_runner.cpp"
        candidate = tempdir / "candidate"
        harness_path.write_text(harness, encoding="utf-8")
        dead_section_flag = "-Wl,-dead_strip" if sys.platform == "darwin" else "-Wl,--gc-sections"
        built = subprocess.run(
            [args.cxx, "-std=c++20", "-ffunction-sections", "-fdata-sections",
             f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
             dead_section_flag, "-o", str(candidate)],
            cwd=root, text=True, capture_output=True, check=False, timeout=60,
        )
        if built.returncode != 0:
            raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")

        angr_cases = [
            {"id": case_id, "path": path, "max_len": max_len,
             "null_destination": null_destination}
            for case_id, path, max_len, null_destination, _ in cases_to_run
        ]
        helper = root / "scripts/angr_execute_native.py"
        retail = subprocess.run(
            [str(angr_python), str(helper), "--binary", str(binary), "--address",
             STRING_FIND_EXTENSION_ADDRESS],
            cwd=root, input=json.dumps(angr_cases), text=True, capture_output=True,
            check=False, timeout=180,
        )
        if retail.returncode != 0:
            raise RuntimeError(f"angr retail execution failed: {retail.stderr[-3000:]}")
        try:
            retail_observations = json.loads(retail.stdout)
        except json.JSONDecodeError as error:
            raise RuntimeError(f"angr runtime helper returned invalid JSON: {error}; {retail.stdout[-1000:]}") from error
        if not isinstance(retail_observations, dict) or retail_observations.get("tool") != "angr":
            raise RuntimeError("angr runtime helper returned an unsupported evidence object")
        if retail_observations.get("function_entry") != STRING_FIND_EXTENSION_ADDRESS:
            raise RuntimeError("angr runtime helper analyzed a different function entry")
        retail_cases = retail_observations.get("cases")
        if not isinstance(retail_cases, list) or len(retail_cases) != len(cases_to_run):
            raise RuntimeError("angr runtime helper returned a different number of cases")

        cases = []
        for index, (case_id, path, max_len, null_destination, category) in enumerate(cases_to_run):
            actual = subprocess.run(
                [str(candidate), path, str(max_len), "1" if null_destination else "0"],
                text=True, capture_output=True, check=False, timeout=5,
            )
            if actual.returncode != 0:
                raise RuntimeError(f"candidate failed for {case_id}: {actual.stderr.strip()}")
            try:
                candidate_offset, candidate_hex = actual.stdout.strip().split()
                candidate_offset = int(candidate_offset, 10)
            except ValueError as error:
                raise RuntimeError(f"candidate output malformed for {case_id}: {actual.stdout!r}") from error
            candidate_bytes = bytes.fromhex(candidate_hex)
            changed = [] if null_destination else [
                i for i, value in enumerate(candidate_bytes) if value != 0xa5
            ]
            candidate_output = {
                "return_value": candidate_offset,
                "state_delta": changed,
                "side_effects": [] if null_destination else ["write:destination-buffer"],
                "destination_hex": candidate_hex,
            }
            reference = retail_cases[index]
            reference_output = {
                "return_value": reference["return_offset"],
                "state_delta": reference["changed_offsets"],
                "side_effects": reference["side_effects"],
                "destination_hex": reference["destination_hex"],
            }
            cases.append({
                "id": case_id,
                "input": {"args": [path, max_len, null_destination],
                          "initial_state": {"destination_fill": "0xa5", "destination_offset": 32}},
                "categories": [category],
                "reference_output": reference_output,
                "candidate_output": candidate_output,
            })

    failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
    report = {
        "schema_version": 1, "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 else "failed", "executed": True,
        "oracle": "retail-binary-execution",
        "address": STRING_FIND_EXTENSION_ADDRESS,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": "The retail x86 function at 0x004B1E90 and its in-binary copy callee at 0x004A5500 were executed by angr's VEX engine from the exact retail PE; the current C++ candidate was compiled and called with identical strings and destination lengths. Return pointers were normalized to offsets from the input path; the complete guarded destination buffer was compared. This is binary emulation, not a live game process.",
        "observable_fields": ["return_value", "state_delta", "side_effects", "destination_hex"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases), "tests_passed": len(cases) - failed,
        "tests_failed": failed, "cases": cases,
        "retail_execution": {
            "engine": retail_observations.get("engine"),
            "tool": retail_observations.get("tool"),
            "version": retail_observations.get("angr_version"),
            "analysis": retail_observations.get("analysis"),
            "function_entry": retail_observations.get("function_entry"),
            "helper_path": helper.relative_to(root).as_posix(),
            "helper_sha256": digest(helper),
            "binary_path": str(binary),
            "binary_sha256": args.binary_sha256,
        },
        "limitations": ["Tested positive destination lengths only; invalid pointers and non-positive lengths are not exercised.",
                        "angr VEX execution is not a live retail process and can model instructions imperfectly."],
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failed == 0 else 1


def retail_context_observation(r2: str, binary: pathlib.Path, pointer: int) -> int:
    stack = "0x70008000"
    commands = ";".join((
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x00560000 0x10000 globals",
        f"aer esp={stack}",
        f"ae 0x{pointer:08x},{CONTEXT_GLOBAL_ADDRESS},=[4]",
        f"ae 0xdeadbeef,{stack},=[4]",
        f"s {CONTEXT_ACCESSOR_ADDRESS}",
        f"aer eip={CONTEXT_ACCESSOR_ADDRESS}",
        "aes", "aes",
        "aer eax",
    ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", commands, str(binary)],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if not values:
        raise RuntimeError(f"Could not read ESIL EAX output: {process.stdout!r} {process.stderr!r}")
    return int(values[-1], 0)


def retail_context_state_observation(r2: str, binary: pathlib.Path, value: int, initial: int) -> tuple[int, int]:
    stack = "0x70008000"
    context = "0x60000000"
    commands = ";".join((
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x00560000 0x10000 globals",
        "aeim 0x60000000 0x1000 context",
        f"aer esp={stack}",
        f"ae 0xdeadbeef,{stack},=[4]",
        f"ae 0x{value:08x},0x70008004,=[4]",
        f"ae {context},{CONTEXT_GLOBAL_ADDRESS},=[4]",
        f"ae 0x{initial:08x},0x6000002c,=[4]",
        f"s {CONTEXT_STATE_SETTER_ADDRESS}",
        f"aer eip={CONTEXT_STATE_SETTER_ADDRESS}",
        "aes", "aes", "aes", "aes",
        "aer eax",
        "ae 0x6000002c,[4]",
    ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", commands, str(binary)],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if not values:
        raise RuntimeError(f"Could not read ESIL context field: {process.stdout!r} {process.stderr!r}")
    return signed32(int(values[-2], 0)), int(values[-1], 0)


def retail_loop_status_observation(r2: str, binary: pathlib.Path, value: int, initial: int) -> tuple[int, int]:
    stack = "0x70008000"
    global_address = "0x005c8de8"
    commands = ";".join((
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x005c0000 0x10000 globals",
        f"aer esp={stack}",
        "aer eax=0xa5a5a5a5",
        f"ae 0x{initial:02x},{global_address},=[1]",
        f"ae 0x{value:02x},0x70008004,=[1]",
        f"s {LOOP_STATUS_SETTER_ADDRESS}",
        f"aer eip={LOOP_STATUS_SETTER_ADDRESS}",
        "aes", "aes", "aes",
        "aer eax",
        f"ae {global_address},[1]",
    ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", commands, str(binary)],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if len(values) < 2:
        raise RuntimeError(f"Could not read ESIL EAX/global outputs: {process.stdout!r} {process.stderr!r}")
    eax, state = (int(value, 0) for value in values[-2:])
    return eax & 0xFF, state & 0xFF


def retail_observation(r2: str, binary: pathlib.Path, address: str, global_address: str, value: int) -> dict:
    stack = "0x70008000"
    commands = ";".join((
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x005c0000 0x10000 globals",
        f"aer esp={stack}",
        f"ae 0,{global_address},=[4]",
        f"ae 0x{value:02x},0x70008004,=[1]",
        f"s {address}",
        "aeip",
        "aes", "aes", "aes",
        "aer eax",
        f"ae {global_address},[4]",
    ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", commands, str(binary)],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if len(values) < 2:
        raise RuntimeError(f"Could not read ESIL EAX and global outputs: {process.stdout!r} {process.stderr!r}")
    eax, stored = (int(v, 0) for v in values[-2:])
    return {
        "return_value": signed32(eax),
        "state_delta": signed32(stored),
        "side_effects": [f"write:{global_address.upper()}"],
    }


def retail_observations_batch(
    r2: str, binary: pathlib.Path, address: str, global_address: str, values: range,
) -> list[dict]:
    """Execute independent byte-input cases in one ESIL process.

    Each invocation resets the argument byte and target global before entering
    the retail function. The four callers of this helper are leaf setters whose
    selected binary bodies touch only that global and return; repeating the
    existing three-instruction trace in one emulator avoids hundreds of r2
    process startups without changing the tested inputs or observations.
    """
    stack = 0x70008000
    commands = [
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x005c0000 0x10000 globals",
    ]
    for value in values:
        commands.extend((
            f"aer esp=0x{stack:08x}",
            f"ae 0,{global_address},=[4]",
            f"ae 0x{value:02x},0x{stack + 4:08x},=[1]",
            f"s {address}",
            "aeip",
            "aes", "aes", "aes",
            "aer eax",
            f"ae {global_address},[4]",
        ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", ";".join(commands), str(binary)],
        text=True, capture_output=True, check=False, timeout=120,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 batched ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values_out = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if len(values_out) != len(values) * 2:
        raise RuntimeError(
            f"Radare2 batched ESIL returned {len(values_out)} values for {len(values)} cases; "
            f"stdout tail={process.stdout[-1000:]!r}"
        )
    observations = []
    for index in range(0, len(values_out), 2):
        eax, stored = (int(item, 0) for item in values_out[index:index + 2])
        observations.append({
            "return_value": signed32(eax),
            "state_delta": signed32(stored),
            "side_effects": [f"write:{global_address.upper()}"],
        })
    return observations


def retail_getter_observation(r2: str, binary: pathlib.Path, address: str, global_address: str, value: int) -> int:
    stack = "0x70008000"
    commands = ";".join((
        "aei",
        "aeim 0x70000000 0x10000 stack",
        "aeim 0x005c0000 0x10000 globals",
        f"aer esp={stack}",
        f"ae 0x{value & 0xffffffff:08x},{global_address},=[4]",
        f"s {address}",
        f"aer eip={address}",
        "aes", "aes",
        "aer eax",
    ))
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", commands, str(binary)],
        text=True, capture_output=True, check=False, timeout=10,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    if not values:
        raise RuntimeError(f"Could not read ESIL getter result: {process.stdout!r} {process.stderr!r}")
    return signed32(int(values[-1], 0))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--r2", default="/opt/homebrew/bin/r2")
    parser.add_argument("--cxx", default="c++")
    args = parser.parse_args()

    address = "0x" + args.address[-8:].upper()
    if address not in (
        CONTEXT_ACCESSOR_ADDRESS, CONTEXT_STATE_SETTER_ADDRESS, LOOP_STATUS_SETTER_ADDRESS,
        MODEL_GET_ATTACHMENT_ADDRESS, MODEL_GET_ATTACHMENT_COUNT_ADDRESS, STRING_FIND_EXTENSION_ADDRESS,
    ) and address not in NATIVES and address not in GETTERS:
        raise SystemExit(f"unsupported native address: {args.address}")
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    if address == STRING_FIND_EXTENSION_ADDRESS:
        raise SystemExit(run_string_find_extension(args, binary, implementation, root))

    if address in (MODEL_GET_ATTACHMENT_ADDRESS, MODEL_GET_ATTACHMENT_COUNT_ADDRESS):
        x86_cxx = "i686-w64-mingw32-g++"
        with tempfile.TemporaryDirectory(prefix="igi-native-x86-candidate-") as temp:
            tempdir = pathlib.Path(temp)
            candidate_object = tempdir / "candidate.o"
            built = subprocess.run(
                [x86_cxx, "-std=c++20", "-O0", f"-I{root / 'engine/include'}",
                 "-c", str(implementation), "-o", str(candidate_object)],
                cwd=root, text=True, capture_output=True, check=False, timeout=45,
            )
            if built.returncode != 0:
                raise RuntimeError(f"x86 candidate compilation failed: {built.stderr[-4000:]}")
            nm = subprocess.run(
                ["i686-w64-mingw32-nm", "-C", str(candidate_object)],
                text=True, capture_output=True, check=False, timeout=10,
            )
            if nm.returncode != 0:
                raise RuntimeError(f"could not inspect x86 candidate object: {nm.stderr.strip()}")
            symbol = (
                "Model_GetAttachmentCount" if address == MODEL_GET_ATTACHMENT_COUNT_ADDRESS
                else "Model_GetAttachment("
            )
            match = re.search(r"(?m)^([0-9a-fA-F]+)\s+T\s+.*" + re.escape(symbol), nm.stdout)
            if not match:
                raise RuntimeError(f"candidate function symbol not found in x86 object: {symbol}")
            candidate_entry = int(match.group(1), 16)
            candidate_disassembly = subprocess.run(
                ["i686-w64-mingw32-objdump", "-d", "-C", str(candidate_object)],
                text=True, capture_output=True, check=False, timeout=10,
            )
            if candidate_disassembly.returncode != 0:
                raise RuntimeError(f"could not disassemble x86 candidate: {candidate_disassembly.stderr.strip()}")
            candidate_text = candidate_disassembly.stdout
            symbol_start = re.search(rf"(?m)^\s*{candidate_entry:08x} <[^>]+>:\s*$", candidate_text)
            if not symbol_start:
                raise RuntimeError("candidate function symbol missing from x86 disassembly")
            section_start = symbol_start.end()
            next_symbol = re.search(r"(?m)^\s*[0-9a-fA-F]+ <[^>]+>:\s*$", candidate_text[section_start:])
            section = candidate_text[section_start:section_start + next_symbol.start()] if next_symbol else candidate_text[section_start:]
            candidate_steps = len(re.findall(r"(?m)^\s*[0-9a-fA-F]+:\s+(?:[0-9a-fA-F]{2}\s+)+[a-zA-Z]", section))
            if candidate_steps < 2:
                raise RuntimeError("could not determine candidate instruction count")

            if address == MODEL_GET_ATTACHMENT_ADDRESS:
                cases_to_run = [0, 1, 2, 3, 7, 15, 31, 63, 127, 255, 511, 1023]
                native_steps = 10
            else:
                cases_to_run = [-32768, -32767, -257, -256, -129, -128, -1,
                                0, 1, 127, 128, 255, 256, 32767]
                native_steps = 5

            def run_leaf(image: pathlib.Path, entry: int, steps: int, first: int, second: int | None,
                         *, retail_image: bool) -> int:
                stack = 0x70008000
                commands = [
                    "aei", "aeim 0x70000000 0x10000 stack", "aeim 0x60000000 0x10000 data",
                    f"aer esp=0x{stack:08x}", f"ae 0xdeadbeef,0x{stack:08x},=[4]",
                    f"ae 0x{MODEL_RECORD:08x},0x{stack + 4:08x},=[4]",
                    f"ae 0x{first & 0xffffffff:08x},0x{stack + 8:08x},=[4]",
                    f"ae 0x{RESOURCE_RECORD:08x},{MODEL_RECORD + 0x0c:#x},=[4]",
                    f"ae 0x{MESH_HEADER:08x},{RESOURCE_RECORD + 0x0c:#x},=[4]",
                ]
                if second is None:
                    commands.append(f"ae 0x{ATTACHMENT_BASE:08x},{MESH_HEADER + 0x8c:#x},=[4]")
                else:
                    commands.append(f"ae 0x{second & 0xffff:04x},{MESH_HEADER + 0x7e:#x},=[2]")
                commands.extend((f"s 0x{entry:x}", f"aer eip=0x{entry:x}", f"aesp 0x{entry:x} {steps}", "aer eax"))
                options = ["-q", "-c", ";".join(commands)]
                if retail_image:
                    options[1:1] = ["-e", "bin.relocs.apply=true"]
                else:
                    options[1:1] = ["-e", "asm.arch=x86", "-e", "asm.bits=32"]
                process = subprocess.run(
                    [args.r2, *options, str(image)], text=True, capture_output=True,
                    check=False, timeout=10,
                )
                if process.returncode != 0:
                    raise RuntimeError(f"Radare2 ESIL failed at {entry:#x}: {process.stderr.strip()}")
                values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|[0-9]+)\s*$", process.stdout)
                if not values:
                    raise RuntimeError(f"Could not read ESIL EAX result: {process.stdout!r}")
                return int(values[-1], 0) & 0xffffffff

            cases = []
            for value in cases_to_run:
                if address == MODEL_GET_ATTACHMENT_ADDRESS:
                    expected = run_leaf(binary, int(address, 16), native_steps, value, None, retail_image=True)
                    observed = run_leaf(candidate_object, candidate_entry, candidate_steps, value, None, retail_image=False)
                    case_input = {"args": [MODEL_RECORD, value], "initial_state": {
                        "model_resource": RESOURCE_RECORD, "resource_mesh": MESH_HEADER,
                        "mesh_attachments": ATTACHMENT_BASE}}
                    categories = ["boundary" if value in (0, 1, 1023) else "nominal"]
                    delta = 0
                else:
                    expected = run_leaf(binary, int(address, 16), native_steps, 0, value, retail_image=True)
                    observed = run_leaf(candidate_object, candidate_entry, candidate_steps, 0, value, retail_image=False)
                    expected = signed32(expected); observed = signed32(observed)
                    case_input = {"args": [MODEL_RECORD], "initial_state": {
                        "mesh_attachment_count": value, "model_resource": RESOURCE_RECORD,
                        "resource_mesh": MESH_HEADER}}
                    categories = ["boundary" if value in (-32768, -1, 0, 1, 32767) else "nominal"]
                    delta = 0
                cases.append({
                    "id": f"case-{value & 0xffffffff:08x}", "input": case_input, "categories": categories,
                    "reference_output": {"return_value": expected, "state_delta": delta, "side_effects": []},
                    "candidate_output": {"return_value": observed, "state_delta": delta, "side_effects": []},
                })
        failed = sum(c["reference_output"] != c["candidate_output"] for c in cases)
        report = {
            "schema_version": 1, "kind": "behavioral-test-report",
            "result": "passed" if failed == 0 else "failed", "executed": True,
            "oracle": "retail-binary-execution", "address": address,
            "binary_sha256": args.binary_sha256, "implementation_sha256": args.implementation_sha256,
            "method": "Retail x86 instructions from the selected igi.exe and the freshly compiled i686 candidate object were both executed by Radare2 ESIL with identical synthetic x86 pointer layouts. The pointer accessor uses 12 non-negative indices within the modeled attachment range; the count accessor uses signed boundary values. This is emulator comparison, not a live IGI process.",
            "observable_fields": ["return_value", "state_delta", "side_effects"],
            "float_tolerance": {"absolute": 0, "relative": 0},
            "tests_total": len(cases), "tests_passed": len(cases) - failed,
            "tests_failed": failed, "cases": cases,
        }
        print(json.dumps(report, separators=(",", ":"), sort_keys=True))
        return 0 if failed == 0 else 1

    if address == LOOP_STATUS_SETTER_ADDRESS:
        harness = '''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/core/frame_natives.hpp"
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const auto value = static_cast<std::uint8_t>(std::strtoul(argv[1], nullptr, 0));
    DAT_005C8DE8 = 0xa5U;
    Loop_SetStatusByte(static_cast<std::int8_t>(value));
    std::printf("0 %u\\n", static_cast<unsigned>(DAT_005C8DE8));
    return 0;
}
'''
        cases = []
        with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
            tempdir = pathlib.Path(temp)
            harness_path = tempdir / "candidate_runner.cpp"
            candidate = tempdir / "candidate"
            harness_path.write_text(harness, encoding="utf-8")
            dead_section_flag = "-Wl,-dead_strip" if sys.platform == "darwin" else "-Wl,--gc-sections"
            built = subprocess.run(
                [args.cxx, "-std=c++20", "-ffunction-sections", "-fdata-sections",
                 f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
                 dead_section_flag, "-o", str(candidate)],
                cwd=root, text=True, capture_output=True, check=False, timeout=45,
            )
            if built.returncode != 0:
                raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")
            for value in range(256):
                initial = value ^ 0x55
                expected_return, expected_state = retail_loop_status_observation(args.r2, binary, value, initial)
                actual = subprocess.run([str(candidate), str(value)], text=True, capture_output=True, check=False, timeout=5)
                if actual.returncode != 0:
                    raise RuntimeError(f"candidate call failed for byte {value}: {actual.stderr.strip()}")
                try:
                    observed_return, observed_state = map(int, actual.stdout.strip().split())
                except ValueError as error:
                    raise RuntimeError(f"candidate output malformed: {actual.stdout!r}") from error
                cases.append({
                    "id": f"byte-{value:02x}",
                    "input": {"args": [value], "initial_state": {"status_byte": initial}},
                    "categories": ["boundary" if value in (0, 1, 127, 128, 254, 255) else "nominal"],
                    # Retail's EAX value is undefined for this void native; compare only the
                    # defined byte store and normalize the incidental register value.
                    "reference_output": {"return_value": 0, "state_delta": expected_state, "side_effects": ["write:0x005C8DE8"]},
                    "candidate_output": {"return_value": 0, "state_delta": observed_state, "side_effects": ["write:0x005C8DE8"]},
                })
        failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
        report = {
            "schema_version": 1, "kind": "behavioral-test-report",
            "result": "passed" if failed == 0 else "failed", "executed": True,
            "oracle": "retail-binary-execution", "address": address,
            "binary_sha256": args.binary_sha256,
            "implementation_sha256": args.implementation_sha256,
            "method": "Retail x86 MOV AL/store AL/RET bytes from the selected igi.exe executed in Radare2 ESIL for every byte input; the native is void, so its incidental EAX value is ignored and only the defined global byte write is compared. This is emulator evidence, not a live IGI.exe process.",
            "observable_fields": ["return_value", "state_delta", "side_effects"],
            "float_tolerance": {"absolute": 0, "relative": 0},
            "tests_total": len(cases), "tests_passed": len(cases) - failed,
            "tests_failed": failed, "cases": cases,
        }
        print(json.dumps(report, separators=(",", ":"), sort_keys=True))
        return 0 if failed == 0 else 1

    if address in (CONTEXT_ACCESSOR_ADDRESS, CONTEXT_STATE_SETTER_ADDRESS):
        if address == CONTEXT_STATE_SETTER_ADDRESS:
            harness = '''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/core/frame_natives.hpp"
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    igi::retail::core::FrameDispatchRecordView context{};
    context.unknown_0x2C = 0xa5a5a5a5U;
    DAT_00567C8C = &context;
    const auto value = static_cast<std::int32_t>(std::strtoll(argv[1], nullptr, 0));
    Flow_SetState(value);
    std::printf("0 %u\\n", context.unknown_0x2C);
    return 0;
}
'''
            cases = []
            value_set = (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x12345678, 0xA5A5A5A5, 0xDEADBEEF)
            with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
                tempdir = pathlib.Path(temp)
                harness_path = tempdir / "candidate_runner.cpp"
                candidate = tempdir / "candidate"
                harness_path.write_text(harness, encoding="utf-8")
                dead_section_flag = "-Wl,-dead_strip" if sys.platform == "darwin" else "-Wl,--gc-sections"
                built = subprocess.run(
                    [args.cxx, "-std=c++20", "-ffunction-sections", "-fdata-sections",
                     f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
                     dead_section_flag, "-o", str(candidate)],
                    cwd=root, text=True, capture_output=True, check=False, timeout=45,
                )
                if built.returncode != 0:
                    raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")
                for index, value in enumerate(value_set):
                    initial = (0x11223344 + index) & 0xFFFFFFFF
                    expected_return, expected_state = retail_context_state_observation(args.r2, binary, value, initial)
                    actual = subprocess.run(
                        [str(candidate), str(value)], text=True, capture_output=True,
                        check=False, timeout=5,
                    )
                    if actual.returncode != 0:
                        raise RuntimeError(f"candidate call failed for value {value:#x}: {actual.stderr.strip()}")
                    try:
                        observed_return, observed_state = map(int, actual.stdout.strip().split())
                    except ValueError as error:
                        raise RuntimeError(f"candidate output malformed: {actual.stdout!r}") from error
                    # Retail's EAX value is undefined for this void native; compare the written
                    # context field and normalize the incidental register value on both sides.
                    reference = {"return_value": 0, "state_delta": expected_state, "side_effects": ["write:context+0x2c"]}
                    actual_output = {"return_value": observed_return, "state_delta": observed_state, "side_effects": ["write:context+0x2c"]}
                    cases.append({
                        "id": f"value-{value:08x}",
                        "input": {"args": [value], "initial_state": {"context_0x2c": initial}},
                        "categories": ["boundary" if value in (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF) else "nominal"],
                        "reference_output": reference,
                        "candidate_output": actual_output,
                    })
            failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
            report = {
                "schema_version": 1, "kind": "behavioral-test-report",
                "result": "passed" if failed == 0 else "failed", "executed": True,
                "oracle": "retail-binary-execution", "address": address,
                "binary_sha256": args.binary_sha256,
                "implementation_sha256": args.implementation_sha256,
                "method": "Retail x86 MOV/MOV/MOV/RET bytes from the selected igi.exe executed in Radare2 ESIL over eight boundary and representative uint32 values; candidate compiled from current C++ and compared for the context field write. This is emulator evidence, not a live IGI.exe process.",
                "observable_fields": ["return_value", "state_delta", "side_effects"],
                "float_tolerance": {"absolute": 0, "relative": 0},
                "tests_total": len(cases), "tests_passed": len(cases) - failed,
                "tests_failed": failed, "cases": cases,
            }
            print(json.dumps(report, separators=(",", ":"), sort_keys=True))
            return 0 if failed == 0 else 1

        harness = '''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/core/frame_natives.hpp"
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const auto raw = static_cast<std::uintptr_t>(std::strtoull(argv[1], nullptr, 0));
    DAT_00567C8C = reinterpret_cast<igi::retail::core::FrameDispatchRecordView*>(raw);
    std::printf("%llu\\n", static_cast<unsigned long long>(
        reinterpret_cast<std::uintptr_t>(Game_GetContext())));
    return 0;
}
'''
        cases = []
        pointer_values = (0, 1, 0x1000, 0x00567C8C, 0x7FFFFFFF, 0x80000000, 0xDEADBEEF, 0xFFFFFFFF)
        with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
            tempdir = pathlib.Path(temp)
            harness_path = tempdir / "candidate_runner.cpp"
            candidate = tempdir / "candidate"
            harness_path.write_text(harness, encoding="utf-8")
            dead_section_flag = "-Wl,-dead_strip" if sys.platform == "darwin" else "-Wl,--gc-sections"
            built = subprocess.run(
                [args.cxx, "-std=c++20", "-ffunction-sections", "-fdata-sections",
                 f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
                 dead_section_flag, "-o", str(candidate)],
                cwd=root, text=True, capture_output=True, check=False, timeout=45,
            )
            if built.returncode != 0:
                raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")
            for pointer in pointer_values:
                expected = retail_context_observation(args.r2, binary, pointer)
                actual = subprocess.run(
                    [str(candidate), str(pointer)], text=True, capture_output=True,
                    check=False, timeout=5,
                )
                if actual.returncode != 0:
                    raise RuntimeError(f"candidate call failed for pointer {pointer:#x}: {actual.stderr.strip()}")
                try:
                    observed = int(actual.stdout.strip(), 10)
                except ValueError as error:
                    raise RuntimeError(f"candidate output malformed: {actual.stdout!r}") from error
                cases.append({
                    "id": f"pointer-{pointer:08x}",
                    "input": {"args": [], "initial_state": {"global": pointer}},
                    "categories": ["boundary" if pointer in (0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF) else "nominal"],
                    "reference_output": {
                        "return_value": expected,
                        "state_delta": pointer,
                        "side_effects": [],
                    },
                    "candidate_output": {
                        "return_value": observed,
                        "state_delta": pointer,
                        "side_effects": [],
                    },
                })
        failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
        report = {
            "schema_version": 1,
            "kind": "behavioral-test-report",
            "result": "passed" if failed == 0 else "failed",
            "executed": True,
            "oracle": "retail-binary-execution",
            "address": address,
            "binary_sha256": args.binary_sha256,
            "implementation_sha256": args.implementation_sha256,
            "method": "Retail x86 MOV/RET bytes from the selected igi.exe were executed in Radare2 ESIL with eight initial global pointer values; candidate compiled from current C++ source and compared by returned pointer. This is emulator evidence, not a live IGI.exe process.",
            "observable_fields": ["return_value", "state_delta", "side_effects"],
            "float_tolerance": {"absolute": 0, "relative": 0},
            "tests_total": len(cases),
            "tests_passed": len(cases) - failed,
            "tests_failed": failed,
            "cases": cases,
        }
        print(json.dumps(report, separators=(",", ":"), sort_keys=True))
        return 0 if failed == 0 else 1

    if address in GETTERS:
        name, field, global_address = GETTERS[address]
        harness = f'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/core/app_context.hpp"
int main(int argc, char** argv) {{
    if (argc != 2) return 2;
    const auto value = static_cast<std::int32_t>(std::strtoll(argv[1], nullptr, 0));
    auto& state = igi::retail::core::AppContext_Get();
    state.{field} = value;
    const auto result = igi::retail::core::{name}();
    std::printf("%d %d\\n", result, state.{field});
    return 0;
}}
'''
        values = (0, 1, -1, 127, 128, -128, 0x12345678, -0x12345678,
                  0x7fffffff, -0x80000000, 0x55555555, -0x55555556)
        cases = []
        with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
            tempdir = pathlib.Path(temp)
            harness_path = tempdir / "candidate_runner.cpp"
            candidate = tempdir / "candidate"
            harness_path.write_text(harness, encoding="utf-8")
            built = subprocess.run(
                [args.cxx, "-std=c++20", f"-I{root / 'engine/include'}", str(implementation),
                 str(harness_path), "-o", str(candidate)],
                cwd=root, text=True, capture_output=True, check=False, timeout=45,
            )
            if built.returncode != 0:
                raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")
            for value in values:
                expected = retail_getter_observation(args.r2, binary, address, global_address, value)
                actual = subprocess.run([str(candidate), str(value)], text=True, capture_output=True,
                                        check=False, timeout=5)
                if actual.returncode != 0:
                    raise RuntimeError(f"candidate getter failed for {value}: {actual.stderr.strip()}")
                try:
                    observed, state_after = map(int, actual.stdout.strip().split())
                except ValueError as error:
                    raise RuntimeError(f"candidate getter output malformed: {actual.stdout!r}") from error
                reference_output = {"return_value": expected, "state_delta": value,
                                    "side_effects": []}
                candidate_output = {"return_value": observed, "state_delta": state_after,
                                    "side_effects": []}
                cases.append({
                    "id": f"state-{value & 0xffffffff:08x}",
                    "input": {"args": [], "initial_state": {field: value}},
                    "categories": ["boundary" if value in (0, -1, 127, 128, -128,
                                                               0x7fffffff, -0x80000000) else "nominal"],
                    "reference_output": reference_output,
                    "candidate_output": candidate_output,
                })
        failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
        report = {
            "schema_version": 1, "kind": "behavioral-test-report",
            "result": "passed" if failed == 0 else "failed", "executed": True,
            "oracle": "retail-binary-execution", "address": address,
            "binary_sha256": args.binary_sha256,
            "implementation_sha256": args.implementation_sha256,
            "method": "Retail x86 global-load/RET instructions from the selected igi.exe were executed in Radare2 ESIL with twelve seeded 32-bit global values; the current candidate was compiled and called with the same values. Return values and unchanged state were compared. This is emulator evidence, not a live IGI.exe process.",
            "observable_fields": ["return_value", "state_delta", "side_effects"],
            "float_tolerance": {"absolute": 0, "relative": 0},
            "tests_total": len(cases), "tests_passed": len(cases) - failed,
            "tests_failed": failed, "cases": cases,
        }
        print(json.dumps(report, separators=(",", ":"), sort_keys=True))
        return 0 if failed == 0 else 1

    name, field, global_address = NATIVES[address]

    harness = f'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/core/app_context.hpp"
int main(int argc, char** argv) {{
    if (argc != 1) return 2;
    auto& state = igi::retail::core::AppContext_Get();
    for (int raw = 0; raw != 256; ++raw) {{
        state.{field} = 0;
        const std::int32_t result = igi::retail::core::{name}(static_cast<std::int8_t>(raw));
        std::printf("%d %d\\n", result, state.{field});
    }}
    return 0;
}}
'''
    cases = []
    with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
        tempdir = pathlib.Path(temp)
        harness_path = tempdir / "candidate_runner.cpp"
        candidate = tempdir / "candidate"
        harness_path.write_text(harness, encoding="utf-8")
        built = subprocess.run(
            [args.cxx, "-std=c++20", f"-I{root / 'engine/include'}", str(implementation),
             str(harness_path), "-o", str(candidate)],
            cwd=root, text=True, capture_output=True, check=False, timeout=45,
        )
        if built.returncode != 0:
            raise RuntimeError(f"candidate harness compilation failed: {built.stderr[-4000:]}")

        raw_values = range(256)
        references = retail_observations_batch(args.r2, binary, address, global_address, raw_values)
        actual = subprocess.run(
            [str(candidate)], text=True, capture_output=True, check=False, timeout=10,
        )
        if actual.returncode != 0:
            raise RuntimeError(f"candidate byte-domain run failed: {actual.stderr.strip()}")
        candidate_lines = actual.stdout.splitlines()
        if len(candidate_lines) != len(raw_values):
            raise RuntimeError(
                f"candidate emitted {len(candidate_lines)} observations for {len(raw_values)} byte inputs"
            )
        for value, reference, candidate_line in zip(raw_values, references, candidate_lines):
            candidate_values = candidate_line.split()
            if len(candidate_values) != 2:
                raise RuntimeError(f"candidate output malformed for byte {value}: {candidate_line!r}")
            result, stored = map(int, candidate_values)
            candidate_output = {
                "return_value": result,
                "state_delta": stored,
                "side_effects": [f"write:{global_address.upper()}"],
            }
            cases.append({
                "id": f"byte-{value:02x}",
                "input": {"args": [value], "initial_state": {"global": 0}},
                "categories": ["boundary" if value in (0, 1, 127, 128, 254, 255) else "nominal"],
                "reference_output": reference,
                "candidate_output": candidate_output,
            })

    if len(cases) != 256:
        raise RuntimeError(f"incomplete byte-domain comparison: {len(cases)}/256 cases")
    failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": "Retail x86 instructions from the selected igi.exe were executed in a single Radare2 ESIL session with the same three-instruction trace repeated for each isolated byte input; the target global and stack byte are reset per case. The current C++ candidate was compiled and executed over the exhaustive byte domain. ESIL emulation was used because this ARM64 macOS host cannot launch the Windows x86 game process.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": len(cases) - failed,
        "tests_failed": failed,
        "cases": cases,
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as exc:
        print(str(exc), file=sys.stderr)
        raise SystemExit(1)
