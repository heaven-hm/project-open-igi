#include "test_framework.hpp"
#include "igi/hud_font.hpp"
#include "igi/mission_world.hpp"
#include "igi/player_fall_impact.hpp"
#include "igi/player_ladder.hpp"
#include "igi/player_view_bob.hpp"
#include "igi/sight_zoom.hpp"
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

TEST_CASE(GameplayHelpers, WorldObjectInteractionAndDestruction) {
    igi::gameplay::MissionWorld world;
    auto& objects = world.placed_objects_mutable();

    // 1. Add interactive door
    igi::gameplay::PlacedObject door;
    door.class_name = "Door";
    door.instance_name = "door_01";
    door.kind = igi::gameplay::PlacedObjectKind::Door;
    door.is_interactive = true;
    door.position = {1000.0, 0.0, 0.0};
    door.closed_position = door.position;
    door.basis = igi::gameplay::MissionWorld::basis_from_euler(0.0f, 0.0f, 0.0f);
    objects.push_back(door);

    // 2. Add computer terminal
    igi::gameplay::PlacedObject terminal;
    terminal.class_name = "Terminal";
    terminal.instance_name = "train_computer";
    terminal.kind = igi::gameplay::PlacedObjectKind::Terminal;
    terminal.is_interactive = true;
    terminal.position = {0.0, 1000.0, 0.0};
    objects.push_back(terminal);

    // 3. Add destructible camera
    igi::gameplay::PlacedObject camera;
    camera.class_name = "SCamera";
    camera.instance_name = "cam_yard";
    camera.kind = igi::gameplay::PlacedObjectKind::SecurityCamera;
    camera.is_destructible = true;
    camera.health = 30.0f;
    camera.position = {0.0, 0.0, 5000.0};
    objects.push_back(camera);

    // Player positioned in front of door looking at door (+X direction, yaw = 0)
    const auto door_interact = world.try_interact({0.0, 0.0, 0.0}, 0.0f, 3000.0f);
    ASSERT_TRUE(door_interact.interacted);
    ASSERT_TRUE(door_interact.kind == igi::gameplay::PlacedObjectKind::Door);
    ASSERT_TRUE(world.placed_objects()[0].is_open);

    // Update doors advances door position
    world.update_doors(0.25f);
    ASSERT_NEAR(world.placed_objects()[0].open_progress, 0.5f, 1e-3f);
    ASSERT_TRUE(world.placed_objects()[0].position.x > 1000.0);

    // Player facing terminal (+Y direction, yaw = pi/2)
    constexpr float kHalfPi = 1.5707963f;
    const auto term_interact = world.try_interact({0.0, 0.0, 0.0}, kHalfPi, 3000.0f);
    ASSERT_TRUE(term_interact.interacted);
    ASSERT_TRUE(term_interact.kind == igi::gameplay::PlacedObjectKind::Terminal);
    ASSERT_EQ(term_interact.script_variable, "DOWNLOAD_DONE");

    // Camera takes damage
    ASSERT_FALSE(world.damage_object(2, 20.0f)); // 30 - 20 = 10 (not destroyed yet)
    ASSERT_FALSE(world.placed_objects()[2].is_destroyed);
    ASSERT_TRUE(world.damage_object(2, 15.0f)); // 10 - 15 <= 0 (destroyed!)
    ASSERT_TRUE(world.placed_objects()[2].is_destroyed);
}

TEST_CASE(GameplayHelpers, SightZoomMechanics) {
    igi::gameplay::SightZoom zoom;

    // Test MP5SD toggle zoom and 0.8 / 1.25 glide factor
    zoom.set_weapon("MP5SD");
    ASSERT_TRUE(zoom.mode() == igi::gameplay::ZoomMode::Mp5Sight);
    ASSERT_FALSE(zoom.is_active());
    ASSERT_NEAR(zoom.current_tan_x(), igi::gameplay::SightZoom::kDefaultTanX, 1e-4f);

    // Rising edge of alternate fire toggles zoom ON
    zoom.tick(true, true);
    ASSERT_TRUE(zoom.is_active());
    // 1 tick of zoom in: tanX narrows by factor 0.8
    ASSERT_NEAR(zoom.current_tan_x(), igi::gameplay::SightZoom::kDefaultTanX * 0.8f, 1e-3f);

    // After several ticks, reaches min_tan_x (0.35f)
    for (int i = 0; i < 10; ++i) {
        zoom.tick(false, true);
    }
    ASSERT_NEAR(zoom.current_tan_x(), 0.35f, 1e-3f);
    ASSERT_TRUE(zoom.magnification() > 1.9f);

    // Another toggle rising edge turns zoom OFF
    zoom.tick(true, true);
    ASSERT_FALSE(zoom.is_active());
    // Glides back out by factor 1.25 per tick
    for (int i = 0; i < 10; ++i) {
        zoom.tick(false, false);
    }
    ASSERT_NEAR(zoom.current_tan_x(), igi::gameplay::SightZoom::kDefaultTanX, 1e-4f);

    // Test Dragunov Sniper Scope
    zoom.set_weapon("Dragunov");
    ASSERT_TRUE(zoom.mode() == igi::gameplay::ZoomMode::DragunovScope);
    // Hold alternate fire engages scope
    zoom.tick(true, true);
    ASSERT_TRUE(zoom.is_active());
    for (int i = 0; i < 60; ++i) {
        zoom.tick(false, true);
    }
    ASSERT_NEAR(zoom.current_tan_x(), 0.1051f, 1e-3f);
    // Dragunov provides ~6.6x optical magnification
    ASSERT_TRUE(zoom.magnification() > 6.0f);
    ASSERT_NEAR(zoom.current_fov_radians(), 2.0f * std::atan(0.1051f), 1e-3f);

    // Reset restores default FOV
    zoom.reset();
    ASSERT_FALSE(zoom.is_active());
    ASSERT_NEAR(zoom.current_tan_x(), igi::gameplay::SightZoom::kDefaultTanX, 1e-4f);
}

