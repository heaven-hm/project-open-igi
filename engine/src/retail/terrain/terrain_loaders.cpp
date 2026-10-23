#include "igi/retail/terrain/terrain_loaders.hpp"

#include "igi/retail/terrain/terrain_quadtree.hpp"

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/script/script_mode.hpp"
#include "igi/retail/texture/loop_terrain_texture.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>

namespace igi::retail::terrain
{
// Retail global state, externally observable by other native translation units.
// Existing host storage covers 20 entries; retail's complete storage extent is unresolved.
std::array<std::uint32_t, 20> g_data_b97b20{};
std::int32_t g_b97d24 = 0;
// 0x00B97DF4 and 0x00BA2058: heightmap stack depth and current heightmap index.  These are
// defined here (outside this file's anonymous namespace) because HeightMap_Enter writes them and
// DynObj_ParseConfigFile_6 (terrain_scene_state.cpp) and Terrain_TraverseContourQuadtree
// (terrain_quadtree.cpp) read or clear the same retail storage.  The declarations live in
// terrain_quadtree.hpp so the one definition below is checked against its extern consumers.
std::int32_t g_b97df4 = 0;
std::uint32_t g_ba2058 = 0;
// Retail texture-modifier state is shared with push/pop native consumers.
std::array<std::uint32_t, 20> g_b97da0{};
std::int32_t g_b97df8 = 0;
std::uint32_t g_ba5060 = 0;

// -------------------------------------------------------------------------------------------------
// Retail state consumed by Terrain_DecodeChunkCommands (0x00510010).
//
// 0x00B16CA4 / 0x00B16C9C are the per-frame record and descriptor counts published by
// 0x004D14E0 (Terrain_TraverseContourQuadtree).  The 0x30-byte records themselves live at
// 0x00B16CE0..0x00B46CDF (0x1000 records); this function reads g_b16ca4 records and advances the
// record pointer by 0x30, it does not read the 0x1000 limit.
// -------------------------------------------------------------------------------------------------
std::int32_t g_b16ca4 = 0;
std::int32_t g_b16c9c = 0;
std::array<std::uint8_t, 0x30 * 0x1000> g_b16ce0{};
std::int32_t g_c31090 = 0; // 0x00C31090: accumulation of the record count, reset by caller 0x0050FE60

// Three identical terrain allocation pools (0x004D23CC family).  The handle tables are byte
// addressed: retail stores `*(int*)((char*)handles + offset)` where offset already contains
// base + map[slot] * stride.
std::int32_t* g_b12b74 = nullptr; // pool A handles, 0x00B12B74
std::int32_t g_b12b68 = 0;        // pool A count,   0x00B12B68
std::int32_t g_b12b6c = 0;        // pool A limit,   0x00B12B6C
std::int32_t* g_b12b64 = nullptr; // pool A map,     0x00B12B64
std::int32_t g_b12b60 = 0;        // pool A base,    0x00B12B60
std::int32_t g_b12b70 = 0;        // pool A stride,  0x00B12B70
std::int32_t* g_b12c54 = nullptr; // pool B handles, 0x00B12C54
std::int32_t g_b12c48 = 0;        // pool B count,   0x00B12C48
std::int32_t g_b12c4c = 0;        // pool B limit,   0x00B12C4C
std::int32_t* g_b12c44 = nullptr; // pool B map,     0x00B12C44
std::int32_t g_b12c40 = 0;        // pool B base,    0x00B12C40
std::int32_t g_b12c50 = 0;        // pool B stride,  0x00B12C50
std::int32_t* g_b16cd4 = nullptr; // pool C handles, 0x00B16CD4
std::int32_t g_b16cc8 = 0;        // pool C count,   0x00B16CC8
std::int32_t g_b16ccc = 0;        // pool C limit,   0x00B16CCC
std::int32_t* g_b16cc4 = nullptr; // pool C map,     0x00B16CC4
std::int32_t g_b16cc0 = 0;        // pool C base,    0x00B16CC0
std::int32_t g_b16cd0 = 0;        // pool C stride,  0x00B16CD0

// 0x00B12C5C / 0x00B12C60: descriptor-array side tables.  The consumer indexes them by
// g_b16c9c.  Retail passes the literal address 0x00B12C60 to both users (0x00513038 in
// Terrain_Draw and 0x004D1559 in Terrain_TraverseContourQuadtree), so 0x00B12C60 is the array
// itself, not a pointer stored there; it is defined here as the single array definition and
// declared `extern std::uint32_t g_b12c60[]` in terrain_quadtree.hpp.  The retail extent is not
// established; 0x1000 entries is the candidate's bounded storage.
std::array<std::uint32_t, 0x1000> g_b12c5c{};
std::uint32_t g_b12c60[0x1000]{};

// 0x00B12BA0 / 0x00B12BA8 / 0x00B12BB0: world-space origin added to the current record before the
// per-material page coordinates are formed.  Set by the render setup; not written here.
double g_b12ba0 = 0.0;
double g_b12ba8 = 0.0;
double g_b12bb0 = 0.0;

// Runtime-populated tables addressed by the packed record token.  Retail keeps these in BSS and
// fills them from the texture-modifier and tile-map loaders; the candidate provides zeroed shadow
// storage so the transcription is self-contained.  Each entry is 0x30 bytes.
std::array<std::uint8_t, 0x30 * 256> g_ba2030_texture_modifiers{}; // 0x00BA2030
std::array<std::uint8_t, 0x30 * 256> g_ba5050_neighbor_modifiers{}; // 0x00BA5050
std::array<std::uint8_t, 0x30 * 256> g_ba80b0_material_pages{};    // 0x00BA80B0
std::uint8_t* g_c30060 = nullptr;                                  // 0x00C30060 vertex-declaration base

namespace
{

// TextureModifier_LoadParameters stores a packed one-bit bitmap along with the
// cube-relative scale and origin.  The renderer must test that bitmap before
// substituting the modifier's material; an enclosing cube alone is not a
// texture modifier.  Coordinates follow the byte-row layout calculated by
// the retail routine (line width is bitmap_size / 8).
bool texture_modifier_covers_point_impl(const TerrainTextureModifierTask& modifier,
                                        double world_x, double world_y)
{
    if (modifier.bitmap_item == nullptr || modifier.bitmap_size < 8U ||
        modifier.bitmap_line_width_shift >= 31U || modifier.local_pos_to_bitmap_pos <= 0.0F)
    {
        return false;
    }

    const auto x = static_cast<std::int64_t>(std::floor(
        (world_x - modifier.cube_min_x) * static_cast<double>(modifier.local_pos_to_bitmap_pos)));
    const auto y = static_cast<std::int64_t>(std::floor(
        (world_y - modifier.cube_min_y) * static_cast<double>(modifier.local_pos_to_bitmap_pos)));
    if (x < 0 || y < 0 || x >= static_cast<std::int64_t>(modifier.bitmap_size) ||
        y >= static_cast<std::int64_t>(modifier.bitmap_size))
    {
        return false;
    }

    const std::size_t row_stride = std::size_t{1} << modifier.bitmap_line_width_shift;
    const std::size_t byte_offset = static_cast<std::size_t>(y) * row_stride +
                                    static_cast<std::size_t>(x) / 8U;
    const std::uint8_t mask = static_cast<std::uint8_t>(1U << (static_cast<std::uint32_t>(x) & 7U));
    return (modifier.bitmap_item[byte_offset] & mask) != 0U;
}

// Retail child index transform table at VA 0x00548060 (64 bytes: 8 cases x 8 children).
constexpr std::array<std::uint8_t, 64> g_548060 = {
    0, 1, 2, 3, 4, 5, 6, 7, // transform case 0: identity
    2, 0, 3, 1, 6, 4, 7, 5, // transform case 1: rotate Z 90 deg CCW
    3, 2, 1, 0, 7, 6, 5, 4, // transform case 2: rotate Z 180 deg CCW
    1, 3, 0, 2, 5, 7, 4, 6, // transform case 3: rotate Z -90 deg CCW
    1, 0, 3, 2, 5, 4, 7, 6, // transform case 4: flip YOZ plane
    3, 1, 2, 0, 7, 5, 6, 4, // transform case 5: flip YOZ plane + rotate Z 90 deg
    2, 3, 0, 1, 6, 7, 4, 5, // transform case 6: flip YOZ plane + rotate Z 180 deg
    0, 2, 1, 3, 4, 6, 5, 7, // transform case 7: flip YOZ plane + rotate Z -90 deg
};

// Retail child orientation composition table (64 bytes: parent_case x child_case).
constexpr std::array<std::uint8_t, 64> g_cube_trans_table = {
    0, 1, 2, 3, 4, 5, 6, 7,
    1, 2, 3, 0, 5, 6, 7, 4,
    2, 3, 0, 1, 6, 7, 4, 5,
    3, 0, 1, 2, 7, 4, 5, 6,
    4, 7, 6, 5, 0, 3, 2, 1,
    5, 4, 7, 6, 1, 0, 3, 2,
    6, 5, 4, 7, 2, 1, 0, 3,
    7, 6, 5, 4, 3, 2, 1, 0,
};

constexpr std::int32_t ROOT_CUBE_HALF_SIZE = 1 << 30; // 1,073,741,824 units

// Retail storage addresses ported to engine globals:
// Material table storage at 0x00C28FE0 (32 groups x 900 bytes).
std::array<TerrainMaterialGroup, 32> g_c28fe0{};

// Tile map storage at 0x00C30080 (64 tilemaps x 64 bytes).
std::array<TerrainTileMapTable, 64> g_c30080{};

// Active texture modifier and heightmap stacks:

std::array<std::uint32_t, 64> g_b97d40{};

[[nodiscard]] std::vector<std::byte> read_binary_file(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        return {};
    }
    const auto size = file.tellg();
    if (size <= 0)
    {
        return {};
    }
    std::vector<std::byte> buffer(static_cast<std::size_t>(size));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));
    return buffer;
}

[[nodiscard]] int get_highest_bit(std::uint32_t value)
{
    int bit = 0;
    while (value > 1U)
    {
        value >>= 1U;
        ++bit;
    }
    return bit;
}

double CalcHMPDeltaZ(const TerrainHeightMapTask& height_map, double vert_x, double vert_y)
{
    if (height_map.height_map_item == nullptr)
    {
        return 0.0;
    }
    const auto* height_map_item = reinterpret_cast<const std::uint8_t*>(height_map.height_map_item);
    const double xf = (vert_x - height_map.cube_min_x) * static_cast<double>(height_map.local_pos_to_hmp_pos);
    const double yf = (vert_y - height_map.cube_min_y) * static_cast<double>(height_map.local_pos_to_hmp_pos);
    const int int_xf = static_cast<int>(xf);
    const int int_yf = static_cast<int>(yf);
    const double decimal_xf = xf - static_cast<double>(int_xf);
    const double decimal_yf = yf - static_cast<double>(int_yf);
    const int shifts = height_map.height_map_line_width_shift;
    const int hmp_dim = 1 << shifts;
    const int hmp_row = hmp_dim + 1;
    const int iy0 = int_yf * hmp_row;
    int iy1 = (int_yf + 1) * hmp_row;
    if (int_yf >= hmp_dim)
    {
        iy1 = iy0;
    }
    const int ix0 = int_xf;
    int ix1 = int_xf + 1;
    if (int_xf >= hmp_dim)
    {
        ix1 = ix0;
    }
    const double h0 = static_cast<double>(*(height_map_item + iy0 + ix0));
    const double h1 = static_cast<double>(*(height_map_item + iy0 + ix1));
    const double h2 = static_cast<double>(*(height_map_item + iy1 + ix0));
    const double h3 = static_cast<double>(*(height_map_item + iy1 + ix1));
    const double x_dir_interpolated1 = (h1 - h0) * decimal_xf + h0;
    const double x_dir_interpolated2 = (h3 - h2) * decimal_xf + h2;
    const double y_dir_interpolated = (x_dir_interpolated2 - x_dir_interpolated1) * decimal_yf + x_dir_interpolated1;
    return (y_dir_interpolated - 64.0) * 256.0;
}

// Retail argument helpers matching Script_GetArg and Script_GetArgFloat
inline int Script_GetArg(const float* args, int index)
{
    return args != nullptr ? static_cast<int>(args[index]) : 0;
}

inline float Script_GetArgFloat(const float* args, int index)
{
    return args != nullptr ? args[index] : 0.0f;
}

