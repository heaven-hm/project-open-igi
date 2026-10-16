#include "igi/player_ladder.hpp"

#include <algorithm>
#include <cmath>

namespace igi::gameplay {

LadderClimbLine::LadderClimbLine(Vector3 bottom, Vector3 top) noexcept
    : bottom_(bottom)
    , top_(top) {
    const float height = std::abs(top.z - bottom.z);
    step_count_ = std::max(1, static_cast<int>(height / kStepLengthUnits));
    top_step_ = std::max(0, step_count_ - kTopStepMargin);
}

Vector3 LadderClimbLine::position_at_step(int step) const noexcept {
    const int clamped = std::clamp(step, 0, step_count_);
    const float fraction = static_cast<float>(clamped) / static_cast<float>(step_count_);
    return {
        bottom_.x + (top_.x - bottom_.x) * fraction,
        bottom_.y + (top_.y - bottom_.y) * fraction,
        bottom_.z + (top_.z - bottom_.z) * fraction,
    };
}

LadderPlacement::LadderPlacement(
    Vector3 origin,
    Vector3 activation_top,
    Vector3 bottom,
    Vector3 top,
    Vector3 outward_normal) noexcept
    : origin_(origin)
    , activation_top_(activation_top)
    , bottom_(bottom)
    , top_(top)
    , outward_normal_(outward_normal)
    , climb_line_(bottom, top) {}

bool LadderPlacement::can_activate(
    Vector3 position,
    Vector3 look_direction) const noexcept {
    const float dx = position.x - origin_.x;
    const float dy = position.y - origin_.y;
    const float dist_xy = std::sqrt(dx * dx + dy * dy);
    if (dist_xy > kActivateRadiusUnits) {
        return false;
    }

    if (position.z < bottom_.z - 2048.0f || position.z > top_.z + 4096.0f) {
        return false;
    }

    // Must face roughly towards the ladder (facing against outward normal)
    const float dot = -(look_direction.x * outward_normal_.x +
                        look_direction.y * outward_normal_.y +
                        look_direction.z * outward_normal_.z);
    return dot >= kFacingCosine;
}

} // namespace igi::gameplay
