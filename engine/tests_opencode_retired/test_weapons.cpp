#include "test_framework.hpp"
#include "igi/weapon_system.hpp"

#include <cmath>

TEST_CASE(WeaponSystem, DefaultLoadoutAndSlotSelection) {
    igi::gameplay::WeaponSystem ws;
    // Scripted retail values (weaponconfig.qvm): M16A2 20, Glock 17,
    // MP5SD 32, AK47 32, Dragunov 10.
    ASSERT_EQ(ws.current_stats().name, "M16A2");
    ASSERT_EQ(ws.rounds_in_magazine(), 20);
    ASSERT_EQ(ws.reserve_ammo(), 80);

    // Slot 1: Glock
    ws.select_slot(1);
    ASSERT_EQ(ws.current_stats().name, "Glock");
    ASSERT_EQ(ws.rounds_in_magazine(), 17);

    // Slot 2: MP5SD
    ws.select_slot(2);
    ASSERT_EQ(ws.current_stats().name, "MP5SD");
    ASSERT_EQ(ws.rounds_in_magazine(), 32);

    // Slot 3: AK47
    ws.select_slot(3);
    ASSERT_EQ(ws.current_stats().name, "AK47");
    ASSERT_EQ(ws.rounds_in_magazine(), 32);

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
    ws.select_slot(0); // M16A2: 500 RPM -> cooldown = 60/500 = 0.12s

    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};
    // First trigger press fires round 1
    auto res1 = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
    ASSERT_TRUE(res1.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 19);

    // Next three ticks (0.1s < 0.12s cooldown): should not fire
    auto res2 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_FALSE(res2.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 19);

    // Next two ticks (0.0667s, 0.1s < 0.12s): still on cooldown
    auto res3 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_FALSE(res3.fired);
    auto res4 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_FALSE(res4.fired);

    // Next tick (total elapsed 0.1333s > 0.12s): should fire round 2
    auto res5 = ws.update(1.0f / 30.0f, true, false, eye, 0.0f, 0.0f);
    ASSERT_TRUE(res5.fired);
    ASSERT_EQ(ws.rounds_in_magazine(), 18);
}

TEST_CASE(WeaponSystem, BurstFireGating) {
    igi::gameplay::WeaponSystem ws;
    ws.select_slot(0); // M16A2 scripted burst 20 = full magazine: full-auto
    const igi::gameplay::NavigationPosition eye{0.0, 0.0, 6553.6};

    int shots_fired = 0;
    // Holding trigger for 1 second (30 ticks) at 500 RPM fires every 4th tick.
    for (int tick = 0; tick < 30; ++tick) {
        const bool pressed = (tick == 0);
        auto res = ws.update(1.0f / 30.0f, true, pressed, eye, 0.0f, 0.0f);
        if (res.fired) {
            ++shots_fired;
        }
    }
    // Shots at ticks 0,4,...,28: 8 shots, magazine 20 -> 12.
    ASSERT_EQ(shots_fired, 8);
    ASSERT_EQ(ws.rounds_in_magazine(), 12);

    // Release trigger for 1 tick, then hold again: another 8 shots (12 -> 4).
    (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);
    shots_fired = 0;
    for (int tick = 0; tick < 30; ++tick) {
        const bool pressed = (tick == 0);
        auto res = ws.update(1.0f / 30.0f, true, pressed, eye, 0.0f, 0.0f);
        if (res.fired) {
            ++shots_fired;
        }
    }
    ASSERT_EQ(shots_fired, 8);
    ASSERT_EQ(ws.rounds_in_magazine(), 4);
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
        // Wait out the cooldown (60/50 = 1.2s = 36 ticks)
        for (int c = 0; c < 37; ++c) {
            (void)ws.update(1.0f / 30.0f, false, false, eye, 0.0f, 0.0f);
        }
    }
    ASSERT_EQ(ws.rounds_in_magazine(), 0);

    // Attempting to fire with empty mag should return dry_fire = true and trigger reload
    auto dry_res = ws.update(1.0f / 30.0f, true, true, eye, 0.0f, 0.0f);
    ASSERT_FALSE(dry_res.fired);
    ASSERT_TRUE(dry_res.dry_fire);

    // During reload (Dragunov scripted 2.0s = 60 ticks), firing should return false
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
