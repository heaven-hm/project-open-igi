#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>

namespace igi::retail::script {

struct QvmFileRegion {
    std::uint32_t begin_offset = 0U;
    std::uint32_t end_offset = 0U;
    std::span<const std::byte> bytes;
};

class QvmFormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct QvmCompiledProgram {
    std::uint32_t format_revision = 0U;
    std::uint32_t field_08 = 0U;
    std::array<std::uint32_t, 8U> fields_0c_through_28{};
    std::optional<QvmFileRegion> first_header_referenced_region;
    std::optional<QvmFileRegion> second_header_referenced_region;
    std::span<const std::byte> code;
    std::uint32_t field_34 = 0U;
    std::uint32_t field_38 = 0U;
};

// Qvm_LoadCompiledProgram_004BD220 - original IGI.EXE address 0x004BD220.
// REA/Ghidra evidence ev_bd052fcbe23ffdb47535d836372b20f7c7e07a937dfd95604e67a7ccf6b29747:
// accepts a LOOP file, revision 8, field +0x08 > 4, and file length > 0x3B;
// code begins at base + dword[0x2C] and has dword[0x30] bytes.
// Static retail behavior is established; C++ runtime parity is not yet verified.
[[nodiscard]] QvmCompiledProgram Qvm_LoadCompiledProgram_004BD220(
    std::span<const std::byte> file_bytes);

} // namespace igi::retail::script
