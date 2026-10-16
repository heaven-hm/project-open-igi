#pragma once

#include "igi/ai_navigation.hpp"
#include "igi/human_motion.hpp"
#include "igi/weapon_view_recoil.hpp"
#include "igi/weapon_view_sway.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace igi::gameplay {

// Per-weapon tuning. Field meanings follow the Gun/GunType layout recovered in
// WEAPON_FIRING_RESEARCH.md. Numeric values in default_loadout() are
// PROVISIONAL: the retail numbers live in script data not yet extracted.
struct WeaponStats {
    std::string name;
    int rounds_per_minute{700};
    int magazine_capacity{30};
    int burst_count{1000}; // >= magazine capacity means full-auto
    float range_meters{150.0f};
    float base_damage{34.0f};
    float spread_initial_deg{0.4f};
    float spread_sustained_deg{1.6f};
    float recoil_yaw_base_deg{0.0f};
    float recoil_pitch_base_deg{0.35f};
    float recoil_yaw_spread_deg{0.25f};
    float recoil_pitch_spread_deg{0.25f};
    float audible_range_units{102400.0f};
};

struct ShotRay {
    NavigationPosition origin{};
    Vector3 direction{1.0f, 0.0f, 0.0f}; // unit vector
    float range_units{};
    float damage{};
};

struct WeaponFireResult {
    bool fired{};
    bool dry_fire{};
    ShotRay shot{};
    float audible_range_units{};
};

class WeaponSystem final {
public:
    static constexpr float kUnitsPerMeter = 4096.0f;
    static constexpr float kRecoilDampingPerTick = 0.9f;

    WeaponSystem();

    [[nodiscard]] static std::vector<WeaponStats> default_loadout();

    void select_slot(std::size_t slot) noexcept;
    void cycle_next() noexcept;
    void cycle_prev() noexcept;
    [[nodiscard]] std::size_t current_slot() const noexcept { return current_slot_; }
    [[nodiscard]] const WeaponStats& current_stats() const noexcept;
    [[nodiscard]] int rounds_in_magazine() const noexcept;
    [[nodiscard]] int reserve_ammo() const noexcept;
    void add_reserve_ammo(int count) noexcept;
    void add_reserve_ammo(std::size_t slot, int count) noexcept;
    [[nodiscard]] float muzzle_flash() const noexcept { return muzzle_flash_; }
    [[nodiscard]] bool is_reloading() const noexcept { return reload_remaining_seconds_ > 0.0f; }

    void request_reload() noexcept;

    // Advances timers one fixed step and attempts to fire if requested.
    // `view_yaw` and `view_pitch` are the aim angles in radians (pitch up positive).
    [[nodiscard]] WeaponFireResult update(
        float delta_seconds,
        bool trigger_held,
        bool trigger_pressed,
        NavigationPosition eye,
        float view_yaw,
        float view_pitch) noexcept;

    // Accumulated recoil (radians), damped each step by 0.9, added to the camera.
    [[nodiscard]] float pitch_recoil() const noexcept { return pitch_recoil_; }
    [[nodiscard]] float yaw_recoil() const noexcept { return yaw_recoil_; }

    [[nodiscard]] float view_sway_pitch() const noexcept { return sway_.pitch_radians(); }
    [[nodiscard]] float view_sway_yaw() const noexcept { return sway_.yaw_radians(); }
    [[nodiscard]] float view_recoil_pitch() const noexcept { return view_recoil_.pitch_radians(); }
    [[nodiscard]] float view_recoil_yaw() const noexcept { return view_recoil_.yaw_radians(); }
    [[nodiscard]] const WeaponViewSway& view_sway() const noexcept { return sway_; }
    [[nodiscard]] const WeaponViewRecoil& view_recoil() const noexcept { return view_recoil_; }

private:
    struct Slot {
        WeaponStats stats;
        int rounds{};
        int reserve{};
    };

    [[nodiscard]] float next_random_unit() noexcept; // [0, 1)
    [[nodiscard]] float next_random_signed() noexcept; // [-1, 1)

    std::vector<Slot> slots_;
    std::size_t current_slot_{};
    WeaponViewSway sway_{};
    WeaponViewRecoil view_recoil_{};
    float cooldown_seconds_{};
    float reload_remaining_seconds_{};
    int burst_shots_fired_{};
    bool burst_trigger_released_{true};
    float burst_recycle_timer_{};
    float pitch_recoil_{};
    float yaw_recoil_{};
    float muzzle_flash_{};
    std::uint32_t rng_state_{0x9E3779B9u};
};

} // namespace igi::gameplay
