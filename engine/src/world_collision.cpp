#include "igi/world_collision.hpp"
#include "igi/logger.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace igi::gameplay {

namespace {

[[nodiscard]] bool intersect_segment_triangle(
    Vector3 orig,
    Vector3 delta,
    const CollisionTriangle& tri,
    float& out_t) noexcept {
    // Möller-Trumbore segment-triangle intersection
    const float e1x = tri.v1.x - tri.v0.x;
    const float e1y = tri.v1.y - tri.v0.y;
    const float e1z = tri.v1.z - tri.v0.z;
    const float e2x = tri.v2.x - tri.v0.x;
    const float e2y = tri.v2.y - tri.v0.y;
    const float e2z = tri.v2.z - tri.v0.z;

    const float px = delta.y * e2z - delta.z * e2y;
    const float py = delta.z * e2x - delta.x * e2z;
    const float pz = delta.x * e2y - delta.y * e2x;

    const float det = e1x * px + e1y * py + e1z * pz;
    if (std::fabs(det) <= 1.0e-6f) {
        return false;
    }

    const float inv_det = 1.0f / det;
    const float tx = orig.x - tri.v0.x;
    const float ty = orig.y - tri.v0.y;
    const float tz = orig.z - tri.v0.z;

    const float u = (tx * px + ty * py + tz * pz) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    const float qx = ty * e1z - tz * e1y;
    const float qy = tz * e1x - tx * e1z;
    const float qz = tx * e1y - ty * e1x;

    const float v = (delta.x * qx + delta.y * qy + delta.z * qz) * inv_det;
    if (v < 0.0f || (u + v) > 1.0f) {
        return false;
    }

    const float t = (e2x * qx + e2y * qy + e2z * qz) * inv_det;
    if (t < 0.0f || t > 1.0f) {
        return false;
    }

    out_t = t;
    return true;
}

[[nodiscard]] float wrap_angle_pi(float radians) noexcept {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kTwoPi = 6.28318530717958647692f;
    while (radians > kPi) {
        radians -= kTwoPi;
    }
    while (radians < -kPi) {
        radians += kTwoPi;
    }
    return radians;
}

} // namespace

void WorldCollision::build_index(const MissionWorld& world) {
    try {
        spatial_grid_.clear();
        const auto& objects = world.placed_objects();
        for (std::size_t i = 0; i < objects.size(); ++i) {
            const PlacedObject& obj = objects[i];
            if (obj.model_id.empty()) {
                continue;
            }
            const ModelMesh* mesh = world.find_model(obj.model_id);
            if (mesh == nullptr ||
                (mesh->collision_triangles_set0.empty() &&
                 mesh->collision_triangles_set1.empty())) {
                continue;
            }

            const double radius = std::max<double>(mesh->bounding_radius, 4096.0);
            const int min_gx = static_cast<int>(std::floor((obj.position.x - radius) / kGridCellSize));
            const int max_gx = static_cast<int>(std::floor((obj.position.x + radius) / kGridCellSize));
            const int min_gy = static_cast<int>(std::floor((obj.position.y - radius) / kGridCellSize));
            const int max_gy = static_cast<int>(std::floor((obj.position.y + radius) / kGridCellSize));

            for (int gy = min_gy; gy <= max_gy; ++gy) {
                for (int gx = min_gx; gx <= max_gx; ++gx) {
                    spatial_grid_[{gx, gy}].push_back(static_cast<int>(i));
                }
            }
        }
        IGI_LOG_INFO("Collision", "Spatial collision index built with " << spatial_grid_.size() << " cells.");
    } catch (const std::exception& e) {
        IGI_LOG_ERROR("Collision", "Exception in WorldCollision::build_index: " << e.what());
    }
}

