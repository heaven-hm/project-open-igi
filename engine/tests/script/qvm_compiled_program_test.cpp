#include "igi/retail/script/qvm_compiled_program.hpp"
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/resource/resource_cache.hpp"

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
using igi::retail::script::Qvm_Load;
using igi::retail::script::Qvm_LoadCompiledProgram;
using igi::retail::resource::Is_ResourceLoaded;
using igi::retail::resource::ResourceCache;
using igi::retail::resource::Resource_Load;
using igi::retail::resource::Resource_Unload;

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
    std::vector<std::byte> file_bytes(0x5EU);
    file_bytes[0U] = std::byte{'L'};
    file_bytes[1U] = std::byte{'O'};
    file_bytes[2U] = std::byte{'O'};
    file_bytes[3U] = std::byte{'P'};
    write_uint32(file_bytes, 0x04U, 8U);
    write_uint32(file_bytes, 0x08U, 5U);
    write_uint32(file_bytes, 0x0CU, 0x3CU);
    write_uint32(file_bytes, 0x10U, 0x40U);
    write_uint32(file_bytes, 0x14U, 4U);
    write_uint32(file_bytes, 0x18U, 6U);
    write_uint32(file_bytes, 0x1CU, 0x46U);
    write_uint32(file_bytes, 0x20U, 0x4EU);
    write_uint32(file_bytes, 0x24U, 8U);
    write_uint32(file_bytes, 0x28U, 8U);
    write_uint32(file_bytes, 0x2CU, 0x56U);
    write_uint32(file_bytes, 0x30U, 8U);
    write_uint32(file_bytes, 0x3CU, 0U);
    file_bytes[0x40U] = std::byte{'a'};
    file_bytes[0x41U] = std::byte{'l'};
    file_bytes[0x42U] = std::byte{'p'};
    file_bytes[0x43U] = std::byte{'h'};
    file_bytes[0x44U] = std::byte{'a'};
    file_bytes[0x45U] = std::byte{0U};
    write_uint32(file_bytes, 0x46U, 0U);
    write_uint32(file_bytes, 0x4AU, 4U);
    file_bytes[0x4EU] = std::byte{'o'};
    file_bytes[0x4FU] = std::byte{'n'};
    file_bytes[0x50U] = std::byte{'e'};
    file_bytes[0x51U] = std::byte{0U};
    file_bytes[0x52U] = std::byte{'t'};
    file_bytes[0x53U] = std::byte{'w'};
    file_bytes[0x54U] = std::byte{'o'};
    file_bytes[0x55U] = std::byte{0U};
    for (std::size_t code_index = 0U; code_index < 8U; ++code_index) {
        file_bytes[0x56U + code_index] = static_cast<std::byte>(code_index + 1U);
    }
    return file_bytes;
}

void test_reads_retail_qvm_header_and_code_span()
{
    const std::vector<std::byte> file_bytes = make_valid_qvm();
    const auto program = Qvm_LoadCompiledProgram(file_bytes);
    require(program.format_revision == 8U, "retail QVM revision is preserved");
    require(program.field_08 == 5U, "retail loader's field at +0x08 is preserved");
    require(program.code.size() == 8U, "retail code byte count is read from +0x30");
    require(program.code.front() == std::byte{1U} && program.code.back() == std::byte{8U},
        "code span begins at the dword offset stored at +0x2C");
    require(program.symbol_names.has_value()
            && program.symbol_names->offset_table.begin_offset == 0x3CU
            && program.symbol_names->offset_table.end_offset == 0x40U
            && program.symbol_names->offset_table.bytes.size() == 4U
            && program.symbol_names->index_limit_bytes == 4U
            && program.symbol_names->string_pool.begin_offset == 0x40U
            && program.symbol_names->string_pool.end_offset == 0x46U,
        "retail QVM symbol table and pool follow fields +0x0C through +0x18");
    require(program.string_constants.has_value()
            && program.string_constants->offset_table.begin_offset == 0x46U
            && program.string_constants->offset_table.end_offset == 0x4EU
            && program.string_constants->index_limit_bytes == 8U
            && program.string_constants->string_pool.begin_offset == 0x4EU
            && program.string_constants->string_pool.end_offset == 0x56U,
        "retail QVM string table and pool follow fields +0x1C through +0x28");
}

void test_resolves_indexed_symbol_names_and_string_constants()
{
    const std::vector<std::byte> file_bytes = make_valid_qvm();
    const auto program = Qvm_LoadCompiledProgram(file_bytes);

    require(program.symbol_names->try_get(0U).has_value()
            && *program.symbol_names->try_get(0U) == "alpha",
        "retail symbol offset resolves within the symbol-name pool");
    require(program.string_constants->try_get(0U).has_value()
            && *program.string_constants->try_get(0U) == "one",
        "retail string offset zero resolves within the string-constant pool");
    require(program.string_constants->try_get(1U).has_value()
            && *program.string_constants->try_get(1U) == "two",
        "retail string offset one resolves to the second pooled constant");
    require(!program.symbol_names->try_get(1U).has_value(),
        "retail byte index bound is retained while an out-of-table offset read is rejected safely");
    require(!program.string_constants->try_get(8U).has_value(),
        "string index at the retail table byte limit is rejected");

    std::vector<std::byte> invalid_offset_file = make_valid_qvm();
    write_uint32(invalid_offset_file, 0x3CU, 6U);
    const auto invalid_offset_program = Qvm_LoadCompiledProgram(invalid_offset_file);
    require(!invalid_offset_program.symbol_names->try_get(0U).has_value(),
        "symbol offset at the pool boundary is rejected");
}

