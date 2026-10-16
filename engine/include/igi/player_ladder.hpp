#pragma once

#include "igi/human_motion.hpp"

namespace igi::gameplay {

enum class LadderTraversalPhase {
    Inactive,
    Climbing,
    GettingOnTop,
    GettingOffTop,
    SlidingDown,
};

enum class LadderStepResult {
    Idle,
    SteppingUp,
    SteppingDown,
    ReachedTop,
    Dismounted,
    Sliding,
};

class LadderClimbLine final {
public:
    static constexpr float kStepLengthUnits = 1228.8f;
    static constexpr int kTopStepMargin = 6;

    LadderClimbLine(Vector3 bottom, Vector3 top) noexcept;

    [[nodiscard]] const Vector3& bottom() const noexcept { return bottom_; }
    [[nodiscard]] const Vector3& top() const noexcept { return top_; }
    [[nodiscard]] int step_count() const noexcept { return step_count_; }
    [[nodiscard]] int top_step() const noexcept { return top_step_; }
    [[nodiscard]] Vector3 position_at_step(int step) const noexcept;

private:
    Vector3 bottom_{};
    Vector3 top_{};
    int step_count_ = 1;
    int top_step_ = 0;
};

class LadderPlacement final {
public:
    // verified-reference: OpenIGI LadderPlacement constants recovered from
    // the vanilla ladder activation and mount handlers.
    static constexpr float kActivateRadiusUnits = 8192.0f;
    static constexpr float kActivateHalfWidthUnits = 2867.2f;
    static constexpr float kFacingCosine = 0.70710678f;
    static constexpr float kBottomStandoffUnits = 1515.52f;
    static constexpr float kBottomVerticalOffsetUnits = 3915.776f;
    static constexpr float kTopVerticalOffsetUnits = 3481.6f;
    static constexpr float kTopStandoffUnits = 1638.4f;

    LadderPlacement(
        Vector3 origin,
        Vector3 activation_top,
        Vector3 bottom,
        Vector3 top,
        Vector3 outward_normal) noexcept;

    [[nodiscard]] const Vector3& origin() const noexcept { return origin_; }
    [[nodiscard]] const Vector3& activation_top() const noexcept { return activation_top_; }
    [[nodiscard]] const Vector3& bottom() const noexcept { return bottom_; }
    [[nodiscard]] const Vector3& top() const noexcept { return top_; }
    [[nodiscard]] const Vector3& outward_normal() const noexcept { return outward_normal_; }
    [[nodiscard]] const LadderClimbLine& climb_line() const noexcept { return climb_line_; }

    [[nodiscard]] bool can_activate(
        Vector3 position,
        Vector3 look_direction) const noexcept;

private:
    Vector3 origin_{};
    Vector3 activation_top_{};
    Vector3 bottom_{};
    Vector3 top_{};
    Vector3 outward_normal_{0.0f, 1.0f, 0.0f};
    LadderClimbLine climb_line_;
};

} // namespace igi::gameplay
