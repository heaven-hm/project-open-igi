// Native: Transcontext_BuildViewBlock; retail VA 0x00497D70; signature/calling convention x86 cdecl int32_t(void*, const void*, const void*, float).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/render/view_block.cpp.
// Tools/evidence: Binary Ninja + Radare2 CFG (docs/fidelity/evidence/0x00497D70-*.json); unicorn 2.1.4 execution of the retail bytes vs this candidate (docs/fidelity/evidence/0x00497D70-behavioral.json).
// Behavior/CFG: one basic block, no branches; copies 40 bytes and transposes the leading 3x3 dwords of the context, copies 24 bytes (three float64), stores fovX at +0x40, then vFOVY = (g_bcabc4 * fovX) / (aspect() * g_bcabc0) - the retail body reuses the incoming stack argument slot as the g_bcabc4 * fovX temporary, vScreenFOVX = g_bcabc8 / vFOVX, vScreenFOVY = aspect() * vScreenFOVX, origins = block origin + half extent, isDraw = eLayer = 1; returns 1.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; the retail body's calls into Display_ComputeScreenAspect (0x00491D40) are covered by the same unicorn run using the true PE image.

// Native: Display_ComputeScreenAspect; retail VA 0x00491D40; signature/calling convention x86 cdecl double().
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/render/view_block.cpp.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja + Radare2 CFG (docs/fidelity/evidence/0x00491D40-*.json); unicorn 2.1.4 execution of the retail bytes vs this candidate (docs/fidelity/evidence/0x00491D40-behavioral.json).
// Behavior/CFG: one basic block, two calls to Display_GetActiveMode (0x00491CF0); fild dword [display+8] * 4.0 / (fild dword [display+4] * 3.0) in x87 extended precision. The declared parameters are never read by the retail body.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; catalog aliases for this address (Display_ClearSurfaces_4) do not describe the body, which is the screen aspect factor.

#include "igi/retail/render/view_block.hpp"

#include <cstring>

