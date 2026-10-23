// Native: Terrain_Draw; retail VA 0x00512C30.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1,384,448 bytes, PE i386, image base 0x00400000, entry 0x004A50F5.
// Tools/evidence: Binary Ninja MCP 0.1.0, radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; evidence artefacts docs/fidelity/evidence/0x00512C30-*.json.
// Behavior/CFG: retail range 0x00512C30-0x005144DC, 6317 bytes, 144 blocks / 215 edges, 1670 instructions; Binary Ninja and radare2 recover the same graph, and the 17-property source comparison is docs/fidelity/evidence/0x00512C30-structural-comparison.json.
// Status/limits: STRUCTURALLY_VERIFIED (source/object-level property comparison; not CFG-isomorphic; not RUNTIME_TESTED); the detailed limits are at the end of this header.
//
//   Evidence artefacts: docs/fidelity/evidence/0x00512C30-*.json; record:
//   docs/fidelity/native-verification.json.
//
// Verified structure (retail image only)
//   Binary Ninja : 0x00512C30-0x005144DC, 6317 bytes, 144 basic blocks, 215 edges, 1670
//                  instructions, 63 call sites (43 `call` instructions plus inline rep-movsd
//                  copies lifted as __builtin_memcpy/__builtin_memset), one `retn` at 0x005144DC.
//   radare2      : size 6317, 144 blocks, 215 edges, 1670 instructions, stackframe 612,
//                  callconv cdecl, cyclomatic complexity 73; identical boundaries.
//   angr CFGFast : exact entry 0x00512C30 recovered, 225 nodes / 316 edges, no unresolved jumps,
//                  returning. The node/edge delta against BN/r2 is CFGFast splitting the
//                  rep-movsd/loop sites; it is not a boundary disagreement.
//   Loops        : 17 latches by dominator back-edge analysis, latch -> header:
//                  0x513089->0x51308B (outer block loop, body 127 blocks; rotated: the latch
//                  falls through),  0x5131EF->0x5131F3 (rotated),  0x51389B->0x51360C,
//                  0x51394A->0x51394E (rotated),  0x513BE7->0x513BA8,  0x513C5A->0x513C03,
//                  0x513E64->0x513D83,  0x513F21->0x513CFA,  0x513FA3->0x513B6A,
//                  0x513FB1->0x513B61,  0x514033->0x514000,  0x51409B->0x51404F,
//                  0x51410F->0x514113 (rotated),  0x514276->0x514194,  0x51444F->0x5143DB,
//                  0x514453->0x5143C2,  0x51446E->0x51447A (rotated).
//   Branch shape : 72 blocks with two successors, 71 with one, 1 exit block (0x5144D2).
//
// Calling convention / ABI
//   `std::int32_t __cdecl Terrain_Draw(void)`: the only caller (0x005116A2 in
//   Terrain_UpdateWaterWaves 0x00511500) pushes no arguments and ignores eax; the body reads no
//   incoming stack slot. `mov eax, 0x14F0; call 0x004A54A0` at 0x00512C30 is the Microsoft C 6
//   `__chkstk` helper: 0x004A54A0 probes one page per 0x1000 bytes and then lowers esp itself, so
//   the call allocates the 0x14F0-byte frame (the matching `add esp, 0x14F0` is at 0x005144D6).
//   Binary Ninja therefore reports "unresolved stack usage" and invents twelve float/double
//   parameters, and radare2 reports 26 args; neither is a real signature. Callee-saved registers
//   ebx/ebp/esi/edi are pushed at 0x00512C81..0x00512C86, after the probe.
//
// Return value on every path (single exit)
//   0x005144C6 `mov eax, [0x00A7F880]`; 0x005144CE `test eax, eax`; 0x005144D0 `jne` back to the
//   drain loop; fall-through to pop/pop/pop/pop, `add esp, 0x14F0`, `retn`. The drain loop only
//   exits when 0x00A7F880 is 0, and the earlier `cmp eax, ebx; je 0x5144D2` at 0x00514462 also
//   requires eax == 0. The single other edge into 0x5144D2 comes from 0x00512FE0 `jne 0x005144D2`
//   right after the vertex-buffer Lock, leaving eax = the Lock HRESULT. Normal frames therefore
//   return 0 (which is why the level-1 capture observed EAX 0 on all 1243 calls); a failed Lock
//   returns its non-zero HRESULT.
//
// Draw pipeline driven by the transcribed part
//   1. allocates the 0x14F0 frame (above);
//   2. derives the fog depth ratio/offset pair 0x00A7F894 and 0x00A7E074 from the camera depth
//      globals 0x00BCADA0/0x00BCADC8/0x00BCAD8C/0x00BCADB4/0x00BCAD70;
//   3. writes -1 into +0x04 of the four matrix slabs whose addresses live in 0x00BCADE8,
//      0x00BCAD7C, 0x00BCADAC and 0x00BCADC4;
//   4. fills the D3DVIEWPORT7 mirror at 0x00A7F860 from display width/height (via
//      0x00491CF0 `mov eax, 0x00C28B40; ret`) and the depth range;
//   5. packs the fog colour into 0x00A7E068 from 0x00A7DA30/0x00A7DA34/0x00A7DA38 scaled by
//      -255.0f/-255.0f/+255.0f;
//   6. builds a 4x4 identity world matrix and issues SetTransform(state 1), SetViewport, 17
//      SetRenderState and 10 SetTextureStageState calls on the device in 0x005CA114;
//   7. Lock(0x2020, &0x00A7F89C, NULL) on the vertex buffer in 0x00A7F890, returning on failure;
//   8. clears 0x00A7E078, advances 0x00C28FBC by the block count 0x00B16C9C, copies the camera
//      origin (0x00BCAB08, three doubles) and view matrix (0x00BCAAE0, ten floats) into the block
//      loop's stack slots, and enters the block loop over 0x00B12C60 when the count is positive;
//   9. the block loop builds vertices into the locked buffer, emits triangle indices through the
//      visible-chunk allocator and records the fog/layer pass selection (0x0051305D..0x005143AD);
//  10. Unlock, then three blend-mode passes (Terrain_UnknownBlendmode 0x005144E0) that bind each
//      chunk's texture (0x00514410) and submit DrawIndexedPrimitiveVB(4, vb, 0, 0x00A7E078,
//      chunk+8, chunk[0x2EE8]*3, 0) (0x0051443D) before clearing chunk[0x2EE8];
//  11. drains the remaining visible-chunk list with index remapping and HeapList_Remove, returns 0.
//
// Scope: FULL TRANSCRIPTION (all 1670 instructions): 299 at 0x00512C30..0x00513057, 1279 at
//   0x0051305D..0x005143AD and 92 at 0x005143B3..0x005144DC. The block loop calls
//   Terrain_BuildMipMaps_2 (0x00514760) for clipped/mixed triangles; that callee is transcribed in
//   terrain_mipmaps.cpp and both units share the declarations in terrain_mipmaps.hpp, so this unit
//   does not declare the callee itself. No change to docs/fidelity/native-verification.json is made
//   by this file.
// Status/limits: STRUCTURALLY_VERIFIED (source/object-level property comparison; not
//   CFG-isomorphic). The 17-property comparison in
//   docs/fidelity/evidence/0x00512C30-structural-comparison.json matches the retail disassembly on
//   identity, ABI, state order, block-loop bounds, culling, projection, strip topology, pass
//   iteration, chunk emission, submission and the drain/return paths; the semantic review is
//   docs/fidelity/evidence/0x00512C30-semantic-review.json. Not RUNTIME_TESTED or VERIFIED: no
//   runtime, rendered or memory observation exists; the candidate machine CFG is not isomorphic;
//   x87/fog arithmetic is modelled with long double and libm; the block null guard is ordered
//   differently (TD-D1); missing host device/buffer throws (TD-D2); and shared callees are tracked
//   separately. No change to docs/fidelity/native-verification.json is made by this file.
#include "igi/retail/terrain/terrain_draw.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "igi/retail/mem/heap_list.hpp"
#include "igi/retail/render/view_block.hpp"
#include "igi/retail/terrain/terrain_mipmaps.hpp"
#include "igi/retail/terrain/terrain_out_of_terrain_renderch.hpp"
#include "igi/retail/terrain/terrain_quadtree.hpp"
// Declares the shared device/fog storage this unit reads and writes but no longer defines:
// g_5ca114 (0x005CA114) and g_a7e068 (0x00A7E068), both owned by terrain_storage.cpp.
#include "igi/retail/terrain/terrain_unknown_blendmode.hpp"

