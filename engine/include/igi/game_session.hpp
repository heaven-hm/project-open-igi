#pragma once

#include "igi/ai_system.hpp"
#include "igi/input_state.hpp"
#include "igi/weapon_system.hpp"
#include "igi/mission_world.hpp"
#include "igi/software_renderer.hpp"
#include "igi/world_collision.hpp"

#include <cstdint>
#include <filesystem>
#include <memory>

namespace igi::gameplay {

class GameSession final {
public:
    GameSession();
    ~GameSession();
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;

    // Loads retail mission data (read-only) and places the player at spawn.
    [[nodiscard]] bool load_mission(const std::filesystem::path& game_root, int mission_number);
    [[nodiscard]] bool has_mission() const noexcept;
    [[nodiscard]] const std::string& mission_status() const noexcept;

    // Accumulates relative mouse motion until the next fixed update consumes it.
    void add_mouse_motion(float delta_x, float delta_y) noexcept;

    [[nodiscard]] bool update(
        double fixed_delta_seconds,
        const InputSnapshot& input_snapshot) noexcept;

    // Renders the current state into the portable framebuffer.
    void render(std::uint32_t width, std::uint32_t height);
    [[nodiscard]] const SoftwareRenderer& renderer() const noexcept;
    [[nodiscard]] const PlayerPhysicsState& player() const noexcept;
    [[nodiscard]] const WeaponSystem& weapons() const noexcept { return weapons_; }
    [[nodiscard]] const AiSystem& ai() const noexcept { return ai_; }

    [[nodiscard]] std::uint64_t simulation_tick_count() const noexcept;
    [[nodiscard]] double simulation_time_seconds() const noexcept;
    [[nodiscard]] const InputSnapshot& current_input() const noexcept;

    void reset() noexcept;

private:
    std::uint64_t simulation_tick_count_{};
    double simulation_time_seconds_{};
    InputSnapshot current_input_{};

    std::unique_ptr<MissionWorld> world_;
    WorldCollision collision_;
    PlayerPhysicsState player_;
    SoftwareRenderer renderer_;
    WeaponSystem weapons_;
    AiSystem ai_;
    float hit_marker_seconds_{};
    float pending_mouse_x_{};
    float pending_mouse_y_{};
};

} // namespace igi::gameplay
