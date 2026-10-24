// Native: Terrain_TraverseContourQuadtree; retail VA 0x004D14E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
//   1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386.
// Tools/evidence: Binary Ninja 5.0 (function 0x004D14E0, 4181 bytes, 146 blocks, 228 edges,
//   1265 instructions); radare2 6.0.5 `afij` (size 4181, 146 blocks, 228 edges, exact match);
//   angr CFGFast (entry 0x004D14E0, 159 blocks, 245 edges, no unresolved jumps; the count differs
//   because CFGFast splits the rep-movsd/loop sites and keeps the two intentional infinite loops).
// Behavior/CFG: the header only declares data; the behaviour and CFG are in
//   engine/src/retail/terrain/terrain_quadtree.cpp (split/merge traversal, 8-octant loop, 0x800-slot
//   cache, return 0x800).
// Scope: this header declares the retail-owned globals the function reads and writes. The values are
//   runtime state, not constants of the reconstruction; each declaration records the retail VA.
// Status/limits: STRUCTURALLY_VERIFIED at source/object level (see
//   docs/fidelity/evidence/0x004D14E0-structural-comparison.json and
//   docs/fidelity/native-verification.json). The level arithmetic now uses cl = level+1 at every
//   retail shift site, and the 128-entry worklist is retail's own frame region with a proved maximum
//   live occupancy of 121. No runtime or rendered comparison exists, so higher statuses are not
//   claimed. The linkage
//   defects this unit used to carry are fixed: g_ba2058/g_b97df4 are external definitions in
//   terrain_loaders.cpp, DynObj_ParseConfigFile_6 returns int32_t, 0x00B16D10 is modelled as the
//   g_b16ce0+0x30 cursor (it is not a variable), and the 0x00B97B1C token read is expressed
//   through the bounded g_data_b97b20 array.
//   This unit is in the igi link (CMakeLists.txt). Its camera/view-matrix inputs g_bcab08
//   (0x00BCAB08) and g_bcaae0 (0x00BCAAE0) are defined in terrain_storage.cpp, the single owner of
//   that retail storage, not in terrain_draw.cpp. The throwing Terrain_TraverseContourQuadtree
//   placeholder in terrain_loaders.cpp was deleted, so this definition is the only one for the
//   symbol.
//   The 32-bit pointer fields below are only valid for the 32-bit retail ABI.
#pragma once

#include <array>
#include <cstdint>

