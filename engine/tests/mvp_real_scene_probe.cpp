#include "igi/diagnostics.hpp"
#include "igi/game_simulation.hpp"
#include "igi/scene_renderer.hpp"

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_set>

using namespace igi::gameplay;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

int main(int argument_count, char** arguments)
{
    try
    {
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
        for (int tick_index = 0; tick_index < 120 && combat.weapon().kills == 0 && combat.health() > 0;
             ++tick_index)
        {
            const auto difference =
                combat.actors()[target_index].position + Vector3{0, 0, 1.2f} - combat.camera_position();
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
