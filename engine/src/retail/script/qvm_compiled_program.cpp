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

// Qvm_LoadCompiledProgram at 0x004BD220 installs base + fields +0x0C/+0x10
// when the gate at +0x14 is nonzero, and base + fields +0x1C/+0x20 when +0x24 is
// nonzero. REA/Ghidra evidence: ev_93c3331eb57f910f0727ac4a0e85af7b06148864571701268d2c22b709f7763d.
[[nodiscard]] QvmFileRegion read_file_region(
    std::span<const std::byte> file_bytes,
    std::uint32_t begin_offset,
    std::uint32_t end_offset)
{
    if (end_offset < begin_offset || static_cast<std::size_t>(end_offset) > file_bytes.size()) {
        throw QvmFormatError("QVM table region has an invalid or out-of-file boundary.");
    }

    const std::size_t begin = static_cast<std::size_t>(begin_offset);
    const std::size_t length = static_cast<std::size_t>(end_offset - begin_offset);
    return {begin_offset, end_offset, file_bytes.subspan(begin, length)};
}

[[nodiscard]] std::optional<QvmIndexedStringTable> read_indexed_string_table(
    std::span<const std::byte> file_bytes,
    std::uint32_t table_begin_offset,
    std::uint32_t table_end_offset,
    std::uint32_t table_size_bytes,
    std::uint32_t string_pool_size)
{
    if (table_size_bytes == 0U) {
        return std::nullopt;
    }

    if (table_end_offset < table_begin_offset
        || table_end_offset - table_begin_offset != table_size_bytes) {
        throw QvmFormatError("QVM indexed string table boundaries do not match the retail byte size.");
    }

    const std::uint64_t string_pool_end =
        static_cast<std::uint64_t>(table_end_offset) + string_pool_size;
    if (string_pool_end > file_bytes.size()) {
        throw QvmFormatError("QVM indexed string pool extends beyond the available file bytes.");
    }

    const std::uint32_t pool_end_offset = static_cast<std::uint32_t>(string_pool_end);
    return QvmIndexedStringTable{
        .offset_table = read_file_region(file_bytes, table_begin_offset, table_end_offset),
        .string_pool = read_file_region(file_bytes, table_end_offset, pool_end_offset),
        .index_limit_bytes = table_size_bytes,
    };
}

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

std::optional<std::string_view> QvmIndexedStringTable::try_get(std::uint32_t entry_index) const noexcept
{
    if (entry_index >= index_limit_bytes) {
        return std::nullopt;
    }

    const std::size_t entry_offset = static_cast<std::size_t>(entry_index) * sizeof(std::uint32_t);
    if (entry_offset > offset_table.bytes.size()
        || sizeof(std::uint32_t) > offset_table.bytes.size() - entry_offset) {
        return std::nullopt;
    }

    std::uint32_t string_offset = 0U;
    for (std::size_t byte_index = 0U; byte_index < sizeof(string_offset); ++byte_index) {
        string_offset |= std::to_integer<std::uint32_t>(offset_table.bytes[entry_offset + byte_index])
            << static_cast<unsigned int>(byte_index * 8U);
    }
    if (string_offset >= string_pool.bytes.size()) {
        return std::nullopt;
    }

    const std::size_t string_begin = static_cast<std::size_t>(string_offset);
    const char* string_data = reinterpret_cast<const char*>(string_pool.bytes.data() + string_begin);
    const char* string_pool_end = reinterpret_cast<const char*>(
        string_pool.bytes.data() + string_pool.bytes.size());
    const char* string_end = std::find(string_data, string_pool_end, '\0');
    return std::string_view(string_data, static_cast<std::size_t>(string_end - string_data));
}

QvmCompiledProgram Qvm_LoadCompiledProgram(std::span<const std::byte> file_bytes)
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

    if (program.fields_0c_through_28[2U] != 0U) {
        program.symbol_names = read_indexed_string_table(
            file_bytes,
            program.fields_0c_through_28[0U],
            program.fields_0c_through_28[1U],
            program.fields_0c_through_28[2U],
            program.fields_0c_through_28[3U]);
    }
    if (program.fields_0c_through_28[6U] != 0U) {
        program.string_constants = read_indexed_string_table(
            file_bytes,
            program.fields_0c_through_28[4U],
            program.fields_0c_through_28[5U],
            program.fields_0c_through_28[6U],
            program.fields_0c_through_28[7U]);
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
