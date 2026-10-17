#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace igi::retail::script
{

// Native context prefix: recovered fields only; this is not the complete retail QVM object.
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
static_assert(offsetof(QvmNativeStatePrefix, instruction_offset) == 0x14U);
static_assert(offsetof(QvmNativeStatePrefix, value_stack_index) == 0x18U);
static_assert(offsetof(QvmNativeStatePrefix, type_stack_index) == 0x1CU);
static_assert(offsetof(QvmNativeStatePrefix, value_stack) == 0x24U);
static_assert(offsetof(QvmNativeStatePrefix, type_stack) == 0x28U);
static_assert(offsetof(QvmNativeStatePrefix, stop_flag) == 0x2CU);
static_assert(sizeof(QvmNativeStatePrefix) == 0x30U);

} // namespace igi::retail::script
