#pragma once

#include "igi/collision_world.hpp"
#include "igi/human_movement_integrator.hpp"
#include "igi/retail_random_generator.hpp"

namespace igi::gameplay
{

struct PlayerInput final
{
    float forward{};
    float sideways{};
    float heading_change{};
    float pitch_change{};
    bool walking{};
    bool crouching{};
    bool jumping{};
    bool firing{};
    bool reloading{};
    bool aiming{};
    int weapon_slot = -1;
    int weapon_cycle{};
    bool interacting{};
    int weapon_category = -1;
};

enum class ActorBehavior
{
    guarding,
    investigating,
    attacking,
    dead
};

struct ActorState final
{
    Vector3 position;
    Vector3 last_seen_position;
    float heading{};
    float health = 1.0f;
    float awareness_time{};
    ActorBehavior behavior = ActorBehavior::guarding;
    Vector3 tracked_aim_position{};
    float hit_probability = 1.0f;
    std::uint32_t ammunition_in_clip{};
    std::int32_t weapon_cooldown_ticks_remaining{};
    std::int32_t rounds_remaining_in_burst{};
    std::int32_t rounds_fired_in_burst{};
    std::int32_t shot_delay_ticks_remaining{};
    float reload_remaining{};
    bool has_tracked_aim_position{};
    bool burst_active{};
    bool first_fire = true;
    float patrol_delay{};
    std::size_t patrol_command_index{};
    std::vector<Vector3> route;
    std::size_t route_index{};
    Vector3 route_destination;
    float route_retry_time{};
    bool active = true;
    Vector3 animation_previous_position{};
    float animation_time{};
    int animation_identifier = 2;
    float muzzle_flash_remaining{};
    std::uint64_t shots_fired{};
};

struct WeaponState final
{
    float sampled_recoil_pitch{};
    float sampled_recoil_yaw{};
    float accumulated_recoil_pitch{};
    float accumulated_recoil_yaw{};
    std::uint32_t magazine{};
    std::uint32_t reserve = 196;
    std::int32_t cooldown_ticks_remaining{};
    float reload_remaining{};
    float muzzle_flash_remaining{};
    float hit_marker_remaining{};
    std::uint64_t shots_fired{};
    std::uint64_t hits{};
    std::uint64_t kills{};
    std::uint32_t rounds_in_current_burst{};
    int animation_identifier = -1;
    float animation_time{};
    int fire_variant{};
    bool is_striking{};
};

struct Projectile final
{
    Vector3 position;
    Vector3 direction;
    float remaining_distance = 400.0f;
    float speed{};
    float damage{};
    int owner = -1;
    float hit_probability = 1.0f;
};

struct InventoryWeapon final
{
    std::size_t definition_index{};
    WeaponState state;
};

class GameSimulation final : private HumanMovementWorld
{
  public:
    explicit GameSimulation(const MissionScene& scene);
    void restart();
    void tick(const PlayerInput& input);
    [[nodiscard]] Vector3 player_position() const noexcept;
    [[nodiscard]] Vector3 camera_position() const noexcept;
    [[nodiscard]] Vector3 aim_direction() const noexcept;

    [[nodiscard]] float heading() const noexcept
    {
        return heading_;
    }

    [[nodiscard]] float pitch() const noexcept
    {
        return pitch_;
    }

    [[nodiscard]] float view_heading() const noexcept
    {
        return heading_ + weapon_.accumulated_recoil_yaw;
    }

    [[nodiscard]] float view_pitch() const noexcept
    {
        return pitch_ + weapon_.accumulated_recoil_pitch;
    }

    [[nodiscard]] float health() const noexcept
    {
        return health_;
    }

    [[nodiscard]] int locomotion_state() const noexcept
    {
        return locomotion_state_;
    }

    [[nodiscard]] bool grounded() const noexcept;

    [[nodiscard]] bool aiming() const noexcept
    {
        return aiming_;
    }

    [[nodiscard]] bool crouching() const noexcept
    {
        return crouching_;
    }

    [[nodiscard]] bool on_ladder() const noexcept
    {
        return on_ladder_;
    }

    [[nodiscard]] const std::vector<MissionObjective>& objectives() const noexcept
    {
        return objectives_;
    }

    [[nodiscard]] bool mission_completed() const noexcept
    {
        return mission_completed_;
    }

    [[nodiscard]] const std::vector<SceneDoor>& doors() const noexcept
    {
        return doors_;
    }

    [[nodiscard]] const std::string& prompt_message() const noexcept
    {
        return prompt_message_;
    }

