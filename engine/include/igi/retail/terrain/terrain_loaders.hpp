#pragma once

#include "igi/retail/terrain/terrain_native_records.hpp"
#include "igi/retail/texture/loop_terrain_texture.hpp"
#include "igi/retail/texture/model_texture_pixels.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace igi::retail::terrain
{

// Material entry for a single LOD level (32 LOD levels per material group).
// Retail layout matches table written by Terrain_CreateMaterial at 0x004C6670.
struct TerrainMaterialEntry
{
    std::uint32_t active = 0;
    float v1 = 0.0f;
    float v2 = 0.0f;
    float f_tex2 = 0.0f;
    std::int32_t tex2_tile_map_group_idx = 0;
    float f_tex1 = 0.0f;
    std::int32_t tex1_tile_map_group_idx = 0;
};

// Material group holding 32 LOD records (900 bytes total: 4 byte id + 32 * 28 bytes).
struct TerrainMaterialGroup
{
    std::uint32_t id = 0;
    std::array<TerrainMaterialEntry, 32> lod_materials{};
};

// Tile map holding 64 texture indices for terrain quadtree mapping (64 bytes).
struct TerrainTileMapTable
{
    std::array<std::uint8_t, 64> tex_indices{};
};

// Decoded terrain vertex for rendering.
struct TerrainVertex
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float normal[3]{0.0f, 0.0f, 1.0f};
    float u = 0.0f;
    float v = 0.0f;
    float alpha = 1.0f;
};

// Render batch referencing an uploaded texture index and triangle indices.
struct TerrainMeshBatch
{
    std::uint32_t texture_index = 0;
    std::vector<std::uint32_t> indices;
};

// Decoded mesh containing vertices and indexed triangle batches.
struct TerrainDecodedMesh
{
    std::vector<TerrainVertex> vertices;
    std::vector<TerrainMeshBatch> batches;
};

// Texture modifier parameters parsed from OBJECTS.QSC (Task_New "TextureModifier").
struct TerrainTextureModifierTask
{
    std::array<double, 3> position{};
    std::uint32_t lod_level = 0;
    std::uint32_t material_idx = 0;
    std::int32_t bitmap_id = 0;
    std::uint32_t bitmap_size = 0;
    std::uint32_t bitmap_line_width_shift = 0;
    float local_pos_to_bitmap_pos = 0.0f;
    double cube_min_x = 0.0;
    double cube_min_y = 0.0;
    const std::uint8_t* bitmap_item = nullptr;
};

// Heightmap parameters parsed from OBJECTS.QSC (Task_New "HeightMap").
struct TerrainHeightMapTask
{
    std::array<double, 3> position{};
    std::uint32_t lod_level = 0;
    std::uint32_t height_map_size = 0;
    std::int32_t height_map_id = 0;
    std::int32_t height_map_line_width_shift = 0;
    float local_pos_to_hmp_pos = 0.0f;
    double cube_min_x = 0.0;
    double cube_min_y = 0.0;
    const std::int8_t* height_map_item = nullptr;
};

// Terrain lightmap parameters parsed from OBJECTS.QSC (Task_New "TerrainLightMap").
struct TerrainLightMapTask
{
    std::array<double, 3> position{};
    std::uint32_t lod_level = 0;
    std::uint32_t light_map_size = 0;
    std::int32_t texture_index = 0;
    float one_over_cube_size = 0.0f;
    float half_pixel_uv = 0.0f;
    double cube_min_x = 0.0;
    double cube_min_y = 0.0;
};

// Complete loaded Level terrain container.
class TerrainLevelData
{
  public:
    std::vector<std::byte> bit_file_bytes;
    std::vector<std::byte> hmp_file_bytes;
    std::vector<std::byte> cmd_file_bytes;
    std::vector<std::byte> ctr_file_bytes;
    std::vector<std::byte> tex_file_bytes;
    std::vector<std::byte> lmp_file_bytes;

    std::array<TerrainGridRecord, 256> bit_records{};
    std::array<const std::uint8_t*, 256> bit_data_pointers{};

    std::array<TerrainGridRecord, 256> hmp_records{};
    std::array<const std::int8_t*, 256> hmp_data_pointers{};

    std::vector<TerrainContourRecord> contour_nodes;
    std::vector<TerrainMaterialGroup> material_groups;
    std::array<TerrainTileMapTable, 64> tile_maps{};

    std::vector<texture::ModelTexturePixels> texture_layers;
    std::vector<texture::ModelTexturePixels> lightmap_layers;

    std::vector<TerrainTextureModifierTask> texture_modifiers;
    std::vector<TerrainHeightMapTask> height_maps;
    std::vector<TerrainLightMapTask> light_maps;

    bool loaded = false;

    // HOST HELPER: load Level terrain files and QSC tasks.
    bool Host_LoadLevelTerrain(const std::filesystem::path& terrain_dir,
                               const std::vector<std::shared_ptr<script::QvmObjectCall>>& object_calls);

