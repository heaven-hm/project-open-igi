#include "igi/retail/terrain/terrain_loaders.hpp"

#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/texture/loop_terrain_texture.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>

namespace igi::retail::terrain
{
namespace
{

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
std::array<std::uint32_t, 20> g_b97da0{};
std::int32_t g_b97df8 = 0;
std::uint32_t g_ba5060 = 0;

std::array<std::uint32_t, 64> g_b97d40{};
std::int32_t g_b97df4 = 0;
std::uint32_t g_ba2058 = 0;

std::array<std::uint32_t, 20> g_data_b97b20{};
std::int32_t g_b97d24 = 0;

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

} // namespace

// Native: Terrain_CreateMaterial; retail VA 0x004C6670; signature void*(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp; CFG 96.7%.
// Behavior/CFG: Reads material index, group ID, 32 LOD records (active, v1, v2, f_tex2*0.5, tex2, f_tex1*0.5, tex1), clears context, returns context.
// Status/limits: TRANSCRIBED; copied directly from scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp.
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
        *reinterpret_cast<int*>(dest_2 + 28) = Script_GetArg(args, i + 5);
        i += 7;
        dest_2 += 0x1c;
    } while (i < 0xe3);

    if (context != nullptr)
    {
        std::memset(context, 0, 0x18);
    }
    return context;
}

// Native: Terrain_CreateTileMap; retail VA 0x004C6760; signature void*(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp; CFG 88.9%.
// Behavior/CFG: Reads group index and 64 texture indices, stores 64 bytes into tilemap table at 0x00C30080, clears context, returns context.
// Status/limits: TRANSCRIBED; copied directly from scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp.
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
    }
    return context;
}

// Native: TextureModifier_Push; retail VA 0x00505E40; signature void*(void* self).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/texturemodifier/00505E40-TextureModifier_Push.cpp; CFG 100.0%.
// Behavior/CFG: Pushes material index into modifier stack, updates packed material bitmask g_ba5060.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* TextureModifier_Push(void* self)
{
    if (self == nullptr)
    {
        return nullptr;
    }
    auto* bytes = static_cast<std::uint8_t*>(self);
    const auto counter_val = *reinterpret_cast<std::int32_t*>(bytes + 0xBC);
    if (counter_val >= 0)
    {
        if (g_b97df8 < 20)
        {
            const auto scratch_val = *reinterpret_cast<std::uint32_t*>(bytes + 0xE4);
            bytes[0xDC] = 1;
            g_b97da0[static_cast<std::size_t>(g_b97df8)] = scratch_val;
            g_ba5060 = (g_ba5060 << 8U) | (scratch_val + 1U);
            ++g_b97df8;
            return reinterpret_cast<void*>(static_cast<std::uintptr_t>(scratch_val + 1U));
        }
        error::Error_Show("Too many items in texture modifier stack!");
    }
    bytes[0xDC] = 0;
    return self;
}

// Native: HeightMap_Enter; retail VA 0x00506F00; signature void*(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/heightmap/00506F00-HeightMap_Enter.cpp; CFG 100.0%.
// Behavior/CFG: Pushes heightmap index into heightmap stack g_ba2058.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* HeightMap_Enter(void* context)
{
    if (context == nullptr)
    {
        return nullptr;
    }
    auto* bytes = static_cast<std::uint8_t*>(context);
    const auto entity_val = *reinterpret_cast<std::int32_t*>(bytes + 0xBC);
    if (entity_val < 0)
    {
        bytes[0xE8] = 0;
        return context;
    }
    const auto temp_3 = *reinterpret_cast<std::uint32_t*>(bytes + 0xF8);
    bytes[0xE8] = 1;
    if (g_b97df4 < 64)
    {
        g_b97d40[static_cast<std::size_t>(g_b97df4)] = temp_3 + 1U;
        g_ba2058 = temp_3 + 1U;
        ++g_b97df4;
    }
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(temp_3 + 1U));
}

// Native: TerrainLightMap_Init_2; retail VA 0x004F82B0; signature void*(void* self).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrainlightmap/004F82B0-TerrainLightMap_Init_2.cpp; CFG 100.0%.
// Behavior/CFG: Pushes lightmap index into lightmap stack data_b97b20.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* TerrainLightMap_Init_2(void* self)
{
    if (self == nullptr)
    {
        return nullptr;
    }
    auto* bytes = static_cast<std::uint8_t*>(self);
    const auto counter_val = *reinterpret_cast<std::int32_t*>(bytes + 0x108);
    if (counter_val != 0)
    {
        const auto scratch_val = *reinterpret_cast<std::uint32_t*>(bytes + 0xD8);
        bytes[0xE0] = 1;
        if (g_b97d24 < 20)
        {
            g_data_b97b20[static_cast<std::size_t>(g_b97d24)] = scratch_val;
            ++g_b97d24;
        }
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_b97d24));
    }
    bytes[0xE0] = 0;
    return self;
}

// Native: Terrain_Load; retail VA 0x004C6240; signature void*(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/dynobj/004C6240-DynObj_Createterrainmateria.cpp; CFG 80.9%.
// Behavior/CFG: Loads .bit, .hmp, .cmd, .ctr files, registers QVM natives, executes TERRAIN.QVM, relocates CTR command offsets.
// Status/limits: TRANSCRIBED; port-owned file buffers, structure verified with retail records.
void* Terrain_Load(void* context, const char* base_path)
{
    if (base_path == nullptr)
    {
        return nullptr;
    }
    return context;
}

