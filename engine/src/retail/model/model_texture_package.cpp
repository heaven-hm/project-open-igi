#include "igi/retail/model/model_texture_package.hpp"

#include <limits>
#include <string_view>

namespace igi::retail::model
{
namespace
{

constexpr std::uint32_t mods_tag = 0x4D4F4453U;
constexpr std::uint32_t vnam_tag = 0x564E414DU;
constexpr std::uint32_t inst_tag = 0x494E5354U;
constexpr std::uint32_t gtt_tag = 0x47545420U;
constexpr std::uint32_t banm_tag = 0x42414E4DU;
constexpr std::size_t root_header_bytes = 12U;
constexpr std::size_t child_header_bytes = 8U;

struct NativeChunkView
{
    std::uint32_t tag = 0U;
    std::span<const std::byte> payload;
};

[[nodiscard]] std::uint32_t read_u32(std::span<const std::byte> bytes, std::size_t offset)
{
    if (offset > bytes.size() || sizeof(std::uint32_t) > bytes.size() - offset)
    {
        throw MtpFormatError("MTP field extends past the available bytes.");
    }
    return std::to_integer<std::uint32_t>(bytes[offset]) |
           (std::to_integer<std::uint32_t>(bytes[offset + 1U]) << 8U) |
           (std::to_integer<std::uint32_t>(bytes[offset + 2U]) << 16U) |
           (std::to_integer<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

[[nodiscard]] std::uint32_t read_big_endian_u32(std::span<const std::byte> bytes, std::size_t offset)
{
    if (offset > bytes.size() || sizeof(std::uint32_t) > bytes.size() - offset)
    {
        throw MtpFormatError("MTP container field extends past the available bytes.");
    }
    return (std::to_integer<std::uint32_t>(bytes[offset]) << 24U) |
           (std::to_integer<std::uint32_t>(bytes[offset + 1U]) << 16U) |
           (std::to_integer<std::uint32_t>(bytes[offset + 2U]) << 8U) |
           std::to_integer<std::uint32_t>(bytes[offset + 3U]);
}

[[nodiscard]] std::size_t align_four(std::size_t value)
{
    if (value > std::numeric_limits<std::size_t>::max() - 3U)
    {
        throw MtpFormatError("MTP child alignment overflows the host address range.");
    }
    return (value + 3U) & ~std::size_t{3U};
}

[[nodiscard]] std::vector<NativeChunkView> read_native_chunks(std::span<const std::byte> file_bytes,
                                                              std::uint32_t& root_tag)
{
    if (file_bytes.size() < root_header_bytes)
    {
        throw MtpFormatError("MTP file is shorter than the retail form header.");
    }
    root_tag = read_big_endian_u32(file_bytes, 0U);
    const std::uint32_t declared_size = read_big_endian_u32(file_bytes, 4U);
    const std::size_t form_end = static_cast<std::size_t>(declared_size) + 8U;
    if (form_end < root_header_bytes || form_end > file_bytes.size())
    {
        throw MtpFormatError("MTP form size is outside the available file range.");
    }
    std::vector<NativeChunkView> chunks;
    std::size_t chunk_offset = root_header_bytes;
    while (chunk_offset < form_end)
    {
        if (child_header_bytes > form_end - chunk_offset)
        {
            throw MtpFormatError("MTP child header is truncated at the end of its form.");
        }

        const std::uint32_t chunk_tag = read_big_endian_u32(file_bytes, chunk_offset);
        const std::uint32_t payload_size = read_big_endian_u32(file_bytes, chunk_offset + 4U);
        const std::size_t payload_offset = chunk_offset + child_header_bytes;
        if (payload_size > form_end - payload_offset)
        {
            throw MtpFormatError("MTP child payload exceeds its parent form.");
        }
        chunks.push_back({
            chunk_tag,
            file_bytes.subspan(payload_offset, static_cast<std::size_t>(payload_size)),
        });

        const std::size_t next_chunk_offset =
            payload_offset + align_four(static_cast<std::size_t>(payload_size));
        if (next_chunk_offset <= chunk_offset || next_chunk_offset > form_end)
        {
            throw MtpFormatError("MTP child alignment points outside its parent form.");
        }
        chunk_offset = next_chunk_offset;
    }
    return chunks;
}

[[nodiscard]] const NativeChunkView& require_chunk(const std::vector<NativeChunkView>& chunks,
                                                   std::uint32_t requested_tag)
{
    for (const NativeChunkView& chunk : chunks)
    {
        if (chunk.tag == requested_tag)
        {
            return chunk;
        }
    }
    throw MtpFormatError("MTP is missing a chunk required by retail native 0x004CF080.");
}

[[nodiscard]] std::string read_c_string(std::span<const std::byte> bytes, std::size_t& offset)
{
    if (offset >= bytes.size())
    {
        throw MtpFormatError("MTP string begins past its chunk payload.");
    }

    const std::size_t begin = offset;
    while (offset < bytes.size() && bytes[offset] != std::byte{0})
    {
        ++offset;
    }
    if (offset == bytes.size())
    {
        throw MtpFormatError("MTP string is missing its NUL terminator.");
    }

    std::string value;
    value.reserve(offset - begin);
    for (std::size_t byte_index = begin; byte_index < offset; ++byte_index)
    {
        value.push_back(static_cast<char>(std::to_integer<unsigned char>(bytes[byte_index])));
    }
    ++offset;
    return value;
}

[[nodiscard]] std::vector<std::string> read_string_table(std::span<const std::byte> payload,
                                                         bool has_native_index_table)
{
    const std::uint32_t string_count = read_u32(payload, 0U);
    if (string_count > payload.size() / sizeof(std::uint32_t))
    {
        throw MtpFormatError("MTP string table count exceeds its chunk payload.");
    }
    const std::size_t index_table_bytes =
        has_native_index_table ? static_cast<std::size_t>(string_count) * sizeof(std::uint32_t) : 0U;
    if (index_table_bytes > payload.size() - sizeof(std::uint32_t))
    {
        throw MtpFormatError("MTP string table count exceeds its chunk payload.");
    }

    std::size_t string_offset = sizeof(std::uint32_t) + index_table_bytes;
    std::vector<std::string> strings;
    strings.reserve(string_count);
    for (std::uint32_t string_index = 0U; string_index < string_count; ++string_index)
    {
        strings.push_back(read_c_string(payload, string_offset));
    }
    return strings;
}

} // namespace

// Host adapter, not a retail native: parses MTP IFF container chunks into a C++ MtpPackage
// struct for diagnostic inspection. The retail DynObj_LoadModelTexturePackage (0x004CF080) is
// igi::retail::dynobj::DynObj_LoadModelTexturePackage(void*, const char*, std::int32_t) in
// engine/src/retail/dynobj/mtp_package_loader.cpp, which loads the package resource itself.
MtpPackage MtpPackage_ParseContainer(std::span<const std::byte> file_bytes)
{
    MtpPackage package{};
    const std::vector<NativeChunkView> chunks = read_native_chunks(file_bytes, package.root_tag);
    for (const auto& chunk : chunks)
    {
        if (chunk.tag == 0x54455846U)
        {
            package.texture_names = read_string_table(chunk.payload, false);
        }
    }
    for (const auto& chunk : chunks)
    {
        if (chunk.tag == banm_tag)
        {
            package.animation_names = read_string_table(chunk.payload, false);
        }
    }
    package.model_names = read_string_table(require_chunk(chunks, mods_tag).payload, false);
    package.instance_names = read_string_table(require_chunk(chunks, vnam_tag).payload, true);

    const NativeChunkView& instance_chunk = require_chunk(chunks, inst_tag);
    const NativeChunkView& material_chunk = require_chunk(chunks, gtt_tag);
    const std::uint32_t material_reference_count = read_u32(material_chunk.payload, 0U);
    if (material_reference_count > material_chunk.payload.size() / 8U)
    {
        throw MtpFormatError("MTP GTT table count exceeds its chunk payload.");
    }
    const std::size_t material_reference_bytes =
        static_cast<std::size_t>(material_reference_count) * 2U * sizeof(std::uint32_t);
    if (material_reference_bytes > material_chunk.payload.size() - sizeof(std::uint32_t))
    {
        throw MtpFormatError("MTP GTT table count exceeds its chunk payload.");
    }

    package.material_references.reserve(material_reference_count);
    for (std::uint32_t reference_index = 0U; reference_index < material_reference_count; ++reference_index)
    {
        const std::size_t reference_offset =
            sizeof(std::uint32_t) + static_cast<std::size_t>(reference_index) * 8U;
        package.material_references.push_back({
            read_u32(material_chunk.payload, reference_offset),
            read_u32(material_chunk.payload, reference_offset + 4U),
        });
    }

    std::size_t instance_offset = 0U;
    package.instances.reserve(package.instance_names.size());
    for (const std::string& instance_name : package.instance_names)
    {
        const std::uint32_t model_index = read_u32(instance_chunk.payload, instance_offset);
        instance_offset += sizeof(std::uint32_t);
        const std::uint32_t instance_material_count = read_u32(instance_chunk.payload, instance_offset);
        instance_offset += sizeof(std::uint32_t);
        if (model_index >= package.model_names.size())
        {
            throw MtpFormatError("MTP instance references a model outside the MODS table.");
        }
        if (instance_material_count > instance_chunk.payload.size() / sizeof(std::uint32_t) ||
            static_cast<std::size_t>(instance_material_count) * sizeof(std::uint32_t) >
                instance_chunk.payload.size() - instance_offset)
        {
            throw MtpFormatError("MTP instance material list exceeds the INST chunk.");
        }

        MtpInstance instance{};
        instance.model_index = model_index;
        instance.instance_name = instance_name;
        instance.material_reference_indices.reserve(instance_material_count);
        for (std::uint32_t reference_index = 0U; reference_index < instance_material_count; ++reference_index)
        {
            const std::uint32_t material_reference_index = read_u32(instance_chunk.payload, instance_offset);
            instance_offset += sizeof(std::uint32_t);
            if (material_reference_index >= package.material_references.size())
            {
                throw MtpFormatError("MTP instance references a GTT entry outside the table.");
            }
            instance.material_reference_indices.push_back(material_reference_index);
        }
        package.instances.push_back(std::move(instance));
    }

    return package;
}

} // namespace igi::retail::model
