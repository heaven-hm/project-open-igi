"""Differential adapter: 0x004BCFD0 Qvm_ExecuteInstructions.

Retail bytes (radare2, 0x004BCFD0-0x004BD00A, 59 bytes):

    0x004bcfd0  push esi
    0x004bcfd1  mov esi, dword [esp + 8]        ; state
    0x004bcfd5  mov dword [esi + 0x2c], 0       ; stop_flag = 0
    0x004bcfdc  mov eax, dword [esi + 0x20]     ; loop: error_status
    0x004bcfdf  test eax, eax
    0x004bcfe1  jne 0x4bd009                    ; -> done
    0x004bcfe3  mov eax, dword [esi + 0x14]     ; instruction_offset
    0x004bcfe6  mov ecx, dword [esi]            ; code_base
    0x004bcfe8  xor edx, edx
    0x004bcfea  push 0                          ; argument 4 = 0
    0x004bcfec  mov dl, byte [eax + ecx]        ; opcode = code_base[instruction_offset]
    0x004bcfef  mov eax, edx
    0x004bcff1  push eax                        ; argument 3 = opcode
    0x004bcff2  push 0                          ; argument 2 = 0
    0x004bcff4  lea ecx, [eax + eax*2]          ; opcode * 3
    0x004bcff7  push esi                        ; argument 1 = state
    0x004bcff8  call dword [ecx*8 + 0xa96108]   ; first handler dword of opcode record
    0x004bcfff  mov eax, dword [esi + 0x2c]     ; stop_flag
    0x004bd002  add esp, 0x10
    0x004bd005  test eax, eax
    0x004bd007  je 0x4bcfdc
    0x004bd009  pop esi
    0x004bd00a  ret

The candidate engine/src/retail/script/qvm_stack_natives.cpp performs the same
loop against the recovered QvmNativeStatePrefix layout.  Both sides are driven
with identical synthetic code buffers and an identical synthetic opcode-handler
policy: every dispatched opcode advances instruction_offset by a per-case stride
and, at a per-case dispatch limit, raises either stop_flag or error_status.  The
dispatch sequence (opcode trace), the cdecl argument order, the argument values,
the final instruction_offset, stop_flag and error_status are compared.

The retail opcode handler table lives at 0x00A96108 in the retail image and is
process-owned relocated data; the candidate table is a C++ array with host-width
pointers.  Its byte layout is therefore never compared, only the behavior it
dispatches.  Retail handler entries are pointed at a sentinel address so this
adapter can observe each dispatch at the real `call [ecx*8 + 0xa96108]` site and
then resume the real interpreter loop.
"""

from __future__ import annotations

import struct

ADDRESS = "0x004BCFD0"
RETAIL_ADDRESS = 0x004BCFD0
NAME = "Qvm_ExecuteInstructions"

STATE_ADDRESS = 0x70012000
CODE_ADDRESS = 0x70016000
CODE_BYTES = 0x1000
STACK_TOP = 0x70008000
SENTINEL = 0x00C0FFEE

# Retail opcode handler table: 0x00A96108, six DWORD slots per opcode record.
OPCODE_HANDLER_TABLE = 0x00A96108
OPCODE_RECORD_BYTES = 24
OPCODE_RECORD_COUNT = 256

# Recovered QvmNativeStatePrefix offsets (retail x86-32 ABI).
STATE_CODE_BASE = 0x00
STATE_INSTRUCTION_OFFSET = 0x14
STATE_ERROR_STATUS = 0x20
STATE_STOP_FLAG = 0x2C
STATE_BYTES = 0x30

MAX_DISPATCHES = 64

LIMITS = (
    "Retail leaves the final stop_flag in EAX (mov eax,[esi+0x2c] before pop/ret), but the "
    "recovered contract is void: no recovered caller reads EAX after Qvm_ExecuteInstructions. "
    "return_value is therefore normalized to null on both sides and the loop state is verified "
    "through instruction_offset, stop_flag and error_status instead. The opcode handlers are "
    "synthetic: retail handler-table first dwords are redirected to a sentinel trap so each real "
    "dispatch site is observed, and both sides apply the same advance/raise policy. The state "
    "pointer and code_base are opaque synthetic addresses; the retail 24-byte opcode table and "
    "the 64-bit host handler-array layout are never compared byte for byte."
)

