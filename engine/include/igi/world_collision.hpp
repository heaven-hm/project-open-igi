#pragma once

#include "igi/human_locomotion_graph.hpp"
#include "igi/human_motion.hpp"
#include "igi/input_state.hpp"
#include "igi/mission_world.hpp"
#include "igi/player_fall_impact.hpp"
#include "igi/player_ladder.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace igi::gameplay {

struct RaycastHit {
    bool hit{};
    float fraction{1.0f};
    NavigationPosition point{};
    Vector3 unit_normal{0.0f, 0.0f, 1.0f};
    std::int16_t material_id{-1};
    int object_index{-1};
};

struct WallSweepResult {
    NavigationPosition resolved_position{};
    Vector3 resolved_velocity{};
    bool hit_wall{};
    bool blocked{};
    int iterations{};
};

struct GroundQueryResult {
    bool found_ground{};
    double ground_z{};
    Vector3 ground_normal{0.0f, 0.0f, 1.0f};
    std::int16_t material_id{-1};
    int object_index{-1};
};

struct PlayerPhysicsState {
    NavigationPosition position{24578514.0, -55852816.0, 174421680.0};
    Vector3 velocity{};
    float yaw{3.095987f};
    float pitch{0.0f};
    float pitch_recoil{0.0f};
    float eye_height{7372.8f};
    bool is_grounded{true};
    bool is_crouching{false};
    bool is_running{true};
    bool is_climbing_ladder{false};
    bool is_slope_sliding{false};
    float slope_slide_speed{0.0f};
    Vector3 ground_normal{0.0f, 0.0f, 1.0f};
    std::uint32_t locomotion_state{0};
    float bob_phase{0.0f};
    float health{100.0f};
    float last_fall_impact_damage{0.0f};
    float last_fall_hearing_alert{0.0f};
    float last_fall_view_kick{0.0f};

    [[nodiscard]] NavigationPosition eye_position() const noexcept {
        return {
            position.x,
            position.y,
            position.z + static_cast<double>(eye_height),
        };
    }

    [[nodiscard]] Basis3x3 view_basis() const noexcept {
        return MissionWorld::basis_from_euler(pitch + pitch_recoil, 0.0f, yaw);
    }

    [[nodiscard]] Basis3x3 body_basis() const noexcept {
        return MissionWorld::basis_from_euler(0.0f, 0.0f, yaw);
    }
};

class WorldCollision final {
public:
    static constexpr float kStandingEyeHeight = 7372.8f;
    static constexpr float kCrouchingEyeHeight = 5324.8f;
    static constexpr float kCrouchEasePerTick = 400.0f;
    static constexpr float kWallSkinUnits = 10.24f;
    static constexpr float kLowProbeHeight = 409.6f;
    static constexpr float kMidProbeStanding = 1638.4f;
    static constexpr float kMidProbeCrouching = 819.2f;
    static constexpr float kSeparationHeight = 7372.8f;
    static constexpr float kSeparationRadius = 2252.8f;
    static constexpr float kPitchLimitRadians = 1.5533431f;

    void build_index(const MissionWorld& world);

    [[nodiscard]] RaycastHit raycast(
        const MissionWorld& world,
        NavigationPosition start,
        NavigationPosition end,
        bool use_human_hull_set1 = false,
        bool include_terrain = true) const noexcept;

    // Retail Human_WallSweep (0x00461A70)
    [[nodiscard]] WallSweepResult sweep_walls(
        const MissionWorld& world,
        NavigationPosition start,
        NavigationPosition desired_end,
        Vector3 velocity,
        bool is_grounded,
        bool is_crouching) const noexcept;

    // Retail Human_GroundProbe (0x00462390)
    [[nodiscard]] GroundQueryResult query_ground(
        const MissionWorld& world,
        NavigationPosition position,
        bool is_grounded,
        bool is_crouching,
        bool is_slope_sliding) const noexcept;

    // Retail Human_Separation (0x00462BA0)
    [[nodiscard]] static NavigationPosition resolve_cylinder_separation(
        NavigationPosition start,
        NavigationPosition candidate,
        const std::vector<NavigationPosition>& blocker_positions) noexcept;

    // Retail slope slide integrator (0x004614B0)
    static void update_slope_slide(
        Vector3 ground_normal,
        bool is_grounded,
        float& io_slide_speed,
        bool& out_is_sliding,
        Vector3& io_displacement) noexcept;

    void step_player(
        const MissionWorld& world,
        const InputSnapshot& input,
        float mouse_delta_x,
        float mouse_delta_y,
        const std::vector<NavigationPosition>& active_soldier_positions,
        PlayerPhysicsState& io_player) const noexcept;

private:
    struct SpatialCellKey {
        int gx{};
        int gy{};
        bool operator==(const SpatialCellKey& other) const noexcept {
            return gx == other.gx && gy == other.gy;
        }
    };

    struct SpatialCellHash {
        std::size_t operator()(const SpatialCellKey& key) const noexcept {
            return static_cast<std::size_t>(key.gx * 73856093) ^
                   static_cast<std::size_t>(key.gy * 19349663);
        }
    };

    static constexpr double kGridCellSize = 32768.0;
    std::unordered_map<SpatialCellKey, std::vector<int>, SpatialCellHash> spatial_grid_;
};

} // namespace igi::gameplay