// -------------------------------------------------------------------------------------------------
// Raw little-endian accessors for the retail record/table layout.  The transcription deliberately
// keeps byte offsets rather than introducing guessed structures for the runtime-populated tables.
// -------------------------------------------------------------------------------------------------
inline std::uint32_t load_u32(const void* base, std::size_t offset) noexcept
{
    std::uint32_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline std::int32_t load_i32(const void* base, std::size_t offset) noexcept
{
    std::int32_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline std::uint16_t load_u16(const void* base, std::size_t offset) noexcept
{
    std::uint16_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline float load_f32(const void* base, std::size_t offset) noexcept
{
    float value = 0.0f;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline double load_f64(const void* base, std::size_t offset) noexcept
{
    double value = 0.0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline void store_u32(void* base, std::size_t offset, std::uint32_t value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

inline void store_i32(void* base, std::size_t offset, std::int32_t value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

inline void store_f32(void* base, std::size_t offset, float value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

inline std::uint8_t* pointer_from_bits(std::uint32_t value) noexcept
{
    return reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(value));
}

inline std::uint32_t pointer_bits(const void* value) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value));
}

// One of the three identical free-list pools.  `handles` is byte addressed, exactly as the retail
// instructions address it (`mov edi,[ecx+edi]` with the pool offset in ecx, no scale).
struct RetailPool
{
    std::int32_t*& handles;
    std::int32_t& count;
    std::int32_t& limit;
    std::int32_t*& map;
    std::int32_t& base;
    std::int32_t& stride;
};

inline RetailPool pool_a() noexcept
{
    return {g_b12b74, g_b12b68, g_b12b6c, g_b12b64, g_b12b60, g_b12b70};
}

inline RetailPool pool_b() noexcept
{
    return {g_b12c54, g_b12c48, g_b12c4c, g_b12c44, g_b12c40, g_b12c50};
}

inline RetailPool pool_c() noexcept
{
    return {g_b16cd4, g_b16cc8, g_b16ccc, g_b16cc4, g_b16cc0, g_b16cd0};
}

inline std::uint32_t pool_allocate(RetailPool pool) noexcept
{
    const std::int32_t slot = pool.count;
    const std::uint32_t offset =
        static_cast<std::uint32_t>(pool.map[slot] * pool.stride + pool.base);
    store_i32(pool.handles, offset, slot);
    ++pool.count;
    return offset;
}

// 0x004D23CC family: remove `data` from the pool and repair the map/handle back-references.
void pool_release(std::uint32_t data, RetailPool pool) noexcept
{
    const std::int32_t handle = load_i32(pool.handles, data);
    const std::int32_t top = --pool.count;
    if (handle != top)
    {
        const std::int32_t slot = pool.map[top] * pool.stride + pool.base;
        const std::int32_t old_mapping = pool.map[handle];
        const std::int32_t moved_handle = load_i32(pool.handles, slot);
        pool.map[handle] = pool.map[moved_handle];
        pool.map[moved_handle] = old_mapping;
        store_i32(pool.handles, slot, handle);
    }
}

// Retail one-bit coverage test used by the phase-2 material selection.  Row addressing here is
// `(y << shift) + (x >> 3)`, without the extra +y term used by the height sampler below.
inline bool coverage_bit(const std::uint8_t* bitmap, std::int32_t x, std::int32_t y,
                         std::uint32_t shift) noexcept
{
    const std::uint32_t row = static_cast<std::uint32_t>(y) << (shift & 31U);
    const std::uint32_t byte_index = row + static_cast<std::uint32_t>(x >> 3);
    const std::uint8_t value = bitmap[static_cast<std::int32_t>(byte_index)];
    return ((value >> (x & 7)) & 1U) != 0;
}

// Height sampler used while building vertices.  The retail index is
// `(y + (y << shift)) + x`, i.e. rows are `1 + (1 << shift)` bytes apart.  The four samples are
// signed bytes; the 64.0 subtraction and 256.0 scale are the retail literals at 0x00534AE0 and
// 0x005342E8.
inline float sample_height_bilinear(const std::uint8_t* bitmap, std::int32_t x, std::int32_t y,
                                    double frac_x, double frac_y, std::uint32_t shift) noexcept
{
    const std::uint32_t row_shift = 1U << (shift & 31U);
    const std::int32_t stride = static_cast<std::int32_t>(row_shift + 1U);
    const std::uint32_t base_u = static_cast<std::uint32_t>(y) +
                                 (static_cast<std::uint32_t>(y) << (shift & 31U)) +
                                 static_cast<std::uint32_t>(x);
    const std::int32_t base = static_cast<std::int32_t>(base_u);
    const double h00 = static_cast<double>(static_cast<std::int8_t>(bitmap[base]));
    const double h10 = static_cast<double>(static_cast<std::int8_t>(bitmap[base + 1]));
    const double h01 = static_cast<double>(static_cast<std::int8_t>(bitmap[base + stride]));
    const double h11 = static_cast<double>(static_cast<std::int8_t>(bitmap[base + stride + 1]));
    const double top = (h10 - h00) * frac_x + h00;
    const double bottom = (h11 - h01) * frac_x + h01;
    return static_cast<float>(((bottom - top) * frac_y + top - 64.0) * 256.0);
}

// -------------------------------------------------------------------------------------------------
// 0x00510010 body.  One 0x30-byte contour record is decoded into a terrain chunk: three world
// coordinates, an LOD level, a packed material token, four texture-modifier indices, and a cache
// entry.  The record's node supplies the vertex count and the packed per-vertex position words.
//
// There is no opcode dispatch and no command-byte stream in this function: `node + 8 + int16(node+2)`
// points at an array of `int16(node+4) + int16(node+6)` 32-bit words, each of which packs three
// signed 6-bit offsets (z = bits 14..19, y = bits 20..25, x = bits 26..31, each biased by 0x18)
// and a low 6-bit material/height slot.  The vertex words are not a byte-code stream.
//
// Retail state written here: g_c31090 += record count; g_b16c9c (descriptor count); g_b12c60 and
// g_b12c5c descriptor entries; the three free-list pools; and the cache entry at record+0x2C
// (+0x10 stamp, +0x14 allocation type, +0x18 data).  Unknown table contents (0x00BA2030,
// 0x00BA5050, 0x00BA80B0, 0x00C28FE4 and 0x00C30080) are represented by candidate storage and are
// listed in the evidence record as unresolved.
// -------------------------------------------------------------------------------------------------
void decode_chunk_record(std::uint8_t* record, std::int32_t& cached_neighbor_token,
                         std::uint8_t* neighbors)
{
    // ---- record header -------------------------------------------------------------------------
    const std::int32_t level = load_i32(record, 0x18);
    const std::uint32_t flags = load_u32(record, 0x1C);
    std::uint8_t* const node = pointer_from_bits(load_u32(record, 0x20));
    const std::uint32_t token = load_u32(record, 0x24);
    const std::uint32_t token_index = token & 0xFFFFU;                 // 0xFFFF = no page
    const std::uint32_t modifier_index = (token >> 16) & 0xFFU;        // byte at record+0x26
    const std::uint32_t neighbor_token = load_u32(record, 0x28);
    std::uint8_t* const cache = pointer_from_bits(load_u32(record, 0x2C));
    const std::uint32_t flag_bit0 = flags & 1U;
    const std::uint32_t axis_flag = flags & 4U;

    // ---- chunk allocation (0x5100DB..0x510107) -------------------------------------------------
    std::uint32_t chunk_bits = 0;
    std::int32_t allocation_type = 0;
    if (g_b12b68 <= g_b12b6c - 3)
    {
        chunk_bits = pool_allocate(pool_a());
        allocation_type = 0;
    }
    else if (g_b12c48 <= g_b12c4c - 3)
    {
        chunk_bits = pool_allocate(pool_b());
        allocation_type = 1;
    }
    else if (g_b16cc8 <= g_b16ccc - 3)
    {
        chunk_bits = pool_allocate(pool_c());
        allocation_type = 2;
    }
    else
    {
        // 0x510107: no pool has room; the cache entry is stamped unused and the record is skipped.
        store_u32(cache, 0x10, 0xFFFFFFFFU);
        return;
    }

    store_u32(cache, 0x18, chunk_bits);
    store_u32(cache, 0x14, static_cast<std::uint32_t>(allocation_type));

    // ---- descriptor publication (0x5101BF..0x5101E0) ------------------------------------------
    const std::int32_t descriptor_index = g_b16c9c;
    g_b12c60[static_cast<std::size_t>(descriptor_index)] = chunk_bits;
    g_b16c9c = descriptor_index + 1;

    std::uint8_t* chunk = pointer_from_bits(chunk_bits);

    // ---- material page parameters (0x5101E6..0x5101F5) ----------------------------------------
    bool has_material_page = token_index != 0xFFFFU;
    float material_scale = 0.0f;
    float material_offset = 0.0f;
    float material_origin_x = 0.0f;
    float material_origin_y = 0.0f;
    std::int32_t material_page = 0;
    if (has_material_page)
    {
        const std::uint8_t* const page =
            g_ba80b0_material_pages.data() + static_cast<std::size_t>(token_index) * 0x30U;
        material_scale = load_f32(page, 0x00) * (1.0f / 256.0f);
        material_offset = load_f32(page, 0x04) * (1.0f / 256.0f);
        material_origin_x = static_cast<float>(load_f64(page, 0x08));
        material_origin_y = static_cast<float>(load_f64(page, 0x10));
        material_page = load_i32(page, 0x18);
    }

    // ---- texture-modifier bitmap parameters (0x51020C..0x51025D) ------------------------------
    const std::int32_t modifier = level < 0xE ? 0 : static_cast<std::int32_t>(modifier_index);
    float modifier_scale = 0.0f;
    float modifier_origin_x = 0.0f;
    float modifier_origin_y = 0.0f;
    std::uint32_t modifier_shift = 0;
    const std::uint8_t* modifier_bitmap = nullptr;
    if (modifier != 0)
    {
        const std::uint8_t* const p =
            g_ba2030_texture_modifiers.data() + static_cast<std::size_t>(modifier) * 0x30U;
        modifier_scale = load_f32(p, 0x10);
        modifier_origin_x = static_cast<float>(load_f64(p, 0x18));
        modifier_origin_y = static_cast<float>(load_f64(p, 0x20));
        modifier_bitmap = pointer_from_bits(load_u32(p, 0x28));
        modifier_shift = (load_u32(p, 0x0C) & 0xFFU) + 3U;
    }

    // ---- neighbouring texture-modifier records (0x510270..0x510341) ---------------------------
    // The four 0x18-byte records are only rebuilt when the packed token changes.  A zero token byte
    // clears only the first dword of its record; the remaining 0x14 bytes keep their previous
    // contents, which is why the buffer persists across records.
    if (cached_neighbor_token != static_cast<std::int32_t>(neighbor_token))
    {
        cached_neighbor_token = static_cast<std::int32_t>(neighbor_token);
        std::uint32_t packed = neighbor_token;
        for (std::uint32_t k = 0; k < 4U; ++k)
        {
            const std::uint32_t byte = packed & 0xFFU;
            packed >>= 8;
            std::uint8_t* const dst = neighbors + static_cast<std::size_t>(k) * 0x18U;
            if (byte == 0)
            {
                store_u32(dst, 0x00, 0);
            }
            else
            {
                const std::uint8_t* const src =
                    g_ba5050_neighbor_modifiers.data() + static_cast<std::size_t>(byte) * 0x30U;
                store_u32(dst, 0x00, load_u32(src, 0x0C)); // line-width shift / present flag
                store_u32(dst, 0x04, load_u32(src, 0x10)); // local-position scale (float bits)
                store_f32(dst, 0x08, static_cast<float>(load_f64(src, 0x18))); // origin x
                store_f32(dst, 0x0C, static_cast<float>(load_f64(src, 0x20))); // origin y
                store_u32(dst, 0x10, load_u32(src, 0x28)); // bitmap base
                store_u32(dst, 0x14, load_u32(src, 0x00)); // material index
            }
        }
    }

    // ---- node counts and packed vertex words (0x510341..0x5103C6) -------------------------------
    const std::int32_t first_vertices = static_cast<std::int16_t>(load_u16(node, 0x04));
    const std::int32_t extra_vertices = static_cast<std::int16_t>(load_u16(node, 0x06));
    const std::int32_t total_vertices = first_vertices + extra_vertices;
    const std::uint32_t vertex_data_bytes = static_cast<std::uint32_t>(total_vertices) * 0x14U -
                                            (static_cast<std::uint32_t>(first_vertices) << 3);
    std::memcpy(chunk, record, 0x18);
    store_u32(chunk, 0x18, (static_cast<std::uint32_t>(first_vertices) << 16) |
                               static_cast<std::uint32_t>(total_vertices));
    store_i32(chunk, 0x1C, level);

    const double vertex_scale =
        static_cast<double>(static_cast<std::int32_t>(1U << ((0x1BU - level) & 31U)));
    std::uint8_t* vcursor = chunk + 0x20;
    std::uint8_t* command = node + 8 + static_cast<std::int16_t>(load_u16(node, 0x02));

    // Decode one packed vertex word and apply the retail axis/mirror permutation.
    const auto orient_offsets = [flags, axis_flag](std::uint32_t packed, std::int32_t& x,
                                                   std::int32_t& y, std::int32_t& z) {
        z = static_cast<std::int32_t>((packed >> 0x0E) & 0x3FU) - 0x18;
        x = static_cast<std::int32_t>((packed >> 0x1A) & 0x3FU) - 0x18;
        y = static_cast<std::int32_t>((packed >> 0x14) & 0x3FU) - 0x18;
        if (axis_flag != 0)
        {
            x = -x;
        }
        if ((flags & 1U) == 0)
        {
            if ((flags & 2U) != 0)
            {
                x = -x;
                y = -y;
            }
        }
        else if ((flags & 2U) == 0)
        {
            const std::int32_t old_x = x;
            x = -y;
            y = old_x;
        }
        else
        {
            const std::int32_t old_x = x;
            x = y;
            y = -old_x;
        }
    };

    // ---- vertices 0..first-1: four floats, with the optional modifier height -------------------
    for (std::int32_t i = 0; i < first_vertices; ++i)
    {
        const std::uint32_t packed = load_u32(command, 0);
        command += 4;
        std::int32_t x = 0;
        std::int32_t y = 0;
        std::int32_t z = 0;
        orient_offsets(packed, x, y, z);
        const double world_x =
            static_cast<double>(x) * vertex_scale / 3.0 + load_f64(record, 0x00);
        const double world_y =
            static_cast<double>(y) * vertex_scale / 3.0 + load_f64(record, 0x08);
        double world_z = static_cast<double>(z) * vertex_scale / 3.0 + load_f64(record, 0x10);
        float height = 0.0f;
        if (modifier != 0)
        {
            const double sx = (world_x - static_cast<double>(modifier_origin_x)) *
                              static_cast<double>(modifier_scale);
            const double sy = (world_y - static_cast<double>(modifier_origin_y)) *
                              static_cast<double>(modifier_scale);
            const std::int32_t ix = static_cast<std::int32_t>(static_cast<std::int64_t>(sx));
            const std::int32_t iy = static_cast<std::int32_t>(static_cast<std::int64_t>(sy));
            height = sample_height_bilinear(modifier_bitmap, ix, iy,
                                            sx - static_cast<double>(ix),
                                            sy - static_cast<double>(iy), modifier_shift);
            if (level <= 0xE)
            {
                // 0x510590: the sampled height becomes the fourth component.
            }
            else
            {
                // 0x510588: add the height to world z and leave the fourth component zero.
                world_z += static_cast<double>(height);
                height = 0.0f;
            }
        }
        store_f32(vcursor, 0x00, static_cast<float>(world_x));
        store_f32(vcursor, 0x04, static_cast<float>(world_y));
        store_f32(vcursor, 0x08, static_cast<float>(world_z));
        store_f32(vcursor, 0x0C, height);
        vcursor += 0x10;
    }

    // ---- vertices first..total-1: three floats plus the packed low-6-bit material slot ---------
    for (std::int32_t i = first_vertices; i < total_vertices; ++i)
    {
        const std::uint32_t packed = load_u32(command, 0);
        command += 4;
        std::int32_t x = 0;
        std::int32_t y = 0;
        std::int32_t z = 0;
        orient_offsets(packed, x, y, z);
        const double world_x =
            static_cast<double>(x) * vertex_scale / 3.0 + load_f64(record, 0x00);
        const double world_y =
            static_cast<double>(y) * vertex_scale / 3.0 + load_f64(record, 0x08);
        double world_z = static_cast<double>(z) * vertex_scale / 3.0 + load_f64(record, 0x10);
        if (modifier != 0)
        {
            const double sx = (world_x - static_cast<double>(modifier_origin_x)) *
                              static_cast<double>(modifier_scale);
            const double sy = (world_y - static_cast<double>(modifier_origin_y)) *
                              static_cast<double>(modifier_scale);
            const std::int32_t ix = static_cast<std::int32_t>(static_cast<std::int64_t>(sx));
            const std::int32_t iy = static_cast<std::int32_t>(static_cast<std::int64_t>(sy));
            const float height = sample_height_bilinear(
                modifier_bitmap, ix, iy, sx - static_cast<double>(ix),
                sy - static_cast<double>(iy), modifier_shift);
            world_z += static_cast<double>(height);
        }
        store_f32(vcursor, 0x00, static_cast<float>(world_x));
        store_f32(vcursor, 0x04, static_cast<float>(world_y));
        store_f32(vcursor, 0x08, static_cast<float>(world_z));
        store_f32(vcursor, 0x0C, static_cast<float>(packed & 0x3FU));
        vcursor += 0x10;
    }

    std::uint8_t* end_vertices = vcursor;

    // ---- per-vertex material selection / UVs (0x5107BC..0x5109C3) -----------------------------
    const std::size_t meta_bytes =
        total_vertices > 0 ? static_cast<std::size_t>(total_vertices) * 0x24U : 0U;
    std::vector<std::uint8_t> metadata(meta_bytes, 0);
    std::uint8_t* const meta = metadata.data();
    std::int32_t material_bits = 0;
    for (std::int32_t i = 0; i < total_vertices; ++i)
    {
        const std::uint8_t* const vertex = chunk + 0x20 + static_cast<std::size_t>(i) * 0x10U;
        const float world_x = load_f32(vertex, 0x00);
        const float world_y = load_f32(vertex, 0x04);
        const float world_z = load_f32(vertex, 0x08);
        const float vertex_material = load_f32(vertex, 0x0C);
        std::uint8_t* const m = meta + static_cast<std::size_t>(i) * 0x24U;
        store_f32(m, 0x1C, (world_x - material_origin_x) * material_scale + material_offset);
        store_f32(m, 0x20, (world_y - material_origin_y) * material_scale + material_offset);

        std::int32_t coverage = 0;
        std::int32_t output_material = 0xFF;
        if (load_u32(neighbors, 0x00) != 0)
        {
            bool hit = false;
            const auto test_entry = [&](const std::uint8_t* entry) {
                const double sx = static_cast<double>(load_f32(entry, 0x04)) *
                                  (static_cast<double>(world_x) -
                                   static_cast<double>(load_f32(entry, 0x08)));
                const double sy = static_cast<double>(load_f32(entry, 0x04)) *
                                  (static_cast<double>(world_y) -
                                   static_cast<double>(load_f32(entry, 0x0C)));
                const std::int32_t ix = static_cast<std::int32_t>(static_cast<std::int64_t>(sx));
                const std::int32_t iy = static_cast<std::int32_t>(static_cast<std::int64_t>(sy));
                return coverage_bit(pointer_from_bits(load_u32(entry, 0x10)), ix, iy,
                                    load_u32(entry, 0x00));
            };
            if (test_entry(neighbors))
            {
                coverage = load_i32(neighbors, 0x14);
                hit = true;
            }
            for (std::uint32_t k = 1; k < 4U && !hit; ++k)
            {
                const std::uint8_t* const entry = neighbors + static_cast<std::size_t>(k) * 0x18U;
                if (load_u32(entry, 0x00) == 0)
                {
                    break;
                }
                if (test_entry(entry))
                {
                    coverage = load_i32(entry, 0x14);
                    hit = true;
                }
            }
            if (!hit)
            {
                if (i < first_vertices)
                {
                    output_material = 0xFF;
                }
                else
                {
                    const std::int32_t source_index = static_cast<std::int32_t>(vertex_material);
                    store_i32(m, 0x00, source_index);
                    const std::uint32_t packed_source =
                        load_u32(meta, static_cast<std::size_t>(source_index) * 0x24U + 0x14U);
                    output_material = static_cast<std::int32_t>(packed_source >> 16);
                    if (output_material == coverage)
                    {
                        output_material = 0xFF;
                    }
                }
            }
        }
        else if (i >= first_vertices)
        {
            store_i32(m, 0x00, static_cast<std::int32_t>(vertex_material));
        }

        store_f32(m, 0x04, world_x - load_f32(record, 0x00));
        store_f32(m, 0x08, world_y - load_f32(record, 0x08));
        store_f32(m, 0x0C, world_z - load_f32(record, 0x10));
        material_bits |= static_cast<std::int32_t>(1U << (coverage & 31));
        store_i32(m, 0x14, static_cast<std::int32_t>((static_cast<std::uint32_t>(coverage) << 16) |
                                                     static_cast<std::uint32_t>(output_material)));
    }

    // ---- material page list (bitmask walk) ------------------------------------------------------
    std::array<std::int32_t, 32> material_list{};
    std::int32_t material_count = 0;
    for (std::uint32_t bit = 0, bits = static_cast<std::uint32_t>(material_bits); bits != 0;
         ++bit, bits >>= 1)
    {
        if ((bits & 1) != 0 && material_count < 32)
        {
            material_list[static_cast<std::size_t>(material_count)] = bit;
            ++material_count;
        }
    }

    // ---- requested chunk size and optional pool promotion (0x5109C9..0x510BD2) ----------------
    const std::uint32_t requested_size =
        vertex_data_bytes +
        (((vertex_data_bytes + 4U) * static_cast<std::uint32_t>(material_count) +
          (static_cast<std::uint32_t>(total_vertices) << 3)) << 1) +
        0x3CU;
    const std::int32_t size_signed = static_cast<std::int32_t>(requested_size);
    const std::int32_t old_type = load_i32(cache, 0x14);
    const std::uint32_t old_bits = chunk_bits;
    bool build_directly = size_signed < 0x640;
    if (!build_directly && old_type == 1 && size_signed < 0xC80)
    {
        build_directly = true;
    }
    if (!build_directly && old_type == 2 && size_signed < 0x1F40)
    {
        build_directly = true;
    }

    bool promoted = false;
    if (!build_directly)
    {
        // 0x510A9D..0x510B52: A -> B when size < 0xC80, otherwise A/B -> C while size < 0x1F40.
        // A full B pool falls through to the C pool.  Pool C never promotes.
        const bool attempt_b = old_type == 0 && size_signed < 0xC80;
        const bool attempt_c = (old_type == 0 || old_type == 1) && size_signed < 0x1F40;
        std::uint32_t new_bits = old_bits;
        std::int32_t new_type = old_type;
        if (attempt_b && g_b12c48 < g_b12c4c - 2)
        {
            new_bits = pool_allocate(pool_b());
            new_type = 1;
            promoted = true;
        }
        else if (attempt_c && g_b16cc8 < g_b16ccc - 2)
        {
            new_bits = pool_allocate(pool_c());
            new_type = 2;
            promoted = true;
        }

        if (promoted)
        {
            const std::uint32_t used_bytes = static_cast<std::uint32_t>(end_vertices - chunk);
            std::uint8_t* const new_chunk = pointer_from_bits(new_bits);
            std::memcpy(new_chunk, chunk, used_bytes);
            chunk = new_chunk;
            vcursor = new_chunk + used_bytes;
            end_vertices = vcursor;
            store_u32(cache, 0x18, new_bits);
            store_u32(cache, 0x14, static_cast<std::uint32_t>(new_type));
            g_b12c5c[static_cast<std::size_t>(g_b16c9c)] = new_bits;
        }

        // 0x510BD2: the old slot is released whether or not a replacement was found.
        if (old_type == 0)
        {
            pool_release(old_bits, pool_a());
        }
        else if (old_type == 1)
        {
            pool_release(old_bits, pool_b());
        }
        else if (old_type == 2)
        {
            pool_release(old_bits, pool_c());
        }

        if (!promoted)
        {
            store_u32(cache, 0x18, 0);
            store_u32(cache, 0x10, 0xFFFFFFFFU);
            --g_b16c9c;
            return;
        }
    }

    // ---- per-material page records (0x510D89..0x510F8A) ---------------------------------------
    struct PageRecord
    {
        std::array<float, 8> values{};
        std::uint32_t slot_a = 0;
        std::uint32_t slot_b = 0;
    };
    std::array<PageRecord, 32> pages{};
    const std::uint8_t* const material_table =
        reinterpret_cast<const std::uint8_t*>(g_c28fe0.data()) + 4;
    const std::uint8_t* const tile_table =
        reinterpret_cast<const std::uint8_t*>(g_c30080.data());
    const double page_x = load_f64(record, 0x00) + g_b12ba0;
    const double page_y = load_f64(record, 0x08) + g_b12ba8;
    const double page_z = load_f64(record, 0x10) + g_b12bb0;
    const auto sar = [](std::int32_t value, std::int32_t count) {
        return value >> (count & 31);
    };
    for (std::int32_t k = 0; k < material_count; ++k)
    {
        const std::uint32_t material =
            static_cast<std::uint32_t>(material_list[static_cast<std::size_t>(k)]);
        const std::uint8_t* const entry =
            material_table + static_cast<std::size_t>(level) * 0x1CU + material * 0x384U;
        const float f_tex2 = load_f32(entry, 0x0C);
        const std::uint32_t tex2_index = load_u32(entry, 0x10);
        const std::int32_t f_tex1 = load_i32(entry, 0x14);
        const std::uint32_t tex1_index = load_u32(entry, 0x18);
        const std::int32_t t1 =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex2) * page_x),
                0x17 - level) & 0x7FF;
        const std::int32_t t2 =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex2) * page_y),
                0x17 - level) & 0x7FF;
        const std::int32_t t3 =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex1) * page_x),
                0x16 - level) & 0x7FF;
        const std::int32_t t4 =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex1) * page_y),
                0x16 - level) & 0x7FF;
        const std::int32_t f_tex2_z =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex2) * page_z), 0x17 - level);
        const std::int32_t f_tex1_z =
            sar(static_cast<std::int32_t>(static_cast<double>(f_tex1) * page_z), 0x16 - level);
        const float page_scale = static_cast<float>(0x40000000 >> (level & 31));
        PageRecord& page = pages[static_cast<std::size_t>(k)];
        page.values[0] = (static_cast<float>(f_tex1_z) + 1.0f) * (1.0f / 256.0f);
        page.values[1] = (static_cast<float>(t4) + 1.0f) * (1.0f / 256.0f);
        page.values[2] = (static_cast<float>(t3 & 0xFF) + 1.0f) * (1.0f / 256.0f);
        page.values[3] = static_cast<float>(f_tex1) / page_scale;
        page.values[4] = (static_cast<float>(f_tex2_z) + 1.0f) * (1.0f / 256.0f);
        page.values[5] = (static_cast<float>(t2 & 0xFF) + 1.0f) * (1.0f / 256.0f);
        page.values[6] = (static_cast<float>(t1 & 0xFF) + 1.0f) * (1.0f / 256.0f);
        page.values[7] = f_tex2 * 0.5f / page_scale;
        page.slot_b = tile_table[(t3 >> 8) + (((t4 >> 8) + tex1_index * 8U) << 3)];
        page.slot_a = tile_table[(t1 >> 8) + (((t2 >> 8) + tex2_index * 8U) << 3)];
    }

    // ---- second header + per-material vertex streams (0x510D13..0x5113EE) ----------------------
    store_u32(end_vertices, 0x00, flags);
    store_u32(end_vertices, 0x04, pointer_bits(node + 8));
    store_u32(end_vertices, 0x08, static_cast<std::uint32_t>(material_count));
    store_u32(end_vertices, 0x0C,
              static_cast<std::uint32_t>(static_cast<std::int16_t>(load_u16(node, 0x00))));
    std::uint8_t* stream = end_vertices + 0x10;
    std::uint8_t* const command_start =
        node + 8 + static_cast<std::int16_t>(load_u16(node, 0x02));

    for (std::int32_t k = 0; k < material_count; ++k)
    {
        const PageRecord& page = pages[static_cast<std::size_t>(k)];
        std::uint8_t* const block_a = stream;
        std::uint8_t* const block_b = stream + vertex_data_bytes + 4U;
        store_u32(block_a, 0x00,
                  pointer_bits(g_c30060 + static_cast<std::size_t>(page.slot_b) * 0x24U + 0xC));
        store_u32(block_b, 0x00,
                  pointer_bits(g_c30060 + static_cast<std::size_t>(page.slot_a) * 0x24U + 0xC));
        std::uint8_t* a = block_a + 4;
        std::uint8_t* b = block_b + 4;
        const std::uint8_t* command_cursor = command_start;
        for (std::int32_t i = 0; i < total_vertices; ++i)
        {
            const std::uint32_t packed_word = load_u32(command_cursor, 0);
            command_cursor += 4;
            const std::uint32_t rotation_a = (packed_word >> 8) & 3U;
            const std::uint32_t rotation_b = (packed_word >> 6) & 3U;
            const std::uint8_t* const m = meta + static_cast<std::size_t>(i) * 0x24U;
            std::int32_t index = 0;
            const std::uint8_t* source = m;
            if (i < first_vertices)
            {
                index = i;
            }
            else
            {
                index = load_i32(m, 0x00);
                source = meta + static_cast<std::size_t>(index) * 0x24U;
            }
            const float dx = load_f32(m, 0x04);
            const float dy = load_f32(m, 0x08);
            const float dz = load_f32(m, 0x0C);
            const float sx = load_f32(source, 0x04);
            const float sy = load_f32(source, 0x08);
            const float sz = load_f32(source, 0x0C);
            const std::uint32_t packed_meta = load_u32(m, 0x14);

            float x1 = 0.0f;
            float y_first = 0.0f;
            float nx = 0.0f;
            float x2 = 0.0f;
            if (rotation_a == 0)
            {
                y_first = dy * page.values[3] + page.values[1];
                x1 = dx * page.values[3] + page.values[2];
                nx = sy * page.values[3] + page.values[1] - y_first;
                x2 = sx * page.values[3] + page.values[2];
            }
            else
            {
                float av = 0.0f;
                float bv = 0.0f;
                float offset = 0.0f;
                if (((rotation_a ^ flag_bit0) & 1U) == 0)
                {
                    av = dx;
                    bv = sx;
                    offset = page.values[2];
                }
                else
                {
                    av = dy;
                    bv = sy;
                    offset = page.values[1];
                }
                y_first = dz * page.values[3] + page.values[0];
                x1 = av * page.values[3] + offset;
                nx = sz * page.values[3] + page.values[0] - y_first;
                x2 = bv * page.values[3] + offset;
            }

            float y1 = 0.0f;
            float y2 = 0.0f;
            float ny = 0.0f;
            float y3 = 0.0f;
            if (rotation_b == 0)
            {
                y1 = dx * page.values[7] + page.values[6];
                y2 = dy * page.values[7] + page.values[5];
                ny = sx * page.values[7] + page.values[6] - y1;
                y3 = sy * page.values[7] + page.values[5];
            }
            else
            {
                float av = 0.0f;
                float bv = 0.0f;
                float offset = 0.0f;
                if (((rotation_b ^ flag_bit0) & 1U) == 0)
                {
                    av = dx;
                    bv = sx;
                    offset = page.values[6];
                }
                else
                {
                    av = dy;
                    bv = sy;
                    offset = page.values[5];
                }
                y1 = av * page.values[7] + offset;
                y2 = dz * page.values[7] + page.values[4];
                ny = bv * page.values[7] + offset - y1;
                y3 = sz * page.values[7] + page.values[4];
            }

            const std::int32_t material = material_list[static_cast<std::size_t>(k)];
            const std::int32_t packed_low = static_cast<std::int32_t>(packed_meta & 0xFFU);
            const std::int32_t packed_high = static_cast<std::int32_t>(packed_meta >> 16);
            std::uint32_t marker_flags = 0;
            if (material == packed_low)
            {
                marker_flags = 8;
            }
            else if (material == packed_high)
            {
                marker_flags = 0x18;
            }
            const std::uint32_t marker = (static_cast<std::uint32_t>(index) << 5) |
                                         marker_flags |
                                         (static_cast<std::uint32_t>(index) << 16);
            store_u32(a, 0x00, marker);
            store_f32(a, 0x04, x1);
            store_f32(a, 0x08, y_first);
            store_u32(b, 0x00, marker + 4U);
            store_f32(b, 0x04, y1);
            store_f32(b, 0x08, y2);
            a += 0x0C;
            b += 0x0C;
            if (i >= first_vertices)
            {
                store_f32(a, 0x00, x2 - x1);
                store_f32(a, 0x04, nx);
                store_f32(b, 0x00, ny);
                store_f32(b, 0x04, y3 - y2);
                a += 0x08;
                b += 0x08;
            }
        }
        stream = block_b + vertex_data_bytes + 4U;
    }

    // ---- trailing descriptor list (0x5113D8..0x5114BD) ----------------------------------------
    if (!has_material_page)
    {
        store_u32(stream, 0x00, 0);
    }
    else
    {
        store_u32(stream, 0x00, static_cast<std::uint32_t>(material_page));
        stream += 4;
        for (std::int32_t i = 0; i < total_vertices; ++i)
        {
            const std::uint8_t* const m = meta + static_cast<std::size_t>(i) * 0x24U;
            const float u = load_f32(m, 0x1C);
            const float v = load_f32(m, 0x20);
            const std::int32_t index = i < first_vertices ? i : load_i32(m, 0x00);
            const std::uint8_t* const source =
                meta + static_cast<std::size_t>(index) * 0x24U;
            store_u32(stream, 0x00, (static_cast<std::uint32_t>(index) << 5) |
                                        (static_cast<std::uint32_t>(index) << 16));
            store_f32(stream, 0x04, u);
            store_f32(stream, 0x08, v);
            stream += 0x0C;
            if (i >= first_vertices)
            {
                store_f32(stream, 0x00, load_f32(source, 0x04) - u);
                store_f32(stream, 0x04, load_f32(source, 0x08) - v);
                stream += 0x08;
            }
        }
    }
}

} // namespace