    // HOST HELPER: build decoded render mesh for the given camera and origin.
    TerrainDecodedMesh Host_BuildRenderMesh(const std::array<double, 3>& origin,
                                           float world_scale,
                                           const float camera_position[3],
                                           const float camera_target[3],
                                           float fov_x, float fov_y,
                                           float view_distance) const;
};

// Retail Native Function Declarations (transcribed from igi.exe):

// Native: Terrain_CreateMaterial; retail VA 0x004C6670; signature int32_t(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp; CFG 96.7%.
// Behavior/CFG: Reads 226 arguments, populates 32 LOD material records in 900-byte material slot at 0x00C28FE0.
// Status/limits: TRANSCRIBED; port-owned material table storage, verified against retail QVM call sequence.
std::int32_t Terrain_CreateMaterial(void* context, const float* args);

// Native: Terrain_CreateTileMap; retail VA 0x004C6760; signature int32_t(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp; CFG 88.9%.
// Behavior/CFG: Reads group index and 64 texture indices, stores 64 bytes into tilemap table at 0x00C30080.
// Status/limits: TRANSCRIBED; port-owned tilemap storage, verified against retail QVM call sequence.
std::int32_t Terrain_CreateTileMap(void* context, const float* args);

// Native: Terrain_Load; retail VA 0x004C6240; signature void*(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/dynobj/004C6240-DynObj_Createterrainmateria.cpp; CFG 80.9%.
// Behavior/CFG: Loads .bit, .hmp, .cmd, .ctr files, registers QVM natives, executes TERRAIN.QVM, relocates CTR command offsets.
// Status/limits: TRANSCRIBED; port-owned file buffers, structure verified with retail records.
void* Terrain_Load(void* context, const char* base_path);

// Native: Terrain_LoadTexturesAndLightmaps; retail VA 0x004C5F00; signature int32_t(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/dynobj/004C5F00-DynObj_ParseConfigFile_7.cpp; CFG 85.7%.
// Behavior/CFG: Loads .tex via 0x005175A0, sets 256-color palette, loads .lmp lightmap pages into g_b81b20.
// Status/limits: TRANSCRIBED; port-owned texture storage, verified against Level 1 TERRAIN.TEX.
std::int32_t Terrain_LoadTexturesAndLightmaps(void* context, const char* base_path);

// Native: Terrain_TraverseContourQuadtree; retail VA 0x004D14E0; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/render/004D14E0-Render_ShouldHaveAllocatedD.cpp; CFG 86.7%.
// Behavior/CFG: Quadtree traversal from CTR[1], frustum culling with view angles, detail exponent selection, emits chunk queue to g_b16ce0.
// Status/limits: TRANSCRIBED; port-owned queue and frustum math, verified against retail octree tables.
std::int32_t Terrain_TraverseContourQuadtree(void* context);

// Native: Terrain_DecodeChunkCommands; retail VA 0x00510010; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrain/00510010-Terrain_RenderQuadtree.cpp; CFG 66.7%.
// Behavior/CFG: Decodes 4-byte packed vertices from CMD, applies orientation transform, heightmap delta_z, material UV mapping.
// Status/limits: TRANSCRIBED; port-owned vertex buffers, structure verified with retail CMD/HMP layouts.
std::int32_t Terrain_DecodeChunkCommands(void* context);

// Native: Terrain_Draw; retail VA 0x00512C30; signature int32_t(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrain/00512C30-Terrain_Draw.cpp; CFG 77.8%.
// Behavior/CFG: Sets Direct3D 7 device render states, transforms, fog, alpha blending, texture stages, and renders terrain chunks.
// Status/limits: TRANSCRIBED; Direct3D 7 backend bridge, verified with device draw calls.
std::int32_t Terrain_Draw(void* context);

// Native: TextureModifier_Push; retail VA 0x00505E40; signature void*(void* self).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/texturemodifier/00505E40-TextureModifier_Push.cpp; CFG 100.0%.
// Behavior/CFG: Pushes material index into modifier stack, updates packed material bitmask g_ba5060.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* TextureModifier_Push(void* self);

// Native: HeightMap_Enter; retail VA 0x00506F00; signature void*(void* context).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/heightmap/00506F00-HeightMap_Enter.cpp; CFG 100.0%.
// Behavior/CFG: Pushes heightmap index into heightmap stack g_ba2058.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* HeightMap_Enter(void* context);

// Native: TerrainLightMap_Init_2; retail VA 0x004F82B0; signature void*(void* self).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrainlightmap/004F82B0-TerrainLightMap_Init_2.cpp; CFG 100.0%.
// Behavior/CFG: Pushes lightmap index into lightmap stack data_b97b20.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* TerrainLightMap_Init_2(void* self);

} // namespace igi::retail::terrain
