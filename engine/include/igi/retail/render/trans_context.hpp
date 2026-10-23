#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::render {

// Retail IGI.EXE transform context.
//
// The active context is the global at VA 0x00BCAAE0 and is installed by
// Transcontext_SetActiveTransContext (VA 0x00497E70), which copies 0xA8 bytes
// from the caller's context and installs it.
//
// Verified field offsets:
//   +0x28  camera position, three float64 (globals 0x00BCAB08, 0x00BCAB10, 0x00BCAB18)
//   +0x40  vFOVX          (float)
//   +0x44  vFOVY          (float)
//   +0x48  vScreenFOVX    (float)
//   +0x4C  vScreenFOVY    (float)
//   +0x50  vScreenOriginX (float)
//   +0x54  vScreenOriginY (float)
//   +0x58  isDraw         (int32_t)
//   +0x60  eLayer         (int32_t)
inline constexpr std::size_t trans_context_size = 0xA8;
inline constexpr std::size_t trans_context_camera_position_offset = 0x28;
inline constexpr std::size_t trans_context_vfovx_offset = 0x40;
inline constexpr std::size_t trans_context_vfovy_offset = 0x44;
inline constexpr std::size_t trans_context_screen_origin_x_offset = 0x50;
inline constexpr std::size_t trans_context_screen_origin_y_offset = 0x54;
inline constexpr std::size_t trans_context_is_draw_offset = 0x58;
inline constexpr std::size_t trans_context_layer_offset = 0x60;

struct TransContext {
    std::array<std::uint8_t, trans_context_size> bytes{};
};

// 0x00497E70 Transcontext_SetActiveTransContext.
// 0x00497E82:
//   ... if (hook) hook(&local)
//   g_6e5bb0 = 1; ++g_6e5bb4
std::int32_t Transcontext_SetActiveTransContext(const TransContext& context);

// The retail active context (global 0x00BCAAE0).
[[nodiscard]] TransContext& Transcontext_GetActiveTransContext();

// Camera position accessors, mapped onto the verified +0x28 / 0x00BCAB08 fields.
[[nodiscard]] std::array<double, 3> Transcontext_GetCameraPosition();
void Transcontext_SetCameraPosition(double x, double y, double z);

// HOST INTEGRATION: reads the retail transform-context half-FOV tangents written by
// Transcontext_BuildViewBlock (0x00497D70).  The Direct3D 7 projection consumes their
// reciprocal, so its aspect-dependent field of view is derived from the retail context.
[[nodiscard]] std::array<float, 2> Transcontext_GetFovTangents();

// Retail counters touched by the setter (globals 0x006E5BB0 / 0x006E5BB4).
[[nodiscard]] std::int32_t Transcontext_GetInstalledFlag();
[[nodiscard]] std::int32_t Transcontext_GetInstallCount();

} // namespace igi::retail::render
