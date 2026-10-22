#!/usr/bin/env python3
"""Differential oracle for GOInRemap 0x004037C0.

The retail instruction bytes are executed under Unicorn with the true PE image
mapped, while the candidate C++ translation unit is compiled and executed
natively. Both sides receive the same action count, action slot and three frame
arguments; the comparison covers the remap-table entry, the incremented action
slot, the returned block's defined dwords and whether the destination pointer
came back.

The retail reads its destination pointer out of its caller's stack frame
(0x0040382E loads [esp+0x28] after the three frame-reader calls are cleaned up at
0x0040381E), so the harness supplies that slot exactly as a caller would. The
frame reader 0x004B8A20 is replaced by a hook that returns the fixture argument,
which is what the retail reader returns for the argument_values path.
"""

import argparse
import hashlib
import json
import pathlib
import struct
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

ADDRESS = "0x004037C0"
RETIAL_ENTRY = 0x004037C0
FRAME_READER = 0x004B8A20
COUNT_ADDR = 0x00BC2380
ACTION_TABLE = 0x00BC2358
ACTION_STRIDE_DWORDS = 345
REMAP_TABLE = 0x00BC25BC
DEST = 0x70012000
FRAME = 0x70012040
VALUES = 0x70012080
STACK_TOP = 0x70008000
SENTINEL = 0x00C0FFEE
BINARY_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def digest(path):
    return hashlib.sha256(pathlib.Path(path).read_bytes()).hexdigest()


def cases():
    fixtures = [
        (1, 0, 1, 2, 3),
        (2, 3, 0x10, 0x20, 0x30),
        (3, 5, 0xFFFFFFFF, 0x80000000, 0xFFFFFFFF),
        (4, 0, 0, 0, 0x40000000),
        (5, 115, 0x7FFFFFFF, 0x12345678, 0xBFFFFFFF),
        (8, 7, 4, 8, 12),
        (16, 2, 0x12345678, 0x9ABCDEF0, 0xFFFF),
        (32, 1, 0xDEADBEEF, 0xCAFEBABE, 0x3FFFFFFF),
    ]
    return [
        {"id": f"goinremap-{index}", "categories": ["nominal" if index < 3 else "boundary"],
         "count": count, "action": action, "v0": v0, "v1": v1, "v2": v2}
        for index, (count, action, v0, v1, v2) in enumerate(fixtures)
    ]


def ensure(emu, address, size):
    try:
        emu.mem_read(address, size)
    except Exception:
        base = address & ~0xFFF
        span = ((address + size - base) + 0xFFF) & ~0xFFF
        emu.mem_map(base, span)


def retail_observation(image, case):
    import unicorn
    import native_unicorn_runner as base

    emulator = base.new_emulator(image)
    count, action = case["count"], case["action"]
    values = [case["v0"], case["v1"], case["v2"]]
    slot = (count - 1) * 115 + action
    slot_addr = ACTION_TABLE + count * ACTION_STRIDE_DWORDS * 4
    entry_addr = REMAP_TABLE + slot * 12
    for address in (COUNT_ADDR, slot_addr, entry_addr):
        ensure(emulator, address, 4)
    emulator.mem_write(COUNT_ADDR, struct.pack("<I", count))
    emulator.mem_write(slot_addr, struct.pack("<I", action))
    emulator.mem_write(entry_addr, b"\xEE" * 12)
    emulator.mem_write(FRAME, struct.pack("<III", 0, 0, VALUES))
    emulator.mem_write(VALUES, b"".join(struct.pack("<I", value & 0xFFFFFFFF) for value in values))
    emulator.mem_write(DEST, b"\xCC" * 24)
    emulator.mem_write(STACK_TOP + 0x1C, struct.pack("<I", DEST))

    def frame_reader(uc, address, size, user):
        esp = uc.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
        index = struct.unpack("<I", uc.mem_read(esp + 8, 4))[0]
        return_address = struct.unpack("<I", uc.mem_read(esp, 4))[0]
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EAX, values[index] & 0xFFFFFFFF)
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, return_address)
        uc.reg_write(unicorn.x86_const.UC_X86_REG_ESP, esp + 4)

    emulator.hook_add(unicorn.UC_HOOK_CODE, frame_reader, begin=FRAME_READER, end=FRAME_READER)
    emulator.mem_write(STACK_TOP, struct.pack("<I", SENTINEL))
    emulator.mem_write(STACK_TOP + 4, struct.pack("<I", DEST))
    emulator.mem_write(STACK_TOP + 8, struct.pack("<I", FRAME))
    emulator.reg_write(unicorn.x86_const.UC_X86_REG_ESP, STACK_TOP)
    emulator.emu_start(RETIAL_ENTRY, SENTINEL)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    block = emulator.mem_read(DEST, 24)
    entry = emulator.mem_read(entry_addr, 12)
    after = struct.unpack("<I", emulator.mem_read(slot_addr, 4))[0]
    return {
        "return_value": 1 if eax == DEST else 0,
        "state_delta": {
            "remap_entry": list(struct.unpack("<III", entry)),
            "action_slot_after": after,
            "block_dword0": struct.unpack("<I", block[0:4])[0],
            "block_dword2": struct.unpack("<I", block[8:12])[0],
            "block_dword3": struct.unpack("<I", block[12:16])[0],
            "block_descriptor": struct.unpack("<I", block[16:20])[0],
        },
        "side_effects": [],
    }