void test_rejects_invalid_signature_revision_and_code_range()
{
    std::vector<std::byte> invalid_file = make_valid_qvm();
    invalid_file[0U] = std::byte{'X'};
    bool rejected_signature = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_signature = true;
    }
    require(rejected_signature, "invalid signature is rejected");

    invalid_file = make_valid_qvm();
    write_uint32(invalid_file, 0x04U, 7U);
    bool rejected_revision = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_revision = true;
    }
    require(rejected_revision, "unsupported revision is rejected");

    invalid_file = make_valid_qvm();
    write_uint32(invalid_file, 0x2CU, 0xFFFFFFF0U);
    bool rejected_range = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_range = true;
    }
    require(rejected_range, "out-of-file bytecode range is rejected");

    invalid_file = make_valid_qvm();
    write_uint32(invalid_file, 0x10U, 0xFFFFFFF0U);
    bool rejected_table_range = false;
    try {
        static_cast<void>(Qvm_LoadCompiledProgram(invalid_file));
    } catch (const QvmFormatError&) {
        rejected_table_range = true;
    }
    require(rejected_table_range, "out-of-file QVM table range is rejected");
}

void test_resource_load_returns_stable_resource_memory_address()
{
    const std::filesystem::path resource_path = std::filesystem::temp_directory_path()
        / "igi_qvm_resource_address_test.qvm";
    const std::vector<std::byte> file_bytes = make_valid_qvm();

    try {
        std::ofstream output_stream(resource_path, std::ios::binary | std::ios::trunc);
        require(static_cast<bool>(output_stream), "temporary QVM resource file can be created");
        output_stream.write(
            reinterpret_cast<const char*>(file_bytes.data()),
            static_cast<std::streamsize>(file_bytes.size()));
        require(static_cast<bool>(output_stream), "temporary QVM resource bytes can be written");
        output_stream.close();

        ResourceCache resource_cache{};
        const std::byte* resource_data = nullptr;
        const auto loaded_resource = Resource_Load(resource_cache, resource_path, &resource_data);
        require(resource_data == loaded_resource->data(),
            "Resource_Load out pointer is the loaded resource byte address");
        require(loaded_resource->size() == file_bytes.size(),
            "Resource_Load reports the complete QVM resource size");
        require(Is_ResourceLoaded(resource_cache, resource_path).get() == loaded_resource.get(),
            "Is_ResourceLoaded returns the cached resource entry");

        const std::byte* cached_resource_data = nullptr;
        const auto cached_resource = Resource_Load(resource_cache, resource_path, &cached_resource_data);
        require(cached_resource.get() == loaded_resource.get()
                && cached_resource_data == resource_data
                && resource_cache.size() == 1U,
            "Resource_Load reuses the same cached entry and memory address");
        require(Resource_Unload(resource_cache, resource_path),
            "Resource_Unload removes the cached entry");
        require(resource_cache.size() == 0U && loaded_resource->data() == resource_data,
            "an active resource handle keeps its loaded memory valid after registry unload");
    } catch (...) {
        std::error_code remove_error;
        std::filesystem::remove(resource_path, remove_error);
        throw;
    }

    std::error_code remove_error;
    std::filesystem::remove(resource_path, remove_error);
}

void test_level_one_qvm_corpus(const std::filesystem::path& level_directory)
{
    std::size_t qvm_file_count = 0U;
    ResourceCache resource_cache{};
    const auto verify_file = [&qvm_file_count, &resource_cache](const std::filesystem::path& file_path) {
        const auto loaded_script = Qvm_Load(resource_cache, file_path);
        const auto& program = loaded_script.program();
        const std::span<const std::byte> file_bytes = loaded_script.file_bytes();
        require(!program.code.empty(), "Level 1 QVM has a nonempty retail code region");
        require(program.code.data() >= file_bytes.data()
                && program.code.data() + program.code.size() <= file_bytes.data() + file_bytes.size(),
            "Qvm_Load bytecode remains inside the Resource_Load memory address range");
        if (program.symbol_names.has_value()) {
            require(program.symbol_names->offset_table.bytes.size()
                        == program.symbol_names->offset_table.end_offset
                            - program.symbol_names->offset_table.begin_offset,
                "Level 1 QVM symbol offset table remains within its declared bounds");
            for (std::uint32_t entry_index = 0U;
                 entry_index < program.symbol_names->index_limit_bytes;
                 ++entry_index) {
                static_cast<void>(program.symbol_names->try_get(entry_index));
            }
        }
        if (program.string_constants.has_value()) {
            require(program.string_constants->offset_table.bytes.size()
                        == program.string_constants->offset_table.end_offset
                            - program.string_constants->offset_table.begin_offset,
                "Level 1 QVM string offset table remains within its declared bounds");
            for (std::uint32_t entry_index = 0U;
                 entry_index < program.string_constants->index_limit_bytes;
                 ++entry_index) {
                static_cast<void>(program.string_constants->try_get(entry_index));
            }
        }
        std::cout << "Qvm_Load accepted " << file_path.filename().string()
                  << ": revision=" << program.format_revision
                  << ", symbol_index_limit_bytes="
                  << (program.symbol_names.has_value()
                          ? program.symbol_names->index_limit_bytes
                          : 0U)
                  << ", string_index_limit_bytes="
                  << (program.string_constants.has_value()
                          ? program.string_constants->index_limit_bytes
                          : 0U)
                  << ", resource_address=" << static_cast<const void*>(file_bytes.data())
                  << ", resource_bytes=" << file_bytes.size()
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

    std::cout << "Level 1 QVM resources loaded: " << qvm_file_count
              << ", resource cache entries=" << resource_cache.size() << '\n';
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_reads_retail_qvm_header_and_code_span();
        test_resolves_indexed_symbol_names_and_string_constants();
        test_rejects_invalid_signature_revision_and_code_range();
        test_resource_load_returns_stable_resource_memory_address();
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