namespace igi::retail::terrain
{
namespace
{
// ---------------------------------------------------------------------------------------------
// Float constants read from the retail image (bytes checked with `px`).
// ---------------------------------------------------------------------------------------------
constexpr float kFogScale = 255.0F;    // 0x005336D4 0x437F0000
constexpr float kFogNegScale = -255.0F; // 0x005342D8 0xC37F0000
constexpr float kBlockPadding = 1.73F; // 0x00534C30 0x3FDD70A4, cull radius = 1.73 * extent
constexpr float kBackfaceEpsilon = -0.001F; // 0x00533824 0xBA83126F
// x87 FLDL2E, used by 0x00513475/0x00513477: the retail multiplies by log2(e), splits the
// result with FRNDINT and evaluates 2^frac with F2XM1 before FSCALE. std::exp2 models that
// sequence; its last-ulp rounding need not match F2XM1 exactly.
constexpr long double kLog2E = 1.442695040888963407359924681001892137L;

// 0x00A7F860: D3DVIEWPORT7-shaped mirror (dwX, dwY, dwWidth, dwHeight, dvMinZ, dvMaxZ).
struct Viewport7
{
    std::uint32_t x;
    std::uint32_t y;
    std::uint32_t width;
    std::uint32_t height;
    float min_z;
    float max_z;
};
static_assert(sizeof(Viewport7) == 0x18);

// Stack-slot inputs for the block loop. Retail copies them at
// 0x00512C46..0x00512C7A and 0x00513011..0x0051304C; they are not written to any global.
struct BlockLoopInputs
{
    std::uint32_t view_coefficient[4]; // 0x00BCABD0, 0x00BCABD4, 0x00BCABD8, 0x00BCABDC
    double camera_origin[3];           // 0x00BCAB08, 24 bytes
    float view_matrix[10];             // 0x00BCAAE0, 40 bytes
    float fog_scale;                   // (float)int32 0x00C31088
    std::int32_t block_index;          // loop cursor, starts at 0
    std::int32_t block_count;          // 0x00B16C9C
    float lod_scale;                   // 0x00C28FC0
    const std::uint32_t* block_array;  // 0x00B12C60
};

// Retail's block-loop stack arrays. The low 16 bits of the count word at block+0x18 bound both
// the vertex array at frame+0x500 (0x20 bytes per entry) and the byte arrays at frame+0x1A8 and
// frame+0x200; the next stack object starts 0x800 bytes later, so the retail layout itself only
// holds 64 entries. The command bytes at frame+0x240 are likewise followed by the fourth-pass
// records at frame+0x280, i.e. 64 bytes. These are retail stack-capacity preconditions, not
// invented limits: a block whose counts exceed them would already overrun the retail frame.
constexpr std::int32_t kBlockLoopMaxVertices = 64;
constexpr std::int32_t kBlockLoopMaxCommands = 64;

// One 0x20-byte projected vertex written into the locked vertex buffer.
struct ProjectedVertex
{
    float x;                  // +0x00 screen x, or clip x when flagged 0x10
    float y;                  // +0x04 screen y, or clip y
    float z;                  // +0x08 depth value, or clip z
    float depth;              // +0x0C depth scale, or 0 when clipped
    std::uint32_t colour;     // +0x10 filled per blend pass; vertex buffer diffuse
    std::uint32_t alpha;      // +0x14 0xAARRGGBB; only bits 24..31 are consumed
    float u;                  // +0x18 texture u, filled per blend pass
    float v;                  // +0x1C texture v, filled per blend pass
};
static_assert(sizeof(ProjectedVertex) == 0x20);

// The 0x40-byte argument record and the 0x14-byte state block for Terrain_BuildMipMaps_2
// (0x00514760) are the shared declarations in terrain_mipmaps.hpp: TerrainMipMapsVertex and
// TerrainMipMapsCall. That header pins the same retail layout this unit writes - the first 0x20
// bytes are byte-for-byte ProjectedVertex, +0x20 is the screen-bound flag, +0x24 is written by the
// callee, and +0x28..+0x3C carry component * depth scale - so one declaration serves both the
// caller (this unit) and the callee (terrain_mipmaps.cpp) and the link has no second, ad-hoc copy
// of either record type. The tail is written by this caller only when the flag does not contain
// 0x10; retail feeds frame garbage in the unused bytes, this candidate zero-initialises them (the
// one documented input difference to the callee).

// ---------------------------------------------------------------------------------------------
// Host-owned storage mirroring the retail globals this native reads or writes. Values marked
// "runtime" are zero in the file image and are produced by other retail subsystems.
//
// These definitions deliberately have external linkage. With internal linkage GCC proves at -O3
// that the never-assigned mirrors are still null, so the retail-style raw dereferences below
// (for example 0x00512CE2's `mov dword [slab+4], -1`) become provably undefined behaviour and the
// whole body folds to a trap. External linkage is also the faithful model: these are the retail
// data symbols (the same VAs the quadtree candidate declares `extern`), not private state.
//
// Shared retail state owned by other translation units is declared through terrain_quadtree.hpp
// and defined exactly once elsewhere: g_b16c9c (0x00B16C9C) and g_b12c60 (0x00B12C60) are defined
// in terrain_loaders.cpp. Defining them here as well produced duplicate symbols with that TU.
//
// The nine storage symbols this unit used to define as well now have exactly one owner,
// terrain_storage.cpp (which is in the same igi link): g_5ca114 (0x005CA114), g_a7e068
// (0x00A7E068), g_bcab08 (0x00BCAB08), g_bcaae0 (0x00BCAAE0) and g_a7f878/g_a7f87c/g_a7f880/
// g_a7f888/g_a7f88c (0x00A7F878..0x00A7F88C). Their declarations are unchanged - Device7*
// g_5ca114 and int32_t g_a7e068 through terrain_unknown_blendmode.hpp, the camera/view arrays
// through terrain_quadtree.hpp, and the visible-chunk fields through
// terrain_out_of_terrain_renderch.hpp - so removing the definitions here changes no access or
// type. The single definition is what retail has.
// ---------------------------------------------------------------------------------------------
} // namespace

std::uint32_t g_bcabd0 = 0; // runtime
std::uint32_t g_bcabd4 = 0; // runtime
std::uint32_t g_bcabd8 = 0; // runtime
std::uint32_t g_bcabdc = 0; // runtime
float g_bcad70 = 0.0F;      // runtime, depth scale
float g_bcad8c = 0.0F;      // runtime, far depth
float g_bcada0 = 0.0F;      // runtime, fog ratio numerator
float g_bcadb4 = 0.0F;      // runtime, near depth
float g_bcadc8 = 0.0F;      // runtime, fog ratio denominator
std::uint32_t g_bcad78 = 0; // runtime, value written to render state 7
std::uint8_t* g_bcad7c = nullptr; // runtime, matrix slab address
std::uint8_t* g_bcadac = nullptr; // runtime, matrix slab address
std::uint8_t* g_bcadc4 = nullptr; // runtime, matrix slab address
std::uint8_t* g_bcade8 = nullptr; // runtime, matrix slab address
std::uint32_t g_a7da18 = 0;       // runtime, value written to render state 0x26
float g_a7da30 = 0.0F;            // runtime, fog colour component
float g_a7da34 = 0.0F;            // runtime, fog colour component
float g_a7da38 = 0.0F;            // runtime, fog colour component
std::int32_t g_c31088 = 0;        // runtime, converted to float for the block loop
std::int32_t g_c28fb8 = 0;        // runtime
std::int32_t g_c28fbc = 0;        // runtime, advanced by the block count
float g_c28fc0 = 0.0F;            // runtime, LOD scale
float g_a7e074 = 0.0F;            // fog range offset
float g_a7f894 = 0.0F;            // fog depth scale
std::uint32_t g_a7e078 = 0;       // vertices written into the locked buffer this frame
Viewport7 g_a7f860{};             // 0x00A7F860
std::uint8_t* g_a7f89c = nullptr; // 0x00A7F89C locked vertex data
float g_bcadc0 = 0.0F;            // runtime, near clip plane compared as a float
float g_bcadcc = 0.0F;            // runtime, block cull distance compared as a float
float g_bcabec = 0.0F;            // runtime, fog exponent multiplier
float g_c311a0 = 0.0F;            // runtime, projection scale applied to y
float g_c311a4 = 0.0F;            // runtime, projection scale applied to x
float g_c311b0 = 0.0F;            // runtime, projection offset added to x
float g_c311b4 = 0.0F;            // runtime, projection offset added to y
std::uint32_t g_a7e06c = 0;       // 0x00A7E06C strip winding bit ((block[0x20] >> 2) & 1)
// 0x00A7E070: 32-bit counter, BSS -> 0. It is the only access pair Terrain_BuildMipMaps_2
// (0x00514760) makes to it: entry `mov ecx, dword [0xa7e070]; inc ecx; mov dword [0xa7e070], ecx`
// (0x00514778/0x0051477E/0x00514780), and `decl dword [0xa7e070]` on the reject paths, for example
// 0x005147D4 (all three vertices outside one plane) and 0x00515125 (near-plane reject). The
// dword width is proven by both instructions and the address lies past the file-backed .data
// extent, so the image value is zero. What the pair counts is not established (terrain_mipmaps.hpp
// records the same limitation); the updates are reproduced exactly and no semantic name is
// invented. The declaration is terrain_mipmaps.hpp's `extern std::uint32_t g_a7e070`.
std::uint32_t g_a7e070 = 0;
VertexBuffer7* g_a7f890 = nullptr; // 0x00A7F890 IDirect3DVertexBuffer7*

namespace
{
// 0x00512C30 never executes FLDCW, so FISTP at 0x00512D23/0x00512D52/0x00512D78 runs with the
// inherited control word. Round-to-nearest-even (the x87 default) is assumed; the control word was
// not observed at runtime, so a different rounding mode would shift these conversions by one.
[[nodiscard]] std::int32_t round_to_int32(long double value)
{
    return static_cast<std::int32_t>(std::nearbyint(value));
}

// 0x00512CC5/0x00512CD0/0x00512CD9/0x00512CE2: `mov dword [slab+4], -1`.
void store_minus_one(std::uint8_t* slab)
{
    *reinterpret_cast<std::int32_t*>(slab + 4) = -1;
}

// 0x00BCABD0..0x00BCABDC and 0x00A7DA18 are dword globals whose bit patterns are compared or
// multiplied as floats (for example 0x005134C4 `fcomp dword [esp+0xbc]` and 0x0051346F
// `fmul dword [0xa7da18]`). memcpy keeps the bit pattern without a strict-aliasing assumption.
[[nodiscard]] float bits_to_float(std::uint32_t bits) noexcept
{
    float value;
    std::memcpy(&value, &bits, sizeof value);
    return value;
}

[[nodiscard]] std::uint16_t load_u16(const std::uint8_t* address) noexcept
{
    std::uint16_t value;
    std::memcpy(&value, address, sizeof value);
    return value;
}

[[nodiscard]] std::uint32_t load_u32(const std::uint8_t* address) noexcept
{
    std::uint32_t value;
    std::memcpy(&value, address, sizeof value);
    return value;
}

[[nodiscard]] float load_f32(const std::uint8_t* address) noexcept
{
    float value;
    std::memcpy(&value, address, sizeof value);
    return value;
}

// The 32-bit retail address space: every pointer the terrain block data carries is a 32-bit dword
// (terrain_loaders.cpp stores pointer bits this way). This candidate is built for i686.
[[nodiscard]] const std::uint8_t* pointer_from_bits(std::uint32_t value) noexcept
{
    return reinterpret_cast<const std::uint8_t*>(static_cast<std::uintptr_t>(value));
}

[[nodiscard]] Device7& require_device()
{
    if (g_5ca114 == nullptr || g_5ca114->vtbl == nullptr)
    {
        throw std::logic_error(
            "Terrain_Draw (0x00512C30): no retail IDirect3DDevice7 is installed (global "
            "0x005CA114 is null); call Terrain_HostInstallRenderDevice first.");
    }
    return *g_5ca114;
}

[[nodiscard]] VertexBuffer7& require_vertex_buffer()
{
    if (g_a7f890 == nullptr || g_a7f890->vtbl == nullptr)
    {
        throw std::logic_error(
            "Terrain_Draw (0x00512C30): no retail IDirect3DVertexBuffer7 is installed (global "
            "0x00A7F890 is null); call Terrain_HostInstallRenderDevice first.");
    }
    return *g_a7f890;
}

// 0x005143B3..0x005144DC: Unlock, three blend-mode passes, heap-list drain, return 0.
[[nodiscard]] std::int32_t flush_and_release()
{
    VertexBuffer7& vertex_buffer = require_vertex_buffer();
    vertex_buffer.vtbl->Unlock(&vertex_buffer); // 0x005143BD

    // 0x005143C0..0x0051445B. The count is sampled before the pass callback and the list pointer
    // after it, exactly as retail does at 0x005143C2/0x005143CE.
    for (std::int32_t blendmode = 0; blendmode < 3; ++blendmode)
    {
        std::uint32_t remaining = g_a7f880;
        static_cast<void>(Terrain_UnknownBlendmode(blendmode)); // 0x005143C9
        std::uint32_t* list = g_a7f87c;

        while (remaining != 0)
        {
            const std::uint32_t element_index = list[remaining - 1]; // 0x005143DB
            --remaining;

            const std::uintptr_t element = static_cast<std::uintptr_t>(element_index) * g_a7f888 +
                                           static_cast<std::uintptr_t>(g_a7f878); // 0x005143E0
            auto* const chunk = reinterpret_cast<std::uint8_t*>(element);

            if (*reinterpret_cast<const std::uint32_t*>(chunk + 0x2EE8) == 0) // 0x005143ED
            {
                continue;
            }
            if (*reinterpret_cast<const std::uint32_t*>(chunk + 0x2EF0) !=
                static_cast<std::uint32_t>(blendmode)) // 0x005143F5
            {
                continue;
            }

            Device7& device = require_device();
            const auto* const texture_owner = reinterpret_cast<const std::uint8_t*>(
                static_cast<std::uintptr_t>(*reinterpret_cast<const std::uint32_t*>(chunk + 0x2EEC)));
            void* const texture = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                *reinterpret_cast<const std::uint32_t*>(texture_owner + 0x20))); // 0x00514408
            device.vtbl->SetTexture(&device, 0, texture);                        // 0x00514410

            const std::uint32_t index_count =
                *reinterpret_cast<const std::uint32_t*>(chunk + 0x2EE8) * 3U; // 0x00514416/0x00514423
            device.vtbl->DrawIndexedPrimitiveVB(&device, 4, g_a7f890, 0, g_a7e078,
                reinterpret_cast<const std::uint16_t*>(chunk + 8), index_count, 0); // 0x0051443D

            *reinterpret_cast<std::uint32_t*>(chunk + 0x2EE8) = 0; // 0x00514443
            list = g_a7f87c;                                       // 0x00514449
        }
    }

