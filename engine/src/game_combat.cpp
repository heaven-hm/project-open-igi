#include "igi/diagnostics.hpp"
#include "igi/game_simulation.hpp"
#include "igi/skeletal_animation.hpp"
#include "igi/vector_math.hpp"
#include "igi/weapon_accuracy.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace igi::gameplay
{
namespace
{

constexpr float tick_duration = 1.0f / 30.0f;
constexpr std::array<float, 17> soldier_hit_zone_multipliers{
    0.5f, 0.75f, 1.0f, 0.5f, 0.75f, 1.0f, 0.5f, 0.75f, 1.0f,
    0.5f, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 100.0f};

struct HumanMeshHit final
{
    float distance{};
    int bone_index{};
    int zone_index{};
};

int hit_zone_for_bone(std::uint32_t bone_index)
{
    constexpr std::array<int, 30> body_part_zones{
        12, 12, 12, 12, 16, 16, 16, 16, 11, 10, 9, 9, 9, 9, 5,
        4, 3, 3, 3, 3, 8, 7, 6, 6, 6, 2, 1, 0, 0, 0};
    if (bone_index >= 20 && bone_index < 23)
    {
        return 12;
    }
    const std::uint32_t body_part_index = bone_index >= 23 ? bone_index - 3 : bone_index;
    return body_part_index < body_part_zones.size() ? body_part_zones[body_part_index] : 12;
}

Vector3 actor_local_to_world(Vector3 position, float heading, Vector3 actor_position)
{
    const float cosine = std::cos(heading);
    const float sine = std::sin(heading);
    return {position.x * cosine - position.y * sine + actor_position.x,
            position.x * sine + position.y * cosine + actor_position.y,
            position.z + actor_position.z};
}

bool intersect_ray_triangle(Vector3 origin,
                            Vector3 direction,
                            Vector3 first,
                            Vector3 second,
                            Vector3 third,
                            float maximum_distance,
                            float& hit_distance)
{
    const Vector3 first_edge = second - first;
    const Vector3 second_edge = third - first;
    const Vector3 perpendicular = cross(direction, second_edge);
    const float determinant = dot(first_edge, perpendicular);
    if (std::abs(determinant) < 0.000001f)
    {
        return false;
    }
    const float inverse_determinant = 1.0f / determinant;
    const Vector3 offset = origin - first;
    const float horizontal = dot(offset, perpendicular) * inverse_determinant;
    if (horizontal < 0.0f || horizontal > 1.0f)
    {
        return false;
    }
    const Vector3 cross_offset = cross(offset, first_edge);
    const float vertical = dot(direction, cross_offset) * inverse_determinant;
    if (vertical < 0.0f || horizontal + vertical > 1.0f)
    {
        return false;
    }
    const float distance = dot(second_edge, cross_offset) * inverse_determinant;
    if (distance < 0.0f || distance >= maximum_distance)
    {
        return false;
    }
    hit_distance = distance;
    return true;
}

std::optional<HumanMeshHit> trace_human_mesh(Vector3 origin,
                                             Vector3 direction,
                                             float maximum_distance,
                                             const SceneActor& definition,
                                             const ActorState& state)
{
    if (!definition.mesh || !definition.animations || definition.mesh->skinned_collision.empty() ||
        !definition.animations->clips.contains(state.animation_identifier))
    {
        return std::nullopt;
    }
    const auto pose = sample_bone_pose(*definition.animations, state.animation_identifier, state.animation_time);
    float nearest_distance = maximum_distance;
    int nearest_bone = -1;
    for (const auto& triangle : definition.mesh->skinned_collision)
    {
        std::array<Vector3, 3> world_positions;
        for (std::size_t vertex_index = 0; vertex_index < triangle.vertices.size(); ++vertex_index)
        {
            const auto& vertex = triangle.vertices[vertex_index];
            const Vector3 local_position = skin_vertex(vertex, pose);
            world_positions[vertex_index] = actor_local_to_world(local_position, state.heading, state.position);
        }
        float distance = 0.0f;
        if (!intersect_ray_triangle(origin,
                                    direction,
                                    world_positions[0],
                                    world_positions[1],
                                    world_positions[2],
                                    nearest_distance,
                                    distance))
        {
            continue;
        }
        nearest_distance = distance;
        const auto first_bone = triangle.vertices[0].bone_index;
        const auto second_bone = triangle.vertices[1].bone_index;
        const auto third_bone = triangle.vertices[2].bone_index;
        nearest_bone = second_bone != first_bone && second_bone == third_bone
                           ? static_cast<int>(second_bone)
                           : static_cast<int>(first_bone);
    }
    if (nearest_bone < 0)
    {
        return std::nullopt;
    }

    const Vector3 incoming = direction * -1.0f;
    const Vector3 forward{-std::sin(state.heading), std::cos(state.heading), 0};
    const float local_forward = incoming.x * forward.x + incoming.y * forward.y;
    const float local_side = incoming.x * -forward.y + incoming.y * forward.x;
    int zone_index = hit_zone_for_bone(static_cast<std::uint32_t>(nearest_bone));
    if (zone_index == 12)
    {
        if (local_forward <= 0.0f && std::abs(local_forward) > std::abs(local_side))
        {
            zone_index = 14;
        }
        if (local_forward >= 0.0f && local_forward > std::abs(local_side))
        {
            zone_index = 13;
        }
    }
    if (local_forward >= 0.0f && pose.size() > 4)
    {
        const glm::vec3 head_local{pose[4][3]};
        const Vector3 head_position = actor_local_to_world(
            {head_local.x, head_local.y, head_local.z}, state.heading, state.position);
        const Vector3 ray_end = origin + direction * maximum_distance;
        const Vector3 segment = ray_end - origin;
        const float segment_squared_length = dot(segment, segment);
        if (segment_squared_length > 0.0f)
        {
            const float projection = std::clamp(dot(head_position - origin, segment) / segment_squared_length,
                                                0.0f,
                                                1.0f);
            if (length(head_position - (origin + segment * projection)) < 0.15f)
            {
                zone_index = 15;
            }
        }
    }
    return HumanMeshHit{nearest_distance, nearest_bone, zone_index};
}

float capsule_intersection(
    Vector3 origin, Vector3 direction, Vector3 feet, float height, float radius, float maximum_distance)
{
    float nearest = maximum_distance;
    const Vector3 relative = origin - feet;
    const float quadratic = direction.x * direction.x + direction.y * direction.y;
    const float mixed = relative.x * direction.x + relative.y * direction.y;
    const float constant = relative.x * relative.x + relative.y * relative.y - radius * radius;
    const float determinant = mixed * mixed - quadratic * constant;
    if (quadratic > 0.000001f && determinant >= 0)
    {
        const float distance = (-mixed - std::sqrt(determinant)) / quadratic;
        const float vertical = relative.z + direction.z * distance;
        if (distance >= 0 && distance < nearest && vertical >= radius && vertical <= height - radius)
        {
            nearest = distance;
        }
    }
    for (const float sphere_height : {radius, height - radius})
    {
        const Vector3 difference = origin - feet - Vector3{0, 0, sphere_height};
        const float projection = dot(difference, direction);
        const float sphere_determinant =
            projection * projection - dot(difference, difference) + radius * radius;
        if (sphere_determinant < 0)
        {
            continue;
        }
        const float distance = -projection - std::sqrt(sphere_determinant);
        if (distance >= 0 && distance < nearest)
        {
            nearest = distance;
        }
    }
    return nearest;
}

} // namespace

void GameSimulation::advance_weapon(const PlayerInput& input)
{
    const auto& parameters = weapon_parameters();
    const auto* definition = weapon_definition();
    for (const float scalar : {parameters.minimum_spread_degrees,
                               parameters.maximum_spread_degrees,
                               parameters.fixed_recoil_pitch_degrees,
                               parameters.fixed_recoil_yaw_degrees,
                               parameters.random_recoil_pitch_degrees,
                               parameters.random_recoil_yaw_degrees})
    {
        if (!std::isfinite(scalar))
        {
            throw std::runtime_error("Original weapon spread or recoil parameter is not finite");
        }
    }
    weapon_.sampled_recoil_pitch = 0;
    weapon_.sampled_recoil_yaw = 0;
    if (!std::isfinite(parameters.shots_per_minute) || !std::isfinite(parameters.muzzle_velocity) ||
        !std::isfinite(parameters.reload_seconds) || !std::isfinite(parameters.damage) ||
        !std::isfinite(parameters.maximum_distance) || parameters.maximum_distance <= 0 ||
        parameters.shots_per_minute < 1 || parameters.magazine_capacity == 0 ||
        parameters.muzzle_velocity <= 0 || parameters.reload_seconds <= 0 || parameters.damage <= 0)
    {
        throw std::runtime_error("Original weapon parameters are invalid");
    }
    if (!input.firing)
    {
        weapon_.rounds_in_current_burst = 0;
    }
    if (weapon_.cooldown_ticks_remaining > 0)
    {
        --weapon_.cooldown_ticks_remaining;
    }
    weapon_.muzzle_flash_remaining = std::max(0.0f, weapon_.muzzle_flash_remaining - tick_duration);
    weapon_.hit_marker_remaining = std::max(0.0f, weapon_.hit_marker_remaining - tick_duration);
    if (weapon_.reload_remaining > 0)
    {
        weapon_.reload_remaining = std::max(0.0f, weapon_.reload_remaining - tick_duration);
        if (weapon_.reload_remaining == 0)
        {
            const auto rounds = std::min(parameters.magazine_capacity - weapon_.magazine, weapon_.reserve);
            weapon_.magazine += rounds;
            weapon_.reserve -= rounds;
            diagnostics::log(diagnostics::Severity::information,
                             "weapon",
                             "Reload completed; magazine=" + std::to_string(weapon_.magazine) +
                                 ", reserve=" + std::to_string(weapon_.reserve));
        }
        return;
    }
    if (!parameters.melee && input.reloading && weapon_.magazine < parameters.magazine_capacity &&
        weapon_.reserve != 0)
    {
        weapon_.reload_remaining = parameters.reload_seconds;
        diagnostics::log(diagnostics::Severity::information, "weapon", "Reload started");
        return;
    }
    if (input.firing && (parameters.automatic || !fire_was_pressed_) && weapon_.magazine != 0 &&
        weapon_.cooldown_ticks_remaining == 0 &&
        weapon_.rounds_in_current_burst != static_cast<std::uint32_t>(parameters.maximum_rounds_per_burst))
    {
        if (parameters.melee)
        {
            int fire_clip = definition->fire_animation_1;
            if (weapon_.fire_variant == 1 && definition->fire_animation_2 >= 0)
            {
                fire_clip = definition->fire_animation_2;
            }
            else if (weapon_.fire_variant == 2 && definition->fire_animation_3 >= 0)
            {
                fire_clip = definition->fire_animation_3;
            }
            weapon_.animation_identifier = fire_clip;
            weapon_.animation_time = 0;
            weapon_.is_striking = true;
            weapon_.fire_variant = (weapon_.fire_variant + 1) % 3;
            if (scene_.first_person_animations && scene_.first_person_animations->clips.contains(fire_clip))
            {
                const float dur = scene_.first_person_animations->clips.at(fire_clip).duration;
                weapon_.cooldown_ticks_remaining = std::max(8, static_cast<int>(dur / 160.0f));
            }
            else
            {
                weapon_.cooldown_ticks_remaining = 14;
            }
        }
        else
        {
            --weapon_.magazine;
            weapon_.cooldown_ticks_remaining =
                WeaponAccuracy::cooldown_ticks(static_cast<std::int32_t>(parameters.shots_per_minute));
            weapon_.muzzle_flash_remaining = 0.055f;
        }
        if (parameters.maximum_rounds_per_burst <= 1)
        {
            // Retail chooses a sound variant here even when audio output is deferred.
            static_cast<void>(random_generator_.next_word());
        }
        const float cone = WeaponAccuracy::cone_angle(parameters, weapon_.rounds_in_current_burst);
        const Vector3 forward = aim_direction();
        const float current_view_heading = view_heading();
        const Vector3 right{std::cos(current_view_heading), std::sin(current_view_heading), 0};
        const Vector3 up = cross(right, forward);
        for (std::int32_t projectile_index = 0; projectile_index < parameters.projectiles_per_round;
             ++projectile_index)
        {
            projectiles_.push_back(
                {camera_position(),
                 WeaponAccuracy::sample_direction(cone, forward, right, up, random_generator_),
                 parameters.maximum_distance,
                 parameters.melee ? 1000.0f : parameters.muzzle_velocity,
                 parameters.damage,
                 -1});
        }
        ++weapon_.shots_fired;
        ++weapon_.rounds_in_current_burst;
        const auto recoil = WeaponAccuracy::sample_recoil(parameters, random_generator_);
        if (is_scoped())
        {
            weapon_.sampled_recoil_pitch = recoil.pitch * 0.05f;
            weapon_.sampled_recoil_yaw = recoil.yaw * 0.05f;
        }
        else
        {
            weapon_.sampled_recoil_pitch = recoil.pitch;
            weapon_.sampled_recoil_yaw = recoil.yaw;
        }
        if (developer_mode_)
        {
            weapon_.reserve = 999;
        }
        diagnostics::log(diagnostics::Severity::debug,
                         "weapon",
                         "Projectile fired; magazine=" + std::to_string(weapon_.magazine) +
                             ", burst round=" + std::to_string(weapon_.rounds_in_current_burst) +
                             ", cone=" + std::to_string(cone) +
                             ", projectiles=" + std::to_string(parameters.projectiles_per_round) +
                             ", recoil_pitch=" + std::to_string(weapon_.sampled_recoil_pitch) +
                             ", recoil_yaw=" + std::to_string(weapon_.sampled_recoil_yaw));
    }
    else if (!input.firing && !weapon_.is_striking)
    {
        weapon_.fire_variant = 0;
    }
}

void GameSimulation::advance_view_animation(const PlayerInput& input)
{
    const auto* definition = weapon_definition();
    if (definition == nullptr || !scene_.first_person_animations)
    {
        return;
    }
    const auto& clips = scene_.first_person_animations->clips;
    const bool firing = weapon_.muzzle_flash_remaining > 0.05f;
    int animation =
        input.forward != 0 || input.sideways != 0 ? definition->move_animation : definition->stand_animation;
    if (weapon_.reload_remaining > 0)
    {
        animation = definition->reload_animation;
    }
    else if (weapon_.is_striking)
    {
        animation = weapon_.animation_identifier;
        if (clips.contains(animation))
        {
            if (weapon_.animation_time + 160 < clips.at(animation).duration)
            {
                weapon_.animation_time += 160;
                return;
            }
            else
            {
                weapon_.is_striking = false;
                animation = input.forward != 0 || input.sideways != 0 ? definition->move_animation
                                                                      : definition->stand_animation;
            }
        }
    }
    else if (firing)
    {
        animation = definition->fire_animation;
    }
    else if ((weapon_.animation_identifier == definition->fire_animation ||
              weapon_.animation_identifier == definition->fire_animation_1 ||
              weapon_.animation_identifier == definition->fire_animation_2 ||
              weapon_.animation_identifier == definition->fire_animation_3) &&
             clips.contains(weapon_.animation_identifier) &&
             weapon_.animation_time + 160 < clips.at(weapon_.animation_identifier).duration)
    {
        animation = weapon_.animation_identifier;
    }
    if (!clips.contains(animation))
    {
        return;
    }
    if (weapon_.animation_identifier != animation || firing)
    {
        weapon_.animation_identifier = animation;
        weapon_.animation_time = 0;
    }
    else
    {
        weapon_.animation_time += 160;
    }
    if (clips.at(animation).duration > 0)
    {
        weapon_.animation_time = std::fmod(weapon_.animation_time, clips.at(animation).duration);
    }
}

void GameSimulation::advance_projectiles()
{
    for (auto& projectile : projectiles_)
    {
        const float travel = std::min(projectile.remaining_distance, projectile.speed * tick_duration);
        const auto world_hit = world_.raycast(projectile.position, projectile.direction, travel);
        float hit_distance = world_hit.hit ? world_hit.distance : travel;
        int target_actor = -1;
        int target_zone = 12;
        int target_bone = -1;
        bool target_player = false;
        if (projectile.owner == -1)
        {
            for (std::size_t actor_index = 0; actor_index < actors_.size(); ++actor_index)
            {
                if (!actors_[actor_index].active || actors_[actor_index].behavior == ActorBehavior::dead)
                {
                    continue;
                }
                const auto& actor_definition = scene_.actors[actor_index];
                const auto& actor_state = actors_[actor_index];
                const bool has_animated_mesh = actor_definition.mesh &&
                                               !actor_definition.mesh->skinned_collision.empty() &&
                                               actor_definition.animations &&
                                               actor_definition.animations->clips.contains(
                                                   actor_state.animation_identifier);
                const auto mesh_hit = has_animated_mesh
                                          ? trace_human_mesh(projectile.position,
                                                             projectile.direction,
                                                             hit_distance,
                                                             actor_definition,
                                                             actor_state)
                                          : std::optional<HumanMeshHit>{};
                const auto distance = has_animated_mesh
                                          ? (mesh_hit ? mesh_hit->distance : hit_distance)
                                          : capsule_intersection(projectile.position,
                                                                 projectile.direction,
                                                                 actor_state.position,
                                                                 1.8f,
                                                                 0.32f,
                                                                 hit_distance);
                if (distance < hit_distance)
                {
                    hit_distance = distance;
                    target_actor = static_cast<int>(actor_index);
                    if (mesh_hit.has_value())
                    {
                        target_zone = mesh_hit->zone_index;
                        target_bone = mesh_hit->bone_index;
                    }
                }
            }
        }
        else if (health_ > 0)
        {
            const auto distance = capsule_intersection(projectile.position,
                                                       projectile.direction,
                                                       player_position(),
                                                       crouching_ ? 1.1f : 1.8f,
                                                       0.3f,
                                                       hit_distance);
            if (distance < hit_distance)
            {
                target_player = true;
            }
        }
        if (target_player && projectile.hit_probability < random_generator_.next_single())
        {
            diagnostics::log(diagnostics::Severity::debug,
                             "combat",
                             "AI round missed the player hit-probability check; probability=" +
                                 std::to_string(projectile.hit_probability));
            target_player = false;
        }
        if (target_actor >= 0)
        {
            auto& actor = actors_[static_cast<std::size_t>(target_actor)];
            const float damage_multiplier =
                soldier_hit_zone_multipliers[static_cast<std::size_t>(target_zone)];
            const float applied_damage = projectile.damage * damage_multiplier;
            actor.health = std::max(0.0f, actor.health - applied_damage);
            actor.last_seen_position = player_position();
            actor.awareness_time = 8;
            actor.behavior = actor.health <= 0 ? ActorBehavior::dead : ActorBehavior::attacking;
            ++weapon_.hits;
            weapon_.hit_marker_remaining = 0.15f;
            diagnostics::log(diagnostics::Severity::debug,
                             "combat",
                             "Soldier " +
                                 std::to_string(scene_.actors[static_cast<std::size_t>(target_actor)].identifier) +
                                 " hit zone=" + std::to_string(target_zone) + ", bone=" +
                                 std::to_string(target_bone) + ", multiplier=" +
                                 std::to_string(damage_multiplier) + ", damage=" +
                                 std::to_string(applied_damage) + ", health=" + std::to_string(actor.health));
            if (actor.behavior == ActorBehavior::dead)
            {
                ++weapon_.kills;
                diagnostics::log(
                    diagnostics::Severity::information,
                    "combat",
                    "Soldier defeated: " +
                        std::to_string(scene_.actors[static_cast<std::size_t>(target_actor)].identifier));
            }
        }
        if (target_player)
        {
            if (!developer_mode_)
            {
                health_ = std::max(0.0f, health_ - projectile.damage);
                damage_flash_remaining_ = 0.2f;
            }
            diagnostics::log(
                diagnostics::Severity::information,
                "combat",
                "Player hit by soldier " +
                    std::to_string(scene_.actors.at(static_cast<std::size_t>(projectile.owner)).identifier) +
                    "; damage=" + std::to_string(projectile.damage) + "; health=" + std::to_string(health_));
            if (health_ <= 0)
            {
                diagnostics::log(
                    diagnostics::Severity::information, "combat", "Player died; press Enter to restart");
            }
        }
        if (world_hit.hit || target_actor >= 0 || target_player)
        {
            projectile.remaining_distance = 0;
        }
        else
        {
            projectile.position = projectile.position + projectile.direction * travel;
            projectile.remaining_distance -= travel;
        }
    }
    std::erase_if(projectiles_,
                  [](const Projectile& projectile) { return projectile.remaining_distance <= 0; });
}

} // namespace igi::gameplay