RaycastHit WorldCollision::raycast(
    const MissionWorld& world,
    NavigationPosition start,
    NavigationPosition end,
    bool use_human_hull_set1,
    bool include_terrain) const noexcept {
    RaycastHit best_hit{};
    best_hit.fraction = 1.0f;

    const double min_x = std::min(start.x, end.x);
    const double max_x = std::max(start.x, end.x);
    const double min_y = std::min(start.y, end.y);
    const double max_y = std::max(start.y, end.y);

    const int min_gx = static_cast<int>(std::floor(min_x / kGridCellSize));
    const int max_gx = static_cast<int>(std::floor(max_x / kGridCellSize));
    const int min_gy = static_cast<int>(std::floor(min_y / kGridCellSize));
    const int max_gy = static_cast<int>(std::floor(max_y / kGridCellSize));

    const auto& objects = world.placed_objects();
    const double seg_mid_x = 0.5 * (start.x + end.x);
    const double seg_mid_y = 0.5 * (start.y + end.y);
    const double seg_mid_z = 0.5 * (start.z + end.z);
    const double seg_half_len = 0.5 * std::sqrt(
        (end.x - start.x) * (end.x - start.x) +
        (end.y - start.y) * (end.y - start.y) +
        (end.z - start.z) * (end.z - start.z));

    // Avoid duplicate object tests when ray spans multiple cells
    std::array<int, 256> tested_objects{};
    std::size_t tested_count = 0;

    for (int gy = min_gy; gy <= max_gy; ++gy) {
        for (int gx = min_gx; gx <= max_gx; ++gx) {
            const auto cell_it = spatial_grid_.find({gx, gy});
            if (cell_it == spatial_grid_.end()) {
                continue;
            }
            for (const int obj_idx : cell_it->second) {
                if (obj_idx < 0 || static_cast<std::size_t>(obj_idx) >= objects.size()) {
                    continue;
                }
                bool already_tested = false;
                for (std::size_t ti = 0; ti < tested_count; ++ti) {
                    if (tested_objects[ti] == obj_idx) {
                        already_tested = true;
                        break;
                    }
                }
                if (already_tested) {
                    continue;
                }
                if (tested_count < tested_objects.size()) {
                    tested_objects[tested_count++] = obj_idx;
                }

                const PlacedObject& obj = objects[static_cast<std::size_t>(obj_idx)];
                if (obj.is_destroyed || obj.is_picked_up ||
                    (obj.kind == PlacedObjectKind::Door && obj.open_progress > 0.85f)) {
                    continue;
                }
                const ModelMesh* mesh = world.find_model(obj.model_id);
                if (mesh == nullptr) {
                    continue;
                }

                // Bounding sphere cull
                const double dx = seg_mid_x - obj.position.x;
                const double dy = seg_mid_y - obj.position.y;
                const double dz = seg_mid_z - obj.position.z;
                const double max_r = static_cast<double>(mesh->bounding_radius) + seg_half_len + 256.0;
                if (dx * dx + dy * dy + dz * dz > max_r * max_r) {
                    continue;
                }

                const auto& tris = (use_human_hull_set1 && !mesh->collision_triangles_set1.empty())
                    ? mesh->collision_triangles_set1
                    : mesh->collision_triangles_set0;
                if (tris.empty()) {
                    continue;
                }

                // Transform ray into object local space
                const Vector3 rel_start{
                    static_cast<float>(start.x - obj.position.x),
                    static_cast<float>(start.y - obj.position.y),
                    static_cast<float>(start.z - obj.position.z),
                };
                const Vector3 rel_end{
                    static_cast<float>(end.x - obj.position.x),
                    static_cast<float>(end.y - obj.position.y),
                    static_cast<float>(end.z - obj.position.z),
                };
                const Vector3 local_start =
                    MissionWorld::transform_world_to_local_offset(rel_start, obj.basis);
                const Vector3 local_end =
                    MissionWorld::transform_world_to_local_offset(rel_end, obj.basis);
                const Vector3 local_delta{
                    local_end.x - local_start.x,
                    local_end.y - local_start.y,
                    local_end.z - local_start.z,
                };

                for (const CollisionTriangle& tri : tris) {
                    if (tri.is_portal) {
                        continue;
                    }
                    float t = 1.0f;
                    if (intersect_segment_triangle(local_start, local_delta, tri, t) &&
                        t < best_hit.fraction) {
                        Vector3 world_n =
                            MissionWorld::transform_local_to_world_offset(tri.unit_normal, obj.basis);
                        // Ensure normal opposes ray direction
                        const float dot_dir =
                            world_n.x * static_cast<float>(end.x - start.x) +
                            world_n.y * static_cast<float>(end.y - start.y) +
                            world_n.z * static_cast<float>(end.z - start.z);
                        if (dot_dir > 0.0f) {
                            world_n.x = -world_n.x;
                            world_n.y = -world_n.y;
                            world_n.z = -world_n.z;
                        }

                        best_hit.hit = true;
                        best_hit.fraction = t;
                        best_hit.point = {
                            start.x + (end.x - start.x) * static_cast<double>(t),
                            start.y + (end.y - start.y) * static_cast<double>(t),
                            start.z + (end.z - start.z) * static_cast<double>(t),
                        };
                        best_hit.unit_normal = world_n;
                        best_hit.material_id = tri.material_id;
                        best_hit.object_index = obj_idx;
                    }
                }
            }
        }
    }

    if (include_terrain) {
        float terrain_z = 0.0f;
        Vector3 terrain_n{0.0f, 0.0f, 1.0f};
        if (world.sample_terrain_height(
                end.x,
                end.y,
                std::min(start.z, end.z),
                std::max(start.z, end.z),
                terrain_z,
                terrain_n)) {
            const double start_above = start.z - static_cast<double>(terrain_z);
            const double end_above = end.z - static_cast<double>(terrain_z);
            if (start_above >= -32.0 && end_above <= 0.0) {
                const double denom = start_above - end_above;
                const float t = (denom > 1.0e-4)
                    ? std::clamp(static_cast<float>(start_above / denom), 0.0f, 1.0f)
                    : 0.0f;
                if (t < best_hit.fraction) {
                    best_hit.hit = true;
                    best_hit.fraction = t;
                    best_hit.point = {
                        start.x + (end.x - start.x) * static_cast<double>(t),
                        start.y + (end.y - start.y) * static_cast<double>(t),
                        static_cast<double>(terrain_z),
                    };
                    best_hit.unit_normal = terrain_n;
                    best_hit.material_id = 0;
                    best_hit.object_index = -1;
                }
            }
        }
    }

    return best_hit;
}

