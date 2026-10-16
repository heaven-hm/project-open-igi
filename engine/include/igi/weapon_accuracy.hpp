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

    [[nodiscard]] static float cone_angle(const WeaponParameters& parameters,
                                          std::uint32_t rounds_this_burst) noexcept;
    [[nodiscard]] static WeaponConeSample sample_cone(float cone_angle,
                                                      const std::array<float, 4>& uniform_samples) noexcept;
    [[nodiscard]] static Vector3 sample_direction(
        float cone_angle, Vector3 forward, Vector3 right, Vector3 up, RetailRandomGenerator& random) noexcept;
    [[nodiscard]] static WeaponRecoilSample sample_recoil(const WeaponParameters& parameters,
                                                          RetailRandomGenerator& random) noexcept;
};

} // namespace igi::gameplay
