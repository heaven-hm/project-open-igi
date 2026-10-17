#include "igi/game_simulation.hpp"

#include "igi/diagnostics.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace igi::gameplay
{
namespace
{

constexpr float units_per_metre = 4096.0f;
constexpr std::array<std::uint32_t, 30> animation_identifiers{2,  4,  37, 35, 64, 70, 68, 11, 10, 57,
                                                              74, 72, 75, 77, 76, 81, 80, 14, 15, 19,
                                                              17, 41, 39, 27, 49, 47, 29, 28, 25, 2};

Vector3 metres(const WorldPosition3& position)
{
    return {static_cast<float>(position.x / units_per_metre),
            static_cast<float>(position.y / units_per_metre),
            static_cast<float>(position.z / units_per_metre)};
}

WorldPosition3 engine_position(Vector3 position)
{
    return {static_cast<double>(position.x) * units_per_metre,
            static_cast<double>(position.y) * units_per_metre,
            static_cast<double>(position.z) * units_per_metre};
}

// Original IGI.EXE game native: Ladder_Update at VA 0x004404C0 (static CFG score: 1.000).
// This C++ helper ports only the ladder activation-eligibility check, not the full native routine.
// Runtime parity for this helper has not yet been verified.
bool HumanPlayer_CanActivateLadder(const SceneLadder& ladder,
                                   Vector3 player_position_metres,
                                   float player_yaw,
                                   bool& activate_from_top,
                                   float& angular_error) noexcept
{
    activate_from_top = false;
    angular_error = std::numeric_limits<float>::max();
    const Vector3 player_position_units = player_position_metres * units_per_metre;
    const Vector3 ladder_origin_units = ladder.origin * units_per_metre;
    const Vector3 activation_top_units = ladder.activation_top * units_per_metre;
    const Vector3 from_origin = player_position_units - ladder_origin_units;
    const float across = dot(from_origin, ladder.orientation_column_one);
    const float normal_distance = dot(from_origin, ladder.orientation_column_two);
    const float forward_x = std::sin(player_yaw);
    const float forward_y = std::cos(player_yaw);
    const float side_sign = normal_distance > 0.0f ? -1.0f : 1.0f;
    const Vector3 target_direction = ladder.orientation_column_two * side_sign;
    const float target_length = std::sqrt(target_direction.x * target_direction.x +
                                          target_direction.y * target_direction.y);
    const float facing_dot = target_length > 0.0f
        ? (forward_x * target_direction.x + forward_y * target_direction.y) / target_length
        : -1.0f;
    const bool bottom_candidate = std::abs(normal_distance) < 8192.0f &&
                                  across > -2867.2f && across < 2867.2f &&
                                  facing_dot > 0.70710678f &&
                                  player_position_units.z > ladder_origin_units.z &&
                                  player_position_units.z < activation_top_units.z;
    if (bottom_candidate)
    {
        angular_error = std::acos(std::clamp(facing_dot, -1.0f, 1.0f));
    }

    const Vector3 from_activation_top = player_position_units - activation_top_units;
    const Vector3 to_activation_top = activation_top_units - player_position_units;
    const float top_distance = length(to_activation_top);
    const float top_forward = forward_x * to_activation_top.x + forward_y * to_activation_top.y;
    const bool top_candidate = dot(from_activation_top, ladder.orientation_column_two) < 0.0f &&
                               top_distance < 8192.0f && top_forward > 0.0f;
    if (top_candidate)
    {
        activate_from_top = true;
        angular_error = std::acos(std::clamp(top_forward / top_distance, -1.0f, 1.0f));
        return true;
    }
    return bottom_candidate;
}

Vector3 sample_translation(const RootMotionClip& clip, float time)
{
    if (clip.keys.empty())
    {
        return {};
    }
    if (time <= clip.keys.front().time)
    {
        return clip.keys.front().position;
    }
    for (std::size_t key_index = 1; key_index < clip.keys.size(); ++key_index)
    {
        const auto& next = clip.keys[key_index];
        if (time > next.time)
        {
            continue;
        }
        const auto& previous = clip.keys[key_index - 1];
        const float duration = next.time - previous.time;
        if (duration <= 0)
        {
            return next.position;
        }
        const float phase = (time - previous.time) / duration;
        const float phase_squared = phase * phase;
        const float phase_cubed = phase_squared * phase;
        return previous.position * (2 * phase_cubed - 3 * phase_squared + 1) +
               next.position * (-2 * phase_cubed + 3 * phase_squared) +
               previous.outgoing_tangent * (duration * (phase_cubed - 2 * phase_squared + phase)) +
               next.incoming_tangent * (duration * (phase_cubed - phase_squared));
    }
    return clip.keys.back().position;
}

} // namespace

GameSimulation::GameSimulation(const MissionScene& scene)
    : scene_(scene),
      collision_triangles_(scene.collision_triangles),
      movement_collision_triangles_(scene.movement_collision_triangles.empty() ? scene.collision_triangles
                                                                               : scene.movement_collision_triangles),
      world_(collision_triangles_),
      movement_world_(movement_collision_triangles_),
      doors_(scene.doors)
{
    if (!std::isfinite(scene.player_parameters.maximum_health) ||
        scene.player_parameters.maximum_health <= 0 ||
        !std::isfinite(scene.player_parameters.root_translation_scale) ||
        scene.player_parameters.root_translation_scale <= 0)
    {
        throw std::runtime_error("Original player health or root-motion scale is invalid");
    }
    restart();
}

int GameSimulation::player_locomotion_animation_identifier() const noexcept
{
    if (locomotion_state_ < 0 || static_cast<std::size_t>(locomotion_state_) >= animation_identifiers.size())
    {
        return -1;
    }
    return static_cast<int>(animation_identifiers[static_cast<std::size_t>(locomotion_state_)]);
}

void GameSimulation::restart()
{
    player_ = {};
    const auto floor = movement_world_.raycast({0, 0, 2.0f}, {0, 0, -1}, 40.0f, true);
    if (!floor.hit)
    {
        throw std::runtime_error("Player spawn has no collision floor in the original mission geometry");
    }
    player_.world_position = engine_position(floor.position);
    player_.movement_flags = HumanMovementIntegrator::grounded_flag;
    player_.wall_sweep_enabled = true;
    player_.ground_query_enabled = true;
    player_.separation_enabled = true;
    player_.slope_processing_enabled = true;
    player_.slope_normal = floor.normal;
    player_.collision_radius = 0.3f * units_per_metre;
    health_ = scene_.player_parameters.maximum_health;
    heading_ = scene_.player_heading;
    pitch_ = 0;
    crouching_ = false;
    aiming_ = false;
    animation_time_ = 0;
    previous_animation_state_ = -1;
    locomotion_state_ = 0;
    jump_was_pressed_ = false;
    tick_count_ = 0;
    developer_mode_ = false;
    pending_player_fall_sound_ = {};
    pending_player_jump_sound_ = {};
    current_zoom_ = 1.0f;
    dragunov_zoom_level_ = 6.5f;
    weapon_ = {};
    random_generator_ = RetailRandomGenerator{};
    weapon_.magazine = scene_.weapon_parameters.magazine_capacity;
    weapon_.reserve = scene_.initial_reserve_ammunition;
    inventory_.clear();
    selected_weapon_slot_ = 0;
    fire_was_pressed_ = false;
    damage_flash_remaining_ = 0;
    for (std::size_t definition_index = 0; definition_index < scene_.weapons.size(); ++definition_index)
    {
        const auto& definition = scene_.weapons[definition_index];
        if (!definition.initially_owned)
        {
            continue;
        }
        WeaponState state;
        state.magazine = definition.parameters.magazine_capacity;
        state.reserve = definition.initial_reserve;
        inventory_.push_back({definition_index, state});
        if (definition.identifier == "WEAPON_ID_MP5SD")
        {
            selected_weapon_slot_ = inventory_.size() - 1;
        }
    }
    if (!inventory_.empty())
    {
        weapon_ = inventory_[selected_weapon_slot_].state;
    }
    pickups_collected_.assign(scene_.weapon_pickups.size(), false);
    soldier_weapons_collected_.assign(scene_.actors.size(), false);
    actors_.clear();
    projectiles_.clear();
    for (const auto& original_actor : scene_.actors)
    {
        auto position = original_actor.position;
        const auto ground = movement_world_.raycast(position + Vector3{0, 0, 0.5f}, {0, 0, -1}, 6.0f, true);
        if (ground.hit)
        {
            position.z = ground.position.z;
        }
        ActorState actor;
        actor.position = position;
        actor.last_seen_position = position;
        actor.heading = original_actor.heading;
        actor.health = original_actor.maximum_health;
        actor.active = original_actor.gameplay_enabled;
        const auto weapon = std::find_if(
            scene_.weapons.begin(),
            scene_.weapons.end(),
            [&](const auto& candidate) { return candidate.identifier == original_actor.weapon_identifier; });
        actor.ammunition_in_clip = weapon == scene_.weapons.end() ? scene_.weapon_parameters.magazine_capacity
                                                                  : weapon->parameters.magazine_capacity;
        actor.animation_previous_position = position;
        actor.animation_identifier = original_actor.stand_animation;
        actors_.push_back(std::move(actor));
    }
    on_ladder_ = false;
    active_ladder_index_ = 0;
    objectives_ = scene_.objectives;
    mission_completed_ = false;
    doors_ = scene_.doors;
    prompt_message_.clear();
    interact_was_pressed_ = false;
    diagnostics::log(diagnostics::Severity::information,
                     "gameplay",
                     "Mission initialized; player grounded on original collision geometry");
}

void GameSimulation::teleport_player(Vector3 position, float heading, float pitch)
{
    heading_ = std::remainder(heading, 2.0f * std::numbers::pi_v<float>);
    pitch_ = std::clamp(pitch, -1.4f, 1.4f);
    const auto ground = movement_world_.raycast(position + Vector3{0, 0, 0.5f}, {0, 0, -1}, 50.0f, true);
    if (ground.hit)
    {
        position.z = ground.position.z;
        player_.slope_normal = ground.normal;
        player_.movement_flags |= HumanMovementIntegrator::grounded_flag;
    }
    else
    {
        player_.movement_flags &= ~HumanMovementIntegrator::grounded_flag;
    }
    player_.world_position = engine_position(position);
    player_.movement_velocity = {};
    player_.position_adjustment = {};
    player_.animation_movement = {};
    on_ladder_ = false;
    locomotion_state_ = 0;
    animation_time_ = 0;
    previous_animation_state_ = -1;
    diagnostics::log(diagnostics::Severity::information,
                     "player",
                     "Player teleported to (" + std::to_string(position.x) + ", " +
                         std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
}

Vector3 GameSimulation::player_position() const noexcept
{
    return metres(player_.world_position);
}

Vector3 GameSimulation::camera_position() const noexcept
{
    return player_position() + Vector3{0, 0, crouching_ ? 0.95f : 1.68f};
}

Vector3 GameSimulation::aim_direction() const noexcept
{
    const float current_view_heading = view_heading();
    const float current_view_pitch = view_pitch();
    return {-std::sin(current_view_heading) * std::cos(current_view_pitch),
            std::cos(current_view_heading) * std::cos(current_view_pitch),
            std::sin(current_view_pitch)};
}

bool GameSimulation::grounded() const noexcept
{
    return (player_.movement_flags & HumanMovementIntegrator::grounded_flag) != 0;
}

void GameSimulation::tick(const PlayerInput& input)
{
    pending_player_fall_sound_ = {};
    pending_player_jump_sound_ = {};
    if (!std::isfinite(input.forward) || !std::isfinite(input.sideways) ||
        !std::isfinite(input.heading_change) || !std::isfinite(input.pitch_change))
    {
        throw std::runtime_error("Player input contains a non-finite value");
    }
    heading_ = std::remainder(heading_ + input.heading_change, 2.0f * std::numbers::pi_v<float>);
    pitch_ = std::clamp(pitch_ + input.pitch_change, -1.4f, 1.4f);
    aiming_ = input.aiming;
    const bool scoped = is_scoped();
    const float target_zoom = aiming_ ? (scoped ? dragunov_zoom_level_ : 1.55f) : 1.0f;
    current_zoom_ += (target_zoom - current_zoom_) * (scoped ? 0.35f : 0.45f);
    if (std::abs(target_zoom - current_zoom_) < 0.01f)
    {
        current_zoom_ = target_zoom;
    }
    if (developer_mode_)
    {
        health_ = scene_.player_parameters.maximum_health;
        weapon_.reserve = 999;
        if (selected_weapon_slot_ < inventory_.size())
        {
            inventory_[selected_weapon_slot_].state.reserve = 999;
        }
    }
    damage_flash_remaining_ = std::max(0.0f, damage_flash_remaining_ - 1.0f / 30.0f);
    if (health_ > 0)
    {
        if (input.weapon_slot >= 0)
        {
            select_weapon(static_cast<std::size_t>(input.weapon_slot));
        }
        if (input.weapon_category >= 0 &&
            static_cast<std::size_t>(input.weapon_category) < scene_.weapon_categories.size())
        {
            const auto& category = scene_.weapon_categories[input.weapon_category];
            for (const auto& identifier : category)
            {
                const auto item = std::find_if(
                    inventory_.begin(),
                    inventory_.end(),
                    [&](const auto& candidate)
                    { return scene_.weapons[candidate.definition_index].identifier == identifier; });
                if (item != inventory_.end())
                {
                    select_weapon(static_cast<std::size_t>(item - inventory_.begin()));
                    break;
                }
            }
        }
        if (input.weapon_cycle != 0 && !inventory_.empty())
        {
            const auto count = static_cast<int>(inventory_.size());
            const auto next =
                (static_cast<int>(selected_weapon_slot_) + (input.weapon_cycle > 0 ? 1 : -1) + count) % count;
            select_weapon(static_cast<std::size_t>(next));
        }
        collect_weapons(input);
        advance_player(input);
        advance_weapon(input);
        weapon_.accumulated_recoil_pitch =
            (weapon_.sampled_recoil_pitch + weapon_.accumulated_recoil_pitch) * 0.899999976f;
        weapon_.accumulated_recoil_yaw =
            (weapon_.sampled_recoil_yaw + weapon_.accumulated_recoil_yaw) * 0.899999976f;
        if (!std::isfinite(weapon_.accumulated_recoil_pitch) ||
            !std::isfinite(weapon_.accumulated_recoil_yaw))
        {
            throw std::runtime_error("Accumulated player view recoil became non-finite");
        }
        advance_view_animation(input);

        // Interactive doors & prompt
        for (auto& door : doors_)
        {
            const float mid_x = (door.min_bounds.x + door.max_bounds.x) * 0.5f;
            const float mid_y = (door.min_bounds.y + door.max_bounds.y) * 0.5f;
            const Vector3 door_center = door.position + rotate_euler(Vector3{mid_x, mid_y, 1.0f}, door.rotation);
            const float dist_hinge = length(player_position() - door.position);
            const float dist_center = length(player_position() - door_center);
            if (dist_hinge < 4.2f || dist_center < 4.2f)
            {
                if (prompt_timer_ <= 0.0f)
                {
                    prompt_message_ = door.open ? "PRESS Q OR F TO CLOSE DOOR" : "PRESS Q OR F TO OPEN DOOR";
                }
                if (input.interacting && !interact_was_pressed_)
                {
                    door.open = !door.open;
                    diagnostics::log(diagnostics::Severity::information,
                                     "gameplay",
                                     std::string("Door ") + (door.open ? "opened" : "closed") + " at (" +
                                         std::to_string(door.position.x) + ", " +
                                         std::to_string(door.position.y) + ", " +
                                         std::to_string(door.position.z) + ")");
                }
                break;
            }
        }
        if (prompt_timer_ > 0.0f)
        {
            prompt_timer_ = std::max(0.0f, prompt_timer_ - 1.0f / 30.0f);
        }
        else if (prompt_message_.empty() || prompt_message_.find("LADDER") != std::string::npos)
        {
            prompt_message_.clear();
            for (const auto& ladder : scene_.ladders)
            {
                bool activate_from_top = false;
                float angular_error = std::numeric_limits<float>::max();
                if (HumanPlayer_CanActivateLadder(
                        ladder, player_position(), heading_, activate_from_top, angular_error))
                {
                    static_cast<void>(activate_from_top);
                    prompt_message_ = on_ladder_ ? "CLIMBING LADDER: W/S TO MOVE, JUMP/Q TO DISMOUNT"
                                                 : "PRESS W OR Q TO CLIMB LADDER";
                    break;
                }
            }
        }
    }
    advance_actors();
    advance_actor_animations();
    advance_projectiles();
    const auto player_current_pos = player_position();
    for (auto& obj : objectives_)
    {
        if (!obj.completed)
        {
            const float dist = length(player_current_pos - obj.target_position);
            if (dist <= obj.trigger_radius)
            {
                obj.completed = true;
                mission_completed_ = true;
                diagnostics::log(diagnostics::Severity::information,
                                 "mission",
                                 "Objective completed: " + obj.text);
            }
        }
    }
    fire_was_pressed_ = input.firing;
    interact_was_pressed_ = input.interacting;
    ++tick_count_;
    if (!finite(player_position()) || std::abs(player_position().z) > 5000)
    {
        throw std::runtime_error("Player simulation left the loaded world; restart the mission");
    }
}

Vector3 GameSimulation::animation_delta(int state)
{
    if (state < 0 || static_cast<std::size_t>(state) >= animation_identifiers.size())
    {
        return {};
    }
    const auto found = scene_.root_motion_clips.find(animation_identifiers[state]);
    if (found == scene_.root_motion_clips.end() || found->second.duration <= 0)
    {
        throw std::runtime_error("Required locomotion animation is missing or has zero duration");
    }
    const auto& clip = found->second;
    if (state != previous_animation_state_)
    {
        animation_time_ = 0;
        previous_animation_state_ = state;
    }
    const auto before = sample_translation(clip, animation_time_);
    float next_time = animation_time_ + 160.0f;
    Vector3 delta;
    if (next_time >= clip.duration)
    {
        next_time = std::fmod(next_time, clip.duration);
        delta = sample_translation(clip, clip.duration) - before + sample_translation(clip, next_time) -
                sample_translation(clip, 0);
    }
    else
    {
        delta = sample_translation(clip, next_time) - before;
    }
    animation_time_ = next_time;
    return delta * scene_.player_parameters.root_translation_scale;
}

void GameSimulation::advance_player(const PlayerInput& input)
{
    bool just_mounted = false;
    if (!on_ladder_)
    {
        const auto p = player_position();
        float smallest_angular_error = std::numeric_limits<float>::max();
        for (std::size_t idx = 0; idx < scene_.ladders.size(); ++idx)
        {
            const auto& ladder = scene_.ladders[idx];
            bool activate_from_top = false;
            float angular_error = std::numeric_limits<float>::max();
            if ((input.forward != 0 || input.interacting) &&
                HumanPlayer_CanActivateLadder(ladder, p, heading_, activate_from_top, angular_error) &&
                angular_error < smallest_angular_error)
            {
                active_ladder_index_ = idx;
                smallest_angular_error = angular_error;
                just_mounted = true;
                // Original IGI.EXE game native: Ladder_Mount at VA 0x004113E0 (static CFG score: 0.9885).
                // This C++ path ports the player placement and heading effects; runtime parity is unverified.
                const Vector3 mount_position = activate_from_top ? ladder.top_mount : ladder.bottom_mount;
                player_.world_position = engine_position(mount_position);
                heading_ = std::remainder(ladder.player_yaw, 2.0f * std::numbers::pi_v<float>);
                diagnostics::log(diagnostics::Severity::information,
                                 "player",
                                 "IGI.EXE Ladder_Mount native reference (VA 0x004113E0): ladder=" +
                                     std::to_string(idx) + (activate_from_top ? ", approach=top" : ", approach=foot"));
            }
        }
        on_ladder_ = just_mounted;
        if (on_ladder_)
        {
            player_.movement_flags &= ~HumanMovementIntegrator::grounded_flag;
            player_.movement_velocity = {};
        }
    }

    if (on_ladder_ && active_ladder_index_ < scene_.ladders.size())
    {
        const auto& ladder = scene_.ladders[active_ladder_index_];
        if (!just_mounted && (input.jumping || (input.interacting && !interact_was_pressed_)))
        {
            on_ladder_ = false;
            player_.movement_velocity = {-ladder.normal.x * 2.0f * units_per_metre / 30.0f,
                                         -ladder.normal.y * 2.0f * units_per_metre / 30.0f,
                                         3.0f * units_per_metre / 30.0f};
            diagnostics::log(diagnostics::Severity::debug, "player", "Dismounted ladder");
        }
        else
        {
            float climb_speed = 0.0f;
            if (input.forward > 0)
            {
                climb_speed = 2.4f / 30.0f;
            }
            else if (input.forward < 0)
            {
                climb_speed = input.crouching ? -4.5f / 30.0f : -2.4f / 30.0f;
            }
            auto pos = player_position();
            pos.z += climb_speed;
            pos.x = ladder.bottom.x + ladder.normal.x * 0.4f;
            pos.y = ladder.bottom.y + ladder.normal.y * 0.4f;

            if (pos.z >= ladder.top.z && input.forward > 0)
            {
                on_ladder_ = false;
                pos = ladder.top_mount;
                player_.world_position = engine_position(pos);
                player_.movement_velocity = {};
                diagnostics::log(diagnostics::Severity::debug, "player", "Reached top of ladder");
            }
            else if (pos.z <= ladder.bottom.z && input.forward < 0)
            {
                on_ladder_ = false;
                pos = ladder.bottom_mount;
                player_.world_position = engine_position(pos);
                player_.movement_velocity = {};
                diagnostics::log(diagnostics::Severity::debug, "player", "Reached bottom of ladder");
            }
            else
            {
                player_.world_position = engine_position(pos);
                return;
            }
        }
    }

    if (input.crouching || !crouching_ ||
        !movement_world_.raycast(player_position() + Vector3{0, 0, 1.05f}, {0, 0, 1}, 0.76f).hit)
    {
        crouching_ = input.crouching;
    }
    const bool moving = input.forward != 0 || input.sideways != 0;
    int direction = 0;
    if (input.forward > 0)
    {
        direction = input.sideways > 0 ? 2 : input.sideways < 0 ? 3 : 1;
    }
    else if (input.forward < 0)
    {
        direction = input.sideways > 0 ? 5 : input.sideways < 0 ? 6 : 4;
    }
    else if (input.sideways != 0)
    {
        direction = input.sideways > 0 ? 7 : 8;
    }
    locomotion_state_ = direction == 0 ? (crouching_ ? 19 : 0)
                                       : direction + (crouching_      ? 19
                                                      : input.walking ? 8
                                                                      : 0);
    bool took_off_this_tick = false;
    if (input.jumping && !jump_was_pressed_ && grounded() && !crouching_)
    {
        const float horizontal_impulse = scene_.player_parameters.jump_horizontal_per_tick * units_per_metre;
        player_.movement_velocity =
            HumanMotion::launch_velocity(player_.animation_movement,
                                         player_.position_adjustment,
                                         horizontal_impulse,
                                         scene_.player_parameters.jump_vertical_per_tick * units_per_metre);
        player_.position_adjustment = {};
        took_off_this_tick = true;
        pending_player_jump_sound_ = "jump_1";
        player_.movement_flags &= ~HumanMovementIntegrator::grounded_flag;
        diagnostics::log(
            diagnostics::Severity::debug, "player", "Jump impulse applied from original player parameters");
    }
    jump_was_pressed_ = input.jumping;
    const Basis3x3 basis{
        std::cos(heading_), -std::sin(heading_), 0, std::sin(heading_), std::cos(heading_), 0, 0, 0, 1};
    player_.animation_movement = {};
    if (!grounded() || !moving)
    {
        previous_animation_state_ = -1;
        animation_time_ = 0;
    }
    if (!grounded())
    {
        locomotion_state_ = player_.movement_velocity.z > 0 ? 28 : 29;
        if (!took_off_this_tick)
        {
            player_.movement_velocity = HumanMotion::integrate_airborne(
                player_.movement_velocity,
                input.sideways < 0,
                input.sideways > 0,
                input.forward > 0,
                input.forward < 0,
                basis,
                scene_.player_parameters.airborne_control_per_tick * units_per_metre);
        }
    }
    else if (moving)
    {
        player_.animation_movement =
            HumanMotion::apply_root_motion(animation_delta(locomotion_state_), basis, 1.0f);
    }
    integrator_.update(player_, *this);
}

void GameSimulation::sweep_walls(HumanMovementState& human,
                                 const WorldPosition3& starting,
                                 WorldPosition3& destination,
                                 Vector3& delta)
{
    const auto requested = metres(destination) - metres(starting);
    const float height = crouching_ ? 1.1f : 1.8f;
    auto resolved = movement_world_.move_capsule(metres(starting), requested, height);
    const auto initial = metres(starting);
    const Vector3 horizontal{requested.x, requested.y, 0};
    const float requested_progress = length(horizontal);
    const float resolved_progress = length(Vector3{resolved.x - initial.x, resolved.y - initial.y, 0});
    if (grounded() && requested_progress > 0.001f && resolved_progress < requested_progress * 0.8f &&
        !movement_world_.raycast(initial + Vector3{0, 0, height}, {0, 0, 1}, 0.35f).hit)
    {
        auto stepped = movement_world_.move_capsule(initial + Vector3{0, 0, 0.35f}, horizontal, height);
        const auto floor = movement_world_.raycast(stepped + Vector3{0, 0, 0.02f}, {0, 0, -1}, 0.42f, true);
        const float stepped_progress = length(Vector3{stepped.x - initial.x, stepped.y - initial.y, 0});
        if (floor.hit && stepped_progress > resolved_progress + 0.001f &&
            floor.position.z <= initial.z + 0.351f)
        {
            stepped.z = floor.position.z;
            resolved = stepped;
        }
    }
    if (requested.z > 0 && resolved.z < metres(destination).z - 0.001f)
    {
        human.movement_velocity.z = 0;
    }
    destination = engine_position(resolved);
    delta = {static_cast<float>(destination.x - starting.x),
             static_cast<float>(destination.y - starting.y),
             static_cast<float>(destination.z - starting.z)};
}

void GameSimulation::query_ground(HumanMovementState& human,
                                  const WorldPosition3& starting,
                                  WorldPosition3& destination,
                                  Vector3& delta,
                                  float query_budget)
{
    const auto from = metres(starting);
    auto target = metres(destination);
    const bool was_grounded = grounded();
    const float upward_budget = was_grounded ? 0.35f : 0.04f;
    const float downward_budget = was_grounded ? std::max(0.1f, query_budget / units_per_metre)
                                               : std::max(0.1f, from.z - target.z + 0.08f);
    const auto ground = movement_world_.raycast(
        {target.x, target.y, from.z + upward_budget}, {0, 0, -1}, upward_budget + downward_budget, true);
    if (ground.hit && human.movement_velocity.z <= 0 &&
        (was_grounded || target.z <= ground.position.z + 0.04f))
    {
        target.z = ground.position.z;
        human.movement_flags |= HumanMovementIntegrator::grounded_flag;
        human.slope_normal = ground.normal;
        if (!was_grounded)
        {
            // Original IGI.EXE 0x00408460 applies gravity before this landing-speed check.
            const HumanFallImpact fall_impact = HumanFallDamage_Calculate(
                human.movement_velocity.z,
                scene_.player_parameters.maximum_health);
            pending_player_fall_sound_ = fall_impact.sound_definition_name;
            if (!developer_mode_ && fall_impact.damage > 0.0F && health_ > 0.0F)
            {
                health_ = std::max(0.0f, health_ - fall_impact.damage);
                damage_flash_remaining_ = 0.35f;
                diagnostics::log(diagnostics::Severity::information,
                                 "player",
                                 "Original IGI.EXE HumanPlayer fall-impact native 0x00408460: damage=" +
                                     std::to_string(fall_impact.damage) + ", speed=" +
                                     std::to_string(fall_impact.speed_metres_per_second) + " m/s, health=" +
                                     std::to_string(health_));
                for (auto& actor : actors_)
                {
                    if (!actor.active || actor.behavior == ActorBehavior::dead)
                    {
                        continue;
                    }
                    const float dist = length(actor.position - target);
                    if (dist < 10.0f && actor.behavior != ActorBehavior::attacking)
                    {
                        actor.behavior = ActorBehavior::investigating;
                        actor.last_seen_position = target;
                        actor.awareness_time = 5.0f;
                    }
                }
            }
            human.movement_velocity = {};
            human.impact_velocity = {};
        }
        else
        {
            human.movement_velocity.z = 0;
        }
    }
    else
    {
        human.movement_flags &= ~HumanMovementIntegrator::grounded_flag;
    }
    destination = engine_position(target);
    delta.z = static_cast<float>(destination.z - starting.z);
}

void GameSimulation::separate_humans(HumanMovementState&,
                                     const WorldPosition3& starting,
                                     WorldPosition3& destination,
                                     Vector3& delta)
{
    auto position = metres(destination);
    for (const auto& actor : actors_)
    {
        if (!actor.active || actor.behavior == ActorBehavior::dead ||
            std::abs(actor.position.z - position.z) > 1.7f)
        {
            continue;
        }
        const Vector3 difference{position.x - actor.position.x, position.y - actor.position.y, 0};
        const float distance = length(difference);
        if (distance > 0.0001f && distance < 0.62f)
        {
            position = movement_world_.move_capsule(
                position, difference * ((0.62f - distance) / distance), crouching_ ? 1.1f : 1.8f);
        }
    }
    for (const auto& door : doors_)
    {
        if (door.open)
        {
            continue;
        }
        const Vector3 rel = position - door.position;
        if (length(rel) > 6.0f)
        {
            continue;
        }
        const float cos_y = std::cos(-door.rotation.z);
        const float sin_y = std::sin(-door.rotation.z);
        const Vector3 local_pos{cos_y * rel.x - sin_y * rel.y, sin_y * rel.x + cos_y * rel.y, rel.z};
        const auto start_pos = metres(starting);
        const Vector3 start_rel = start_pos - door.position;
        const Vector3 start_local{cos_y * start_rel.x - sin_y * start_rel.y, sin_y * start_rel.x + cos_y * start_rel.y, start_rel.z};
        constexpr float radius = 0.40f;
        Vector3 min_b = door.min_bounds;
        Vector3 max_b = door.max_bounds;
        if (min_b.x >= max_b.x || min_b.y >= max_b.y)
        {
            min_b = {-0.25f, -0.8f, 0.0f};
            max_b = {0.25f, 0.8f, 2.4f};
        }
        if (local_pos.z + 1.8f >= min_b.z && local_pos.z <= max_b.z + 0.2f &&
            local_pos.x >= min_b.x - radius && local_pos.x <= max_b.x + radius &&
            local_pos.y >= min_b.y - radius && local_pos.y <= max_b.y + radius)
        {
            const float size_x = max_b.x - min_b.x;
            const float size_y = max_b.y - min_b.y;
            float resolved_x = local_pos.x;
            float resolved_y = local_pos.y;
            if (size_x < size_y)
            {
                // Door pane thickness is along X
                const float mid_x = (min_b.x + max_b.x) * 0.5f;
                resolved_x = (start_local.x >= mid_x) ? max_b.x + radius + 0.02f : min_b.x - radius - 0.02f;
            }
            else
            {
                // Door pane thickness is along Y
                const float mid_y = (min_b.y + max_b.y) * 0.5f;
                resolved_y = (start_local.y >= mid_y) ? max_b.y + radius + 0.02f : min_b.y - radius - 0.02f;
            }
            const float cos_f = std::cos(door.rotation.z);
            const float sin_f = std::sin(door.rotation.z);
            position.x = cos_f * resolved_x - sin_f * resolved_y + door.position.x;
            position.y = sin_f * resolved_x + cos_f * resolved_y + door.position.y;
        }
    }
    destination = engine_position(position);
    delta = {static_cast<float>(destination.x - starting.x),
             static_cast<float>(destination.y - starting.y),
             static_cast<float>(destination.z - starting.z)};
}

void GameSimulation::commit_transform(HumanMovementState& human, const WorldPosition3& destination, float)
{
    human.world_position = destination;
}

bool GameSimulation::is_scoped() const noexcept
{
    const auto* def = weapon_definition();
    if (!aiming_ || def == nullptr)
    {
        return false;
    }
    return def->identifier == "WEAPON_ID_DRAGUNOV" ||
           def->identifier.find("DRAGUNOV") != std::string::npos ||
           def->identifier.find("Dragunov") != std::string::npos;
}

void GameSimulation::adjust_zoom(int direction) noexcept
{
    if (direction > 0)
    {
        dragunov_zoom_level_ = std::min(14.0f, dragunov_zoom_level_ + 1.0f);
    }
    else if (direction < 0)
    {
        dragunov_zoom_level_ = std::max(3.0f, dragunov_zoom_level_ - 1.0f);
    }
}

void GameSimulation::toggle_developer_mode()
{
    developer_mode_ = !developer_mode_;
    if (developer_mode_)
    {
        for (std::size_t def_idx = 0; def_idx < scene_.weapons.size(); ++def_idx)
        {
            const auto& def = scene_.weapons[def_idx];
            auto it = std::find_if(inventory_.begin(),
                                   inventory_.end(),
                                   [&](const auto& item) { return item.definition_index == def_idx; });
            if (it != inventory_.end())
            {
                it->state.magazine = def.parameters.magazine_capacity;
                it->state.reserve = 999;
            }
            else
            {
                WeaponState ws{};
                ws.magazine = def.parameters.magazine_capacity;
                ws.reserve = 999;
                inventory_.push_back({def_idx, ws});
            }
        }
        weapon_.reserve = 999;
        const auto* cur_def = weapon_definition();
        if (cur_def)
        {
            weapon_.magazine = cur_def->parameters.magazine_capacity;
        }
        health_ = scene_.player_parameters.maximum_health;
        diagnostics::log(diagnostics::Severity::information,
                         "developer",
                         "Player-only cheat mode enabled: all player weapons granted, unlimited player ammo, player god mode");
        set_prompt_message("CHEAT MODE: GOD MODE + ALL WEAPONS + INF AMMO", 3.5f);
    }
    else
    {
        diagnostics::log(diagnostics::Severity::information, "developer", "Developer mode disabled");
        set_prompt_message("CHEAT MODE: DISABLED", 2.0f);
    }
}

void GameSimulation::set_prompt_message(std::string message, float duration)
{
    prompt_message_ = std::move(message);
    prompt_timer_ = duration;
}

} // namespace igi::gameplay
