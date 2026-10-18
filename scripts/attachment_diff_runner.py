#!/usr/bin/env python3
"""Compare the two retail model attachment accessors with freshly compiled x86 candidates.

The fixtures execute original and candidate instruction bodies in Radare2 ESIL
with matching virtual pointer layouts. Caller-visible read-only behavior is
established separately from the retained instruction listings.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import pathlib
import re
import subprocess
import tempfile

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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--r2", default="/opt/homebrew/bin/r2")
    args = parser.parse_args()
    address = "0x" + args.address[-8:].upper()
    if address not in (MODEL_GET_ATTACHMENT_ADDRESS, MODEL_GET_ATTACHMENT_COUNT_ADDRESS):
        parser.error("only the two model attachment accessor entries are supported")
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256:
        raise RuntimeError("selected binary or implementation digest changed")
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
        instruction_lines = [line for line in section.splitlines() if re.match(
            r"\s*[0-9a-fA-F]+:\s+(?:[0-9a-fA-F]{2}\s+)+[a-zA-Z]", line)]
        # Object sections can end in alignment NOPs. Stop at the leaf's RET,
        # then require ESIL to have consumed the synthetic return address.
        return_index = next((i for i, line in enumerate(instruction_lines)
                             if re.search(r"\bret\b", line)), None)
        if return_index is None:
            raise RuntimeError("candidate leaf has no identified return instruction")
        candidate_steps = return_index + 1

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
            commands.extend((f"s 0x{entry:x}", f"aer eip=0x{entry:x}",
                             f"aesp 0x{entry:x} {steps}", "aer eax", "aer eip"))
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
            if len(values) < 2:
                raise RuntimeError(f"Could not read ESIL EAX/EIP result: {process.stdout!r}")
            if int(values[-1], 0) != 0xdeadbeef:
                raise RuntimeError(f"ESIL did not reach the return sentinel at {entry:#x}: {values[-1]}")
            return int(values[-2], 0) & 0xffffffff

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
                expected = signed32(expected)
                observed = signed32(observed)
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
        "method": "Retail x86 instructions from the selected igi.exe and the freshly compiled i686 candidate object were executed by Radare2 ESIL with identical synthetic x86 pointer layouts. Both executions must reach the synthetic return sentinel. The pointer accessor uses 12 non-negative indices; the count accessor uses signed boundary values. Return registers are compared exactly. Caller-visible read-only behavior is established by independent retained instruction listings, rather than a dynamic write trace.",
        "limitations": ["Valid Windows x86 model pointer chains and valid array indices only.",
                        "ESIL emulation is not a live game or factory integration test."],
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases), "tests_passed": len(cases) - failed,
        "tests_failed": failed, "cases": cases,
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failed == 0 else 1



if __name__ == "__main__":
    raise SystemExit(main())
