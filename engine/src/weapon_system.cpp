#include "igi/weapon_system.hpp"
#include "igi/logger.hpp"

#include <algorithm>
#include <cmath>

namespace igi::gameplay {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kDegToRad = kPi / 180.0f;
constexpr float kReloadSeconds = 2.2f;
// Retail ticks run at 30 Hz; the recoil damping factor applies per retail tick.
constexpr float kRetailTickSeconds = 1.0f / 30.0f;

[[nodiscard]] Vector3 normalize(Vector3 v) noexcept {
    const float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 1.0e-9f) {
        return {1.0f, 0.0f, 0.0f};
    }
    return {v.x / len, v.y / len, v.z / len};
}

} // namespace

WeaponSystem::WeaponSystem() {
    for (const WeaponStats& stats : default_loadout()) {
        Slot slot;
        slot.stats = stats;
        slot.rounds = stats.magazine_capacity;
        slot.reserve = stats.magazine_capacity * 4;
        slots_.push_back(std::move(slot));
    }
}

std::vector<WeaponStats> WeaponSystem::default_loadout() {
    std::vector<WeaponStats> loadout;
    WeaponStats m16;
    m16.name = "M16A2";
    m16.rounds_per_minute = 800;
    m16.burst_count = 3;
    loadout.push_back(m16);

    WeaponStats glock;
    glock.name = "Glock";
    glock.rounds_per_minute = 360;
    glock.magazine_capacity = 15;
    glock.burst_count = 1;
    glock.range_meters = 60.0f;
    glock.base_damage = 28.0f;
    glock.spread_initial_deg = 0.6f;
    glock.spread_sustained_deg = 1.0f;
    glock.recoil_pitch_base_deg = 0.8f;
    loadout.push_back(glock);

    WeaponStats mp5;
    mp5.name = "MP5SD";
    mp5.rounds_per_minute = 800;
    mp5.range_meters = 70.0f;
    mp5.base_damage = 24.0f;
    mp5.spread_sustained_deg = 2.0f;
    mp5.audible_range_units = 20480.0f;
    loadout.push_back(mp5);

    WeaponStats ak47;
    ak47.name = "AK47";
    ak47.rounds_per_minute = 600;
    ak47.base_damage = 40.0f;
    ak47.recoil_pitch_base_deg = 0.55f;
    loadout.push_back(ak47);

    WeaponStats dragunov;
    dragunov.name = "Dragunov";
    dragunov.rounds_per_minute = 90;
    dragunov.magazine_capacity = 10;
    dragunov.burst_count = 1;
    dragunov.range_meters = 400.0f;
    dragunov.base_damage = 100.0f;
    dragunov.spread_initial_deg = 0.05f;
    dragunov.spread_sustained_deg = 0.1f;
    dragunov.recoil_pitch_base_deg = 1.6f;
    loadout.push_back(dragunov);
    return loadout;
}

void WeaponSystem::select_slot(std::size_t slot) noexcept {
    if (slot < slots_.size() && slot != current_slot_) {
        current_slot_ = slot;
        cooldown_seconds_ = 0.0f;
        reload_remaining_seconds_ = 0.0f;
        burst_shots_fired_ = 0;
        sway_.lower();
        sway_.raise();
        IGI_LOG_DEBUG("Weapon", "Selected weapon slot " << slot << ": " << slots_[slot].stats.name);
    }
}

const WeaponStats& WeaponSystem::current_stats() const noexcept {
    return slots_[current_slot_].stats;
}

int WeaponSystem::rounds_in_magazine() const noexcept {
    return slots_[current_slot_].rounds;
}

int WeaponSystem::reserve_ammo() const noexcept {
    return slots_[current_slot_].reserve;
}

void WeaponSystem::request_reload() noexcept {
    const Slot& slot = slots_[current_slot_];
    if (reload_remaining_seconds_ > 0.0f || slot.reserve <= 0 ||
        slot.rounds >= slot.stats.magazine_capacity) {
        return;
    }
    reload_remaining_seconds_ = kReloadSeconds;
    IGI_LOG_DEBUG("Weapon", "Reload started for " << slot.stats.name << " (" << slot.rounds << "/" << slot.stats.magazine_capacity << ")");
}

float WeaponSystem::next_random_unit() noexcept {
    rng_state_ ^= rng_state_ << 13;
    rng_state_ ^= rng_state_ >> 17;
    rng_state_ ^= rng_state_ << 5;
    return static_cast<float>(rng_state_ & 0xFFFFFFu) / 16777216.0f;
}

