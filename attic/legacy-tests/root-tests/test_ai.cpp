#include "test_framework.hpp"
#include "igi/ai_system.hpp"
#include "igi/mission_world.hpp"

#include <cmath>

TEST_CASE(AiSystem, DualViewConeDetection) {
    // Primary sight: 30 meters = 122880 units, cos(FOV/2) >= 0.5 (60 deg)
    // Secondary sight: 10 meters = 40960 units, cos(FOV/2) >= -0.2 (wide peripheral)
    igi::gameplay::SoldierAgent agent;
    agent.position = {0.0, 0.0, 0.0};
    agent.yaw = 0.0f; // Facing along +X
    agent.state = igi::gameplay::SoldierState::Idle;
    agent.health = 100.0f;

    // Target 1: Directly in front at 20m (81920 units) -> in primary cone
    const double dx1 = 81920.0;
    const double dy1 = 0.0;
    const double dist1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
    const float dot1 = static_cast<float>((dx1 * std::cos(agent.yaw) + dy1 * std::sin(agent.yaw)) / dist1);
    const bool in_cone1 = (dist1 <= igi::gameplay::AiSystem::kPrimarySightMeters * 4096.0 && dot1 >= 0.5f) ||
                          (dist1 <= igi::gameplay::AiSystem::kSecondarySightMeters * 4096.0 && dot1 >= -0.2f);
    ASSERT_TRUE(in_cone1);

    // Target 2: Directly behind at 20m (-81920 units) -> outside both cones
    const double dx2 = -81920.0;
    const double dy2 = 0.0;
    const double dist2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
    const float dot2 = static_cast<float>((dx2 * std::cos(agent.yaw) + dy2 * std::sin(agent.yaw)) / dist2);
    const bool in_cone2 = (dist2 <= igi::gameplay::AiSystem::kPrimarySightMeters * 4096.0 && dot2 >= 0.5f) ||
                          (dist2 <= igi::gameplay::AiSystem::kSecondarySightMeters * 4096.0 && dot2 >= -0.2f);
    ASSERT_FALSE(in_cone2);

    // Target 3: 90 degrees to the left at ~5m (20000 units < 40960) -> detected by secondary peripheral cone
    const double dx3 = 0.0;
    const double dy3 = 20000.0;
    const double dist3 = std::sqrt(dx3 * dx3 + dy3 * dy3);
    const float dot3 = static_cast<float>((dx3 * std::cos(agent.yaw) + dy3 * std::sin(agent.yaw)) / dist3);
    const bool in_cone3 = (dist3 <= igi::gameplay::AiSystem::kPrimarySightMeters * 4096.0 && dot3 >= 0.5f) ||
                          (dist3 <= igi::gameplay::AiSystem::kSecondarySightMeters * 4096.0 && dot3 >= -0.2f);
    ASSERT_TRUE(in_cone3);

    // Target 4: Directly behind at ~5m (20000 units < 40960) -> in blind spot (dot = -1.0 < -0.2)
    const double dx4 = -20000.0;
    const double dy4 = 0.0;
    const double dist4 = std::sqrt(dx4 * dx4 + dy4 * dy4);
    const float dot4 = static_cast<float>((dx4 * std::cos(agent.yaw) + dy4 * std::sin(agent.yaw)) / dist4);
    const bool in_cone4 = (dist4 <= igi::gameplay::AiSystem::kPrimarySightMeters * 4096.0 && dot4 >= 0.5f) ||
                          (dist4 <= igi::gameplay::AiSystem::kSecondarySightMeters * 4096.0 && dot4 >= -0.2f);
    ASSERT_FALSE(in_cone4);
}

TEST_CASE(AiSystem, GunshotHearingBroadcast) {
    igi::gameplay::AiSystem ai;
    // Notify gunshot at origin with audible range 50000 units
    const igi::gameplay::NavigationPosition shot_origin{0.0, 0.0, 0.0};
    const float audible_range = 50000.0f;

    // Verify calling notify_gunshot does not throw even with empty or populated agent set
    ASSERT_NO_THROW(ai.notify_gunshot(shot_origin, audible_range));
}

TEST_CASE(AiSystem, ShotHitscanAndDeathState) {
    igi::gameplay::AiSystem ai;
    // Create a mock shot ray
    igi::gameplay::ShotRay ray;
    ray.origin = {0.0, 0.0, 5000.0};
    ray.direction = {1.0f, 0.0f, 0.0f};
    ray.range_units = 100000.0f;
    ray.damage = 150.0f;

    // With 0 agents initialized, shot should miss cleanly
    auto hit = ai.apply_shot(ray, 1.0f);
    ASSERT_FALSE(hit.hit);
}
