#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::terrain
{

// Retail data layouts, recovered from instructions rather than reference-project structs.
// 0x004C6240 - REA/Ghidra + BN + radare2; source CFG catalog 0.809 (below gate).
// Layout evidence: ev_84a279844258af41f55848d87719fc5845164a3660c0620c2dc309565b7707f3.
// Field offsets/strides are observed; this does not verify the entire loader's behavior.
union TerrainGridDataReference
{
    std::uint32_t file_value = 0;
    const std::byte* runtime_pointer;
};

struct TerrainGridRecord
{
    TerrainGridDataReference data;
    std::uint8_t runtime_state = 0;
    std::array<std::byte, 3> unknown_after_state{};
    std::uint32_t dimension = 0;
};

// 0x00510010 - REA/Ghidra vertex consumer; whole-routine CFG/runtime parity pending.
// Evidence: ev_ba370d099419afc676d7082c9828a35e206e9a7de421482b56f8582b67ffb169.
// The native reads these as signed words; do not silently substitute unsigned counts.
struct TerrainCommandHeader
{
    std::int16_t triangle_count = 0;
    std::int16_t triangle_byte_count = 0;
    std::int16_t base_vertex_count = 0;
    std::int16_t extra_vertex_count = 0;
};

union TerrainCommandReference
{
    std::uint32_t file_offset = 0;
    const TerrainCommandHeader* runtime_pointer;
};

struct TerrainContourRecord
{
    std::array<std::byte, 0x1C> unverified_tree_fields{};
    TerrainCommandReference commands;
};

// 0x004D14E0 emits descriptors consumed at 0x00510010; both complete routines remain unported.
// Evidence: ev_0fdafcedeef167b35b39c1e3cd5193f4985378063ea6ddaa60d0993dc15ee8d1.
// Preserve unknown words until their producers/consumers are verified; no guessed bounding boxes.
struct TerrainRenderChunkRecord
{
    std::array<std::byte, 0x18> origin_fields{};
    std::int32_t detail_exponent = 0;
    std::uint32_t orientation_flags = 0;
    const TerrainCommandHeader* commands = nullptr;
    std::uint32_t packed_binding_identifiers = 0;
    std::uint32_t unknown_0x28 = 0;
    std::byte* cache_record = nullptr;
};

#if defined(_M_IX86) || defined(__i386__) || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4)
static_assert(sizeof(void*) == 4, "Retail terrain layouts require the Windows x86 ABI.");
static_assert(sizeof(TerrainGridRecord) == 12);
static_assert(offsetof(TerrainGridRecord, runtime_state) == 4);
static_assert(offsetof(TerrainGridRecord, dimension) == 8);
static_assert(sizeof(TerrainCommandHeader) == 8);
static_assert(sizeof(TerrainContourRecord) == 32);
static_assert(offsetof(TerrainContourRecord, commands) == 0x1C);
static_assert(sizeof(TerrainRenderChunkRecord) == 48);
static_assert(offsetof(TerrainRenderChunkRecord, commands) == 0x20);
static_assert(offsetof(TerrainRenderChunkRecord, packed_binding_identifiers) == 0x24);
static_assert(offsetof(TerrainRenderChunkRecord, cache_record) == 0x2C);
#endif

} // namespace igi::retail::terrain

