#include "igi/game_simulation.hpp"
#include "igi/weapon_accuracy.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

igi::gameplay::MissionScene make_scene()
{
    using igi::gameplay::Vector3;
    igi::gameplay::MissionScene scene;
    scene.collision_triangles = {{{Vector3{-100, -100, -2}, Vector3{100, -100, -2}, Vector3{100, 100, -2}}},
                                 {{Vector3{-100, -100, -2}, Vector3{100, 100, -2}, Vector3{-100, 100, -2}}}};
    return scene;
}

} // namespace

int main()
{
    try
    {
        auto parameters = make_scene().weapon_parameters;
        parameters.minimum_spread_degrees = -2;
        parameters.maximum_spread_degrees = 6;
        require(igi::gameplay::WeaponAccuracy::cone_angle(parameters, 3) ==
                        2 * igi::gameplay::WeaponAccuracy::degrees_to_radians &&
                    igi::gameplay::WeaponAccuracy::cone_angle(parameters, 4) ==
                        6 * igi::gameplay::WeaponAccuracy::degrees_to_radians,
                "Retail spread changes after four shots and takes the absolute signed half-angle");
        const auto cone = igi::gameplay::WeaponAccuracy::sample_cone(2, {0.5f, 0.25f, 0.75f, 0.5f});
        require(cone.radius == 0.1875f &&
                    cone.bearing == 0.5f * igi::gameplay::WeaponAccuracy::full_turn_radians,
                "Cone radius must multiply three samples; the fourth selects bearing");
        igi::gameplay::RetailRandomGenerator zero_cone_random;
        const auto zero_cone_state = zero_cone_random.state();
        static_cast<void>(igi::gameplay::WeaponAccuracy::sample_direction(
            0, {0, 1, 0}, {1, 0, 0}, {0, 0, 1}, zero_cone_random));
        require(zero_cone_random.state() == zero_cone_state, "A zero cone must consume no direction samples");
        parameters.fixed_recoil_pitch_degrees = 2;
        parameters.fixed_recoil_yaw_degrees = -3;
        igi::gameplay::RetailRandomGenerator recoil_random;
        const auto recoil = igi::gameplay::WeaponAccuracy::sample_recoil(parameters, recoil_random);
        require(recoil.pitch == 2 * igi::gameplay::WeaponAccuracy::degrees_to_radians &&
                    recoil.yaw == -3 * igi::gameplay::WeaponAccuracy::degrees_to_radians,
                "Fixed recoil must use original signed degree-to-radian terms");
        igi::gameplay::RetailRandomGenerator expected_recoil_random;
        static_cast<void>(expected_recoil_random.next_single());
        static_cast<void>(expected_recoil_random.next_single());
        require(recoil_random.state() == expected_recoil_random.state(),
                "Even fixed recoil consumes the original accepted pair of random draws");

        auto scene = make_scene();
        scene.weapon_parameters.projectiles_per_round = 3;
        scene.weapon_parameters.minimum_spread_degrees = -10;
        scene.weapon_parameters.maximum_spread_degrees = 20;
        igi::gameplay::GameSimulation simulation(scene);
        igi::gameplay::PlayerInput input;
        input.firing = true;
        simulation.tick(input);
        require(simulation.projectiles().size() == 3,
                "A round must create the original configured number of projectiles");
        require(simulation.weapon().magazine == 31 && simulation.weapon().shots_fired == 1,
                "Multiple projectiles consume one round and count as one shot");
        for (const auto& projectile : simulation.projectiles())
        {
            require(std::abs(projectile.direction.x) > 0 && std::abs(projectile.direction.z) > 0,
                    "Nonzero original spread must produce lateral and vertical projectile offsets");
            require(std::abs(igi::gameplay::length(projectile.direction) - 1.0f) < 0.000001f,
                    "Spread projectile directions must remain normalized");
        }
        const auto first_direction = simulation.projectiles().front().direction;
        simulation.restart();
        simulation.tick(input);
        require(igi::gameplay::length(simulation.projectiles().front().direction - first_direction) == 0,
                "Restart must restore the initial random stream for reproducible port replays");

        auto straight_scene = make_scene();
        igi::gameplay::GameSimulation straight(straight_scene);
        straight.tick(input);
        require(igi::gameplay::length(straight.projectiles().front().direction - straight.aim_direction()) ==
                    0,
                "Zero spread must keep the original barrel direction");
        auto empty_round_scene = make_scene();
        empty_round_scene.weapon_parameters.projectiles_per_round = 0;
        igi::gameplay::GameSimulation empty_round(empty_round_scene);
        empty_round.tick(input);
        require(empty_round.projectiles().empty() && empty_round.weapon().magazine == 31,
                "Original zero-projectile loop still performs round bookkeeping");
        auto invalid_scene = make_scene();
        invalid_scene.weapon_parameters.minimum_spread_degrees = std::numeric_limits<float>::quiet_NaN();
        bool rejected = false;
        try
        {
            igi::gameplay::GameSimulation invalid(invalid_scene);
            invalid.tick(input);
        }
        catch (const std::runtime_error&)
        {
            rejected = true;
        }
        require(rejected, "Invalid spread must fail through a controlled runtime exception");
        {
            auto cadence_scene = make_scene();
            cadence_scene.weapon_parameters.automatic = true;
            cadence_scene.weapon_parameters.projectiles_per_round = 1;
            cadence_scene.weapon_parameters.shots_per_minute = 600;
            cadence_scene.weapon_parameters.maximum_rounds_per_burst = 1000;
            igi::gameplay::GameSimulation cadence(cadence_scene);
            igi::gameplay::PlayerInput held;
            held.firing = true;
            for (int tick = 0; tick < 12; ++tick)
            {
                cadence.tick(held);
            }
            require(cadence.weapon().shots_fired == 6,
                    "600 RPM retail cooldown is two ticks, so twelve held ticks fire six rounds");
        }
        // Values executed from the original cooldown block under 53-bit, round-toward-zero control word 0x0E7F.
        const std::pair<std::int32_t, std::int32_t> retail_cooldowns[] = {
            {9, 199}, {18, 99}, {25, 71}, {36, 49}, {45, 39}, {300, 5}, {400, 4}, {600, 2}, {700, 2},
            {750, 2}, {900, 1}, {1800, 0}};
        for (const auto& [rate, ticks] : retail_cooldowns)
        {
            require(igi::gameplay::WeaponAccuracy::cooldown_ticks(rate) == ticks,
                    "Cooldown ticks must match the original rounding-toward-zero calculation");
        }
        std::cout << "Weapon projectile count, spread, normalization and restart checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Weapon accuracy check failed: " << exception.what() << '\n';
        return 1;
    }
}
