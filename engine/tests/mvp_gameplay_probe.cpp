#include "igi/game_simulation.hpp"
#include "igi/vector_math.hpp"

#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string_view>

using namespace igi::gameplay;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

MissionScene make_scene(bool enemy, bool occluded)
{
    MissionScene scene;
    scene.collision_triangles = {{{Vector3{-100, -100, -2}, Vector3{100, -100, -2}, Vector3{100, 100, -2}}},
                                 {{Vector3{-100, -100, -2}, Vector3{100, 100, -2}, Vector3{-100, 100, -2}}}};
    if (occluded)
    {
        scene.collision_triangles.push_back({{Vector3{-3, 3, -2}, Vector3{3, 3, -2}, Vector3{3, 3, 3}}});
        scene.collision_triangles.push_back({{Vector3{-3, 3, -2}, Vector3{3, 3, 3}, Vector3{-3, 3, 3}}});
    }
    if (enemy)
    {
        scene.actors.push_back({{0, 6, -2}, std::numbers::pi_v<float>, 21, 1, {}});
    }
    const Vector3 derivative{0, 12288.0f / 4800.0f, 0};
    scene.root_motion_clips[4] = {
        4800, {{0, {}, derivative, derivative}, {4800, {0, 12288, 0}, derivative, derivative}}};
    return scene;
}