    // 0x0051445D..0x005144D0: drain the visible-chunk list, remapping indices so that removing an
    // element never leaves a hole.
    std::uint32_t remaining = g_a7f880;
    if (remaining == 0)
    {
        return static_cast<std::int32_t>(remaining); // 0x00514464 -> 0x005144DC, eax = 0
    }

    for (;;)
    {
        std::uint32_t* const list = g_a7f87c;             // 0x0051446E/0x00514474
        const std::uint32_t stride = g_a7f888;            // 0x0051446E
        const std::uint32_t base = g_a7f878;              // 0x0051447C
        const std::uint32_t slot_offset = g_a7f88c;       // 0x00514485
        const std::uintptr_t element =
            static_cast<std::uintptr_t>(*list) * stride + base; // 0x0051447A..0x0051448B
        --remaining;                                          // 0x0051448D
        // 0x0051448E: the slot index lives at element + g_a7f88c (0x2EF4 after the pool
        // initialiser); g_a7f88c is the scalar element size, not a reverse-map base pointer.
        const std::int32_t moved = *reinterpret_cast<const std::int32_t*>(
            static_cast<std::uintptr_t>(slot_offset) + element);
        g_a7f880 = remaining; // 0x00514491

        if (moved != static_cast<std::int32_t>(remaining)) // 0x00514496
        {
            const std::uint32_t last_element =
                list[remaining] * stride + base;                     // 0x0051449A..0x005144A3
            const std::uint32_t moved_list_value =
                list[static_cast<std::uint32_t>(moved)];             // 0x005144A0
            const std::int32_t last_slot =
                *reinterpret_cast<const std::int32_t*>(
                    static_cast<std::uintptr_t>(slot_offset) + last_element); // 0x005144A5
            list[static_cast<std::uint32_t>(moved)] =
                list[static_cast<std::uint32_t>(last_slot)];         // 0x005144A8/0x005144AB
            g_a7f87c[static_cast<std::uint32_t>(last_slot)] = moved_list_value; // 0x005144B4
            // 0x005144B7 reloads 0x00A7F88C; the store is at last_element + that offset.
            *reinterpret_cast<std::int32_t*>(
                static_cast<std::uintptr_t>(g_a7f88c) + last_element) =
                moved;                                              // 0x005144BD
        }

        // 0x005144C1: HeapList_Remove(element). The retail call is `push esi; call 0x004AF960;
        // add esp, 4` and discards the result; the port reuses the transcribed native.
        static_cast<void>(
            igi::retail::mem::HeapList_Remove(reinterpret_cast<void**>(element)));

        remaining = g_a7f880; // 0x005144C6
        if (remaining == 0)
        {
            break;
        }
    }

    return static_cast<std::int32_t>(remaining); // 0x005144D2..0x005144DC, eax = 0
}
} // namespace