namespace igi::retail::render {
namespace {

// Host mirror of the fields the retail helpers read from the display object 0x00C28B40.
struct HostDisplayMode {
    std::int32_t reserved;
    std::int32_t width;
    std::int32_t height;
};

HostDisplayMode g_display_mode{0, 0, 0};

// Host mirror of the registered screen framing block (retail global 0x00BCABA0).
struct HostScreenBlock {
    float origin_x;
    float origin_y;
    float width;
    float height;
    float half_width;
    float half_height;
};

HostScreenBlock g_screen_block{};

} // namespace

std::int32_t Display_HostGetWidth()
{
    return g_display_mode.width;
}

std::int32_t Display_HostGetHeight()
{
    return g_display_mode.height;
}

void Display_HostSetDimensions(std::int32_t width, std::int32_t height)
{
    g_display_mode.width = width;
    g_display_mode.height = height;
}

// HOST HELPER: install the screen framing block from a window rect.
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/render/view_block.cpp.
// Tools/evidence: Radare2 disassembly of ViewBlock_FillWindowRect (0x00497680) and the registration copy in Display_RegisterViewBlock (0x00497620).
// Behavior/CFG: mirrors the observed field mapping (+0x18 x, +0x1C y, +0x20 width, +0x24 height, +0x28 width/2, +0x2C height/2); the retail producer computes the rect from the display object and the viewport task fields.
// Status/limits: HOST HELPER; the port installs a full-window rect instead of running the viewport QTask rect computation.
void ScreenBlock_HostInstallRect(float x, float y, float width, float height)
{
    g_screen_block.origin_x = x;
    g_screen_block.origin_y = y;
    g_screen_block.width = width;
    g_screen_block.height = height;
    g_screen_block.half_width = width * 0.5F;
    g_screen_block.half_height = height * 0.5F;
}

double Display_ComputeScreenAspect()
{
    // 0x00491D40: fild [display+8] (height) * 4.0f; fild [display+4] (width) * 3.0f; fdivp.
    const double height = static_cast<double>(g_display_mode.height);
    const double width = static_cast<double>(g_display_mode.width);
    return (4.0 * height) / (3.0 * width);
}

std::int32_t Transcontext_BuildViewBlock(
    void* context, const void* view_source, const void* camera_source, float fov_x)
{
    auto* ctx = static_cast<std::uint8_t*>(context);
    // rep movsd x10: the retail body first copies the 40-byte view block into the context.
    std::memcpy(ctx, view_source, 0x28);
    // The retail body then transposes the leading 3x3 dword matrix in place through the
    // stack: it reads [ebx], [ebx+0xC], [ebx+0x18], [ebx+4], [ebx+0x10], [ebx+0x1C],
    // [ebx+8], [ebx+0x14], [ebx+0x20], [ebx+0x24] into ten contiguous stack dwords and
    // copies them back with rep movsd (net order 0,3,6,1,4,7,2,5,8,9). The retail-byte
    // execution fixture measures the transposed block, so the transcription reproduces it.
    std::uint32_t block_words[10];
    std::memcpy(block_words, ctx, sizeof(block_words));
    const std::uint32_t transposed[10] = {
        block_words[0], block_words[3], block_words[6],
        block_words[1], block_words[4], block_words[7],
        block_words[2], block_words[5], block_words[8],
        block_words[9],
    };
    std::memcpy(ctx, transposed, sizeof(transposed));
    // rep movsd x6: the view's camera position (three float64).
    std::memcpy(ctx + 0x28, camera_source, 0x18);
    std::memcpy(ctx + 0x40, &fov_x, sizeof(fov_x));

    // Retail: fld [0x00BCABC4]; fmul fovX; fstp [esp+0x44]  (the stack argument slot is reused as the temporary)
    //         call Display_ComputeScreenAspect; fmul [0x00BCABC0]; fdivr [esp+0x44]; fstp [ctx+0x44].
    const float scaled_fov_x = g_screen_block.height * fov_x;
    const double aspect = Display_ComputeScreenAspect();
    const double divisor = aspect * static_cast<double>(g_screen_block.width);
    const float view_fov_y = static_cast<float>(static_cast<double>(scaled_fov_x) / divisor);
    std::memcpy(ctx + 0x44, &view_fov_y, sizeof(view_fov_y));

    // Retail: fld [0x00BCABC8]; fdiv [ctx+0x40]; fstp [ctx+0x48].
    const double screen_fov_x_wide =
        static_cast<double>(g_screen_block.half_width) / static_cast<double>(fov_x);
    const float screen_fov_x = static_cast<float>(screen_fov_x_wide);
    std::memcpy(ctx + 0x48, &screen_fov_x, sizeof(screen_fov_x));

    // Retail: call Display_ComputeScreenAspect; fmul [ctx+0x48]; fstp [ctx+0x4C].
    const float screen_fov_y = static_cast<float>(
        Display_ComputeScreenAspect() * static_cast<double>(screen_fov_x));
    std::memcpy(ctx + 0x4C, &screen_fov_y, sizeof(screen_fov_y));

    // Retail: fld [0x00BCABB8]; fadd [0x00BCABC8]; fstp [ctx+0x50]; same for Y with 0x00BCABBC/0x00BCABCC.
    const float origin_x = g_screen_block.origin_x + g_screen_block.half_width;
    const float origin_y = g_screen_block.origin_y + g_screen_block.half_height;
    std::memcpy(ctx + 0x50, &origin_x, sizeof(origin_x));
    std::memcpy(ctx + 0x54, &origin_y, sizeof(origin_y));

    const std::int32_t one = 1;
    std::memcpy(ctx + 0x58, &one, sizeof(one));
    std::memcpy(ctx + 0x60, &one, sizeof(one));
    return 1;
}

} // namespace igi::retail::render
