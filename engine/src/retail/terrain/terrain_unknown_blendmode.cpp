// Native: Terrain_UnknownBlendmode; retail VA 0x005144E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP 0.1.0 (single selected decompiler), radare2 6.2.0 CFG and angr 10.0.4.post1 CFGFast on retail IGI.EXE; evidence docs/fidelity/evidence/0x005144E0-*.json; no Ghidra analysis was run for this target.
// Behavior/CFG: modes 0/1/2 each reload 0x005CA114 and call device vtable +0x50 and +0x94, returning the last +0x50 HRESULT; any other mode reports "Unknown blendmode" through 0x004AF7B0 and does not return. Binary Ninja and radare2 recover the same retail graph.
// Status/limits: STRUCTURALLY_VERIFIED; not wired into CMakeLists.txt; no runtime capture exists. The null-device and scope limits are detailed below.
//
// Retail structure (from instructions, not decompiler naming)
//   0x005144E0  mov eax, [esp+4]                  ; mode
//   0x005144E4  sub eax, 0 / 0x005144E7 je 0x51460C  ; mode == 0
//   0x005144ED  dec eax / 0x005144EE je 0x514589     ; mode == 1
//   0x005144F4  dec eax / 0x005144F5 je 0x514506     ; mode == 2
//   0x005144F7  push "Unknown blendmode"; call 0x004AF7B0; 0x00514504 jmp $
//   The three accepted blocks each reload 0x005CA114 before every indirect call, invoke
//   device vtable +0x50 with (device, state, value), invoke +0x94 with
//   (device, stage, type, value), and return the last +0x50 HRESULT in eax.
//
// Direct3D 7 mapping (inference from the offset/argument shape, confirmed against the
// mingw-w64 d3d.h IDirect3DDevice7 declaration):
//   +0x50 = SetRenderState(state, value)               (retail call shape: this, state, value)
//   +0x94 = SetTextureStageState(stage, type, value)   (retail call shape: this, stage, type, value)
// The port has no real Direct3D layer, so terrain_draw.hpp's compact host vtable supplies the
// same methods with the same order and semantics. The host vtable offsets are not retail memory.
//
// Render-state semantics (D3D7 enumerants; the names are evidence-based, the human labels
// "opaque/additive/multiplicative" are an inference from the state combinations):
//   mode 0: ALPHABLENDENABLE=0, FOGCOLOR=g_a7e068, FOGENABLE=g_5470b4,
//           TEXTURE[0].COLOROP=MODULATE, ZFUNC=LESS, ZWRITEENABLE=1
//   mode 1: ALPHABLENDENABLE=1, SRCBLEND=ONE, DESTBLEND=ONE, FOGCOLOR=0,
//           FOGENABLE=g_5470b4, TEXTURE[0].COLOROP=MODULATE, ZFUNC=LESSEQUAL,
//           ZWRITEENABLE=0
//   mode 2: ALPHABLENDENABLE=1, SRCBLEND=ZERO, FOGCOLOR=0xFFFFFFFF, DESTBLEND=SRCCOLOR,
//           FOGENABLE=g_5470b4, TEXTURE[0].COLOROP=SELECTARG1, ZFUNC=LESSEQUAL,
//           ZWRITEENABLE=0
// Every other 32-bit mode reaches the fatal "Unknown blendmode" path, which does not return.
//
// Status/limits: STRUCTURALLY_VERIFIED; not wired into CMakeLists.txt; no runtime capture exists
// for this source. The function deliberately performs no null check on 0x005CA114: retail would
// fault on a null device and inventing an exception here would change the observable behavior.
#include "igi/retail/terrain/terrain_unknown_blendmode.hpp"

#include "igi/retail/error/error_show.hpp"

