#include "igi/retail/script/qvm_stack_natives.hpp"

namespace igi::retail::script
{

// Native: Qvm_PushInteger() - retail VA 0x004BDA00; Binary Ninja label Sys_PushStack_29.
// Behavior: stores tag 1 at type_stack[--type_stack_index] and the supplied DWORD at value_stack[--value_stack_index].
// Evidence: REA/Ghidra ev_fc8c4a96e5363fb9ba98aad334c2607004b6ee4cd58f1894cf78e5d43a93a525; bytes verified against IGI.EXE.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; source CFG agreement 100% (not C++ semantic accuracy).
// Confidence: high for this leaf's writes; caller state allocation and complete VM integration are not ported.
// Signature: x86 cdecl with two 32-bit stack arguments; current declaration retains the observed argument order.
// Usage/tests: opcode handlers call this native; unit test checks both index decrements and both stack writes.
void Qvm_PushInteger(QvmNativeStatePrefix* state, std::uint32_t value)
{
    const std::int32_t next_type_stack_index = state->type_stack_index - 1;
    const std::int32_t next_value_stack_index = state->value_stack_index - 1;

    state->value_stack_index = next_value_stack_index;
    state->type_stack_index = next_type_stack_index;
    state->type_stack[next_type_stack_index] = 1U;
    state->value_stack[next_value_stack_index] = value;
}

} // namespace igi::retail::script