float WeaponSystem::next_random_signed() noexcept {
    return next_random_unit() * 2.0f - 1.0f;
}

WeaponFireResult WeaponSystem::update(
    float delta_seconds,
    bool trigger_held,
    bool trigger_pressed,
    NavigationPosition eye,
    float view_yaw,
    float view_pitch) noexcept {
    WeaponFireResult result{};
    Slot& slot = slots_[current_slot_];
    const WeaponStats& stats = slot.stats;

    // Recoil damping: factor 0.9 per retail 30 Hz tick.
    const float damp = std::pow(kRecoilDampingPerTick, delta_seconds / kRetailTickSeconds);
    pitch_recoil_ *= damp;
    yaw_recoil_ *= damp;
    sway_.advance();
    view_recoil_.advance();
    muzzle_flash_ = std::max(0.0f, muzzle_flash_ - delta_seconds * 12.0f);
    cooldown_seconds_ = std::max(0.0f, cooldown_seconds_ - delta_seconds);

    if (reload_remaining_seconds_ > 0.0f) {
        reload_remaining_seconds_ -= delta_seconds;
        if (reload_remaining_seconds_ <= 0.0f) {
            reload_remaining_seconds_ = 0.0f;
            const int needed = stats.magazine_capacity - slot.rounds;
            const int moved = std::min(needed, slot.reserve);
            slot.rounds += moved;
            slot.reserve -= moved;
        }
        return result;
    }

    if (!trigger_held) {
        burst_shots_fired_ = 0;
        burst_trigger_released_ = true;
    }

    const bool wants_fire = trigger_held && (trigger_pressed || burst_shots_fired_ > 0 ||
                                            stats.burst_count >= stats.magazine_capacity);
    const bool burst_exhausted =
        stats.burst_count < stats.magazine_capacity && burst_shots_fired_ >= stats.burst_count;
    if (!wants_fire || burst_exhausted || cooldown_seconds_ > 0.0f) {
        return result;
    }

    if (slot.rounds <= 0) {
        if (trigger_pressed) {
            result.dry_fire = true;
            request_reload();
        }
        return result;
    }

    // Cooldown = (60 / rpm) seconds (retail: (60 / rpm) * 30 ticks).
    cooldown_seconds_ = 60.0f / static_cast<float>(std::max(1, stats.rounds_per_minute));
    --slot.rounds;
    ++burst_shots_fired_;

    // Spread cone: cubic falloff u1*u2*u3 * half-angle, uniform azimuth.
    const float cone_deg = burst_shots_fired_ <= 3 ? stats.spread_initial_deg
                                                   : stats.spread_sustained_deg;
    const float theta = next_random_unit() * next_random_unit() * next_random_unit() *
                        cone_deg * kDegToRad;
    const float phi = next_random_unit() * 2.0f * kPi;

    const float cy = std::cos(view_yaw);
    const float sy = std::sin(view_yaw);
    const float cp = std::cos(view_pitch);
    const float sp = std::sin(view_pitch);
    const Vector3 forward{cp * cy, cp * sy, sp};
    const Vector3 right{sy, -cy, 0.0f};
    const Vector3 up{-sp * cy, -sp * sy, cp};
    const float tan_theta = std::tan(theta);
    const float ox = std::cos(phi) * tan_theta;
    const float oy = std::sin(phi) * tan_theta;
    const Vector3 dir = normalize({
        forward.x + right.x * ox + up.x * oy,
        forward.y + right.y * ox + up.y * oy,
        forward.z + right.z * ox + up.z * oy,
    });

    // Recoil kick: base + unit-disk sample scaled by spread (degrees -> radians).
    float dx = 0.0f;
    float dy = 0.0f;
    do {
        dx = next_random_signed();
        dy = next_random_signed();
    } while (dx * dx + dy * dy > 1.0f);
    yaw_recoil_ += (stats.recoil_yaw_base_deg + dx * stats.recoil_yaw_spread_deg) * kDegToRad;
    pitch_recoil_ += (stats.recoil_pitch_base_deg + dy * stats.recoil_pitch_spread_deg) * kDegToRad;
    view_recoil_.trigger_degrees(stats.recoil_pitch_base_deg, stats.recoil_yaw_base_deg);
    muzzle_flash_ = 1.0f;

    result.fired = true;
    result.shot.origin = eye;
    result.shot.direction = dir;
    result.shot.range_units = stats.range_meters * kUnitsPerMeter;
    result.shot.damage = stats.base_damage;
    result.audible_range_units = stats.audible_range_units;
    return result;
}

} // namespace igi::gameplay
