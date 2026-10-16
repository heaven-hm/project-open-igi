#pragma once

#include "igi/human_motion.hpp"

#include <cmath>

namespace igi::gameplay
{

inline Vector3 operator+(Vector3 first, Vector3 second) noexcept
{
    return {first.x + second.x, first.y + second.y, first.z + second.z};
}

inline Vector3 operator-(Vector3 first, Vector3 second) noexcept
{
    return {first.x - second.x, first.y - second.y, first.z - second.z};
}

inline Vector3 operator*(Vector3 vector, float factor) noexcept
{
    return {vector.x * factor, vector.y * factor, vector.z * factor};
}

inline float dot(Vector3 first, Vector3 second) noexcept
{
    return first.x * second.x + first.y * second.y + first.z * second.z;
}

inline Vector3 cross(Vector3 first, Vector3 second) noexcept
{
    return {first.y * second.z - first.z * second.y,
            first.z * second.x - first.x * second.z,
            first.x * second.y - first.y * second.x};
}

inline float length(Vector3 vector) noexcept
{
    return std::sqrt(dot(vector, vector));
}

inline Vector3 normalized(Vector3 vector) noexcept
{
    const float magnitude = length(vector);
    return magnitude > 0.000001f ? vector * (1.0f / magnitude) : Vector3{};
}

inline bool finite(Vector3 vector) noexcept
{
    return std::isfinite(vector.x) && std::isfinite(vector.y) && std::isfinite(vector.z);
}

inline Vector3 rotate_euler(Vector3 v, Vector3 rot) noexcept
{
    if (rot.x == 0.0f && rot.y == 0.0f)
    {
        const float cosine = std::cos(rot.z);
        const float sine = std::sin(rot.z);
        return {cosine * v.x + sine * v.y, -sine * v.x + cosine * v.y, v.z};
    }
    const float cx = std::cos(rot.x), sx = std::sin(rot.x);
    const float cy = std::cos(rot.y), sy = std::sin(rot.y);
    const float cz = std::cos(rot.z), sz = std::sin(rot.z);

    const Vector3 after_z{cz * v.x + sz * v.y, -sz * v.x + cz * v.y, v.z};
    const Vector3 after_y{cy * after_z.x - sy * after_z.z, after_z.y, sy * after_z.x + cy * after_z.z};
    return {after_y.x, cx * after_y.y + sx * after_y.z, -sx * after_y.y + cx * after_y.z};
}

} // namespace igi::gameplay