bool TextureModifier_CoversPoint(const TerrainTextureModifierTask& modifier,
                                     double world_x, double world_y)
{
    return texture_modifier_covers_point_impl(modifier, world_x, world_y);
}

// Native: Terrain_CreateMaterial; retail VA 0x004C6670; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 CFG MCP on retail IGI.EXE (VA 0x004C6670-0x004C6755); candidate reference scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp.
// Behavior/CFG: Reads the material index with Script_GetArg(args,0) (0x004B8A20), stores arg1 at 0x00C28FE0 + index*900, then writes 32 LOD entries of 0x1C bytes starting at group+4 (disassembly 0x004C66AA-0x004C671C): int arg[i-1], float args[i], float args[i+1], float args[i+2]*0.5 (constant 0x3F000000 at 0x00533504), int arg[i+3], float args[i+4]*0.5, int arg[i+5], with i stepping by 7 from 3 while i < 0xE3; finally copies the 24-byte {0,0,0,0,0x00567C74,*uninitialized*} block to the context with rep movsd x6.
// Status/limits: VERIFIED; retail index arithmetic has no bounds check (negative/oversized indices write outside 0x00C28FE0 in retail), the port clamps into its 32-group store and documents that deviation; the context block's dwords 1 and 5 are uninitialized stack in retail and are written as zero by the port. Differential execution: scripts/native_unicorn_runner.py (8/8 bounded x86 replay fixtures); live game-loop reachability remains unproven.
void* Terrain_CreateMaterial(void* context, const float* args)
{
    if (args == nullptr)
    {
        return context;
    }
    const int material_id = Script_GetArg(args, 0);
    const std::size_t safe_mat_idx = (material_id >= 0 && static_cast<std::size_t>(material_id) < g_c28fe0.size())
                                         ? static_cast<std::size_t>(material_id)
                                         : 0U;
    char* dest_1 = reinterpret_cast<char*>(&g_c28fe0[safe_mat_idx]);
    *reinterpret_cast<int*>(dest_1) = Script_GetArg(args, 1);
    int i = 3;
    char* dest_2 = dest_1;

    do
    {
        *reinterpret_cast<int*>(dest_2 + 4) = Script_GetArg(args, i - 1);
        *reinterpret_cast<float*>(dest_2 + 8) = Script_GetArgFloat(args, i);
        *reinterpret_cast<float*>(dest_2 + 12) = Script_GetArgFloat(args, i + 1);
        *reinterpret_cast<float*>(dest_2 + 16) = Script_GetArgFloat(args, i + 2) * 0.5f;
        *reinterpret_cast<int*>(dest_2 + 20) = Script_GetArg(args, i + 3);
        *reinterpret_cast<float*>(dest_2 + 24) = Script_GetArgFloat(args, i + 4) * 0.5f;
        // Retail writes seven fields per 0x1C entry (disassembly 0x004C66AA-0x004C6716):
        // arg indices esi-1..esi+5 with esi stepping by 7, the last being `mov [edi+0x14], eax`
        // (entry offset +0x18, i.e. dest_2+0x1C) fed by `lea edx, [esi+5]; call Script_GetArg`.
        *reinterpret_cast<int*>(dest_2 + 28) = Script_GetArg(args, i + 5);
        i += 7;
        dest_2 += 0x1c;
    } while (i < 0xe3);

    if (context != nullptr)
    {
        std::memset(context, 0, 0x18);
        const std::uint32_t type_descriptor = 0x00567C74U;
        std::memcpy(static_cast<std::byte*>(context) + 0x10, &type_descriptor, sizeof(type_descriptor));
    }
    return context;
}

