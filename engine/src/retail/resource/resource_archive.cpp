#include "igi/retail/resource/resource_archive.hpp"

#include "igi/retail/model/mef_model_loader.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <utility>

namespace igi::retail::resource {
namespace {

constexpr std::uint32_t legacy_resource_signature = 0x4F534552U;
constexpr std::size_t ilff_header_bytes = 20U;
constexpr std::size_t legacy_header_bytes = 0x20U;
constexpr std::size_t legacy_entry_header_bytes = 12U;
constexpr std::uint32_t maximum_legacy_entry_count = 1'000'000U;

[[nodiscard]] std::uint32_t read_u32(
    std::span<const std::byte> bytes,
    std::size_t offset,
    std::string_view source_name)
{
    if (offset > bytes.size() || sizeof(std::uint32_t) > bytes.size() - offset) {
        throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
    }

    return std::to_integer<std::uint32_t>(bytes[offset])
        | (std::to_integer<std::uint32_t>(bytes[offset + 1U]) << 8U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 2U]) << 16U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

[[nodiscard]] std::size_t aligned_legacy_size(
    std::size_t value,
    std::string_view source_name)
{
    constexpr std::size_t alignment = 32U;
    if (value > std::numeric_limits<std::size_t>::max() - (alignment - 1U)) {
        throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
    }
    return (value + alignment - 1U) & ~(alignment - 1U);
}

[[nodiscard]] ResourceArchive load_ires_archive(
    std::span<const std::byte> resource_bytes,
    std::string_view source_name)
{
    ResourceArchive archive{};
    try {
        const model::IlffContainer container = model::Ilff_ReadContainer(resource_bytes);
        if (container.form_type != ResourceArchive::ires_form) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }

        archive.form_type = container.form_type;
        std::size_t chunk_index = 0U;
        while (chunk_index < container.chunks.size()) {
            const model::IlffChunk& name_chunk = container.chunks[chunk_index];
            if (name_chunk.tag != ResourceArchive::name_chunk
                || chunk_index + 1U >= container.chunks.size()) {
                throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
            }

            const model::IlffChunk& body_chunk = container.chunks[chunk_index + 1U];
            if (body_chunk.tag != ResourceArchive::body_chunk) {
                throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
            }

            ResourceEntry entry{};
            for (const std::byte character_byte : name_chunk.payload) {
                const char character = static_cast<char>(std::to_integer<unsigned char>(character_byte));
                if (character == '\0') {
                    break;
                }
                entry.name.push_back(character);
            }
            if (entry.name.empty()) {
                throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
            }

            entry.body = body_chunk.payload;
            archive.entries.push_back(std::move(entry));
            chunk_index += 2U;
        }
    } catch (const model::MefFormatError&) {
        throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
    }

    return archive;
}

[[nodiscard]] ResourceArchive load_legacy_resource_archive(
    std::span<const std::byte> resource_bytes,
    std::string_view source_name)
{
    if (resource_bytes.size() < legacy_header_bytes) {
        throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
    }

    const std::uint32_t entry_count = read_u32(resource_bytes, 8U, source_name);
    if (entry_count > maximum_legacy_entry_count) {
        throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
    }

    ResourceArchive archive{};
    archive.form_type = legacy_resource_signature;
    std::size_t record_offset = legacy_header_bytes;

    for (std::uint32_t entry_index = 0U; entry_index < entry_count; ++entry_index) {
        if (record_offset > resource_bytes.size()
            || legacy_entry_header_bytes > resource_bytes.size() - record_offset) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }

        const std::uint32_t body_size = read_u32(resource_bytes, record_offset + 4U, source_name);
        const std::uint32_t name_size = read_u32(resource_bytes, record_offset + 8U, source_name);
        const std::size_t name_offset = record_offset + legacy_entry_header_bytes;
        if (name_size > resource_bytes.size() - name_offset) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }

        const std::size_t body_relative_offset = aligned_legacy_size(
            legacy_entry_header_bytes + static_cast<std::size_t>(name_size),
            source_name);
        if (body_relative_offset > resource_bytes.size() - record_offset) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }

        const std::size_t body_offset = record_offset + body_relative_offset;
        if (body_size > resource_bytes.size() - body_offset) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }

        ResourceEntry entry{};
        entry.name.reserve(name_size);
        for (std::size_t byte_index = 0U; byte_index < name_size; ++byte_index) {
            entry.name.push_back(static_cast<char>(
                std::to_integer<unsigned char>(resource_bytes[name_offset + byte_index])));
        }
        if (!entry.name.empty() && entry.name.back() == '\0') {
            entry.name.pop_back();
        }

        entry.body.assign(
            resource_bytes.begin() + static_cast<std::ptrdiff_t>(body_offset),
            resource_bytes.begin() + static_cast<std::ptrdiff_t>(body_offset + body_size));
        archive.entries.push_back(std::move(entry));

        const std::size_t aligned_body_size = aligned_legacy_size(body_size, source_name);
        if (aligned_body_size > resource_bytes.size() - body_offset) {
            throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
        }
        record_offset = body_offset + aligned_body_size;
    }

    return archive;
}

} // namespace

