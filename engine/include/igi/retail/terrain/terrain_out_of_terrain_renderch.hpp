// Native: Terrain_OutOfTerrainRenderch; retail VA 0x00514680.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
//   (1,384,448 bytes, PE i386, image base 0x00400000, entry 0x004A50F5).
// Tools/evidence (this transcription): Binary Ninja MCP 0.1.0 (`bn_function_info`,
//   `bn_function_basic_blocks`, `bn_function_decompile`, `bn_function_disassembly`,
//   `bn_function_callees`, `bn_function_callers`, `bn_function_complexity`), radare2 6.2.0
//   (`aaa`, `af`, `afij`, `afbj`, `pdf`, `axt`), angr 10.0.4.post1 CFGFast.
// Behavior/CFG: 0x00514680..0x0051475A (218 bytes), 9 basic blocks / 13 edges, 62 instructions,
//   2 direct calls (0x004AF7B0 Debug_Error and 0x004AF910 Camera_ApplyLineFilter_33), one
//   `retn` and one intentional `jmp $`. radare2 agrees exactly on the boundary, 9 blocks, 13
//   edges and 218 bytes; angr recovers the exact entry with no unresolved jumps (its 11 nodes are
//   call-site normalisation, not a boundary disagreement).
// Calling convention: `TerrainRenderChunk* __cdecl Terrain_OutOfTerrainRenderch(
//   const void* texture_owner, std::int32_t pass)`; two 32-bit stack arguments, pointer return
//   in eax. The three callers are Terrain_Draw 0x00512C30 at 0x00513C7F (pass 0), 0x00513C9E
//   (pass 1) and 0x005140B8 (pass 2).
//
// Argument meaning (inferred from the call sites and from how Terrain_Draw consumes the result,
// not from a symbol):
//   texture_owner: the 32-bit texture-owner/material pointer read from the terrain block record.
//     Terrain_Draw later treats the stored dword as a native pointer and reads [owner+0x20] as
//     the IDirect3DTexture2 for SetTexture.
//   pass: the blend-mode pass index 0/1/2 that is also passed to Terrain_UnknownBlendmode.
//   The retail function itself only hashes and compares both words; it never dereferences them.
//
// Status/limits: TRANSCRIBED candidate only. No candidate-vs-retail CFG comparison, semantic
//   review or paired runtime capture was performed. The translation unit is in the igi link
//   (CMakeLists.txt). The retail-owned pool/bucket globals are declared here and each has exactly
//   one engine definition: g_a7e080 and g_a7f884 in terrain_out_of_terrain_renderch.cpp,
//   g_a7f878/g_a7f87c/g_a7f880/g_a7f888/g_a7f88c in terrain_storage.cpp (their former duplicate
//   definitions in terrain_draw.cpp were removed). The candidate does not run the retail
//   initialisers 0x00516621 (buckets) or 0x00516640 -> 0x004B0B00 (pool); see the definition notes
//   in terrain_out_of_terrain_renderch.cpp.
#pragma once

#include <cstdint>

namespace igi::retail::terrain
{
// One 0x2EF4-byte terrain render chunk. The pool (0x004B0B00 called from 0x00516640 with
// (&g_a7f878, 0x2EF4, 0x80, 0)) aligns the stride to 0x2EF8; the four bytes at +0x2EF4 are used
// for the element's slot index (the g_a7f88c offset), so sizeof(TerrainRenderChunk) intentionally
// excludes them.
struct TerrainRenderChunk
{
    TerrainRenderChunk* next;      // +0x0000: next node, or the bucket sentinel
    TerrainRenderChunk* prev;      // +0x0004: back link, or the bucket record for the first node
    std::uint8_t indices[0x2EE0];  // +0x0008: triangle indices, 3 uint16 per triangle
    std::uint32_t triangle_count;  // +0x2EE8: number of triangles (draw uses count*3 indices)
    std::uint32_t texture_owner;   // +0x2EEC: 32-bit native texture-owner pointer value
    std::uint32_t pass;            // +0x2EF0: blend-mode pass index
};

// One 12-byte hash bucket. The sentinel is the fake node rooted at +0x0004: its "next" dword is
// +0x0004 (zero), and its "prev" dword is +0x0008 (the bucket record on an empty bucket, then the
// first real node). 0x00516621 initialises 509 records from 0x00A7E080 through 0x00A7F85B.
struct TerrainRenderChunkBucket
{
    TerrainRenderChunk* head;   // +0x00: list head, or &sentinel_next when empty
    std::uint32_t sentinel_next;// +0x04: always zero
    void* sentinel_prev;        // +0x08: bucket record initially, first node after an insert
};

inline constexpr std::uint32_t terrain_render_chunk_bucket_count = 0x1FD; // 509; 0x00A7E080..0x00A7F85C
inline constexpr std::uint32_t terrain_render_chunk_pool_capacity = 0x80; // g_a7f884 after 0x004B0B00
inline constexpr std::uint32_t terrain_render_chunk_pool_stride = 0x2EF8; // aligned 0x2EF4 request

inline constexpr std::uint32_t chunk_offset_next = 0x0000;
inline constexpr std::uint32_t chunk_offset_prev = 0x0004;
inline constexpr std::uint32_t chunk_offset_triangle_count = 0x2EE8;
inline constexpr std::uint32_t chunk_offset_texture_owner = 0x2EEC;
inline constexpr std::uint32_t chunk_offset_pass = 0x2EF0;
inline constexpr std::uint32_t chunk_offset_pool_slot = 0x2EF4;

// 0x00A7E080: 509 hash buckets, 12 bytes each, initialised by 0x00516621.
extern TerrainRenderChunkBucket g_a7e080[terrain_render_chunk_bucket_count];

// 0x00A7F878: visible-chunk element base address (32-bit), set by 0x004B0B00 from 0x00516640.
extern std::uint32_t g_a7f878;
// 0x00A7F87C: free-slot list pointer, set to base + stride*capacity by 0x004B0B00.
extern std::uint32_t* g_a7f87c;
// 0x00A7F880: used element count; incremented by this native.
extern std::uint32_t g_a7f880;
// 0x00A7F884: element capacity (0x80 from the 0x00516640 initialiser).
extern std::uint32_t g_a7f884;
// 0x00A7F888: aligned element stride (0x2EF8 after 0x004B0B00).
extern std::uint32_t g_a7f888;
// 0x00A7F88C: element-relative slot-index offset. 0x004B0B1D `mov dword [esi+0x14], ecx`
// (ecx = arg2 = 0x2EF4, the requested element size from 0x005166A4) stores a scalar here; it is
// not a pointer. Both readers use it as a byte offset from the element base: 0x00514713/0x0051471B
// (`mov ecx, [0xa7f88c]; mov [ecx + esi], eax`) and 0x00514485/0x0051448E
// (`mov edi, [0xa7f88c]; mov edx, [edi + esi]`). Defined once as std::uint32_t in
// terrain_storage.cpp; terrain_draw.cpp's drain loop therefore uses offset arithmetic, not a
// reverse-map pointer.
extern std::uint32_t g_a7f88c;

extern "C" TerrainRenderChunk* Terrain_OutOfTerrainRenderch(const void* texture_owner,
                                                           std::int32_t pass);
} // namespace igi::retail::terrain