WallSweepResult WorldCollision::sweep_walls(
    const MissionWorld& world,
    NavigationPosition start,
    NavigationPosition desired_end,
    Vector3 velocity,
    bool is_grounded,
    bool is_crouching) const noexcept {
    // Retail Human_WallSweep (0x00461A70)
    WallSweepResult result{};
    result.resolved_position = desired_end;
    result.resolved_velocity = velocity;

    const std::array<float, 3> probe_heights = {
        kLowProbeHeight,
        is_crouching ? kMidProbeCrouching : kMidProbeStanding,
        is_crouching ? kCrouchingEyeHeight : kStandingEyeHeight,
    };
    const int first_probe = is_grounded ? 1 : 0;

    NavigationPosition candidate = desired_end;
    Vector3 cur_vel = velocity;
    int checks = 0;

    while (true) {
        const double dx = candidate.x - start.x;
        const double dy = candidate.y - start.y;
        const double dz = candidate.z - start.z;
        const double move_len_sq = dx * dx + dy * dy + dz * dz;
        if (move_len_sq <= 1.0e-8) {
            break;
        }

        const double move_len = std::sqrt(move_len_sq);
        const double inv_len = 1.0 / move_len;
        const double skin_x = dx * inv_len * static_cast<double>(kWallSkinUnits);
        const double skin_y = dy * inv_len * static_cast<double>(kWallSkinUnits);
        const double skin_z = dz * inv_len * static_cast<double>(kWallSkinUnits);

        bool hit_wall_in_pass = false;
        RaycastHit closest_wall{};
        closest_wall.fraction = 1.0f;

        for (int p = first_probe; p < 3; ++p) {
            const double z_off = static_cast<double>(probe_heights[static_cast<std::size_t>(p)]);
            const NavigationPosition p_start{start.x, start.y, start.z + z_off};
            const NavigationPosition p_end{
                candidate.x + skin_x,
                candidate.y + skin_y,
                candidate.z + z_off + skin_z,
            };

            const RaycastHit hit = raycast(world, p_start, p_end, true, false);
            if (!hit.hit) {
                continue;
            }
            if (std::fabs(hit.unit_normal.z) >= 0.1f) {
                continue;
            }
            if (hit.fraction < closest_wall.fraction) {
                closest_wall = hit;
                hit_wall_in_pass = true;
            }
        }

        if (!hit_wall_in_pass) {
            break;
        }

        result.hit_wall = true;
        ++checks;
        if (checks > 10) {
            candidate = start;
            result.blocked = true;
            break;
        }

        float nx = closest_wall.unit_normal.x;
        float ny = closest_wall.unit_normal.y;
        const float n_xy_len = std::sqrt(nx * nx + ny * ny);
        if (n_xy_len <= 1.0e-5f) {
            candidate = start;
            result.blocked = true;
            break;
        }
        nx /= n_xy_len;
        ny /= n_xy_len;

        // Project velocity onto wall plane
        const float vel_dot_n = cur_vel.x * nx + cur_vel.y * ny;
        if (vel_dot_n < 0.0f) {
            cur_vel.x -= vel_dot_n * nx;
            cur_vel.y -= vel_dot_n * ny;
        }

        // Slide remaining displacement along wall plane with 10.24 unit skin
        const double hit_x = start.x + (candidate.x + skin_x - start.x) * static_cast<double>(closest_wall.fraction);
        const double hit_y = start.y + (candidate.y + skin_y - start.y) * static_cast<double>(closest_wall.fraction);
        const double pen_x = candidate.x - hit_x;
        const double pen_y = candidate.y - hit_y;
        const double pen_dot_n = pen_x * static_cast<double>(nx) + pen_y * static_cast<double>(ny);

        candidate.x -= (pen_dot_n - static_cast<double>(kWallSkinUnits)) * static_cast<double>(nx);
        candidate.y -= (pen_dot_n - static_cast<double>(kWallSkinUnits)) * static_cast<double>(ny);
    }

    result.resolved_position = candidate;
    result.resolved_velocity = cur_vel;
    result.iterations = checks;
    return result;
}