const ResourceEntry* ResourceArchive::find(std::string_view resource_name) const noexcept
{
    const auto entry_iterator = std::find_if(
        entries.begin(),
        entries.end(),
        [resource_name](const ResourceEntry& entry) {
            return entry.name == resource_name;
        });
    return entry_iterator == entries.end() ? nullptr : &*entry_iterator;
}

// Native: Sys_Resource() - VA 0x004B5F80 - partial adapter, not a complete literal port.
// Behavior: Recognizes ILFF/IRES, legacy RESOURCE entries, and the native raw-resource fallback.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_756c70344e9eb0cff58f5a134a0308a81a96d2483abaf4a70ae305f706b05829.
// CFG: r2 blocks=11, edges=14; BN blocks=11; source agreement=100.0%.
// Confidence: partial archive adapter; raw fallback and intrusive registry insertion remain unported.
// Signature: C++ span/name -> ResourceArchive differs from the retail registry/buffer interface.
// Usage/tests: Used to mount model/texture RES archives; archive fixtures passed; full parity pending.
ResourceArchive Sys_Resource(
    std::span<const std::byte> resource_bytes,
    std::string_view source_name)
{
    if (resource_bytes.size() >= ilff_header_bytes
        && model::Ilff_ReadLittleEndian32(resource_bytes, 0U) == ResourceArchive::ilff_signature) {
        return load_ires_archive(resource_bytes, source_name);
    }

    if (resource_bytes.size() >= 8U) {
        constexpr std::string_view legacy_signature = "RESOURCE";
        const bool has_legacy_signature = std::equal(
            legacy_signature.begin(),
            legacy_signature.end(),
            resource_bytes.begin(),
            [](char expected, std::byte actual) {
                return static_cast<unsigned char>(expected) == std::to_integer<unsigned char>(actual);
            });
        if (has_legacy_signature) {
            return load_legacy_resource_archive(resource_bytes, source_name);
        }
    }

    throw ResourceFormatError("Error in resource file: '" + std::string(source_name) + "'");
}

// Native: Resource_Load() - VA 0x004B5F00 - partial adapter, not a complete literal port.
// Behavior: Loads cached/unpacked bytes and optionally writes their uint32 byte count.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_87092f5147addf2b19ea080dbc70a44d1218ec706e022f019e4ba19bad366057.
// CFG: r2 blocks=10, edges=12; BN blocks=10; source agreement=100.0%.
// Confidence: partial host adapter; retail allocator/VFS/registry behavior is incomplete.
// Signature: C++ ResourceHandle/ResourceArchive signatures adapt the retail raw-pointer API.
// Usage/tests: Used by model/QVM loading; resource fixtures and model-scene runtime passed.
ResourceArchive Resource_Load(const std::filesystem::path& resource_path)
{
    std::ifstream resource_stream(resource_path, std::ios::binary | std::ios::ate);
    if (!resource_stream) {
        throw ResourceFormatError("Failed to load resource: '" + resource_path.string() + "'");
    }

    const std::streamoff file_size = resource_stream.tellg();
    if (file_size <= 0 || static_cast<std::uint64_t>(file_size) > std::numeric_limits<std::size_t>::max()) {
        throw ResourceFormatError("Failed to load resource: '" + resource_path.string() + "'");
    }

    std::vector<char> raw_bytes(static_cast<std::size_t>(file_size));
    resource_stream.seekg(0, std::ios::beg);
    resource_stream.read(raw_bytes.data(), static_cast<std::streamsize>(raw_bytes.size()));
    if (!resource_stream) {
        throw ResourceFormatError("Failed to load resource: '" + resource_path.string() + "'");
    }

    std::vector<std::byte> resource_bytes(raw_bytes.size());
    std::transform(
        raw_bytes.begin(),
        raw_bytes.end(),
        resource_bytes.begin(),
        [](char value) {
            return static_cast<std::byte>(static_cast<unsigned char>(value));
        });

    return Sys_Resource(resource_bytes, resource_path.string());
}

} // namespace igi::retail::resource