    void teleport_player(Vector3 position, float heading, float pitch);

    [[nodiscard]] const WeaponState& weapon() const noexcept
    {
        return weapon_;
    }

    [[nodiscard]] const WeaponParameters& weapon_parameters() const noexcept;
    [[nodiscard]] std::shared_ptr<SceneMesh> weapon_mesh() const noexcept;

    [[nodiscard]] std::size_t inventory_size() const noexcept
    {
        return inventory_.size();
    }

    [[nodiscard]] std::size_t selected_weapon_slot() const noexcept
    {
        return selected_weapon_slot_;
    }

    [[nodiscard]] float damage_flash_remaining() const noexcept
    {
        return damage_flash_remaining_;
    }

    [[nodiscard]] const SceneWeapon* weapon_definition() const noexcept;

    [[nodiscard]] const std::vector<ActorState>& actors() const noexcept
    {
        return actors_;
    }

    [[nodiscard]] const std::vector<Projectile>& projectiles() const noexcept
    {
        return projectiles_;
    }

    [[nodiscard]] std::array<std::uint32_t, 3> random_state() const noexcept
    {
        return random_generator_.state();
    }

    [[nodiscard]] const CollisionWorld& world() const noexcept
    {
        return world_;
    }

    [[nodiscard]] const CollisionWorld& movement_world() const noexcept
    {
        return movement_world_;
    }

    [[nodiscard]] std::uint64_t tick_count() const noexcept
    {
        return tick_count_;
    }

    void set_difficulty(int difficulty) noexcept
    {
        difficulty_ = difficulty;
    }

    [[nodiscard]] int difficulty() const noexcept
    {
        return difficulty_;
    }

    void toggle_developer_mode();

    [[nodiscard]] bool developer_mode() const noexcept
    {
        return developer_mode_;
    }

    [[nodiscard]] bool is_scoped() const noexcept;

    [[nodiscard]] float zoom_factor() const noexcept
    {
        return current_zoom_;
    }

    void adjust_zoom(int direction) noexcept;

  private:
    void advance_player(const PlayerInput& input);
    void advance_weapon(const PlayerInput& input);
    void advance_actors();
    void advance_projectiles();
    void advance_actor_animations();
    void advance_view_animation(const PlayerInput& input);
    void select_weapon(std::size_t slot);
    void collect_weapons(const PlayerInput& input);
    void advance_patrol(std::size_t actor_index);
    void move_actor(std::size_t actor_index, Vector3 destination, float speed);
    [[nodiscard]] Vector3 animation_delta(int state);
    void sweep_walls(HumanMovementState&, const WorldPosition3&, WorldPosition3&, Vector3&) override;
    void query_ground(HumanMovementState&, const WorldPosition3&, WorldPosition3&, Vector3&, float) override;
    void separate_humans(HumanMovementState&, const WorldPosition3&, WorldPosition3&, Vector3&) override;
    void commit_transform(HumanMovementState&, const WorldPosition3&, float) override;
    const MissionScene& scene_;
    std::vector<CollisionTriangle> collision_triangles_;
    std::vector<CollisionTriangle> movement_collision_triangles_;
    CollisionWorld world_;
    CollisionWorld movement_world_;
    std::vector<SceneDoor> doors_;
    std::string prompt_message_;
    bool interact_was_pressed_{};
    HumanMovementState player_;
    HumanMovementIntegrator integrator_;
    WeaponState weapon_;
    RetailRandomGenerator random_generator_;
    std::vector<ActorState> actors_;
    std::vector<Projectile> projectiles_;
    std::vector<InventoryWeapon> inventory_;
    std::vector<bool> pickups_collected_;
    std::vector<bool> soldier_weapons_collected_;
    std::size_t selected_weapon_slot_{};
    bool fire_was_pressed_{};
    float damage_flash_remaining_{};
    float heading_{};
    float pitch_{};
    float health_{};
    float animation_time_{};
    int locomotion_state_{};
    int previous_animation_state_ = -1;
    bool jump_was_pressed_{};
    bool crouching_{};
    bool aiming_{};
    bool on_ladder_{};
    std::size_t active_ladder_index_{};
    std::vector<MissionObjective> objectives_;
    bool mission_completed_{};
    int difficulty_{1};
    std::uint64_t tick_count_{};
    bool developer_mode_{};
    float current_zoom_{1.0f};
    float dragunov_zoom_level_{6.5f};
};

} // namespace igi::gameplay
