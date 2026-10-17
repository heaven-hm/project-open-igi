#pragma once

#include "igi/mission_scene.hpp"
#include "igi/retail_random_generator.hpp"

#include <array>

namespace igi::gameplay
{

struct WeaponConeSample final
{
    float radius{};
    float bearing{};
};

struct WeaponRecoilSample final
{
    float pitch{};
    float yaw{};
};

class WeaponAccuracy final
{
  public:
    static constexpr float degrees_to_radians = 0.017453292f;
    static constexpr float full_turn_radians = 6.2831855f;

    // Port helper for original IGI.EXE Gun_Fire at VA 0x00478BA0; not a separate retail entrypoint.
    [[nodiscard]] static float Weapon_ComputeAccuracyCone(
        const WeaponParameters& parameters, std::uint32_t rounds_this_burst) noexcept;
    // Port helper for original IGI.EXE Gun_Fire at VA 0x00478BA0; not a separate retail entrypoint.
    [[nodiscard]] static WeaponConeSample Weapon_SampleAccuracyCone(
        float cone_angle, const std::array<float, 4>& uniform_samples) noexcept;
    // Port helper for original IGI.EXE Gun_Fire at VA 0x00478BA0; not a separate retail entrypoint.
    [[nodiscard]] static Vector3 Weapon_SampleShotDirection(
        float cone_angle, Vector3 forward, Vector3 right, Vector3 up, RetailRandomGenerator& random) noexcept;
    // Port helper for original IGI.EXE Gun_FireMessage at VA 0x00479780; not a separate retail entrypoint.
    [[nodiscard]] static WeaponRecoilSample Weapon_SampleRecoil(
        const WeaponParameters& parameters, RetailRandomGenerator& random) noexcept;
    // Port support helper for the retail 30 Hz firing schedule. Its exact native address remains unverified.
    [[nodiscard]] static std::int32_t Weapon_ComputeCooldownTicks(std::int32_t rounds_per_minute);
};

} // namespace igi::gameplay