int main()
{
    try
    {
        const Vector3 retail_orientation = rotate_euler({1.25f, -2.5f, 4.0f}, {0.31f, -0.47f, 1.12f});
        require(std::abs(retail_orientation.x - 0.29084343f) < 0.00001f &&
                    std::abs(retail_orientation.y + 0.78521296f) < 0.00001f &&
                    std::abs(retail_orientation.z - 4.80742662f) < 0.00001f,
                "Three-axis placement rotation must match the retail Z-Y-X row-matrix composition");
        const Vector3 small_retail_rotation = rotate_euler({1.0f, 0.0f, 0.0f}, {0.0f, 0.00005f, 0.0f});
        require(std::abs(small_retail_rotation.z - 0.00005f) < 0.000001f,
                "Small authored angles must not be discarded by a placement rotation shortcut");
        for (const float invalid_range : {0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN()})
        {
            auto invalid_scene = make_scene(false, false);
            invalid_scene.weapon_parameters.maximum_distance = invalid_range;
            bool rejected = false;
            try
            {
                GameSimulation invalid_simulation(invalid_scene);
                invalid_simulation.tick({});
            }
            catch (const std::runtime_error&)
            {
                rejected = true;
            }
            require(rejected, "Invalid weapon range must raise a controlled runtime error");
        }
        auto short_range_scene = make_scene(true, false);
        short_range_scene.weapon_parameters.maximum_distance = 1.0f;
        GameSimulation short_range_enemy(short_range_scene);
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            short_range_enemy.tick({});
        }
        require(short_range_enemy.actors().front().shots_fired > 0,
                "Range regression must exercise actual enemy shots");
        require(short_range_enemy.health() == short_range_scene.player_parameters.maximum_health,
                "Enemy rounds must expire at their configured range before reaching the player");
        auto burst_scene = make_scene(false, false);
        burst_scene.weapon_parameters.maximum_rounds_per_burst = 3;
        GameSimulation burst_fire(burst_scene);
        PlayerInput held_trigger;
        held_trigger.firing = true;
        for (int tick_index = 0; tick_index < 60; ++tick_index)
        {
            burst_fire.tick(held_trigger);
        }
        require(burst_fire.weapon().shots_fired == 3,
                "Holding the trigger must stop at the original configured burst limit");
        burst_fire.tick({});
        require(burst_fire.weapon().rounds_in_current_burst == 0,
                "Releasing the trigger must reset the current burst count");
        for (int tick_index = 0; tick_index < 60; ++tick_index)
        {
            burst_fire.tick(held_trigger);
        }
        require(burst_fire.weapon().shots_fired == 6,
                "A fresh trigger pull must allow another complete burst");
        const auto movement_scene = make_scene(false, false);
        require(movement_scene.player_parameters.maximum_health == 1.0f,
                "Retail human health must use a one-point damage accumulator");
        const auto ordinary_jump_impact = HumanFallDamage_Calculate(
            5.0F * 4096.0F / 30.0F,
            movement_scene.player_parameters.maximum_health);
        require(ordinary_jump_impact.damage == 0.0F && ordinary_jump_impact.sound_definition_name.empty(),
                "Retail landing logic must not damage or play a fall-damage cue after an ordinary short jump");
        const auto light_fall_impact = HumanFallDamage_Calculate(14.5F * 4096.0F / 30.0F, 100.0F);
        const auto medium_fall_impact = HumanFallDamage_Calculate(15.5F * 4096.0F / 30.0F, 100.0F);
        const auto heavy_fall_impact = HumanFallDamage_Calculate(17.0F * 4096.0F / 30.0F, 100.0F);
        require(std::abs(light_fall_impact.damage - 100.0F / 26.0F) < 0.0001F &&
                    light_fall_impact.sound_definition_name == "player_fall_1" &&
                    medium_fall_impact.sound_definition_name == "player_fall_2" &&
                    heavy_fall_impact.sound_definition_name == "player_fall_3",
                "Retail fall damage must select the three sound definitions at the original health fractions");
        auto sound_landing_scene = movement_scene;
        sound_landing_scene.player_parameters.jump_vertical_per_tick = 0.8F;
        GameSimulation sound_landing(sound_landing_scene);
        PlayerInput high_jump_input;
        high_jump_input.jumping = true;
        std::string_view observed_fall_sound;
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            sound_landing.tick(high_jump_input);
            high_jump_input.jumping = false;
            const std::string_view sound = sound_landing.take_player_fall_sound();
            if (!sound.empty())
            {
                observed_fall_sound = sound;
            }
        }
        require(observed_fall_sound == "player_fall_3" && sound_landing.take_player_fall_sound().empty(),
                "A damaging landing must emit its retail fall cue once and consume it exactly once");
        GameSimulation jump_timing(movement_scene);
        const float jump_floor = jump_timing.player_position().z;
        PlayerInput jump_input;
        jump_input.jumping = true;
        jump_timing.tick(jump_input);
        require(jump_timing.take_player_jump_sound() == "jump_1",
                "Retail jump audio must fire on the take-off edge");
        const float launch_distance = movement_scene.player_parameters.jump_vertical_per_tick;
        require(std::abs(jump_timing.player_position().z - jump_floor - launch_distance) < 0.000001f,
                "Retail jump launch must apply its full velocity without gravity or impulse damping");
        jump_timing.tick({});
        require(jump_timing.take_player_jump_sound().empty(),
                "A jump sound must not repeat after the take-off tick");
        const float second_jump_distance = launch_distance - HumanMotion::gravity_per_tick / 4096.0f;
        require(std::abs(jump_timing.player_position().z - jump_floor - launch_distance -
                         second_jump_distance) < 0.000001f,
                "Retail airborne gravity starts after launch and does not damp movement velocity");

        GameSimulation running_jump(movement_scene);
        PlayerInput running_input;
        running_input.forward = 1;
        running_jump.tick(running_input);
        const auto jump_start = running_jump.player_position();
        running_jump.tick(jump_input);
        const float launch_forward = running_jump.player_position().y - jump_start.y;
        require(std::abs(launch_forward - movement_scene.player_parameters.jump_horizontal_per_tick) <
                    0.000001f,
                "Jump direction must come from the previous animation step after movement is released");
        const float launch_position = running_jump.player_position().y;
        running_jump.tick({});
        require(std::abs(running_jump.player_position().y - launch_position - launch_forward) < 0.000001f,
                "Airborne horizontal momentum must survive without the separate hit-impulse damping");
        for (int tick_index = 0; tick_index < 60; ++tick_index)
        {
            running_jump.tick({});
        }
        require(running_jump.grounded(), "Corrected running jump must land on the original floor");
        require(running_jump.health() == movement_scene.player_parameters.maximum_health,
                "A normal player jump and landing must leave retail health unchanged");
        const auto landing_position = running_jump.player_position();
        for (int tick_index = 0; tick_index < 10; ++tick_index)
        {
            running_jump.tick({});
        }
        require(length(running_jump.player_position() - landing_position) == 0,
                "Landing must clear motion and impact velocities instead of sliding on the ground");
        GameSimulation movement(movement_scene);
        PlayerInput input;
        input.forward = 1;
        for (int tick_index = 0; tick_index < 6; ++tick_index)
        {
            movement.tick(input);
        }
        require(movement.player_position().y > 1.04f && movement.player_position().y < 1.06f,
                "Walking must use animation-track displacement and translation scale");
        GameSimulation looping(movement_scene);
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            looping.tick(input);
        }
        require(std::abs(looping.player_position().y - 21.0f) < 0.01f,
                "Looping root motion must preserve travel across animation seams");
        input = {};
        input.jumping = true;
        movement.tick(input);
        require(!movement.grounded(), "Jump input must leave the ground");
        const float ground_height = -2;
        float maximum_height = movement.player_position().z;
        input.jumping = false;
        for (int tick_index = 0; tick_index < 90; ++tick_index)
        {
            movement.tick(input);
            maximum_height = std::max(maximum_height, movement.player_position().z);
        }
        require(maximum_height > ground_height + 0.3f && movement.grounded(),
                "Jump must rise and return to the real collision floor");
        input.crouching = true;
        movement.tick(input);
        require(movement.camera_position().z < ground_height + 1.0f, "Crouching must lower the camera");

        auto separate_meshes = make_scene(false, true);
        separate_meshes.movement_collision_triangles.assign(separate_meshes.collision_triangles.begin(),
                                                            separate_meshes.collision_triangles.begin() + 2);
        GameSimulation walking_mesh(separate_meshes);
        PlayerInput forward_input;
        forward_input.forward = 1;
        for (int tick_index = 0; tick_index < 30; ++tick_index)
        {
            walking_mesh.tick(forward_input);
        }
        require(walking_mesh.player_position().y > 4 && walking_mesh.grounded(),
                "Human movement must use mesh level one rather than the separate default trace hull");
        require(walking_mesh.world().raycast({0, 0, -1}, {0, 1, 0}, 6).hit &&
                    !walking_mesh.movement_world().raycast({0, 0, -1}, {0, 1, 0}, 6).hit,
                "Movement and projectile/sight geometry must remain independently queryable");

        const auto combat_scene = make_scene(true, false);
        GameSimulation combat(combat_scene);
        input = {};
        input.firing = true;
        for (int tick_index = 0; tick_index < 100; ++tick_index)
        {
            combat.tick(input);
        }
        require(combat.weapon().shots_fired == 32 && combat.weapon().magazine == 0,
                "Automatic fire must respect the magazine capacity");
        require(combat.weapon().kills == 1 && combat.weapon().hits >= 6 &&
                    combat.actors().front().behavior == ActorBehavior::dead,
                "Projectiles must accumulate original body-zone damage and defeat the target");
        for (int tick_index = 0; tick_index < 180; ++tick_index)
        {
            combat.tick({});
        }
        require(combat.actors().front().behavior == ActorBehavior::dead,
                "A defeated enemy must remain down after its one-shot death animation completes");
        require(combat.actors().front().animation_identifier == 22 ||
                    combat.actors().front().animation_identifier == 23,
                "An ordinary MP5 kill must select a retail crumple clip, not a shotgun throw or recovery clip");
        auto distant_scene = make_scene(true, false);
        distant_scene.actors.front().position.y = 12;
        GameSimulation distant_combat(distant_scene);
        const auto guard_start = distant_combat.actors().front().position;
        for (int tick_index = 0; tick_index < 75; ++tick_index)
        {
            distant_combat.tick({});
        }
        require(distant_combat.actors().front().shots_fired > 0,
                "A visible armed guard must produce observable firing events");
        require(distant_combat.health() < distant_scene.player_parameters.maximum_health,
                "An unopposed visible guard must damage the player before it is defeated");
        require(length(distant_combat.actors().front().position - guard_start) < 0.01f,
                "A guard with a clear firing line must hold its firing position rather than chase-run");
        auto burst_cadence_scene = make_scene(true, false);
        burst_cadence_scene.actors.front().position.y = 12;
        burst_cadence_scene.actors.front().maximum_rounds_per_sequence = 3;
        burst_cadence_scene.actors.front().minimum_clip_fraction = 1.0f;
        burst_cadence_scene.actors.front().random_clip_fraction = 0.0f;
        burst_cadence_scene.weapon_parameters.shots_per_minute = 700.0f;
        burst_cadence_scene.weapon_parameters.maximum_rounds_per_burst = 32;
        GameSimulation burst_cadence_simulation(burst_cadence_scene);
        burst_cadence_simulation.tick({});
        require(burst_cadence_simulation.actors().front().shots_fired == 1,
                "A newly visible guard must fire the first round immediately after its aim state begins");
        burst_cadence_simulation.tick({});
        require(burst_cadence_simulation.actors().front().shots_fired == 1,
                "AI fire must wait for the retail weapon cooldown between rounds");
        burst_cadence_simulation.tick({});
        require(burst_cadence_simulation.actors().front().shots_fired == 2,
                "The second AI round must follow the configured 700-round-per-minute tick cadence");
        burst_cadence_simulation.tick({});
        burst_cadence_simulation.tick({});
        require(burst_cadence_simulation.actors().front().shots_fired == 3 &&
                    burst_cadence_simulation.actors().front().rounds_remaining_in_burst == 0,
                "The AI settings row must cap a magazine burst at its configured three rounds");
        burst_cadence_simulation.tick({});
        require(burst_cadence_simulation.actors().front().shots_fired == 3,
                "The guard must return through its stand phase after finishing a burst");

        auto retail_rounding_scene = make_scene(true, false);
        retail_rounding_scene.actors.front().position.y = 12;
        retail_rounding_scene.actors.front().minimum_clip_fraction = std::nextafter(0.125f, 0.0f);
        retail_rounding_scene.actors.front().random_clip_fraction = 0.0f;
        GameSimulation retail_rounding_simulation(retail_rounding_scene);
        retail_rounding_simulation.tick({});
        require(retail_rounding_simulation.actors().front().shots_fired == 1 &&
                    retail_rounding_simulation.actors().front().rounds_remaining_in_burst == 2,
                "Retail truncate-before-rounding must keep a clip fraction just below four rounds at three");

        auto reload_scene = make_scene(true, false);
        reload_scene.actors.front().position.y = 12;
        reload_scene.actors.front().maximum_rounds_per_sequence = 2;
        reload_scene.actors.front().minimum_clip_fraction = 1.0f;
        reload_scene.actors.front().random_clip_fraction = 0.0f;
        reload_scene.weapon_parameters.magazine_capacity = 2;
        reload_scene.weapon_parameters.maximum_rounds_per_burst = 2;
        reload_scene.weapon_parameters.reload_seconds = 2.0f;
        GameSimulation reload_simulation(reload_scene);
        for (int tick_index = 0; tick_index < 7; ++tick_index)
        {
            reload_simulation.tick({});
        }
        require(reload_simulation.actors().front().ammunition_in_clip == 0 &&
                    reload_simulation.actors().front().reload_remaining > 0.0f &&
                    reload_simulation.actors().front().reload_remaining <= 2.0f,
                "An exhausted AI weapon must enter the weapon definition's two-second reload");
        const auto shots_before_reload_completion = reload_simulation.actors().front().shots_fired;
        for (int tick_index = 0;
             tick_index < 60 && reload_simulation.actors().front().reload_remaining > 0.0f;
             ++tick_index)
        {
            reload_simulation.tick({});
        }
        require(reload_simulation.actors().front().ammunition_in_clip == 2 &&
                    reload_simulation.actors().front().reload_remaining == 0.0f &&
                    reload_simulation.actors().front().shots_fired == shots_before_reload_completion,
                "An AI reload must restore the configured magazine after sixty 30 Hz ticks");
        auto zero_accuracy_scene = make_scene(true, false);
        zero_accuracy_scene.actors.front().position.y = 12;
        zero_accuracy_scene.actors.front().hit_probability_at_minimum_range = 0.0f;
        zero_accuracy_scene.actors.front().hit_probability_at_maximum_range = 0.0f;
        GameSimulation zero_accuracy_combat(zero_accuracy_scene);
        for (int tick_index = 0; tick_index < 75; ++tick_index)
        {
            zero_accuracy_combat.tick({});
        }
        require(zero_accuracy_combat.actors().front().shots_fired > 0,
                "Zero hit probability must still allow visible guards to fire rounds");
        require(zero_accuracy_combat.health() == zero_accuracy_scene.player_parameters.maximum_health,
                "An AI round that fails the configured hit-probability check must not damage the player");
        auto standard_maximum_health_scene = make_scene(true, false);
        standard_maximum_health_scene.actors.front().position.y = 12;
        auto reduced_maximum_health_scene = standard_maximum_health_scene;
        reduced_maximum_health_scene.actors.front().maximum_health = 0.25f;
        GameSimulation standard_maximum_health_simulation(standard_maximum_health_scene);
        GameSimulation reduced_maximum_health_simulation(reduced_maximum_health_scene);
        require(reduced_maximum_health_simulation.actors().front().health == 0.25f,
                "The AI difficulty row's damage-scale value must initialize soldier maximum health");
        for (int tick_index = 0; tick_index < 75; ++tick_index)
        {
            standard_maximum_health_simulation.tick({});
            reduced_maximum_health_simulation.tick({});
        }
        require(standard_maximum_health_simulation.health() == reduced_maximum_health_simulation.health(),
                "An AI soldier's maximum health must not scale the damage of its weapon rounds");
        auto low_cover_scene = make_scene(true, false);
        low_cover_scene.collision_triangles.push_back(
            {{Vector3{-3, 3, -2}, Vector3{3, 3, -2}, Vector3{3, 3, -0.9025f}}});
        low_cover_scene.collision_triangles.push_back(
            {{Vector3{-3, 3, -2}, Vector3{3, 3, -0.9025f}, Vector3{-3, 3, -0.9025f}}});
        GameSimulation low_cover_perception(low_cover_scene);
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            low_cover_perception.tick({});
        }
        require(low_cover_perception.actors().front().shots_fired == 0,
                "Retail AI sight must trace to the exact standing target mark below low cover");
        auto crouched_cover_scene = make_scene(true, false);
        crouched_cover_scene.collision_triangles.push_back(
            {{Vector3{-3, 3, -2}, Vector3{3, 3, -2}, Vector3{3, 3, -1.083f}}});
        crouched_cover_scene.collision_triangles.push_back(
            {{Vector3{-3, 3, -2}, Vector3{3, 3, -1.083f}, Vector3{-3, 3, -1.083f}}});
        GameSimulation crouched_cover_perception(crouched_cover_scene);
        PlayerInput crouch_behind_low_cover;
        crouch_behind_low_cover.crouching = true;
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            crouched_cover_perception.tick(crouch_behind_low_cover);
        }
        require(crouched_cover_perception.actors().front().shots_fired > 0,
                "Retail AI sight must use the exact crouched target mark above this low-cover edge");
        input = {};
        input.reloading = true;
        combat.tick(input);
        require(combat.weapon().reload_remaining > 0, "Reload must begin before ammunition is transferred");
        input.reloading = false;
        for (int tick_index = 0; tick_index < 62; ++tick_index)
        {
            combat.tick(input);
        }
        require(combat.weapon().magazine == 32 && combat.weapon().reserve == 164,
                "Reload must transfer ammunition only after the configured duration");

        const auto blocked_scene = make_scene(true, true);
        GameSimulation blocked(blocked_scene);
        input.firing = true;
        for (int tick_index = 0; tick_index < 100; ++tick_index)
        {
            blocked.tick(input);
        }
        require(blocked.weapon().hits == 0 && blocked.actors().front().health == 1,
                "World geometry must occlude projectiles and protect the soldier");
        require(blocked.health() == blocked_scene.player_parameters.maximum_health,
                "Occluded guard must not damage the player through a wall");
        require(blocked.actors().front().behavior == ActorBehavior::investigating,
                "An occluded guard hearing a shot must investigate rather than shoot through the wall");
        GameSimulation replay_first(movement_scene);
        GameSimulation replay_second(movement_scene);
        for (int tick_index = 0; tick_index < 100; ++tick_index)
        {
            PlayerInput replay_input;
            replay_input.forward = tick_index < 20 ? 1.0f : 0.0f;
            replay_input.jumping = tick_index == 30;
            replay_input.firing = tick_index >= 40 && tick_index < 70;
            replay_first.tick(replay_input);
            replay_second.tick(replay_input);
            require(length(replay_first.player_position() - replay_second.player_position()) == 0 &&
                        replay_first.weapon().magazine == replay_second.weapon().magazine &&
                        replay_first.health() == replay_second.health(),
                    "Identical replay input must produce identical simulation state");
        }
        combat.restart();
        require(combat.weapon().shots_fired == 0 && combat.weapon().kills == 0 &&
                    combat.actors().front().health == 1,
                "Restart must reset combat and actor state");
        std::cout << "Gameplay probe passed: root motion, jump, landing, crouch, automatic fire, damage, "
                     "death, reload, occlusion, restart\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Gameplay probe failed: " << exception.what() << '\n';
        return 1;
    }
}
