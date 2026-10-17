#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace igi::retail::model {

// Original IGI.EXE: DynObj_LoadModelMesh at VA 0x004CF390.
// This object owns the ILFF byte stream and the parsed chunk tree it exposes.
class MefFormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct IlffChunk {
    std::uint32_t tag = 0;
    std::uint32_t declared_size = 0;
    std::uint32_t alignment = 0;
    std::uint32_t next_sibling_offset = 0;
    std::size_t header_offset = 0;
    std::vector<std::byte> payload;
    std::vector<IlffChunk> children;

    [[nodiscard]] const IlffChunk* find_first(std::uint32_t requested_tag) const noexcept;
};

struct IlffContainer {
    std::uint32_t form_type = 0;
    std::uint32_t declared_size = 0;
    std::uint32_t alignment = 0;
    std::uint32_t next_sibling_offset = 0;
    std::vector<IlffChunk> chunks;

    [[nodiscard]] const IlffChunk* find_first(std::uint32_t requested_tag) const noexcept;
};

struct MefRenderChunks {
    const IlffChunk* d3d_render_header = nullptr;
    const IlffChunk* draw_records = nullptr;
    const IlffChunk* vertex_stream = nullptr;
};

// Original IGI.EXE backend routine at VA 0x0049FB10 accepts MESH+0x20 values 0, 1, and 3.
// The stripped retail binary does not provide a symbol name; the enum names describe the
// observed renderer paths and remain working labels.
enum class MefRenderFormat : std::uint32_t {
    diffuse = 0U,
    skinned = 1U,
    lightmapped = 3U,
};

struct MefRenderHeader {
    std::uint32_t vertex_count = 0U;
    std::uint32_t submesh_count = 0U;
    std::uint32_t lightmap_page_count = 0U;
    std::uint32_t extra_influence_count = 0U;
    std::uint32_t base_vertex_count = 0U;
};

// Original IGI.EXE entry retail routine 0x0049B700 at VA 0x0049B700 (semantic label unresolved).
// REA/Ghidra evidence ev_7b50c35acc556348073cf6b485533b219fa3e483f5f98028979a4228a713c446:
// 40-byte records; position/normal/UV floats at +0x00..+0x1C, influence weight at +0x20,
// accumulated target vertex at +0x24, and transform/bone index at +0x26. This identifies
// the record consumed by the native, not its association with a specific MEF chunk.
struct MefSkinnedVertex {
    float position_x = 0.0F;
    float position_y = 0.0F;
    float position_z = 0.0F;
    float normal_x = 0.0F;
    float normal_y = 0.0F;
    float normal_z = 0.0F;
    float diffuse_u = 0.0F;
    float diffuse_v = 0.0F;
    float influence_weight = 0.0F;
    std::uint16_t target_vertex_index = 0;
    std::uint16_t bone_index = 0;
};

// FourCC integers are the values compared by the retail x86 code. The bytes on disk are
// little-endian, so a native constant such as 'MESH' is stored as the byte sequence "HSEM".
namespace mef_tag {
inline constexpr std::uint32_t ilff = 0x46464C49U;
inline constexpr std::uint32_t mesh = 0x4D455348U;
inline constexpr std::uint32_t attachments = 0x41545441U;
inline constexpr std::uint32_t legacy_vertices = 0x4D565458U;
inline constexpr std::uint32_t portals = 0x504F5254U;
inline constexpr std::uint32_t portal_faces = 0x50544643U;
inline constexpr std::uint32_t portal_vertices = 0x50545658U;
inline constexpr std::uint32_t texture_animation = 0x4E415854U;
inline constexpr std::uint32_t d3d_render_header = 0x52443344U;
inline constexpr std::uint32_t draw_records = 0x52454E44U;
inline constexpr std::uint32_t vertices = 0x56525458U;
} // namespace mef_tag

class MefModel {
public:
    static constexpr std::uint32_t form_ocem = 0x4D45434FU;
    static constexpr std::uint32_t mesh_header_size = 0x9CU;
    static constexpr std::uint32_t supported_version_bits = 0x3DCCCCCDU;

    IlffContainer container;
    std::vector<std::byte> mesh_header;
    float version = 0.0F;
    MefRenderFormat render_format = MefRenderFormat::diffuse;
    std::optional<MefRenderHeader> render_header;
    std::optional<IlffChunk> attachments;
    std::optional<IlffChunk> legacy_vertices;
    std::optional<IlffChunk> portals;
    std::optional<IlffChunk> portal_faces;
    std::optional<IlffChunk> portal_vertices;

    [[nodiscard]] static MefModel DynObj_LoadModelMesh(std::span<const std::byte> file_bytes);
    [[nodiscard]] MefRenderChunks render_chunks() const noexcept;
};

// retail routine 0x0049FB10 - 0x0049FB10 - accepts only MESH+0x20 values 0, 1, and 3.
// Evidence: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01;
// Radare2 decoded body 842 bytes / 16 basic blocks, including the original "Mesh format not supported" path.
// The native identity is statically established; this C++ port has not been compared in a live render replay.
[[nodiscard]] MefRenderFormat MefModel_ReadRetailRenderFormat(std::span<const std::byte> mesh_header);

// retail routine 0x0049FB10 - 0x0049FB10 - reads D3DR renderer data according to MESH+0x20.
// Field offsets follow the native format branches; the skinned count interpretation is cross-referenced
// with retail routine 0x0049B700 (REA evidence ev_7b50c35acc556348073cf6b485533b219fa3e483f5f98028979a4228a713c446).
[[nodiscard]] MefRenderHeader MefRenderHeader_ReadRetail(
    std::span<const std::byte> render_header_bytes,
    MefRenderFormat render_format);

// Reads only the 40-byte record consumed by retail routine 0x0049B700. This does not identify
// which MEF chunk or mesh format supplies the record; that association remains unverified.
[[nodiscard]] MefSkinnedVertex MefSkinnedVertex_ReadRetailRecord(
    std::span<const std::byte> record_bytes);

// Original IGI.EXE ILFF helpers used by DynObj_LoadModelMesh:
// 0x004B7490 computes the first aligned child position; 0x004B74D0 resolves a child
// payload from its parent base; 0x004B7590 searches sibling tags.
[[nodiscard]] IlffContainer Ilff_ReadContainer(std::span<const std::byte> bytes);
[[nodiscard]] std::uint32_t Ilff_ReadLittleEndian32(std::span<const std::byte> bytes, std::size_t offset);

} // namespace igi::retail::model
