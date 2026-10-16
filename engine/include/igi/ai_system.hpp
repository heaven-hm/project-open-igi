#pragma once

#include "igi/mission_world.hpp"
#include "igi/weapon_system.hpp"
#include "igi/world_collision.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace igi::gameplay {

enum class SoldierState : std::uint8_t {
    Idle,
    Patrol,
    Investigate, // heard something / lost sight, moving to last known position
    Combat,
    Dead,
};

struct SoldierAgent {
    std::size_t spawn_index{};
    NavigationPosition position{};
    float yaw{};
    float health{100.0f};
    SoldierState state{SoldierState::Idle};

    std::size_t patrol_cursor{};
    float wait_seconds{};
    bool has_walk_target{};
    bool walk_is_run{};
    NavigationPosition walk_target{};
    // Edge-following route: intermediate retail graph node positions leading
    // to walk_target. Empty means walk_target is approached directly.
    std::vector<NavigationPosition> route_waypoints{};

    NavigationPosition last_known_player{};
    float perception_timer{};
    float lose_sight_timer{};
    float fire_timer{};
    float reaction_timer{0.8f};
    float burst_cooldown{0.0f};
    int burst_shots{0};
    float death_seconds{};
    bool sees_player{};
    float walk_phase{0.0f};
    bool is_firing{false};

    [[nodiscard]] bool alive() const noexcept { return state != SoldierState::Dead; }
};

struct SoldierShotHit {
    bool hit{};
    std::size_t soldier_index{};
    float fraction{1.0f};
    bool is_headshot{};
    bool is_lethal{};
};

// Portable enemy AI. Thresholds follow AI_PERCEPTION_AND_PURSUIT_RESEARCH.md.
// Speeds and combat accuracy are provisional (not yet measured from retail).
class AiSystem final {
public:
    static constexpr float kPrimarySightMeters = 30.0f;
    static constexpr float kSecondarySightMeters = 10.0f;
    static constexpr float kLosClearFraction = 0.79f;
    static constexpr float kWaypointArrivalUnits = 1228.8f;
    static constexpr float kTurnRadiansPerRetailTick = 0.025925925f;
    static constexpr float kFireConeCosine = 0.96f;
    static constexpr float kCylinderHeight = 7372.8f;
    static constexpr float kCylinderRadius = 1638.4f;

    void initialize(const MissionWorld& world);

    // Returns damage dealt to the player this step.
    [[nodiscard]] float update(
        float delta_seconds,
        const MissionWorld& world,
        const WorldCollision& collision,
        const PlayerPhysicsState& player) noexcept;

    // Broadcast of a gunshot heard within `audible_range_units` of `origin`.
    void notify_gunshot(NavigationPosition origin, float audible_range_units) noexcept;

    // Tests a shot ray against soldier cylinders; applies damage on hit.
    // `max_fraction` limits the ray (e.g. a wall hit fraction).
    [[nodiscard]] SoldierShotHit apply_shot(const ShotRay& shot, float max_fraction) noexcept;

    [[nodiscard]] const std::vector<SoldierAgent>& agents() const noexcept { return agents_; }
    [[nodiscard]] std::size_t alive_count() const noexcept;
    [[nodiscard]] std::vector<NavigationPosition> alive_positions() const;

    // BFS shortest node-id route over retail graph links (per-node link1/link2
    // plus the edge list), treated as undirected. Returns the node id sequence
    // from origin to target inclusive; empty when either id is unknown or the
    // target is unreachable.
    [[nodiscard]] static std::vector<int> find_graph_route(
        const igi::assets::NavigationGraph& graph, int from_node_id, int to_node_id);
    [[nodiscard]] static int nearest_graph_node_id(
        const igi::assets::NavigationGraph& graph,
        NavigationPosition position) noexcept;

private:
    void assign_route_target(
        SoldierAgent& agent,
        const igi::assets::NavigationGraph& graph,
        int target_node_id,
        NavigationPosition target_position,
        bool is_run) noexcept;
    void update_perception(
        SoldierAgent& agent,
        const MissionWorld& world,
        const WorldCollision& collision,
        const PlayerPhysicsState& player) noexcept;
    void update_patrol(SoldierAgent& agent, const MissionWorld& world, float dt) noexcept;
    [[nodiscard]] bool step_toward(
        SoldierAgent& agent,
        const WorldCollision& collision,
        const MissionWorld& world,
        NavigationPosition target,
        float speed_units_per_second,
        float dt) noexcept;
    void turn_toward(SoldierAgent& agent, NavigationPosition target, float dt) noexcept;
    [[nodiscard]] float next_random() noexcept;

    std::vector<SoldierAgent> agents_;
    std::uint32_t rng_state_{0x1234ABCDu};
};

} // namespace igi::gameplay
