#include "test_framework.hpp"

#include "igi/skeletal_pose.hpp"

#include <cmath>

using namespace igi::test;
using igi::gameplay::compose_bone_world_positions;
using igi::gameplay::normalize_quaternion;
using igi::gameplay::PoseKeyframe;
using igi::gameplay::PoseQuaternion;
using igi::gameplay::PoseVector;
using igi::gameplay::rotate_vector;
using igi::gameplay::sample_rotation_track;
using igi::gameplay::slerp_quaternions;

namespace {

constexpr float kHalfSqrt2 = 0.70710678f;

} // namespace

TEST_CASE(SkeletalPose, SlerpMidpointOfQuarterTurn) {
    // Identity -> 90 degrees about Z. Midpoint must be 45 degrees about Z.
    const PoseQuaternion identity{0.0f, 0.0f, 0.0f, 1.0f};
    const PoseQuaternion quarter{0.0f, 0.0f, kHalfSqrt2, kHalfSqrt2};
    const PoseQuaternion mid = slerp_quaternions(identity, quarter, 0.5f);
    const float expected = 0.38268343f; // sin(22.5 deg)
    ASSERT_NEAR(static_cast<double>(mid.z), expected, 1.0e-5);
    ASSERT_NEAR(static_cast<double>(mid.w), 0.92387953, 1.0e-5);
    ASSERT_NEAR(static_cast<double>(mid.x), 0.0, 1.0e-6);
    ASSERT_NEAR(static_cast<double>(mid.y), 0.0, 1.0e-6);
}

TEST_CASE(SkeletalPose, SlerpEndpointsAreExact) {
    const PoseQuaternion a{0.1f, 0.2f, 0.3f, 0.9f};
    const PoseQuaternion b{0.4f, -0.1f, 0.2f, 0.85f};
    const PoseQuaternion na = normalize_quaternion(a);
    const PoseQuaternion nb = normalize_quaternion(b);
    const PoseQuaternion at0 = slerp_quaternions(a, b, 0.0f);
    const PoseQuaternion at1 = slerp_quaternions(a, b, 1.0f);
    ASSERT_NEAR(static_cast<double>(at0.x), na.x, 1.0e-6);
    ASSERT_NEAR(static_cast<double>(at1.w), nb.w, 1.0e-6);
}

TEST_CASE(SkeletalPose, RotationTurnsVector) {
    const PoseQuaternion quarter{0.0f, 0.0f, kHalfSqrt2, kHalfSqrt2};
    const PoseVector turned = rotate_vector(quarter, {1.0f, 0.0f, 0.0f});
    ASSERT_NEAR(static_cast<double>(turned.x), 0.0, 1.0e-5);
    ASSERT_NEAR(static_cast<double>(turned.y), 1.0, 1.0e-5);
    ASSERT_NEAR(static_cast<double>(turned.z), 0.0, 1.0e-5);
}

TEST_CASE(SkeletalPose, TrackSamplesBetweenKeys) {
    const std::vector<PoseKeyframe> keys{
        {0.0f, {0.0f, 0.0f, 0.0f, 1.0f}},
        {1.0f, {0.0f, 0.0f, kHalfSqrt2, kHalfSqrt2}},
    };
    const PoseQuaternion half = sample_rotation_track(keys, 0.5f, 1.0f);
    ASSERT_NEAR(static_cast<double>(half.z), 0.38268343, 1.0e-5);
    // Negative time wraps cyclically: -0.25 -> 0.75 -> 67.5 deg about Z.
    const PoseQuaternion wrapped_neg = sample_rotation_track(keys, -0.25f, 1.0f);
    ASSERT_NEAR(static_cast<double>(wrapped_neg.z), 0.55557024, 1.0e-5);
    ASSERT_NEAR(static_cast<double>(wrapped_neg.w), 0.83146961, 1.0e-5);
    const PoseQuaternion after = sample_rotation_track(keys, 5.0f, 1.0f);
    ASSERT_NEAR(static_cast<double>(after.w), 1.0, 1.0e-5); // wraps to t=0
}

TEST_CASE(SkeletalPose, EmptyTrackIsIdentity) {
    const PoseQuaternion q = sample_rotation_track({}, 0.3f, 1.0f);
    ASSERT_NEAR(static_cast<double>(q.w), 1.0, 1.0e-9);
}

TEST_CASE(SkeletalPose, HierarchyComposesRestOffsets) {
    // Root at origin, child rest offset (10,0,0); identity rotations.
    const std::vector<int> parents{-1, 0};
    const std::vector<PoseVector> rests{{0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}};
    const std::vector<PoseQuaternion> local{
        {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
    std::vector<PoseVector> world;
    compose_bone_world_positions(parents, rests, local, world);
    ASSERT_EQ(world.size(), 2u);
    ASSERT_NEAR(static_cast<double>(world[1].x), 10.0, 1.0e-5);
}

TEST_CASE(SkeletalPose, ParentRotationMovesChild) {
    // Root rotated 90 deg about Z; child rest offset (10,0,0) lands at (0,10,0).
    const std::vector<int> parents{-1, 0};
    const std::vector<PoseVector> rests{{0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}};
    const std::vector<PoseQuaternion> local{
        {0.0f, 0.0f, kHalfSqrt2, kHalfSqrt2}, {0.0f, 0.0f, 0.0f, 1.0f}};
    std::vector<PoseVector> world;
    compose_bone_world_positions(parents, rests, local, world);
    ASSERT_NEAR(static_cast<double>(world[1].x), 0.0, 1.0e-4);
    ASSERT_NEAR(static_cast<double>(world[1].y), 10.0, 1.0e-4);
}

TEST_CASE(SkeletalPose, InvalidParentDetachesSafely) {
    const std::vector<int> parents{-1, 99};
    const std::vector<PoseVector> rests{{1.0f, 2.0f, 3.0f}, {10.0f, 0.0f, 0.0f}};
    const std::vector<PoseQuaternion> local{
        {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
    std::vector<PoseVector> world;
    compose_bone_world_positions(parents, rests, local, world);
    ASSERT_EQ(world.size(), 2u);
    ASSERT_NEAR(static_cast<double>(world[1].x), 10.0, 1.0e-5);
}
