#include "igi/retail/texture/loop_terrain_texture.hpp"

#include <limits>
#include <string>

namespace igi::retail::texture {
namespace {

constexpr std::size_t loop_header_bytes = 0x34U;
constexpr std::size_t palette_header_bytes = 0x10U;
constexpr std::size_t version_seven_record_bytes = 0x28U;
constexpr std::size_t version_nine_record_bytes = 0x20U;

[[nodiscard]] std::uint16_t read_little_endian_u16(std::span<const std::byte> bytes, std::size_t offset)
{
    if (offset > bytes.size() || sizeof(std::uint16_t) > bytes.size() - offset) {
        throw LoopTextureFormatError("LOOP texture field exceeds the available file bytes.");
    }

    return static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(bytes[offset]))
        | static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(bytes[offset + 1U])) << 8U);
}

[[nodiscard]] std::uint32_t read_little_endian_u32(std::span<const std::byte> bytes, std::size_t offset)
{
    if (offset > bytes.size() || sizeof(std::uint32_t) > bytes.size() - offset) {
        throw LoopTextureFormatError("LOOP texture field exceeds the available file bytes.");
    }

    return std::to_integer<std::uint32_t>(bytes[offset])
        | (std::to_integer<std::uint32_t>(bytes[offset + 1U]) << 8U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 2U]) << 16U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

void require_range(std::size_t offset, std::size_t length, std::size_t limit, const char* description)
{
    if (offset > limit || length > limit - offset) {
        throw LoopTextureFormatError(std::string(description) + " exceeds the LOOP texture file.");
    }
}

} // namespace

LoopTerrainTextureFile LoopTerrainTextureFile::Terrain_TextureFileNotOfRequ(
    std::span<const std::byte> file_bytes)
{
    if (file_bytes.size() < loop_header_bytes) {
        throw LoopTextureFormatError("LOOP texture is shorter than the fields read by retail native 0x005175A0.");
    }

    if (read_little_endian_u32(file_bytes, 0U) != loop_signature) {
        throw LoopTextureFormatError("Retail terrain texture native expected the LOOP signature.");
    }

    LoopTerrainTextureFile texture_file{};
    texture_file.version = read_little_endian_u32(file_bytes, 4U);
    if (texture_file.version != 7U && texture_file.version != 9U) {
        throw LoopTextureFormatError(
            "Retail terrain texture native 0x005175A0 only accepts LOOP versions 7 and 9.");
    }

    texture_file.palette_offset = read_little_endian_u32(file_bytes, 0x18U);
    texture_file.texture_count = read_little_endian_u32(file_bytes, 0x20U);
    if (texture_file.palette_offset != 0U) {
        const std::size_t palette_offset = texture_file.palette_offset;
        require_range(palette_offset, palette_header_bytes, file_bytes.size(), "LOOP palette header");
        texture_file.palette_size = read_little_endian_u32(file_bytes, palette_offset + 0x0CU);
        const std::size_t palette_data_offset = palette_offset + palette_header_bytes;
        require_range(palette_data_offset, texture_file.palette_size, file_bytes.size(), "LOOP palette data");
        texture_file.palette_operation_value = texture_file.palette_size;
        texture_file.palette_bytes.assign(
            file_bytes.begin() + static_cast<std::ptrdiff_t>(palette_data_offset),
            file_bytes.begin()
                + static_cast<std::ptrdiff_t>(palette_data_offset + texture_file.palette_size));
    }

    const std::size_t record_stride = texture_file.version == 7U
        ? version_seven_record_bytes
        : version_nine_record_bytes;
    const std::size_t record_table_offset = loop_header_bytes;
    if (texture_file.texture_count > std::numeric_limits<std::size_t>::max() / record_stride) {
        throw LoopTextureFormatError("LOOP texture record count overflows the host address range.");
    }
    const std::size_t record_table_bytes =
        static_cast<std::size_t>(texture_file.texture_count) * record_stride;
    require_range(record_table_offset, record_table_bytes, file_bytes.size(), "LOOP texture record table");

    texture_file.records.reserve(texture_file.texture_count);
    for (std::size_t record_index = 0U; record_index < texture_file.texture_count; ++record_index) {
        const std::size_t record_offset = record_table_offset + record_index * record_stride;
        LoopTerrainTextureRecord record{};
        record.data_offset = read_little_endian_u32(file_bytes, record_offset);
        record.shared_parameter = read_little_endian_u32(file_bytes, record_offset + 4U);

        if (texture_file.version == 7U) {
            record.version_seven_parameters = {
                read_little_endian_u32(file_bytes, record_offset + 8U),
                read_little_endian_u32(file_bytes, record_offset + 12U),
                read_little_endian_u32(file_bytes, record_offset + 16U),
            };
        } else {
            record.version_nine_parameters = {
                read_little_endian_u16(file_bytes, record_offset + 8U),
                read_little_endian_u16(file_bytes, record_offset + 10U),
                read_little_endian_u16(file_bytes, record_offset + 12U),
            };
        }

        if (record.data_offset >= file_bytes.size()) {
            throw LoopTextureFormatError("LOOP texture record points outside the file data.");
        }
        texture_file.records.push_back(record);
    }

    return texture_file;
}

} // namespace igi::retail::texture