// Native: Terrain_CreateTileMap; retail VA 0x004C6760; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 CFG MCP on retail IGI.EXE (VA 0x004C6760-0x004C67D0); candidate reference scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp.
// Behavior/CFG: Reads the group index with Script_GetArg(args,0), stores 64 bytes at 0x00C30080 + group*64 where byte i = (int8)Script_GetArg(args,i+1) for i = 0..0x3F (disassembly 0x004C6781-0x004C6796), then copies the 24-byte {0,0,0,0,0x00567C74,*uninitialized*} block to the context with rep movsd x6.
// Status/limits: VERIFIED; retail group arithmetic has no bounds check, the port clamps into its 64-table store and documents that deviation; the context block's dwords 1 and 5 are uninitialized stack in retail and are written as zero by the port. Differential execution: scripts/native_unicorn_runner.py (8/8 bounded x86 replay fixtures); live game-loop reachability remains unproven.
void* Terrain_CreateTileMap(void* context, const float* args)
{
    if (args == nullptr)
    {
        return context;
    }
    const int item_2c = 0;
    const float* item_30 = args;
    int i = 0;
    const int group_idx = Script_GetArg(item_30, item_2c);
    const std::size_t safe_group_idx = (group_idx >= 0 && static_cast<std::size_t>(group_idx) < g_c30080.size())
                                           ? static_cast<std::size_t>(group_idx)
                                           : 0U;
    char* flagP = reinterpret_cast<char*>(&g_c30080[safe_group_idx]);

    do
    {
        item_30 = args;
        *(flagP + i) = static_cast<char>(Script_GetArg(item_30, i + 1));
        i += 1;
    } while (i < 0x40);

    if (context != nullptr)
    {
        std::memset(context, 0, 0x18);
        const std::uint32_t type_descriptor = 0x00567C74U;
        std::memcpy(static_cast<std::byte*>(context) + 0x10, &type_descriptor, sizeof(type_descriptor));
    }
    return context;
}

