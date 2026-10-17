#include "igi/retail/script/qvm_compiled_program.hpp"

#include <algorithm>

namespace igi::retail::script {
namespace {

constexpr std::size_t retail_qvm_header_minimum_size = 0x3CU;
constexpr std::uint32_t retail_qvm_format_revision = 8U;
constexpr std::size_t field_08_offset = 0x08U;
constexpr std::size_t field_table_offset = 0x0CU;
constexpr std::size_t field_table_count = 8U;
constexpr std::size_t code_offset_field_offset = 0x2CU;
constexpr std::size_t code_size_field_offset = 0x30U;
constexpr std::size_t field_34_offset = 0x34U;
constexpr std::size_t field_38_offset = 0x38U;

[[nodiscard]] std::uint32_t read_little_endian_uint32(
    std::span<const std::byte> file_bytes,
    std::size_t field_offset)
{
    if (field_offset > file_bytes.size() || sizeof(std::uint32_t) > file_bytes.size() - field_offset) {
        throw QvmFormatError("QVM header field extends beyond the available file bytes.");
    }

    std::uint32_t field_value = 0U;
    for (std::size_t byte_index = 0U; byte_index < sizeof(field_value); ++byte_index) {
        field_value |= std::to_integer<std::uint32_t>(file_bytes[field_offset + byte_index])
            << static_cast<unsigned int>(byte_index * 8U);
    }
    return field_value;
}

} // namespace

QvmCompiledProgram Qvm_LoadCompiledProgram_004BD220(std::span<const std::byte> file_bytes)
{
    if (file_bytes.size() < retail_qvm_header_minimum_size) {
        throw QvmFormatError("QVM file is shorter than the retail loader's minimum header size.");
    }

    constexpr std::array<std::byte, 4U> retail_qvm_signature = {
        std::byte{'L'}, std::byte{'O'}, std::byte{'O'}, std::byte{'P'},
    };
    if (!std::equal(retail_qvm_signature.begin(), retail_qvm_signature.end(), file_bytes.begin())) {
        throw QvmFormatError("QVM file does not have the retail LOOP signature.");
    }

    QvmCompiledProgram program{};
    program.format_revision = read_little_endian_uint32(file_bytes, 0x04U);
    program.field_08 = read_little_endian_uint32(file_bytes, field_08_offset);
    if (program.format_revision != retail_qvm_format_revision || program.field_08 <= 4U) {
        throw QvmFormatError("QVM header values do not pass the retail compiled-program checks.");
    }

    for (std::size_t field_index = 0U; field_index < field_table_count; ++field_index) {
        program.fields_0c_through_28[field_index] = read_little_endian_uint32(
            file_bytes,
            field_table_offset + field_index * sizeof(std::uint32_t));
    }

    const std::uint32_t code_offset = read_little_endian_uint32(file_bytes, code_offset_field_offset);
    const std::uint32_t code_size = read_little_endian_uint32(file_bytes, code_size_field_offset);
    const std::size_t code_start = static_cast<std::size_t>(code_offset);
    const std::size_t code_length = static_cast<std::size_t>(code_size);
    if (code_start > file_bytes.size() || code_length > file_bytes.size() - code_start) {
        throw QvmFormatError("QVM bytecode range extends beyond the available file bytes.");
    }

    program.code = file_bytes.subspan(code_start, code_length);
    program.field_34 = read_little_endian_uint32(file_bytes, field_34_offset);
    program.field_38 = read_little_endian_uint32(file_bytes, field_38_offset);
    return program;
}

} // namespace igi::retail::script
