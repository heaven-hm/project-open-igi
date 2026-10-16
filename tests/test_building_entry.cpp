#include "test_framework.hpp"

#include "igi/mission_world.hpp"
#include "igi/world_collision.hpp"

#include <cmath>
#include <filesystem>

using namespace igi::test;
using igi::gameplay::MissionWorld;
using igi::gameplay::NavigationPosition;
using igi::gameplay::PlacedObjectKind;
using igi::gameplay::WorldCollision;

namespace {

constexpr double kPi = 3.14159265358979323846;

} // namespace

// End-to-end building entry against retail Mission 1 data: a closed door must
// block passage along some horizontal axis through its center, and opening it
// via the retail Activate flow must clear that axis.
TEST_CASE(BuildingEntry, RetailDoorBlocksWhenClosedAndOpensForEntry) {
    const std::filesystem::path retail_root = "/Users/haseeb-mir/Downloads/IGI";
    if (!std::filesystem::exists(retail_root)) {
        return; // Skip if retail files are not installed
    }
    MissionWorld world;
    ASSERT_TRUE(world.load_from_retail_install(retail_root, 1));

    int door_index = -1;
    for (std::size_t i = 0; i < world.placed_objects().size(); ++i) {
        if (world.placed_objects()[i].kind == PlacedObjectKind::Door &&
            !world.placed_objects()[i].model_id.empty() &&
            world.find_model(world.placed_objects()[i].model_id) != nullptr) {
            door_index = static_cast<int>(i);
            break;
        }
    }
    ASSERT_TRUE(door_index >= 0);

    WorldCollision collision;
    collision.build_index(world);

    const NavigationPosition door_pos = world.placed_objects()[static_cast<std::size_t>(door_index)].position;
    const double probe_z = door_pos.z + 4000.0; // ~1 m above door base

    // Find a horizontal axis the closed door blocks.
    bool found_blocking_axis = false;
    double axis_x = 1.0;
    double axis_y = 0.0;
    for (int step = 0; step < 8; ++step) {
        const double angle = static_cast<double>(step) * kPi / 4.0;
        const double dx = std::cos(angle);
        const double dy = std::sin(angle);
        const NavigationPosition from{door_pos.x - dx * 1500.0, door_pos.y - dy * 1500.0, probe_z};
        const NavigationPosition to{door_pos.x + dx * 1500.0, door_pos.y + dy * 1500.0, probe_z};
        const auto hit = collision.raycast(world, from, to, true, false);
        if (hit.hit) {
            found_blocking_axis = true;
            axis_x = dx;
            axis_y = dy;
            break;
        }
    }
    ASSERT_TRUE(found_blocking_axis);

    // HUD hint must name the retail Activate key. Check while the door is
    // still closed and unmoved, from close range along the blocking axis.
    const float player_yaw = static_cast<float>(std::atan2(axis_y, axis_x));
    const NavigationPosition hint_eye{
        door_pos.x - axis_x * 1200.0, door_pos.y - axis_y * 1200.0, door_pos.z + 7372.8};
    const std::string closed_hint = world.get_interaction_hint(hint_eye, player_yaw);
    ASSERT_TRUE(closed_hint.find("[Q]") != std::string::npos);

    // Stand off along the axis, face the door, and use the retail Activate flow.
    const NavigationPosition player_pos{
        door_pos.x - axis_x * 4500.0, door_pos.y - axis_y * 4500.0, door_pos.z};
    auto interact = world.try_interact(player_pos, player_yaw);
    ASSERT_TRUE(interact.interacted);
    ASSERT_TRUE(interact.kind == PlacedObjectKind::Door);

    // Animate the door fully open. The doorway stays at the closed position
    // (the panel slides aside and collision is skipped past 0.85 progress).
    for (int tick = 0; tick < 40; ++tick) {
        world.update_doors(1.0f / 30.0f);
    }
    ASSERT_TRUE(
        world.placed_objects()[static_cast<std::size_t>(door_index)].open_progress > 0.85f);
    const NavigationPosition from{door_pos.x - axis_x * 1500.0, door_pos.y - axis_y * 1500.0, probe_z};
    const NavigationPosition to{door_pos.x + axis_x * 1500.0, door_pos.y + axis_y * 1500.0, probe_z};
    const auto open_hit = collision.raycast(world, from, to, true, false);
    ASSERT_FALSE(open_hit.hit);
}