// Native: TextureModifier_Push; retail VA 0x00505E40; x86 cdecl void*(void*).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja view_1; RE-Agent radare2 ev_0041b4b3f7b953ab4744f3a74c261506c6a81a78b59bfd384961ac0184b2d01e.
// Behavior/CFG: Requires [self+0xBC] >= 0 and the source row [[self+0x68]+0xDCC] to hold a non-zero dword at counter*3 (0x00505E4E-0x00505E5E); pushes [self+0xE4] onto the 20-slot stack at 0x00B97DA0 under counter 0x00B97DF8, folds the packed bitmask 0x00BA5060 = (mask << 8) | (scratch+1), sets self[0xDC] = 1 and returns scratch+1; when the stack is full calls Debug_Error("Too many items in texture modifier stack!") (0x004AF7B0) and halts in the retail self-loop (0x00505E76 jmp $); otherwise self[0xDC] = 0 and returns self.
// Status/limits: STRUCTURALLY_VERIFIED; scripts/texture_modifier_push_diff_runner.py cannot link the current translation unit (undefined resource symbols in unrelated host functions), so it produces no behavioral pass; natural runtime integration remains provisional.
void* TextureModifier_Push(void* self)
{
    auto* bytes = static_cast<std::uint8_t*>(self);
    const auto counter_val = *reinterpret_cast<std::int32_t*>(bytes + 0xBC);
    if (counter_val >= 0)
    {
        // Retail 0x00505E4E-0x00505E5E validates the push source before touching the stack:
        // row = [[self+0x68] + 0xDCC], and row[counter*3] must be non-zero, otherwise the
        // push is refused with self[0xDC] = 0.
        const auto source_table =
            *reinterpret_cast<const std::uint8_t* const*>(bytes + 0x68);
        const auto source_row = *reinterpret_cast<const std::uint32_t* const*>(
            reinterpret_cast<const std::uint8_t*>(source_table) + 0xDCCu);
        if (source_row[static_cast<std::size_t>(counter_val) * 3U] != 0U)
        {
            if (g_b97df8 < 20)
            {
                const auto scratch_val = *reinterpret_cast<std::uint32_t*>(bytes + 0xE4);
                const std::uint32_t enabled = 1;
                std::memcpy(bytes + 0xDC, &enabled, sizeof(enabled));
                const auto stack_index = g_b97df8;
                g_b97da0[static_cast<std::size_t>(stack_index)] = scratch_val;
                const auto shifted_mask = g_ba5060 << 8U;
                g_ba5060 = shifted_mask;
                const auto result = *reinterpret_cast<std::uint32_t*>(bytes + 0xE4) + 1U;
                g_ba5060 = shifted_mask | result;
                g_b97df8 = stack_index + 1;
                return reinterpret_cast<void*>(static_cast<std::uintptr_t>(result));
            }
            Debug_Error("Too many items in texture modifier stack!");
            // Retail 0x00505E76: jmp $ — the fatal-error halt.
            for (;;)
            {
            }
        }
    }
    const std::uint32_t disabled = 0;
    std::memcpy(bytes + 0xDC, &disabled, sizeof(disabled));
    return self;
}

// Native: HeightMap_Enter; retail VA 0x00506F00; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja view_1 0x00506F00; RE-Agent radare2 ev_22f3f96b2b347b99a962a88ea7097bdba7db979f2778c016ff0e252ae2cc0754.
// Behavior/CFG: Rejects negative/disabled source rows; writes the DWORD flag and the active index; the source-level 0x00B97D40 stack push is elided in the production object.
// Status/limits: STRUCTURALLY_VERIFIED; retail 4-block control flow/returns re-checked; the compiled object dead-store-eliminates the retail 0x00B97D40 stack write, so no behavioral pass is claimed for the compiled candidate.
void* HeightMap_Enter(void* context)
{
    auto* bytes = static_cast<std::uint8_t*>(context);
    const auto entity_val = *reinterpret_cast<std::int32_t*>(bytes + 0xBC);
    if (entity_val < 0 ||
        (*reinterpret_cast<std::uint32_t**>(
            *reinterpret_cast<std::uint8_t**>(bytes + 0x68) + 0xDD0))[entity_val * 3] == 0U)
    {
        *reinterpret_cast<std::uint32_t*>(bytes + 0xE8) = 0U;
        return context;
    }
    const auto stored_index = *reinterpret_cast<std::uint32_t*>(bytes + 0xF8);
    *reinterpret_cast<std::uint32_t*>(bytes + 0xE8) = 1U;
    const auto stack_index = g_b97df4;
    g_b97d40[static_cast<std::size_t>(stack_index)] = stored_index + 1U;
    const auto active_index = *reinterpret_cast<std::uint32_t*>(bytes + 0xF8) + 1U;
    g_ba2058 = active_index;
    g_b97df4 = stack_index + 1;
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(active_index));
}

// Native: TerrainLightMap_PushEntry; retail VA 0x004F82B0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja and RE-Agent Radare2, docs/fidelity/evidence/lightmap-enter-20261020/; no second decompiler used for this correction.
// Behavior/CFG: Calls 0x004F1160, tests DWORD +0x108, writes DWORD +0xE0, pushes DWORD +0xD8 and increments 0x00B97D24. Both mode branches retained; no retail stack-cap branch exists here.
// Status/limits: STRUCTURALLY_VERIFIED; retail CFG structure checked, but no differential runner covers the host flag/stack behaviour; nonaliasing valid contexts and existing host stack storage only; overflow/storage layout and natural runtime integration remain unresolved.
void* TerrainLightMap_PushEntry(void* self)
{
    const auto mode = script::Script_GetParseMode();
    auto* bytes = static_cast<std::uint8_t*>(self);
    const auto counter_val = *reinterpret_cast<std::int32_t*>(bytes + 0x108);
    if (mode != 0)
    {
        if (counter_val != 0)
        {
            const auto scratch_val = *reinterpret_cast<std::uint32_t*>(bytes + 0xD8);
            *reinterpret_cast<std::uint32_t*>(bytes + 0xE0) = 1;
            const auto stack_index = g_b97d24;
            g_data_b97b20[static_cast<std::size_t>(stack_index)] = scratch_val;
            g_b97d24 = stack_index + 1;
            return reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_b97d24));
        }
    }
    else if (counter_val != 0)
    {
        const auto scratch_val = *reinterpret_cast<std::uint32_t*>(bytes + 0xD8);
        *reinterpret_cast<std::uint32_t*>(bytes + 0xE0) = 1;
        const auto stack_index = g_b97d24;
        g_data_b97b20[static_cast<std::size_t>(stack_index)] = scratch_val;
        g_b97d24 = stack_index + 1;
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_b97d24));
    }
    *reinterpret_cast<std::uint32_t*>(bytes + 0xE0) = 0;
    return self;
}

// Native: Terrain_Load; retail VA 0x004C6240; signature void* __cdecl(void* context, const char* base_path).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP (25 blocks, 1069 bytes), Radare2 CFG (25 nodes/36 edges), angr CFGFast (exact entry, no unresolved jumps), live retail capture 20261023T015650-level1-retail.
// Behavior/CFG: HOST FILE-STAGE BRIDGE; retail's single exit 0x004C666C returns DynObj_UpdateDynTree_11(context) (0x004CE3D0), i.e. arg1, and the live retail EAX 0x038910B4 equals the context argument.
// Status/limits: STRUCTURALLY_VERIFIED (host-backed bridge); no byte-identical retail layout, QVM call graph or context+0xDC8 tail write; no paired runtime capture.
// Note: x86 cdecl, two stack arguments (context [esp+4] moved to EBP at 0x004C62B6, path [esp+8]); BN types
// the path word int32_t while the code types it const char*. The port passes TerrainLevelData as the context
// and runs the file sequence through Terrain_LoadLevel(base.parent_path(), {}, false); the QSC task stage is
// applied later by the phase-0 caller. The clear of context+0xDC8 in the retail tail is not performed because
// the host object does not mirror that retail offset; the arg1 return value is.
void* Terrain_Load(void* context, const char* base_path)
{
    auto* const data = static_cast<TerrainLevelData*>(context);
    if (data == nullptr || base_path == nullptr)
    {
        return nullptr;
    }

    const std::filesystem::path base(base_path);
    data->Terrain_LoadLevel(base.parent_path(), {}, false);
    return data;
}

// Native: Terrain_LoadTexturesAndLightmaps; retail VA 0x004C5F00; signature int32_t __cdecl(void* context, const char* base_path).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP (20 blocks, 599 bytes), Radare2 CFG (20 nodes/30 edges), angr CFGFast (exact entry, no unresolved jumps), live retail capture 20261023T015650-level1-retail (EAX 0x0000000A).
// Behavior/CFG: HOST FILE-STAGE BRIDGE; retail returns 0 when App_ContextGetTerrainLightmapsUsed()==0 (0x004C600B) or Resource_FileExists("<path>.lmp")==0 (0x004C6096), otherwise the EAX of Resource_Unload (0x004C6144) survives to the single ret 0x004C6156.
// Status/limits: STRUCTURALLY_VERIFIED (host file-stage bridge); the .lmp is not registered in the retail-style resource table, so the observed pool-count 0xA is not reproduced; no paired runtime capture.
// Note: Resource_Unload (0x004B6380) returns the node kind for kinds 1/3, otherwise Resource_FreeNode's EAX:
// the remaining pool count, or the relocated node address when the freed slot was not last. The observed
// 0xA is therefore the post-unload pool count (no slot swap), not a success flag. The host bridge returns 0
// on the two early paths and Resource_Unload's value on the loaded path only when the port resource table
// holds the .lmp; the host file stage reads TERRAIN.LMP with ifstream and never registers it, so it returns 0.
// Retail's App_ContextGetTerrainLightmapsUsed gate is omitted because Config_SetResolution_2 (0x004063D1),
// the setter's caller, is not wired in this branch.
std::int32_t Terrain_LoadTexturesAndLightmaps(void* context, const char* base_path)
{
    auto* const data = static_cast<TerrainLevelData*>(context);
    if (data == nullptr || base_path == nullptr)
    {
        return 0;
    }

    const std::filesystem::path base(base_path);
    return data->Terrain_LoadLightmaps(base.parent_path());
}

