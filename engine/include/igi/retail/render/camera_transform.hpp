#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::render {

// Native: Matrix_InvalidateSlot() - VA 0x004B7450 - complete port.
// Previous catalog alias: Sys_PushStack_19.
// Clears the 32-bit matrix slot at `matrix + slot_index * 4 + 0x14` and returns `slot_index`.
// Behaviour verified from the disassembly: three instructions, no branches.
std::int32_t Matrix_InvalidateSlot(std::int32_t* matrix, std::int32_t slot_index);
inline std::int32_t Sys_PushStack_19(std::int32_t* matrix, std::int32_t slot_index)
{
    return Matrix_InvalidateSlot(matrix, slot_index);
}

// Camera_UpdateTransform - VA 0x004A0110.
// Invalidates the eight camera matrix slots. The retail globals referenced by the native are
// 0x00BCAD7C, 0x00BCADAC and 0x00BCADE8 (matrix slabs) plus the eight slot indices
// 0x007205B8, 0x00723508, 0x007205BC, 0x00723504, 0x007ADDD4, 0x007205B0, 0x007AC09C, 0x007234B8.
// In the port those globals are port-owned storage (see Camera_GetMatrixState); the retail
// display initialiser that points them at engine memory is not ported yet.
void Camera_UpdateTransform();

inline constexpr std::size_t camera_matrix_slot_base = 0x14;
inline constexpr std::size_t camera_matrix_slot_count = 0x100;
inline constexpr std::size_t camera_palette_entry_count = 0x100;

// Camera color context layout matching retail offsets +0x64 (R), +0x68 (G), +0x6C (B).
struct CameraColorContext {
    std::array<std::uint8_t, 0x64> unported_prefix{};
    float color_r = 0.0f; // offset 0x64
    float color_g = 0.0f; // offset 0x68
    float color_b = 0.0f; // offset 0x6c
};

#if defined(_M_IX86) || defined(__i386__)
static_assert(sizeof(CameraColorContext) == 0x70);
#endif

// Native: Camera_UpdateFogPalette() - VA 0x004A26C0 - complete port.
// Previous catalog alias: Camera_UpdateTransform_2.
// Behavior: scales float RGB by -255.0f (R, G) and 255.0f (B), computes packed color,
//           fills 256-dword palette buffer at 0x00935C10 with stosd, returns packed color DWORD.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004A26C0-0x004A2716, 86 bytes, digest f7a98c1e60c3d681dd99aabe724a45e4633a27e832b6cc7226ad1526e4111b0e.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact arithmetic and palette store; full render loop integration pending.
// Signature: x86 cdecl int32_t(void* arg1); ported as std::int32_t Camera_UpdateFogPalette(const CameraColorContext* context).
// Usage/tests: initializes fog/lighting palette; test covers color component scaling, packing, and 256-dword write.
std::int32_t Camera_UpdateFogPalette(const CameraColorContext* context);
inline std::int32_t Camera_UpdateTransform_2(const CameraColorContext* context)
{
    return Camera_UpdateFogPalette(context);
}

// Native: Camera_ResetMatrix() - VA 0x004A2600 - complete port.
// Behavior: calculates half-intensity (scale 128.0f) and full-intensity (scale 255.0f) colors,
//           loops 4 times writing 32 dwords of half-intensity then 32 dwords of full-intensity
//           into 0x00935C10 palette buffer, returns full-intensity color DWORD.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004A2600-0x004A26BC, 188 bytes,
//                 digest 53d921653980d1425da75ba9b611a3a143e0c4681bba96cc5b7d40852704d66b.
// CFG: radare2 3 blocks/3 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact loop logic and memory writes; retail camera reset chain pending.
// Signature: x86 cdecl int32_t(void* arg1); ported as std::int32_t Camera_ResetMatrix(const CameraColorContext* context).
// Usage/tests: sets alternating 32-dword banded palette; test verifies 4-band alternating structure and return value.
std::int32_t Camera_ResetMatrix(const CameraColorContext* context);

// Port-owned storage for the globals touched by Camera_UpdateTransform, Camera_UpdateTransform_2, and Camera_ResetMatrix.
struct CameraMatrixState {
    std::array<std::int32_t, camera_matrix_slot_count> slab_bcad7c{};
    std::array<std::int32_t, camera_matrix_slot_count> slab_bcadac{};
    std::array<std::int32_t, camera_matrix_slot_count> slab_bcade8{};
    std::array<std::uint32_t, camera_palette_entry_count> palette_935c10{};
    std::int32_t slot_7205b8 = 0;
    std::int32_t slot_723508 = 0;
    std::int32_t slot_7205bc = 0;
    std::int32_t slot_723504 = 0;
    std::int32_t slot_7addd4 = 0;
    std::int32_t slot_7205b0 = 0;
    std::int32_t slot_7ac09c = 0;
    std::int32_t slot_7234b8 = 0;
};

[[nodiscard]] CameraMatrixState& Camera_GetMatrixState();

} // namespace igi::retail::render

