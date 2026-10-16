#include "igi/player_view_bob.hpp"

#include <algorithm>
#include <cmath>

namespace igi::gameplay {

void PlayerViewBob::update(
    float dt,
    bool is_grounded,
    float horizontal_speed,
    bool is_running,
    bool is_crouching) noexcept {
    float target_amplitude = 0.0f;
    float step_rate = 0.0f;

    if (is_grounded && horizontal_speed > 10.0f) {
        if (is_crouching) {
            target_amplitude = 20.0f;
            step_rate = 0.10f;
        } else if (is_running) {
            target_amplitude = 65.0f;
            step_rate = 0.22f;
        } else {
            target_amplitude = 35.0f;
            step_rate = 0.14f;
        }
        phase_ += step_rate;
        constexpr float kTwoPi = 6.28318530717958647692f;
        if (phase_ > kTwoPi) {
            phase_ -= kTwoPi;
        }
    }

    const float blend = std::clamp(dt * 8.0f, 0.0f, 1.0f);
    amplitude_ += (target_amplitude - amplitude_) * blend;
}

void PlayerViewBob::reset() noexcept {
    phase_ = 0.0f;
    amplitude_ = 0.0f;
}

ViewBobOffset PlayerViewBob::current_offset() const noexcept {
    ViewBobOffset offset{};
    if (amplitude_ < 0.1f) {
        return offset;
    }
    offset.vertical_offset = std::sin(phase_ * 2.0f) * amplitude_;
    offset.lateral_offset = std::cos(phase_) * (amplitude_ * 0.45f);
    offset.pitch_bob = -std::sin(phase_ * 2.0f) * (amplitude_ * 0.000045f);
    return offset;
}

} // namespace igi::gameplay
