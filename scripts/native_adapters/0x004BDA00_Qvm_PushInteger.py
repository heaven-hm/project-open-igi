"""Differential adapter: 0x004BDA00 Qvm_PushInteger.

Retail bytes (radare2, 0x004BDA00-0x004BDA29):

    mov eax, dword [esp + 4]        ; state
    mov ecx, dword [eax + 0x18]     ; value_stack_index
    sub ecx, 1
    mov dword [eax + 0x18], ecx     ; value_stack_index -= 1
    mov edx, dword [eax + 0x1C]     ; type_stack_index
    sub edx, 1
    mov dword [eax + 0x1C], edx     ; type_stack_index -= 1
    mov edx, dword [eax + 0x28]     ; type_stack base
    mov dword [edx + ecx * 4], 1    ; type tag = 1 (Integer)
    mov ecx, dword [eax + 0x18]     ; reload value_stack_index
    mov edx, dword [eax + 0x24]     ; value_stack base
    mov eax, dword [esp + 8]        ; value
    mov dword [edx + ecx * 4], eax  ; value_stack[value_stack_index] = value
    ret

The candidate engine/src/retail/script/qvm_stack_natives.cpp performs the same
writes through the recovered QvmNativeStatePrefix layout.  Both sides are given
identical synthetic stack indices, stack bases and value, and the resulting
indices and stack contents are compared semantically (never by raw struct byte
offset, because the candidate host is 64-bit while retail is x86-32).
"""

from __future__ import annotations

import struct

ADDRESS = "0x004BDA00"
RETAIL_ADDRESS = 0x004BDA00
NAME = "Qvm_PushInteger"

STATE_ADDRESS = 0x70012000
VALUE_STACK_ADDRESS = 0x70013000
TYPE_STACK_ADDRESS = 0x70014000
STACK_DWORDS = 0x100
FILL_DWORD = 0xA5A5A5A5
SENTINEL = 0x00C0FFEE

# Recovered QvmNativeStatePrefix offsets (retail x86-32 ABI).
STATE_VALUE_STACK_INDEX = 0x18
STATE_TYPE_STACK_INDEX = 0x1C
STATE_VALUE_STACK = 0x24
STATE_TYPE_STACK = 0x28

LIMITS = (
    "Retail leaves the pushed value in EAX, but the recovered contract is void: no recovered "
    "caller reads EAX after Qvm_PushInteger. return_value is therefore normalized to null on "
    "both sides and the pushed value is verified through the value stack instead. The state "
    "pointer is an opaque synthetic address; stack bases are compared through their contents, "
    "not through raw struct byte offsets."
)

