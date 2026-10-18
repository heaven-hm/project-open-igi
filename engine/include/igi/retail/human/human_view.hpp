#pragma once

namespace igi::retail::human {

// Retail IGI.EXE first-person view tangents, carried on the Human struct.
//
// Verified field offsets (unicorn execution of retail bytes 0x004659E0):
//   +0x1E4  view tangent (tan of the half horizontal FOV); default tan(pi/4) = 1.0
//   +0x1E8  view-model tangent; default tan(pi/6) = 0.57735026
//   +0x1EC  auxiliary int32, cleared by the reset
inline constexpr unsigned long human_view_tangent_x_offset = 0x1E4UL;
inline constexpr unsigned long human_view_tangent_viewmodel_offset = 0x1E8UL;
inline constexpr unsigned long human_view_aux_offset = 0x1ECUL;

// 0x004659E0 Human_TaskViewReset: restores the retail default view tangents
// (and clears the auxiliary field) on the human struct, returning the human pointer.
void* Human_TaskViewReset(void* human);

} // namespace igi::retail::human
