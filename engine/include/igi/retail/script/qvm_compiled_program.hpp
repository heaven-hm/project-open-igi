#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>

namespace igi::retail::script
{

struct QvmFileRegion
{
    std::uint32_t begin_offset = 0U;
    std::uint32_t end_offset = 0U;
    std::span<const std::byte> bytes;
};

struct QvmIndexedStringTable
{
    QvmFileRegion offset_table;
    QvmFileRegion string_pool;
    std::uint32_t index_limit_bytes = 0U;

    // Port helper for retail retail routine 0x004BEC40 at VA 0x004BEC40.
    [[nodiscard]] std::optional<std::string_view> try_get(std::uint32_t entry_index) const noexcept;
};

class QvmFormatError final : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

struct QvmCompiledProgram
{
    std::uint32_t format_revision = 0U;
    std::uint32_t field_08 = 0U;
    std::array<std::uint32_t, 8U> fields_0c_through_28{};
    std::optional<QvmIndexedStringTable> symbol_names;
    std::optional<QvmIndexedStringTable> string_constants;
    std::span<const std::byte> code;
    std::uint32_t field_34 = 0U;
    std::uint32_t field_38 = 0U;
};

// Qvm_LoadCompiledProgram - retail helper at original IGI.EXE address 0x004BD220.
// REA/Ghidra evidence ev_bd052fcbe23ffdb47535d836372b20f7c7e07a937dfd95604e67a7ccf6b29747:
// accepts a LOOP file, revision 8, field +0x08 > 4, and file length > 0x3B;
// code begins at base + dword[0x2C] and has dword[0x30] bytes.
// Static retail behavior is established; C++ runtime parity is not yet verified.
[[nodiscard]] QvmCompiledProgram Qvm_LoadCompiledProgram(std::span<const std::byte> file_bytes);

} // namespace igi::retail::script
