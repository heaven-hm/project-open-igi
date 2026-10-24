// Native: unnamed in retail; Binary Ninja's `DynObj_ParseConfigFile_6` name contradicts the body.; retail VA 0x004C5810.
//   previous note: Native: unnamed in retail; Binary Ninja's `DynObj_ParseConfigFile_6` name contradicts the body.
// Retail VA 0x004C5810; x86 cdecl `int32_t(void)`.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01,
//   1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386.
// Tools/evidence (single decompiler: Binary Ninja, plus Radare2 CFG and angr CFGFast):
//   * Binary Ninja: function 0x004C5810, 28 bytes (0x004C5810-0x004C582B), 1 basic block,
//     0 edges, 7 instructions, cdecl, definition `int32_t()`. Exact bytes:
//     33c0a3f47db900a3247db900a3f87db900a35820ba00a36050ba00c3
//     (SHA-256 4cce5d36a0bae1030c3f8acdf998d5ad5a68c3c5f2ca4d284a2d480b97300c78).
//   * Radare2 6.2.0 `afij`/`pdf`: size 28, 1 block, 0 edges, 7 instructions, calltype cdecl,
//     noreturn=false, no stack frame.
//   * angr 10.0.4.post1 CFGFast: exact entry recovered, 1 block, 0 edges, no unresolved jumps,
//     returning=true.
// Behavior: `xor eax, eax` then five dword stores of that zero, in this order:
//   0x00B97DF4, 0x00B97D24, 0x00B97DF8, 0x00BA2058, 0x00BA5060; then `retn` with EAX=0.
//   No arguments, no stack frame, no calls, no memory reads, no conditional branches. The function
//   therefore only clears the five globals and returns 0; it parses no configuration file despite
//   the Binary Ninja symbol name.
// Globals (all dword, zero in the file image, and all part of the terrain scene/modifier state):
//   * 0x00B97DF4 = g_b97df4: heightmap stack depth. HeightMap_Enter (0x00506F00) increments it and
//     fills the parallel 64-dword stack at 0x00B97D40 (candidate g_b97d40).
//   * 0x00B97D24 = g_b97d24: terrain-lightmap/token stack depth. TerrainLightMap_PushEntry
//     (0x004F82B0) increments it and fills the 20-dword stack at 0x00B97B20 (g_data_b97b20).
//   * 0x00B97DF8 = g_b97df8: texture-modifier stack depth, capped at 20 by TextureModifier_Push
//     (0x00505E40), which fills the 20-dword stack at 0x00B97DA0 (candidate g_b97da0).
//   * 0x00BA2058 = g_ba2058: current heightmap index published by HeightMap_Enter.
//   * 0x00BA5060 = g_ba5060: packed texture-modifier/material bitmask updated by
//     TextureModifier_Push (`mask = (mask << 8) | (value + 1)`).
// Callers (11): Terrain_TraverseContourQuadtree 0x004D14E0 at 0x004D1579 (first statement of the
//   traversal, return value ignored), World_TraceBatch 0x004CDB20 at 0x004CDE3E,
//   DynObj_InsertHashItem_7 0x004C5B20 at 0x004C5C7C, DynObj_RemoveSymbol_9 0x004C9750 at
//   0x004C975C, DynObj_RegisterSymbol_10 0x004C9A30 at 0x004C9B4D, DynObj_LookupHashItem_10
//   0x004CABF0 at 0x004CABFC, DynObj_ParseConfigFile_10 0x004CB810 at 0x004CB820 and 0x004CB9D0,
//   DynObj_UpdateDynTree_10 0x004CB9F0 at 0x004CB9FC, DynObj_RemoveSymbol_10 0x004CBE40 at
//   0x004CBE4C and DynObj_FindSymbol_11 0x004CC2A0 at 0x004CC5BB. Callees: none.
// Placed here because its first consumer is the terrain contour traversal and the state it clears
//   is the terrain heightmap/lightmap/texture-modifier stack state; it also serves the dyn-object
//   and world-trace callers, so it is shared scene state rather than a terrain-only helper.
// Interface note: terrain_quadtree.hpp (owned by another change) declares this symbol with a
//   `void` return. Retail returns 0 in EAX, and this definition preserves that. The i386 cdecl
//   call/return ABI is identical regardless of the caller's declared return type, and every retail
//   caller ignores the result; the declaration/definition return-type mismatch is recorded here
//   because that header is outside this task's files.
// Status/limits: ANALYZED, STRUCTURALLY_VERIFIED and BUILD_VERIFIED (MinGW x86); no runtime
//   evidence.
// Linkage status (blocks a full link today; reported, not worked around):
//   * g_b97d24, g_b97df8 and g_ba5060 are external definitions in terrain_loaders.cpp.
//   * g_b97df4 and g_ba2058 are defined inside that file's anonymous namespace (terrain_loaders.cpp
//     lines 158-159, after `namespace {` at line 88), so they have internal linkage. This
//     translation unit must write the SAME objects the heightmap code uses; it therefore declares
//     them `extern` and does not duplicate the storage. A private copy would silently split the
//     heightmap stack depth (0x00B97DF4) and current index (0x00BA2058) into two unrelated sets of
//     variables, which would be worse than an unresolved symbol.
//   * The owner of terrain_loaders.cpp must move those two definitions out of the anonymous
//     namespace (and expose declarations) before this file can link. This file does not own that
//     translation unit, so it does not change it. Until then the object compiles to COFF i386 but
//     the link has two more undefined externals.

#include <cstdint>

namespace igi::retail::terrain
{
// Same objects as retail 0x00B97DF4, 0x00B97D24, 0x00B97DF8, 0x00BA2058 and 0x00BA5060; the
// widths match the candidate definitions in terrain_loaders.cpp.
extern std::int32_t g_b97df4;
extern std::int32_t g_b97d24;
extern std::int32_t g_b97df8;
extern std::uint32_t g_ba2058;
extern std::uint32_t g_ba5060;
} // namespace igi::retail::terrain

extern "C" int DynObj_ParseConfigFile_6()
{
    // Retail stores EAX (zero) into each global in this exact instruction order.
    igi::retail::terrain::g_b97df4 = 0;
    igi::retail::terrain::g_b97d24 = 0;
    igi::retail::terrain::g_b97df8 = 0;
    igi::retail::terrain::g_ba2058 = 0U;
    igi::retail::terrain::g_ba5060 = 0U;
    return 0;
}