// Terrain_TraverseContourQuadtree (0x004D14E0) is NOT defined here. The throwing placeholder this
// unit used to carry was removed when terrain_quadtree.cpp was added to the igi link: that unit
// defines `std::int32_t Terrain_TraverseContourQuadtree(void*)` with C++ linkage in namespace
// igi::retail::terrain, exactly the mangled symbol this placeholder produced (same namespace, same
// name, same parameter type, same return type), so keeping both is a link-time multiple definition.
// The native transcription is the honest owner, and its declaration is terrain_quadtree.hpp (this
// loader header no longer redeclares it). Neither form had a caller anywhere in this branch (a
// repository-wide search for the name finds only declarations, the definition, and documentation),
// so no consumer depends on the throw.

// Native: Terrain_DecodeChunkCommands; retail VA 0x00510010; signature int32_t __cdecl(void).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja binaryninja_ui_mcp 0.1.0 (143 blocks / 212 edges, 5,348 bytes), radare2 6.2.0 (identical 143 / 212, cdecl, no arguments, no calls), angr 10.0.4.post1 CFGFast (157 nodes, no unresolved jumps).
// Behavior/CFG: per-frame consumer of the 0x30-byte contour records published by 0x004D14E0 (g_b16ca4); allocates/extends the three terrain chunk pools, decodes packed 6-bit vertex offsets, resolves material pages and neighbour modifiers, emits two vertex streams plus the descriptor lists, and publishes g_b16c9c / g_b12c60 / g_b12c5c.
// Status/limits: STRUCTURALLY_VERIFIED (retail CFG recovered and stage-by-stage source review; candidate decomposes into a 7-block driver plus decode_chunk_record, so this is not a block-for-block or behavioral equivalence claim); runtime-populated tables (0x00BA2030, 0x00BA5050, 0x00BA80B0, 0x00C28FE4, 0x00C30080) and exact x87 precision remain unverified; no differential execution.
//
// Detail retained from the retail analysis: the entry value of g_b16ca4 is returned on every path
// (the frame's record count), not a command count or opcode result.  The function is a leaf: no call
// instructions and no indirect jumps; the six "callees" Binary Ninja reports at 0x00C35040 are
// rep-movs artifacts.  The task premise of a "chunk command byte stream with opcode dispatch" is not
// what this VA does.  Runtime-populated tables and the pool limits are candidate storage / unresolved;
// x87 80-bit intermediates are modelled with float/double; the D3D vertex-declaration objects at
// 0x00C30060 and the exact per-material stream consumption are not executed here.  Live retail
// observation remains retail-only: 733 calls with EAX 0x1F3, 2, 2, 1
// (docs/fidelity/evidence/retail-level4-frida.json and the level-1 capture).
std::int32_t Terrain_DecodeChunkCommands()
{
    const std::int32_t count = g_b16ca4;
    g_c31090 += count;
    if (count <= 0)
    {
        return count;
    }

    std::uint8_t* record = g_b16ce0.data();
    std::int32_t cached_neighbor_token = -1;
    std::array<std::uint8_t, 4 * 0x18> neighbors{};
    for (std::int32_t i = 0; i < count; ++i)
    {
        decode_chunk_record(record, cached_neighbor_token, neighbors.data());
        record += 0x30;
    }
    return count;
}

// Terrain_Draw (0x00512C30) is NOT defined here. The placeholder this unit used to carry took a
// `void* context` argument, which contradicts the retail ABI: the only caller (0x005116A2 inside
// Terrain_UpdateWaterWaves 0x00511500) pushes no argument, the body reads no incoming stack slot,
// and the frame is allocated by the MSVC 6 __chkstk helper at 0x00512C30. Because the parameter
// list differs, the placeholder produced a different mangled symbol from the transcription
// (`Terrain_Draw()` in terrain_draw.cpp), so it was a second, wrong-ABI entry point rather than a
// link conflict. It had no caller anywhere in this branch, so it was deleted instead of being
// kept as a second entry point. The declaration lives in terrain_draw.hpp with the retail
// signature and the transcription in terrain_draw.cpp.

// HOST HELPER: load Level terrain files and QSC tasks.
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: scripts/GameNatives/dynobj/004C6240-DynObj_Createterrainmateria.cpp; Project-IGI-Terrain/source/level/terrain.cpp.
// Behavior/CFG: Bridges retail .bit, .hmp, .cmd, .ctr, .tex, .lmp files into TerrainLevelData container.
// Status/limits: Host loader bridge; does not claim retail binary memory ownership.
bool TerrainLevelData::Terrain_LoadLevel(
    const std::filesystem::path& terrain_dir,
    const std::vector<std::shared_ptr<script::QvmObjectCall>>& object_calls,
    bool apply_tasks)
{
    loaded = false;

    // Load .bit
    bit_file_bytes = read_binary_file(terrain_dir / "TERRAIN.BIT");
    if (bit_file_bytes.empty())
    {
        bit_file_bytes = read_binary_file(terrain_dir / "terrain.bit");
    }
    if (bit_file_bytes.size() >= 0xC00U)
    {
        const auto* records = reinterpret_cast<const TerrainGridRecord*>(bit_file_bytes.data());
        const auto* body = reinterpret_cast<const std::uint8_t*>(bit_file_bytes.data() + 0xC00U);
        std::size_t offset = 0U;
        for (std::size_t i = 0U; i < 256U; ++i)
        {
            bit_records[i] = records[i];
            const auto dim = bit_records[i].dimension;
            if (dim != 0U && offset < bit_file_bytes.size() - 0xC00U)
            {
                bit_data_pointers[i] = body + offset;
                offset += (static_cast<std::size_t>(dim) * dim + 7U) / 8U;
            }
            else
            {
                bit_data_pointers[i] = nullptr;
            }
        }
    }

    // Load .hmp
    hmp_file_bytes = read_binary_file(terrain_dir / "TERRAIN.HMP");
    if (hmp_file_bytes.empty())
    {
        hmp_file_bytes = read_binary_file(terrain_dir / "terrain.hmp");
    }
    if (hmp_file_bytes.size() >= 0xC00U)
    {
        const auto* records = reinterpret_cast<const TerrainGridRecord*>(hmp_file_bytes.data());
        const auto* body = reinterpret_cast<const std::int8_t*>(hmp_file_bytes.data() + 0xC00U);
        std::size_t offset = 0U;
        for (std::size_t i = 0U; i < 256U; ++i)
        {
            hmp_records[i] = records[i];
            const auto dim = hmp_records[i].dimension;
            if (dim != 0U && offset < hmp_file_bytes.size() - 0xC00U)
            {
                hmp_data_pointers[i] = body + offset;
                offset += static_cast<std::size_t>(dim + 1U) * (dim + 1U);
            }
            else
            {
                hmp_data_pointers[i] = nullptr;
            }
        }
    }

    // Load .cmd
    cmd_file_bytes = read_binary_file(terrain_dir / "TERRAIN.CMD");
    if (cmd_file_bytes.empty())
    {
        cmd_file_bytes = read_binary_file(terrain_dir / "terrain.cmd");
    }

    // Load .ctr
    ctr_file_bytes = read_binary_file(terrain_dir / "TERRAIN.CTR");
    if (ctr_file_bytes.empty())
    {
        ctr_file_bytes = read_binary_file(terrain_dir / "terrain.ctr");
    }
    if (ctr_file_bytes.size() >= sizeof(TerrainContourRecord))
    {
        const std::size_t count = ctr_file_bytes.size() / sizeof(TerrainContourRecord);
        contour_nodes.resize(count);
        std::memcpy(contour_nodes.data(), ctr_file_bytes.data(), count * sizeof(TerrainContourRecord));
    }

    // Load .tex (LOOP texture container)
    tex_file_bytes = read_binary_file(terrain_dir / "TERRAIN.TEX");
    if (tex_file_bytes.empty())
    {
        tex_file_bytes = read_binary_file(terrain_dir / "terrain.tex");
    }
    if (tex_file_bytes.size() >= 52U)
    {
        const auto* bytes = tex_file_bytes.data();
        std::uint32_t ident = 0;
        std::uint32_t version = 0;
        std::uint32_t layer_count = 0;
        std::memcpy(&ident, bytes, 4);
        std::memcpy(&version, bytes + 4, 4);
        std::memcpy(&layer_count, bytes + 0x20, 4);

        if (ident == texture::LoopTerrainTextureFile::loop_signature && version == 9U && layer_count > 0U)
        {
            texture_layers.resize(layer_count);
            for (std::size_t i = 0U; i < layer_count; ++i)
            {
                const std::size_t header_offset = 52U + i * 32U;
                if (header_offset + 32U > tex_file_bytes.size())
                {
                    break;
                }
                std::int32_t image_offset = 0;
                std::int32_t image_mode = 0;
                std::int16_t line_width = 0;
                std::int16_t width = 0;
                std::int16_t height = 0;
                std::memcpy(&image_offset, bytes + header_offset, 4);
                std::memcpy(&image_mode, bytes + header_offset + 4, 4);
                std::memcpy(&line_width, bytes + header_offset + 8, 2);
                std::memcpy(&width, bytes + header_offset + 10, 2);
                std::memcpy(&height, bytes + header_offset + 12, 2);

                if (image_offset > 0 && width > 0 && height > 0 &&
                    static_cast<std::size_t>(image_offset) < tex_file_bytes.size())
                {
                    auto& layer = texture_layers[i];
                    layer.width = static_cast<std::uint32_t>(width);
                    layer.height = static_cast<std::uint32_t>(height);
                    layer.argb_pixels.resize(layer.width * layer.height);

                    const auto* src_pixels = reinterpret_cast<const std::uint8_t*>(bytes + image_offset);
                    const auto stride = static_cast<std::size_t>(line_width > 0 ? line_width : width * 2);

                    for (std::uint32_t y = 0U; y < layer.height; ++y)
                    {
                        const auto* row = reinterpret_cast<const std::uint16_t*>(src_pixels + y * stride);
                        for (std::uint32_t x = 0U; x < layer.width; ++x)
                        {
                            const std::uint16_t pixel = row[x];
                            const std::uint32_t r = ((pixel >> 10U) & 0x1FU) * 255U / 31U;
                            const std::uint32_t g = ((pixel >> 5U) & 0x1FU) * 255U / 31U;
                            const std::uint32_t b = (pixel & 0x1FU) * 255U / 31U;
                            layer.argb_pixels[y * layer.width + x] =
                                0xFF000000U | (r << 16U) | (g << 8U) | b;
                        }
                    }
                }
            }
        }
    }

    // The lightmap pages are loaded by Terrain_LoadTexturesAndLightmaps
    // (0x004C5F00), which Level_Start calls before Terrain_Load.

    if (apply_tasks)
    {
    // Parse QSC script calls (TextureModifier, HeightMap, TerrainLightMap)
    for (const auto& call : object_calls)
    {
        if (call->function_name == "Task_New" && call->arguments.size() > 2U)
        {
            if (const auto* type_str = std::get_if<std::string>(&call->arguments[1]))
            {
                if (*type_str == "TextureModifier" && call->arguments.size() >= 10U)
                {
                    TerrainTextureModifierTask task{};
                    for (std::size_t a = 0U; a < 3U; ++a)
                    {
                        if (const auto* real = std::get_if<float>(&call->arguments[3U + a]))
                        {
                            task.position[a] = static_cast<double>(*real);
                        }
                    }
                    if (const auto* lod = std::get_if<std::int32_t>(&call->arguments[6]))
                    {
                        task.lod_level = static_cast<std::uint32_t>(*lod);
                    }
                    if (const auto* mat = std::get_if<std::int32_t>(&call->arguments[7]))
                    {
                        task.material_idx = static_cast<std::uint32_t>(*mat);
                    }
                    if (const auto* bmp = std::get_if<std::int32_t>(&call->arguments[8]))
                    {
                        task.bitmap_id = *bmp;
                    }
                    if (const auto* sz = std::get_if<std::int32_t>(&call->arguments[9]))
                    {
                        task.bitmap_size = static_cast<std::uint32_t>(*sz);
                    }
                    if (task.bitmap_size >= 8U)
                    {
                        const int shift = get_highest_bit(task.bitmap_size);
                        task.bitmap_line_width_shift = static_cast<std::uint32_t>(shift - 3);
                        const auto cube_half_shift = 30U - task.lod_level;
                        const auto bitmap_half_shift = static_cast<std::uint32_t>(shift - 1);
                        const float ratio = static_cast<float>(task.bitmap_size - 1U) / static_cast<float>(task.bitmap_size);
                        task.local_pos_to_bitmap_pos = (1.0f / static_cast<float>(1U << (cube_half_shift - bitmap_half_shift))) * ratio;
                        if (task.bitmap_id >= 0 && task.bitmap_id < 256)
                        {
                            task.bitmap_item = bit_data_pointers[static_cast<std::size_t>(task.bitmap_id)];
                        }
                        const auto cube_half_size = ROOT_CUBE_HALF_SIZE >> task.lod_level;
                        task.cube_min_x = task.position[0] - static_cast<double>(cube_half_size);
                        task.cube_min_y = task.position[1] - static_cast<double>(cube_half_size);
                        texture_modifiers.push_back(task);
                    }
                }
                else if (*type_str == "HeightMap" && call->arguments.size() >= 10U)
                {
                    TerrainHeightMapTask task{};
                    for (std::size_t a = 0U; a < 3U; ++a)
                    {
                        if (const auto* real = std::get_if<float>(&call->arguments[3U + a]))
                        {
                            task.position[a] = static_cast<double>(*real);
                        }
                    }
                    if (const auto* lod = std::get_if<std::int32_t>(&call->arguments[6]))
                    {
                        task.lod_level = static_cast<std::uint32_t>(*lod);
                    }
                    if (const auto* sz = std::get_if<std::int32_t>(&call->arguments[7]))
                    {
                        task.height_map_size = static_cast<std::uint32_t>(*sz);
                    }
                    if (const auto* bmp = std::get_if<std::int32_t>(&call->arguments[9]))
                    {
                        task.height_map_id = *bmp;
                    }
                    if (task.height_map_size >= 8U)
                    {
                        const int shift = get_highest_bit(task.height_map_size);
                        task.height_map_line_width_shift = shift;
                        const auto cube_half_shift = 30U - task.lod_level;
                        const auto hmp_half_shift = static_cast<std::uint32_t>(shift);
                        task.local_pos_to_hmp_pos = 1.0f / static_cast<float>(1U << (cube_half_shift - hmp_half_shift));
                        if (task.height_map_id >= 0 && task.height_map_id < 256)
                        {
                            task.height_map_item = hmp_data_pointers[static_cast<std::size_t>(task.height_map_id)];
                        }
                        const auto cube_half_size = ROOT_CUBE_HALF_SIZE >> task.lod_level;
                        task.cube_min_x = task.position[0] - static_cast<double>(cube_half_size);
                        task.cube_min_y = task.position[1] - static_cast<double>(cube_half_size);
                        height_maps.push_back(task);
                    }
                }
                else if (*type_str == "TerrainLightMap" && call->arguments.size() >= 9U)
                {
                    TerrainLightMapTask task{};
                    for (std::size_t a = 0U; a < 3U; ++a)
                    {
                        if (const auto* real = std::get_if<float>(&call->arguments[3U + a]))
                        {
                            task.position[a] = static_cast<double>(*real);
                        }
                    }
                    if (const auto* lod = std::get_if<std::int32_t>(&call->arguments[6]))
                    {
                        task.lod_level = static_cast<std::uint32_t>(*lod);
                    }
                    if (const auto* sz = std::get_if<std::int32_t>(&call->arguments[7]))
                    {
                        task.light_map_size = static_cast<std::uint32_t>(*sz);
                    }
                    if (const auto* tex = std::get_if<std::int32_t>(&call->arguments[8]))
                    {
                        task.texture_index = *tex;
                    }
                    const auto cube_half_size = ROOT_CUBE_HALF_SIZE >> task.lod_level;
                    task.one_over_cube_size = 1.0f / static_cast<float>(cube_half_size * 2);
                    task.half_pixel_uv = task.light_map_size > 0U ? 0.5f / static_cast<float>(task.light_map_size) : 0.0f;
                    task.cube_min_x = task.position[0] - static_cast<double>(cube_half_size);
                    task.cube_min_y = task.position[1] - static_cast<double>(cube_half_size);
                    light_maps.push_back(task);
                }
            }
        }
    }

    // Execute TERRAIN.QVM calls if present
    const auto qvm_file_bytes = read_binary_file(terrain_dir / "TERRAIN.QVM");
    if (!qvm_file_bytes.empty())
    {
        try
        {
            const auto program = script::Qvm_LoadCompiledProgram(qvm_file_bytes);
            const auto calls = script::Qvm_ReadObjectCalls(program);
            for (const auto& call : calls)
            {
                if (call->function_name == "CreateTerrainMaterial" && call->arguments.size() >= 226U)
                {
                    std::vector<float> args;
                    args.reserve(call->arguments.size());
                    for (const auto& arg : call->arguments)
                    {
                        if (const auto* real = std::get_if<float>(&arg))
                        {
                            args.push_back(*real);
                        }
                        else if (const auto* integer = std::get_if<std::int32_t>(&arg))
                        {
                            args.push_back(static_cast<float>(*integer));
                        }
                        else
                        {
                            args.push_back(0.0f);
                        }
                    }
                    Terrain_CreateMaterial(nullptr, args.data());
                }
                else if (call->function_name == "CreateTerrainTileMap" && call->arguments.size() >= 65U)
                {
                    std::vector<float> args;
                    args.reserve(call->arguments.size());
                    for (const auto& arg : call->arguments)
                    {
                        if (const auto* real = std::get_if<float>(&arg))
                        {
                            args.push_back(*real);
                        }
                        else if (const auto* integer = std::get_if<std::int32_t>(&arg))
                        {
                            args.push_back(static_cast<float>(*integer));
                        }
                        else
                        {
                            args.push_back(0.0f);
                        }
                    }
                    Terrain_CreateTileMap(nullptr, args.data());
                }
            }
        }
        catch (const std::exception&)
        {
            // QVM parse fallback: keep default materials
        }
    }
    } // apply_tasks

    material_groups.assign(g_c28fe0.begin(), g_c28fe0.end());
    tile_maps = g_c30080;

    loaded = !cmd_file_bytes.empty() && !contour_nodes.empty() && !texture_layers.empty();
    return loaded;
}

