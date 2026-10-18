#pragma once

#include <cstdint>

namespace igi::retail::human {

// Retail human camera state offsets used by the first-person eye:
//   +0x20  feet X (float64)
//   +0x28  feet Y (float64)
//   +0x30  feet Z (float64)
//   +0x33D crouch flag byte
// Verified by unicorn execution of the retail bytes 0x00465820 / 0x00463180.
inline constexpr unsigned long human_camera_position_offset = 0x20UL;
inline constexpr unsigned long human_camera_crouch_flag_offset = 0x33DUL;

// 0x00463180 Human_IsCrouched: returns the crouch flag byte (al) at human+0x33D.
[[nodiscard]] std::uint8_t Human_IsCrouched(const void* human) noexcept;

// 0x00465820 Human_CameraEyePosition: writes the first-person eye as three float64
// (x, y, z + eye height) to out; the eye height is the standing 0.69 m double, replaced by the
// crouched 0.35 m double when the crouch flag is set.
void Human_CameraEyePosition(void* out, const void* human) noexcept;

} // namespace igi::retail::human
