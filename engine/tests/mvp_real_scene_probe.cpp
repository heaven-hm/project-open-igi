#include "igi/diagnostics.hpp"
#include "igi/game_simulation.hpp"
#include "igi/scene_renderer.hpp"
#include "igi/skeletal_animation.hpp"

#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>
#include <unordered_set>

using namespace igi::gameplay;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

bool ray_intersects_triangle(Vector3 origin,
                             Vector3 direction,
                             Vector3 first,
                             Vector3 second,
                             Vector3 third,
                             float maximum_distance)
{
    const auto cross_product = [](Vector3 first_vector, Vector3 second_vector)
    {
        return Vector3{first_vector.y * second_vector.z - first_vector.z * second_vector.y,
                       first_vector.z * second_vector.x - first_vector.x * second_vector.z,
                       first_vector.x * second_vector.y - first_vector.y * second_vector.x};
    };
    const auto dot_product = [](Vector3 first_vector, Vector3 second_vector)
    {
        return first_vector.x * second_vector.x + first_vector.y * second_vector.y +
               first_vector.z * second_vector.z;
    };
    const Vector3 first_edge = second - first;
    const Vector3 second_edge = third - first;
    const Vector3 perpendicular = cross_product(direction, second_edge);
    const float determinant = dot_product(first_edge, perpendicular);
    if (std::abs(determinant) < 0.000001f)
    {
        return false;
    }
    const float inverse_determinant = 1.0f / determinant;
    const Vector3 offset = origin - first;
    const float horizontal = dot_product(offset, perpendicular) * inverse_determinant;
    if (horizontal < 0.0f || horizontal > 1.0f)
    {
        return false;
    }
    const Vector3 cross_offset = cross_product(offset, first_edge);
    const float vertical = dot_product(direction, cross_offset) * inverse_determinant;
    if (vertical < 0.0f || horizontal + vertical > 1.0f)
    {
        return false;
    }
    const float distance = dot_product(second_edge, cross_offset) * inverse_determinant;
    return distance >= 0.0f && distance <= maximum_distance;
}

