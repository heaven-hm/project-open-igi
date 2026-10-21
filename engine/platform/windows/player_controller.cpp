#include "player_controller.hpp"

#include "igi/retail/human/human_camera_eye.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

// Retail movement model evidence for the constants used here (igi.exe SHA256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01):
//   0x00408460 airborne integrator: gravity 84.741692 units/tick^2, air control, no damping.
//   0x004614B0 move-and-collide: sums velocity (+1636) with the root-motion delta (+1312),
//              chooses the ground-query step-down budget, runs the slope-slide bands.
//   0x00462390 ground query: probes down from 1638.4 standing / 819.2 crouched, step-down
//              0 airborne / 2048 grounded / 8192 sliding, roof clearance 7372.8.
//   0x00465820 Human_CameraEyePosition (transcribed in engine/src/retail/human/human_camera_eye.cpp):
//              standing eye 0.69 m, crouched eye 0.35 m.
//   humanplayer.qvm DefineHumanPlayerGeneral(1.75, 17.5, 27, 0.5, ...): root-motion scale
//              1.75, jump 17.5 km/h horizontal, 27 km/h vertical, air control 0.5 km/h.
//   config.qvm: the platform adapter reads movement, jump, crouch and walk/run remaps;
//              mouse look uses the retail hardcoded rate.
// The C# transcription in /Users/haseeb-mir/Documents/Code/open-igi/src/OpenIGI.Game/Player/
// (HumanMotion, HumanGroundQuery, HumanSlopeSlide, HumanPlayerParameters) documents the same
// values per native; this file is host code that applies them and is not a native record.

namespace igi::host
{
namespace
{

constexpr double kPitchLimit = 1.5533;
constexpr double kDegreesPerRadian = 57.2957763671875;

// Retail 0x004B27D0 clamps its argument into [-1, 1] before the arc sine, so a denormalised
// normal cannot produce a NaN. Flat ground reads 0 degrees, a vertical face 90.
double slope_degrees(const std::array<float, 3>& normal)
{
    const double x = normal[0];
    const double y = normal[1];
    const double z = normal[2];
    const double length = std::sqrt((x * x) + (y * y) + (z * z));
    if (length == 0.0)
    {
        return 90.0;
    }
    const double unit_z = std::clamp(z / length, -1.0, 1.0);
    return 90.0 - (std::asin(unit_z) * kDegreesPerRadian);
}

std::array<float, 3> triangle_normal(const std::array<double, 3>& a, const std::array<double, 3>& b,
                                     const std::array<double, 3>& c)
{
    const double ab[3] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
    const double ac[3] = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};
    double normal[3] = {
        (ab[1] * ac[2]) - (ab[2] * ac[1]),
        (ab[2] * ac[0]) - (ab[0] * ac[2]),
        (ab[0] * ac[1]) - (ab[1] * ac[0]),
    };
    const double length = std::sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) +
                                    (normal[2] * normal[2]));
    if (length < 1.0e-12)
    {
        return {0.0F, 0.0F, 1.0F};
    }
    normal[0] /= length;
    normal[1] /= length;
    normal[2] /= length;
    if (normal[2] < 0.0)
    {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
    }
    return {static_cast<float>(normal[0]), static_cast<float>(normal[1]),
            static_cast<float>(normal[2])};
}

} // namespace

