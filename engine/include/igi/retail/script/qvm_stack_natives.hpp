#pragma once

#include "igi/retail/script/qvm_native_state.hpp"

#include <cstdint>
#include <array>

namespace igi::retail::script
{

using QvmOpcodeHandler = void (*)(
    QvmNativeStatePrefix* state,
    std::uint32_t argument_2,
    std::uint32_t opcode,
    std::uint32_t argument_4);

struct QvmOpcodeHandlerRecord
{
    std::array<QvmOpcodeHandler, 6U> handlers;
};

// Retail Qvm_InitializeOpcodeHandlers at 0x004BD430 iterates opcode values 0..0x30.
// Evidence: REA/Ghidra ev_706918aa1c654363bf4c883f76003d858e55d4475ebd6d490f31a67f1b8215ed.
// Each registration call uses six DWORD slots per opcode, as observed in Qvm_SetOpcodeHandler at 0x004BD3D0.
// Storage: relocated process-owned data; this C++ array is not a dereference of retail VA 0x00A96108.
// Confidence: record count, field stride and address arithmetic are static observations; initialization is incomplete.
// Tests: handler-slot selection and dispatcher execution are tested; all 49 retail opcode mappings are not installed.
inline constexpr std::size_t qvm_opcode_handler_count = 0x31U;
extern QvmOpcodeHandlerRecord QvmOpcodeHandlerTable[qvm_opcode_handler_count];

// Native: Qvm_PushInteger() - retail VA 0x004BDA00; Binary Ninja label Sys_PushStack_29.
// Behavior: decrements the +0x18/+0x1C stack indices, writes integer type tag 1, then stores the DWORD.
// Evidence: REA/Ghidra ev_fc8c4a96e5363fb9ba98aad334c2607004b6ee4cd58f1894cf78e5d43a93a525; byte range 0x2A.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; source CFG agreement 100% (structure only).
// Confidence: high for source boundary and observed writes; the full QVM context and interpreter remain partial.
// Signature: x86 cdecl, state pointer then 32-bit value; prefix layout is asserted for Windows x86.
// Usage/tests: called by retail constant-push opcodes; focused stack/index/tag test added; in-engine QVM dispatch pending.
// The retail body is one basic block that ends with `mov [value_stack + index*4], eax`, so EAX - the
// register a caller sees after the call - still holds the pushed value. The port returns it explicitly
// instead of leaving the post-call register to the host compiler's allocation.
[[nodiscard]] std::uint32_t Qvm_PushInteger(QvmNativeStatePrefix* state, std::uint32_t value);

// Native: Qvm_SetOpcodeHandler() - retail VA 0x004BD3D0; Binary Ninja label Sys_AllocateMemory_29.
// Behavior: stores the callback at handler_table + (variant + opcode * 6) * 4 without bounds checks.
// Evidence: REA/Ghidra ev_5cda7406ec531b3d34c1241bac86c40d46966b0ae4b27c81dfbd92c510cb8dcf; 26-byte body.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; source CFG agreement 100% (structure only).
// Confidence: high for address, arithmetic and parameter order; the Binary Ninja catalog label conflicts with behavior.
// Signature: x86 cdecl, opcode, variant, callback; C++ stores callbacks in a relocated x86 6-slot record table.
// Usage/tests: QVM initialization registers opcode handlers; focused table-index test added, full initializer pending.
void Qvm_SetOpcodeHandler(std::int32_t opcode, std::int32_t variant, QvmOpcodeHandler handler);

// Native: Qvm_ExecuteInstructions() - retail VA 0x004BCFD0; Binary Ninja label Sys_ExecuteBytecode_28.
// Behavior: clears +0x2C, checks status +0x20, dispatches the byte at code_base + instruction_offset, and repeats until stopped.
// Evidence: REA/Ghidra ev_7b5b04f8089dae45a2e6a5b52cdcec6c771948bcf7b02c21d635a76164439de4; body is 59 bytes.
// CFG: radare2 4 blocks/5 edges; Binary Ninja 4 blocks; source CFG agreement 100% (not semantic accuracy).
// Confidence: high for loop, offsets and callback arguments; table initialization and all retail opcodes remain incomplete.
// Signature: x86 cdecl with one state pointer; dispatch callback is cdecl(state, 0, opcode, 0), caller cleans 16 bytes.
// Usage/tests: called by retail QVM expression execution; dispatch loop and error/stop cases have focused tests.
// Retail's exit paths load the terminating field into EAX before returning: error_status on the
// error path and stop_flag on the loop exit. The port returns that value so the post-call register
// carries the termination reason instead of host-compiler residue.
[[nodiscard]] std::uint32_t Qvm_ExecuteInstructions(QvmNativeStatePrefix* state);

} // namespace igi::retail::script
