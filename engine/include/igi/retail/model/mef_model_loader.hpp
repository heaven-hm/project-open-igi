#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace igi::retail::model {

// Original IGI.EXE: DynObj_LocalModelsSMef at VA 0x004CF390.
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
inline constexpr std::uint32_t draw_records = 0x444E4552U;
inline constexpr std::uint32_t vertices = 0x58545256U;
} // namespace mef_tag

class MefModel {
public:
    static constexpr std::uint32_t form_ocem = 0x4D45434FU;
    static constexpr std::uint32_t mesh_header_size = 0x9CU;
    static constexpr std::uint32_t supported_version_bits = 0x3DCCCCCDU;

    IlffContainer container;
    std::vector<std::byte> mesh_header;
    float version = 0.0F;
    std::optional<IlffChunk> attachments;
    std::optional<IlffChunk> legacy_vertices;
    std::optional<IlffChunk> portals;
    std::optional<IlffChunk> portal_faces;
    std::optional<IlffChunk> portal_vertices;

    [[nodiscard]] static MefModel DynObj_LocalModelsSMef(std::span<const std::byte> file_bytes);
};

// Original IGI.EXE ILFF helpers used by DynObj_LocalModelsSMef:
// 0x004B7490 computes the first aligned child position; 0x004B74D0 resolves a child
// payload from its parent base; 0x004B7590 searches sibling tags.
[[nodiscard]] IlffContainer Ilff_ReadContainer(std::span<const std::byte> bytes);
[[nodiscard]] std::uint32_t Ilff_ReadLittleEndian32(std::span<const std::byte> bytes, std::size_t offset);

} // namespace igi::retail::model
