#include "test_framework.hpp"
#include "igi/player_fall_impact.hpp"
#include "igi/player_ladder.hpp"
#include "igi/weapon_view_recoil.hpp"
#include "igi/weapon_view_sway.hpp"

#include <cmath>

TEST_CASE(GameplayHelpers, FallImpactSafeLanding) {
    // 10 m/s at 30 Hz = 10 * 4096 / 30 = 1365.33 units/tick (well below 14 m/s safe threshold)
    const float safe_velocity = -1365.33f;
    const auto impact = igi::gameplay::calculate_vanilla_fall_impact(safe_velocity, 100.0f);

    ASSERT_NEAR(impact.speed_meters_per_second, 10.0f, 0.1f);
    ASSERT_EQ(impact.damage, 0.0f);
    ASSERT_TRUE(impact.sound_name.empty());
    ASSERT_EQ(impact.hearing_radius_units, 20480.0f); // Safe impact hearing alert
    ASSERT_TRUE(impact.view_kick_units < 0.0f);
}

TEST_CASE(GameplayHelpers, FallImpactDamagingAndLethalLanding) {
    // 20 m/s at 30 Hz = 20 * 4096 / 30 = 2730.67 units/tick
    // Speed is 6 m/s above safe 14 m/s threshold -> damage = (6 / 13) * 100 ≈ 46.15
    const float damaging_velocity = -2730.67f;
    const auto impact = igi::gameplay::calculate_vanilla_fall_impact(damaging_velocity, 100.0f);

    ASSERT_NEAR(impact.speed_meters_per_second, 20.0f, 0.1f);
    ASSERT_NEAR(impact.damage, (6.0f / 13.0f) * 100.0f, 0.5f);
    ASSERT_EQ(impact.sound_name, "player_fall_3");
    ASSERT_EQ(impact.hearing_radius_units, 40960.0f); // Damaging impact alert radius
}

TEST_CASE(GameplayHelpers, WeaponViewSwayLowerAndRaise11Ticks) {
    igi::gameplay::WeaponViewSway sway;
    ASSERT_TRUE(sway.is_settled());
    ASSERT_EQ(sway.pitch_radians(), 0.0f);
    ASSERT_EQ(sway.yaw_radians(), 0.0f);

    sway.lower();
    ASSERT_FALSE(sway.is_settled());

    // Advance 10 ticks: should not be settled yet
    for (int t = 0; t < 10; ++t) {
        sway.advance();
        ASSERT_FALSE(sway.is_settled());
    }

    // 11th tick: should reach target and settle
    sway.advance();
    ASSERT_TRUE(sway.is_settled());
    ASSERT_NEAR(sway.pitch_radians(), igi::gameplay::WeaponViewSway::kLoweredPitchRadians, 1e-4f);
    ASSERT_NEAR(sway.yaw_radians(), igi::gameplay::WeaponViewSway::kLoweredYawRadians, 1e-4f);

    // Raise back to ready position
    sway.raise();
    ASSERT_FALSE(sway.is_settled());

    for (int t = 0; t < 11; ++t) {
        sway.advance();
    }
    ASSERT_TRUE(sway.is_settled());
    ASSERT_NEAR(sway.pitch_radians(), 0.0f, 1e-4f);
    ASSERT_NEAR(sway.yaw_radians(), 0.0f, 1e-4f);
}

TEST_CASE(GameplayHelpers, WeaponViewRecoilRecovery3Ticks) {
    igi::gameplay::WeaponViewRecoil recoil;
    ASSERT_EQ(recoil.pitch_radians(), 0.0f);
    ASSERT_EQ(recoil.yaw_radians(), 0.0f);

    recoil.trigger_degrees(2.0f, 1.0f);
    ASSERT_TRUE(recoil.pitch_radians() > 0.0f);
    ASSERT_TRUE(recoil.yaw_radians() > 0.0f);

    // Advance 3 recovery ticks
    recoil.advance(); // tick 1
    ASSERT_TRUE(recoil.pitch_radians() > 0.0f);
    recoil.advance(); // tick 2
    ASSERT_TRUE(recoil.pitch_radians() > 0.0f);
    recoil.advance(); // tick 3: fully recovered to zero
    ASSERT_NEAR(recoil.pitch_radians(), 0.0f, 1e-5f);
    ASSERT_NEAR(recoil.yaw_radians(), 0.0f, 1e-5f);
}

TEST_CASE(GameplayHelpers, LadderActivationAndClimbLine) {
    const igi::gameplay::Vector3 bottom{0.0f, 0.0f, 0.0f};
    const igi::gameplay::Vector3 top{0.0f, 0.0f, 12288.0f}; // 10 steps of 1228.8 units
    const igi::gameplay::Vector3 normal{0.0f, 1.0f, 0.0f};

    igi::gameplay::LadderPlacement ladder(bottom, top, bottom, top, normal);
    ASSERT_EQ(ladder.climb_line().step_count(), 10);

    const auto mid_pos = ladder.climb_line().position_at_step(5);
    ASSERT_NEAR(mid_pos.z, 6144.0f, 1e-3f);

    // Player positioned 1 meter in front of ladder (y = 4096 < 8192) looking towards -Y
    const igi::gameplay::Vector3 player_pos{0.0f, 4096.0f, 1000.0f};
    const igi::gameplay::Vector3 facing_ladder{0.0f, -1.0f, 0.0f};
    ASSERT_TRUE(ladder.can_activate(player_pos, facing_ladder));

    // Player looking away from ladder (+Y) -> cannot activate
    const igi::gameplay::Vector3 facing_away{0.0f, 1.0f, 0.0f};
    ASSERT_FALSE(ladder.can_activate(player_pos, facing_away));

    // Player too far (> 8192 units away, y = 10000) -> cannot activate
    const igi::gameplay::Vector3 far_pos{0.0f, 10000.0f, 1000.0f};
    ASSERT_FALSE(ladder.can_activate(far_pos, facing_ladder));
}