void TerrainCollisionGrid::rebuild(const igi::retail::terrain::TerrainDecodedMesh& mesh, double centre_x,
                                   double centre_y)
{
    triangles_.clear();
    for (auto& cell : cells_)
    {
        cell.clear();
    }
    origin_x_ = centre_x - (extent_metres * 0.5);
    origin_y_ = centre_y - (extent_metres * 0.5);
    indexed_ = true;

    const auto vertex_count = mesh.vertices.size();
    for (const auto& batch : mesh.batches)
    {
        const auto& indices = batch.indices;
        for (std::size_t index = 0; index + 2U < indices.size(); index += 3U)
        {
            if (indices[index] >= vertex_count || indices[index + 1U] >= vertex_count ||
                indices[index + 2U] >= vertex_count)
            {
                continue;
            }
            const igi::retail::terrain::TerrainVertex& va = mesh.vertices[indices[index]];
            const igi::retail::terrain::TerrainVertex& vb = mesh.vertices[indices[index + 1U]];
            const igi::retail::terrain::TerrainVertex& vc = mesh.vertices[indices[index + 2U]];
            Triangle triangle{};
            triangle.a = {va.x, va.y, va.z};
            triangle.b = {vb.x, vb.y, vb.z};
            triangle.c = {vc.x, vc.y, vc.z};

            const double min_x = std::min({triangle.a[0], triangle.b[0], triangle.c[0]});
            const double max_x = std::max({triangle.a[0], triangle.b[0], triangle.c[0]});
            const double min_y = std::min({triangle.a[1], triangle.b[1], triangle.c[1]});
            const double max_y = std::max({triangle.a[1], triangle.b[1], triangle.c[1]});
            if (max_x < origin_x_ || min_x > origin_x_ + extent_metres ||
                max_y < origin_y_ || min_y > origin_y_ + extent_metres)
            {
                continue;
            }

            const auto first_cell = [this](double value, double origin)
            {
                const int cell = static_cast<int>(std::floor((value - origin) / cell_size_metres));
                return std::clamp(cell, 0, cells_per_axis - 1);
            };
            const int cell_min_x = first_cell(min_x, origin_x_);
            const int cell_max_x = first_cell(max_x, origin_x_);
            const int cell_min_y = first_cell(min_y, origin_y_);
            const int cell_max_y = first_cell(max_y, origin_y_);

            const auto triangle_index = static_cast<std::uint32_t>(triangles_.size());
            triangles_.push_back(triangle);
            for (int cell_y = cell_min_y; cell_y <= cell_max_y; ++cell_y)
            {
                for (int cell_x = cell_min_x; cell_x <= cell_max_x; ++cell_x)
                {
                    cells_[static_cast<std::size_t>(cell_y * cells_per_axis + cell_x)]
                        .push_back(triangle_index);
                }
            }
        }
    }
}

GroundSample TerrainCollisionGrid::sample(double x, double y) const
{
    GroundSample result{};
    if (triangles_.empty())
    {
        return result;
    }
    const int cell_x =
        static_cast<int>(std::floor((x - origin_x_) / cell_size_metres));
    const int cell_y =
        static_cast<int>(std::floor((y - origin_y_) / cell_size_metres));
    if (cell_x < 0 || cell_x >= cells_per_axis || cell_y < 0 || cell_y >= cells_per_axis)
    {
        return result;
    }

    double best_z = -std::numeric_limits<double>::infinity();
    std::array<float, 3> best_normal{0.0F, 0.0F, 1.0F};
    for (const std::uint32_t triangle_index :
         cells_[static_cast<std::size_t>(cell_y * cells_per_axis + cell_x)])
    {
        const Triangle& triangle = triangles_[triangle_index];
        const double a_x = triangle.a[0];
        const double a_y = triangle.a[1];
        const double b_x = triangle.b[0];
        const double b_y = triangle.b[1];
        const double c_x = triangle.c[0];
        const double c_y = triangle.c[1];
        const double denominator =
            ((b_y - c_y) * (a_x - c_x)) + ((c_x - b_x) * (a_y - c_y));
        if (std::abs(denominator) < 1.0e-12)
        {
            continue;
        }
        const double weight_a =
            (((b_y - c_y) * (x - c_x)) + ((c_x - b_x) * (y - c_y))) / denominator;
        const double weight_b =
            (((c_y - a_y) * (x - c_x)) + ((a_x - c_x) * (y - c_y))) / denominator;
        const double weight_c = 1.0 - weight_a - weight_b;
        constexpr double epsilon = 1.0e-9;
        if (weight_a < -epsilon || weight_b < -epsilon || weight_c < -epsilon)
        {
            continue;
        }
        const double z = (weight_a * triangle.a[2]) + (weight_b * triangle.b[2]) +
                         (weight_c * triangle.c[2]);
        if (z > best_z)
        {
            best_z = z;
            best_normal = triangle_normal(triangle.a, triangle.b, triangle.c);
        }
    }
    if (!std::isfinite(best_z))
    {
        return result;
    }
    result.found = true;
    result.z = best_z;
    result.normal = best_normal;
    return result;
}

