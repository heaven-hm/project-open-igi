"""Differential adapter: 0x004037C0 GOInRemap.

Retail bytes (Binary Ninja / radare2, 0x004037C0-0x00403860):

    mov eax, [0x00BC2380]              ; action count
    lea ecx, [eax+eax*2]; shl ecx,3; sub ecx,eax
    lea ecx, [ecx+ecx*2]; lea ecx,[ecx+ecx*4]   ; ecx = 345 * count
    mov edx, [ecx*4 + 0x00BC2358]      ; action slot value
    lea esi, [ecx*4 + 0x00BC2358]      ; &InputRemap_ActionTable[count]
    lea eax, [eax + edx - 0x73]        ; (count - 1) * 115 + action
    lea eax, [eax+eax*2]
    lea edi, [eax*4 + 0x00BC25BC]      ; &InputRemap_Table[slot]
    push 0; push ebx; call 0x004B8A20  ; Script_EvalArgInt(frame, 0)
    mov [edi], eax
    push 1; push ebx; call 0x004B8A20  ; Script_EvalArgInt(frame, 1)
    mov [edi+4], eax
    push 2; push ebx; call 0x004B8A20  ; Script_EvalArgInt(frame, 2)
    and eax, 0x3FFFFFFF
    mov [edi+8], eax
    mov eax, [esi]; inc eax; mov [esi], eax     ; increment the action slot
    ... build the six-dword block {0, ?, 0, 0, 0x00567C74, ?} ...
    rep movsd dword [edi], [esi]                ; copy to arg1
    ret                                         ; EAX still arg1

The frame reader 0x004B8A20 is replaced by a local stub that returns the case
argument for the requested index, so the comparison covers the native's own
arithmetic, stores, increment, block build and returned destination pointer
without depending on the retail QVM reader.  The retail destination is a caller
buffer; both sides use the same synthetic address and the result is compared by
content, never by an address from one side being meaningful to the other.

The candidate engine/src/retail/config/input_remap.cpp keeps the same
instruction sequence; its configuration decoder has no QVM result buffer, so
the header's single-argument overload supplies a host destination and forwards
to the retail-shaped three-argument entry.
"""

from __future__ import annotations

import struct

ADDRESS = "0x004037C0"
RETAIL_ADDRESS = 0x004037C0
NAME = "GOInRemap"

RETAIL_ACTION_COUNT = 0x00BC2380
RETAIL_ACTION_TABLE = 0x00BC2358
RETAIL_REMAP_TABLE = 0x00BC25BC
RETAIL_EVAL_ARG_INT = 0x004B8A20

ACTION_STRIDE_DWORDS = 345
REMAP_STRIDE_DWORDS = 3
REMAP_SLOT_FACTOR = 115

FRAME_ADDRESS = 0x70018000
DESTINATION_ADDRESS = 0x70017000
ARGUMENT_TABLE = 0x70016000
SENTINEL = 0x00C0FFEE
FILL_DWORD = 0xA5A5A5A5
REMAP_FILL_BYTES = 0x20000

LIMITS = (
    "The frame reader 0x004B8A20 is stubbed to return the case argument, so the differential "
    "covers the native's arithmetic, remap stores, action-slot increment, result-block build and "
    "returned destination pointer, not the retail QVM reader itself. Result-block dwords 1 and 5 "
    "are never written by retail (stack residue); the comparison records only dwords 0, 2, 3 and 4 "
    "and the candidate's deterministic zero for dwords 1 and 5 is a documented port deviation. "
    "Case counts stay below the candidate's action-table and remap-table bounds."
)

# (id, categories, count, action, arg0, arg1, arg2)
CASES = [
    ("go-in-remap-nominal-first", ["nominal"], 1, 1, 10, 20, 30),
    ("go-in-remap-nominal-zero-args", ["nominal"], 2, 2, 0, 0, 0),
    ("go-in-remap-nominal-high-bits", ["nominal"], 3, 3, 0xFFFFFFFF, 0x80000000, 0x40000000),
    ("go-in-remap-nominal-small", ["nominal"], 5, 5, 1, 2, 3),
    ("go-in-remap-nominal-mid", ["nominal"], 12, 12, 0x11111111, 0x22222222, 0x33333333),
    ("go-in-remap-nominal-last", ["nominal"], 20, 20, 0x7FFFFFFF, 0xFFFFFFFF, 0x3FFFFFFF),
    ("go-in-remap-boundary-count-zero-wrap", ["boundary"], 0, 115, 0, 0, 0),
    ("go-in-remap-boundary-action-zero", ["boundary"], 1, 0, 0xDEADBEEF, 0xCAFEBABE, 0xC0000001),
    ("go-in-remap-boundary-mask-bit30", ["boundary"], 7, 200, 4, 5, 0x40000001),
]


def cases() -> list[dict]:
    return [
        {
            "id": case_id,
            "categories": categories,
            "count": count,
            "action": action,
            "arg0": arg0,
            "arg1": arg1,
            "arg2": arg2,
            "seed": seed,
        }
        for seed, (case_id, categories, count, action, arg0, arg1, arg2) in enumerate(CASES)
    ]


def case_literal(address: int, case: dict) -> str:
    return (
        '        { "%s", %dU, %dU, %dU, %dU, %dU },'
        % (case["id"], case["count"], case["action"], case["arg0"], case["arg1"], case["arg2"])
    )


