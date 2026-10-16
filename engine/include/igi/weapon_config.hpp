#pragma once

// Retail weapon configuration registry (data layer).
// Decoded from `WEAPONS/weaponconfig.qvm` (24 `Task_New(..., "WeaponConfig", ...)`
// records) via QVM decompilation. Field order follows the scripted record
// layout; names are documented from the task definition with open-igi's field
// table as the cross-check lead and retail bytes as ground truth. Records may
// span multiple decompiled lines; the loader accumulates to paren balance.
// Zero OS APIs.

#include <string>
#include <unordered_map>
#include <vector>

namespace igi::gameplay {

struct RetailWeaponConfig {
    std::string display_name;
    int weapon_id{-1};
    std::string script_id; // e.g. WEAPON_ID_M16A2
    int mass_grams{};
    int calibre_id{};
    float damage_factor{};
    float penetration_power{};
    float reload_seconds{};
    float muzzle_velocity{};
    int bullets_per_round{1};
    int rounds_per_minute{};
    int rounds_per_clip{};
    int max_rounds_per_burst{};
    float min_spread{};
    float max_spread{};
    float fixed_recoil_x{};
    float fixed_recoil_z{};
    float random_recoil_x{};
    float random_recoil_z{};
    std::string weapon_class; // e.g. <ASSAULT_RIFLE>
    float effective_range{};
    std::string gun_model;
    std::string casing_model;
};

class WeaponConfigRegistry final {
public:
    // Parses decompiled QSC text; returns WeaponConfig records accepted.
    std::size_t load_from_qsc_text(const std::string& qsc_text);

    void clear() noexcept { entries_by_script_id_.clear(); }

    [[nodiscard]] std::size_t weapon_count() const noexcept { return entries_by_script_id_.size(); }
    [[nodiscard]] const RetailWeaponConfig* find_by_script_id(
        const std::string& script_id) const noexcept;

private:
    std::unordered_map<std::string, RetailWeaponConfig> entries_by_script_id_;
};

} // namespace igi::gameplay