void PlayerController::reset(const std::array<double, 3>& feet_position, double yaw, double pitch,
                             const std::array<double, 3>& level_origin, double world_scale)
{
    position_ = feet_position;
    level_origin_ = level_origin;
    world_scale_ = world_scale > 0.0 ? world_scale : (1.0 / units_per_metre);
    yaw_ = yaw;
    pitch_ = std::clamp(pitch, -kPitchLimit, kPitchLimit);
    velocity_ = {};
    slide_ = {};
    ground_height_ = feet_position[2];
    tick_accumulator_ = 0.0;
    ground_found_ = true;
    on_ground_ = true;
    crouched_ = false;
    run_toggled_ = true;
    sliding_ = false;
    jump_held_ = false;
    crouch_held_ = false;
    walk_toggle_held_ = false;
    active_locomotion_state_ = -1;
    animation_time_ms_ = 0.0;
}

void PlayerController::set_parameters(const HumanPlayerParameters& parameters) noexcept
{
    parameters_ = parameters;
    parameters_set_ = true;
}

void PlayerController::set_locomotion_clips(std::vector<AnimationClip> clips)
{
    locomotion_clips_ = std::move(clips);
    active_locomotion_state_ = -1;
    animation_time_ms_ = 0.0;
}

int PlayerController::select_locomotion_state(const PlayerInputFrame& input) const noexcept
{
    // The retail human state table at 0x00538870 is indexed by the state number
    // the movement code stores at +0x10: 0 idle, 1..8 the eight standing
    // directions, 9..16 their crouched counterparts.
    int state = 0;
    if (input.move_forward && input.strafe_right)
    {
        state = 2;
    }
    else if (input.move_forward && input.strafe_left)
    {
        state = 3;
    }
    else if (input.move_forward)
    {
        state = 1;
    }
    else if (input.move_backward && input.strafe_right)
    {
        state = 5;
    }
    else if (input.move_backward && input.strafe_left)
    {
        state = 6;
    }
    else if (input.move_backward)
    {
        state = 4;
    }
    else if (input.strafe_right)
    {
        state = 7;
    }
    else if (input.strafe_left)
    {
        state = 8;
    }

    if (state != 0 && crouched_)
    {
        state += 8;
    }
    return state;
}

const AnimationClip* PlayerController::find_locomotion_clip(int state) const noexcept
{
    if (state < 0 || static_cast<std::size_t>(state) >= human_locomotion_animation_ids.size())
    {
        return nullptr;
    }
    const std::uint32_t id = human_locomotion_animation_ids[static_cast<std::size_t>(state)];
    for (const AnimationClip& clip : locomotion_clips_)
    {
        if (clip.id == id)
        {
            return &clip;
        }
    }
    return nullptr;
}

