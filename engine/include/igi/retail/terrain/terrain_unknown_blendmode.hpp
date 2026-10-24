// Native: Terrain_UnknownBlendmode; retail VA 0x005144E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
//   (1,384,448 bytes, PE i386, image base 0x00400000, entry 0x004A50F5).
// Tools/evidence (this transcription): Binary Ninja MCP 0.1.0 (`bn_function_info`,
//   `bn_function_basic_blocks`, `bn_function_decompile`, `bn_function_disassembly`,
//   `bn_function_callees`, `bn_function_callers`, `bn_function_complexity`, `bn_data_at`),
//   radare2 6.2.0 (`aaa`, `af`, `afij`, `afbj`, `pdf`, `axt`), angr 10.0.4.post1 CFGFast.
// Behavior/CFG: 0x005144E0..0x00514676 (406 bytes), 8 basic blocks / 8 edges, 153 x86
//   instructions (radare2 ninstrs=153; Binary Ninja's 164-line disassembly report also counts
//   block separators), 23 calls (22 indirect Direct3D7 device calls plus 0x004AF7B0 Debug_Error),
//   one `retn` per accepted mode and one intentional `jmp $` on the default path. radare2 agrees
//   exactly on the boundary, 8 blocks, 8 edges and 406 bytes; angr recovers the exact entry with
//   no unresolved jumps (its 31 nodes are call-site normalisation, not a boundary disagreement).
// Calling convention: `std::int32_t __cdecl Terrain_UnknownBlendmode(std::int32_t blendmode)`.
//   The only caller (Terrain_Draw 0x00512C30 at 0x005143C9) pushes one argument and executes
//   `add esp, 4`; the function reads only [esp+4] and returns eax.
// Status/limits: TRANSCRIBED candidate only. No candidate-vs-retail CFG comparison, semantic
//   review or paired runtime capture was performed. The translation unit is in the igi link
//   (CMakeLists.txt), which proves only that it compiles and links. The Direct3D7 device is the
//   host abstraction declared by terrain_draw.hpp, not retail memory. See
//   terrain_unknown_blendmode.cpp and the record in docs/fidelity/native-verification.json for
//   the full scope.
#pragma once

#include <cstdint>

#include "igi/retail/terrain/terrain_draw.hpp"

namespace igi::retail::terrain
{
// 0x005CA114: IDirect3DDevice7* installed by the renderer (retail writes it at 0x00494630 and
// 0x00493BB2). The native reloads this global before every indirect method call, so the host
// mirror is reloaded the same way rather than cached.
extern Device7* g_5ca114;

// 0x00A7E068: packed fog colour (D3DRS_FOGCOLOR value) staged by Terrain_Draw at 0x00512D8D:
//   int(0xA7DA38 * 255) - ((int(0xA7DA30 * -255) << 8) + int(0xA7DA34 * -255)) << 8
// over the three float components at 0x00A7DA30/0x00A7DA34/0x00A7DA38. Read only by mode 0.
extern std::int32_t g_a7e068;

// 0x005470B4: fog-enable dword. The file image holds 1; code at 0x004E7FA0 overwrites it from
// [ebx+0xBC] (the producing object type is not established). Read by all three accepted branches
// and passed to D3DRS_FOGENABLE. The semantic label is inferred from the render-state id, not from
// a symbol.
extern std::int32_t g_5470b4;

// The retail prototype is declared by terrain_draw.hpp:
//   extern "C" std::int32_t Terrain_UnknownBlendmode(std::int32_t blendmode);
// This translation unit defines it. The definition and the declaration both have C linkage.
} // namespace igi::retail::terrain
