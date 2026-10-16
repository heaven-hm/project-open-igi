#pragma once

namespace igi::gameplay {

// Fixed-step first-person rig lowering used by the retail weapon state machine.
// verified-reference: OpenIGI HumanViewSway literals recovered from the vanilla HumanView weapon-change path.
class WeaponViewSway final {
public:
    static constexpr float kLoweredPitchRadians = -0.61086524f;
    static constexpr float kLoweredYawRadians = -0.34906587f;
    static constexpr float kPitchStepRadians = 0.058177643f;
    static constexpr float kYawStepRadians = 0.033244368f;
    static constexpr int kTicksToTravel = 11;

    void reset() noexcept;
    void lower() noexcept;
    void raise() noexcept;
    void snap_to(float pitch_radians, float yaw_radians) noexcept;
    void advance() noexcept;

    [[nodiscard]] bool is_settled() const noexcept {
        return pitch_radians_ == target_pitch_radians_ &&
               yaw_radians_ == target_yaw_radians_;
    }
    [[nodiscard]] float pitch_radians() const noexcept { return pitch_radians_; }
    [[nodiscard]] float yaw_radians() const noexcept { return yaw_radians_; }
    [[nodiscard]] float target_pitch_radians() const noexcept { return target_pitch_radians_; }
    [[nodiscard]] float target_yaw_radians() const noexcept { return target_yaw_radians_; }

private:
    [[nodiscard]] static float step_towards(
        float current_radians,
        float target_radians,
        float step_radians) noexcept;

    float pitch_radians_ = 0.0f;
    float yaw_radians_ = 0.0f;
    float target_pitch_radians_ = 0.0f;
    float target_yaw_radians_ = 0.0f;
};

} // namespace igi::gameplay
