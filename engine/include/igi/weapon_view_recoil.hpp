#pragma once

namespace igi::gameplay {

// Short fixed-step first-person recoil presentation cue.
// The simulation/ballistics owns aim recoil and damage rays;
// this type animates the visible first-person weapon rig.
class WeaponViewRecoil final {
public:
    static constexpr int kRecoveryTicks = 3;

    void reset() noexcept;
    void trigger_degrees(float pitch_degrees, float yaw_degrees) noexcept;
    void advance() noexcept;

    [[nodiscard]] float pitch_radians() const noexcept { return pitch_radians_; }
    [[nodiscard]] float yaw_radians() const noexcept { return yaw_radians_; }

private:
    static constexpr float kDegreesToRadians = 0.017453292519943295f;

    float peak_pitch_radians_ = 0.0f;
    float peak_yaw_radians_ = 0.0f;
    float pitch_radians_ = 0.0f;
    float yaw_radians_ = 0.0f;
    int recovery_ticks_remaining_ = 0;
};

} // namespace igi::gameplay
