#include "test_framework.hpp"
#include "igi/weapon_system.hpp"

#include <cmath>

TEST_CASE(WeaponSystem, DefaultLoadoutAndSlotSelection) {
    igi::gameplay::WeaponSystem ws;
    ASSERT_EQ(ws.current_stats().name, "M16A2");
    ASSERT_EQ(ws.rounds_in_magazine(), 30);
    ASSERT_EQ(ws.reserve_ammo(), 120);

    // Slot 1: Glock
    ws.select_slot(1);
    ASSERT_EQ(ws.current_stats().name, "Glock");
    ASSERT_EQ(ws.rounds_in_magazine(), 15);

    // Slot 2: MP5SD
    ws.select_slot(2);
    ASSERT_EQ(ws.current_stats().name, "MP5SD");
    ASSERT_EQ(ws.rounds_in_magazine(), 30);

    // Slot 3: AK47
    ws.select_slot(3);
    ASSERT_EQ(ws.current_stats().name, "AK47");

    // Slot 4: Dragunov
    ws.select_slot(4);
    ASSERT_EQ(ws.current_stats().name, "Dragunov");
    ASSERT_EQ(ws.rounds_in_magazine(), 10);

    // Out-of-bounds slot should be ignored
    ws.select_slot(99);
    ASSERT_EQ(ws.current_stats().name, "Dragunov");
}

TEST_CASE(WeaponSystem, RateOfFireAndCooldown) {
    igi::gameplay::WeaponSystem ws;
    ws.select_slot(0); // M16A2: 800 RPM -> cooldown = 60/800 = 0.075s (approx 2.25 retail ticks)

    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};
    // First trigger press fires round 1
    auto res1 = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
    ASSERT_TRUE(res1.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 29);

    // Immediate next tick (0.0333s < 0.075s cooldown): should not fire
    auto res2 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_FALSE(res2.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 29);

    // Next tick (total 0.0666s < 0.075s): still on cooldown
    auto res3 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_FALSE(res3.fired);

    // Next tick (total elapsed > 0.075s): should fire round 2
    auto res4 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_TRUE(res4.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 28);
}

TEST_CASE(WeaponSystem, BurstFireGating) {
    igi::gameplay::WeaponSystem ws;
    ws.select_slot(0); // M16A2 has burst_count = 3
    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};

    int shots_fired = 0;
    // Simulate holding trigger for 1 second (30 ticks)
    for (int tick = 0; tick < 30; ++tick) {
        const bool pressed = (tick == 0);
        auto res = ws.update(1.0f / 30.0f, true, pressed, eye, 0.0f, 0.0f);
        if (res.fired) {
            ++shots_fired;
        }
    }
    // Exactly 3 shots should have fired because burst limit is 3
    ASSERT_EQ(shots_fired, 3);
    ASSERT_EQ(ws.rounds_in_magazine(), 27);

    // Release trigger for 1 tick
    (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);

    // Press trigger again: second burst of 3 should fire
    shots_fired = 0;
    for (int tick = 0; tick < 30; ++tick) {
        const bool pressed = (tick == 0);
        auto res = ws.update(1.0f / 30.0f, true, pressed, eye, 0.0f, 0.0f);
        if (res.fired) {
            ++shots_fired;
        }
    }
    ASSERT_EQ(shots_fired, 3);
    ASSERT_EQ(ws.rounds_in_magazine(), 24);
}

TEST_CASE(WeaponSystem, RecoilDamping) {
    igi::gameplay::WeaponSystem ws;
    ws.select_slot(0);
    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};

    // Fire 1 shot to generate recoil
    auto res = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
    ASSERT_TRUE(res.fired);
    const float initial_pitch_recoil = ws.pitch_recoil();
    ASSERT_TRUE(initial_pitch_recoil > 0.0f);

    // Advance 10 ticks without firing; recoil must decay exponentially with 0.9 damping
    float prev_recoil = initial_pitch_recoil;
    for (int tick = 0; tick < 10; ++tick) {
        (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);
        const float cur_recoil = ws.pitch_recoil();
        ASSERT_TRUE(cur_recoil < prev_recoil);
        prev_recoil = cur_recoil;
    }
    // After 10 ticks at 0.9 damping, recoil should be approximately 0.9^10 ≈ 0.348 of initial
    ASSERT_NEAR(ws.pitch_recoil(), initial_pitch_recoil * std::pow(0.9f, 10.0f), 0.005f);
}

TEST_CASE(WeaponSystem, ReloadingAndDryFire) {
    igi::gameplay::WeaponSystem ws;
    ws.select_slot(4); // Dragunov: 10 rounds mag, 40 reserve
    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};

    // Fire all 10 rounds
    for (int r = 0; r < 10; ++r) {
        // Dragunov has burst_count = 1, so each shot requires a new press
        auto res = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
        ASSERT_TRUE(res.fired);
        // Wait out the cooldown (60/90 = 0.667s ≈ 20 ticks)
        for (int c = 0; c < 21; ++c) {
            (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);
        }
    }
    ASSERT_EQ(ws.rounds_in_magazine(), 0);

    // Attempting to fire with empty mag should return dry_fire = true and trigger reload
    auto dry_res = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
    ASSERT_FALSE(dry_res.fired);
    ASSERT_TRUE(dry_res.dry_fire);

    // During reload (2.2s ≈ 66 ticks), firing should return false
    for (int tick = 0; tick < 50; ++tick) {
        auto mid_res = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
        ASSERT_FALSE(mid_res.fired);
    }
    ASSERT_EQ(ws.rounds_in_magazine(), 0);

    // Complete the reload (remaining 20 ticks)
    for (int tick = 0; tick < 25; ++tick) {
        (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);
    }
    // Magazine should now be full (10 rounds) and reserve ammo decremented by 10 (40 -> 30)
    ASSERT_EQ(ws.rounds_in_magazine(), 10);
    ASSERT_EQ(ws.reserve_ammo(), 30);
}
