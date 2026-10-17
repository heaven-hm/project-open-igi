#include "igi/retail/texture/loop_terrain_texture.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using igi::retail::texture::LoopTerrainTextureFile;
using igi::retail::texture::LoopTextureFormatError;

void write_u16(std::vector<std::byte>& bytes, std::size_t offset, std::uint16_t value)
{
    bytes[offset] = static_cast<std::byte>(value & 0xFFU);
    bytes[offset + 1U] = static_cast<std::byte>((value >> 8U) & 0xFFU);
}

void write_u32(std::vector<std::byte>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        bytes[offset + byte_index] = static_cast<std::byte>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

void require(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

std::vector<std::byte> make_loop_texture(std::uint32_t version)
{
    const std::size_t record_stride = version == 7U ? 0x28U : 0x20U;
    std::vector<std::byte> file_bytes(0x34U + record_stride + 4U);
    write_u32(file_bytes, 0U, LoopTerrainTextureFile::loop_signature);
    write_u32(file_bytes, 4U, version);
    write_u32(file_bytes, 0x20U, 1U);
    write_u32(file_bytes, 0x34U, static_cast<std::uint32_t>(file_bytes.size() - 4U));
    write_u32(file_bytes, 0x38U, 0x12345678U);
    if (version == 7U) {
        write_u32(file_bytes, 0x3CU, 0x11111111U);
        write_u32(file_bytes, 0x40U, 0x22222222U);
        write_u32(file_bytes, 0x44U, 0x33333333U);
    } else {
        write_u16(file_bytes, 0x3CU, 0x1111U);
        write_u16(file_bytes, 0x3EU, 0x2222U);
        write_u16(file_bytes, 0x40U, 0x3333U);
    }
    return file_bytes;
}

[[nodiscard]] std::vector<std::byte> read_file(const std::filesystem::path& file_path)
{
    std::ifstream input_stream(file_path, std::ios::binary | std::ios::ate);
    if (!input_stream) {
        throw std::runtime_error("Could not open Level 1 texture: " + file_path.string());
    }

    const std::streamoff file_size = input_stream.tellg();
    if (file_size < 0) {
        throw std::runtime_error("Could not determine texture size: " + file_path.string());
    }
    std::vector<char> input_bytes(static_cast<std::size_t>(file_size));
    input_stream.seekg(0, std::ios::beg);
    input_stream.read(input_bytes.data(), static_cast<std::streamsize>(input_bytes.size()));
    if (!input_stream) {
        throw std::runtime_error("Could not read complete texture: " + file_path.string());
    }

    std::vector<std::byte> file_bytes(input_bytes.size());
    for (std::size_t byte_index = 0U; byte_index < input_bytes.size(); ++byte_index) {
        file_bytes[byte_index] = static_cast<std::byte>(
            static_cast<unsigned char>(input_bytes[byte_index]));
    }
    return file_bytes;
}

void test_version_seven_dispatch_metadata()
{
    const std::vector<std::byte> file_bytes = make_loop_texture(7U);
    const LoopTerrainTextureFile texture_file =
        LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes);

    require(texture_file.version == 7U, "LOOP version 7 is retained");
    require(texture_file.texture_count == 1U && texture_file.records.size() == 1U,
        "LOOP version 7 record count is read");
    require(texture_file.records[0U].data_offset == file_bytes.size() - 4U,
        "LOOP version 7 data offset is read");
    require(texture_file.records[0U].shared_parameter == 0x12345678U,
        "LOOP version 7 shared dispatch parameter is read");
    require(texture_file.records[0U].version_seven_parameters[2U] == 0x33333333U,
        "LOOP version 7 dispatch parameters are read");
}

void test_version_nine_dispatch_metadata()
{
    const std::vector<std::byte> file_bytes = make_loop_texture(9U);
    const LoopTerrainTextureFile texture_file =
        LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes);

    require(texture_file.version == 9U, "LOOP version 9 is retained");
    require(texture_file.records.size() == 1U, "LOOP version 9 record is read");
    require(texture_file.records[0U].version_nine_parameters[2U] == 0x3333U,
        "LOOP version 9 16-bit dispatch parameter is read");
}

void test_palette_payload_matches_native_offsets()
{
    std::vector<std::byte> file_bytes = make_loop_texture(7U);
    constexpr std::uint32_t palette_offset = 0x80U;
    constexpr std::uint32_t palette_size = 4U;
    file_bytes.resize(palette_offset + 0x10U + palette_size);
    write_u32(file_bytes, 0x18U, palette_offset);
    write_u32(file_bytes, palette_offset + 0x0CU, palette_size);
    file_bytes[palette_offset + 0x10U] = std::byte{0x10};
    file_bytes[palette_offset + 0x11U] = std::byte{0x20};
    file_bytes[palette_offset + 0x12U] = std::byte{0x30};
    file_bytes[palette_offset + 0x13U] = std::byte{0x40};

    const LoopTerrainTextureFile texture_file =
        LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes);
    require(texture_file.palette_size == palette_size, "palette byte count uses native field +0x0C");
    require(texture_file.palette_operation_value == palette_size,
        "palette operation receives the native palette byte count");
    require(texture_file.palette_bytes.size() == palette_size
            && texture_file.palette_bytes[3U] == std::byte{0x40},
        "palette bytes begin at the native field +0x10");
}

void test_rejects_unsupported_version_and_truncation()
{
    bool rejected_version = false;
    try {
        const std::vector<std::byte> file_bytes = make_loop_texture(8U);
        static_cast<void>(LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes));
    } catch (const LoopTextureFormatError&) {
        rejected_version = true;
    }
    require(rejected_version, "unsupported LOOP versions are rejected");

    bool rejected_truncation = false;
    try {
        const std::vector<std::byte> file_bytes(0x34U);
        static_cast<void>(LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes));
    } catch (const LoopTextureFormatError&) {
        rejected_truncation = true;
    }
    require(rejected_truncation, "truncated LOOP tables are rejected");
}

void test_level_one_terrain_texture(const std::filesystem::path& texture_path)
{
    const std::vector<std::byte> file_bytes = read_file(texture_path);
    const LoopTerrainTextureFile texture_file =
        LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(file_bytes);
    std::cout << "Level 1 Terrain_TextureFileNotOfRequ verified: file="
              << texture_path << "; bytes=" << file_bytes.size()
              << "; LOOP version=" << texture_file.version
              << "; texture records=" << texture_file.texture_count
              << "; palette bytes=" << texture_file.palette_size << '\n';
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_version_seven_dispatch_metadata();
        test_version_nine_dispatch_metadata();
        test_palette_payload_matches_native_offsets();
        test_rejects_unsupported_version_and_truncation();

        if (argument_count == 2) {
            test_level_one_terrain_texture(arguments[1]);
        } else if (argument_count != 1) {
            throw std::runtime_error("Expected zero arguments or one Level 1 TERRAIN.TEX path.");
        }

        std::cout << "All retail terrain texture metadata tests passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Retail terrain texture test failed: " << exception.what() << '\n';
        return 1;
    }
}
