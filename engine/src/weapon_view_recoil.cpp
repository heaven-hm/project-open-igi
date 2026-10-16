#include "igi/weapon_view_recoil.hpp"

#include <cmath>

namespace igi::gameplay {

void WeaponViewRecoil::reset() noexcept {
    peak_pitch_radians_ = 0.0f;
    peak_yaw_radians_ = 0.0f;
    pitch_radians_ = 0.0f;
    yaw_radians_ = 0.0f;
    recovery_ticks_remaining_ = 0;
}

void WeaponViewRecoil::trigger_degrees(float pitch_degrees, float yaw_degrees) noexcept {
    peak_pitch_radians_ = pitch_degrees * kDegreesToRadians;
    peak_yaw_radians_ = yaw_degrees * kDegreesToRadians;
    pitch_radians_ = peak_pitch_radians_;
    yaw_radians_ = peak_yaw_radians_;
    recovery_ticks_remaining_ = kRecoveryTicks;
}

void WeaponViewRecoil::advance() noexcept {
    if (recovery_ticks_remaining_ <= 0) {
        pitch_radians_ = 0.0f;
        yaw_radians_ = 0.0f;
        return;
    }

    --recovery_ticks_remaining_;
    const float fraction = static_cast<float>(recovery_ticks_remaining_) /
                           static_cast<float>(kRecoveryTicks);
    pitch_radians_ = peak_pitch_radians_ * fraction;
    yaw_radians_ = peak_yaw_radians_ * fraction;
}

} // namespace igi::gameplay