# (id, categories, code bytes, initial_error_status, initial_stop_flag,
#  instruction stride per dispatch, dispatch limit, raise mode (0 stop_flag, 1 error_status))
CASES = [
    ("qvm-execute-instructions-nominal-single", ["nominal"],
     [0x00], 0, 0, 1, 1, 0),
    ("qvm-execute-instructions-nominal-sequence", ["nominal"],
     [0x01, 0x02, 0x03, 0x04], 0, 0, 1, 3, 0),
    ("qvm-execute-instructions-nominal-repeat-opcode", ["nominal"],
     [0x07, 0x07, 0x07], 0, 0, 1, 3, 0),
    ("qvm-execute-instructions-nominal-error-exit", ["nominal"],
     [0x05, 0x06, 0x07], 0, 0, 1, 2, 1),
    ("qvm-execute-instructions-nominal-error-immediate", ["nominal"],
     [0x08, 0x09], 0, 0, 1, 1, 1),
    ("qvm-execute-instructions-boundary-initial-error", ["boundary"],
     [0x01, 0x02], 1, 1, 1, 5, 0),
    ("qvm-execute-instructions-boundary-max-opcode", ["boundary"],
     [0x30], 0, 0, 1, 1, 0),
    ("qvm-execute-instructions-boundary-zero-opcode", ["boundary"],
     [0x00, 0x00], 0, 0, 1, 2, 0),
    ("qvm-execute-instructions-boundary-stride-four", ["boundary"],
     [0x10, 0x11, 0x12, 0x13, 0x14], 0, 0, 4, 2, 0),
    ("qvm-execute-instructions-boundary-stop-exact", ["boundary"],
     [0x02, 0x03, 0x04], 0, 0, 1, 3, 0),
    ("qvm-execute-instructions-boundary-initial-stop-flag", ["boundary"],
     [0x01], 0, 1, 1, 1, 0),
]


def _packed_code(code: list[int]) -> int:
    packed = 0
    for index, value in enumerate(code):
        packed |= (value & 0xFF) << (8 * index)
    return packed


def cases() -> list[dict]:
    return [
        {
            "id": case_id,
            "categories": categories,
            "packed_code": _packed_code(code),
            "code_size": len(code),
            "initial_error_status": initial_error_status,
            "initial_stop_flag": initial_stop_flag,
            "instruction_stride": instruction_stride,
            "dispatch_limit": dispatch_limit,
            "raise_mode": raise_mode,
            "seed": seed,
        }
        for seed, (case_id, categories, code, initial_error_status, initial_stop_flag,
                   instruction_stride, dispatch_limit, raise_mode) in enumerate(CASES)
    ]


def case_literal(address: int, case: dict) -> str:
    return (
        '        { "%s", 0x%XULL, %dU, %dU, %dU, %dU, %dU, %dU },'
        % (case["id"], case["packed_code"], case["code_size"],
           case["initial_error_status"], case["initial_stop_flag"],
           case["instruction_stride"], case["dispatch_limit"], case["raise_mode"])
    )


def _write_u32(emulator, address: int, value: int) -> None:
    emulator.mem_write(address, struct.pack("<I", value & 0xFFFFFFFF))


def _code_bytes(case: dict) -> bytes:
    return bytes(
        (case["packed_code"] >> (8 * index)) & 0xFF for index in range(case["code_size"]))


def _termination(error_status: int, stop_flag: int) -> str:
    if error_status != 0:
        return "error_status"
    if stop_flag != 0:
        return "stop_flag"
    return "returned_without_stop"


