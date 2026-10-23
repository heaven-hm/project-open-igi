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
// Shared native globals at retail 0x00B97B20 and 0x00B97D24.
// The current port's 20-element storage is a documented subset, not a retail extent claim.
extern std::array<std::uint32_t, 20> g_data_b97b20;
extern std::int32_t g_b97d24;
// TextureModifier_Push/Pop retail globals 0x00B97DA0, 0x00B97DF8, 0x00BA5060.
extern std::array<std::uint32_t, 20> g_b97da0;
extern std::int32_t g_b97df8;
extern std::uint32_t g_ba5060;

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
static_assert(sizeof(TerrainMaterialGroup) == 900, "TerrainMaterialGroup must be exactly 900 bytes");

// Tile map holding 64 texture indices for terrain quadtree mapping (64 bytes).
struct TerrainTileMapTable
{
    std::array<std::uint8_t, 64> tex_indices{};
};
static_assert(sizeof(TerrainTileMapTable) == 64, "TerrainTileMapTable must be exactly 64 bytes");

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

// HOST HELPER: return whether a world-space point is enabled by the modifier's
// packed one-bit coverage mask.  The mask format and coordinate parameters are
// populated from TextureModifier_LoadParameters (retail VA 0x005062A0).
[[nodiscard]] bool TextureModifier_CoversPoint(const TerrainTextureModifierTask& modifier,
                                                    double world_x, double world_y);

// HOST HELPER: expose the ported terrain globals to the differential harnesses. These mirror
// the retail storage at 0x00C28FE0 (material groups), 0x00C30080 (tile-map tables) and
// 0x00B97DA0/0x00B97DF8/0x00BA5060 (texture-modifier stack); the angr side reads the absolute
// retail addresses directly from the igi.exe image state.
void Terrain_GetMaterialGroupBytes(std::uint32_t index, std::uint8_t* out);
void Terrain_GetTileMapBytes(std::uint32_t index, std::uint8_t* out);
void TextureModifier_GetState(std::uint8_t* out);
void TextureModifier_SetCount(std::int32_t count);
void Terrain_ResetGlobals();

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
    // apply_tasks=false performs the file stage only, which the phase-0
    // Level_Start order uses before objects.qsc; true applies the
    // TextureModifier/HeightMap/TerrainLightMap tasks after the script load.
    bool Terrain_LoadLevel(const std::filesystem::path& terrain_dir,
                               const std::vector<std::shared_ptr<script::QvmObjectCall>>& object_calls,
                               bool apply_tasks = true);

    // HOST HELPER: load the TERRAIN.LMP lightmap pages.
    // Retail calls Terrain_LoadTexturesAndLightmaps (0x004C5F00) before
    // Terrain_Load (0x004C6240) at 0x00415C31/0x00415C44.
    // Returns the retail-shaped result of the lightmap stage: 0 when the .lmp
    // is absent, otherwise Resource_Unload's EAX when the port resource table
    // holds the .lmp, or 0 when the host file stage did not register it.
    std::int32_t Terrain_LoadLightmaps(const std::filesystem::path& terrain_dir);

    // HOST HELPER: build decoded render mesh for the given camera and origin.
    TerrainDecodedMesh Terrain_BuildRenderMesh(const std::array<double, 3>& origin,
                                           float world_scale,
                                           const float camera_position[3],
                                           const float camera_target[3],
                                           float fov_x, float fov_y,
                                           float view_distance) const;
};

// Retail Native Function Declarations (transcribed from igi.exe):

// Native: Terrain_CreateMaterial; retail VA 0x004C6670; signature void*(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp; CFG 96.7%.
// Behavior/CFG: Reads material index, group ID, 32 LOD records (active, v1, v2, f_tex2*0.5, tex2, f_tex1*0.5, tex1), clears context, returns context.
// Status/limits: TRANSCRIBED; copied directly from scripts/GameNatives/misc/004C6670-CreateTerrainMaterial.cpp.
void* Terrain_CreateMaterial(void* context, const float* args);

// Native: Terrain_CreateTileMap; retail VA 0x004C6760; signature void*(void* context, const float* args).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp; CFG 88.9%.
// Behavior/CFG: Reads group index and 64 texture indices, stores 64 bytes into tilemap table at 0x00C30080, clears context, returns context.
// Status/limits: TRANSCRIBED; copied directly from scripts/GameNatives/create/004C6760-Create_TerrainTileMap.cpp.
void* Terrain_CreateTileMap(void* context, const float* args);