// HOST HELPER: load TERRAIN.LMP lightmap pages for Terrain_LoadTexturesAndLightmaps.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Level_Start 0x00415C31 calls 0x004C5F00 before 0x004C6240; retail tail 0x004C613F-0x004C6156 returns Resource_Unload's EAX.
// Behavior/CFG: host file-stage bridge using the port LMP page layout; returns 0 when the .lmp is absent, otherwise the port Resource_Unload value when the .lmp is registered, otherwise 0.
// Status/limits: Host file-stage bridge, not a retail memory-layout transcription; the retail pool count cannot be reconstructed.
// Note: the retail loader walks the LMP page list; this bridge keeps the same file source and square-page
// layout used by the port renderer. The retail Resource_FileExists check is modeled by the empty-file early
// return, and the retail tail's Resource_Unload value is propagated only for a registered resource (see the
// native above for the full retail return-path evidence).
std::int32_t TerrainLevelData::Terrain_LoadLightmaps(const std::filesystem::path& terrain_dir)
{
    lmp_file_bytes = read_binary_file(terrain_dir / "TERRAIN.LMP");
    if (lmp_file_bytes.empty())
    {
        lmp_file_bytes = read_binary_file(terrain_dir / "terrain.lmp");
    }

    lightmap_layers.clear();
    if (!lmp_file_bytes.empty())
    {
        std::size_t offset = 0U;
        while (offset + 4U <= lmp_file_bytes.size())
        {
            std::uint32_t size = 0;
            std::memcpy(&size, lmp_file_bytes.data() + offset, 4);
            offset += 4U;
            if (size > 0U && size <= 2048U &&
                offset + static_cast<std::size_t>(size * size) <= lmp_file_bytes.size())
            {
                texture::ModelTexturePixels lmp_page{};
                lmp_page.width = size;
                lmp_page.height = size;
                lmp_page.argb_pixels.resize(size * size);
                const auto* intensity = reinterpret_cast<const std::uint8_t*>(lmp_file_bytes.data() + offset);
                for (std::size_t p = 0U; p < size * size; ++p)
                {
                    const std::uint32_t c = intensity[p];
                    lmp_page.argb_pixels[p] = 0xFF000000U | (c << 16U) | (c << 8U) | c;
                }
                lightmap_layers.push_back(std::move(lmp_page));
                offset += size * size;
            }
        }
    }

    // Retail 0x004C6096: Resource_FileExists("<path>.lmp") == 0 leaves EAX 0.
    if (lmp_file_bytes.empty())
    {
        return 0;
    }

    // Retail 0x004C6144: the function returns the EAX of Resource_Unload("<path>.lmp").
    // Resource_Unload (0x004B6380) is the port's transcription of the same native, so its
    // value is propagated unchanged when the host resource table holds the .lmp. The host
    // file stage reads the file directly and does not register it; that path returns 0,
    // which the source record documents as an unreproduced retail value.
    const std::string lmp_name = (terrain_dir / "terrain.lmp").string();
    if (resource::Is_ResourceLoaded(lmp_name.c_str(), nullptr) != 0)
    {
        return resource::Resource_Unload(lmp_name.c_str());
    }
    return 0;
}

