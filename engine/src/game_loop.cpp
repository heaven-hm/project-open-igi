#include "igi/game_loop.hpp"

#include <cmath>

namespace igi::gameplay {

FrameAdvance GameLoop::advance(double elapsed_seconds) noexcept {
    return advance(elapsed_seconds, {});
}

FrameAdvance GameLoop::advance(
    double elapsed_seconds,
    const SimulationUpdate& simulation_update) {
    FrameAdvance frame_advance{};

    if (!std::isfinite(elapsed_seconds) || elapsed_seconds < 0.0) {
        frame_advance.total_simulation_steps = total_simulation_steps_;
        frame_advance.interpolation_alpha =
            accumulated_time_seconds_ / fixed_step_seconds;
        return frame_advance;
    }

    if (elapsed_seconds > maximum_frame_delta_seconds) {
        frame_advance.discarded_time_seconds =
            elapsed_seconds - maximum_frame_delta_seconds;
        elapsed_seconds = maximum_frame_delta_seconds;
    }

    accumulated_time_seconds_ += elapsed_seconds;

    while (accumulated_time_seconds_ >= fixed_step_seconds &&
           frame_advance.simulation_steps < maximum_simulation_steps_per_frame) {
        accumulated_time_seconds_ -= fixed_step_seconds;
        ++frame_advance.simulation_steps;
        ++total_simulation_steps_;
        if (simulation_update) {
            simulation_update(fixed_step_seconds);
        }
    }

    if (accumulated_time_seconds_ >= fixed_step_seconds) {
        const double overdue_steps =
            std::floor(accumulated_time_seconds_ / fixed_step_seconds);
        const double discarded_backlog = overdue_steps * fixed_step_seconds;
        accumulated_time_seconds_ -= discarded_backlog;
        frame_advance.discarded_time_seconds += discarded_backlog;
    }

    frame_advance.total_simulation_steps = total_simulation_steps_;
    frame_advance.interpolation_alpha =
        accumulated_time_seconds_ / fixed_step_seconds;
    return frame_advance;
}

void GameLoop::reset() noexcept {
    accumulated_time_seconds_ = 0.0;
    total_simulation_steps_ = 0;
}

} // namespace igi::gameplay
