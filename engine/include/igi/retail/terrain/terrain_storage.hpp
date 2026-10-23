// Native storage unit: terrain state shared by the contour traversal and its consumers.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
//   1,384,448 bytes, PE i386, image base 0x00400000, entry 0x004A50F5.
// Tools/evidence: pefile 2024.8.26 (section table, raw/virtual extents), Binary Ninja MCP 0.1.0
//   (`bn_function_decompile`, `bn_function_disassembly`, `bn_memory_read`, `bn_data_at`,
//   `bn_data_xrefs_to`), radare2 6.2.0 (`aaa`, `af`, `pdf`, `axt`, `/x`, `px`), angr 10.0.4.post1
//   CFGFast (exact entries for the transcribed initialiser and its descriptor constructor).
//
// Scope: this header declares the retail storage defined in terrain_storage.cpp. The first set is
//   the storage half of the symbols left unresolved when terrain_draw.cpp,
//   terrain_out_of_terrain_renderch.cpp, terrain_quadtree.cpp and terrain_unknown_blendmode.cpp
//   were added to the igi target. Each declaration keeps the exact name, type and size used by the
//   native headers; the numeric suffix of every name is the retail VA.
//
//   A second link attempt (the three excluded terrain units without terrain_draw.cpp) additionally
//   needed nine symbols that are also defined in terrain_storage.cpp, but that already have an
//   owning declaration header, so they are not declared again here:
//     0x005CA114, 0x00A7E068  - terrain_unknown_blendmode.hpp
//     0x00BCAB08, 0x00BCAAE0  - terrain_quadtree.hpp
//     0x00A7F878, 0x00A7F87C, 0x00A7F880, 0x00A7F888, 0x00A7F88C - terrain_out_of_terrain_renderch.hpp
//   terrain_draw.cpp used to carry a second copy of those nine; those definitions were removed so
//   this unit is the only owner and terrain_draw.cpp now links with it.
//
//   Two globals read by the transcribed 0x00516640 (0x005CA110 the Direct3D 7 interface pointer and
//   0x00BCAD74 the renderer mode dword) are defined in terrain_storage.cpp but deliberately not
//   declared here: the initialiser is their only engine reader, and the interface type it needs is
//   local to that definition. Their retail VAs and access widths are documented there.
//
// Image layout used to establish the initial contents (PE section table):
//   .data  0x00536000 .. 0x00C32558 virtual (0x6FC558 bytes)
//          file-backed only through 0x0054FFFF (PointerToRawData 0x00136000, SizeOfRawData
//          0x0001A000). Every address below except 0x005470B4 lies beyond the raw extent, so the
//          PE loader supplies zero bytes (BSS). The single non-zero image value in this file is
//          0x005470B4 == 1, read from file offset 0x001470B4 as 01 00 00 00.
//
// The three earlier unresolved names, as they stand on this branch (see terrain_storage.cpp for
// the instruction evidence and the resolved ownership):
//   0x00B16D10  used only as an immediate record-cursor start value; no instruction reads or
//               writes memory there, so it is not a retail variable and no symbol exists. The
//               cursor is modelled in terrain_quadtree.cpp as g_b16ce0.data() + 0x30.
//   0x00B97B1C  has no separate storage: the reader indexes one dword below the established
//               20-dword token stack 0x00B97B20 (g_data_b97b20), so terrain_quadtree.cpp performs
//               the bounded alias g_data_b97b20[index - 1]. The retail stack extent is still
//               unresolved, but it is owned and documented by terrain_loaders.cpp/.hpp.
//   0x00BA2058  defined once at terrain_loaders.cpp:32 outside that file's anonymous namespace, so
//               it has external linkage and satisfies this branch's consumers.
//   Debug_Error (0x004AF7B0) and DynObj_ParseConfigFile_6 (0x004C5810) are functions, not storage,
//   and are transcribed in retail_error.cpp and terrain_scene_state.cpp respectively.
#pragma once

#include <cstdint>

#include "igi/retail/terrain/terrain_quadtree.hpp"

namespace igi::retail::terrain
{
// 0x00A44344: 4-byte pointer to the render world, written at 0x00415C00 and read at 0x004D14EC.
extern void* g_a44344;
// 0x00A4EF38 / 0x00A53740: five 0x40-byte node copies; 0x140 bytes each.
extern std::uint8_t g_a4ef38[0x140];
extern std::uint8_t g_a53740[0x140];
// 0x00A9C4E0 / 0x00A9CAE0: split/merge callback tables. Callers scale a uint16 category by 4;
// the 0x600-byte table stride and the 0x180 category row width are established in the .cpp.
extern TerrainContourCallback g_a9c4e0[0x180];
extern TerrainContourCallback g_a9cae0[0x180];
// 0x00B04340: 0x800 entries x 0x1C bytes previous-frame contour hash cache.
extern TerrainContourHashEntry g_b04340[0x800];
// 0x00B12B80: records dropped after the 0x1000 record limit.
extern std::uint32_t g_b12b80;
// 0x00B12BB8: callback-forced child-mask suppression flag.
extern std::int32_t g_b12bb8;
// 0x00B12BC0 / 0x00B12BC4 / 0x00B12BC8: camera-space offsets copied from context+0xDE0..0xDE8.
extern std::int32_t g_b12bc0;
extern std::int32_t g_b12bc4;
extern std::int32_t g_b12bc8;
// 0x00B12BE0: 0x40-byte octant permutation table written by 0x004D0EF0.
extern std::uint8_t g_b12be0[0x40];
// 0x00B12C20: number of 8-byte allocation records emitted.
extern std::uint32_t g_b12c20;
// 0x00B12C58: cursor into the 8-byte allocation-record array (0x00B16C98 in this image).
extern std::uint8_t* g_b12c58;
// 0x00B16C6C: contour detail cut-off, floored at 0x46.
extern std::int32_t g_b16c6c;
// 0x00BCAB20 / 0x00BCAB24: view angles in radians (single-precision floats).
extern float g_bcab20;
extern float g_bcab24;
// 0x00C10380: frame counter, low 16 bits used as the cache stamp.
extern std::uint32_t g_c10380;
// 0x005470B4: fog-enable dword; the retail image holds 1 at this address.
extern std::int32_t g_5470b4;
} // namespace igi::retail::terrain
