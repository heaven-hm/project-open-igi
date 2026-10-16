#include "test_framework.hpp"

#include "igi/ai_system.hpp"
#include "igi/mission_world.hpp"
#include "igi/weapon_system.hpp"
#include "igi/world_collision.hpp"

#include <cmath>
#include <filesystem>

using namespace igi::test;
using igi::gameplay::AiSystem;
using igi::gameplay::MissionWorld;
using igi::gameplay::NavigationPosition;
using igi::gameplay::PlayerPhysicsState;
using igi::gameplay::SoldierState;
using igi::gameplay::WeaponSystem;
using igi::gameplay::WorldCollision;

namespace {

constexpr double kUnitsPerMeter = 4096.0;
constexpr float kRetailTick = 1.0f / 30.0f;
constexpr float kEyeHeight = 7372.8f;

// Finds a soldier with a clear eye-to-eye line to a point `meters` in front
// of its facing. Returns the soldier index or -1.
int find_exposed_soldier(
    const MissionWorld& world,
    const WorldCollision& collision,
    const AiSystem& ai,
    double meters,
    NavigationPosition& out_player_pos) {
    const auto& agents = ai.agents();
    for (std::size_t i = 0; i < agents.size(); ++i) {
        const auto& soldier = agents[i];
        const double fx = std::cos(soldier.yaw);
        const double fy = std::sin(soldier.yaw);
        NavigationPosition candidate{
            soldier.position.x + fx * meters * kUnitsPerMeter,
            soldier.position.y + fy * meters * kUnitsPerMeter,
            soldier.position.z,
        };
        const NavigationPosition soldier_eye{
            soldier.position.x, soldier.position.y, soldier.position.z + kEyeHeight};
        const NavigationPosition player_eye{
            candidate.x, candidate.y, candidate.z + kEyeHeight};
        const auto los = collision.raycast(world, soldier_eye, player_eye, false, true);
        if (!los.hit || los.fraction >= 0.98f) {
            out_player_pos = candidate;
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace

// Enemy direction: an exposed player must be spotted and take damage.
TEST_CASE(CombatIntegration, ExposedPlayerTakesEnemyFire) {
    const std::filesystem::path retail_root = "/Users/haseeb-mir/Downloads/IGI";
    if (!std::filesystem::exists(retail_root)) {
        return; // Skip if retail files are not installed
    }
    MissionWorld world;
    ASSERT_TRUE(world.load_from_retail_install(retail_root, 1));
    WorldCollision collision;
    collision.build_index(world);
    AiSystem ai;
    ai.initialize(world);
    ASSERT_TRUE(ai.alive_count() > 0u);

    NavigationPosition player_pos{};
    const int target = find_exposed_soldier(world, collision, ai, 8.0, player_pos);
    ASSERT_TRUE(target >= 0);

    PlayerPhysicsState player;
    player.position = player_pos;
    player.yaw = 0.0f;
    player.is_grounded = true;
    player.is_running = false;
    player.is_crouching = false;
    player.health = 100.0f;

    float total_damage = 0.0f;
    for (int tick = 0; tick < 600; ++tick) {
        total_damage += ai.update(kRetailTick, world, collision, player);
    }
    ASSERT_TRUE(total_damage > 0.0f);
    ASSERT_TRUE(ai.agents()[static_cast<std::size_t>(target)].state == SoldierState::Combat);
}

// Player direction: aimed retail-weapon fire through the real firing chain
// (WeaponSystem -> wall-limited ray -> apply_shot) must wound and kill.
TEST_CASE(CombatIntegration, AimedFireKillsExposedSoldier) {
    const std::filesystem::path retail_root = "/Users/haseeb-mir/Downloads/IGI";
    if (!std::filesystem::exists(retail_root)) {
        return; // Skip if retail files are not installed
    }
    MissionWorld world;
    ASSERT_TRUE(world.load_from_retail_install(retail_root, 1));
    WorldCollision collision;
    collision.build_index(world);
    AiSystem ai;
    ai.initialize(world);

    NavigationPosition player_pos{};
    const int target = find_exposed_soldier(world, collision, ai, 15.0, player_pos);
    ASSERT_TRUE(target >= 0);

    const auto& soldier = ai.agents()[static_cast<std::size_t>(target)];
    const float yaw_to_soldier = static_cast<float>(std::atan2(
        soldier.position.y - player_pos.y, soldier.position.x - player_pos.x));
    const NavigationPosition eye{player_pos.x, player_pos.y, player_pos.z + kEyeHeight};

    WeaponSystem weapons;
    bool wounded = false;
    for (int tick = 0; tick < 300; ++tick) {
        const bool pressed = (tick == 0);
        const auto fire = weapons.update(kRetailTick, true, pressed, eye, yaw_to_soldier, 0.0f);
        if (!fire.fired) {
            continue;
        }
        const NavigationPosition far_end{
            fire.shot.origin.x + fire.shot.direction.x * fire.shot.range_units,
            fire.shot.origin.y + fire.shot.direction.y * fire.shot.range_units,
            fire.shot.origin.z + fire.shot.direction.z * fire.shot.range_units,
        };
        const auto wall = collision.raycast(world, fire.shot.origin, far_end, false, true);
        const auto hit = ai.apply_shot(fire.shot, wall.hit ? wall.fraction : 1.0f);
        if (hit.hit) {
            wounded = true;
        }
        if (!ai.agents()[static_cast<std::size_t>(target)].alive()) {
            break;
        }
    }
    ASSERT_TRUE(wounded);
    ASSERT_TRUE(!ai.agents()[static_cast<std::size_t>(target)].alive());
}
