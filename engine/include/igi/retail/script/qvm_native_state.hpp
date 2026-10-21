#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace igi::retail::script
{

// Layout: partial 32-bit QVM state prefix observed in retail QVM dispatch and stack helpers.
// Evidence: REA/Ghidra ev_7b5b04f8089dae45a2e6a5b52cdcec6c771948bcf7b02c21d635a76164439de4 and
// ev_fc8c4a96e5363fb9ba98aad334c2607004b6ee4cd58f1894cf78e5d43a93a525; REA ranges 0x004BCFD0 and 0x004BDA00.
// Fields: instruction offset +0x00; code pointer +0x14; value/type indexes +0x18/+0x1C;
// status +0x20; value/type stack pointers +0x24/+0x28; stop flag +0x2C.
// ABI: recovered from x86 dword offsets; static assertions apply to the Windows x86 build.
// Confidence: high for these accessed fields and offsets; total object size and intervening field meanings are unknown.
// Tests: QVM stack and dispatcher tests validate accessed offsets; full compiled-script execution remains pending.
struct QvmNativeStatePrefix
{
    const std::byte* code_base;                       // +0x00
    std::array<std::uint32_t, 4U> unknown_04_through_10; // +0x04..+0x10
    std::uint32_t instruction_offset;                // +0x14
    std::int32_t value_stack_index;                  // +0x18
    std::int32_t type_stack_index;                   // +0x1C
    std::uint32_t error_status;                      // +0x20
    std::uint32_t* value_stack;                      // +0x24
    std::uint32_t* type_stack;                       // +0x28
    std::uint32_t stop_flag;                         // +0x2C
};

static_assert(std::is_standard_layout_v<QvmNativeStatePrefix>);

// The recovered layout is the retail 32-bit ABI. On a 64-bit host the pointers are wider, so the
// byte offsets cannot hold; the offsets are only checked on the i386 target.
#if defined(_M_IX86) || defined(__i386__)
static_assert(offsetof(QvmNativeStatePrefix, instruction_offset) == 0x14U);
static_assert(offsetof(QvmNativeStatePrefix, value_stack_index) == 0x18U);
static_assert(offsetof(QvmNativeStatePrefix, type_stack_index) == 0x1CU);
static_assert(offsetof(QvmNativeStatePrefix, value_stack) == 0x24U);
static_assert(offsetof(QvmNativeStatePrefix, type_stack) == 0x28U);
static_assert(offsetof(QvmNativeStatePrefix, stop_flag) == 0x2CU);
static_assert(sizeof(QvmNativeStatePrefix) == 0x30U);
#endif

} // namespace igi::retail::script
