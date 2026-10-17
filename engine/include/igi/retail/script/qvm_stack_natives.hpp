#pragma once

#include "igi/retail/script/qvm_native_state.hpp"

#include <cstdint>

namespace igi::retail::script
{

// Native: Qvm_PushInteger() - retail VA 0x004BDA00; Binary Ninja label Sys_PushStack_29.
// Behavior: decrements the +0x18/+0x1C stack indices, writes integer type tag 1, then stores the DWORD.
// Evidence: REA/Ghidra ev_fc8c4a96e5363fb9ba98aad334c2607004b6ee4cd58f1894cf78e5d43a93a525; byte range 0x2A.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; source CFG agreement 100% (structure only).
// Confidence: high for source boundary and observed writes; the full QVM context and interpreter remain partial.
// Signature: x86 cdecl, state pointer then 32-bit value; prefix layout is asserted for Windows x86.
// Usage/tests: called by retail constant-push opcodes; focused stack/index/tag test added; in-engine QVM dispatch pending.
void Qvm_PushInteger(QvmNativeStatePrefix* state, std::uint32_t value);

} // namespace igi::retail::script
