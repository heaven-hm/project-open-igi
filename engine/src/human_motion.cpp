#include "igi/human_motion.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace igi::gameplay
{

HumanFallImpact HumanFallDamage_Calculate(float vertical_velocity_units_per_tick, float maximum_health)
{
    if (!std::isfinite(vertical_velocity_units_per_tick) || !std::isfinite(maximum_health) ||
        maximum_health <= 0.0F)
    {
        throw std::invalid_argument("Fall damage requires finite velocity and positive maximum health");
    }

    constexpr float engine_units_per_metre = 4096.0F;
    constexpr float safe_impact_speed_metres_per_second = 14.0F;
    constexpr float lethal_speed_range_metres_per_second = 13.0F;
    constexpr float maximum_view_kick_units = 1024.0F;
    const float speed_metres_per_second =
        std::abs(vertical_velocity_units_per_tick) * (1.0F / engine_units_per_metre) * 30.0F;
    const float damage = std::max(0.0F, speed_metres_per_second - safe_impact_speed_metres_per_second) *
                         (1.0F / lethal_speed_range_metres_per_second) * maximum_health;
    const float view_kick_units =
        -std::clamp(speed_metres_per_second * 0.125F, 0.0F, 1.0F) * maximum_view_kick_units;
    return {speed_metres_per_second, damage, view_kick_units};
}

Vector3 HumanMotion::launch_velocity(Vector3 previous_animation_movement,
                                     Vector3 inherited_velocity,
                                     float horizontal_speed,
                                     float vertical_speed) noexcept
{
    long double horizontal_motion = previous_animation_movement.x;
    long double forward_motion = previous_animation_movement.y;
    if (horizontal_motion != 0 || forward_motion != 0)
    {
        const long double inverse_length =
            1.0L / std::sqrt(horizontal_motion * horizontal_motion + forward_motion * forward_motion);
        const float normalized_horizontal = static_cast<float>(horizontal_motion * inverse_length);
        horizontal_motion = static_cast<float>(static_cast<long double>(horizontal_speed) *
                                                normalized_horizontal);
        forward_motion = forward_motion * inverse_length * horizontal_speed;
    }
    return {
        static_cast<float>(horizontal_motion + inherited_velocity.x),
        static_cast<float>(forward_motion + inherited_velocity.y),
        vertical_speed + inherited_velocity.z,
    };
}

Vector3 HumanMotion::integrate_airborne(Vector3 velocity,
                                        bool axis_negative,
                                        bool axis_positive,
                                        bool side_positive,
                                        bool side_negative,
                                        const Basis3x3& basis,
                                        float air_control_per_tick) noexcept
{
    float first_axis = axis_negative ? -air_control_per_tick : 0.0f;
    if (axis_positive)
        first_axis = air_control_per_tick + first_axis;
    float second_axis = side_positive ? air_control_per_tick : 0.0f;
    if (side_negative)
        second_axis -= air_control_per_tick;

    const float next_z = velocity.z - gravity_per_tick;
    velocity.x = first_axis * basis.m11 + second_axis * basis.m12 + velocity.x;
    velocity.y = first_axis * basis.m21 + second_axis * basis.m22 + velocity.y;
    velocity.z = next_z + first_axis * basis.m31 + second_axis * basis.m32;
    return velocity;
}

Vector3 HumanMotion::integrate_ladder_slide(Vector3 velocity) noexcept
{
    velocity.x *= ladder_drag_per_tick;
    velocity.y *= ladder_drag_per_tick;
    velocity.z = (velocity.z - ladder_gravity_per_tick) * ladder_drag_per_tick;
    return velocity;
}

Vector3 HumanMotion::apply_root_motion(Vector3 delta,
                                       const Basis3x3& basis,
                                       float translation_scale,
                                       bool suppress_scale) noexcept
{
    if (!suppress_scale)
    {
        delta.x *= translation_scale;
        delta.y *= translation_scale;
        delta.z *= translation_scale;
    }
    return {
        basis.m11 * delta.x + basis.m12 * delta.y + basis.m13 * delta.z,
        basis.m21 * delta.x + basis.m22 * delta.y + basis.m23 * delta.z,
        basis.m31 * delta.x + basis.m32 * delta.y + basis.m33 * delta.z,
    };
}

} // namespace igi::gameplay
