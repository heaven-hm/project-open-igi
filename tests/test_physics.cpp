#include "test_framework.hpp"
#include "igi/human_locomotion_graph.hpp"
#include "igi/human_motion.hpp"
#include "igi/mission_world.hpp"
#include "igi/world_collision.hpp"

#include <cmath>

TEST_CASE(Physics, HumanLocomotionGraphTopology) {
    ASSERT_EQ(igi::gameplay::HumanLocomotionGraph::state_count, 49u);

    // State 0 (Idle) should have valid outgoing transitions
    const auto idle_trans = igi::gameplay::HumanLocomotionGraph::transitions(0);
    ASSERT_FALSE(idle_trans.empty());

    // Every edge across all 49 states must target a valid state index in [0, 48]
    for (int s = 0; s < 49; ++s) {
        const auto trans = igi::gameplay::HumanLocomotionGraph::transitions(s);
        for (int target : trans) {
            ASSERT_TRUE(target >= 0 && target < 49);
        }
    }

    // Invalid negative state or >= 49 should yield empty transitions
    const auto invalid_trans = igi::gameplay::HumanLocomotionGraph::transitions(99);
    ASSERT_TRUE(invalid_trans.empty());
}

TEST_CASE(Physics, HumanMotionAirborneAndLadder) {
    // Airborne gravity integration: retail address 0x00408460
    // In each tick, gravity subtracts 84.74169 units from Z velocity
    igi::gameplay::Basis3x3 identity_basis{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    igi::gameplay::Vector3 vel{0.0f, 0.0f, 0.0f};
    vel = igi::gameplay::HumanMotion::integrate_airborne(
        vel, false, false, false, false, identity_basis, 0.0f);
    ASSERT_NEAR(vel.z, -84.74169f, 1e-4f);

    vel = igi::gameplay::HumanMotion::integrate_airborne(
        vel, false, false, false, false, identity_basis, 0.0f);
    ASSERT_NEAR(vel.z, -169.48338f, 1e-3f);

    // Ladder slide integration: retail address 0x0040DBF0
    // X and Y multiplied by 0.99; Z evaluated as (Z - 44.600887) * 0.99
    igi::gameplay::Vector3 ladder_vel{100.0f, -50.0f, 10.0f};
    ladder_vel = igi::gameplay::HumanMotion::integrate_ladder_slide(ladder_vel);
    ASSERT_NEAR(ladder_vel.x, 99.0f, 1e-4f);
    ASSERT_NEAR(ladder_vel.y, -49.5f, 1e-4f);
    const float expected_z = (10.0f - 44.600887f) * 0.99f;
    ASSERT_NEAR(ladder_vel.z, expected_z, 1e-4f);
}

TEST_CASE(Physics, RetailWallSweepProbeHeightsAndConstants) {
    ASSERT_NEAR(igi::gameplay::WorldCollision::kLowProbeHeight, 409.6f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kMidProbeStanding, 1638.4f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kMidProbeCrouching, 819.2f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kStandingEyeHeight, 7372.8f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kCrouchingEyeHeight, 5324.8f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kWallSkinUnits, 10.24f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kSeparationHeight, 7372.8f, 1e-3f);
    ASSERT_NEAR(igi::gameplay::WorldCollision::kSeparationRadius, 2252.8f, 1e-3f);
}

TEST_CASE(Physics, CylinderSeparationRepulsion) {
    const igi::gameplay::NavigationPosition start_pos{0.0, 0.0, 0.0};
    const igi::gameplay::NavigationPosition target_pos{100.0, 0.0, 0.0};

    // Blocker soldier placed at (1000, 0, 0), well within separation radius (kSeparationRadius = 2252.8 units)
    std::vector<igi::gameplay::NavigationPosition> blockers = {
        {1000.0, 0.0, 0.0}
    };

    // Candidate moves toward blocker: cand_dist (900) < start_dist (1000), so movement is blocked -> returns start_pos
    const auto separated = igi::gameplay::WorldCollision::resolve_cylinder_separation(start_pos, target_pos, blockers);
    ASSERT_EQ(separated.x, start_pos.x);
    ASSERT_EQ(separated.y, start_pos.y);
    ASSERT_EQ(separated.z, start_pos.z);

    // Candidate moving AWAY from blocker (target_pos = (-100, 0, 0)) should be accepted
    const igi::gameplay::NavigationPosition retreat_pos{-100.0, 0.0, 0.0};
    const auto retreated = igi::gameplay::WorldCollision::resolve_cylinder_separation(start_pos, retreat_pos, blockers);
    ASSERT_EQ(retreated.x, retreat_pos.x);
    ASSERT_EQ(retreated.y, retreat_pos.y);
    ASSERT_EQ(retreated.z, retreat_pos.z);
}

TEST_CASE(Physics, SlopeSlideDamping) {
    // Normal tilted at 45 degrees
    const igi::gameplay::Vector3 steep_normal{0.70710678f, 0.0f, 0.70710678f};
    bool is_grounded = true;
    float slide_speed = 100.0f;
    bool is_sliding = false;
    igi::gameplay::Vector3 displacement{0.0f, 0.0f, 0.0f};

    igi::gameplay::WorldCollision::update_slope_slide(
        steep_normal, is_grounded, slide_speed, is_sliding, displacement);
    ASSERT_TRUE(is_sliding);
    ASSERT_TRUE(slide_speed > 0.0f);
    ASSERT_TRUE(displacement.x > 0.0f); // sliding downhill along +X
}