def retail(image, case: dict) -> dict:
    import unicorn

    emulator = BASE.new_emulator(image)

    # Point every retail opcode record's first handler dword at the sentinel so the
    # real `call dword [ecx*8 + 0xa96108]` trap can be observed per dispatch.
    for opcode in range(OPCODE_RECORD_COUNT):
        _write_u32(emulator, OPCODE_HANDLER_TABLE + opcode * OPCODE_RECORD_BYTES, SENTINEL)

    emulator.mem_write(STATE_ADDRESS, bytes(STATE_BYTES))
    _write_u32(emulator, STATE_ADDRESS + STATE_CODE_BASE, CODE_ADDRESS)
    _write_u32(emulator, STATE_ADDRESS + STATE_INSTRUCTION_OFFSET, 0)
    _write_u32(emulator, STATE_ADDRESS + STATE_ERROR_STATUS, case["initial_error_status"])
    _write_u32(emulator, STATE_ADDRESS + STATE_STOP_FLAG, case["initial_stop_flag"])

    code = _code_bytes(case)
    emulator.mem_write(CODE_ADDRESS, code + bytes([0xEE]) * (CODE_BYTES - len(code)))

    BASE.run_with_arguments(emulator, RETAIL_ADDRESS, [STATE_ADDRESS], SENTINEL)

    opcode_trace: list[int] = []
    arguments_canonical = True
    state_argument_matches = True
    dispatches = 0
    instruction_offset = 0
    fault = ""
    while True:
        eip = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EIP)
        esp = emulator.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
        if eip != SENTINEL:
            fault = "unexpected_eip_0x%08x" % eip
            break
        if esp == STACK_TOP + 4:
            break
        if dispatches >= MAX_DISPATCHES:
            fault = "dispatch_limit_exceeded"
            break
        return_address = BASE.read_u32(emulator, esp)
        argument_1 = BASE.read_u32(emulator, esp + 4)
        argument_2 = BASE.read_u32(emulator, esp + 8)
        argument_3 = BASE.read_u32(emulator, esp + 12)
        argument_4 = BASE.read_u32(emulator, esp + 16)
        dispatches += 1
        opcode_trace.append(argument_3)
        if argument_2 != 0 or argument_4 != 0:
            arguments_canonical = False
        if argument_1 != STATE_ADDRESS:
            state_argument_matches = False

        instruction_offset += case["instruction_stride"]
        _write_u32(emulator, STATE_ADDRESS + STATE_INSTRUCTION_OFFSET, instruction_offset)
        if dispatches == case["dispatch_limit"]:
            if case["raise_mode"] == 0:
                _write_u32(emulator, STATE_ADDRESS + STATE_STOP_FLAG, 1)
            else:
                _write_u32(emulator, STATE_ADDRESS + STATE_ERROR_STATUS, 1)

        emulator.reg_write(unicorn.x86_const.UC_X86_REG_ESP, esp + 4)
        try:
            emulator.emu_start(return_address, SENTINEL, 0, 0x100000)
        except unicorn.UcError as error:  # noqa: BLE001 - reported through the observables
            fault = "fault:%s" % error
            break

    final_error_status = BASE.read_u32(emulator, STATE_ADDRESS + STATE_ERROR_STATUS)
    final_stop_flag = BASE.read_u32(emulator, STATE_ADDRESS + STATE_STOP_FLAG)
    final_instruction_offset = BASE.read_u32(
        emulator, STATE_ADDRESS + STATE_INSTRUCTION_OFFSET)

    return {
        "return_value": None,
        "state_delta": {
            "dispatches": dispatches,
            "opcode_trace": opcode_trace,
            "dispatch_arguments_canonical": arguments_canonical,
            "state_argument_matches": state_argument_matches,
            "instruction_offset": final_instruction_offset,
            "stop_flag": final_stop_flag,
            "error_status": final_error_status,
            "termination": _termination(final_error_status, final_stop_flag),
            "fault": fault,
        },
        "side_effects": [],
    }


