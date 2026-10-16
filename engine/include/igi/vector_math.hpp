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
    if (std::abs(rot.x) < 0.0001f && std::abs(rot.y) < 0.0001f)
    {
        const float cosine = std::cos(rot.z);
        const float sine = std::sin(rot.z);
        return {cosine * v.x - sine * v.y, sine * v.x + cosine * v.y, v.z};
    }
    const float cx = std::cos(rot.x), sx = std::sin(rot.x);
    const float cy = std::cos(rot.y), sy = std::sin(rot.y);
    const float cz = std::cos(rot.z), sz = std::sin(rot.z);

    const Vector3 v1{v.x, cx * v.y - sx * v.z, sx * v.y + cx * v.z};
    const Vector3 v2{cy * v1.x + sy * v1.z, v1.y, -sy * v1.x + cy * v1.z};
    return {cz * v2.x - sz * v2.y, sz * v2.x + cz * v2.y, v2.z};
}

} // namespace igi::gameplay