// Native: Terrain_Load; retail VA 0x004C6240; signature void* __cdecl(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 CFG and angr CFGFast on retail igi.exe (2026-10-23); live retail
// capture build/runtime-traces/20261023T015650-level1-retail returned 0x038910B4 = its context argument.
// Behavior/CFG: Loads .bit, .hmp, .cmd, .ctr files, registers QVM natives, executes TERRAIN.QVM, relocates CTR
// command offsets; single exit 0x004C666C returns DynObj_UpdateDynTree_11(context) (0x004CE3D0), i.e. arg1.
// Status/limits: BUILD_VERIFIED (host-backed bridge); file-stage equivalent only, not a byte-identical
// transcription. Retail allocation layout and the full QSC call graph remain unreproduced.
void* Terrain_Load(void* context, const char* base_path);

// Native: Terrain_LoadTexturesAndLightmaps; retail VA 0x004C5F00; signature int32_t __cdecl(void* context, const char* base_path).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 CFG and angr CFGFast on retail igi.exe (2026-10-23); live retail
// capture build/runtime-traces/20261023T015650-level1-retail (EAX 0x0000000A).
// Behavior/CFG: Loads .tex via 0x005175A0, sets 256-color palette, loads .lmp lightmap pages. The loaded path
// returns Resource_Unload's EAX at 0x004C6144 (remaining resource-pool count or relocated node address).
// Status/limits: BUILD_VERIFIED (host file-stage bridge); the .lmp is read with ifstream and not registered in
// the retail-style resource table, so the retail pool-count value (0xA) is not reproduced.
std::int32_t Terrain_LoadTexturesAndLightmaps(void* context, const char* base_path);

// Terrain_TraverseContourQuadtree (0x004D14E0) is declared in terrain_quadtree.hpp, where the
// transcription in terrain_quadtree.cpp owns it. It is intentionally not redeclared here: the
// throwing placeholder that used to live in terrain_loaders.cpp was deleted (it was the same
// symbol as the transcription and had no caller).

// Native: Terrain_DecodeChunkCommands; retail VA 0x00510010; signature int32_t __cdecl(void).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 5.0 (143 blocks / 212 edges, 1,381 instructions), radare2 6.2.0
//   (identical 143 / 212), angr CFGFast (no unresolved jumps); live retail EAX observations cited in
//   the implementation comment.  The function takes no arguments and has no callees.
// Behavior/CFG: Per-frame consumer of the 0x30-byte contour records counted by g_b16ca4 (0x00B16CA4).
//   It builds terrain chunk buffers in the three retail pools, decodes each node's packed 6-bit
//   vertex offsets into world positions, resolves material pages through the texture-modifier
//   bitmaps, emits two per-material vertex streams plus a trailing descriptor list, and publishes
//   descriptors through g_b16c9c / g_b12c60 / g_b12c5c.  It returns the record count, not a decoded
//   command count.
// Status/limits: TRANSCRIBED only.  No paired candidate capture and no runtime test; runtime tables,
//   pool limits and x87 precision remain unresolved as listed in native-verification.json.
std::int32_t Terrain_DecodeChunkCommands();

// Terrain_Draw (0x00512C30) is declared in terrain_draw.hpp as `std::int32_t Terrain_Draw()` - the
// retail ABI, which takes no argument. The `Terrain_Draw(void*)` placeholder in
// terrain_loaders.cpp was a second, wrong-ABI entry point and had no caller, so it was deleted
// rather than kept alongside the transcription in terrain_draw.cpp.

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

// Native: TerrainLightMap_PushEntry; retail VA 0x004F82B0; signature void*(void* self).
// Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Ghidra and Binary Ninja on scripts/GameNatives/terrainlightmap/004F82B0-TerrainLightMap_PushEntry.cpp; CFG 100.0%.
// Behavior/CFG: Pushes lightmap index into lightmap stack data_b97b20.
// Status/limits: TRANSCRIBED; exact stack operations matching retail binary.
void* TerrainLightMap_PushEntry(void* self);

} // namespace igi::retail::terrain