// HOST HELPER: build decoded render mesh for the given camera and origin.
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: scripts/GameNatives/terrain/00510010-Terrain_RenderQuadtree.cpp; 004D14E0-Render_ShouldHaveAllocatedD.cpp.
// Behavior/CFG: Traverses contour quadtree, culls frustum, decodes chunk vertices, samples heightmaps, emits batches.
// Status/limits: Host render mesh generator; produces vertex and index batches for Direct3D 7 device rendering.
TerrainDecodedMesh TerrainLevelData::Terrain_BuildRenderMesh(
    const std::array<double, 3>& origin,
    float world_scale,
    const float camera_position[3],
    const float camera_target[3],
    float fov_x, float fov_y,
    float view_distance) const
{
    TerrainDecodedMesh mesh{};
    static_cast<void>(camera_target);
    static_cast<void>(fov_x);
    static_cast<void>(fov_y);
    if (!loaded || contour_nodes.size() < 2U || cmd_file_bytes.empty())
    {
        return mesh;
    }

    const double cam_world_x = origin[0] + static_cast<double>(camera_position[0]) / world_scale;
    const double cam_world_y = origin[1] + static_cast<double>(camera_position[1]) / world_scale;
    const double cam_world_z = origin[2] + static_cast<double>(camera_position[2]) / world_scale;

    const double max_range_units = static_cast<double>(view_distance) / world_scale;

    // Traverse the contour quadtree starting from CTR[1] (root).
    struct TraversalState
    {
        std::int16_t ctr_index = 1;
        std::uint32_t lod_level = 0;
        std::array<double, 3> pos{};
        std::uint32_t trans_flag = 0;
    };

    std::vector<TraversalState> stack;
    stack.reserve(256);
    stack.push_back({1, 0, {0.0, 0.0, 0.0}, 0});

    std::vector<TraversalState> chunks_to_render;
    chunks_to_render.reserve(1024);

    while (!stack.empty())
    {
        const auto state = stack.back();
        stack.pop_back();

        if (state.ctr_index <= 0 || static_cast<std::size_t>(state.ctr_index) >= contour_nodes.size())
        {
            continue;
        }

        const auto& node = contour_nodes[static_cast<std::size_t>(state.ctr_index)];
        const double half_size = static_cast<double>(ROOT_CUBE_HALF_SIZE >> state.lod_level);

        // Distance check from camera
        const double dx = state.pos[0] - cam_world_x;
        const double dy = state.pos[1] - cam_world_y;
        const double dz = state.pos[2] - cam_world_z;
        const double dist = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (max_range_units > 0.0 && dist > max_range_units + half_size * 1.732)
        {
            continue;
        }

        const double max_axis_aligned_dist = std::max({std::abs(dx), std::abs(dy), std::abs(dz)});
        const double delta = max_axis_aligned_dist - half_size;
        const int shift = 25 - static_cast<int>(state.lod_level);
        const int v = shift >= 0 ? (static_cast<int>(delta) >> shift) : (static_cast<int>(delta) << (-shift));

        const bool has_children = (node.children_mask != 0U) && (state.lod_level < 16U);
        // Restore the host path's single threshold. The retail producer at
        // 0x004D2540 derives its threshold from quality/FOV state; see the terrain
        // divergence audit. This host default is not proof of that producer's parity.
        const bool draw_cur_cube = ((v >= 163 && state.lod_level >= 7U) || state.lod_level >= 16U || !has_children);

        if (draw_cur_cube)
        {
            if (state.lod_level >= 7U && node.commands.file_offset > 0U &&
                node.commands.file_offset < cmd_file_bytes.size())
            {
                chunks_to_render.push_back(state);
            }
        }
        else
        {
            const auto child_half_size = half_size * 0.5;
            const auto* row = &g_548060[8U * state.trans_flag];
            for (std::uint32_t child_no = 0U; child_no < 8U; ++child_no)
            {
                const auto child_idx = static_cast<std::uint32_t>(row[child_no]);
                if ((node.children_mask & (1U << child_idx)) != 0U)
                {
                    const auto next_ctr = node.children[child_idx];
                    if (next_ctr > 0 && static_cast<std::size_t>(next_ctr) < contour_nodes.size())
                    {
                        const auto child_trans = static_cast<std::uint32_t>(static_cast<std::uint8_t>(node.cmd_transform[child_idx])) & 7U;
                        const auto next_trans = static_cast<std::uint32_t>(g_cube_trans_table[state.trans_flag * 8U + child_trans]);

                        const double sx = (child_no & 1U) ? child_half_size : -child_half_size;
                        const double sy = (child_no & 2U) ? child_half_size : -child_half_size;
                        const double sz = (child_no & 4U) ? child_half_size : -child_half_size;

                        stack.push_back({next_ctr,
                                         state.lod_level + 1U,
                                         {state.pos[0] + sx, state.pos[1] + sy, state.pos[2] + sz},
                                         next_trans});
                    }
                }
            }
        }
    }

    // Now decode chunks and build render batches
    std::map<std::uint32_t, std::vector<std::uint32_t>> batch_map;

    for (const auto& chunk : chunks_to_render)
    {
        const auto& node = contour_nodes[static_cast<std::size_t>(chunk.ctr_index)];
        const auto cmd_offset = node.commands.file_offset;
        if (cmd_offset + sizeof(TerrainCommandHeader) > cmd_file_bytes.size())
        {
            continue;
        }

        TerrainCommandHeader header{};
        std::memcpy(&header, cmd_file_bytes.data() + cmd_offset, sizeof(header));
        if (header.triangle_count <= 0 || header.base_vertex_count <= 0)
        {
            continue;
        }

        const auto total_vert_cnt = static_cast<std::size_t>(header.base_vertex_count + header.extra_vertex_count);
        const auto tri_cnt = static_cast<std::size_t>(header.triangle_count);
        const auto tri_bytes = static_cast<std::size_t>(header.triangle_byte_count);

        const auto tri_data_offset = cmd_offset + sizeof(TerrainCommandHeader);
        const auto vert_data_offset = tri_data_offset + tri_bytes;

        if (vert_data_offset + total_vert_cnt * 4U > cmd_file_bytes.size())
        {
            continue;
        }

        const auto* cmd_triangles = reinterpret_cast<const std::uint32_t*>(cmd_file_bytes.data() + tri_data_offset);
        const auto* cmd_vertices = reinterpret_cast<const std::uint32_t*>(cmd_file_bytes.data() + vert_data_offset);

        // Find active heightmap for this chunk if LOD >= 14
        const TerrainHeightMapTask* active_hmp = nullptr;
        if (chunk.lod_level >= 14U)
        {
            for (const auto& hmp : height_maps)
            {
                const auto hmp_half = ROOT_CUBE_HALF_SIZE >> hmp.lod_level;
                if (chunk.pos[0] >= hmp.cube_min_x && chunk.pos[0] <= hmp.cube_min_x + hmp_half * 2 &&
                    chunk.pos[1] >= hmp.cube_min_y && chunk.pos[1] <= hmp.cube_min_y + hmp_half * 2)
                {
                    active_hmp = &hmp;
                    break;
                }
            }
        }

        // A modifier applies only where its packed bitmap bit is set.  Test the
        // chunk centre so a mask cannot paint its entire enclosing quadtree cube.
        const TerrainTextureModifierTask* active_tm = nullptr;
        for (const auto& tm : texture_modifiers)
        {
            const auto tm_half = ROOT_CUBE_HALF_SIZE >> tm.lod_level;
            if (chunk.pos[0] >= tm.cube_min_x && chunk.pos[0] <= tm.cube_min_x + tm_half * 2 &&
                chunk.pos[1] >= tm.cube_min_y && chunk.pos[1] <= tm.cube_min_y + tm_half * 2 &&
                TextureModifier_CoversPoint(tm, chunk.pos[0], chunk.pos[1]))
            {
                active_tm = &tm;
                break;
            }
        }

        const double scale = static_cast<double>(1 << (27 - chunk.lod_level));
        const double one_over_three = 1.0 / 3.0;
        const auto base_index = static_cast<std::uint32_t>(mesh.vertices.size());

        std::uint32_t mat_idx = 0U;
        if (active_tm != nullptr)
        {
            mat_idx = active_tm->material_idx;
        }
        if (mat_idx >= material_groups.size())
        {
            mat_idx = 0U;
        }
        const auto& mat_group = material_groups[mat_idx];
        const auto lod_clamped = std::min(chunk.lod_level, 31U);
        const auto& cur_tm = mat_group.lod_materials[lod_clamped];

        const auto tile_map_idx = static_cast<std::size_t>(cur_tm.tex1_tile_map_group_idx);
        const auto& tilemap = tile_maps[tile_map_idx < 64U ? tile_map_idx : 0U];

        const int shift_tex1 = 22 - static_cast<int>(chunk.lod_level);
        const auto y_off = (((static_cast<std::int32_t>(static_cast<std::int64_t>(cur_tm.f_tex1 * chunk.pos[1])) >> shift_tex1) & 0x7FF) >> 8) & 7;
        const auto x_off = (((static_cast<std::int32_t>(static_cast<std::int64_t>(cur_tm.f_tex1 * chunk.pos[0])) >> shift_tex1) & 0x7FF) >> 8) & 7;
        const auto tex_index = static_cast<std::uint32_t>(tilemap.tex_indices[static_cast<std::size_t>(8 * y_off + x_off)]);

        const auto cube_half_size = static_cast<double>(ROOT_CUBE_HALF_SIZE >> chunk.lod_level);
        const float uv1_f = cube_half_size > 0.0 ? cur_tm.f_tex1 / static_cast<float>(cube_half_size) : 0.0f;
        const float uv1_x = (static_cast<float>(static_cast<std::uint8_t>(static_cast<int>(cur_tm.f_tex1 * chunk.pos[0]) >> shift_tex1)) + 1.0f) * (1.0f / 256.0f);
        const float uv1_y = (static_cast<float>(static_cast<std::uint8_t>(static_cast<int>(cur_tm.f_tex1 * chunk.pos[1]) >> shift_tex1)) + 1.0f) * (1.0f / 256.0f);

        for (std::size_t v = 0U; v < total_vert_cnt; ++v)
        {
            const std::uint32_t cmd_vert = cmd_vertices[v];
            const int shifted_x = (static_cast<int>(cmd_vert >> 26U) & 0x3F);
            const int shifted_y = (static_cast<int>(cmd_vert >> 20U) & 0x3F);
            const int shifted_z = (static_cast<int>(cmd_vert >> 14U) & 0x3F);

            int x = shifted_x - 24;
            int y = shifted_y - 24;
            int z = shifted_z - 24;

            if (chunk.trans_flag & 4U)
            {
                x = 24 - shifted_x;
            }
            if (chunk.trans_flag & 1U)
            {
                if (chunk.trans_flag & 2U)
                {
                    x = -x;
                }
                else
                {
                    y = 24 - shifted_y;
                }
                const int temp = x;
                x = y;
                y = temp;
            }
            else
            {
                if (chunk.trans_flag & 2U)
                {
                    x = -x;
                    y = 24 - shifted_y;
                }
            }

            double vx = static_cast<double>(x) * scale * one_over_three + chunk.pos[0];
            double vy = static_cast<double>(y) * scale * one_over_three + chunk.pos[1];
            double vz = static_cast<double>(z) * scale * one_over_three + chunk.pos[2];

            if (active_hmp != nullptr)
            {
                vz += CalcHMPDeltaZ(*active_hmp, vx, vy);
            }

            // Convert raw world coordinates to scene meter coordinates relative to origin:
            TerrainVertex vert{};
            vert.x = static_cast<float>(vx - origin[0]) * world_scale;
            vert.y = static_cast<float>(vy - origin[1]) * world_scale;
            vert.z = static_cast<float>(vz - origin[2]) * world_scale;

            const float lx = static_cast<float>(vx - chunk.pos[0]);
            const float ly = static_cast<float>(vy - chunk.pos[1]);
            vert.u = lx * uv1_f + uv1_x;
            vert.v = ly * uv1_f + uv1_y;
            vert.alpha = 1.0f;

            mesh.vertices.push_back(vert);
        }

        // Append triangle indices
        auto& batch_indices = batch_map[tex_index];
        for (std::size_t t = 0U; t < tri_cnt; ++t)
        {
            const std::uint32_t tri = cmd_triangles[t];
            std::uint32_t i0 = 0, i1 = 0, i2 = 0;
            if (chunk.trans_flag & 4U)
            {
                i0 = (tri >> 8U) & 0xFFU;
                i1 = tri & 0xFFU;
                i2 = (tri >> 16U) & 0xFFU;
            }
            else
            {
                i0 = (tri >> 16U) & 0xFFU;
                i1 = tri & 0xFFU;
                i2 = (tri >> 8U) & 0xFFU;
            }

            if (i0 < total_vert_cnt && i1 < total_vert_cnt && i2 < total_vert_cnt)
            {
                batch_indices.push_back(base_index + i0);
                batch_indices.push_back(base_index + i1);
                batch_indices.push_back(base_index + i2);
            }
        }
    }

    mesh.batches.reserve(batch_map.size());
    for (auto& [tex_idx, indices] : batch_map)
    {
        if (!indices.empty())
        {
            mesh.batches.push_back({tex_idx, std::move(indices)});
        }
    }

    return mesh;
}

// HOST HELPER: expose the ported terrain globals to differential harnesses. These mirror the
// retail storage at 0x00C28FE0 / 0x00C30080 / 0x00B97DA0 / 0x00B97DF8 / 0x00BA5060; the retail
// addresses themselves are absolute in igi.exe and are read directly by the angr side.
void Terrain_GetMaterialGroupBytes(std::uint32_t index, std::uint8_t* out)
{
    if (index >= g_c28fe0.size() || out == nullptr)
    {
        return;
    }
    std::memcpy(out, &g_c28fe0[index], sizeof(TerrainMaterialGroup));
}

void Terrain_GetTileMapBytes(std::uint32_t index, std::uint8_t* out)
{
    if (index >= g_c30080.size() || out == nullptr)
    {
        return;
    }
    std::memcpy(out, &g_c30080[index], sizeof(TerrainTileMapTable));
}

void TextureModifier_GetState(std::uint8_t* out)
{
    if (out == nullptr)
    {
        return;
    }
    std::memcpy(out, &g_b97df8, sizeof(g_b97df8));
    std::memcpy(out + 4, g_b97da0.data(), sizeof(g_b97da0));
    std::memcpy(out + 4 + sizeof(g_b97da0), &g_ba5060, sizeof(g_ba5060));
}

void TextureModifier_SetCount(std::int32_t count)
{
    g_b97df8 = count;
}

void Terrain_ResetGlobals()
{
    for (auto& group : g_c28fe0)
    {
        group = TerrainMaterialGroup{};
    }
    for (auto& table : g_c30080)
    {
        table = TerrainTileMapTable{};
    }
    g_b97da0.fill(0U);
    g_b97df8 = 0;
    g_ba5060 = 0U;
}

} // namespace igi::retail::terrain
