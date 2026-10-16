#pragma once

namespace igi::gameplay {

struct ViewBobOffset {
    float vertical_offset{};   // eye height Z offset (units)
    float lateral_offset{};    // lateral right-vector offset (units)
    float pitch_bob{};         // view pitch perturbation (radians)
};

// Computes rhythmic camera and weapon view bobbing during movement.
class PlayerViewBob final {
public:
    void update(
        float dt,
        bool is_grounded,
        float horizontal_speed,
        bool is_running,
        bool is_crouching) noexcept;

    void reset() noexcept;

    [[nodiscard]] ViewBobOffset current_offset() const noexcept;
    [[nodiscard]] float phase() const noexcept { return phase_; }
    [[nodiscard]] float amplitude() const noexcept { return amplitude_; }

private:
    float phase_{0.0f};
    float amplitude_{0.0f};
};

} // namespace igi::gameplay