def _slot(count: int, action: int) -> int:
    return ((count - 1) * REMAP_SLOT_FACTOR + action) & 0xFFFFFFFF


def retail(image, case: dict) -> dict:
    import unicorn

    emulator = BASE.new_emulator(image)

    # Script_EvalArgInt(frame, index) -> argument_table[index].
    stub = (b"\x8B\x44\x24\x08"                                    # mov eax, [esp+8]
            + b"\x8B\x04\x85" + struct.pack("<I", ARGUMENT_TABLE)  # mov eax, [eax*4+table]
            + b"\xC3")                                             # ret
    emulator.mem_write(RETAIL_EVAL_ARG_INT, stub)
    for index, key in enumerate(("arg0", "arg1", "arg2")):
        emulator.mem_write(ARGUMENT_TABLE + index * 4,
                           struct.pack("<I", case[key] & 0xFFFFFFFF))
    emulator.mem_write(FRAME_ADDRESS, bytes(0x20))

    emulator.mem_write(RETAIL_ACTION_COUNT, struct.pack("<I", case["count"] & 0xFFFFFFFF))
    # The action record for count 1 starts at 0x00BC28BC, inside the 0x00BC25BC
    # remap region, so fill the remap region first and stamp the action slot
    # afterwards. Retail only reads the low dwords of an action record, which is
    # why the two regions can overlap.
    emulator.mem_write(RETAIL_REMAP_TABLE, bytes([0xA5]) * REMAP_FILL_BYTES)
    action_address = RETAIL_ACTION_TABLE + case["count"] * ACTION_STRIDE_DWORDS * 4
    emulator.mem_write(action_address, struct.pack("<I", case["action"] & 0xFFFFFFFF))
    emulator.mem_write(DESTINATION_ADDRESS, bytes([0xA5]) * 0x18)

    BASE.run_with_arguments(
        emulator, RETAIL_ADDRESS,
        [DESTINATION_ADDRESS, 0, FRAME_ADDRESS], SENTINEL)

    return_value = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    slot = _slot(case["count"], case["action"])
    entry_address = RETAIL_REMAP_TABLE + slot * REMAP_STRIDE_DWORDS * 4
    entry = [BASE.read_u32(emulator, entry_address + index * 4) for index in range(3)]
    action_slot_after = BASE.read_u32(emulator, action_address)
    block = [BASE.read_u32(emulator, DESTINATION_ADDRESS + index * 4) for index in range(6)]

    return {
        # The retail return value is the destination pointer. Addresses from the
        # emulator and the 64-bit host are not comparable across processes, so
        # the scalar return is normalized to null and the relation
        # return == destination is compared instead.
        "return_value": None,
        "state_delta": {
            "return_is_destination": 1 if return_value == DESTINATION_ADDRESS else 0,
            "remap_entry": entry,
            "action_slot_after": action_slot_after,
            "result_word0": block[0],
            "result_word2": block[2],
            "result_word3": block[3],
            "result_word4": block[4],
        },
        "side_effects": [],
    }


def candidate_source() -> str:
    return r"""
#include <cstdint>
#include <cstdio>

#include "igi/retail/config/input_remap.hpp"

namespace
{
constexpr std::uint32_t fill_dword = 0xA5A5A5A5U;
constexpr std::uint32_t destination = 0x70017000U;
}

int main()
{
    struct Case
    {
        const char* id;
        std::uint32_t count;
        std::uint32_t action;
        std::uint32_t arg0;
        std::uint32_t arg1;
        std::uint32_t arg2;
    };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        ::InputRemap_ActionCount = item.count;
        for (std::uint32_t& word : ::InputRemap_Table) { word = fill_dword; }
        std::uint32_t* const action_slot =
            ::InputRemap_ActionTable + static_cast<std::size_t>(item.count)
                * igi::retail::config::action_table_stride;
        *action_slot = item.action;

        std::uint32_t values[3] = { item.arg0, item.arg1, item.arg2 };
        igi::retail::config::QvmFrameView frame{};
        frame.argument_values = values;

        std::uint32_t result[6];
        for (std::uint32_t& word : result) { word = fill_dword; }
        std::uint32_t* const returned =
            GOInRemap(result, 0, static_cast<void*>(&frame));

        const std::uint32_t slot = (item.count - 1U) * 115U + item.action;
        const std::uint32_t* const entry =
            ::InputRemap_Table + static_cast<std::size_t>(slot)
                * igi::retail::config::remap_table_stride;

        std::printf(
            "{\"id\":\"%s\",\"return_value\":null,\"state_delta\":{"
            "\"return_is_destination\":%d,\"remap_entry\":[%u,%u,%u],"
            "\"action_slot_after\":%u,\"result_word0\":%u,\"result_word2\":%u,"
            "\"result_word3\":%u,\"result_word4\":%u},\"side_effects\":[]}\n",
            item.id,
            returned == result ? 1 : 0,
            static_cast<unsigned>(entry[0]),
            static_cast<unsigned>(entry[1]),
            static_cast<unsigned>(entry[2]),
            static_cast<unsigned>(*action_slot),
            static_cast<unsigned>(result[0]),
            static_cast<unsigned>(result[2]),
            static_cast<unsigned>(result[3]),
            static_cast<unsigned>(result[4]));
    }
    return 0;
}
"""


if __name__ == "__main__":
    raise SystemExit("this module is loaded by scripts/native_unicorn_runner.py")