namespace igi::retail::terrain
{
// A contour item. Only the fields touched by 0x004D14E0 are named here; every access in the
// implementation is a byte offset so that this view does not claim a complete type.
//   +0x1C uint16 category index (zero-extended to select a callback)
//   +0x48 callback argument
//   +0x58 next item
//   +0x5C uint16 item flags (bit 0 alloc, bit 1 merge, bit 7 split callback, bit 8 merge callback)
//   +0x60 merge suppression
using TerrainContourCallback = void (*)(void* item, void* argument);

// 0x00A9CAE0 / 0x00A9C4E0: per-category callback tables, uint16 category index. Both tables are
// runtime-populated and are all zero in the file image, so their contents cannot be recovered
// statically; the declarations are deliberately unsized. 0x00A9C4E0 is used on the split path,
// 0x00A9CAE0 on the merge path.
extern TerrainContourCallback g_a9c4e0[];
extern TerrainContourCallback g_a9cae0[];

// 0x00A44344: pointer to the render world; the function reads the qword pointer at +0xDFC, whose
// pointee supplies the height cut-off (or 0xE0000000 when the pointer is null).
extern void* g_a44344;
// 0x00C10380: frame counter. Bits 0..15 are the frame stamp; (frame-1) bits 0..15 select cache
// entries written on the previous frame.
extern std::uint32_t g_c10380;

// 0x00B12BE0: 0x40-byte octant permutation table. It is not present in the file image (BSS); the
// static-analysis writer is 0x004D0EF0. The implementation copies the runtime table to the stack
// exactly as retail does and does not synthesise its contents.
extern std::uint8_t g_b12be0[0x40];

// 0x00BCAB08 / 0x00BCAB10 / 0x00BCAB18: camera position (three consecutive doubles).
// 0x00BCAB20 / 0x00BCAB24: view angles in radians (floats) used to build the extent slopes.
// 0x00BCAAE0: ten-float 3x3 transform applied to the eight octree cube corners.
// Retail loads the three qwords directly at 0x004D15FE, 0x004D1604 and 0x004D161B; modelling
// them as one array keeps the single terrain_storage.cpp definition at the retail layout
// 0x00BCAB08..0x00BCAB1F. Use g_bcab08[0..2], not separate scalar symbols.
extern double g_bcab08[3];
extern float g_bcab20;
extern float g_bcab24;
extern float g_bcaae0[10];

// 0x00B12BC0 / 0x00B12BC4 / 0x00B12BC8: camera-space offsets installed by the caller before the
// traversal (caller 0x004D2540 copies them from its argument).
extern std::int32_t g_b12bc0;
extern std::int32_t g_b12bc4;
extern std::int32_t g_b12bc8;
// 0x00B16C6C: detail cut-off (minimum 0x46 when the caller computes it).
extern std::int32_t g_b16c6c;

// 0x00B16D10 is NOT a variable: the only reference is 0x004D1526 `mov dword [var_a0h], 0xb16d10`,
// which loads the address constant as the initial record cursor.  No instruction reads or writes
// memory at 0x00B16D10.  The records are written backwards from that boundary into the 0x30-byte
// slots starting at 0x00B16CE0 (`record = cursor - 0x30`), so the cursor is `g_b16ce0 + 0x30`.
// 0x00B12C60: base of the descriptor array written through a local cursor. Retail passes the
// literal address to both consumers: 0x004D1559 `mov dword [esp+0x9c], 0xb12c60` here and
// 0x00513038 `mov dword [esp+0x94], 0xb12c60` in Terrain_Draw. The dwords at 0x00B12C60 are the
// array itself, not a pointer stored there, so this is an unsized array. Defined once in
// terrain_loaders.cpp.
extern std::uint32_t g_b12c60[];
// 0x00B12C58: 8-byte allocation records {uint8 type, 3 pad bytes, item pointer}; updated globally.
extern std::uint8_t* g_b12c58;
// 0x00B12C20: number of allocation records emitted.
extern std::uint32_t g_b12c20;
// 0x00B12B80: number of hierarchy records dropped after the 0x1000 record limit.
extern std::uint32_t g_b12b80;

// 0x00B12BB8: set to zero before the split descent; a callback may set it to force the child mask
// of the current node to be ignored for the octant pass.
extern std::int32_t g_b12bb8;

// 0x00B97D24: lightmap/token stack depth, reset by 0x004C5810 and raised by
// TerrainLightMap_PushEntry (0x004F82B0), which fills the 20-dword stack at 0x00B97B20
// (g_data_b97b20, declared in terrain_loaders.hpp and defined in terrain_loaders.cpp).
// The traversal's token read at 0x004D2226 addresses 0x00B97B1C + 4*index; because
// 0x00B97B1C is exactly four bytes below 0x00B97B20, element index n aliases
// g_data_b97b20[n - 1].  That established array and bound replace the undeclared-extent
// symbol: no separate g_b97b1c storage is invented.
// 0x00B97DF4: heightmap stack depth, raised by HeightMap_Enter (0x00506F00); the token reset
// 0x004C5810 clears it together with g_b97d24.  Defined once in terrain_loaders.cpp.
extern std::int32_t g_b97d24;
extern std::int32_t g_b97df4;
// 0x00BA2058 is shifted left 16 and OR-ed into the token; 0x00BA5060 is stored beside each record.
extern std::uint32_t g_ba2058;
extern std::uint32_t g_ba5060;

// 0x00B16CA4 / 0x00B16C9C: record and descriptor counts published for the caller.
extern std::int32_t g_b16ca4;
extern std::int32_t g_b16c9c;
// 0x00B16CE0: the 0x30-byte contour record array the traversal writes.  0x1000 records is the
// candidate's bounded storage; the retail extent is not established here.  Defined once in
// terrain_loaders.cpp and consumed there by Terrain_DecodeChunkCommands.
extern std::array<std::uint8_t, 0x30 * 0x1000> g_b16ce0;

// 0x00A4EF38: five 0x40-byte scratch copies of the input LOD nodes, saved before the node's child
// pointer block and visibility byte are reversed, and restored on exit.
extern std::uint8_t g_a4ef38[0x140];
// 0x00A53740: five 0x40-byte traversal roots; child[?] at +0x0C is pointed at the input LOD node.
extern std::uint8_t g_a53740[0x140];

// 0x00B04340: 0x800-entry, 0x1C-byte previous-frame hash cache. The stamp is compared against the
// 16-bit frame counter; 0xFFFFFFFF means "unused". The array is zero in the file image (BSS), and
// this function clears reclaimed entries to 0xFFFFFFFF; no initialiser is visible in this function.
struct TerrainContourHashEntry
{
    std::int32_t x;              // +0x00
    std::int32_t y;              // +0x04
    std::int32_t z;              // +0x08
    std::int32_t detail;         // +0x0C
    std::uint32_t stamp;         // +0x10
    std::uint32_t allocation_type; // +0x14 (3 for a live hierarchy record)
    std::uint32_t data;          // +0x18 (allocator handle or record pointer)
};
static_assert(sizeof(TerrainContourHashEntry) == 0x1C, "retail hash entry is 0x1C bytes");
extern TerrainContourHashEntry g_b04340[0x800];

// Three identical free-list pools. Each pool stores a handle table (0x..74), a live count (0x..68),
// a handle map (0x..64), a base index (0x..60) and a stride (0x..70).
//   pool A: handles 0x00B12B74, count 0x00B12B68, map 0x00B12B64, base 0x00B12B60, stride 0x00B12B70
//   pool B: handles 0x00B12C54, count 0x00B12C48, map 0x00B12C44, base 0x00B12C40, stride 0x00B12C50
//   pool C: handles 0x00B16CD4, count 0x00B16CC8, map 0x00B16CC4, base 0x00B16CC0, stride 0x00B16CD0
extern std::int32_t* g_b12b74;
extern std::int32_t g_b12b68;
extern std::int32_t* g_b12b64;
extern std::int32_t g_b12b60;
extern std::int32_t g_b12b70;
extern std::int32_t* g_b12c54;
extern std::int32_t g_b12c48;
extern std::int32_t* g_b12c44;
extern std::int32_t g_b12c40;
extern std::int32_t g_b12c50;
extern std::int32_t* g_b16cd4;
extern std::int32_t g_b16cc8;
extern std::int32_t* g_b16cc4;
extern std::int32_t g_b16cc0;
extern std::int32_t g_b16cd0;

// 0x004C5810: resets g_b97df4, g_b97d24, g_b97df8, g_ba2058 and g_ba5060 and returns 0. Its body
// was decompiled (twelve instructions) and is reconstructed in terrain_scene_state.cpp.
// Retail returns 0 in EAX, so the declaration below matches the definition's int32_t result even
// though every retail caller discards it.
extern "C" std::int32_t DynObj_ParseConfigFile_6();

// 0x004AF7B0: Debug_Error(char* format, ...). Reaches a fatal dialog through sprintf into 0xA84640;
// the reconstruction only needs the three literal messages used below.
extern "C" int Debug_Error(const char* format, ...);

// Function 0x004D14E0: Terrain_TraverseContourQuadtree, int32_t __cdecl(void*).
// Arguments: one caller-supplied terrain context. The function reads context+0x0C (the tile-map
// base used as both the root pointer and the child record base), context+0x10..0x20 (five LOD node
// pointers selected through the byte table at 0x00548444) and writes no field of the context.
// Return value: always 0x800, the number of hash slots scanned by the eviction pass.
std::int32_t Terrain_TraverseContourQuadtree(void* context);
} // namespace igi::retail::terrain