std::array<double, 3> PlayerController::advance_root_motion(const AnimationClip& clip)
{
    // Retail 0x004D2D30 sets the per-tick clip advance to 4800/ticks_per_second
    // (160 ms at 30 Hz).  The root delta is the difference between the samples
    // at the old and new clip times; a loop restarts at 0 without adding the
    // discontinuity between the clip end and the clip start, exactly as the
    // retail track does.
    constexpr double epsilon = 1e-9;
    std::array<double, 3> delta{};
    double remaining = animation_step_ms;
    double time = std::clamp(animation_time_ms_, 0.0, clip.duration_ms);
    while (remaining > epsilon)
    {
        const double available = clip.duration_ms - time;
        if (available <= epsilon)
        {
            time = 0.0;
            continue;
        }
        const double step = std::min(remaining, available);
        const std::array<double, 3> before = clip.sample_root(time);
        const std::array<double, 3> after = clip.sample_root(time + step);
        for (std::size_t axis = 0; axis < 3U; ++axis)
        {
            delta[axis] += after[axis] - before[axis];
        }
        time += step;
        remaining -= step;
        if (time >= clip.duration_ms - epsilon)
        {
            time = 0.0;
        }
    }
    animation_time_ms_ = time;
    return delta;
}

void PlayerController::update_view(const PlayerInputFrame& input)
{
    yaw_ += static_cast<double>(input.look_delta_x) * look_radians_per_count;
    pitch_ = std::clamp(pitch_ - (static_cast<double>(input.look_delta_y) * look_radians_per_count),
                        -kPitchLimit, kPitchLimit);
}

void PlayerController::update(const PlayerInputFrame& input, double delta_seconds,
                              const TerrainCollisionGrid& ground)
{
    if (!parameters_set_)
    {
        throw std::logic_error(
            "HumanPlayer parameters are not set; read humanplayer.qvm before the first update.");
    }

    update_view(input);

    if (input.walk_toggle && !walk_toggle_held_)
    {
        run_toggled_ = !run_toggled_;
    }
    walk_toggle_held_ = input.walk_toggle;
    if (input.crouch && !crouch_held_)
    {
        crouched_ = !crouched_;
    }
    crouch_held_ = input.crouch;
    const bool jump_pressed = input.jump && !jump_held_;
    jump_held_ = input.jump;

    tick_accumulator_ += delta_seconds;
    constexpr double tick_seconds = 1.0 / ticks_per_second;
    int steps = 0;
    while (tick_accumulator_ >= tick_seconds && steps < 4)
    {
        tick_step(input, jump_pressed && steps == 0, ground);
        tick_accumulator_ -= tick_seconds;
        ++steps;
    }
    if (tick_accumulator_ > tick_seconds)
    {
        tick_accumulator_ = tick_seconds;
    }
}