HARNESS = r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/input_remap.hpp"

int main()
{
    struct Case { const char* id; std::uint32_t count; std::uint32_t action;
                  std::uint32_t v0; std::uint32_t v1; std::uint32_t v2; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        using namespace igi::retail::config;
        InputRemap_ActionCount = item.count;
        std::memset(InputRemap_ActionTable, 0, sizeof(InputRemap_ActionTable));
        std::memset(InputRemap_Table, 0, sizeof(InputRemap_Table));
        InputRemap_ActionTable[item.count * action_table_stride] = item.action;
        const std::uint32_t values[3] = {item.v0, item.v1, item.v2};
        QvmFrameView frame{};
        frame.argument_values = const_cast<std::uint32_t*>(values);
        std::uint32_t* const returned = GOInRemap(&frame);
        const std::size_t slot = (item.count - 1U) * 115U + item.action;
        const std::uint32_t* const entry = InputRemap_Table + slot * remap_table_stride;
        const std::uint32_t after = InputRemap_ActionTable[item.count * action_table_stride];
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%d,"
            "\"state_delta\":{\"remap_entry\":[%u,%u,%u],\"action_slot_after\":%u,"
            "\"block_dword0\":%u,\"block_dword2\":%u,\"block_dword3\":%u,"
            "\"block_descriptor\":%u},\"side_effects\":[]}\n",
            item.id, returned != nullptr ? 1 : 0,
            entry[0], entry[1], entry[2], after,
            returned[0], returned[2], returned[3], returned[4]);
    }
    return 0;
}
"""


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", default=ADDRESS)
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", type=pathlib.Path, required=True)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", type=pathlib.Path, default=ROOT)
    arguments = parser.parse_args()
    if arguments.address.lower() not in (ADDRESS.lower(), "0x4037c0"):
        raise SystemExit(f"unsupported native address: {arguments.address}")
    if digest(arguments.binary) != arguments.binary_sha256:
        raise SystemExit("retail image does not match the supplied digest")
    if digest(arguments.implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation does not match the supplied digest")
    import native_unicorn_runner as base

    image = base.PebImage(arguments.binary)
    case_list = cases()
    literals = "\n".join(
        f'        {{"{c["id"]}", {c["count"]}U, {c["action"]}U, {c["v0"]}U, {c["v1"]}U, {c["v2"]}U}},'
        for c in case_list)
    with tempfile.TemporaryDirectory(prefix="igi-goinremap-") as workspace:
        folder = pathlib.Path(workspace)
        harness = folder / "harness.cpp"
        harness.write_text(HARNESS.replace("@CASES@", literals))
        candidate = folder / "harness"
        build = subprocess.run(
            ["c++", "-std=c++20", "-O1", f"-I{arguments.root / 'engine/include'}",
             str(arguments.implementation.resolve()), str(harness), "-o", str(candidate)],
            cwd=str(arguments.root), text=True, capture_output=True)
        if build.returncode != 0:
            raise SystemExit(f"candidate harness failed to build: {build.stderr[-1500:]}")
        run = subprocess.run([str(candidate)], text=True, capture_output=True)
        if run.returncode != 0:
            raise SystemExit(f"candidate harness failed: {run.stderr[-1500:]}")
    outputs = [json.loads(line) for line in run.stdout.splitlines() if line.strip()]
    cases_out = []
    failed = 0
    for case, actual in zip(case_list, outputs):
        reference = retail_observation(image, case)
        actual = {key: value for key, value in actual.items() if key in reference}
        problems = []
        if reference["return_value"] != actual["return_value"]:
            problems.append(f"return_value: reference={reference['return_value']} candidate={actual['return_value']}")
        for key, expected in reference["state_delta"].items():
            if expected != actual["state_delta"].get(key):
                problems.append(f"state_delta.{key}: reference={expected} candidate={actual['state_delta'].get(key)}")
        passed = not problems
        failed += 0 if passed else 1
        cases_out.append({
            "id": case["id"], "categories": case["categories"], "input": case,
            "reference_output": {**reference, **({"comparison_problems": problems} if problems else {})},
            "candidate_output": actual, "result": "passed" if passed else "failed",
        })
    report = {
        "schema_version": 1, "kind": "behavioral-test-report", "address": "0x004037C0",
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "executed": True, "oracle": "retail-binary-execution",
        "runner": {"path": "scripts/goinremap_diff_runner.py", "sha256": digest(__file__),
                   "command": [sys.executable, sys.argv[0], *sys.argv[1:]]},
        "result": "failed" if failed else "passed",
        "method": ("Retail instruction bytes for 0x004037C0 were executed under Unicorn with the true PE "
                   "image mapped and a synthetic cdecl stack; the candidate C++ translation unit was compiled "
                   "and executed natively with the same table and frame fixtures. The frame reader 0x004B8A20 "
                   "is replaced by a hook that returns the fixture argument, and the caller's destination slot "
                   "is supplied at [esp+0x28], which is where the retail reads it."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "tests_total": len(cases_out), "tests_passed": len(cases_out) - failed, "tests_failed": failed,
        "cases": cases_out,
    }
    print(json.dumps(report))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
