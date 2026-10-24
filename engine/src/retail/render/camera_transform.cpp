#include "igi/retail/render/camera_transform.hpp"

#include <cmath>

namespace igi::retail::render {
namespace {

CameraMatrixState camera_matrix_state{};

} // namespace

// Native: Matrix_InvalidateSlot; retail VA 0x004B7450; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: Clears the 32-bit slot at matrix + slot_index * 4 + 0x14 and returns slot_index. Named "PushStack" by the catalog, but the body is a slot invalidation, not a stack push.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
std::int32_t Matrix_InvalidateSlot(std::int32_t* matrix, std::int32_t slot_index)
{
    auto* const slot = reinterpret_cast<std::int32_t*>(
        reinterpret_cast<std::uint8_t*>(matrix) + (static_cast<std::size_t>(slot_index) * 4U)
        + camera_matrix_slot_base);
    *slot = 0;
    return slot_index;
}

// 0x004A0110  Camera_UpdateTransform
// Eight slot invalidations over the three camera matrix slabs, in the retail call order.
void Camera_UpdateTransform()
{
    CameraMatrixState& state = camera_matrix_state;

    Matrix_InvalidateSlot(state.slab_bcad7c.data(), state.slot_7205b8);
    Matrix_InvalidateSlot(state.slab_bcadac.data(), state.slot_723508);
    Matrix_InvalidateSlot(state.slab_bcadac.data(), state.slot_7205bc);
    Matrix_InvalidateSlot(state.slab_bcade8.data(), state.slot_723504);
    Matrix_InvalidateSlot(state.slab_bcadac.data(), state.slot_7addd4);
    Matrix_InvalidateSlot(state.slab_bcade8.data(), state.slot_7205b0);
    Matrix_InvalidateSlot(state.slab_bcad7c.data(), state.slot_7ac09c);
    Matrix_InvalidateSlot(state.slab_bcadac.data(), state.slot_7234b8);
}

// Native: Camera_UpdateFogPalette; retail VA 0x004A26C0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004A26C0-0x004A2716, 86 bytes, digest f7a98c1e60c3d681dd99aabe724a45e4633a27e832b6cc7226ad1526e4111b0e.
// Behavior/CFG: scales float RGB by -255.0f (R, G) and 255.0f (B), computes packed color, fills 256-dword palette buffer at 0x00935C10 with stosd, returns packed color DWORD. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact arithmetic and palette store; full render loop integration pending.
std::int32_t Camera_UpdateFogPalette(const CameraColorContext* context)
{
    if (context == nullptr) {
        return 0;
    }

    constexpr float scale_neg_255 = -255.0f;
    constexpr float scale_pos_255 = 255.0f;

    const auto r_scaled = static_cast<std::int32_t>(std::lrint(context->color_r * scale_neg_255));
    auto ecx = r_scaled << 8;

    const auto g_scaled = static_cast<std::int32_t>(std::lrint(context->color_g * scale_neg_255));
    ecx += g_scaled;
    ecx = ecx << 8;

    const auto b_scaled = static_cast<std::int32_t>(std::lrint(context->color_b * scale_pos_255));
    const std::int32_t result = b_scaled - ecx;

    camera_matrix_state.palette.fill(static_cast<std::uint32_t>(result));
    return result;
}

// Native: Camera_ResetMatrix; retail VA 0x004A2600; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004A2600-0x004A26BC, 188 bytes, digest 53d921653980d1425da75ba9b611a3a143e0c4681bba96cc5b7d40852704d66b.
// Behavior/CFG: calculates half-intensity (scale 128.0f) and full-intensity (scale 255.0f) colors, loops 4 times writing 32 dwords of half-intensity then 32 dwords of full-intensity into 0x00935C10 palette buffer, returns full-intensity color DWORD. radare2 3 blocks/3 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact loop logic and memory writes; retail camera reset chain pending.
std::int32_t Camera_ResetMatrix(const CameraColorContext* context)
{
    if (context == nullptr) {
        return 0;
    }

    constexpr float scale_neg_128 = -128.0f;
    constexpr float scale_pos_128 = 128.0f;
    constexpr float scale_neg_255 = -255.0f;
    constexpr float scale_pos_255 = 255.0f;

    const auto r_half = static_cast<std::int32_t>(std::lrint(context->color_r * scale_neg_128));
    auto ecx_half = r_half << 8;
    const auto g_half = static_cast<std::int32_t>(std::lrint(context->color_g * scale_neg_128));
    ecx_half += g_half;
    ecx_half = ecx_half << 8;
    const auto b_half = static_cast<std::int32_t>(std::lrint(context->color_b * scale_pos_128));
    const auto color_half = static_cast<std::uint32_t>(b_half - ecx_half);

    const auto r_full = static_cast<std::int32_t>(std::lrint(context->color_r * scale_neg_255));
    auto ecx_full = r_full << 8;
    const auto g_full = static_cast<std::int32_t>(std::lrint(context->color_g * scale_neg_255));
    ecx_full += g_full;
    ecx_full = ecx_full << 8;
    const auto b_full = static_cast<std::int32_t>(std::lrint(context->color_b * scale_pos_255));
    const std::int32_t result_full = b_full - ecx_full;
    const auto color_full = static_cast<std::uint32_t>(result_full);

    std::size_t dest_index = 0;
    for (std::int32_t iter = 0; iter < 4; ++iter) {
        for (std::size_t i = 0; i < 0x20; ++i) {
            camera_matrix_state.palette[dest_index++] = color_half;
        }
        for (std::size_t i = 0; i < 0x20; ++i) {
            camera_matrix_state.palette[dest_index++] = color_full;
        }
    }

    return result_full;
}

CameraMatrixState& Camera_GetMatrixState()
{
    return camera_matrix_state;
}

} // namespace igi::retail::render