GroundQueryResult WorldCollision::query_ground(
    const MissionWorld& world,
    NavigationPosition position,
    bool is_grounded,
    bool is_crouching,
    bool is_slope_sliding) const noexcept {
    // Retail Human_GroundProbe (0x00462390)
    GroundQueryResult result{};
    const double probe_origin_offset = is_crouching ? kMidProbeCrouching : kMidProbeStanding;
    const double step_down_budget = is_slope_sliding
        ? 8192.0
        : (is_grounded ? 2048.0 : 0.0);

    const NavigationPosition ray_start{
        position.x,
        position.y,
        position.z + probe_origin_offset,
    };
    const NavigationPosition ray_end{
        position.x,
        position.y,
        position.z - step_down_budget,
    };

    const RaycastHit obj_hit = raycast(world, ray_start, ray_end, false, false);
    if (obj_hit.hit && obj_hit.unit_normal.z >= 0.1f) {
        result.found_ground = true;
        result.ground_z = obj_hit.point.z;
        result.ground_normal = obj_hit.unit_normal;
        result.material_id = obj_hit.material_id;
        result.object_index = obj_hit.object_index;
    }

    float terrain_z = 0.0f;
    Vector3 terrain_n{0.0f, 0.0f, 1.0f};
    if (world.sample_terrain_height(
            position.x,
            position.y,
            ray_end.z,
            ray_start.z,
            terrain_z,
            terrain_n)) {
        const double t_z = static_cast<double>(terrain_z);
        if (t_z <= ray_start.z && t_z >= ray_end.z) {
            if (!result.found_ground || t_z >= result.ground_z) {
                result.found_ground = true;
                result.ground_z = t_z;
                result.ground_normal = terrain_n;
                result.material_id = 0;
                result.object_index = -1;
            }
        }
    }

    return result;
}