int main(int argument_count, char** arguments)
{
    try
    {
        igi::logging::Logger::instance().set_min_level(igi::logging::LogLevel::Debug);
        if (argument_count != 3 && argument_count != 4)
        {
            throw std::runtime_error(
                "Expected original game and cache directories, and optional replay output");
        }
        std::ofstream replay_output;
        if (argument_count == 4)
        {
            replay_output.open(arguments[3]);
            if (!replay_output)
            {
                throw std::runtime_error("Cannot create combat replay");
            }
            replay_output << std::setprecision(9);
        }
        const auto scene = load_mission(arguments[1], arguments[2]);
        require(scene.missing_object_count == 0 && scene.loaded_object_count > 300 &&
                    scene.actors.size() >= 20,
                "Real mission must load its original models and soldiers");
        require(scene.object_placement_audits.size() == scene.loaded_object_count,
                "Every loaded original object must retain an objects.qvm placement audit");
        require(scene.verified_object_count == scene.loaded_object_count && scene.mismatch_object_count == 0,
                "Every Level 1 object placement must verify against the emitted runtime scene");
        std::size_t soldier_collision_triangle_count = 0;
        std::size_t bone_mapped_collision_triangle_count = 0;
        for (const auto& actor : scene.actors)
        {
            if (!actor.gameplay_enabled || !actor.mesh)
            {
                continue;
            }
            soldier_collision_triangle_count += actor.mesh->skinned_collision.size();
            for (const auto& triangle : actor.mesh->skinned_collision)
            {
                const bool all_vertices_have_bones = std::all_of(
                    triangle.vertices.begin(),
                    triangle.vertices.end(),
                    [&](const SceneVertex& vertex)
                    { return vertex.bone_index < actor.mesh->bone_rest_positions.size(); });
                if (all_vertices_have_bones)
                {
                    ++bone_mapped_collision_triangle_count;
                }
            }
        }
        require(soldier_collision_triangle_count > 0 && bone_mapped_collision_triangle_count > 0,
                "Level 1 gameplay guards must load collision triangles that retain their rig bone indices");
        std::cout << "Level 1 soldier collision: triangles=" << soldier_collision_triangle_count
                  << ", bone-mapped=" << bone_mapped_collision_triangle_count << '\n';
        std::unordered_set<std::size_t> audited_source_calls;
        bool verified_rotated_geometry = false;
        for (const auto& audit : scene.object_placement_audits)
        {
            require(audit.position_matches && audit.orientation_matches,
                    "Loaded original object position or orientation differs from objects.qvm");
            require(finite(audit.authored_position) && finite(audit.runtime_position) &&
                        finite(audit.authored_rotation) && finite(audit.runtime_rotation),
                    "Loaded original object transform must remain finite");
            require(
                std::isfinite(audit.position_error) && std::isfinite(audit.orientation_error) &&
                    std::isfinite(audit.verification_tolerance) && audit.verification_tolerance > 0.0f &&
                    audit.verification_tolerance <= 0.001f &&
                    audit.position_error <= audit.verification_tolerance &&
                    audit.orientation_error <= audit.verification_tolerance,
                "Loaded original object transform error must remain within its float precision tolerance");
            require(audited_source_calls.insert(audit.source_call_index).second,
                    "Each loaded object must retain a unique objects.qvm source call");
            verified_rotated_geometry =
                verified_rotated_geometry ||
                (audit.emitted_geometry_verified && length(audit.authored_rotation) > 0.001f);
        }
        require(verified_rotated_geometry,
                "Level 1 placement verification must exercise a rotated emitted model transform");
        const auto route_graph =
            std::find_if(scene.navigation_graphs.begin(),
                         scene.navigation_graphs.end(),
                         [](const NavigationGraph& graph) { return graph.identifier == 4019; });
        require(route_graph != scene.navigation_graphs.end() && route_graph->route_table_width == 300 &&
                    route_graph->route_table.size() == 90000,
                "Level 1 graph4019 must retain its original 300-by-300 route table");
        bool disabled_cinematic_actor = false;
        for (const auto& actor : scene.actors)
        {
            if (actor.identifier == 3031)
            {
                disabled_cinematic_actor = !actor.gameplay_enabled;
            }
        }
        require(disabled_cinematic_actor, "Level 1's cinematic train actor must remain outside gameplay");
        for (const auto& weapon : scene.weapons)
        {
            if (weapon.identifier == "WEAPON_ID_MP5SD")
            {
                require(weapon.parameters.projectiles_per_round == 1 &&
                            weapon.parameters.minimum_spread_degrees == -2 &&
                            weapon.parameters.maximum_spread_degrees == 2 &&
                            weapon.parameters.fixed_recoil_pitch_degrees == 0.8f &&
                            weapon.parameters.random_recoil_pitch_degrees == 0.8f,
                        "MP5 count, spread and recoil must match the original decompiled weapon script");
            }
            else if (weapon.identifier == "WEAPON_ID_GLOCK")
            {
                require(weapon.parameters.projectiles_per_round == 1 &&
                            weapon.parameters.minimum_spread_degrees == -1.5f &&
                            weapon.parameters.maximum_spread_degrees == 1.5f &&
                            weapon.parameters.fixed_recoil_pitch_degrees == 1.5f &&
                            weapon.parameters.random_recoil_pitch_degrees == 0,
                        "Glock count, spread and recoil must match the original decompiled weapon script");
            }
            const float expected_range =
                weapon.identifier == "WEAPON_ID_AK47"                                              ? 2000.0f
                : weapon.identifier == "WEAPON_ID_MP5SD" || weapon.identifier == "WEAPON_ID_GLOCK" ? 80.0f
                : weapon.identifier == "WEAPON_ID_KNIFE"                                           ? 1.0f
                                                                                                   : -1.0f;
            const int expected_burst =
                weapon.identifier == "WEAPON_ID_AK47"                                              ? 30
                : weapon.identifier == "WEAPON_ID_MP5SD"                                           ? 32
                : weapon.identifier == "WEAPON_ID_GLOCK" || weapon.identifier == "WEAPON_ID_KNIFE" ? 1
                                                                                                   : -1;
            if (expected_burst > 0)
            {
                require(weapon.parameters.maximum_rounds_per_burst == expected_burst,
                        "Loaded burst limit must match original DefineWeaponType argument 17");
            }
            if (expected_range > 0)
            {
                require(weapon.parameters.maximum_distance == expected_range,
                        "Loaded weapon range must match its original DefineWeaponType argument");
            }
        }
        GameSimulation movement(scene);
        const auto start = movement.player_position();
        PlayerInput input;
        input.forward = 1;
        for (int tick_index = 0; tick_index < 10; ++tick_index)
        {
            movement.tick(input);
        }
        require(length(movement.player_position() - start) > 0.5f,
                "Player must move through the original level");
        input = {};
        input.jumping = true;
        movement.tick(input);
        require(!movement.grounded(), "Player must jump on original collision geometry");
        input.jumping = false;
        float highest = movement.player_position().z;
        for (int tick_index = 0; tick_index < 80; ++tick_index)
        {
            movement.tick(input);
            highest = std::max(highest, movement.player_position().z);
        }
        require(highest > start.z + 0.2f && movement.grounded(), "Original level jump must rise and land");

        {
            // Holding forward must never step backward, including at animation-loop starts, and a
            // jump taken at any phase must launch the way the player was already moving.
            for (int jump_tick = 8; jump_tick < 24; ++jump_tick)
            {
                GameSimulation forward(scene);
                PlayerInput held;
                held.forward = 1;
                for (int tick_index = 0; tick_index < 6; ++tick_index)
                {
                    forward.tick(held);
                }
                const auto origin = forward.player_position();
                Vector3 direction{};
                for (int tick_index = 6; tick_index < 80; ++tick_index)
                {
                    const auto before = forward.player_position();
                    held.jumping = tick_index == jump_tick;
                    forward.tick(held);
                    const auto after = forward.player_position();
                    if (tick_index == 6)
                    {
                        direction = Vector3{after.x - origin.x, after.y - origin.y, 0};
                        direction = direction * (1.0f / std::max(length(direction), 1e-6f));
                    }
                    const float progress =
                        (after.x - before.x) * direction.x + (after.y - before.y) * direction.y;
                    require(progress > -1e-4f,
                            "Holding forward (with or without a jump) must never step backward");
                }
            }
        }

        GameSimulation combat(scene);
        std::size_t target_index = scene.actors.size();
        float target_distance = std::numeric_limits<float>::max();
        Vector3 target_aim_position{};
        for (std::size_t actor_index = 0; actor_index < combat.actors().size(); ++actor_index)
        {
            if (!combat.actors()[actor_index].active || scene.actors[actor_index].team == 0)
            {
                continue;
            }
            const auto aim_point = combat.actors()[actor_index].position + Vector3{0, 0, 1.2f};
            const auto difference = aim_point - combat.camera_position();
            const float distance = length(difference);
            const auto obstruction =
                combat.world().raycast(combat.camera_position(), normalized(difference), distance);
            if ((!obstruction.hit || obstruction.distance > distance - 0.3f) && distance < target_distance &&
                distance < 350)
            {
                target_index = actor_index;
                target_distance = distance;
                target_aim_position = aim_point;
            }
        }
        if (target_index == scene.actors.size())
        {
            for (std::size_t actor_index = 0; actor_index < combat.actors().size() &&
                                                   target_index == scene.actors.size();
                 ++actor_index)
            {
                const auto& actor = combat.actors()[actor_index];
                const auto& definition = scene.actors[actor_index];
                if (!actor.active || definition.team == 0 || !definition.mesh ||
                    definition.mesh->skinned_collision.empty())
                {
                    continue;
                }
                const auto aim_point = actor.position + Vector3{0, 0, 1.2f};
                for (int candidate_index = 0; candidate_index < 16; ++candidate_index)
                {
                    const float candidate_heading =
                        static_cast<float>(candidate_index) * std::numbers::pi_v<float> / 8.0f;
                    const Vector3 candidate_position =
                        actor.position + Vector3{std::cos(candidate_heading) * 5.0f,
                                                std::sin(candidate_heading) * 5.0f,
                                                0};
                    combat.teleport_player(candidate_position, 0, 0);
                    const auto difference = aim_point - combat.camera_position();
                    const float distance = length(difference);
                    const auto obstruction =
                        combat.world().raycast(combat.camera_position(), normalized(difference), distance);
                    if ((!obstruction.hit || obstruction.distance > distance - 0.3f) && distance < 350)
                    {
                        target_index = actor_index;
                        target_distance = distance;
                        target_aim_position = aim_point;
                        std::cout << "Level 1 combat probe moved near guard " << definition.identifier << '\n';
                        break;
                    }
                }
            }
        }
        if (target_index == scene.actors.size())
        {
            input = {};
            input.firing = true;
            for (int tick_index = 0; tick_index < 100; ++tick_index)
            {
                combat.tick(input);
            }
            require(combat.weapon().shots_fired == 32 && combat.weapon().magazine == 0,
                    "Original level must simulate player fire even when active guards are out of sight");
            SceneRenderer renderer(640, 480);
            renderer.render(scene, combat);
            require(renderer.visible_triangles() > 100, "Original scene must render its textured geometry");
            std::cout << "Real-level probe passed: load, move, jump, land, fire, render; gameplay guards "
                         "outside spawn view\n";
            return 0;
        }
        std::cout << "Original soldier target: " << scene.actors[target_index].identifier
                  << "; distance=" << target_distance << '\n';
        GameSimulation ai_combat_simulation = combat;
        std::size_t visible_guard_index = scene.actors.size();
        for (std::size_t actor_index = 0;
             actor_index < scene.actors.size() && visible_guard_index == scene.actors.size();
             ++actor_index)
        {
            const auto& actor_definition = scene.actors[actor_index];
            const auto& actor_state = ai_combat_simulation.actors()[actor_index];
            if (!actor_state.active || actor_definition.team == 0 || !actor_definition.gameplay_enabled)
            {
                continue;
            }
            const Vector3 actor_eye_position = actor_state.position + Vector3{0, 0, 1.5f};
            const Vector3 actor_facing{-std::sin(actor_state.heading), std::cos(actor_state.heading), 0};
            for (int candidate_direction_index = 0; candidate_direction_index < 16; ++candidate_direction_index)
            {
                const float candidate_azimuth = static_cast<float>(candidate_direction_index) *
                                                std::numbers::pi_v<float> / 8.0f;
                const Vector3 candidate_position =
                    actor_state.position + Vector3{std::cos(candidate_azimuth) * 5.0f,
                                                   std::sin(candidate_azimuth) * 5.0f,
                                                   0};
                ai_combat_simulation.teleport_player(candidate_position, 0, 0);
                const Vector3 player_detection_position =
                    ai_combat_simulation.player_position() + Vector3{0, 0, 0.69f};
                const Vector3 direction_to_player = normalized(player_detection_position - actor_eye_position);
                if (dot(actor_facing, direction_to_player) <= 0.35f)
                {
                    continue;
                }
                const float player_detection_distance = length(player_detection_position - actor_eye_position);
                const auto line_of_sight_obstruction = ai_combat_simulation.world().raycast(
                    actor_eye_position, direction_to_player, player_detection_distance);
                if (!line_of_sight_obstruction.hit ||
                    line_of_sight_obstruction.distance > player_detection_distance - 0.3f)
                {
                    visible_guard_index = actor_index;
                    break;
                }
            }
        }
        require(visible_guard_index < scene.actors.size(),
                "Level 1 must provide a clear, in-cone gameplay guard for the AI firing check");
        require(
            scene.actors[visible_guard_index].maximum_health == 0.5f &&
                scene.actors[visible_guard_index].hit_probability_at_minimum_range == 1.0f &&
                scene.actors[visible_guard_index].hit_probability_at_maximum_range < 1.0f &&
                scene.actors[visible_guard_index].aim_precision > 0.0f &&
                scene.actors[visible_guard_index].maximum_rounds_per_sequence > 0 &&
                scene.actors[visible_guard_index].minimum_clip_fraction >= 0.0f &&
                scene.actors[visible_guard_index].minimum_clip_fraction <= 1.0f &&
                scene.actors[visible_guard_index].random_clip_fraction >= 0.0f &&
                scene.actors[visible_guard_index].random_clip_fraction <= 1.0f,
            "Level 1 guard health and combat values must load from the original difficulty-specific AI row");
        const auto guard_weapon =
            std::find_if(scene.weapons.begin(),
                         scene.weapons.end(),
                         [&](const auto& weapon) {
                             return weapon.identifier == scene.actors[visible_guard_index].weapon_identifier;
                         });
        require(guard_weapon != scene.weapons.end() && guard_weapon->parameters.magazine_capacity > 0 &&
                    guard_weapon->parameters.shots_per_minute > 0.0f &&
                    guard_weapon->parameters.reload_seconds > 0.0f &&
                    guard_weapon->parameters.maximum_rounds_per_burst > 0,
                "Level 1 guard weapon cadence, magazine, reload and burst fields must load from weapon.qvm");
        for (int tick_index = 0; tick_index < 90; ++tick_index)
        {
            ai_combat_simulation.tick({});
        }
        require(ai_combat_simulation.actors()[visible_guard_index].shots_fired > 0,
                "The Level 1 guard must fire through the real mission AI configuration");
        const auto ai_combat_log_entries = igi::logging::Logger::instance().snapshot_recent_entries(4096);
        const std::string target_shot_message =
            "Soldier " + std::to_string(scene.actors[visible_guard_index].identifier) + " fired shot";
        const bool verified_timestamped_ai_shot =
            std::any_of(ai_combat_log_entries.begin(),
                        ai_combat_log_entries.end(),
                        [&](const igi::logging::LogEntry& entry) {
                            return entry.subsystem == "ai" &&
                                   entry.message.find(target_shot_message) != std::string::npos;
                        });
        require(verified_timestamped_ai_shot,
                "A real Level 1 AI projectile must be present in the timestamped gameplay log");
        const std::string target_burst_message =
            "Soldier " + std::to_string(scene.actors[visible_guard_index].identifier) + " started burst";
        const bool verified_timestamped_ai_burst =
            std::any_of(ai_combat_log_entries.begin(),
                        ai_combat_log_entries.end(),
                        [&](const igi::logging::LogEntry& entry) {
                            return entry.subsystem == "ai" &&
                                   entry.message.find(target_burst_message) != std::string::npos;
                        });
        require(verified_timestamped_ai_burst,
                "A real Level 1 AI burst must be visible in the timestamped gameplay log");
        const auto& target_definition = scene.actors[target_index];
        const auto& target_state = combat.actors()[target_index];
        const auto target_pose = sample_bone_pose(
            *target_definition.animations, target_state.animation_identifier, target_state.animation_time);
        bool target_point_intersects_mesh = false;
        for (int height_step = 1; height_step <= 50 && !target_point_intersects_mesh; ++height_step)
        {
            const float relative_height = static_cast<float>(height_step) * 0.05f;
            const Vector3 candidate_aim_position = target_state.position + Vector3{0, 0, relative_height};
            const Vector3 candidate_direction =
                normalized(candidate_aim_position - combat.camera_position());
            std::size_t candidate_intersections = 0;
            for (const auto& triangle : target_definition.mesh->skinned_collision)
            {
                std::array<Vector3, 3> world_positions;
                for (std::size_t vertex_index = 0; vertex_index < triangle.vertices.size(); ++vertex_index)
                {
                    const auto local = skin_vertex(triangle.vertices[vertex_index], target_pose);
                    const float cosine = std::cos(target_state.heading);
                    const float sine = std::sin(target_state.heading);
                    world_positions[vertex_index] = {local.x * cosine - local.y * sine + target_state.position.x,
                                                     local.x * sine + local.y * cosine + target_state.position.y,
                                                     local.z + target_state.position.z};
                }
                if (ray_intersects_triangle(combat.camera_position(),
                                            candidate_direction,
                                            world_positions[0],
                                            world_positions[1],
                                            world_positions[2],
                                            8.0f))
                {
                    ++candidate_intersections;
                }
            }
            if (candidate_intersections > 0)
            {
                target_aim_position = candidate_aim_position;
                target_point_intersects_mesh = true;
                std::cout << "Collision-mesh aim height=" << relative_height
                          << " m; intersected faces=" << candidate_intersections << '\n';
            }
        }
        require(target_point_intersects_mesh,
                "The head-to-feet scan must find a visible projectile ray through the guard collision mesh");
        Vector3 collision_minimum{std::numeric_limits<float>::max(),
                                  std::numeric_limits<float>::max(),
                                  std::numeric_limits<float>::max()};
        Vector3 collision_maximum{-std::numeric_limits<float>::max(),
                                  -std::numeric_limits<float>::max(),
                                  -std::numeric_limits<float>::max()};
        for (const auto& triangle : target_definition.mesh->skinned_collision)
        {
            for (const auto& vertex : triangle.vertices)
            {
                const auto local = skin_vertex(vertex, target_pose);
                const float cosine = std::cos(target_state.heading);
                const float sine = std::sin(target_state.heading);
                const Vector3 world_position{local.x * cosine - local.y * sine + target_state.position.x,
                                             local.x * sine + local.y * cosine + target_state.position.y,
                                             local.z + target_state.position.z};
                collision_minimum.x = std::min(collision_minimum.x, world_position.x);
                collision_minimum.y = std::min(collision_minimum.y, world_position.y);
                collision_minimum.z = std::min(collision_minimum.z, world_position.z);
                collision_maximum.x = std::max(collision_maximum.x, world_position.x);
                collision_maximum.y = std::max(collision_maximum.y, world_position.y);
                collision_maximum.z = std::max(collision_maximum.z, world_position.z);
            }
        }
        std::cout << "Target skinned collision bounds: min=(" << collision_minimum.x << ',' << collision_minimum.y
                  << ',' << collision_minimum.z << ") max=(" << collision_maximum.x << ','
                  << collision_maximum.y << ',' << collision_maximum.z << ") target=("
                  << target_state.position.x << ',' << target_state.position.y << ',' << target_state.position.z
                  << ")\n";
        const auto centerline_direction = normalized(target_aim_position - combat.camera_position());
        std::size_t centerline_collision_count = 0;
        for (const auto& triangle : target_definition.mesh->skinned_collision)
        {
            std::array<Vector3, 3> world_positions;
            for (std::size_t vertex_index = 0; vertex_index < triangle.vertices.size(); ++vertex_index)
            {
                const auto local = skin_vertex(triangle.vertices[vertex_index], target_pose);
                const float cosine = std::cos(target_state.heading);
                const float sine = std::sin(target_state.heading);
                world_positions[vertex_index] = {local.x * cosine - local.y * sine + target_state.position.x,
                                                 local.x * sine + local.y * cosine + target_state.position.y,
                                                 local.z + target_state.position.z};
            }
            if (ray_intersects_triangle(combat.camera_position(),
                                        centerline_direction,
                                        world_positions[0],
                                        world_positions[1],
                                        world_positions[2],
                                        8.0f))
            {
                ++centerline_collision_count;
            }
        }
        std::cout << "Target centerline skinned-face hits=" << centerline_collision_count << '\n';
        for (int tick_index = 0; tick_index < 120 && combat.weapon().kills == 0 && combat.health() > 0;
             ++tick_index)
        {
            const auto difference = target_aim_position - combat.camera_position();
            input = {};
            input.heading_change = std::atan2(-difference.x, difference.y) - combat.heading();
            input.pitch_change =
                std::atan2(difference.z,
                           std::sqrt(difference.x * difference.x + difference.y * difference.y)) -
                combat.pitch();
            input.firing = true;
            if (replay_output.is_open())
            {
                replay_output << "0,0," << input.heading_change << ',' << input.pitch_change
                              << ",0,0,0,1,0,0\n";
            }
            combat.tick(input);
        }
        require(combat.weapon().hits > 0 && combat.weapon().kills == 1,
                "Player projectiles must defeat an original scene soldier");
        const auto combat_log_entries = igi::logging::Logger::instance().snapshot_recent_entries(4096);
        const bool verified_mesh_hit = std::any_of(
            combat_log_entries.begin(),
            combat_log_entries.end(),
            [](const igi::logging::LogEntry& entry)
            {
                return entry.subsystem == "combat" &&
                       entry.message.find("hit zone=5, bone=14") != std::string::npos;
            });
        require(verified_mesh_hit,
                "A Level 1 player hit must report the struck collision-mesh bone and retail damage zone");
        if (replay_output.is_open())
        {
            replay_output.flush();
            require(static_cast<bool>(replay_output), "Combat replay output failed");
        }
        SceneRenderer renderer(640, 480);
        renderer.render(scene, combat);
        require(renderer.visible_triangles() > 100, "Original scene must produce visible textured geometry");
        std::cout
            << "Real-level probe passed: load, move, jump, land, aim, fire, damage, defeat, render; shots="
            << combat.weapon().shots_fired << ", hits=" << combat.weapon().hits << '\n';
        return 0;
    }
    catch (const std::exception& exception)
    {
        igi::diagnostics::log(igi::diagnostics::Severity::error, "real-level-probe", exception.what());
        return 1;
    }
}