void PlayerController::tick_step(const PlayerInputFrame& input, bool jump_pressed,
                                 const TerrainCollisionGrid& ground)
{
    // Yaw is measured from +Y towards +X, matching the host camera convention:
    // forward = (sin yaw, cos yaw, 0), right = forward x up = (cos yaw, -sin yaw, 0).
    const double forward_x = std::sin(yaw_);
    const double forward_y = std::cos(yaw_);
    const double right_x = std::cos(yaw_);
    const double right_y = -std::sin(yaw_);

    double wish_x = 0.0;
    double wish_y = 0.0;
    if (input.move_forward)
    {
        wish_x += forward_x;
        wish_y += forward_y;
    }
    if (input.move_backward)
    {
        wish_x -= forward_x;
        wish_y -= forward_y;
    }
    if (input.strafe_left)
    {
        wish_x -= right_x;
        wish_y -= right_y;
    }
    if (input.strafe_right)
    {
        wish_x += right_x;
        wish_y += right_y;
    }
    const double wish_length = std::sqrt((wish_x * wish_x) + (wish_y * wish_y));
    const int locomotion_state = select_locomotion_state(input);

    if (on_ground_)
    {
        if (locomotion_state != 0)
        {
            const AnimationClip* clip = find_locomotion_clip(locomotion_state);
            if (clip == nullptr || clip->root_keys.empty())
            {
                throw std::logic_error(
                    "Ground locomotion requires the retail animation root-motion clip for "
                    "state " +
                    std::to_string(locomotion_state) + "; no fallback speed is available.");
            }
            if (active_locomotion_state_ != locomotion_state)
            {
                active_locomotion_state_ = locomotion_state;
                animation_time_ms_ = 0.0;
            }

            // Retail 0x004D2FB0 samples the root track and 0x004109A0 scales it by
            // humanplayer.qvm argument 0 (1.75) before rotating it by the body
            // matrix.  0x004614B0 then adds the result to the per-tick position.
            const std::array<double, 3> local = advance_root_motion(*clip);
            const double scale = parameters_.root_motion_scale;
            const double local_x = local[0] * scale;
            const double local_y = local[1] * scale;
            const double local_z = local[2] * scale;
            const double cosine_yaw = std::cos(yaw_);
            const double sine_yaw = std::sin(yaw_);
            // Host yaw basis: forward = (sin, cos), right = (cos, -sin).  The clip's
            // local +Y is forward and +X is right (clip 4 +Y, clip 11 +X, clip 10 -X).
            position_[0] += (cosine_yaw * local_x) + (sine_yaw * local_y);
            position_[1] += (-sine_yaw * local_x) + (cosine_yaw * local_y);
            position_[2] += local_z;
            velocity_ = {};
        }
        else
        {
            active_locomotion_state_ = 0;
        }
        // Slope slide displacement (retail 0x004614B0 bands).
        position_[0] += slide_[0];
        position_[1] += slide_[1];

        // Ground query at the new footprint (retail 0x00462390, second pass of the
        // integrator): snap down within the step-down budget or go airborne.
        const double metres_x = (position_[0] - level_origin_[0]) * world_scale_;
        const double metres_y = (position_[1] - level_origin_[1]) * world_scale_;
        const GroundSample surface = ground.sample(metres_x, metres_y);
        if (surface.found)
        {
            ground_found_ = true;
            ground_height_ = (surface.z / world_scale_) + level_origin_[2];
            apply_slope_slide(surface.normal);
            sliding_ = slope_degrees(surface.normal) > slide_shallow_degrees ||
                       (slide_[0] != 0.0) || (slide_[1] != 0.0);
            const double budget = sliding_ ? sliding_step_down : grounded_step_down;
            if (position_[2] - ground_height_ <= budget)
            {
                position_[2] = ground_height_;
                velocity_[2] = 0.0;
            }
            else
            {
                on_ground_ = false;
            }
        }
        else if (!ground.empty())
        {
            // Outside the indexed window: no surface known, so the human is airborne.
            ground_found_ = false;
            sliding_ = false;
            slide_ = {};
            on_ground_ = false;
        }

        slide_[0] *= slide_damping;
        slide_[1] *= slide_damping;
        slide_[2] *= slide_damping;

        if (jump_pressed)
        {
            // Jump (humanplayer.qvm general arguments 1 and 2): 27 km/h up, and the
            // horizontal take-off carries 17.5 km/h along the direction being travelled,
            // or the facing direction when standing still.
            velocity_[2] = parameters_.jump_vertical_kmh * units_per_tick_per_kmh;
            const double take_off_x = wish_length > 0.0 ? wish_x / wish_length : forward_x;
            const double take_off_y = wish_length > 0.0 ? wish_y / wish_length : forward_y;
            velocity_[0] = take_off_x * parameters_.jump_horizontal_kmh * units_per_tick_per_kmh;
            velocity_[1] = take_off_y * parameters_.jump_horizontal_kmh * units_per_tick_per_kmh;
            on_ground_ = false;
            sliding_ = false;
        }
        return;
    }

    // Airborne integration (retail 0x00408460): gravity is subtracted from the vertical
    // first, then the held movement slots add the air-control speed along the body axes.
    velocity_[2] -= gravity_per_tick_squared;
    if (input.move_forward)
    {
        velocity_[0] += forward_x * parameters_.air_control_kmh * units_per_tick_per_kmh;
        velocity_[1] += forward_y * parameters_.air_control_kmh * units_per_tick_per_kmh;
    }
    if (input.move_backward)
    {
        velocity_[0] -= forward_x * parameters_.air_control_kmh * units_per_tick_per_kmh;
        velocity_[1] -= forward_y * parameters_.air_control_kmh * units_per_tick_per_kmh;
    }
    if (input.strafe_left)
    {
        velocity_[0] -= right_x * parameters_.air_control_kmh * units_per_tick_per_kmh;
        velocity_[1] -= right_y * parameters_.air_control_kmh * units_per_tick_per_kmh;
    }
    if (input.strafe_right)
    {
        velocity_[0] += right_x * parameters_.air_control_kmh * units_per_tick_per_kmh;
        velocity_[1] += right_y * parameters_.air_control_kmh * units_per_tick_per_kmh;
    }
    position_[0] += velocity_[0];
    position_[1] += velocity_[1];
    position_[2] += velocity_[2];
    slide_ = {};
    sliding_ = false;

    // Landing: the airborne step-down budget is 0, so the human re-lands on the tick its
    // feet reach the surface.
    const double metres_x = (position_[0] - level_origin_[0]) * world_scale_;
    const double metres_y = (position_[1] - level_origin_[1]) * world_scale_;
    const GroundSample surface = ground.sample(metres_x, metres_y);
    if (surface.found)
    {
        ground_found_ = true;
        ground_height_ = (surface.z / world_scale_) + level_origin_[2];
        if (position_[2] - ground_height_ <= airborne_step_down)
        {
            position_[2] = ground_height_;
            velocity_ = {};
            on_ground_ = true;
            apply_slope_slide(surface.normal);
        }
    }
}

