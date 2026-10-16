#pragma once

// Portable skeletal pose sampling (zero OS APIs).
// Quaternion slerp between retail IFF key rotations plus hierarchical
// composition of rest offsets, replacing the earlier cyclic sine
// approximation. Math mirrors the retail-correct evaluation order:
// sample local rotation -> rotate rest offset by parent world rotation ->
// accumulate world positions down the hierarchy.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace igi::gameplay {

struct PoseQuaternion {
    float x{};
    float y{};
    float z{};
    float w{1.0f};
};

struct PoseVector {
    float x{};
    float y{};
    float z{};
};

struct PoseKeyframe {
    float time{};
    PoseQuaternion rotation{};
};

[[nodiscard]] inline PoseQuaternion normalize_quaternion(PoseQuaternion q) noexcept {
    const float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (!(len_sq > 1.0e-12f) || !std::isfinite(len_sq)) {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }
    const float inv = 1.0f / std::sqrt(len_sq);
    return {q.x * inv, q.y * inv, q.z * inv, q.w * inv};
}

[[nodiscard]] inline PoseQuaternion multiply_quaternions(
    PoseQuaternion a, PoseQuaternion b) noexcept {
    return {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
    };
}

[[nodiscard]] inline PoseVector rotate_vector(PoseQuaternion q, PoseVector v) noexcept {
    const PoseQuaternion unit = normalize_quaternion(q);
    const PoseQuaternion p{v.x, v.y, v.z, 0.0f};
    const PoseQuaternion conj{-unit.x, -unit.y, -unit.z, unit.w};
    const PoseQuaternion spun = multiply_quaternions(multiply_quaternions(unit, p), conj);
    return {spun.x, spun.y, spun.z};
}

[[nodiscard]] inline PoseQuaternion slerp_quaternions(
    PoseQuaternion first, PoseQuaternion second, float fraction) noexcept {
    PoseQuaternion a = normalize_quaternion(first);
    PoseQuaternion b = normalize_quaternion(second);
    float cosine = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    if (cosine < 0.0f) {
        cosine = -cosine;
        b = {-b.x, -b.y, -b.z, -b.w};
    }
    const float t = std::clamp(fraction, 0.0f, 1.0f);
    if (cosine > 0.9995f) {
        return normalize_quaternion({
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t,
        });
    }
    const float angle = std::acos(std::clamp(cosine, -1.0f, 1.0f));
    const float inv_sin = 1.0f / std::sin(angle);
    const float wa = std::sin((1.0f - t) * angle) * inv_sin;
    const float wb = std::sin(t * angle) * inv_sin;
    return {
        a.x * wa + b.x * wb,
        a.y * wa + b.y * wb,
        a.z * wa + b.z * wb,
        a.w * wa + b.w * wb,
    };
}

[[nodiscard]] inline PoseQuaternion sample_rotation_track(
    const std::vector<PoseKeyframe>& keys, float time_seconds, float duration_seconds) noexcept {
    if (keys.empty()) {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }
    if (keys.size() == 1 || duration_seconds <= 0.0f) {
        return normalize_quaternion(keys.front().rotation);
    }
    float wrapped = std::fmod(time_seconds, duration_seconds);
    if (wrapped < 0.0f) {
        wrapped += duration_seconds;
    }
    if (wrapped <= keys.front().time) {
        return normalize_quaternion(keys.front().rotation);
    }
    for (std::size_t i = 1; i < keys.size(); ++i) {
        if (wrapped <= keys[i].time) {
            const float span = keys[i].time - keys[i - 1].time;
            const float fraction = span > 0.0f ? (wrapped - keys[i - 1].time) / span : 0.0f;
            return slerp_quaternions(keys[i - 1].rotation, keys[i].rotation, fraction);
        }
    }
    // Past the last key: hold it (non-cyclic tail).
    return normalize_quaternion(keys.back().rotation);
}

// Composes world-space bone positions from hierarchy data. `parents[i]` is
// the parent bone index or negative; `rest_offsets[i]` is the bone's rest
// offset relative to its parent; `local_rotations[i]` is the sampled local
// rotation. Invalid parents and cycles resolve to a detached bone (no
// propagation past the break) instead of recursing forever.
inline void compose_bone_world_positions(
    const std::vector<int>& parents,
    const std::vector<PoseVector>& rest_offsets,
    const std::vector<PoseQuaternion>& local_rotations,
    std::vector<PoseVector>& out_world_positions) noexcept {
    const std::size_t count = parents.size();
    out_world_positions.assign(count, {0.0f, 0.0f, 0.0f});
    if (count == 0) {
        return;
    }
    std::vector<PoseQuaternion> world_rotations(count, {0.0f, 0.0f, 0.0f, 1.0f});
    std::vector<char> resolved(count, 0);
    for (std::size_t i = 0; i < count; ++i) {
        std::size_t cursor = i;
        std::size_t guard = 0;
        // Resolve the ancestor chain iteratively (cycle-safe).
        std::size_t chain[512];
        std::size_t chain_len = 0;
        while (cursor < count && !resolved[cursor] && guard <= count) {
            chain[chain_len++] = cursor;
            const int parent = (cursor < parents.size()) ? parents[cursor] : -1;
            if (parent < 0 || static_cast<std::size_t>(parent) >= count || chain_len >= 512) {
                break;
            }
            cursor = static_cast<std::size_t>(parent);
            ++guard;
        }
        while (chain_len > 0) {
            const std::size_t bone = chain[--chain_len];
            const int parent = (bone < parents.size()) ? parents[bone] : -1;
            const PoseVector rest = (bone < rest_offsets.size()) ? rest_offsets[bone] : PoseVector{};
            const PoseQuaternion local = (bone < local_rotations.size())
                ? normalize_quaternion(local_rotations[bone])
                : PoseQuaternion{0.0f, 0.0f, 0.0f, 1.0f};
            if (parent >= 0 && static_cast<std::size_t>(parent) < count && resolved[parent]) {
                world_rotations[bone] = multiply_quaternions(world_rotations[parent], local);
                const PoseVector turned = rotate_vector(world_rotations[parent], rest);
                const PoseVector base = out_world_positions[parent];
                out_world_positions[bone] = {base.x + turned.x, base.y + turned.y, base.z + turned.z};
            } else {
                world_rotations[bone] = local;
                out_world_positions[bone] = rest;
            }
            resolved[bone] = 1;
        }
    }
}

} // namespace igi::gameplay
