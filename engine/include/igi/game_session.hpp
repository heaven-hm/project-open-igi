#pragma once

#include "igi/ai_system.hpp"
#include "igi/input_state.hpp"
#include "igi/level_flow.hpp"
#include "igi/mission_expression.hpp"
#include "igi/mission_world.hpp"
#include "igi/player_view_bob.hpp"
#include "igi/sight_zoom.hpp"
#include "igi/software_renderer.hpp"
#include "igi/weapon_system.hpp"
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
    [[nodiscard]] const LevelFlow& level_flow() const noexcept { return level_flow_; }
    [[nodiscard]] LevelFlow& level_flow() noexcept { return level_flow_; }
    [[nodiscard]] const MissionExpressionState& expression_state() const noexcept { return expression_state_; }
    [[nodiscard]] MissionExpressionState& expression_state() noexcept { return expression_state_; }
    [[nodiscard]] const MissionWorld* world() const noexcept { return world_.get(); }
    [[nodiscard]] MissionWorld* world() noexcept { return world_.get(); }
    void set_alarm_active(bool active) noexcept {
        if (world_) {
            world_->set_alarm_active(active);
        }
    }
    [[nodiscard]] const SightZoom& sight_zoom() const noexcept { return sight_zoom_; }
    [[nodiscard]] const PlayerViewBob& view_bob() const noexcept { return view_bob_; }

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
    LevelFlow level_flow_;
    MissionExpressionState expression_state_;
    SightZoom sight_zoom_;
    PlayerViewBob view_bob_;
    float hit_marker_seconds_{};
    bool hit_marker_headshot_{false};
    float pending_mouse_x_{};
    float pending_mouse_y_{};
    double last_terrain_center_x_{};
    double last_terrain_center_y_{};
};

} // namespace igi::gameplay