void Terrain_HostInstallRenderDevice(Device7* device, VertexBuffer7* vertex_buffer)
{
    g_5ca114 = device;    // retail 0x005CA114
    g_a7f890 = vertex_buffer; // retail 0x00A7F890
}

// 0x00514760: Terrain_BuildMipMaps_2. The declaration is the shared one in terrain_mipmaps.hpp
// and the definition is terrain_mipmaps.cpp (same igi link). It is called twice here, at
// 0x00513EF7 and 0x00514308, with the ABI observed there: three 0x40-byte records passed by value
// and a fourth argument pointing at a five-dword state block (0xC4 bytes of arguments).

std::int32_t Terrain_Draw()
{
    // 0x00512C30: `mov eax, 0x14F0; call 0x004A54A0` -- MSVC 6 __chkstk probes and allocates the
    // 0x14F0-byte frame. The C++ frame replaces it; retail sets up no ebp frame pointer.

    // 0x00512C3A..0x00512CBF: fog depth scale 0x00A7F894 and range offset 0x00A7E074.
    const long double depth_ratio = 1.0L / (1.0L -
        static_cast<long double>(g_bcada0) / static_cast<long double>(g_bcadc8));
    const long double depth_span =
        static_cast<long double>(g_bcad8c) - static_cast<long double>(g_bcadb4);
    g_a7f894 = static_cast<float>(-(depth_span * static_cast<long double>(g_bcad70) *
        static_cast<long double>(g_bcada0) * depth_ratio));
    g_a7e074 = static_cast<float>(depth_span * depth_ratio + static_cast<long double>(g_bcadb4));

    // 0x00512C46..0x00512C7A: the block loop's view coefficients (stack slots only).
    BlockLoopInputs inputs{};
    inputs.view_coefficient[0] = g_bcabd0;
    inputs.view_coefficient[1] = g_bcabd4;
    inputs.view_coefficient[2] = g_bcabd8;
    inputs.view_coefficient[3] = g_bcabdc;

    // 0x00512C77..0x00512CE2: eax = -1; +0x04 of four matrix slabs.
    store_minus_one(g_bcade8);
    store_minus_one(g_bcad7c);
    store_minus_one(g_bcadac);
    store_minus_one(g_bcadc4);

    // 0x00512CE5..0x00512D37: D3DVIEWPORT7. Retail calls 0x00491CF0 twice (`mov eax,
    // 0x00C28B40; ret`) and reads width from +0x04 and height from +0x08.
    g_a7f860.x = 0;
    g_a7f860.y = 0;
    g_a7f860.width = static_cast<std::uint32_t>(render::Display_HostGetWidth());
    g_a7f860.height = static_cast<std::uint32_t>(render::Display_HostGetHeight());
    g_a7f860.min_z = g_bcadb4;
    g_a7f860.max_z = g_bcad8c;

    // 0x00512D03..0x00512D8D: packed fog colour. Components 0x00A7DA30/0x00A7DA34 are scaled by
    // -255.0f and 0x00A7DA38 by +255.0f, each converted with FISTP to an int64 whose low dword is
    // used, then folded as b - (((r << 8) + g) << 8), i.e. r lands in bits 16..23, g in bits 8..15
    // and b in bits 0..7. The negation asymmetry is retail's and is preserved; why two channels are
    // negated is not established. Retail interleaves these stores with the viewport width/height
    // stores; the two groups touch disjoint globals, so the order is not observable.
    const std::int32_t fog_r = round_to_int32(static_cast<long double>(g_a7da30) * kFogNegScale);
    const std::int32_t fog_g = round_to_int32(static_cast<long double>(g_a7da34) * kFogNegScale);
    const std::int32_t fog_b = round_to_int32(static_cast<long double>(g_a7da38) * kFogScale);
    const std::uint32_t fog_middle =
        (static_cast<std::uint32_t>(fog_r) << 8) + static_cast<std::uint32_t>(fog_g);
    g_a7e068 = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(fog_b) - (fog_middle << 8));

    // 0x00512D8D..0x00512DBC: zero 0x40 bytes, then set the 4x4 diagonal at +0x00/+0x14/+0x28/+0x3C.
    float world_matrix[16] = {};
    world_matrix[0] = 1.0F;
    world_matrix[5] = 1.0F;
    world_matrix[10] = 1.0F;
    world_matrix[15] = 1.0F;

    Device7& device = require_device();
    device.vtbl->SetTransform(&device, 1, world_matrix); // 0x00512DCD, state 1
    device.vtbl->SetViewport(&device, &g_a7f860);        // 0x00512DDD

    // 0x00512DEF..0x00512F02: 17 render states in retail order. The state numbers agree with the
    // D3DRS_* numbering (0x88 clipping, 0x26 fog density, 0x23 fog table mode, 0x8C fog vertex
    // mode, 0x30 range fog, 0x24/0x25 fog start/end, 0x2F z bias, 0x1B alpha blend, 0x16 cull
    // mode, 0x89 lighting, 0x8D colour vertex, 0x1A dither, 0x0F alpha test, 0x07 z enable,
    // 0x0E z write, 0x1D specular); the decompilers do not name them, so the names stay
    // unasserted here.
    device.vtbl->SetRenderState(&device, 0x88, 1);
    device.vtbl->SetRenderState(&device, 0x26, g_a7da18);
    device.vtbl->SetRenderState(&device, 0x23, 0);
    device.vtbl->SetRenderState(&device, 0x8C, 3);
    device.vtbl->SetRenderState(&device, 0x30, 1);
    device.vtbl->SetRenderState(&device, 0x24, 0x461C4000U); // 10000.0f
    device.vtbl->SetRenderState(&device, 0x25, 0x46EA6000U); // 30000.0f
    device.vtbl->SetRenderState(&device, 0x2F, 0);
    device.vtbl->SetRenderState(&device, 0x1B, 0);
    device.vtbl->SetRenderState(&device, 0x16, 1);
    device.vtbl->SetRenderState(&device, 0x89, 0);
    device.vtbl->SetRenderState(&device, 0x8D, 1);
    device.vtbl->SetRenderState(&device, 0x1A, 1);
    device.vtbl->SetRenderState(&device, 0x0F, 0);
    device.vtbl->SetRenderState(&device, 0x07, g_bcad78);
    device.vtbl->SetRenderState(&device, 0x0E, 1);
    device.vtbl->SetRenderState(&device, 0x1D, 0);

    // 0x00512F05..0x00512FC2: 10 texture-stage states. Stage 0 gets min/mag/mip filter linear,
    // address 3, texcoord index 0, alpha op 1, colour op 4 with args 2 and 0; stage 1 disables
    // colour op. Values are transcribed literally; the stage-state names are not asserted.
    device.vtbl->SetTextureStageState(&device, 0, 0x11, 2);
    device.vtbl->SetTextureStageState(&device, 0, 0x10, 2);
    device.vtbl->SetTextureStageState(&device, 0, 0x0C, 3);
    device.vtbl->SetTextureStageState(&device, 0, 0x12, 2);
    device.vtbl->SetTextureStageState(&device, 0, 0x0B, 0);
    device.vtbl->SetTextureStageState(&device, 0, 0x04, 1);
    device.vtbl->SetTextureStageState(&device, 0, 0x01, 4);
    device.vtbl->SetTextureStageState(&device, 0, 0x02, 2);
    device.vtbl->SetTextureStageState(&device, 0, 0x03, 0);
    device.vtbl->SetTextureStageState(&device, 1, 0x01, 1);

    // 0x00512FC2..0x00512FE4: clear the frame vertex count and Lock; the HRESULT is the return
    // value when it is non-zero (0x00512FE0 `jne 0x005144D2`).
    g_a7e078 = 0;
    VertexBuffer7& vertex_buffer = require_vertex_buffer();
    const std::int32_t lock_result = vertex_buffer.vtbl->Lock(
        &vertex_buffer, 0x2020U, reinterpret_cast<void**>(&g_a7f89c), nullptr);
    if (lock_result != 0)
    {
        return lock_result;
    }

    // 0x00512FE6..0x00513057: block-loop setup. The global writes happen in retail order
    // (0x00C28FBC at 0x00513013, 0x00C28FB8 at 0x00513032).
    const std::int32_t block_count = g_b16c9c;
    g_c28fbc = g_c28fbc + block_count;
    inputs.fog_scale = static_cast<float>(g_c31088);
    std::memcpy(inputs.camera_origin, g_bcab08, sizeof inputs.camera_origin);
    std::memcpy(inputs.view_matrix, g_bcaae0, sizeof inputs.view_matrix);
    inputs.block_count = block_count;
    inputs.lod_scale = g_c28fc0;
    inputs.block_index = 0;
    inputs.block_array = g_b12c60;
    g_c28fb8 = 0;

    if (block_count <= 0) // 0x00513057 `jle 0x005143B5`
    {
        return flush_and_release();
    }

    // ---------------------------------------------------------------------------------------
    // 0x0051305D..0x005143AD: the terrain block loop.
    //
    // Retail frame regions reproduced by the arrays below (offsets from the post-probe esp):
    //   0x1A8  per-vertex "skip" flag (0 = copy the vertex into the locked buffer, 1 = skip)
    //   0x200  per-vertex screen-bound byte: bit0 sx < left, bit1 sy < top, bit2 sx > right,
    //          bit3 sy > bottom, bit4 clipped against the near plane, bit5 back-facing
    //   0x240  per-command byte: 0 = plain triangle, 0x20 = culled, other value = special record
    //   0x280  three 0x40-byte special records for the fourth chunk section
    //   0x340  three 0x40-byte special records for a blend pass
    //   0x400  per-vertex fog amount (float)
    //   0x500  64 * 0x20 projected vertices
    //   0xD00  64 * 8 dwords colour ramp addressed by the command stream (the 16-bit values in
    //          the per-pass vertex records are dword indices into this ramp)
    //
    // The retail frame is 0x14F0 bytes, so the low-16-bit vertex count at block+0x18 must be at
    // most 64 for the vertex/flag arrays and the command count at header+0xC at most 64 for the
    // command bytes; those are the retail stack capacities, not new limits. Likewise the colour
    // ramp indices and the stitch-record reference/fog indices are block-supplied values that the
    // retail code indexes without a bound check.
    // ---------------------------------------------------------------------------------------
    const float camera_x = static_cast<float>(inputs.camera_origin[0]); // 0x0051305D
    const float camera_y = static_cast<float>(inputs.camera_origin[1]); // 0x0051306B
    const float camera_z = static_cast<float>(inputs.camera_origin[2]); // 0x00513079
    const float negative_camera_x = -camera_x; // 0x005130B2
    const float negative_camera_y = -camera_y; // 0x005130C9
    const float negative_camera_z = -camera_z; // 0x005130D3

    const float* const view = inputs.view_matrix; // frame+0xDC, ten floats
    const float left_bound = bits_to_float(inputs.view_coefficient[0]);   // frame+0xBC
    const float top_bound = bits_to_float(inputs.view_coefficient[1]);    // frame+0xC0
    const float right_bound = bits_to_float(inputs.view_coefficient[2]);  // frame+0xC8
    const float bottom_bound = bits_to_float(inputs.view_coefficient[3]); // frame+0xCC
    const float near_clip = g_bcadc0;
    const float block_cull_limit = g_bcadcc;
    const float fog_density = bits_to_float(g_a7da18);

    for (std::int32_t block_index = 0; block_index < block_count; ++block_index) // 0x0051308B
    {
        const std::uint8_t* const block = pointer_from_bits(inputs.block_array[block_index]);
        if (block == nullptr) // 0x005130E0
        {
            continue;
        }

        // 0x005130F0..0x0051314A: the word at block+0x18 holds the vertex count in its low 16
        // bits and the first-loop count (signed) in its high 16 bits; block+0x1C is the LOD.
        const std::int32_t vertex_count_total =
            static_cast<std::int32_t>(load_u16(block + 0x18));
        const std::int32_t vertex_count_first =
            static_cast<std::int32_t>(static_cast<std::int16_t>(load_u16(block + 0x1A)));
        const std::int32_t lod = static_cast<std::int32_t>(load_u32(block + 0x1C));

        const std::uint32_t subdivisions = std::uint32_t{1} << ((25 - lod) & 31);
        const float inverse_subdivisions = 0.375F / static_cast<float>(subdivisions);
        const std::int32_t cull_extent = 0x40000000 >> (lod & 31);

        // 0x0051314E..0x005131B6: cull on the block origin's distance along the view axis.
        const double* const origin = reinterpret_cast<const double*>(block);
        const long double origin_dx =
            static_cast<long double>(origin[0]) - static_cast<long double>(inputs.camera_origin[0]);
        const long double origin_dy =
            static_cast<long double>(origin[1]) - static_cast<long double>(inputs.camera_origin[1]);
        const long double origin_dz =
            static_cast<long double>(origin[2]) - static_cast<long double>(inputs.camera_origin[2]);
        const long double view_distance =
            (static_cast<long double>(view[8]) * origin_dz +
                static_cast<long double>(view[7]) * origin_dy) +
            static_cast<long double>(view[6]) * origin_dx;
        const long double cull_radius =
            static_cast<long double>(cull_extent) * static_cast<long double>(kBlockPadding);
        if (view_distance + cull_radius >= static_cast<long double>(block_cull_limit))
        {
            continue; // 0x005131B6 `je 0x0051438A`
        }

        ProjectedVertex vertices[kBlockLoopMaxVertices] = {};
        float fog_amount[kBlockLoopMaxVertices] = {};
        std::uint8_t copy_flag[kBlockLoopMaxVertices] = {};
        std::uint8_t screen_flag[kBlockLoopMaxVertices] = {};
        std::uint8_t command_type[kBlockLoopMaxCommands] = {};
        std::uint32_t colour_ramp[kBlockLoopMaxVertices * 8] = {};

        std::uint32_t fog_pass_bits = 0; // frame+0x48
        bool any_clipped = false;        // frame+0x28 during the projection loops
        const std::uint8_t* vertex_record = block + 0x20;

        // 0x00513373..0x00513544 + 0x00513698..0x0051385F: shared transform, near clip,
        // perspective divide, depth/alpha and the four screen-bound tests. The retail transform
        // reads the vertex deltas as floats but keeps the products in x87 registers; the clip
        // test uses the unrounded Z' while the divide and alpha read the stored float Z'.
        const auto project = [&](std::int32_t index, long double dx, long double dy, long double dz)
            -> std::uint8_t
        {
            const long double clip_x =
                (dz * static_cast<long double>(view[2]) +
                    dy * static_cast<long double>(view[1])) +
                dx * static_cast<long double>(view[0]);
            const long double clip_y =
                (dz * static_cast<long double>(view[5]) +
                    dy * static_cast<long double>(view[4])) +
                dx * static_cast<long double>(view[3]);
            const long double clip_z =
                (dz * static_cast<long double>(view[8]) +
                    dy * static_cast<long double>(view[7])) +
                dx * static_cast<long double>(view[6]);
            const float clip_x_float = static_cast<float>(clip_x);
            const float clip_y_float = static_cast<float>(clip_y);
            const float clip_z_float = static_cast<float>(clip_z);

            if (clip_z < static_cast<long double>(near_clip)) // 0x005133E3
            {
                vertices[index].x = clip_x_float;
                vertices[index].y = clip_y_float;
                vertices[index].z = clip_z_float;
                vertices[index].depth = 0.0F;
                vertices[index].alpha = 0xFF000000U;
                any_clipped = true; // 0x00513549
                return 0x10U;       // 0x0051358B
            }

            const long double inverse = 1.0L / static_cast<long double>(clip_z_float);
            const long double screen_x =
                inverse * static_cast<long double>(clip_x_float) *
                    static_cast<long double>(g_c311a4) +
                static_cast<long double>(g_c311b0); // 0x0051340E..0x0051341B
            const long double screen_y =
                inverse * static_cast<long double>(clip_y_float) *
                    static_cast<long double>(g_c311a0) +
                static_cast<long double>(g_c311b4); // 0x00513421..0x00513432

            vertices[index].x = static_cast<float>(screen_x);
            vertices[index].y = static_cast<float>(screen_y);
            vertices[index].z = static_cast<float>(
                inverse * static_cast<long double>(g_a7f894) + static_cast<long double>(g_a7e074));
            vertices[index].depth =
                static_cast<float>(inverse * static_cast<long double>(g_bcad70));

            // 0x00513462..0x005134BF: alpha = round(255 / exp(Z' * 0x00BCABEC * 0x00A7DA18)).
            // exp2 models the FRNDINT/F2XM1/FSCALE sequence; see the kLog2E note above.
            const long double exponent =
                static_cast<long double>(clip_z_float) * static_cast<long double>(g_bcabec) *
                static_cast<long double>(fog_density);
            const long double exponential = std::exp2(exponent * kLog2E);
            long double opacity = 1.0L / exponential;
            if (opacity < 0.0L) // 0x00513491..0x005134A0
            {
                opacity = 0.0L;
            }
            // 0x005134B4 is a 64-bit FISTP; the following `mov eax, [esp+0x4C]` takes the low
            // dword before `shl eax, 0x18`. llrint keeps that 64-bit rounding for out-of-range
            // values where a 32-bit conversion would differ.
            const std::int64_t rounded_alpha = std::llrint(
                opacity * static_cast<long double>(kFogScale));
            vertices[index].alpha =
                static_cast<std::uint32_t>(static_cast<std::uint64_t>(rounded_alpha)) << 24;

            std::uint8_t flags = 0;
            if (screen_x < static_cast<long double>(left_bound))   // 0x005134C4
            {
                flags |= 0x01U;
            }
            if (screen_y < static_cast<long double>(top_bound))    // 0x005134DA
            {
                flags |= 0x02U;
            }
            if (screen_x > static_cast<long double>(right_bound))  // 0x005134F8
            {
                flags |= 0x04U;
            }
            if (screen_y > static_cast<long double>(bottom_bound)) // 0x00513513
            {
                flags |= 0x08U;
            }
            return flags;
        };

        // 0x00513D2F..0x00513DDA + 0x00514135..0x005141E9: build one 0x40-byte special record.
        // The vertex bytes are copied verbatim, +0x20 receives the screen-bound flag, and unless
        // the flag carries 0x10 the +0x08 slot is replaced with the depth scale and the remaining
        // tail is filled from the vertex attributes. R uses the unrounded depth*0x00BCABEC while
        // G/B/u/v/A use the float-rounded value (the retail reloads the +0x08 store for those).
        const auto make_mipmaps_record = [](const ProjectedVertex& vertex, std::uint8_t flag)
            -> TerrainMipMapsVertex
        {
            TerrainMipMapsVertex record{};
            // +0x00..0x1F: the 0x20-byte ProjectedVertex copy (rep movsd in retail).
            record.x = vertex.x;
            record.y = vertex.y;
            record.z = vertex.z;
            record.depth = vertex.depth;
            record.colour = vertex.colour;
            record.alpha = vertex.alpha;
            record.u = vertex.u;
            record.v = vertex.v;
            record.flag = flag;
            if ((flag & 0x10U) == 0) // 0x00513D50
            {
                const long double depth_wide =
                    static_cast<long double>(vertex.depth) * static_cast<long double>(g_bcabec);
                const float depth_scale = static_cast<float>(depth_wide);
                record.z = depth_scale;
                record.red = static_cast<float>(
                    static_cast<long double>((vertex.colour >> 16) & 0xFFU) * depth_wide);
                record.green = static_cast<float>(
                    static_cast<long double>((vertex.colour >> 8) & 0xFFU) *
                    static_cast<long double>(depth_scale));
                record.blue = static_cast<float>(
                    static_cast<long double>(vertex.colour & 0xFFU) *
                    static_cast<long double>(depth_scale));
                record.tex_u = static_cast<float>(static_cast<long double>(vertex.u) *
                    static_cast<long double>(depth_scale));
                record.tex_v = static_cast<float>(static_cast<long double>(vertex.v) *
                    static_cast<long double>(depth_scale));
                record.alpha_f = static_cast<float>(
                    static_cast<long double>(vertex.alpha >> 24) *
                    static_cast<long double>(depth_scale));
            }
            return record;
        };

        // 0x005131ED..0x00513602: first projection loop. Retail is a rotated loop, so one record
        // is always consumed even when vertex_count_first is zero.
        std::int32_t vertex_index = 0;
        do
        {
            const float vertex_x = load_f32(vertex_record + 0x00);
            const float vertex_y = load_f32(vertex_record + 0x04);
            const float vertex_z = load_f32(vertex_record + 0x08);
            const float vertex_w = load_f32(vertex_record + 0x0C);
            vertex_record += 0x10;

            const long double dx = static_cast<long double>(vertex_x) +
                static_cast<long double>(negative_camera_x);
            const long double dy = static_cast<long double>(vertex_y) +
                static_cast<long double>(negative_camera_y);
            const long double dz = static_cast<long double>(vertex_z) +
                static_cast<long double>(negative_camera_z);
            const long double max_abs =
                std::max(std::max(std::fabs(dx), std::fabs(dy)), std::fabs(dz));

            // 0x00513292..0x00513302: fog = clamp((max|d| * i2 - fogScale) * lodScale, 0, 1).
            long double fog = (max_abs * static_cast<long double>(inverse_subdivisions) -
                                  static_cast<long double>(inputs.fog_scale)) *
                              static_cast<long double>(inputs.lod_scale);
            if (fog < 0.0L)
            {
                fog = 0.0L;
            }
            else if (fog > 1.0L)
            {
                fog = 1.0L;
            }
            fog_amount[vertex_index] = static_cast<float>(fog);
            if (fog > 0.01) // 0x005132D8; 0.01 is a double at 0x00533720
            {
                fog_pass_bits |= 0x02U;
            }
            if (fog < 0.99) // 0x005132EF; 0.99 is a double at 0x00534C28
            {
                fog_pass_bits |= 0x01U;
            }

            // 0x00513323..0x00513371: the colour ramp entries are derived from round(255*(1-fog))
            // and round(255*(1-fog)^2), each replicated to r=g=b.
            const long double one_minus_fog = 1.0L - fog;
            const long double c1_wide = one_minus_fog * static_cast<long double>(kFogScale);
            const std::int32_t c1 = round_to_int32(c1_wide);
            const std::int32_t c2 = round_to_int32(c1_wide * one_minus_fog);
            const std::uint32_t c1_packed = static_cast<std::uint32_t>(c1) * 0x010101U;
            const std::uint32_t c2_packed = static_cast<std::uint32_t>(c2) * 0x010101U;
            const std::uint32_t inverted_c1 = 0xFFFFFFU - c1_packed;

            // 0x00513306..0x0051331C: the fog shift is added to z and rounded to float before the
            // camera-space subtraction.
            const float fog_shifted_z = static_cast<float>(
                static_cast<long double>(vertex_z) +
                one_minus_fog * static_cast<long double>(vertex_w));
            const long double fog_dz = static_cast<long double>(fog_shifted_z) +
                static_cast<long double>(negative_camera_z);

            screen_flag[vertex_index] = project(vertex_index, dx, dy, fog_dz);

            const std::size_t ramp = static_cast<std::size_t>(vertex_index) * 8;
            colour_ramp[ramp + 0] = 0; // frame+0xD00: 0x005135A6
            colour_ramp[ramp + 1] = 0; // frame+0xD04: 0x005135A0
            colour_ramp[ramp + 2] = c1_packed - c2_packed;
            colour_ramp[ramp + 3] = 0xFFFFFFU - 2U * c1_packed + c2_packed;
            colour_ramp[ramp + 4] = c2_packed;
            colour_ramp[ramp + 5] = c1_packed - c2_packed;
            colour_ramp[ramp + 6] = c1_packed;
            colour_ramp[ramp + 7] = inverted_c1;
            copy_flag[vertex_index] = 1; // 0x005135EA
            ++vertex_index;
        } while (vertex_index < vertex_count_first);

        // 0x0051360C..0x005138AE: stitch records. Each one lerps x/y/z towards the vertex named
        // by its +0x0C dword using that vertex's fog amount, then projects with the plain camera
        // deltas (no fog z shift). x keeps the x87 result while y and z are rounded to float.
        if (vertex_index != vertex_count_total)
        {
            do
            {
                const float vertex_x = load_f32(vertex_record + 0x00);
                const float vertex_y = load_f32(vertex_record + 0x04);
                const float vertex_z = load_f32(vertex_record + 0x08);
                const std::uint32_t reference = load_u32(vertex_record + 0x0C);
                vertex_record += 0x10;

                const std::uint8_t* const reference_vertex =
                    block + 0x20 + static_cast<std::size_t>(reference) * 0x10;
                const float weight = fog_amount[static_cast<std::size_t>(reference)];
                const long double lerp_x = static_cast<long double>(vertex_x) +
                    (static_cast<long double>(load_f32(reference_vertex + 0x00)) -
                        static_cast<long double>(vertex_x)) *
                        static_cast<long double>(weight);
                const float lerp_y = static_cast<float>(static_cast<long double>(vertex_y) +
                    (static_cast<long double>(load_f32(reference_vertex + 0x04)) -
                        static_cast<long double>(vertex_y)) *
                        static_cast<long double>(weight));
                const float lerp_z = static_cast<float>(static_cast<long double>(vertex_z) +
                    (static_cast<long double>(load_f32(reference_vertex + 0x08)) -
                        static_cast<long double>(vertex_z)) *
                        static_cast<long double>(weight));

                const long double dx = lerp_x + static_cast<long double>(negative_camera_x);
                const long double dy = static_cast<long double>(lerp_y) +
                    static_cast<long double>(negative_camera_y);
                const long double dz = static_cast<long double>(lerp_z) +
                    static_cast<long double>(negative_camera_z);
                screen_flag[vertex_index] = project(vertex_index, dx, dy, dz);
                copy_flag[vertex_index] = 1; // 0x005138A4
                ++vertex_index;
            } while (vertex_index < vertex_count_total);
        }

        // 0x005138B4..0x00513919: block tail header, command count, layer count and winding bit.
        const std::uint8_t* const header = vertex_record;
        const std::uint32_t command_bits = load_u32(header + 0x00);
        const std::uint32_t* const commands = reinterpret_cast<const std::uint32_t*>(
            pointer_from_bits(load_u32(header + 0x04)));
        const std::int32_t layer_count = static_cast<std::int32_t>(load_u32(header + 0x08));
        const std::int32_t command_count = static_cast<std::int32_t>(load_u32(header + 0x0C));
        g_a7e06c = (command_bits >> 2) & 1U; // 0x005138C0
        const std::uint8_t* pass_data = header + 0x10;

        bool chunk_created = false; // frame+0x38 (0x00513B59)
        if (any_clipped)
        {
            // 0x00513B0A..0x00513B55: every command takes the special-record path (0x10 has
            // bit 0x20 clear) and every projected vertex is written into the locked buffer.
            for (std::int32_t index = 0; index < command_count; ++index)
            {
                command_type[index] = 0x10;
            }
            for (std::int32_t index = 0; index < vertex_count_total; ++index)
            {
                copy_flag[index] = 0;
            }
        }
        else
        {
            // 0x005138E6..0x00513B08: strip topology. The command dword holds the newest vertex
            // in byte 0 and (only for the first triangle after a restart) the other two vertices
            // in bytes 1 and 2; subsequent triangles carry the previous two vertices. Bit
            // 0x10000000 restarts the strip. A triangle is front-facing when area * winding <
            // -0.001; only front-facing triangles clear their three skip flags and clear bit 5.
            const float wire_winding = (g_a7e06c != 0) ? -1.0F : 1.0F; // frame+0x90
            float winding = wire_winding;                              // frame+0x4C
            std::int32_t strip_state = 0;                              // frame+0x18
            std::uint32_t draw_accumulator = 0xFFFFFFFFU;              // frame+0x28
            std::uint8_t* type_cursor = command_type;                  // frame+0x10
            const std::uint32_t* command_cursor = commands;
            const std::uint32_t* const command_end = commands + command_count;

            float previous_x0 = 0.0F; // frame+0x68
            float previous_y0 = 0.0F; // frame+0x6C
            float previous_y1 = 0.0F; // frame+0x64
            float carry_x = 0.0F;     // the x87 carry (frame+0x24)
            std::int32_t previous_i0_flag = 0; // [frame+0x2F]
            std::int32_t previous_i1_flag = 0; // bl
            std::uint8_t* previous_i0_state = nullptr;
            std::uint8_t* previous_i1_state = nullptr;

            do // 0x0051394E (rotated: command_count == 0 still processes one command)
            {
                const std::uint32_t command = *command_cursor;
                ++command_cursor;

                // 0x0051394A..0x00513988: rotate the carried strip state.
                float i1_x = previous_x0;
                float i1_y = previous_y0;
                float i2_x = carry_x;
                float i2_y = previous_y1;
                std::int32_t i1_flag = previous_i0_flag;
                std::int32_t i2_flag = previous_i1_flag;
                std::uint8_t* i0_state = nullptr;
                std::uint8_t* i1_state = previous_i0_state;
                std::uint8_t* i2_state = previous_i1_state;

                if (strip_state == 0) // 0x0051398A
                {
                    i2_x = vertices[(command >> 16) & 0xFFU].x;
                    i2_y = vertices[(command >> 16) & 0xFFU].y;
                    i2_flag = screen_flag[(command >> 16) & 0xFFU];
                    i2_state = &copy_flag[(command >> 16) & 0xFFU];
                    i1_x = vertices[(command >> 8) & 0xFFU].x;
                    i1_y = vertices[(command >> 8) & 0xFFU].y;
                    i1_flag = screen_flag[(command >> 8) & 0xFFU];
                    i1_state = &copy_flag[(command >> 8) & 0xFFU];
                }

                const std::int32_t i0 = static_cast<std::int32_t>(command & 0xFFU);
                const float i0_x = vertices[i0].x;
                const float i0_y = vertices[i0].y;
                i0_state = &copy_flag[i0];
                const std::int32_t i0_flag = screen_flag[i0];

                // 0x00513A2E..0x00513A45: OR the three flags and set bit 5 unless all three
                // share a flag bit.
                std::uint32_t type =
                    static_cast<std::uint32_t>(i0_flag | i1_flag | i2_flag) | 0x20U;
                std::uint32_t draw_flag = 1; // frame+0x54
                if ((i0_flag & i1_flag & i2_flag) == 0)
                {
                    const long double area =
                        (static_cast<long double>(i1_x) - static_cast<long double>(i0_x)) *
                            (static_cast<long double>(i2_y) - static_cast<long double>(i0_y)) -
                        (static_cast<long double>(i2_x) - static_cast<long double>(i0_x)) *
                            (static_cast<long double>(i1_y) - static_cast<long double>(i0_y));
                    if (area * static_cast<long double>(winding) <
                        static_cast<long double>(kBackfaceEpsilon)) // 0x00513A7D
                    {
                        *i0_state = 0; // 0x00513A83
                        *i1_state = 0; // 0x00513A86
                        *i2_state = 0; // 0x00513A8C
                        type ^= 0x20U; // 0x00513A89
                        draw_flag = 0; // 0x00513A8F
                    }
                }

                *type_cursor = static_cast<std::uint8_t>(type); // 0x00513AA9
                ++type_cursor;
                draw_accumulator &= draw_flag; // 0x00513ABC
                winding = -winding;            // 0x00513A9F..0x00513AB8
                ++strip_state;                 // 0x00513AC2
                if ((command & 0x10000000U) != 0) // 0x00513AC7
                {
                    strip_state = 0;
                    winding = wire_winding;
                }

                previous_i0_flag = i0_flag;
                previous_i1_flag = i1_flag;
                previous_y1 = i1_y;
                previous_y0 = i0_y;
                previous_x0 = i0_x;
                previous_i0_state = i0_state;
                previous_i1_state = i1_state;
                carry_x = i1_x;
            } while (command_cursor < command_end);

            if (draw_accumulator != 0) // 0x00513B02
            {
                continue; // no front-facing triangle in this block
            }
        }

        // 0x00513B61..0x00513FC0: blend passes. The fog bits only ever contain 0x1 and 0x2
        // (0x005132E9/0x00513300), so pass 3 never selects; the layer loop still consumes all
        // three passes of data and the pass loop is rotated (pass 1 runs even for an empty layer
        // count, which the data layout precludes).
        std::int32_t layer = 0; // frame+0x20
        do
        {
            std::int32_t pass = 1; // frame+0x48
            do
            {
                if ((fog_pass_bits & static_cast<std::uint32_t>(pass)) != 0) // 0x00513B6E
                {
                    const std::uint32_t texture_bits = load_u32(pass_data);
                    pass_data += 4;
                    const std::uint32_t base_vertex = g_a7e078;    // frame+0x34
                    std::uint8_t* const destination = g_a7f89c;    // frame+0x10
                    std::int32_t pass_vertex_count = 0;            // frame+0x14
                    std::int32_t index = 0;
                    do // 0x00513BA8
                    {
                        if (copy_flag[index] == 0) // 0x00513BB1
                        {
                            ProjectedVertex& vertex = vertices[index];
                            vertex.colour = colour_ramp[
                                static_cast<std::size_t>(load_u32(pass_data) & 0xFFFFU)];
                            vertex.u = load_f32(pass_data + 0x04);
                            vertex.v = load_f32(pass_data + 0x08);
                            std::memcpy(destination + static_cast<std::size_t>(index) * 0x20,
                                &vertex, sizeof vertex);
                        }
                        pass_data += 0x0C;
                        ++index;
                    } while (index < vertex_count_first);

                    if (index != vertex_count_total) // 0x00513BFD
                    {
                        do // 0x00513C03
                        {
                            if (copy_flag[index] == 0) // 0x00513C0C
                            {
                                const std::uint32_t code = load_u32(pass_data);
                                const std::int32_t fog_index =
                                    static_cast<std::int32_t>(code) >> 16;
                                const float weight =
                                    fog_amount[static_cast<std::size_t>(fog_index)];
                                ProjectedVertex& vertex = vertices[index];
                                vertex.colour = colour_ramp[
                                    static_cast<std::size_t>(code & 0xFFFFU)];
                                vertex.u = static_cast<float>(
                                    static_cast<long double>(weight) *
                                        static_cast<long double>(load_f32(pass_data + 0x0C)) +
                                    static_cast<long double>(load_f32(pass_data + 0x04)));
                                vertex.v = static_cast<float>(
                                    static_cast<long double>(weight) *
                                        static_cast<long double>(load_f32(pass_data + 0x10)) +
                                    static_cast<long double>(load_f32(pass_data + 0x08)));
                                std::memcpy(destination + static_cast<std::size_t>(index) * 0x20,
                                    &vertex, sizeof vertex);
                            }
                            pass_data += 0x14;
                            ++index;
                        } while (index < vertex_count_total);
                    }
                    pass_vertex_count = index;

                    // 0x00513C70..0x00513CA8: the first chunk of this block uses pass 0, every
                    // later chunk (including later layers) uses pass 1.
                    const void* const texture = reinterpret_cast<const void*>(
                        static_cast<std::uintptr_t>(texture_bits));
                    TerrainRenderChunk* chunk = nullptr;
                    if (!chunk_created)
                    {
                        chunk = Terrain_OutOfTerrainRenderch(texture, 0);
                        chunk_created = true;
                    }
                    else
                    {
                        chunk = Terrain_OutOfTerrainRenderch(texture, 1);
                    }

                    // 0x00513CAC..0x00513F7A: emit the pass's triangles into the chunk's index
                    // buffer and advance the global vertex cursor by the (possibly callee-
                    // adjusted) vertex count.
                    std::uint8_t* const vertex_output = g_a7f89c;
                    std::uint16_t* const index_base =
                        reinterpret_cast<std::uint16_t*>(chunk->indices) +
                        3U * chunk->triangle_count;
                    std::uint16_t* index_cursor = index_base;
                    std::int32_t emitted = 0;
                    const std::uint8_t* type_cursor = command_type;
                    const std::uint32_t* command_cursor = commands;
                    const std::uint32_t* const command_end = commands + command_count;
                    do // 0x00513CFA (rotated)
                    {
                        const std::uint8_t type = *type_cursor;
                        ++type_cursor;
                        if (type == 0) // 0x00513D07
                        {
                            const std::uint32_t command = *command_cursor;
                            const std::uint32_t packed_base = base_vertex * 0x10001U;
                            const std::uint32_t low_pair =
                                (command & 0x00FF00FFU) + packed_base;
                            std::memcpy(index_cursor, &low_pair, sizeof low_pair);
                            index_cursor[2] = static_cast<std::uint16_t>(
                                ((command >> 8) & 0xFFU) + base_vertex);
                            index_cursor += 3;
                            ++emitted;
                        }
                        else if ((type & 0x20U) == 0) // 0x00513D29
                        {
                            const std::uint32_t command = *command_cursor;
                            const std::uint16_t order[3] = {
                                static_cast<std::uint16_t>(command & 0xFFU),
                                static_cast<std::uint16_t>((command >> 16) & 0xFFU),
                                static_cast<std::uint16_t>((command >> 8) & 0xFFU)};
                            std::int32_t triangles = emitted;
                            TerrainMipMapsVertex records[3];
                            for (std::int32_t element = 0; element < 3; ++element)
                            {
                                records[element] = make_mipmaps_record(
                                    vertices[order[element]], screen_flag[order[element]]);
                            }
                            TerrainMipMapsCall call{vertex_output, &pass_vertex_count, index_base,
                                &triangles, base_vertex};
                            static_cast<void>(
                                Terrain_BuildMipMaps_2(records[0], records[1], records[2], &call));
                            emitted = triangles;
                            index_cursor = index_base + 3U * static_cast<std::uint32_t>(emitted);
                        }
                        ++command_cursor;
                    } while (command_cursor < command_end);

                    chunk->triangle_count += static_cast<std::uint32_t>(emitted); // 0x00513F53
                    g_a7f89c += static_cast<std::ptrdiff_t>(pass_vertex_count) * 0x20; // 0x00513F74
                    g_a7e078 += static_cast<std::uint32_t>(pass_vertex_count);        // 0x00513F7A
                }
                else
                {
                    // 0x00513F86: skip the pass's vertex block without touching the globals.
                    pass_data += static_cast<std::ptrdiff_t>(4) +
                        static_cast<std::ptrdiff_t>(vertex_count_total) * 0x14 -
                        static_cast<std::ptrdiff_t>(vertex_count_first) * 0x08;
                }
                ++pass; // 0x00513FA3
            } while (pass < 3);
            ++layer; // 0x00513FB1
        } while (layer < layer_count);

        // 0x00513FC6..0x00514388: fourth chunk section. A non-zero texture pointer selects an
        // unconditional fourth pass (chunk pass 2) whose vertices use a constant white diffuse.
        const std::uint32_t fourth_texture_bits = load_u32(pass_data);
        pass_data += 4;
        if (fourth_texture_bits == 0) // 0x00513FD2
        {
            continue;
        }

        const std::uint32_t base_vertex = g_a7e078; // frame+0x48
        std::uint8_t* const destination = g_a7f89c; // frame+0x4C
        std::int32_t pass_vertex_count = 0;         // frame+0x14
        std::int32_t index = 0;
        do // 0x00514000
        {
            if (copy_flag[index] == 0) // 0x00514009
            {
                ProjectedVertex& vertex = vertices[index];
                vertex.colour = 0xFFFFFFU; // 0x00514026
                vertex.u = load_f32(pass_data + 0x04);
                vertex.v = load_f32(pass_data + 0x08);
                std::memcpy(destination + static_cast<std::size_t>(index) * 0x20, &vertex,
                    sizeof vertex);
            }
            pass_data += 0x0C;
            ++index;
        } while (index < vertex_count_first);

        if (index != vertex_count_total) // 0x00514049
        {
            do // 0x0051404F
            {
                if (copy_flag[index] == 0) // 0x00514058
                {
                    const std::uint32_t code = load_u32(pass_data);
                    const std::int32_t fog_index = static_cast<std::int32_t>(code) >> 16;
                    const float weight = fog_amount[static_cast<std::size_t>(fog_index)];
                    ProjectedVertex& vertex = vertices[index];
                    vertex.colour = 0xFFFFFFU;
                    vertex.u = static_cast<float>(
                        static_cast<long double>(weight) *
                            static_cast<long double>(load_f32(pass_data + 0x0C)) +
                        static_cast<long double>(load_f32(pass_data + 0x04)));
                    vertex.v = static_cast<float>(
                        static_cast<long double>(weight) *
                            static_cast<long double>(load_f32(pass_data + 0x10)) +
                        static_cast<long double>(load_f32(pass_data + 0x08)));
                    std::memcpy(destination + static_cast<std::size_t>(index) * 0x20, &vertex,
                        sizeof vertex);
                }
                pass_data += 0x14;
                ++index;
            } while (index < vertex_count_total);
        }
        pass_vertex_count = index;

        const void* const fourth_texture = reinterpret_cast<const void*>(
            static_cast<std::uintptr_t>(fourth_texture_bits));
        TerrainRenderChunk* const chunk = Terrain_OutOfTerrainRenderch(fourth_texture, 2);

        std::uint8_t* const vertex_output = g_a7f89c;
        std::uint16_t* const index_base =
            reinterpret_cast<std::uint16_t*>(chunk->indices) + 3U * chunk->triangle_count;
        std::uint16_t* index_cursor = index_base;
        std::int32_t emitted = 0;
        const std::uint8_t* type_cursor = command_type;
        const std::uint32_t* command_cursor = commands;
        const std::uint32_t* const command_end = commands + command_count;
        do // 0x00514113 (rotated)
        {
            const std::uint8_t type = *type_cursor;
            ++type_cursor;
            if (type == 0)
            {
                const std::uint32_t command = *command_cursor;
                const std::uint32_t packed_base = base_vertex * 0x10001U;
                const std::uint32_t low_pair = (command & 0x00FF00FFU) + packed_base;
                std::memcpy(index_cursor, &low_pair, sizeof low_pair);
                index_cursor[2] = static_cast<std::uint16_t>(
                    ((command >> 8) & 0xFFU) + base_vertex);
                index_cursor += 3;
                ++emitted;
            }
            else if ((type & 0x20U) == 0)
            {
                const std::uint32_t command = *command_cursor;
                const std::uint16_t order[3] = {
                    static_cast<std::uint16_t>(command & 0xFFU),
                    static_cast<std::uint16_t>((command >> 16) & 0xFFU),
                    static_cast<std::uint16_t>((command >> 8) & 0xFFU)};
                std::int32_t triangles = emitted;
                TerrainMipMapsVertex records[3];
                for (std::int32_t element = 0; element < 3; ++element)
                {
                    records[element] = make_mipmaps_record(
                        vertices[order[element]], screen_flag[order[element]]);
                }
                TerrainMipMapsCall call{vertex_output, &pass_vertex_count, index_base, &triangles,
                    base_vertex};
                static_cast<void>(
                    Terrain_BuildMipMaps_2(records[0], records[1], records[2], &call));
                emitted = triangles;
                index_cursor = index_base + 3U * static_cast<std::uint32_t>(emitted);
            }
            ++command_cursor;
        } while (command_cursor < command_end);

        chunk->triangle_count += static_cast<std::uint32_t>(emitted);
        g_a7f89c += static_cast<std::ptrdiff_t>(pass_vertex_count) * 0x20;
        g_a7e078 += static_cast<std::uint32_t>(pass_vertex_count);
    }

    return flush_and_release();
}
} // namespace igi::retail::terrain
