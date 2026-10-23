// Native: Terrain_Draw; retail VA 0x00512C30.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
//   PE i386, image base 0x00400000, entry 0x004A50F5; function range 0x00512C30..0x005144DC
//   (0x18AC bytes in the retail image; r2 `afij` realsz 6317, maxaddr 0x005144DD).
// Tools/evidence: Binary Ninja MCP 0.1.0 (`bn_function_info`, `bn_function_basic_blocks`,
//   `bn_function_decompile`, `bn_function_disassembly`, `bn_function_callees`,
//   `bn_function_callers`, `bn_function_complexity`), radare2 6.2.0 (`af`, `afbj`, `pdf`),
//   angr 10.0.4.post1 CFGFast; artefacts docs/fidelity/evidence/0x00512C30-*.json and the record
//   in docs/fidelity/native-verification.json.
// Behavior/CFG: 144 blocks / 215 edges / 1670 instructions / 17 loop latches / one `retn` at
//   0x005144DC. All 1670 instructions are transcribed here: 299 at 0x00512C30..0x00513057 (frame
//   probe, fog-range globals, four matrix-slab invalidations, the D3DVIEWPORT7 fill, the packed
//   fog colour, the identity world matrix, 17 SetRenderState + 10 SetTextureStageState calls, and
//   the vertex-buffer Lock with its early return); 1279 at 0x0051305D..0x005143AD (the terrain
//   block loop: per-block LOD/cull, the two vertex-projection loops, strip topology and
//   screen-bound flags, the three blend passes, the fourth-chunk section and the index emitters);
//   and 92 at 0x005143B3..0x005144DC (Unlock, the three blend-mode passes with one SetTexture +
//   DrawIndexedPrimitiveVB per visible chunk, the heap-list drain and the single return).
// Callee: the block loop calls Terrain_BuildMipMaps_2 at 0x00514760 for clipped or
//   mixed-attribute triangles (call sites 0x00513EF7 and 0x00514308). That native (7839 bytes,
//   210 basic blocks) has its own transcription, terrain_mipmaps.cpp, and its shared declaration
//   is terrain_mipmaps.hpp; this header does not redeclare it. The two call sites pass three
//   0x40-byte records by value plus a pointer to a five-dword state block, which is the ABI both
//   units now share.
// Scope/status: TRANSCRIBED candidate (all instructions present). Not STRUCTURALLY_VERIFIED,
//   RUNTIME_TESTED or VERIFIED: no candidate-vs-retail CFG, semantic or runtime comparison was
//   performed. BUILD_VERIFIED only in the narrow sense that the unit compiles and links into the
//   igi target - that says nothing about behavioural equivalence. The throwing Terrain_Draw
//   placeholder in terrain_loaders.cpp was deleted (wrong ABI, no callers); terrain_draw.cpp is
//   the only definition.
#pragma once

#include <cstdint>