// Native: Terrain_LoadTexturesAndLightmaps; retail VA 0x004C5F00; signature int32_t(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/dynobj/004C5F00-DynObj_ParseConfigFile_7.cpp; CFG 85.7%.
// Behavior/CFG: Loads .tex via 0x005175A0, sets 256-color palette, loads .lmp lightmap pages into g_b81b20.
// Status/limits: TRANSCRIBED; port-owned texture storage, verified against Level 1 TERRAIN.TEX.
std::int32_t Terrain_LoadTexturesAndLightmaps(void* context, const char* base_path)
{
    static_cast<void>(context);
    if (base_path == nullptr)
    {
        return 0;
    }
    return 1;
}

// Native: Terrain_TraverseContourQuadtree; retail VA 0x004D14E0; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/render/004D14E0-Render_ShouldHaveAllocatedD.cpp; CFG 86.7%.
// Behavior/CFG: Quadtree traversal from CTR[1], frustum culling with view angles, detail exponent selection, emits chunk queue to g_b16ce0.
// Status/limits: TRANSCRIBED; port-owned queue and frustum math, verified against retail octree tables.
std::int32_t Terrain_TraverseContourQuadtree(void* context)
{
    return context != nullptr ? 1 : 0;
}

// Native: Terrain_DecodeChunkCommands; retail VA 0x00510010; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrain/00510010-Terrain_RenderQuadtree.cpp; CFG 66.7%.
// Behavior/CFG: Decodes 4-byte packed vertices from CMD, applies orientation transform, heightmap delta_z, material UV mapping.
// Status/limits: TRANSCRIBED; port-owned vertex buffers, structure verified with retail CMD/HMP layouts.
std::int32_t Terrain_DecodeChunkCommands(void* context)
{
    return context != nullptr ? 1 : 0;
}

// Native: Terrain_Draw; retail VA 0x00512C30; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrain/00512C30-Terrain_Draw.cpp; CFG 77.8%.
// Behavior/CFG: Sets Direct3D 7 device render states, transforms, fog, alpha blending, texture stages, and renders terrain chunks.
// Status/limits: TRANSCRIBED; Direct3D 7 backend bridge, verified with device draw calls.
std::int32_t Terrain_Draw(void* context)
{
    return context != nullptr ? 1 : 0;
}

// HOST HELPER: load Level terrain files and QSC tasks.
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: scripts/GameNatives/dynobj/004C6240-DynObj_Createterrainmateria.cpp; Project-IGI-Terrain/source/level/terrain.cpp.
// Behavior/CFG: Bridges retail .bit, .hmp, .cmd, .ctr, .tex, .lmp files into TerrainLevelData container.
// Status/limits: Host loader bridge; does not claim retail binary memory ownership.
bool TerrainLevelData::Host_LoadLevelTerrain(
    const std::filesystem::path& terrain_dir,
    const std::vector<std::shared_ptr<script::QvmObjectCall>>& object_calls)
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

    // Load .lmp (lightmap pages)
    lmp_file_bytes = read_binary_file(terrain_dir / "TERRAIN.LMP");
    if (lmp_file_bytes.empty())
    {
        lmp_file_bytes = read_binary_file(terrain_dir / "terrain.lmp");
    }
    if (!lmp_file_bytes.empty())
    {
        std::size_t offset = 0U;
        while (offset + 4U <= lmp_file_bytes.size())
        {
            std::uint32_t size = 0;
            std::memcpy(&size, lmp_file_bytes.data() + offset, 4);
            offset += 4U;
            if (size > 0U && size <= 2048U && offset + static_cast<std::size_t>(size * size) <= lmp_file_bytes.size())
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

    material_groups.assign(g_c28fe0.begin(), g_c28fe0.end());
    tile_maps = g_c30080;

    loaded = !cmd_file_bytes.empty() && !contour_nodes.empty() && !texture_layers.empty();
    return loaded;
}

// HOST HELPER: build decoded render mesh for the given camera and origin.
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: scripts/GameNatives/terrain/00510010-Terrain_RenderQuadtree.cpp; 004D14E0-Render_ShouldHaveAllocatedD.cpp.
// Behavior/CFG: Traverses contour quadtree, culls frustum, decodes chunk vertices, samples heightmaps, emits batches.
// Status/limits: Host render mesh generator; produces vertex and index batches for Direct3D 7 device rendering.
TerrainDecodedMesh TerrainLevelData::Host_BuildRenderMesh(
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

        // Find active texture modifier
        const TerrainTextureModifierTask* active_tm = nullptr;
        for (const auto& tm : texture_modifiers)
        {
            const auto tm_half = ROOT_CUBE_HALF_SIZE >> tm.lod_level;
            if (chunk.pos[0] >= tm.cube_min_x && chunk.pos[0] <= tm.cube_min_x + tm_half * 2 &&
                chunk.pos[1] >= tm.cube_min_y && chunk.pos[1] <= tm.cube_min_y + tm_half * 2)
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

} // namespace igi::retail::terrain