void PlayerController::apply_slope_slide(const std::array<float, 3>& normal)
{
    // Retail 0x004614B0: four bands over the ground normal. The two steep bands set the
    // accumulator outright, the two shallow ones add to it, and the Z component is left
    // alone in every case because the slide is horizontal.
    const double x = normal[0];
    const double y = normal[1];
    const double length = std::sqrt((x * x) + (y * y) + (normal[2] * normal[2]));
    const double unit_x = length > 0.0 ? x / length : 0.0;
    const double unit_y = length > 0.0 ? y / length : 0.0;
    const double slope = slope_degrees(normal);
    if (slope > slide_very_steep_degrees)
    {
        slide_ = {unit_x * slide_very_steep_per_tick, unit_y * slide_very_steep_per_tick, 0.0};
    }
    else if (slope > slide_steep_degrees)
    {
        slide_ = {unit_x * slide_steep_per_tick, unit_y * slide_steep_per_tick, 0.0};
    }
    else if (slope > slide_moderate_degrees)
    {
        slide_[0] += unit_x * slide_moderate_per_tick;
        slide_[1] += unit_y * slide_moderate_per_tick;
    }
    else if (slope > slide_shallow_degrees)
    {
        slide_[0] += unit_x * slide_shallow_per_tick;
        slide_[1] += unit_y * slide_shallow_per_tick;
    }
    // Below 45 degrees nothing is added; the accumulator decays through the damping only,
    // which is what lets a gentle slope build drift over several ticks and fade after it.
}

std::array<double, 3> PlayerController::eye_position() const
{
    // Retail human camera state: the feet position as doubles at +0x20 / +0x28 / +0x30 and
    // the crouch flag byte at +0x33D. Human_CameraEyePosition (0x00465820) copies x and y,
    // adds the standing (0.69 m) or crouched (0.35 m) eye height to z, and returns the eye
    // as three doubles.
    std::array<unsigned char, 0x340> human_bytes{};
    std::memcpy(human_bytes.data() + 0x20, &position_[0], sizeof(double));
    std::memcpy(human_bytes.data() + 0x28, &position_[1], sizeof(double));
    std::memcpy(human_bytes.data() + 0x30, &position_[2], sizeof(double));
    human_bytes[0x33D] = crouched_ ? 1U : 0U;
    std::array<double, 3> eye{};
    igi::retail::human::Human_CameraEyePosition(eye.data(), human_bytes.data());
    return eye;
}

} // namespace igi::host