namespace igi::retail::terrain
{
namespace
{
// Direct3D 7 render-state ids (d3dtypes.h D3DRENDERSTATETYPE).
constexpr std::uint32_t kZWriteEnable = 0x0E;      // D3DRS_ZWRITEENABLE
constexpr std::uint32_t kSrcBlend = 0x13;          // D3DRS_SRCBLEND
constexpr std::uint32_t kDestBlend = 0x14;         // D3DRS_DESTBLEND
constexpr std::uint32_t kZFunc = 0x17;             // D3DRS_ZFUNC
constexpr std::uint32_t kAlphaBlendEnable = 0x1B;  // D3DRS_ALPHABLENDENABLE
constexpr std::uint32_t kFogEnable = 0x1C;         // D3DRS_FOGENABLE
constexpr std::uint32_t kFogColor = 0x22;          // D3DRS_FOGCOLOR

// Direct3D 7 texture-stage ids and operations (d3dtypes.h).
constexpr std::uint32_t kColorOp = 0x01;           // D3DTSS_COLOROP
constexpr std::uint32_t kSelectArg1 = 0x02;        // D3DTOP_SELECTARG1
constexpr std::uint32_t kModulate = 0x04;          // D3DTOP_MODULATE

// Direct3D 7 comparison and blend functions (d3dtypes.h).
constexpr std::uint32_t kCmpLess = 0x02;           // D3DCMP_LESS
constexpr std::uint32_t kCmpLessEqual = 0x04;      // D3DCMP_LESSEQUAL
constexpr std::uint32_t kBlendZero = 0x01;         // D3DBLEND_ZERO
constexpr std::uint32_t kBlendOne = 0x02;          // D3DBLEND_ONE
constexpr std::uint32_t kBlendSrcColor = 0x03;     // D3DBLEND_SRCCOLOR

// 0x00514504: retail jumps to itself after the fatal error. Keep the hang observable instead of
// letting the optimizer fall through a side-effect-free loop.
[[noreturn]] void halt_forever() noexcept
{
    for (;;)
    {
    }
}

// 0x005144xx: mov eax, [0x005CA114]; mov ecx, [eax]; call [ecx+0x50].
// The global is reloaded for every call, matching the retail instruction stream.
std::int32_t set_render_state(std::uint32_t state, std::uint32_t value)
{
    Device7* const device = g_5ca114;
    return device->vtbl->SetRenderState(device, state, value);
}

// 0x005144xx: mov eax, [0x005CA114]; mov ecx, [eax]; call [ecx+0x94].
void set_texture_stage_state(std::uint32_t stage, std::uint32_t type, std::uint32_t value)
{
    Device7* const device = g_5ca114;
    static_cast<void>(device->vtbl->SetTextureStageState(device, stage, type, value));
}
} // namespace

extern "C" std::int32_t Terrain_UnknownBlendmode(std::int32_t blendmode)
{
    // Retail compares with sub/je, dec/je, dec/je in this order.
    if (blendmode == 0) // 0x005144E7 -> 0x0051460C
    {
        static_cast<void>(set_render_state(kAlphaBlendEnable, 0));                       // 0x00514611-0x00514618
        static_cast<void>(set_render_state(kFogColor, static_cast<std::uint32_t>(g_a7e068))); // 0x00514620-0x0051462C
        static_cast<void>(set_render_state(kFogEnable, static_cast<std::uint32_t>(g_5470b4))); // 0x00514634-0x00514640
        set_texture_stage_state(0, kColorOp, kModulate);                                 // 0x00514648-0x00514651
        static_cast<void>(set_render_state(kZFunc, kCmpLess));                           // 0x0051465C-0x00514663
        return set_render_state(kZWriteEnable, 1);                                       // 0x0051466B-0x00514672
    }

    if (blendmode == 1) // 0x005144EE -> 0x00514589
    {
        static_cast<void>(set_render_state(kAlphaBlendEnable, 1));   // 0x0051458E-0x00514595
        static_cast<void>(set_render_state(kSrcBlend, kBlendOne));   // 0x0051459D-0x005145A4
        static_cast<void>(set_render_state(kDestBlend, kBlendOne));  // 0x005145AC-0x005145B3
        static_cast<void>(set_render_state(kFogColor, 0));           // 0x005145BB-0x005145C2
        static_cast<void>(set_render_state(kFogEnable, static_cast<std::uint32_t>(g_5470b4))); // 0x005145CA-0x005145D6
        set_texture_stage_state(0, kColorOp, kModulate);             // 0x005145DE-0x005145E7
        static_cast<void>(set_render_state(kZFunc, kCmpLessEqual));  // 0x005145F2-0x005145F9
        return set_render_state(kZWriteEnable, 0);                   // 0x00514601-0x00514608
    }

    if (blendmode == 2) // 0x005144F5 -> 0x00514506
    {
        static_cast<void>(set_render_state(kAlphaBlendEnable, 1));          // 0x0051450B-0x00514512
        static_cast<void>(set_render_state(kSrcBlend, kBlendZero));         // 0x0051451A-0x00514521
        static_cast<void>(set_render_state(kFogColor, 0xFFFFFFFFU));        // 0x00514529-0x00514530
        static_cast<void>(set_render_state(kDestBlend, kBlendSrcColor));    // 0x00514538-0x0051453F
        static_cast<void>(set_render_state(kFogEnable, static_cast<std::uint32_t>(g_5470b4))); // 0x00514547-0x00514553
        set_texture_stage_state(0, kColorOp, kSelectArg1);                  // 0x0051455B-0x00514564
        static_cast<void>(set_render_state(kZFunc, kCmpLessEqual));         // 0x0051456F-0x00514576
        return set_render_state(kZWriteEnable, 0);                          // 0x0051457E-0x00514585
    }

    // 0x005144F7-0x00514504: fatal error, then an intentional infinite loop; no return path.
    error::Error_Show("Unknown blendmode"); // retail callee 0x004AF7B0 (Debug_Error)
    halt_forever();
}
} // namespace igi::retail::terrain
