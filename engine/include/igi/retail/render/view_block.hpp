#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::render {

// Retail IGI.EXE screen framing state.
//
// The viewport task (0x004E7E30) fills a 76-byte block, registers it with
// Display_RegisterViewBlock (0x00497620), which copies the block to the global
// 0x00BCABA0: ViewBlock_FillWindowRect (0x00497680) writes the rect and the
// derived half-extent fields. Transcontext_BuildViewBlock (0x00497D70) then
// reads those globals to frame the transform context.
inline constexpr std::size_t screen_block_origin_x_offset = 0x18;
inline constexpr std::size_t screen_block_origin_y_offset = 0x1C;
inline constexpr std::size_t screen_block_width_offset = 0x20;
inline constexpr std::size_t screen_block_height_offset = 0x24;
inline constexpr std::size_t screen_block_half_width_offset = 0x28;
inline constexpr std::size_t screen_block_half_height_offset = 0x2C;

// Host-side mirror of the retail display object 0x00C28B40 (width at +4, height at +8),
// read by Display_ComputeScreenAspect (0x00491D40).
[[nodiscard]] std::int32_t Display_HostGetWidth();
[[nodiscard]] std::int32_t Display_HostGetHeight();
void Display_HostSetDimensions(std::int32_t width, std::int32_t height);

// Host-side install of the screen framing block, mirroring the observed field
// mapping of ViewBlock_FillWindowRect (0x00497680): origin, extents, half-extents.
void ScreenBlock_HostInstallRect(float x, float y, float width, float height);

// 0x00491D40: (4 * display_height) / (3 * display_width) in extended precision,
// returned as a double. The retail body ignores every incoming argument and reads
// the display singleton instead.
[[nodiscard]] double Display_ComputeScreenAspect();

// 0x00497D70: build the transform-context view block.
//
//   context        +0x00..0x27 receives 40 bytes copied from view_source,
//                  +0x28..0x3F receives 24 bytes copied from camera_source (three float64),
//                  +0x40 vFOVX, +0x44 vFOVY, +0x48 vScreenFOVX, +0x4C vScreenFOVY,
//                  +0x50/+0x54 screen origin, +0x58 isDraw = 1, +0x60 eLayer = 1.
//   fov_x          the horizontal half-FOV tangent from the human view (see Human_TaskViewReset).
//
// Returns 1, matching the retail EAX.
std::int32_t Transcontext_BuildViewBlock(
    void* context, const void* view_source, const void* camera_source, float fov_x);

} // namespace igi::retail::render