CASES = [
    ("qvm-push-integer-nominal-default", ["nominal"], 8, 8, 0x12345678),
    ("qvm-push-integer-nominal-zero", ["nominal"], 4, 4, 0x00000000),
    ("qvm-push-integer-nominal-max", ["nominal"], 6, 5, 0x7FFFFFFF),
    ("qvm-push-integer-nominal-sign-bit", ["nominal"], 0x20, 0x20, 0x80000000),
    ("qvm-push-integer-boundary-first-slot", ["boundary"], 1, 1, 0xFFFFFFFF),
    ("qvm-push-integer-boundary-last-slot", ["boundary"], STACK_DWORDS, STACK_DWORDS, 0xDEADBEEF),
    ("qvm-push-integer-boundary-mid-slot", ["boundary"], STACK_DWORDS // 2, 0x3F, 0x0000ABCD),
    ("qvm-push-integer-boundary-skewed-indices", ["boundary"], 0x40, 0x20, 0x5A5A5A5A),
]


def cases() -> list[dict]:
    return [
        {
            "id": case_id,
            "categories": categories,
            "value_stack_index": value_stack_index,
            "type_stack_index": type_stack_index,
            "value": value,
            "seed": seed,
        }
        for seed, (case_id, categories, value_stack_index, type_stack_index, value) in enumerate(CASES)
    ]


def case_literal(address: int, case: dict) -> str:
    return (
        '        { "%s", %dU, %dU, %dU },'
        % (case["id"], case["value_stack_index"], case["type_stack_index"], case["value"])
    )


def _changed_dwords(buffer: bytes) -> int:
    return sum(
        1 for offset in range(0, len(buffer), 4)
        if struct.unpack_from("<I", buffer, offset)[0] != FILL_DWORD
    )


def retail(image, case: dict) -> dict:
    emulator = BASE.new_emulator(image)
    emulator.mem_write(STATE_ADDRESS, bytes(0x30))
    emulator.mem_write(STATE_ADDRESS + STATE_VALUE_STACK_INDEX,
                       struct.pack("<I", case["value_stack_index"] & 0xFFFFFFFF))
    emulator.mem_write(STATE_ADDRESS + STATE_TYPE_STACK_INDEX,
                       struct.pack("<I", case["type_stack_index"] & 0xFFFFFFFF))
    emulator.mem_write(STATE_ADDRESS + STATE_VALUE_STACK, struct.pack("<I", VALUE_STACK_ADDRESS))
    emulator.mem_write(STATE_ADDRESS + STATE_TYPE_STACK, struct.pack("<I", TYPE_STACK_ADDRESS))
    fill = bytes([0xA5]) * (STACK_DWORDS * 4)
    emulator.mem_write(VALUE_STACK_ADDRESS, fill)
    emulator.mem_write(TYPE_STACK_ADDRESS, fill)

    BASE.run_with_arguments(
        emulator, RETAIL_ADDRESS, [STATE_ADDRESS, case["value"] & 0xFFFFFFFF], SENTINEL)

    value_stack_index = BASE.read_u32(emulator, STATE_ADDRESS + STATE_VALUE_STACK_INDEX)
    type_stack_index = BASE.read_u32(emulator, STATE_ADDRESS + STATE_TYPE_STACK_INDEX)
    value_stack = emulator.mem_read(VALUE_STACK_ADDRESS, STACK_DWORDS * 4)
    type_stack = emulator.mem_read(TYPE_STACK_ADDRESS, STACK_DWORDS * 4)

    return {
        "return_value": None,
        "state_delta": {
            "value_stack_index": value_stack_index,
            "type_stack_index": type_stack_index,
            "stored_value": struct.unpack_from("<I", value_stack, value_stack_index * 4)[0],
            "stored_type_tag": struct.unpack_from("<I", type_stack, type_stack_index * 4)[0],
            "value_stack_changed_dwords": _changed_dwords(value_stack),
            "type_stack_changed_dwords": _changed_dwords(type_stack),
        },
        "side_effects": [],
    }


def candidate_source() -> str:
    return r"""
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "igi/retail/script/qvm_stack_natives.hpp"

namespace
{

constexpr std::size_t stack_dwords = 0x100U;
constexpr std::uint32_t fill_dword = 0xA5A5A5A5U;

template <std::size_t Size>
std::size_t changed_dwords(const std::array<std::uint32_t, Size>& buffer)
{
    std::size_t changed = 0;
    for (const std::uint32_t word : buffer)
    {
        if (word != fill_dword) { ++changed; }
    }
    return changed;
}

} // namespace

int main()
{
    struct Case
    {
        const char* id;
        std::uint32_t value_stack_index;
        std::uint32_t type_stack_index;
        std::uint32_t value;
    };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        std::array<std::uint32_t, stack_dwords> value_stack{};
        std::array<std::uint32_t, stack_dwords> type_stack{};
        value_stack.fill(fill_dword);
        type_stack.fill(fill_dword);

        igi::retail::script::QvmNativeStatePrefix state{};
        state.value_stack_index = static_cast<std::uint32_t>(item.value_stack_index);
        state.type_stack_index = static_cast<std::uint32_t>(item.type_stack_index);
        state.value_stack = value_stack.data();
        state.type_stack = type_stack.data();

        igi::retail::script::Qvm_PushInteger(&state, item.value);

        std::printf(
            "{\"id\":\"%s\",\"return_value\":null,\"state_delta\":{"
            "\"value_stack_index\":%u,\"type_stack_index\":%u,"
            "\"stored_value\":%u,\"stored_type_tag\":%u,"
            "\"value_stack_changed_dwords\":%zu,\"type_stack_changed_dwords\":%zu},"
            "\"side_effects\":[]}\n",
            item.id,
            static_cast<unsigned>(state.value_stack_index),
            static_cast<unsigned>(state.type_stack_index),
            static_cast<unsigned>(value_stack[state.value_stack_index]),
            static_cast<unsigned>(type_stack[state.type_stack_index]),
            changed_dwords(value_stack),
            changed_dwords(type_stack));
    }
    return 0;
}
"""


if __name__ == "__main__":
    raise SystemExit("this module is loaded by scripts/native_unicorn_runner.py")