NavigationPosition WorldCollision::resolve_cylinder_separation(
    NavigationPosition start,
    NavigationPosition candidate,
    const std::vector<NavigationPosition>& blocker_positions) noexcept {
    // Retail Human_Separation (0x00462BA0)
    const double radius_sq =
        static_cast<double>(kSeparationRadius) * static_cast<double>(kSeparationRadius);

    for (const NavigationPosition& blocker : blocker_positions) {
        if (candidate.z > blocker.z + static_cast<double>(kSeparationHeight) ||
            candidate.z + static_cast<double>(kSeparationHeight) < blocker.z) {
            continue;
        }
        const double cand_dx = blocker.x - candidate.x;
        const double cand_dy = blocker.y - candidate.y;
        const double cand_dist_sq = cand_dx * cand_dx + cand_dy * cand_dy;
        if (cand_dist_sq >= radius_sq) {
            continue;
        }
        const double start_dx = blocker.x - start.x;
        const double start_dy = blocker.y - start.y;
        const double start_dist_sq = start_dx * start_dx + start_dy * start_dy;
        if (cand_dist_sq < start_dist_sq) {
            return start;
        }
    }
    return candidate;
}

void WorldCollision::update_slope_slide(
    Vector3 ground_normal,
    bool is_grounded,
    float& io_slide_speed,
    bool& out_is_sliding,
    Vector3& io_displacement) noexcept {
    // Retail slope slide (0x004614B0)
    if (!is_grounded) {
        io_slide_speed = 0.0f;
        out_is_sliding = false;
        return;
    }

    const float clamped_nz = std::clamp(ground_normal.z, -1.0f, 1.0f);
    const float slope_degrees = 90.0f - std::asin(clamped_nz) * 57.295776f;

    if (slope_degrees > 80.0f) {
        io_slide_speed = 1137.0f;
    } else if (slope_degrees > 60.0f) {
        io_slide_speed = 948.0f;
    } else if (slope_degrees > 50.0f) {
        io_slide_speed += 56.888889f;
    } else if (slope_degrees > 45.0f) {
        io_slide_speed += 18.962963f;
    }

    io_slide_speed *= 0.89999998f;
    if (io_slide_speed <= 1.0f) {
        io_slide_speed = 0.0f;
        out_is_sliding = false;
        return;
    }

    out_is_sliding = true;
    const float n_xy_len = std::sqrt(
        ground_normal.x * ground_normal.x + ground_normal.y * ground_normal.y);
    if (n_xy_len > 1.0e-5f) {
        io_displacement.x += (ground_normal.x / n_xy_len) * io_slide_speed;
        io_displacement.y += (ground_normal.y / n_xy_len) * io_slide_speed;
    }
}

