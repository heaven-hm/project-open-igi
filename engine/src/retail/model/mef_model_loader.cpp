#include "igi/retail/model/mef_model_loader.hpp"

#include <bit>
#include <limits>
#include <optional>
#include <sstream>
#include <utility>

namespace igi::retail::model {
namespace {

// DynObj_LocalModelsSMef - 0x004CF390 - Native static evidence: CFG 0.991 (PASS), RE-Agent 5/6; port runtime parity unverified.
// Tools: Ghidra/REA and Binary Ninja; runtime parity is not verified.
// Evidence: ev_0058f0bca939b5f1425ff13a7b66ab40a13e5437023203f4895513bf504c3e7c;
// RE-Agent catalog assets/RE_AGENT_VERIFIED_NATIVES.md (0x004CF390, CFG 0.991, 5/6).
// The catalog name is provisional: REA labels this procedure FUN_004cf390.
// Source export: assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.cpp.
constexpr std::size_t container_header_size = 20U;
// Original IGI.EXE chunk-navigation helpers: 0x004B7490, 0x004B74D0,
// 0x004B7590 and 0x004B7470. Their decompilation evidence IDs are recorded below.
constexpr std::size_t chunk_header_size = 16U;
constexpr std::size_t mesh_version_offset = 0U;
constexpr std::size_t minimum_mesh_header_size = 0x9CU;

[[nodiscard]] bool is_power_of_two(std::uint32_t value) noexcept {
    return value != 0U && (value & (value - 1U)) == 0U;
}

[[nodiscard]] std::size_t align_up(std::size_t value, std::uint32_t alignment) {
    const std::size_t alignment_size = static_cast<std::size_t>(alignment);
    if (value > std::numeric_limits<std::size_t>::max() - (alignment_size - 1U)) {
        throw MefFormatError("ILFF alignment overflows the host address range.");
    }

    return (value + alignment_size - 1U) & ~(alignment_size - 1U);
}

void require_range(std::size_t offset, std::size_t length, std::size_t limit, const char* description) {
    if (offset > limit || length > limit - offset) {
        throw MefFormatError(std::string(description) + " extends past its ILFF parent container.");
    }
}

void write_u32(std::vector<std::byte>& destination, std::size_t offset, std::uint32_t value) {
    if (destination.size() < offset + sizeof(value)) {
        destination.resize(offset + sizeof(value));
    }
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        destination[offset + byte_index] = static_cast<std::byte>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

[[nodiscard]] IlffContainer read_container(std::span<const std::byte> bytes, std::size_t depth) {
    if (depth > 16U) {
        throw MefFormatError("ILFF nested-container depth exceeds 16.");
    }

    if (bytes.size() < container_header_size) {
        throw MefFormatError("ILFF container is shorter than its 20-byte header.");
    }

    if (Ilff_ReadLittleEndian32(bytes, 0U) != mef_tag::ilff) {
        throw MefFormatError("Expected the retail ILFF container signature.");
    }

    IlffContainer container{};
    container.declared_size = Ilff_ReadLittleEndian32(bytes, 4U);
    container.alignment = Ilff_ReadLittleEndian32(bytes, 8U);
    container.next_sibling_offset = Ilff_ReadLittleEndian32(bytes, 12U);
    container.form_type = Ilff_ReadLittleEndian32(bytes, 16U);

    if (!is_power_of_two(container.alignment)) {
        throw MefFormatError("ILFF container alignment is not a nonzero power of two.");
    }

    if (container.declared_size < container_header_size || container.declared_size > bytes.size()) {
        throw MefFormatError("ILFF container size is outside the available file range.");
    }

    const std::size_t container_size = static_cast<std::size_t>(container.declared_size);
    std::size_t chunk_offset = align_up(container_header_size, container.alignment);

    while (chunk_offset <= container_size && chunk_header_size <= container_size - chunk_offset) {
        const auto chunk_header = bytes.subspan(chunk_offset, chunk_header_size);
        IlffChunk chunk{};
        chunk.tag = Ilff_ReadLittleEndian32(chunk_header, 0U);
        chunk.declared_size = Ilff_ReadLittleEndian32(chunk_header, 4U);
        chunk.alignment = Ilff_ReadLittleEndian32(chunk_header, 8U);
        chunk.next_sibling_offset = Ilff_ReadLittleEndian32(chunk_header, 12U);
        chunk.header_offset = chunk_offset;

        if (!is_power_of_two(chunk.alignment) || chunk.alignment > container.alignment) {
            throw MefFormatError("ILFF chunk alignment is invalid for its parent container.");
        }

        std::size_t payload_offset = 0U;
        std::size_t payload_size = static_cast<std::size_t>(chunk.declared_size);
        if (chunk.tag == mef_tag::ilff) {
            if (payload_size < container_header_size) {
                throw MefFormatError("Nested ILFF chunk is shorter than its container header.");
            }
            payload_offset = chunk_offset + chunk_header_size;
            payload_size -= chunk_header_size;
        } else {
            payload_offset = align_up(chunk_offset + chunk_header_size, chunk.alignment);
        }

        require_range(payload_offset, payload_size, container_size, "ILFF chunk payload");
        chunk.payload.assign(bytes.begin() + static_cast<std::ptrdiff_t>(payload_offset),
            bytes.begin() + static_cast<std::ptrdiff_t>(payload_offset + payload_size));

        if (chunk.tag == mef_tag::ilff) {
            if (chunk.payload.size() < sizeof(std::uint32_t)) {
                throw MefFormatError("Nested ILFF chunk does not contain a form type.");
            }

            std::vector<std::byte> nested_container_bytes(chunk_header_size);
            write_u32(nested_container_bytes, 0U, mef_tag::ilff);
            write_u32(nested_container_bytes, 4U, chunk.declared_size);
            write_u32(nested_container_bytes, 8U, chunk.alignment);
            write_u32(nested_container_bytes, 12U, chunk.next_sibling_offset);
            nested_container_bytes.insert(
                nested_container_bytes.end(),
                chunk.payload.begin(),
                chunk.payload.end());
            chunk.children = read_container(nested_container_bytes, depth + 1U).chunks;
        }

        const std::uint32_t sibling_distance = chunk.next_sibling_offset;
        container.chunks.push_back(std::move(chunk));

        if (sibling_distance == 0U) {
            break;
        }

        if (sibling_distance < chunk_header_size) {
            throw MefFormatError("ILFF sibling distance cannot contain a chunk header.");
        }

        const std::size_t distance = static_cast<std::size_t>(sibling_distance);
        if (distance > container_size - chunk_offset) {
            throw MefFormatError("ILFF sibling offset points past the parent container.");
        }
        chunk_offset += distance;
    }

    return container;
}

} // namespace

const IlffChunk* IlffChunk::find_first(std::uint32_t requested_tag) const noexcept {
    for (const IlffChunk& child : children) {
        if (child.tag == requested_tag) {
            return &child;
        }
    }

    return nullptr;
}

const IlffChunk* IlffContainer::find_first(std::uint32_t requested_tag) const noexcept {
    for (const IlffChunk& chunk : chunks) {
        if (chunk.tag == requested_tag) {
            return &chunk;
        }
    }

    return nullptr;
}

std::uint32_t Ilff_ReadLittleEndian32(std::span<const std::byte> bytes, std::size_t offset) {
    if (offset > bytes.size() || sizeof(std::uint32_t) > bytes.size() - offset) {
        throw MefFormatError("Attempted to read a 32-bit ILFF field past the available bytes.");
    }

    return std::to_integer<std::uint32_t>(bytes[offset])
        | (std::to_integer<std::uint32_t>(bytes[offset + 1U]) << 8U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 2U]) << 16U)
        | (std::to_integer<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

MefSkinnedVertex MefSkinnedVertex_ReadRetailRecord(std::span<const std::byte> record_bytes) {
    constexpr std::size_t record_size = 40U;
    if (record_bytes.size() != record_size) {
        throw MefFormatError("FUN_0049b700 consumes a 40-byte skinned vertex record.");
    }

    const auto read_float = [record_bytes](std::size_t offset) {
        return std::bit_cast<float>(Ilff_ReadLittleEndian32(record_bytes, offset));
    };
    const auto read_u16 = [record_bytes](std::size_t offset) {
        if (offset > record_bytes.size() || sizeof(std::uint16_t) > record_bytes.size() - offset) {
            throw MefFormatError("Skinned vertex index extends past its 40-byte record.");
        }
        return static_cast<std::uint16_t>(
            std::to_integer<std::uint16_t>(record_bytes[offset])
            | (std::to_integer<std::uint16_t>(record_bytes[offset + 1U]) << 8U));
    };

    return {
        read_float(0U),
        read_float(4U),
        read_float(8U),
        read_float(12U),
        read_float(16U),
        read_float(20U),
        read_float(24U),
        read_float(28U),
        read_float(32U),
        read_u16(36U),
        read_u16(38U),
    };
}

MefRenderFormat MefModel_ReadRetailRenderFormat(std::span<const std::byte> mesh_header) {
    constexpr std::size_t format_offset = 0x20U;
    if (mesh_header.size() < format_offset + sizeof(std::uint32_t)) {
        throw MefFormatError("MESH header is too short to read the renderer format at offset 0x20.");
    }

    const std::uint32_t raw_format = Ilff_ReadLittleEndian32(mesh_header, format_offset);
    switch (raw_format) {
    case static_cast<std::uint32_t>(MefRenderFormat::diffuse):
        return MefRenderFormat::diffuse;
    case static_cast<std::uint32_t>(MefRenderFormat::skinned):
        return MefRenderFormat::skinned;
    case static_cast<std::uint32_t>(MefRenderFormat::lightmapped):
        return MefRenderFormat::lightmapped;
    default:
        throw MefFormatError("Mesh format not supported");
    }
}

MefRenderHeader MefRenderHeader_ReadRetail(
    std::span<const std::byte> render_header_bytes,
    MefRenderFormat render_format) {
    MefRenderHeader render_header{};
    std::size_t vertex_count_offset = 0U;
    std::size_t submesh_count_offset = 0U;
    std::size_t required_header_size = 0U;

    switch (render_format) {
    case MefRenderFormat::diffuse:
        vertex_count_offset = 12U;
        submesh_count_offset = 8U;
        required_header_size = 48U;
        break;
    case MefRenderFormat::skinned:
        vertex_count_offset = 20U;
        submesh_count_offset = 8U;
        required_header_size = 56U;
        break;
    case MefRenderFormat::lightmapped:
        vertex_count_offset = 16U;
        submesh_count_offset = 12U;
        required_header_size = 56U;
        break;
    default:
        throw MefFormatError("Mesh format not supported");
    }

    if (render_header_bytes.size() < required_header_size) {
        throw MefFormatError("D3DR renderer header is shorter than the native format requires.");
    }

    if (render_format == MefRenderFormat::skinned) {
        render_header.extra_influence_count = Ilff_ReadLittleEndian32(render_header_bytes, 12U);
        render_header.base_vertex_count = Ilff_ReadLittleEndian32(render_header_bytes, 16U);
    } else if (render_format == MefRenderFormat::lightmapped) {
        render_header.lightmap_page_count = Ilff_ReadLittleEndian32(render_header_bytes, 4U);
    }

    render_header.vertex_count = Ilff_ReadLittleEndian32(render_header_bytes, vertex_count_offset);
    render_header.submesh_count = Ilff_ReadLittleEndian32(render_header_bytes, submesh_count_offset);
    return render_header;
}

IlffContainer Ilff_ReadContainer(std::span<const std::byte> bytes) {
    return read_container(bytes, 0U);
}

MefModel MefModel::DynObj_LocalModelsSMef(std::span<const std::byte> file_bytes) {
    MefModel model{};
    model.container = Ilff_ReadContainer(file_bytes);

    if (model.container.form_type != form_ocem) {
        throw MefFormatError("Retail DynObj_LocalModelsSMef expects an OCEM model container.");
    }

    const IlffChunk* mesh_chunk = model.container.find_first(mef_tag::mesh);
    if (mesh_chunk == nullptr) {
        throw MefFormatError("Retail DynObj_LocalModelsSMef could not find its MESH chunk.");
    }

    if (mesh_chunk->payload.size() < minimum_mesh_header_size) {
        throw MefFormatError("MESH payload does not contain the fields accessed by the retail loader.");
    }

    model.mesh_header = mesh_chunk->payload;
    const std::uint32_t version_bits = Ilff_ReadLittleEndian32(model.mesh_header, mesh_version_offset);
    model.version = std::bit_cast<float>(version_bits);
    if (version_bits != supported_version_bits) {
        std::ostringstream error_message;
        error_message << "Retail MESH version mismatch: got " << model.version << ", expected 0.1.";
        throw MefFormatError(error_message.str());
    }
    model.render_format = MefModel_ReadRetailRenderFormat(model.mesh_header);

    const IlffChunk* render_header_chunk = model.container.find_first(mef_tag::d3d_render_header);
    if (render_header_chunk != nullptr) {
        model.render_header = MefRenderHeader_ReadRetail(
            render_header_chunk->payload,
            model.render_format);
    }

    const auto copy_nonempty_chunk = [&model](std::uint32_t tag) -> std::optional<IlffChunk> {
        const IlffChunk* chunk = model.container.find_first(tag);
        if (chunk == nullptr || chunk->payload.empty()) {
            return std::nullopt;
        }
        return *chunk;
    };

    model.attachments = copy_nonempty_chunk(mef_tag::attachments);
    model.legacy_vertices = copy_nonempty_chunk(mef_tag::legacy_vertices);
    model.portals = copy_nonempty_chunk(mef_tag::portals);
    model.portal_faces = copy_nonempty_chunk(mef_tag::portal_faces);
    model.portal_vertices = copy_nonempty_chunk(mef_tag::portal_vertices);

    return model;
}

MefRenderChunks MefModel::render_chunks() const noexcept {
    return {
        container.find_first(mef_tag::d3d_render_header),
        container.find_first(mef_tag::draw_records),
        container.find_first(mef_tag::vertices),
    };
}

} // namespace igi::retail::model
