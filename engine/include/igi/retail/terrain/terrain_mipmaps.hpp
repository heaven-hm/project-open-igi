// Native: Terrain_BuildMipMaps_2; retail VA 0x00514760 (catalog name; see the naming note below).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01, 1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386 (PE32).
// Tools/evidence: Binary Ninja MCP 0.1.0 (selected decompiler), radare2 6.2.0 and angr 10.0.4.post1 CFGFast at the exact entry; full commands and digests in docs/fidelity/evidence/0x00514760-*.
// Behavior/CFG: 7839 bytes, 210 blocks / 284 edges / 2257 instructions; near clip in clip space, four screen-space stages with one-outside recursion, then a 3-vertex emitter.
// Status/limits: STRUCTURALLY_VERIFIED (source/object-level property comparison; not CFG-isomorphic). Not RUNTIME_TESTED. Unresolved items are listed in the .cpp.
//
// Detailed tool counts for this header and the matching .cpp:
//   Binary Ninja 0.1.0: 7839 bytes, 210 blocks, 284 edges, 2257 instructions, 7 return blocks,
//     26 call sites (5 self-recursion + 21 `rep movsd` copied as __builtin_memcpy). The rendered
//     prototype (stack offsets 0x24/0x64/0xA4/0xC4) is stack-frame numbering, not the ABI.
//   radare2 6.2.0 `afij`: size 7839, 210 blocks, 284 edges, 2257 instructions, cdecl, recursive,
//     stackframe 372, cyclomatic complexity 88, in-degree 5, out-degree 5.
//   angr 10.0.4.post1 CFGFast at the exact entry: 7839 bytes, 2257 instructions, 259 nodes / 354
//     edges; extra nodes are call-site splits, extra edges its call-block artefacts.
// Control flow: 210/210 blocks are accounted for by the regions documented in the .cpp. Exactly
//   four loops, all single-level 3-iteration do/while loops with no nesting:
//     0x0051487B -> 0x0051481A  near-plane vertex transform/scaling pass
//     0x00514C3E -> 0x00514C3E  near-plane count==2 attribute scale pass
//     0x0051501A -> 0x0051501A  near-plane count==1 attribute scale pass
//     0x0051651B -> 0x0051651B  final vertex/index emitter
//   No indirect jump, no indirect call, and no callee other than the function itself: the five
//   `call 0x00514760` sites are at 0x00515101, 0x005156A0, 0x00515BB0, 0x00516079 and 0x005164D3.
//   Binary Ninja additionally lists 21 `__builtin_memcpy` "calls"; those are the 21 `rep movsd`
//   (16-dword) copies in this function, not external functions.
//
// Calling convention and interface (derived from the only two external call sites, both inside
// Terrain_Draw at 0x00513EF7 and 0x00514308):
//   __cdecl
//   std::uint32_t Terrain_BuildMipMaps_2(TerrainMipMapsVertex r1,   // [E+0x04], 0x40 bytes by value
//                                        TerrainMipMapsVertex r2,   // [E+0x44], 0x40 bytes by value
//                                        TerrainMipMapsVertex r3,   // [E+0x84], 0x40 bytes by value
//                                        TerrainMipMapsCall* call); // [E+0xC4], 0x14-byte record
//   The caller pushes the call-record pointer, then `sub esp,0x40` + `rep movsd` three times to
//   place the three records, and cleans up with `add esp,0xC4`. Because the records are passed by
//   value they live in the caller's frame and this function mutates those copies in place; the
//   caller does not read them again after the call. A C++ transcription with by-value parameters
//   reproduces that: the copies are the callee's own argument slots.
//   The five recursion sites push the same shape (one 0x14 pointer argument already in hand plus
//   three 0x40-byte records) and then `add esp,0xC4`.
//
// Return value: no caller reads it (`stream`-style void use in Terrain_Draw). The exact register
//   value per return path is recorded in the .cpp; it is incidental state, not a status code.
//
// Algorithm (established from the instruction stream; the catalog name is not supported by it):
//   The function does not build mip levels. It is the terrain triangle clip-project-and-emit
//   routine: it takes three already-transformed terrain triangle vertices with outcodes, clips
//   them against the near plane in clip space and against four screen bounds in screen space,
//   re-projects the new vertices, and emits the resulting triangle into the caller's vertex and
//   index buffers, converting each vertex to the 0x20-byte output format.
//   1. 0x00514766..0x005147E4: increment g_a7e070, take the three outcode bytes, and return
//      immediately when (r1 & r2 & r3) != 0 (all three vertices outside the same plane).
//   2. 0x005147E5..0x00515136 (near plane, bit 0x10): when any vertex has bit 0x10, transform all
//      three into the clip-space scratch slots at [E-0x30..E-0x01] (flagged vertices keep their
//      incoming xyz; unflagged ones are unprojected back through g_c311A4/g_c311B0 and
//      g_c311A0/g_c311B4 and 1/z), set the +0x28..+0x3C attributes from the packed colour, then
//      clip in clip space against g_bcadc0 and project the intersections back to screen space.
//   3. 0x00515136..0x005165F4: four screen-space stages in this order - left x/g_bcabd0 (bit 0x01),
//      top y/g_bcabd4 (bit 0x02), right x/g_bcabd8 (bit 0x04), bottom y/g_bcabdc (bit 0x08). Each
//      stage counts how many of the three vertices carry its bit: two outside -> replace both with
//      edge intersections and continue; one outside -> save it, build two intersections, recurse
//      once with (I1, I2, inside2) and continue with the other half; zero outside -> skip.
//   4. 0x005164F3..0x005165F4: emit three vertices and three indices through the call record.
//   Each stage recomputes only the outcodes of the planes it has not tested yet, and returns when
//   the three current outcode bytes share a bit (the triangle is entirely outside that plane).
//   Nothing is allocated: the only memory written is the caller's vertex/index buffers (through
//   the call record), the three by-value record copies, and the g_a7e070 counter.
//
// Naming note: the retail catalog name is `Terrain_BuildMipMaps_2`, and the retail evidence does
//   not support "mipmaps": the body contains no level loop, no texture dimension and no surface
//   allocation. It is a per-triangle clipper fed by Terrain_Draw's clipped-triangle path. The name
//   is kept because it is the established symbol for this VA in this repository and in the
//   catalog; the discrepancy is recorded rather than silently renamed.
//
// Layout guard: the static_asserts below require the 32-bit ABI (the 0x24 pointer field is four
//   bytes), so this header only compiles for a 32-bit target - intentionally, since the retail
//   record is fixed at 0x40 bytes.
//
// Status: ANALYZED + TRANSCRIBED + STRUCTURALLY_VERIFIED by a source/object-level property
//   comparison against the retail Binary Ninja/radare2/angr evidence (see
//   docs/fidelity/evidence/0x00514760-structural-comparison-20261023.json). The candidate is not
//   CFG-isomorphic: the four duplicated retail screen stages are one parameterised stage function
//   driven by a four-entry loop, and helper calls replace inlined code. NOT RUNTIME_TESTED: no
//   paired runtime observation exists, so RUNTIME_TESTED and VERIFIED are excluded.
//
// Unresolved (documented, not guessed): see the UNRESOLVED section of terrain_mipmaps.cpp. The
//   main items are the purpose of g_a7e070, the x87 control word in effect at the FISTP sites, the
//   content of the three padding bytes at record +0x21..+0x23 (never read here), and the fact that
//   the +0x24 clip pointer written into each record copy dangles once the function returns.
//
// Ownership: this unit is in the igi link (CMakeLists.txt). It links against the retail global
//   definitions in terrain_draw.cpp and terrain_storage.cpp, and against g_a7e070, which
//   terrain_draw.cpp defines with the retail 0x00A7E070 evidence.
#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::terrain
{
// One terrain triangle vertex as this function sees it: 0x40 bytes, the layout Terrain_Draw's
// clipped-triangle path (ProjectedVertex 0x20 bytes + MipMapsVertex extension) builds.
//   +0x00 x      screen x. When the terrain_outcode_near bit is set on this vertex the caller has not
//                projected it yet, so x/y/z hold clip-space xyz instead.
//   +0x04 y      screen y (or clip-space y).
//   +0x08 z      reciprocal depth: the caller stores the depth scale it computed, and this function
//                stores 1/clip_z into the vertices it projects. The emitter turns it into fog.
//   +0x0C depth  not read on input; the emitter overwrites it with z * g_bcad70.
//   +0x10 colour 0x00RRGGBB packed colour (only the low three bytes are read).
//   +0x14 alpha  only the top byte is read, as the alpha component.
//   +0x18 u, +0x1C v  texture coordinates, copied to the output vertex verbatim.
//   +0x20 flag   outcode: 0x01 x<left, 0x02 y<top, 0x04 x>right, 0x08 y>bottom, 0x10 z<near.
//   +0x21..+0x23 padding, never read by this function (terrain_draw.cpp documents that retail's
//                stack copies leave these three bytes uninitialised).
//   +0x24 clip   pointer to the 3-float clip-space scratch this function installs. It is written
//                into the record (and therefore into the caller's by-value copies) but never read
//                after the call returns, so the pointer dangles exactly as it does in retail.
//   +0x28 red, +0x2C green, +0x30 blue, +0x34 tex u, +0x38 tex v, +0x3C alpha: the interpolation
//                attributes. The near-plane transform rewrites them from the packed colour/u/v/alpha.
struct TerrainMipMapsVertex
{
    float x;
    float y;
    float z;
    float depth;
    std::uint32_t colour;
    std::uint32_t alpha;
    float u;
    float v;
    std::uint8_t flag;
    std::uint8_t pad[3];
    float* clip;
    float red;
    float green;
    float blue;
    float tex_u;
    float tex_v;
    float alpha_f;
};
static_assert(sizeof(TerrainMipMapsVertex) == 0x40, "retail vertex record is 0x40 bytes");
static_assert(offsetof(TerrainMipMapsVertex, colour) == 0x10, "colour is at +0x10");
static_assert(offsetof(TerrainMipMapsVertex, u) == 0x18, "u is at +0x18");
static_assert(offsetof(TerrainMipMapsVertex, flag) == 0x20, "outcode is at +0x20");
static_assert(offsetof(TerrainMipMapsVertex, clip) == 0x24, "clip scratch pointer is at +0x24");
static_assert(offsetof(TerrainMipMapsVertex, red) == 0x28, "interpolation attributes start at +0x28");

// The 0x14-byte call record built by Terrain_Draw's make_mipmaps_record lambda and passed as the
// fourth argument. All five fields are pointers/live state; this function both reads and writes
// every one of them except base_vertex (read only).
struct TerrainMipMapsCall
{
    std::uint8_t* vertex_output;    // +0x00 base of the 0x20-byte output vertex buffer
    std::int32_t* vertex_count;     // +0x04 live vertex count (read, then post-incremented)
    std::uint16_t* index_output;    // +0x08 base of the uint16 index buffer
    std::int32_t* triangle_count;   // +0x0C live triangle count (read, then post-incremented)
    std::uint32_t base_vertex;      // +0x10 value added to each emitted index
};
static_assert(sizeof(TerrainMipMapsCall) == 0x14, "retail call record is 0x14 bytes");
static_assert(offsetof(TerrainMipMapsCall, index_output) == 0x08, "index buffer is at +0x08");
static_assert(offsetof(TerrainMipMapsCall, base_vertex) == 0x10, "base vertex is at +0x10");

// Outcode bits, in the order the stages test them. The bit is set when the vertex is outside the
// matching plane; a bit present in all three vertices means the whole triangle is outside.
inline constexpr std::uint32_t terrain_outcode_left = 0x01;   // x < g_bcabd0
inline constexpr std::uint32_t terrain_outcode_top = 0x02;    // y < g_bcabd4
inline constexpr std::uint32_t terrain_outcode_right = 0x04;  // x > g_bcabd8
inline constexpr std::uint32_t terrain_outcode_bottom = 0x08; // y > g_bcabdc
inline constexpr std::uint32_t terrain_outcode_near = 0x10;   // z < g_bcadc0

// ---------------------------------------------------------------------------------------------
// Retail-owned globals read or written by 0x00514760. Every declaration records the retail VA.
// The definitions live in terrain_draw.cpp and terrain_storage.cpp, and are declared here with the
// same types those translation units define, so this header cannot introduce a conflicting type
// for an existing symbol. g_a7e070 (0x00A7E070) is defined in terrain_draw.cpp.
// ---------------------------------------------------------------------------------------------
// 0x00BCABD0: left screen bound, compared as a float. 0x00BCABD4: top screen bound.
// 0x00BCABD8: right screen bound. 0x00BCABDC: bottom screen bound. Retail stores dwords; the
// function loads each as a float (`fcomp dword [...]`), so the comparison goes through
// bits_to_float() below.
extern std::uint32_t g_bcabd0;
extern std::uint32_t g_bcabd4;
extern std::uint32_t g_bcabd8;
extern std::uint32_t g_bcabdc;

// 0x00BCADC0: near clip plane in clip space, compared against clip-space z as a float.
extern float g_bcadc0;
// 0x00BCAD70: depth scale; the emitter stores inv * g_bcad70 into output vertex +0x0C.
extern float g_bcad70;
// 0x00A7F894 / 0x00A7E074: fog depth scale and fog range offset; the emitter computes
// inv * g_a7f894 + g_a7e074 into output vertex +0x08.
extern float g_a7f894;
extern float g_a7e074;
// 0x00C311A4 / 0x00C311B0: projection scale and offset for x. 0x00C311A0 / 0x00C311B4: the same
// for y. Used to unproject screen-space vertices and to project clip-space intersections.
extern float g_c311a0;
extern float g_c311a4;
extern float g_c311b0;
extern float g_c311b4;
// 0x00A7E070: incremented on entry and decremented on every return path of 0x00514760. The only
// accesses in this function are that pair, so the purpose is not established; it is modelled
// exactly as the pair of updates, not as a named semantic quantity.
extern std::uint32_t g_a7e070;

// 0x00514760: see the provenance block above for the ABI and status. `call` is dereferenced
// without a null check, matching retail.
extern "C" std::uint32_t Terrain_BuildMipMaps_2(TerrainMipMapsVertex r1,
                                                TerrainMipMapsVertex r2,
                                                TerrainMipMapsVertex r3,
                                                TerrainMipMapsCall* call);
} // namespace igi::retail::terrain