def candidate_source() -> str:
    return r"""
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "igi/retail/script/qvm_stack_natives.hpp"

namespace
{

using igi::retail::script::QvmNativeStatePrefix;

struct Case
{
    const char* id;
    unsigned long long packed_code;
    std::uint32_t code_size;
    std::uint32_t initial_error_status;
    std::uint32_t initial_stop_flag;
    std::uint32_t instruction_stride;
    std::uint32_t dispatch_limit;
    std::uint32_t raise_mode;
};

struct DispatchPolicy
{
    std::uint32_t instruction_stride;
    std::uint32_t dispatch_limit;
    std::uint32_t raise_mode;
};

DispatchPolicy g_policy{};
QvmNativeStatePrefix* g_expected_state = nullptr;
std::uint32_t g_dispatches = 0;
std::vector<std::uint32_t> g_opcode_trace;
bool g_arguments_canonical = true;
bool g_state_argument_matches = true;

void RecordingOpcodeHandler(QvmNativeStatePrefix* state, std::uint32_t argument_2,
    std::uint32_t opcode, std::uint32_t argument_4)
{
    ++g_dispatches;
    g_opcode_trace.push_back(opcode);
    if (argument_2 != 0U || argument_4 != 0U) { g_arguments_canonical = false; }
    if (state != g_expected_state) { g_state_argument_matches = false; }

    state->instruction_offset += g_policy.instruction_stride;
    if (g_dispatches == g_policy.dispatch_limit)
    {
        if (g_policy.raise_mode == 0U) { state->stop_flag = 1U; }
        else { state->error_status = 1U; }
    }
}

const char* Termination(std::uint32_t error_status, std::uint32_t stop_flag)
{
    if (error_status != 0U) { return "error_status"; }
    if (stop_flag != 0U) { return "stop_flag"; }
    return "returned_without_stop";
}

} // namespace

int main()
{
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        std::array<std::byte, 8U> code{};
        for (std::size_t index = 0U; index < item.code_size; ++index)
        {
            code[index] = static_cast<std::byte>(
                static_cast<std::uint8_t>((item.packed_code >> (8U * index)) & 0xFFULL));
        }

        for (std::size_t opcode = 0U;
             opcode < igi::retail::script::qvm_opcode_handler_count; ++opcode)
        {
            igi::retail::script::QvmOpcodeHandlerTable[opcode].handlers[0] =
                &RecordingOpcodeHandler;
        }

        g_policy.instruction_stride = item.instruction_stride;
        g_policy.dispatch_limit = item.dispatch_limit;
        g_policy.raise_mode = item.raise_mode;
        g_dispatches = 0U;
        g_opcode_trace.clear();
        g_arguments_canonical = true;
        g_state_argument_matches = true;

        igi::retail::script::QvmNativeStatePrefix state{};
        state.code_base = reinterpret_cast<const std::byte*>(code.data());
        state.instruction_offset = 0U;
        state.error_status = item.initial_error_status;
        state.stop_flag = item.initial_stop_flag;
        g_expected_state = &state;

        igi::retail::script::Qvm_ExecuteInstructions(&state);

        std::printf("{\"id\":\"%s\",\"return_value\":null,\"state_delta\":{"
                    "\"dispatches\":%u,\"opcode_trace\":[", item.id,
            static_cast<unsigned>(g_dispatches));
        for (std::size_t index = 0U; index < g_opcode_trace.size(); ++index)
        {
            std::printf("%s%u", index == 0U ? "" : ",",
                static_cast<unsigned>(g_opcode_trace[index]));
        }
        std::printf("],\"dispatch_arguments_canonical\":%s,\"state_argument_matches\":%s,"
                    "\"instruction_offset\":%u,\"stop_flag\":%u,\"error_status\":%u,"
                    "\"termination\":\"%s\",\"fault\":\"\"},\"side_effects\":[]}\n",
            g_arguments_canonical ? "true" : "false",
            g_state_argument_matches ? "true" : "false",
            static_cast<unsigned>(state.instruction_offset),
            static_cast<unsigned>(state.stop_flag),
            static_cast<unsigned>(state.error_status),
            Termination(state.error_status, state.stop_flag));
    }
    return 0;
}
"""


if __name__ == "__main__":
    raise SystemExit("this module is loaded by scripts/native_unicorn_runner.py")
