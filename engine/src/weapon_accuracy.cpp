#include "igi/weapon_accuracy.hpp"

#include <cmath>

namespace igi::gameplay
{

float WeaponAccuracy::cone_angle(const WeaponParameters& parameters, std::uint32_t rounds_this_burst) noexcept
{
    const float degrees =
        rounds_this_burst > 3 ? parameters.maximum_spread_degrees : parameters.minimum_spread_degrees;
    const float radians = degrees * degrees_to_radians;
    return radians < 0 ? -radians : radians;
}

WeaponConeSample WeaponAccuracy::sample_cone(float cone_angle,
                                             const std::array<float, 4>& uniform_samples) noexcept
{
    float radius = 1;
    for (std::size_t sample_index = 0; sample_index < 3; ++sample_index)
    {
        radius *= uniform_samples[sample_index];
    }
    return {radius * cone_angle, uniform_samples[3] * full_turn_radians};
}

Vector3 WeaponAccuracy::sample_direction(
    float cone_angle, Vector3 forward, Vector3 right, Vector3 up, RetailRandomGenerator& random) noexcept
{
    if (cone_angle == 0)
    {
        return forward;
    }
    const std::array<float, 4> samples{
        random.next_single(), random.next_single(), random.next_single(), random.next_single()};
    const auto cone = sample_cone(cone_angle, samples);
    const float sine = std::sin(cone.radius);
    const Vector3 lateral = right * (std::cos(cone.bearing) * sine) + up * (std::sin(cone.bearing) * sine);
    return normalized(forward * std::cos(cone.radius) + lateral);
}

WeaponRecoilSample WeaponAccuracy::sample_recoil(const WeaponParameters& parameters,
                                                 RetailRandomGenerator& random) noexcept
{
    long double pitch_sample;
    long double yaw_sample;
    do
    {
        const long double first = random.next_single();
        pitch_sample = (first - 0.5L) + (first - 0.5L);
        const long double second = random.next_single();
        yaw_sample = (second - 0.5L) + (second - 0.5L);
    } while (std::sqrt(yaw_sample * yaw_sample + pitch_sample * pitch_sample) > 1.0L);
    return {
        static_cast<float>(parameters.random_recoil_pitch_degrees *
                               static_cast<long double>(degrees_to_radians) * pitch_sample +
                           parameters.fixed_recoil_pitch_degrees *
                               static_cast<long double>(degrees_to_radians)),
        static_cast<float>(
            parameters.random_recoil_yaw_degrees * static_cast<long double>(degrees_to_radians) * yaw_sample +
            parameters.fixed_recoil_yaw_degrees * static_cast<long double>(degrees_to_radians)),
    };
}

} // namespace igi::gameplay
