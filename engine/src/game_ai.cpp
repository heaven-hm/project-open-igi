#include "igi/diagnostics.hpp"
#include "igi/game_simulation.hpp"
#include "igi/navigation.hpp"
#include "igi/skeletal_animation.hpp"
#include "igi/weapon_accuracy.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace igi::gameplay
{
namespace
{

constexpr float tick_duration = 1.0f / 30.0f;
constexpr float retail_standing_detection_raise_metres = 0.69f;
constexpr float retail_crouched_detection_raise_metres = 0.35f;

const NavigationGraph* actor_graph(const MissionScene& scene, std::size_t actor_index)
{
    const int identifier = scene.actors[actor_index].navigation_graph_identifier;
    const auto graph =
        std::find_if(scene.navigation_graphs.begin(),
                     scene.navigation_graphs.end(),
                     [identifier](const auto& candidate) { return candidate.identifier == identifier; });
    return graph == scene.navigation_graphs.end() ? nullptr : &*graph;
}

} // namespace

void GameSimulation::move_actor(std::size_t actor_index, Vector3 destination, float speed)
{
    auto& actor = actors_[actor_index];
    const auto* graph = actor_graph(scene_, actor_index);
    if (graph != nullptr)
    {
        if (actor.route_retry_time == 0 &&
            (actor.route.empty() || length(destination - actor.route_destination) > 2.0f))
        {
            actor.route = find_navigation_path(*graph, actor.position, destination);
            actor.route_index = 0;
            actor.route_destination = destination;
            actor.route_retry_time = 1.0f;
        }
        while (actor.route_index < actor.route.size() &&
               length(actor.route[actor.route_index] - actor.position) < 0.6f)
        {
            ++actor.route_index;
        }
        if (actor.route_index < actor.route.size())
        {
            destination = actor.route[actor.route_index];
        }
    }
    const Vector3 horizontal{destination.x - actor.position.x, destination.y - actor.position.y, 0};
    const float remaining = length(horizontal);
    if (remaining < 0.15f)
    {
        return;
    }
    actor.heading = std::atan2(-horizontal.x, horizontal.y);
    auto next_position = movement_world_.move_capsule(
        actor.position, normalized(horizontal) * std::min(speed * tick_duration, remaining), 1.8f);
    const auto floor = movement_world_.raycast(next_position + Vector3{0, 0, 1.5f}, {0, 0, -1}, 5.0f, true);
    if (floor.hit)
    {
        next_position.z = floor.position.z;
        bool occupied = false;
        for (std::size_t other_index = 0; other_index < actors_.size(); ++other_index)
        {
            if (other_index == actor_index || !actors_[other_index].active ||
                actors_[other_index].behavior == ActorBehavior::dead)
            {
                continue;
            }
            const auto difference = next_position - actors_[other_index].position;
            if (std::abs(difference.z) < 1.6f &&
                difference.x * difference.x + difference.y * difference.y < 0.36f)
            {
                occupied = true;
                break;
            }
        }
        if (!occupied && length(next_position - player_position()) > 0.65f)
        {
            actor.position = next_position;
        }
    }
}

void GameSimulation::advance_patrol(std::size_t actor_index)
{
    auto& actor = actors_[actor_index];
    const int identifier = scene_.actors[actor_index].patrol_path_identifier;
    const auto path =
        std::find_if(scene_.patrol_paths.begin(),
                     scene_.patrol_paths.end(),
                     [identifier](const auto& candidate) { return candidate.identifier == identifier; });
    if (path == scene_.patrol_paths.end() || path->commands.empty())
    {
        return;
    }
    if (actor.patrol_delay > 0)
    {
        actor.patrol_delay = std::max(0.0f, actor.patrol_delay - tick_duration);
        return;
    }
    if (actor.patrol_command_index >= path->commands.size())
    {
        const auto marker = std::find_if(path->commands.begin(),
                                         path->commands.end(),
                                         [](const auto& command) { return command.kind == 6; });
        if (marker == path->commands.end() || marker + 1 == path->commands.end())
        {
            return;
        }
        actor.patrol_command_index = static_cast<std::size_t>(marker - path->commands.begin()) + 1;
    }
    const auto& command = path->commands[actor.patrol_command_index];
    if (command.kind == 7 || command.kind == 8 || command.kind < 0 || command.kind > 8)
    {
        return;
    }
    if (command.kind == 1)
    {
        actor.patrol_delay = std::clamp(static_cast<float>(command.operand) * tick_duration, 0.0f, 3600.0f);
    }
    else if (command.kind == 2 || command.kind == 3 || command.kind == 5)
    {
        const auto* graph = actor_graph(scene_, actor_index);
        if (graph == nullptr)
        {
            return;
        }
        const auto node =
            std::find_if(graph->nodes.begin(),
                         graph->nodes.end(),
                         [&](const auto& candidate) { return candidate.identifier == command.operand; });
        if (node == graph->nodes.end())
        {
            return;
        }
        if (command.kind == 5)
        {
            const auto difference = node->position - actor.position;
            actor.heading = std::atan2(-difference.x, difference.y);
        }
        else
        {
            if (length(node->position - actor.position) > 0.7f)
            {
                move_actor(actor_index, node->position, command.kind == 3 ? 3.5f : 1.5f);
                return;
            }
            actor.route.clear();
            actor.route_retry_time = 0;
        }
    }
    else if (command.kind == 0)
    {
        actor.patrol_delay = 1.0f;
    }
    ++actor.patrol_command_index;
}

void GameSimulation::advance_actors()
{
    const auto player_feet = player_position();
    const float detection_height =
        crouching_ ? retail_crouched_detection_raise_metres : retail_standing_detection_raise_metres;
    const auto player_detection_mark = player_feet + Vector3{0, 0, detection_height};
    for (std::size_t actor_index = 0; actor_index < actors_.size(); ++actor_index)
    {
        auto& actor = actors_[actor_index];
        actor.muzzle_flash_remaining = std::max(0.0f, actor.muzzle_flash_remaining - tick_duration);
        if (!actor.active || actor.behavior == ActorBehavior::dead || scene_.actors[actor_index].team == 0 ||
            health_ <= 0)
        {
            continue;
        }
        if (actor.weapon_cooldown_ticks_remaining > 0)
        {
            --actor.weapon_cooldown_ticks_remaining;
        }
        bool reload_completed_this_tick = false;
        if (actor.reload_remaining > 0)
        {
            actor.reload_remaining = std::max(0.0f, actor.reload_remaining - tick_duration);
            if (actor.reload_remaining == 0)
            {
                const auto definition = std::find_if(
                    scene_.weapons.begin(),
                    scene_.weapons.end(),
                    [&](const auto& candidate)
                    { return candidate.identifier == scene_.actors[actor_index].weapon_identifier; });
                const auto magazine_capacity = definition == scene_.weapons.end()
                                                   ? scene_.weapon_parameters.magazine_capacity
                                                   : definition->parameters.magazine_capacity;
                actor.ammunition_in_clip = magazine_capacity;
                diagnostics::log(
                    diagnostics::Severity::information,
                    "ai",
                    "Soldier " + std::to_string(scene_.actors[actor_index].identifier) +
                        " completed weapon reload; magazine=" + std::to_string(actor.ammunition_in_clip));
                reload_completed_this_tick = true;
            }
        }
        actor.route_retry_time = std::max(0.0f, actor.route_retry_time - tick_duration);
        const Vector3 eye = actor.position + Vector3{0, 0, 1.5f};
        const Vector3 to_detection_mark = player_detection_mark - eye;
        const float detection_distance = length(to_detection_mark);
        const Vector3 direction = normalized(to_detection_mark);
        const Vector3 facing{-std::sin(actor.heading), std::cos(actor.heading), 0};
        const float sight_dot = dot(facing, direction);
        const float distance_multiplier = crouching_ ? 0.65f : 1.0f;
        const float max_range = (actor.behavior == ActorBehavior::guarding ? 75.0f : 80.0f) * distance_multiplier;
        const bool within_view = (actor.behavior == ActorBehavior::guarding)
                                     ? (sight_dot > 0.35f)
                                     : (sight_dot > -0.2f || detection_distance <= 6.0f);
        bool visible = false;
        if (within_view && detection_distance > 0.1f && detection_distance < max_range)
        {
            const auto obstruction = world_.raycast(eye, direction, detection_distance);
            visible = !obstruction.hit || obstruction.distance > detection_distance - 0.3f;
        }
        float hearing_radius = 45.0f;
        const auto* cur_weapon = weapon_definition();
        if (cur_weapon && (cur_weapon->identifier == "WEAPON_ID_MP5SD" ||
                           cur_weapon->parameters.name.find("SD") != std::string::npos))
        {
            hearing_radius = 3.0f;
        }
        else if (weapon_parameters().melee)
        {
            hearing_radius = 2.0f;
        }
        const bool heard_shot =
            weapon_.muzzle_flash_remaining > 0 && length(player_feet - eye) < hearing_radius;
        if (visible || heard_shot)
        {
            actor.last_seen_position = player_feet;
            actor.awareness_time = 5.0f;
            if (actor.behavior == ActorBehavior::guarding)
            {
                actor.route.clear();
                actor.has_tracked_aim_position = false;
                actor.first_fire = true;
                actor.burst_active = false;
                actor.rounds_remaining_in_burst = 0;
                diagnostics::log(diagnostics::Severity::debug,
                                 "ai",
                                 "Soldier " + std::to_string(scene_.actors[actor_index].identifier) +
                                     " acquired player stimulus");
            }
            actor.behavior = visible ? ActorBehavior::attacking : ActorBehavior::investigating;
        }
        else
        {
            actor.awareness_time = std::max(0.0f, actor.awareness_time - tick_duration);
            if (actor.awareness_time == 0)
            {
                if (actor.behavior != ActorBehavior::guarding)
                {
                    actor.route.clear();
                }
                actor.behavior = ActorBehavior::guarding;
            }
            else if (actor.behavior == ActorBehavior::attacking)
            {
                actor.behavior = ActorBehavior::investigating;
            }
        }
        if (actor.behavior == ActorBehavior::guarding)
        {
            advance_patrol(actor_index);
            continue;
        }
        if (!visible)
        {
            move_actor(actor_index, actor.last_seen_position, 1.8f);
        }
        else
        {
            actor.heading = std::atan2(-to_detection_mark.x, to_detection_mark.y);
            const Vector3 authored_aim_point = player_feet + Vector3{0, 0, crouching_ ? 0.25f : 0.5f};
            if (!actor.has_tracked_aim_position)
            {
                actor.tracked_aim_position = authored_aim_point;
                actor.has_tracked_aim_position = true;
            }
            else
            {
                const float aim_precision = std::clamp(scene_.actors[actor_index].aim_precision, 0.0f, 1.0f);
                actor.tracked_aim_position =
                    actor.tracked_aim_position +
                    (authored_aim_point - actor.tracked_aim_position) * aim_precision;
            }
        }
        if (!visible)
        {
            continue;
        }
        const auto& actor_definition = scene_.actors[actor_index];
        if (reload_completed_this_tick)
        {
            continue;
        }
        const auto definition =
            std::find_if(scene_.weapons.begin(),
                         scene_.weapons.end(),
                         [&](const auto& candidate)
                         { return candidate.identifier == actor_definition.weapon_identifier; });
        const auto& parameters =
            definition == scene_.weapons.end() ? scene_.weapon_parameters : definition->parameters;
        if (parameters.category != WeaponCategory::gun && parameters.category != WeaponCategory::pistol &&
            parameters.category != WeaponCategory::shotgun)
        {
            continue;
        }
        if (actor.reload_remaining > 0)
        {
            continue;
        }
        if (actor.ammunition_in_clip == 0)
        {
            actor.burst_active = false;
            actor.rounds_remaining_in_burst = 0;
            if (!std::isfinite(parameters.reload_seconds) || parameters.reload_seconds <= 0)
            {
                throw std::runtime_error("Original AI weapon reload time is invalid for " +
                                         actor_definition.weapon_identifier);
            }
            actor.reload_remaining = parameters.reload_seconds;
            diagnostics::log(diagnostics::Severity::information,
                             "ai",
                             "Soldier " + std::to_string(actor_definition.identifier) +
                                 " started weapon reload; duration=" +
                                 std::to_string(actor.reload_remaining) + " seconds");
            continue;
        }
        if (actor.burst_active && actor.rounds_remaining_in_burst == 0)
        {
            actor.burst_active = false;
            continue;
        }
        if (!actor.burst_active)
        {
            const auto magazine_capacity = static_cast<std::int32_t>(parameters.magazine_capacity);
            const auto weapon_burst_size = parameters.maximum_rounds_per_burst;
            if (magazine_capacity <= 0 || weapon_burst_size < 0 ||
                actor_definition.maximum_rounds_per_sequence <= 0)
            {
                throw std::runtime_error("Original AI weapon burst configuration is invalid for " +
                                         actor_definition.weapon_identifier);
            }
            const float burst_clip_fraction =
                actor_definition.minimum_clip_fraction +
                random_generator_.next_single() * actor_definition.random_clip_fraction;
            if (weapon_burst_size > 1 && weapon_burst_size != magazine_capacity)
            {
                actor.rounds_remaining_in_burst = weapon_burst_size;
            }
            else
            {
                // Retail sets x87 rounding to truncate and converts this product with FISTP.
                const auto rounds_from_clip_fraction = static_cast<std::int32_t>(
                    static_cast<double>(magazine_capacity) * static_cast<double>(burst_clip_fraction));
                actor.rounds_remaining_in_burst =
                    std::clamp(rounds_from_clip_fraction, 1, actor_definition.maximum_rounds_per_sequence);
            }
            actor.burst_active = true;
            actor.rounds_fired_in_burst = 0;
            const float primary_view_length = std::max(actor_definition.primary_view_length, 0.01f);
            const float distance_to_target = length(actor.tracked_aim_position - actor.position);
            const float clamped_target_distance = std::min(distance_to_target, primary_view_length);
            const float hit_probability_range = actor_definition.hit_probability_at_maximum_range -
                                                actor_definition.hit_probability_at_minimum_range;
            actor.hit_probability = actor_definition.hit_probability_at_minimum_range +
                                    hit_probability_range * (clamped_target_distance / primary_view_length);
            if (actor.first_fire)
            {
                actor.hit_probability *= 0.5f;
                actor.first_fire = false;
            }
            actor.hit_probability = std::clamp(actor.hit_probability, 0.0f, 1.0f);
            diagnostics::log(diagnostics::Severity::debug,
                             "ai",
                             "Soldier " + std::to_string(actor_definition.identifier) +
                                 " started burst; rounds=" + std::to_string(actor.rounds_remaining_in_burst) +
                                 "; hit probability=" + std::to_string(actor.hit_probability));
        }
        if (actor.weapon_cooldown_ticks_remaining > 0 || actor.shot_delay_ticks_remaining > 0)
        {
            if (actor.shot_delay_ticks_remaining > 0)
            {
                --actor.shot_delay_ticks_remaining;
            }
            continue;
        }
        if (!std::isfinite(parameters.shots_per_minute) || parameters.shots_per_minute < 1 ||
            parameters.shots_per_minute > static_cast<float>(std::numeric_limits<std::int32_t>::max()))
        {
            throw std::runtime_error("Original AI weapon firing rate is invalid for " +
                                     actor_definition.weapon_identifier);
        }
        const auto weapon_muzzle_position = actor.position + Vector3{0, 0, 1.5f};
        const auto target_aim_position = actor.has_tracked_aim_position
                                             ? actor.tracked_aim_position
                                             : player_feet + Vector3{0, 0, crouching_ ? 0.25f : 0.5f};
        const Vector3 projectile_forward = normalized(target_aim_position - weapon_muzzle_position);
        const Vector3 projectile_right{std::cos(actor.heading), std::sin(actor.heading), 0};
        const Vector3 projectile_up = cross(projectile_right, projectile_forward);
        const float projectile_spread =
            WeaponAccuracy::cone_angle(parameters, static_cast<std::uint32_t>(actor.rounds_fired_in_burst));
        const Vector3 projectile_direction = WeaponAccuracy::sample_direction(
            projectile_spread, projectile_forward, projectile_right, projectile_up, random_generator_);
        actor.muzzle_flash_remaining = 0.055f;
        ++actor.shots_fired;
        projectiles_.push_back({weapon_muzzle_position,
                                projectile_direction,
                                parameters.maximum_distance,
                                parameters.muzzle_velocity,
                                parameters.damage,
                                static_cast<int>(actor_index),
                                actor.hit_probability});
        --actor.ammunition_in_clip;
        --actor.rounds_remaining_in_burst;
        ++actor.rounds_fired_in_burst;
        actor.weapon_cooldown_ticks_remaining =
            WeaponAccuracy::cooldown_ticks(static_cast<std::int32_t>(parameters.shots_per_minute));
        if (parameters.maximum_rounds_per_burst == 1)
        {
            const float shot_delay =
                actor_definition.minimum_shot_delay_seconds +
                random_generator_.next_single() * actor_definition.random_shot_delay_seconds;
            actor.shot_delay_ticks_remaining = static_cast<std::int32_t>(shot_delay * 30.0f);
        }
        diagnostics::log(diagnostics::Severity::debug,
                         "ai",
                         "Soldier " + std::to_string(actor_definition.identifier) + " fired shot " +
                             std::to_string(actor.shots_fired) +
                             "; magazine=" + std::to_string(actor.ammunition_in_clip) +
                             "; rounds left in burst=" + std::to_string(actor.rounds_remaining_in_burst) +
                             "; hit probability=" + std::to_string(actor.hit_probability));
    }
}

void GameSimulation::advance_actor_animations()
{
    for (std::size_t actor_index = 0; actor_index < actors_.size(); ++actor_index)
    {
        auto& actor = actors_[actor_index];
        const auto& definition = scene_.actors[actor_index];
        if (!actor.active || !definition.animations)
        {
            continue;
        }
        if (actor.behavior == ActorBehavior::dead)
        {
            const auto found = definition.animations->clips.find(actor.animation_identifier);
            if (found != definition.animations->clips.end())
            {
                const auto& clip = found->second;
                if (clip.duration > 0)
                {
                    actor.animation_time =
                        std::min(actor.animation_time + 160.0f, static_cast<float>(clip.duration));
                }
            }
            continue;
        }
        const float displacement = length(actor.position - actor.animation_previous_position);
        actor.animation_previous_position = actor.position;
        int identifier = definition.stand_animation;
        if (actor.flinch_time > 0.0f)
        {
            actor.flinch_time = std::max(0.0f, actor.flinch_time - tick_duration);
            if (actor.flinch_animation > 0 && definition.animations->clips.contains(actor.flinch_animation))
            {
                identifier = actor.flinch_animation;
            }
        }
        else if (actor.behavior == ActorBehavior::attacking)
        {
            identifier = 56;
        }
        else if (displacement > 0.001f)
        {
            if (actor.behavior == ActorBehavior::guarding || actor.behavior == ActorBehavior::investigating)
            {
                if (definition.animations->clips.contains(4))
                {
                    identifier = 4; // Authentic calm soldier patrol / search walk (duration 5280)
                }
                else if (definition.animations->clips.contains(3))
                {
                    identifier = 3;
                }
                else if (definition.animations->clips.contains(34))
                {
                    identifier = 34;
                }
                else if (definition.animations->clips.contains(48))
                {
                    identifier = 48;
                }
            }
            else
            {
                if (definition.animations->clips.contains(3))
                {
                    identifier = 3; // Alerted sprint / run (duration 3360)
                }
                else if (definition.animations->clips.contains(4))
                {
                    identifier = 4;
                }
                else if (definition.animations->clips.contains(34))
                {
                    identifier = 34;
                }
                else if (definition.animations->clips.contains(48))
                {
                    identifier = 48;
                }
            }
        }
        const auto found = definition.animations->clips.find(identifier);
        if (found == definition.animations->clips.end())
        {
            continue;
        }
        if (actor.animation_identifier != identifier)
        {
            actor.animation_identifier = identifier;
            actor.animation_time = 0;
        }
        const auto& clip = found->second;
        float advance = 160.0f;
        if (displacement > 0.001f && clip.root_motion.keys.size() > 1)
        {
            const auto motion =
                clip.root_motion.keys.back().position - clip.root_motion.keys.front().position;
            const float distance = std::sqrt(motion.x * motion.x + motion.y * motion.y) / 4096.0f;
            if (distance > 0.001f)
            {
                advance = displacement / distance * clip.duration;
            }
        }
        actor.animation_time =
            clip.duration > 0 ? std::fmod(actor.animation_time + advance, clip.duration) : 0;
    }
}

} // namespace igi::gameplay
