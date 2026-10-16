#pragma once

#include <cstdint>
#include <functional>

namespace igi::gameplay {

struct FrameAdvance {
    std::uint32_t simulation_steps{};
    std::uint64_t total_simulation_steps{};
    double interpolation_alpha{};
    double discarded_time_seconds{};
};

class GameLoop final {
public:
    using SimulationUpdate = std::function<void(double)>;

    static constexpr double fixed_step_seconds = 1.0 / 30.0; // retail tick rate (recovered per-tick constants assume 30 Hz)
    static constexpr double maximum_frame_delta_seconds = 0.25;
    static constexpr std::uint32_t maximum_simulation_steps_per_frame = 8;

    [[nodiscard]] FrameAdvance advance(double elapsed_seconds) noexcept;
    [[nodiscard]] FrameAdvance advance(
        double elapsed_seconds,
        const SimulationUpdate& simulation_update);
    void reset() noexcept;

private:
    double accumulated_time_seconds_{};
    std::uint64_t total_simulation_steps_{};
};

} // namespace igi::gameplay
