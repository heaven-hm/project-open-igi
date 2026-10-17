#include "igi/retail/script/qvm_compiled_program.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

using igi::retail::script::QvmFormatError;
using igi::retail::script::Qvm_LoadCompiledProgram_004BD220;

void require(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void write_uint32(std::vector<std::byte>& file_bytes, std::size_t field_offset, std::uint32_t field_value)
{
    for (std::size_t byte_index = 0U; byte_index < sizeof(field_value); ++byte_index) {
        file_bytes[field_offset + byte_index] = static_cast<std::byte>(
            (field_value >> static_cast<unsigned int>(byte_index * 8U)) & 0xFFU);
    }
}

[[nodiscard]] std::vector<std::byte> make_valid_qvm()
{
    std::vector<std::byte> file_bytes(0x48U);
    file_bytes[0U] = std::byte{'L'};
    file_bytes[1U] = std::byte{'O'};
    file_bytes[2U] = std::byte{'O'};
    file_bytes[3U] = std::byte{'P'};
    write_uint32(file_bytes, 0x04U, 8U);
    write_uint32(file_bytes, 0x08U, 5U);
    write_uint32(file_bytes, 0x2CU, 0x40U);
    write_uint32(file_bytes, 0x30U, 8U);
    for (std::size_t code_index = 0U; code_index < 8U; ++code_index) {
        file_bytes[0x40U + code_index] = static_cast<std::byte>(code_index + 1U);
    }
    return file_bytes;
}

void test_reads_retail_qvm_header_and_code_span()
{
    const std::vector<std::byte> file_bytes = make_valid_qvm();
    const auto program = Qvm_LoadCompiledProgram_004BD220(file_bytes);
    require(program.format_revision == 8U, "retail QVM revision is preserved");
    require(program.field_08 == 5U, "retail loader's field at +0x08 is preserved");
    require(program.code.size() == 8U, "retail code byte count is read from +0x30");
    require(program.code.front() == std::byte{1U} && program.code.back() == std::byte{8U},
        "code span begins at the dword offset stored at +0x2C");
}

void test_rejects_invalid_signature_revision_and_code_range()
{
    std::vector<std::byte> invalid_file = make_valid_qvm();
    invalid_file[0U] = std::byte{'X'};
    bool rejected_signature = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram_004BD220(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_signature = true;
    }
    require(rejected_signature, "invalid signature is rejected");

    invalid_file = make_valid_qvm();
    write_uint32(invalid_file, 0x04U, 7U);
    bool rejected_revision = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram_004BD220(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_revision = true;
    }
    require(rejected_revision, "unsupported revision is rejected");

    invalid_file = make_valid_qvm();
    write_uint32(invalid_file, 0x2CU, 0xFFFFFFF0U);
    bool rejected_range = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram_004BD220(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_range = true;
    }
    require(rejected_range, "out-of-file bytecode range is rejected");
}

void test_level_one_qvm_corpus(const std::filesystem::path& level_directory)
{
    std::size_t qvm_file_count = 0U;
    const auto verify_file = [&qvm_file_count](const std::filesystem::path& file_path) {
        std::ifstream input_stream(file_path, std::ios::binary);
        if (!input_stream) {
            throw std::runtime_error("Could not open QVM file: " + file_path.string());
        }
        const std::vector<char> raw_bytes{
            std::istreambuf_iterator<char>(input_stream),
            std::istreambuf_iterator<char>(),
        };
        const auto* byte_data = reinterpret_cast<const std::byte*>(raw_bytes.data());
        const std::span<const std::byte> file_bytes(byte_data, raw_bytes.size());
        const auto program = Qvm_LoadCompiledProgram_004BD220(file_bytes);
        require(!program.code.empty(), "Level 1 QVM has a nonempty retail code region");
        std::cout << "Accepted " << file_path.filename().string()
                  << ": revision=" << program.format_revision
                  << ", code_bytes=" << program.code.size() << '\n';
        ++qvm_file_count;
    };

    verify_file(level_directory / "OBJECTS.QVM");
    verify_file(level_directory / "MISSION.QVM");
    for (const std::filesystem::directory_entry& entry :
         std::filesystem::directory_iterator(level_directory / "AI")) {
        if (entry.is_regular_file() && entry.path().extension() == ".QVM") {
            verify_file(entry.path());
        }
    }

    std::cout << "Level 1 compiled QVM files accepted: " << qvm_file_count << '\n';
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_reads_retail_qvm_header_and_code_span();
        test_rejects_invalid_signature_revision_and_code_range();
        if (argument_count > 1) {
            test_level_one_qvm_corpus(std::filesystem::path(arguments[1]));
        }
        std::cout << "QVM compiled-program tests passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "QVM compiled-program tests failed: " << exception.what() << '\n';
        return 1;
    }
}
