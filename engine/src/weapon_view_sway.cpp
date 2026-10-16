#include "igi/weapon_view_sway.hpp"

#include <algorithm>

namespace igi::gameplay {

void WeaponViewSway::reset() noexcept {
    snap_to(0.0f, 0.0f);
}

void WeaponViewSway::lower() noexcept {
    target_pitch_radians_ = kLoweredPitchRadians;
    target_yaw_radians_ = kLoweredYawRadians;
}

void WeaponViewSway::raise() noexcept {
    target_pitch_radians_ = 0.0f;
    target_yaw_radians_ = 0.0f;
}

void WeaponViewSway::snap_to(float pitch_radians, float yaw_radians) noexcept {
    pitch_radians_ = pitch_radians;
    yaw_radians_ = yaw_radians;
    target_pitch_radians_ = pitch_radians;
    target_yaw_radians_ = yaw_radians;
}

void WeaponViewSway::advance() noexcept {
    if (is_settled()) {
        return;
    }

    pitch_radians_ = step_towards(
        pitch_radians_,
        target_pitch_radians_,
        kPitchStepRadians);
    yaw_radians_ = step_towards(
        yaw_radians_,
        target_yaw_radians_,
        kYawStepRadians);
}

float WeaponViewSway::step_towards(
    float current_radians,
    float target_radians,
    float step_radians) noexcept {
    if (current_radians == target_radians) {
        return current_radians;
    }

    if (current_radians < target_radians) {
        return std::min(current_radians + step_radians, target_radians);
    }
    return std::max(current_radians - step_radians, target_radians);
}

} // namespace igi::gameplay