TEST_CASE(GameplayHelpers, PlayerViewBobLocomotion) {
    igi::gameplay::PlayerViewBob bob;

    // Stationary on ground: amplitude settles to 0
    bob.update(1.0f / 30.0f, true, 0.0f, false, false);
    ASSERT_NEAR(bob.amplitude(), 0.0f, 1e-4f);
    auto off = bob.current_offset();
    ASSERT_NEAR(off.vertical_offset, 0.0f, 1e-4f);
    ASSERT_NEAR(off.lateral_offset, 0.0f, 1e-4f);

    // Running forward: amplitude ramps up to ~65 units, phase advances by 0.22/tick
    for (int tick = 0; tick < 30; ++tick) {
        bob.update(1.0f / 30.0f, true, 680.0f, true, false);
    }
    ASSERT_TRUE(bob.amplitude() > 50.0f);
    off = bob.current_offset();
    ASSERT_TRUE(std::fabs(off.vertical_offset) > 0.0f);
    ASSERT_TRUE(std::fabs(off.lateral_offset) > 0.0f);

    // Stopping: amplitude decays back towards 0
    for (int tick = 0; tick < 60; ++tick) {
        bob.update(1.0f / 30.0f, true, 0.0f, false, false);
    }
    ASSERT_TRUE(bob.amplitude() < 5.0f);
}

TEST_CASE(GameplayHelpers, HudFontMonospaceRenderer) {
    ASSERT_EQ(igi::gameplay::HudFont::string_width("M16A2", 1), 5 * 8);
    ASSERT_EQ(igi::gameplay::HudFont::string_width("HP 100%", 2), 7 * 16);
    ASSERT_EQ(igi::gameplay::HudFont::line_height(1), 8);
    ASSERT_EQ(igi::gameplay::HudFont::line_height(2), 16);

    std::vector<std::uint32_t> buffer(64 * 32, 0xFF000000u);
    igi::gameplay::HudFont::draw_string(
        buffer.data(), 64, 32, 2, 2, "TEST", 0xFFFFFFFFu, 1, true);

    // Verify some pixels were written as text and shadow
    bool found_white = false;
    for (std::uint32_t px : buffer) {
        if (px == 0xFFFFFFFFu) {
            found_white = true;
            break;
        }
    }
    ASSERT_TRUE(found_white);
}

TEST_CASE(GameplayHelpers, InteractionHintAndAlarmState) {
    igi::gameplay::MissionWorld world;
    ASSERT_FALSE(world.is_alarm_active());
    world.set_alarm_active(true);
    ASSERT_TRUE(world.is_alarm_active());

    // Add closed door at (1000, 0, 0)
    auto& objects = world.placed_objects_mutable();
    igi::gameplay::PlacedObject door;
    door.class_name = "Door";
    door.instance_name = "security_gate";
    door.kind = igi::gameplay::PlacedObjectKind::Door;
    door.is_interactive = true;
    door.position = {1000.0, 0.0, 0.0};
    objects.push_back(door);

    // Player at origin looking at door (+X, yaw = 0); retail Activate key is Q.
    const std::string hint = world.get_interaction_hint({0.0, 0.0, 0.0}, 0.0f);
    ASSERT_EQ(hint, "[Q] OPEN DOOR");

    // Player looking away (-X, yaw = pi)
    constexpr float kPi = 3.14159265f;
    const std::string no_hint = world.get_interaction_hint({0.0, 0.0, 0.0}, kPi);
    ASSERT_TRUE(no_hint.empty());
}