namespace igi::retail::terrain
{
// Retail signature: `std::int32_t __cdecl Terrain_Draw(void)`.
// Evidence: the only caller (0x005116A2 inside Terrain_UpdateWaterWaves 0x00511500) pushes no
// arguments and discards eax; the body reads no incoming stack argument; `mov eax, 0x14F0;
// call 0x004A54A0` at 0x00512C30 is the MSVC 6 `__chkstk` helper allocating the 0x14F0-byte
// frame (0x004A54A0 probes pages and sets esp; the matching `add esp, 0x14F0` is at 0x005144D6),
// not a game-native call.
//
// Return value: the function has exactly one `retn` (0x005144DC) and eax is either the value of
// 0x00A7F880 after the drain loop (always 0 when that loop exits) or the failed Lock HRESULT from
// the 0x00512FE0 early jump. A successful frame therefore returns 0, which matches the level-1
// retail capture observing 1243 calls all with EAX 0.
[[nodiscard]] std::int32_t Terrain_Draw();

// True only when 0x0051305D..0x005143AD has been transcribed. It is true in this candidate. The
// block loop still depends on Terrain_BuildMipMaps_2 (0x00514760), which is declared but not
// defined here; see the open-dependency note above.
inline constexpr bool terrain_draw_block_loop_transcribed = true;

// ---------------------------------------------------------------------------------------------
// Render objects touched through fixed vtable slots.
//
// Retail reads the device pointer from global 0x005CA114 (31 reads; written by the renderer at
// 0x00494630 and 0x00493BB2) and the vertex-buffer pointer from 0x00A7F890. Every device call is
// an indirect call through `(*device)[offset]`; the offsets observed in this function are
// +0x2C, +0x34, +0x50, +0x80, +0x8C and +0x94, and the vertex-buffer calls are +0x0C and +0x10.
// Those offsets and argument lists match the Direct3D 7 `IDirect3DDevice7` /
// `IDirect3DVertexBuffer7` declarations exactly (mingw-w64 d3d.h, Wine-derived DX7 order:
// +0x2C SetTransform, +0x34 SetViewport, +0x50 SetRenderState, +0x80 DrawIndexedPrimitiveVB,
// +0x8C SetTexture, +0x94 SetTextureStageState; +0x0C Lock, +0x10 Unlock). The mapping is an
// inference from offset + argument shape; the decompilers do not name these calls.
//
// The candidate has no Direct3D layer, so the host supplies both objects. This table is host
// layout, not a claim about retail memory.
// ---------------------------------------------------------------------------------------------
struct Device7;
struct VertexBuffer7;

struct Device7Vtbl
{
    // retail +0x2C: (self, state, matrix) -- 0x00512DCD passes state 1 (D3DTS_WORLD).
    std::int32_t(__stdcall* SetTransform)(Device7* self, std::uint32_t state, const float* matrix);
    // retail +0x34: (self, viewport) -- 6 dwords at 0x00A7F860.
    std::int32_t(__stdcall* SetViewport)(Device7* self, const void* viewport);
    // retail +0x50: (self, state, value) -- 17 call sites.
    std::int32_t(__stdcall* SetRenderState)(Device7* self, std::uint32_t state, std::uint32_t value);
    // retail +0x80: (self, primitive, vb, start, vertex_count, indices, index_count, flags).
    std::int32_t(__stdcall* DrawIndexedPrimitiveVB)(Device7* self, std::uint32_t primitive_type,
        VertexBuffer7* vertex_buffer, std::uint32_t start_vertex, std::uint32_t vertex_count,
        const std::uint16_t* indices, std::uint32_t index_count, std::uint32_t flags);
    // retail +0x8C: (self, stage, texture) -- one call site, stage 0.
    std::int32_t(__stdcall* SetTexture)(Device7* self, std::uint32_t stage, void* texture);
    // retail +0x94: (self, stage, type, value) -- 10 call sites.
    std::int32_t(__stdcall* SetTextureStageState)(
        Device7* self, std::uint32_t stage, std::uint32_t type, std::uint32_t value);
};

struct Device7
{
    const Device7Vtbl* vtbl;
};

struct VertexBuffer7Vtbl
{
    // retail +0x0C: Lock(flags, void** data, DWORD* size) -- 0x00512FDB passes 0x2020 and
    // stores the returned data pointer in 0x00A7F89C; the HRESULT is returned to the caller.
    std::int32_t(__stdcall* Lock)(VertexBuffer7* self, std::uint32_t flags, void** data, std::uint32_t* size);
    // retail +0x10: Unlock() -- 0x005143BD.
    std::int32_t(__stdcall* Unlock)(VertexBuffer7* self);
};

struct VertexBuffer7
{
    const VertexBuffer7Vtbl* vtbl;
};

// Installs the retail-owned render objects (globals 0x005CA114 and 0x00A7F890). Both must be set
// before Terrain_Draw runs; with no object installed the native throws a documented
// std::logic_error instead of dereferencing null.
void Terrain_HostInstallRenderDevice(Device7* device, VertexBuffer7* vertex_buffer);

// 0x005144E0: per-pass blend-mode setup, retail 406 bytes / 8 blocks / __cdecl.
// Transcribed in terrain_unknown_blendmode.cpp and linked into the igi target; this declaration is
// the C-linkage interface both units share. The transcribed epilogue calls it once per pass.
// Return value: the HRESULT of the final SetRenderState (device vtable +0x50) on every path.
// Each valid branch ends with that indirect call immediately followed by `retn`:
//   blendmode 0: 0x00514672 `call dword [edx+0x50]`, 0x00514675 `retn`
//   blendmode 1: 0x00514608 `call dword [edx+0x50]`, 0x0051460B `retn`
//   blendmode 2: 0x00514585 `call dword [ecx+0x50]`, 0x00514588 `retn`
// The invalid-blendmode path never returns (Debug_Error then `jmp $` at 0x00514504). The only
// caller, Terrain_Draw at 0x005143C9, discards eax, so this return value is not observable there.
extern "C" std::int32_t Terrain_UnknownBlendmode(std::int32_t blendmode);
} // namespace igi::retail::terrain
