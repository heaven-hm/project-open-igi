#include "test_framework.hpp"

#include "igi/weapon_config.hpp"
#include "igi/weapon_system.hpp"
#include "parsers/qvm_decompiler.h"
#include "parsers/qvm_parser.h"

#include <filesystem>

using namespace igi::test;
using igi::gameplay::WeaponConfigRegistry;
using igi::gameplay::WeaponSystem;

// Native verification of open-igi's weapon field table against retail bytes:
// this decodes WEAPONS/weaponconfig.qvm independently (own decompiler +
// own record parser) and pins the scripted values the engine consumes.
TEST_CASE(WeaponConfig, RetailRosterDecodes) {
    const std::filesystem::path retail_qvm = "/Users/haseeb-mir/Downloads/IGI/WEAPONS/weaponconfig.qvm";
    if (!std::filesystem::exists(retail_qvm)) {
        return; // Skip if retail files are not installed
    }
    const QVMFile parsed = QVM_Parse(retail_qvm.string());
    ASSERT_TRUE(parsed.valid);
    WeaponConfigRegistry registry;
    const std::size_t accepted = registry.load_from_qsc_text(QVM_DecompileToString(parsed));
    ASSERT_TRUE(accepted >= 23u); // 24 records; knife (-1 clip) is not a firearm record
    ASSERT_TRUE(registry.weapon_count() >= 23u);

    const auto* m16 = registry.find_by_script_id("WEAPON_ID_M16A2");
    ASSERT_TRUE(m16 != nullptr);
    ASSERT_EQ(m16->rounds_per_minute, 500);
    ASSERT_EQ(m16->rounds_per_clip, 20);
    ASSERT_EQ(m16->max_rounds_per_burst, 20);
    ASSERT_NEAR(m16->reload_seconds, 2.0, 1.0e-6);
    ASSERT_NEAR(m16->effective_range, 300.0, 1.0e-6);
    ASSERT_NEAR(m16->damage_factor, 0.3, 1.0e-6);
    ASSERT_TRUE(m16->gun_model == "108_01_1");

    const auto* glock = registry.find_by_script_id("WEAPON_ID_GLOCK");
    ASSERT_TRUE(glock != nullptr);
    ASSERT_EQ(glock->rounds_per_minute, 400);
    ASSERT_EQ(glock->rounds_per_clip, 17);
    ASSERT_EQ(glock->max_rounds_per_burst, 1);
    ASSERT_NEAR(glock->effective_range, 60.0, 1.0e-6);

    const auto* mp5 = registry.find_by_script_id("WEAPON_ID_MP5SD");
    ASSERT_TRUE(mp5 != nullptr);
    ASSERT_EQ(mp5->rounds_per_minute, 700);
    ASSERT_EQ(mp5->rounds_per_clip, 32);
    ASSERT_NEAR(mp5->reload_seconds, 1.8, 1.0e-6);
    ASSERT_NEAR(mp5->effective_range, 100.0, 1.0e-6);

    const auto* ak = registry.find_by_script_id("WEAPON_ID_AK47");
    ASSERT_TRUE(ak != nullptr);
    ASSERT_EQ(ak->rounds_per_minute, 650);
    ASSERT_EQ(ak->rounds_per_clip, 32);
    ASSERT_NEAR(ak->effective_range, 300.0, 1.0e-6);

    const auto* dragunov = registry.find_by_script_id("WEAPON_ID_DRAGUNOV");
    ASSERT_TRUE(dragunov != nullptr);
    ASSERT_EQ(dragunov->rounds_per_minute, 50);
    ASSERT_EQ(dragunov->rounds_per_clip, 10);
    ASSERT_NEAR(dragunov->effective_range, 1000.0, 1.0e-6);
    ASSERT_TRUE(registry.find_by_script_id("WEAPON_ID_NOPE") == nullptr);
}

// The playable loadout must equal the scripted retail values above.
TEST_CASE(WeaponConfig, LoadoutMatchesScriptedValues) {
    WeaponSystem weapons;
    ASSERT_EQ(weapons.current_stats().rounds_per_minute, 500); // M16A2
    ASSERT_EQ(weapons.current_stats().magazine_capacity, 20);
    ASSERT_EQ(weapons.current_stats().burst_count, 20);
    ASSERT_NEAR(weapons.current_stats().reload_seconds, 2.0, 1.0e-6);
    weapons.select_slot(1); // Glock
    ASSERT_EQ(weapons.current_stats().rounds_per_minute, 400);
    ASSERT_EQ(weapons.current_stats().magazine_capacity, 17);
    weapons.select_slot(2); // MP5SD
    ASSERT_EQ(weapons.current_stats().rounds_per_minute, 700);
    ASSERT_EQ(weapons.current_stats().magazine_capacity, 32);
    weapons.select_slot(3); // AK47
    ASSERT_EQ(weapons.current_stats().rounds_per_minute, 650);
    weapons.select_slot(4); // Dragunov
    ASSERT_EQ(weapons.current_stats().rounds_per_minute, 50);
    ASSERT_NEAR(weapons.current_stats().range_meters, 1000.0f, 1.0e-6);
}