void WorldCollision::step_player(
    const MissionWorld& world,
    const InputSnapshot& input,
    float mouse_delta_x,
    float mouse_delta_y,
    const std::vector<NavigationPosition>& active_soldier_positions,
    PlayerPhysicsState& io_player) const noexcept {
    // 1. Camera look controls (Arrow keys + Mouse delta)
    constexpr float kKeyYawSpeed = 0.045f;
    constexpr float kKeyPitchSpeed = 0.035f;
    constexpr float kMouseSensitivity = 0.0028f;

    float yaw_delta = -mouse_delta_x * kMouseSensitivity;
    float pitch_delta = -mouse_delta_y * kMouseSensitivity;

    if (input.is_down(InputCode::ArrowLeft)) {
        yaw_delta += kKeyYawSpeed;
    }
    if (input.is_down(InputCode::ArrowRight)) {
        yaw_delta -= kKeyYawSpeed;
    }
    if (input.is_down(InputCode::ArrowUp)) {
        pitch_delta += kKeyPitchSpeed;
    }
    if (input.is_down(InputCode::ArrowDown)) {
        pitch_delta -= kKeyPitchSpeed;
    }

    io_player.yaw = wrap_angle_pi(io_player.yaw + yaw_delta);
    io_player.pitch = std::clamp(
        io_player.pitch + pitch_delta,
        -kPitchLimitRadians,
        kPitchLimitRadians);

    // 2. Crouch state & eye-height easing (400 units/tick)
    io_player.is_crouching =
        input.is_down(InputCode::KeyC) ||
        input.is_down(InputCode::LeftControl) ||
        input.is_down(InputCode::RightControl);
    const float target_eye = io_player.is_crouching ? kCrouchingEyeHeight : kStandingEyeHeight;
    if (io_player.eye_height < target_eye) {
        io_player.eye_height = std::min(target_eye, io_player.eye_height + kCrouchEasePerTick);
    } else if (io_player.eye_height > target_eye) {
        io_player.eye_height = std::max(target_eye, io_player.eye_height - kCrouchEasePerTick);
    }

    // 3. Movement intents (WASD)
    const bool move_forward = input.is_down(InputCode::KeyW);
    const bool move_backward = input.is_down(InputCode::KeyS);
    const bool strafe_left = input.is_down(InputCode::KeyA);
    const bool strafe_right = input.is_down(InputCode::KeyD);
    const bool shift_held =
        input.is_down(InputCode::LeftShift) || input.is_down(InputCode::RightShift);
    io_player.is_running = !shift_held && !io_player.is_crouching;

    // Check if near a ClimbLadder object using verified LadderPlacement
    bool near_ladder = false;
    for (const PlacedObject& obj : world.placed_objects()) {
        if (obj.kind != PlacedObjectKind::ClimbLadder) {
            continue;
        }
        const Vector3 player_pos{
            static_cast<float>(io_player.position.x),
            static_cast<float>(io_player.position.y),
            static_cast<float>(io_player.position.z),
        };
        const Vector3 ladder_origin{
            static_cast<float>(obj.position.x),
            static_cast<float>(obj.position.y),
            static_cast<float>(obj.position.z),
        };
        const Vector3 ladder_top{
            ladder_origin.x,
            ladder_origin.y,
            ladder_origin.z + 40000.0f,
        };
        const Vector3 look_direction{
            std::cos(io_player.yaw),
            std::sin(io_player.yaw),
            0.0f,
        };
        const LadderPlacement placement{
            ladder_origin,
            ladder_top,
            ladder_origin,
            ladder_top,
            {0.0f, 1.0f, 0.0f},
        };
        if (placement.can_activate(player_pos, look_direction)) {
            near_ladder = true;
            break;
        }
    }
    if (!near_ladder) {
        io_player.is_climbing_ladder = false;
    } else if (move_forward || move_backward) {
        io_player.is_climbing_ladder = true;
    }

    const Basis3x3 body_basis = io_player.body_basis();

    if (io_player.is_climbing_ladder) {
        io_player.velocity = HumanMotion::integrate_ladder_slide(io_player.velocity);
        if (move_forward) {
            io_player.velocity.z = 360.0f;
        } else if (move_backward) {
            io_player.velocity.z = -360.0f;
        }
        if (input.was_pressed(InputCode::Space)) {
            io_player.is_climbing_ladder = false;
            io_player.is_grounded = false;
            io_player.velocity.z = 620.0f;
        }
    } else if (io_player.is_grounded) {
        float forward_input = 0.0f;
        float right_input = 0.0f;
        if (move_forward) forward_input += 1.0f;
        if (move_backward) forward_input -= 1.0f;
        if (strafe_right) right_input += 1.0f;
        if (strafe_left) right_input -= 1.0f;

        const float input_len = std::sqrt(
            forward_input * forward_input + right_input * right_input);
        if (input_len > 1.0e-4f) {
            forward_input /= input_len;
            right_input /= input_len;
        }

        const float move_speed = io_player.is_crouching
            ? 245.0f
            : (io_player.is_running ? 680.0f : 380.0f);

        // In IGI's coordinate frame: +X is forward at yaw=0, +Y is left at yaw=0
        const Vector3 local_step{
            forward_input * move_speed,
            -right_input * move_speed,
            0.0f,
        };
        const Vector3 world_step =
            HumanMotion::apply_root_motion(local_step, body_basis, 1.0f, false);
        io_player.velocity.x = world_step.x;
        io_player.velocity.y = world_step.y;
        io_player.velocity.z = 0.0f;

        if (input.was_pressed(InputCode::Space)) {
            io_player.is_grounded = false;
            io_player.velocity.z = 740.0f;
        }

        if (input_len > 0.1f) {
            io_player.bob_phase += io_player.is_running ? 0.22f : 0.13f;
        }
    } else {
        // Airborne integration using verified HumanMotion::integrate_airborne (0x408460)
        io_player.velocity = HumanMotion::integrate_airborne(
            io_player.velocity,
            move_backward,
            move_forward,
            strafe_left,
            strafe_right,
            body_basis,
            24.0f);
    }

    Vector3 displacement = io_player.velocity;
    update_slope_slide(
        io_player.ground_normal,
        io_player.is_grounded,
        io_player.slope_slide_speed,
        io_player.is_slope_sliding,
        displacement);

    const NavigationPosition start_pos = io_player.position;
    const NavigationPosition desired_pos{
        start_pos.x + static_cast<double>(displacement.x),
        start_pos.y + static_cast<double>(displacement.y),
        start_pos.z + static_cast<double>(displacement.z),
    };

    // 4. Wall sweep (0x00461A70)
    const WallSweepResult wall_res = sweep_walls(
        world,
        start_pos,
        desired_pos,
        io_player.velocity,
        io_player.is_grounded,
        io_player.is_crouching);
    io_player.velocity = wall_res.resolved_velocity;

    // 5. Cylinder separation against live soldiers (0x00462BA0)
    NavigationPosition separated_pos = resolve_cylinder_separation(
        start_pos,
        wall_res.resolved_position,
        active_soldier_positions);

    // 6. Ground query & step-down snap (0x00462390)
    if (!io_player.is_climbing_ladder) {
        const bool was_airborne = !io_player.is_grounded;
        const float prior_vertical_velocity = io_player.velocity.z;

        const GroundQueryResult ground_res = query_ground(
            world,
            separated_pos,
            io_player.is_grounded,
            io_player.is_crouching,
            io_player.is_slope_sliding);
        if (ground_res.found_ground && io_player.velocity.z <= 0.0f) {
            separated_pos.z = ground_res.ground_z;
            io_player.velocity.z = 0.0f;
            io_player.is_grounded = true;
            io_player.ground_normal = ground_res.ground_normal;

            if (was_airborne && prior_vertical_velocity < -50.0f) {
                const PlayerFallImpact impact = calculate_vanilla_fall_impact(
                    prior_vertical_velocity,
                    100.0f);
                io_player.health = std::max(0.0f, io_player.health - impact.damage);
                io_player.last_fall_impact_damage = impact.damage;
                io_player.last_fall_hearing_alert = impact.hearing_radius_units;
                io_player.last_fall_view_kick = impact.view_kick_units;
            }
        } else {
            io_player.is_grounded = false;
            io_player.ground_normal = {0.0f, 0.0f, 1.0f};
        }
    }

    io_player.position = separated_pos;

    // 7. Update HumanLocomotionGraph state
    const bool any_move = move_forward || move_backward || strafe_left || strafe_right;
    std::uint32_t target_loco = 0;
    if (!io_player.is_grounded) {
        target_loco = 8; // Jump / airborne
    } else if (io_player.is_crouching) {
        target_loco = any_move ? 4 : 3;
    } else if (any_move) {
        target_loco = io_player.is_running ? 2 : 1;
    } else {
        target_loco = 0;
    }
    if (io_player.locomotion_state < HumanLocomotionGraph::state_count) {
        const auto available_transitions = HumanLocomotionGraph::transitions(
            static_cast<int>(io_player.locomotion_state));
        if (std::find(available_transitions.begin(), available_transitions.end(),
                      static_cast<int>(target_loco)) != available_transitions.end()) {
            io_player.locomotion_state = target_loco;
        }
    }
}

} // namespace igi::gameplay
