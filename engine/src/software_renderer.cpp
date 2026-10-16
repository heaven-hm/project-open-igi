#include "igi/software_renderer.hpp"
#include "igi/hud_font.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>

namespace igi::gameplay {

namespace {

constexpr float kNearPlane = 200.0f;
constexpr float kFogStart = 220000.0f;
constexpr float kFogEnd = 540000.0f;
constexpr std::uint32_t kFogColor = 0xFFB8C4CCu;

[[nodiscard]] std::uint32_t modulate(std::uint32_t argb, float light) noexcept {
    const float l = std::clamp(light, 0.0f, 1.5f);
    const auto scale = [l](std::uint32_t c) {
        return std::min<std::uint32_t>(255u, static_cast<std::uint32_t>(static_cast<float>(c) * l));
    };
    return 0xFF000000u | (scale((argb >> 16) & 0xFFu) << 16) |
           (scale((argb >> 8) & 0xFFu) << 8) | scale(argb & 0xFFu);
}

[[nodiscard]] std::uint32_t blend(std::uint32_t a, std::uint32_t b, float t) noexcept {
    const float k = std::clamp(t, 0.0f, 1.0f);
    const auto mix = [k](std::uint32_t x, std::uint32_t y) {
        return static_cast<std::uint32_t>(
            static_cast<float>(x) + (static_cast<float>(y) - static_cast<float>(x)) * k);
    };
    return 0xFF000000u | (mix((a >> 16) & 0xFFu, (b >> 16) & 0xFFu) << 16) |
           (mix((a >> 8) & 0xFFu, (b >> 8) & 0xFFu) << 8) | mix(a & 0xFFu, b & 0xFFu);
}

struct CameraBasis {
    Vector3 forward;
    Vector3 right;
    Vector3 up;
};

[[nodiscard]] CameraBasis make_camera_basis(float yaw, float pitch) noexcept {
    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);
    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    CameraBasis b{};
    b.forward = {cp * cy, cp * sy, sp};
    b.right = {sy, -cy, 0.0f};
    b.up = {-sp * cy, -sp * sy, cp};
    return b;
}

[[nodiscard]] Vector3 to_camera(const CameraBasis& b, float dx, float dy, float dz) noexcept {
    return {
        dx * b.right.x + dy * b.right.y + dz * b.right.z,
        dx * b.up.x + dy * b.up.y + dz * b.up.z,
        dx * b.forward.x + dy * b.forward.y + dz * b.forward.z,
    };
}

} // namespace

void SoftwareRenderer::resize(std::uint32_t width, std::uint32_t height) {
    width_ = width;
    height_ = height;
    const std::size_t count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    color_.assign(count, 0xFF000000u);
    depth_.assign(count, 1.0e30f);
}

void SoftwareRenderer::clear_with_sky(const CameraView& camera) {
    if (width_ == 0 || height_ == 0) {
        return;
    }
    const float half_h = static_cast<float>(height_) * 0.5f;
    const float focal = half_h / std::tan(camera.vertical_fov_radians * 0.5f);
    const float horizon_row = half_h + focal * std::tan(camera.pitch);
    constexpr std::uint32_t kZenith = 0xFF3C6AA8u;
    constexpr std::uint32_t kHorizon = 0xFFA9C1D4u;
    for (std::uint32_t y = 0; y < height_; ++y) {
        const float above = horizon_row - static_cast<float>(y);
        const std::uint32_t row_color = above > 0.0f
            ? blend(kHorizon, kZenith, above / std::max(1.0f, focal * 1.2f))
            : kFogColor;
        std::fill_n(color_.begin() + static_cast<std::ptrdiff_t>(y * width_), width_, row_color);
    }
    std::fill(depth_.begin(), depth_.end(), 1.0e30f);
}

void SoftwareRenderer::draw_triangle(
    const ClipVertex& a,
    const ClipVertex& b,
    const ClipVertex& c,
    const TextureImage* texture,
    float focal_px) {
    const float cx = static_cast<float>(width_) * 0.5f;
    const float cy = static_cast<float>(height_) * 0.5f;
    const float ax = cx + focal_px * a.x / a.z;
    const float ay = cy - focal_px * a.y / a.z;
    const float bx = cx + focal_px * b.x / b.z;
    const float by = cy - focal_px * b.y / b.z;
    const float ccx = cx + focal_px * c.x / c.z;
    const float ccy = cy - focal_px * c.y / c.z;

    const float area = (bx - ax) * (ccy - ay) - (by - ay) * (ccx - ax);
    if (std::fabs(area) < 1.0e-4f) {
        return;
    }

    const int min_x = std::max(0, static_cast<int>(std::floor(std::min({ax, bx, ccx}))));
    const int max_x = std::min(static_cast<int>(width_) - 1,
                               static_cast<int>(std::ceil(std::max({ax, bx, ccx}))));
    const int min_y = std::max(0, static_cast<int>(std::floor(std::min({ay, by, ccy}))));
    const int max_y = std::min(static_cast<int>(height_) - 1,
                               static_cast<int>(std::ceil(std::max({ay, by, ccy}))));
    if (min_x > max_x || min_y > max_y) {
        return;
    }

    const float inv_area = 1.0f / area;
    const float iz0 = 1.0f / a.z;
    const float iz1 = 1.0f / b.z;
    const float iz2 = 1.0f / c.z;

    for (int y = min_y; y <= max_y; ++y) {
        const float py = static_cast<float>(y) + 0.5f;
        for (int x = min_x; x <= max_x; ++x) {
            const float px = static_cast<float>(x) + 0.5f;
            const float w0 = ((bx - px) * (ccy - py) - (by - py) * (ccx - px)) * inv_area;
            const float w1 = ((ccx - px) * (ay - py) - (ccy - py) * (ax - px)) * inv_area;
            const float w2 = 1.0f - w0 - w1;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }
            const float inv_z = w0 * iz0 + w1 * iz1 + w2 * iz2;
            if (inv_z <= 0.0f) {
                continue;
            }
            const float z = 1.0f / inv_z;
            const std::size_t index = static_cast<std::size_t>(y) * width_ + static_cast<std::size_t>(x);
            if (z >= depth_[index]) {
                continue;
            }

            const float u = (w0 * a.u * iz0 + w1 * b.u * iz1 + w2 * c.u * iz2) * z;
            const float v = (w0 * a.v * iz0 + w1 * b.v * iz1 + w2 * c.v * iz2) * z;
            const float light = w0 * a.light + w1 * b.light + w2 * c.light;
            const float opacity = std::clamp(
                w0 * a.opacity + w1 * b.opacity + w2 * c.opacity, 0.0f, 1.0f);

            std::uint32_t texel = 0xFF8C9088u;
            float texel_alpha = 1.0f;
            if (texture != nullptr) {
                texel = texture->sample_bilinear(u, v);
                if (texture->has_alpha) {
                    texel_alpha = static_cast<float>(texel >> 24) / 255.0f;
                }
            }
            const float effective_alpha = opacity * texel_alpha;
            if (effective_alpha < 0.02f) {
                continue; // effectively transparent: retail cutout
            }
            std::uint32_t shaded = modulate(texel, light);
            if (z > kFogStart) {
                shaded = blend(shaded, kFogColor, (z - kFogStart) / (kFogEnd - kFogStart));
            }
            if (effective_alpha >= 0.985f) {
                depth_[index] = z;
                color_[index] = shaded;
            } else {
                // Retail translucency (glass, fences, flats): source-over the
                // shaded texel without writing depth. Unsorted, noted limit.
                color_[index] = blend(color_[index], shaded, effective_alpha);
            }
        }
    }
}

void SoftwareRenderer::draw_clipped_triangle(
    ClipVertex a,
    ClipVertex b,
    ClipVertex c,
    const TextureImage* texture,
    float focal_px) {
    ClipVertex in[3] = {a, b, c};
    ClipVertex out[4];
    int out_count = 0;
    for (int i = 0; i < 3; ++i) {
        const ClipVertex& cur = in[i];
        const ClipVertex& nxt = in[(i + 1) % 3];
        const bool cur_in = cur.z >= kNearPlane;
        const bool nxt_in = nxt.z >= kNearPlane;
        if (cur_in) {
            out[out_count++] = cur;
        }
        if (cur_in != nxt_in) {
            const float t = (kNearPlane - cur.z) / (nxt.z - cur.z);
            ClipVertex m{};
            m.x = cur.x + (nxt.x - cur.x) * t;
            m.y = cur.y + (nxt.y - cur.y) * t;
            m.z = kNearPlane;
            m.u = cur.u + (nxt.u - cur.u) * t;
            m.v = cur.v + (nxt.v - cur.v) * t;
            m.light = cur.light + (nxt.light - cur.light) * t;
            m.opacity = cur.opacity + (nxt.opacity - cur.opacity) * t;
            out[out_count++] = m;
        }
    }
    if (out_count >= 3) {
        draw_triangle(out[0], out[1], out[2], texture, focal_px);
    }
    if (out_count == 4) {
        draw_triangle(out[0], out[2], out[3], texture, focal_px);
    }
}

void SoftwareRenderer::render_world(
    const MissionWorld& world,
    const CameraView& camera,
    const std::vector<NavigationPosition>& soldier_positions) {
    std::vector<SoldierRenderData> soldiers;
    soldiers.reserve(soldier_positions.size());
    for (const auto& pos : soldier_positions) {
        SoldierRenderData srd;
        srd.position = pos;
        soldiers.push_back(srd);
    }
    render_world(world, camera, soldiers);
}

void SoftwareRenderer::render_world(
    const MissionWorld& world,
    const CameraView& camera,
    const std::vector<SoldierRenderData>& soldiers) {
    if (width_ == 0 || height_ == 0) {
        return;
    }
    clear_with_sky(camera);

    const float half_h = static_cast<float>(height_) * 0.5f;
    const float focal = half_h / std::tan(camera.vertical_fov_radians * 0.5f);
    const CameraBasis basis = make_camera_basis(camera.yaw, camera.pitch);

    // Fixed sun direction (world space, pointing toward the sun).
    constexpr Vector3 kSun{0.35f, -0.25f, 0.90f};
    const float sun_len = std::sqrt(kSun.x * kSun.x + kSun.y * kSun.y + kSun.z * kSun.z);

    const auto light_for_normal = [&](float nx, float ny, float nz) {
        const float len = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (len < 1.0e-6f) {
            return 0.8f;
        }
        const float d = (nx * kSun.x + ny * kSun.y + nz * kSun.z) / (len * sun_len);
        return 0.50f + 0.55f * std::max(0.0f, d);
    };

    const auto world_to_clip = [&](double wx, double wy, double wz, float u, float v) {
        const Vector3 cam = to_camera(
            basis,
            static_cast<float>(wx - camera.eye.x),
            static_cast<float>(wy - camera.eye.y),
            static_cast<float>(wz - camera.eye.z));
        ClipVertex cv{};
        cv.x = cam.x;
        cv.y = cam.y;
        cv.z = cam.z;
        cv.u = u;
        cv.v = v;
        return cv;
    };

    // Terrain
    for (const TerrainTriangle& tri : world.terrain_triangles()) {
        const double mx = (static_cast<double>(tri.v0.x) + tri.v1.x + tri.v2.x) / 3.0 - camera.eye.x;
        const double my = (static_cast<double>(tri.v0.y) + tri.v1.y + tri.v2.y) / 3.0 - camera.eye.y;
        if (mx * mx + my * my > static_cast<double>(kFogEnd) * kFogEnd) {
            continue;
        }
        const TextureImage* tex = world.terrain_tile_texture(tri.tile_index);
        ClipVertex a = world_to_clip(tri.v0.x, tri.v0.y, tri.v0.z, tri.u0 * 4.0f, tri.v0_uv * 4.0f);
        ClipVertex b = world_to_clip(tri.v1.x, tri.v1.y, tri.v1.z, tri.u1 * 4.0f, tri.v1_uv * 4.0f);
        ClipVertex c = world_to_clip(tri.v2.x, tri.v2.y, tri.v2.z, tri.u2 * 4.0f, tri.v2_uv * 4.0f);
        a.light = light_for_normal(tri.n0.x, tri.n0.y, tri.n0.z);
        b.light = light_for_normal(tri.n1.x, tri.n1.y, tri.n1.z);
        c.light = light_for_normal(tri.n2.x, tri.n2.y, tri.n2.z);
        if (a.z < kNearPlane && b.z < kNearPlane && c.z < kNearPlane) {
            continue;
        }
        draw_clipped_triangle(a, b, c, tex, focal);
    }

    // Placed models
    for (const PlacedObject& obj : world.placed_objects()) {
        if (obj.is_destroyed || obj.is_picked_up || obj.model_id.empty()) {
            continue;
        }
        const ModelMesh* mesh = world.find_model(obj.model_id);
        if (mesh == nullptr) {
            continue;
        }
        const double dx = obj.position.x - camera.eye.x;
        const double dy = obj.position.y - camera.eye.y;
        const double dz = obj.position.z - camera.eye.z;
        const double reach = static_cast<double>(kFogEnd) + mesh->bounding_radius;
        if (dx * dx + dy * dy + dz * dz > reach * reach) {
            continue;
        }
        const float cam_forward = static_cast<float>(
            dx * basis.forward.x + dy * basis.forward.y + dz * basis.forward.z);
        if (cam_forward < -mesh->bounding_radius) {
            continue;
        }

        // Pre-transform vertices once per object.
        std::vector<ClipVertex> verts(mesh->vertices.size());
        std::vector<Vector3> world_pts(mesh->vertices.size());
        for (std::size_t i = 0; i < mesh->vertices.size(); ++i) {
            const MeshVertex& mv = mesh->vertices[i];
            const Vector3 off = MissionWorld::transform_local_to_world_offset(mv.position, obj.basis);
            world_pts[i] = off;
            verts[i] = world_to_clip(
                obj.position.x + off.x, obj.position.y + off.y, obj.position.z + off.z, mv.u, mv.v);
            const Vector3 wn = MissionWorld::transform_local_to_world_offset(mv.normal, obj.basis);
            const float len_sq = wn.x * wn.x + wn.y * wn.y + wn.z * wn.z;
            if (len_sq > 0.01f) {
                verts[i].light = light_for_normal(wn.x, wn.y, wn.z);
            } else {
                verts[i].light = -1.0f;
            }
        }

        const auto draw_range = [&](std::size_t start, std::size_t count, const TextureImage* tex,
                                    float block_opacity) {
            const std::size_t end = std::min(start + count, mesh->triangles.size());
            for (std::size_t t = start; t < end; ++t) {
                const auto& idx = mesh->triangles[t];
                if (idx[0] >= verts.size() || idx[1] >= verts.size() || idx[2] >= verts.size()) {
                    continue;
                }
                ClipVertex a = verts[idx[0]];
                ClipVertex b = verts[idx[1]];
                ClipVertex c = verts[idx[2]];
                a.opacity = b.opacity = c.opacity = block_opacity;
                if (a.z < kNearPlane && b.z < kNearPlane && c.z < kNearPlane) {
                    continue;
                }
                if (a.light < 0.0f || b.light < 0.0f || c.light < 0.0f) {
                    const Vector3& p0 = world_pts[idx[0]];
                    const Vector3& p1 = world_pts[idx[1]];
                    const Vector3& p2 = world_pts[idx[2]];
                    const float e1x = p1.x - p0.x, e1y = p1.y - p0.y, e1z = p1.z - p0.z;
                    const float e2x = p2.x - p0.x, e2y = p2.y - p0.y, e2z = p2.z - p0.z;
                    const float light = light_for_normal(
                        e1y * e2z - e1z * e2y, e1z * e2x - e1x * e2z, e1x * e2y - e1y * e2x);
                    const float cl = std::max(light, 0.55f);
                    if (a.light < 0.0f) a.light = cl;
                    if (b.light < 0.0f) b.light = cl;
                    if (c.light < 0.0f) c.light = cl;
                }
                draw_clipped_triangle(a, b, c, tex, focal);
            }
        };

        const auto texture_for_slot = [&](int slot) -> const TextureImage* {
            if (slot < 0 || static_cast<std::size_t>(slot) >= mesh->slot_texture_names.size()) {
                return nullptr;
            }
            return world.find_texture(mesh->slot_texture_names[static_cast<std::size_t>(slot)]);
        };

        if (mesh->sub_blocks.empty()) {
            draw_range(0, mesh->triangles.size(), texture_for_slot(0), 1.0f);
        } else {
            for (const MeshSubBlock& block : mesh->sub_blocks) {
                draw_range(block.triangle_start, block.triangle_count,
                           texture_for_slot(block.material_slot),
                           std::clamp(block.opacity, 0.0f, 1.0f));
            }
        }
    }

    // Render soldiers with state cues (patrol camo, combat alert, or fallen corpse)
    static const TextureImage kCombatTexture = [] {
        TextureImage t;
        t.name = "soldier_combat";
        t.width = 1;
        t.height = 1;
        t.argb_pixels = {0xFFD03020u};
        return t;
    }();
    static const TextureImage kPatrolTexture = [] {
        TextureImage t;
        t.name = "soldier_patrol";
        t.width = 1;
        t.height = 1;
        t.argb_pixels = {0xFF4A6838u};
        return t;
    }();
    static const TextureImage kDeadTexture = [] {
        TextureImage t;
        t.name = "soldier_dead";
        t.width = 1;
        t.height = 1;
        t.argb_pixels = {0xFF382824u};
        return t;
    }();

    // Multi-part soldier rendering with animated walking legs, torso, helmet, aiming rifle, and muzzle flash
    for (const SoldierRenderData& s : soldiers) {
        if (!s.is_alive) {
            const float corpse_len = 4500.0f;
            const float corpse_w = 1200.0f;
            const double fx = static_cast<double>(std::cos(s.yaw)) * corpse_len;
            const double fy = static_cast<double>(std::sin(s.yaw)) * corpse_len;
            const double rx = static_cast<double>(basis.right.x) * corpse_w * 0.5;
            const double ry = static_cast<double>(basis.right.y) * corpse_w * 0.5;
            ClipVertex bl = world_to_clip(s.position.x - rx, s.position.y - ry, s.position.z + 200.0, 0.0f, 1.0f);
            ClipVertex br = world_to_clip(s.position.x + rx, s.position.y + ry, s.position.z + 200.0, 1.0f, 1.0f);
            ClipVertex tl = world_to_clip(s.position.x + fx - rx, s.position.y + fy - ry, s.position.z + 200.0, 0.0f, 0.0f);
            ClipVertex tr = world_to_clip(s.position.x + fx + rx, s.position.y + fy + ry, s.position.z + 200.0, 1.0f, 0.0f);
            bl.light = br.light = tl.light = tr.light = 0.55f;
            draw_clipped_triangle(bl, br, tr, &kDeadTexture, focal);
            draw_clipped_triangle(bl, tr, tl, &kDeadTexture, focal);
            continue;
        }

        const TextureImage* uniform_tex = s.is_combat ? &kCombatTexture : &kPatrolTexture;
        const double rx = static_cast<double>(basis.right.x);
        const double ry = static_cast<double>(basis.right.y);

        // 1. Torso & Tactical Vest (Z: 3200 to 5800, width: 900)
        {
            const float tw = 900.0f;
            ClipVertex bl = world_to_clip(s.position.x - rx * tw, s.position.y - ry * tw, s.position.z + 3200.0, 0.0f, 1.0f);
            ClipVertex br = world_to_clip(s.position.x + rx * tw, s.position.y + ry * tw, s.position.z + 3200.0, 1.0f, 1.0f);
            ClipVertex tl = world_to_clip(s.position.x - rx * tw, s.position.y - ry * tw, s.position.z + 5800.0, 0.0f, 0.0f);
            ClipVertex tr = world_to_clip(s.position.x + rx * tw, s.position.y + ry * tw, s.position.z + 5800.0, 1.0f, 0.0f);
            bl.light = br.light = tl.light = tr.light = 0.95f;
            draw_clipped_triangle(bl, br, tr, uniform_tex, focal);
            draw_clipped_triangle(bl, tr, tl, uniform_tex, focal);
        }

        // 2. Helmet & Head (Z: 5800 to 7200, width: 550)
        {
            const float hw = 550.0f;
            ClipVertex bl = world_to_clip(s.position.x - rx * hw, s.position.y - ry * hw, s.position.z + 5800.0, 0.0f, 1.0f);
            ClipVertex br = world_to_clip(s.position.x + rx * hw, s.position.y + ry * hw, s.position.z + 5800.0, 1.0f, 1.0f);
            ClipVertex tl = world_to_clip(s.position.x - rx * hw, s.position.y - ry * hw, s.position.z + 7200.0, 0.0f, 0.0f);
            ClipVertex tr = world_to_clip(s.position.x + rx * hw, s.position.y + ry * hw, s.position.z + 7200.0, 1.0f, 0.0f);
            bl.light = br.light = tl.light = tr.light = 1.05f;
            draw_clipped_triangle(bl, br, tr, uniform_tex, focal);
            draw_clipped_triangle(bl, tr, tl, uniform_tex, focal);
        }

        // 3. Animated Walking Legs (Z: 0 to 3200, swinging forward/backward with walk_phase)
        {
            const float leg_w = 350.0f;
            const float swing = std::sin(s.walk_phase) * 800.0f;
            const double fwd_x = std::cos(s.yaw);
            const double fwd_y = std::sin(s.yaw);

            // Left leg
            const double ll_fx = fwd_x * swing - rx * 450.0;
            const double ll_fy = fwd_y * swing - ry * 450.0;
            ClipVertex ll_bl = world_to_clip(s.position.x + ll_fx - rx * leg_w, s.position.y + ll_fy - ry * leg_w, s.position.z, 0.0f, 1.0f);
            ClipVertex ll_br = world_to_clip(s.position.x + ll_fx + rx * leg_w, s.position.y + ll_fy + ry * leg_w, s.position.z, 1.0f, 1.0f);
            ClipVertex ll_tl = world_to_clip(s.position.x - rx * (450.0 + leg_w), s.position.y - ry * (450.0 + leg_w), s.position.z + 3200.0, 0.0f, 0.0f);
            ClipVertex ll_tr = world_to_clip(s.position.x - rx * (450.0 - leg_w), s.position.y - ry * (450.0 - leg_w), s.position.z + 3200.0, 1.0f, 0.0f);
            ll_bl.light = ll_br.light = ll_tl.light = ll_tr.light = 0.85f;
            draw_clipped_triangle(ll_bl, ll_br, ll_tr, uniform_tex, focal);
            draw_clipped_triangle(ll_bl, ll_tr, ll_tl, uniform_tex, focal);

            // Right leg
            const double rl_fx = -fwd_x * swing + rx * 450.0;
            const double rl_fy = -fwd_y * swing + ry * 450.0;
            ClipVertex rl_bl = world_to_clip(s.position.x + rl_fx - rx * leg_w, s.position.y + rl_fy - ry * leg_w, s.position.z, 0.0f, 1.0f);
            ClipVertex rl_br = world_to_clip(s.position.x + rl_fx + rx * leg_w, s.position.y + rl_fy + ry * leg_w, s.position.z, 1.0f, 1.0f);
            ClipVertex rl_tl = world_to_clip(s.position.x + rx * (450.0 - leg_w), s.position.y + ry * (450.0 - leg_w), s.position.z + 3200.0, 0.0f, 0.0f);
            ClipVertex rl_tr = world_to_clip(s.position.x + rx * (450.0 + leg_w), s.position.y + ry * (450.0 + leg_w), s.position.z + 3200.0, 1.0f, 0.0f);
            rl_bl.light = rl_br.light = rl_tl.light = rl_tr.light = 0.85f;
            draw_clipped_triangle(rl_bl, rl_br, rl_tr, uniform_tex, focal);
            draw_clipped_triangle(rl_bl, rl_tr, rl_tl, uniform_tex, focal);
        }

        // 4. Raised Rifle pointing in s.yaw direction
        {
            const double fwd_x = std::cos(s.yaw);
            const double fwd_y = std::sin(s.yaw);
            const double gun_ox = s.position.x + rx * 300.0;
            const double gun_oy = s.position.y + ry * 300.0;
            const double gun_oz = s.position.z + 4600.0;
            const double tip_x = gun_ox + fwd_x * 1600.0;
            const double tip_y = gun_oy + fwd_y * 1600.0;

            ClipVertex g_bl = world_to_clip(gun_ox, gun_oy, gun_oz - 120.0, 0.0f, 1.0f);
            ClipVertex g_br = world_to_clip(tip_x, tip_y, gun_oz - 120.0, 1.0f, 1.0f);
            ClipVertex g_tl = world_to_clip(gun_ox, gun_oy, gun_oz + 120.0, 0.0f, 0.0f);
            ClipVertex g_tr = world_to_clip(tip_x, tip_y, gun_oz + 120.0, 1.0f, 0.0f);
            g_bl.light = g_br.light = g_tl.light = g_tr.light = 0.70f;
            draw_clipped_triangle(g_bl, g_br, g_tr, &kDeadTexture, focal);
            draw_clipped_triangle(g_bl, g_tr, g_tl, &kDeadTexture, focal);

            // 5. Enemy Muzzle Flash when firing
            if (s.is_firing) {
                const float mfl = 700.0f;
                ClipVertex mf_bl = world_to_clip(tip_x - rx * mfl, tip_y - ry * mfl, gun_oz - mfl, 0.0f, 1.0f);
                ClipVertex mf_br = world_to_clip(tip_x + rx * mfl, tip_y + ry * mfl, gun_oz - mfl, 1.0f, 1.0f);
                ClipVertex mf_tl = world_to_clip(tip_x - rx * mfl, tip_y - ry * mfl, gun_oz + mfl, 0.0f, 0.0f);
                ClipVertex mf_tr = world_to_clip(tip_x + rx * mfl, tip_y + ry * mfl, gun_oz + mfl, 1.0f, 0.0f);
                mf_bl.light = mf_br.light = mf_tl.light = mf_tr.light = 2.0f;
                static const TextureImage kMuzzleTex = [] {
                    TextureImage t;
                    t.name = "soldier_flash";
                    t.width = 1;
                    t.height = 1;
                    t.argb_pixels = {0xFFFFF060u};
                    return t;
                }();
                draw_clipped_triangle(mf_bl, mf_br, mf_tr, &kMuzzleTex, focal);
                draw_clipped_triangle(mf_bl, mf_tr, mf_tl, &kMuzzleTex, focal);
            }
        }
    }
}

void SoftwareRenderer::render_first_person_weapon(
    const HudState& hud,
    float bob_phase,
    float sway_yaw,
    float sway_pitch) {
    if (width_ == 0 || height_ == 0 || hud.is_sniper_scope) {
        return;
    }
    const int w = static_cast<int>(width_);
    const int h = static_cast<int>(height_);

    // Compute weapon bobbing and sway offsets in screen pixels
    const float bob_x = std::cos(bob_phase * 0.5f) * (static_cast<float>(w) * 0.012f);
    const float bob_y = std::abs(std::sin(bob_phase)) * (static_cast<float>(h) * 0.016f);
    const float sway_x = std::clamp(sway_yaw * 25.0f, -30.0f, 30.0f) * (static_cast<float>(w) / 480.0f);
    const float sway_y = std::clamp(sway_pitch * 20.0f, -25.0f, 25.0f) * (static_cast<float>(h) / 300.0f);

    // Recoil kickback translation
    const float recoil_y = hud.muzzle_flash * (static_cast<float>(h) * 0.05f);
    const float recoil_x = hud.muzzle_flash * (static_cast<float>(w) * 0.008f);

    // Anchor point for weapon base (bottom right)
    const int base_x = static_cast<int>(static_cast<float>(w) * 0.64f + bob_x + sway_x - recoil_x);
    const int base_y = static_cast<int>(static_cast<float>(h) * 0.52f + bob_y + sway_y - recoil_y);

    const std::string& name = hud.weapon_name;

    const bool is_ak = name.find("AK") != std::string::npos;
    const bool is_m16 = name.find("M16") != std::string::npos;
    const bool is_mp5 = name.find("MP5") != std::string::npos;
    const bool is_dragunov = name.find("Drag") != std::string::npos;
    const bool is_glock = name.find("Glock") != std::string::npos;

    const int scale = std::max(1, h / 300);
    const int gun_w = (is_glock ? 32 : 54) * scale;
    const int gun_h = (is_glock ? 90 : 150) * scale;
    const int barrel_w = (is_glock ? 18 : (is_mp5 ? 24 : 14)) * scale;
    const int barrel_len = (is_glock ? 45 : (is_dragunov ? 110 : 80)) * scale;

    const int gx0 = base_x;
    const int gy0 = base_y;

    // 1. Hands / Glove (tactical glove holding the weapon)
    fill_rect(gx0 - 12 * scale, gy0 + 40 * scale, gx0 + 38 * scale, gy0 + 110 * scale, 0xFF1C221Au);
    fill_rect(gx0 - 8 * scale, gy0 + 50 * scale, gx0 + 32 * scale, gy0 + 95 * scale, 0xFF283024u);

    // 2. Weapon Receiver / Slide
    const std::uint32_t steel_main = is_glock ? 0xFF222426u : 0xFF2A2E32u;
    const std::uint32_t steel_dark = 0xFF16181Au;
    const std::uint32_t steel_highlight = 0xFF42484Eu;

    fill_rect(gx0, gy0, gx0 + gun_w, gy0 + gun_h, steel_main);
    fill_rect(gx0, gy0, gx0 + 3 * scale, gy0 + gun_h, steel_dark);
    fill_rect(gx0 + gun_w - 3 * scale, gy0, gx0 + gun_w, gy0 + gun_h, steel_highlight);

    // 3. Barrel & Handguard / Furniture
    const int bx0 = gx0 + (gun_w - barrel_w) / 2;
    const int by0 = gy0 - barrel_len;

    if (is_ak || is_dragunov) {
        const std::uint32_t wood_base = 0xFF7A3618u;
        const std::uint32_t wood_light = 0xFF964B22u;
        const std::uint32_t wood_dark = 0xFF58240Eu;
        fill_rect(bx0 - 4 * scale, gy0 - 35 * scale, bx0 + barrel_w + 4 * scale, gy0 + 5 * scale, wood_base);
        fill_rect(bx0 - 2 * scale, gy0 - 32 * scale, bx0 + barrel_w + 2 * scale, gy0 - 20 * scale, wood_light);
        fill_rect(bx0 - 4 * scale, gy0 - 5 * scale, bx0 + barrel_w + 4 * scale, gy0 + 5 * scale, wood_dark);
        fill_rect(bx0 + 2 * scale, by0, bx0 + barrel_w - 2 * scale, gy0 - 35 * scale, steel_main);
    } else if (is_mp5) {
        fill_rect(bx0 - 5 * scale, by0, bx0 + barrel_w + 5 * scale, gy0, 0xFF1E2022u);
        for (int rib = by0 + 4 * scale; rib < gy0 - 4 * scale; rib += 6 * scale) {
            fill_rect(bx0 - 5 * scale, rib, bx0 + barrel_w + 5 * scale, rib + 2 * scale, 0xFF121415u);
        }
    } else if (is_m16) {
        fill_rect(bx0 - 3 * scale, gy0 - 45 * scale, bx0 + barrel_w + 3 * scale, gy0, 0xFF1E2224u);
        for (int rib = gy0 - 42 * scale; rib < gy0 - 5 * scale; rib += 5 * scale) {
            fill_rect(bx0 - 3 * scale, rib, bx0 + barrel_w + 3 * scale, rib + 1 * scale, 0xFF121416u);
        }
        fill_rect(bx0 + 1 * scale, by0, bx0 + barrel_w - 1 * scale, gy0 - 45 * scale, steel_main);
        fill_rect(gx0 + 10 * scale, gy0 - 16 * scale, gx0 + 36 * scale, gy0, 0xFF202428u);
    } else {
        fill_rect(bx0, by0, bx0 + barrel_w, gy0, steel_main);
        fill_rect(bx0 + 1 * scale, by0, bx0 + 3 * scale, gy0, steel_highlight);
    }

    // 4. Front Iron Sight Post
    fill_rect(bx0 + barrel_w / 2 - 1 * scale, by0 - 6 * scale, bx0 + barrel_w / 2 + 2 * scale, by0, 0xFF101214u);
    fill_rect(bx0 + barrel_w / 2, by0 - 5 * scale, bx0 + barrel_w / 2 + 1 * scale, by0 - 2 * scale, 0xFFFFFFFFu);

    // 5. Magazine / Magazine Well
    if (!is_glock) {
        const int mag_w = 16 * scale;
        const int mag_h = 45 * scale;
        const int mx0 = gx0 + 8 * scale;
        const int my0 = gy0 + 45 * scale;
        const std::uint32_t mag_col = is_ak ? 0xFF3E2C22u : 0xFF181B1Du;
        fill_rect(mx0, my0, mx0 + mag_w, my0 + mag_h, mag_col);
    }

    const int muzzle_x = bx0 + barrel_w / 2;
    const int muzzle_y = by0 - 4 * scale;

    // 6. Brass Ejected Shell Casing
    if (hud.muzzle_flash > 0.15f) {
        const float t = 1.0f - hud.muzzle_flash;
        const int case_x = gx0 + gun_w + static_cast<int>(t * 55.0f * static_cast<float>(scale));
        const int case_y = gy0 + 10 * scale + static_cast<int>((t * 40.0f - 8.0f) * static_cast<float>(scale));
        const int case_w = 4 * scale;
        const int case_h = 7 * scale;
        fill_rect(case_x, case_y, case_x + case_w, case_y + case_h, 0xFFE0B830u);
        fill_rect(case_x + 1, case_y + 1, case_x + case_w - 1, case_y + case_h - 1, 0xFFFFF070u);
    }

    // 7. Dynamic First-Person 8-point Starburst Muzzle Flash
    if (hud.muzzle_flash > 0.0f) {
        const float intensity = std::min(1.0f, hud.muzzle_flash);
        const int radius = static_cast<int>(32.0f * intensity * static_cast<float>(scale));

        const int cr = std::max(2, radius / 3);
        fill_rect(muzzle_x - cr, muzzle_y - cr, muzzle_x + cr + 1, muzzle_y + cr + 1, 0xFFFFFFFFu);

        for (int d = -radius; d <= radius; ++d) {
            const int span = radius - std::abs(d);
            const int py = muzzle_y + d;
            if (py >= 0 && py < h) {
                const int px0 = std::max(0, muzzle_x - span / 2);
                const int px1 = std::min(w - 1, muzzle_x + span / 2);
                for (int px = px0; px <= px1; ++px) {
                    const std::size_t idx = static_cast<std::size_t>(py) * width_ + static_cast<std::size_t>(px);
                    color_[idx] = blend(color_[idx], 0xFFFFE050u, 0.75f * intensity);
                }
            }
        }

        const int spike_len = static_cast<int>(42.0f * intensity * static_cast<float>(scale));
        fill_rect(muzzle_x - spike_len, muzzle_y - 1, muzzle_x + spike_len + 1, muzzle_y + 2, 0xFFFFFA80u);
        fill_rect(muzzle_x - 1, muzzle_y - spike_len, muzzle_x + 2, muzzle_y + spike_len + 1, 0xFFFFFA80u);
        const int diag_len = spike_len * 7 / 10;
        for (int step = -diag_len; step <= diag_len; ++step) {
            const int px = muzzle_x + step;
            const int py1 = muzzle_y + step;
            const int py2 = muzzle_y - step;
            if (px >= 0 && px < w) {
                if (py1 >= 0 && py1 < h) {
                    color_[static_cast<std::size_t>(py1) * width_ + static_cast<std::size_t>(px)] = 0xFFFFC040u;
                }
                if (py2 >= 0 && py2 < h) {
                    color_[static_cast<std::size_t>(py2) * width_ + static_cast<std::size_t>(px)] = 0xFFFFC040u;
                }
            }
        }
    }
}

void SoftwareRenderer::fill_rect(int x0, int y0, int x1, int y1, std::uint32_t argb) {
    x0 = std::max(0, x0);
    y0 = std::max(0, y0);
    x1 = std::min(static_cast<int>(width_), x1);
    y1 = std::min(static_cast<int>(height_), y1);
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            color_[static_cast<std::size_t>(y) * width_ + static_cast<std::size_t>(x)] = argb;
        }
    }
}

void SoftwareRenderer::render_hud(const HudState& hud) {
    if (width_ == 0 || height_ == 0) {
        return;
    }
    const int w = static_cast<int>(width_);
    const int h = static_cast<int>(height_);
    const int cx = w / 2;
    const int cy = h / 2;
    const int margin = 12;

    // 1. Sniper Scope Overlay (for Dragunov zoom)
    if (hud.is_sniper_scope) {
        const float radius = static_cast<float>(std::min(w, h)) * 0.44f;
        const float r_sq = radius * radius;
        for (int y = 0; y < h; ++y) {
            const float dy = static_cast<float>(y - cy);
            for (int x = 0; x < w; ++x) {
                const float dx = static_cast<float>(x - cx);
                if (dx * dx + dy * dy > r_sq) {
                    color_[static_cast<std::size_t>(y) * width_ + static_cast<std::size_t>(x)] = 0xFF080808u;
                }
            }
        }
        // Scope crosshair lines
        const int r_int = static_cast<int>(radius);
        fill_rect(cx - r_int, cy, cx + r_int, cy + 1, 0xFF102018u);
        fill_rect(cx, cy - r_int, cx + 1, cy + r_int, 0xFF102018u);
        // Mil-dot tick marks along scope axes
        for (int step = -4; step <= 4; ++step) {
            if (step == 0) continue;
            const int offset = step * (r_int / 5);
            fill_rect(cx + offset, cy - 3, cx + offset + 1, cy + 4, 0xFF204030u);
            fill_rect(cx - 3, cy + offset, cx + 4, cy + offset + 1, 0xFF204030u);
        }
        // Center cross point
        const std::uint32_t reticle_color = hud.hit_marker_active
            ? (hud.is_headshot ? 0xFFFFD000u : 0xFFFF3030u)
            : 0xFF40D050u;
        fill_rect(cx - 2, cy - 2, cx + 3, cy + 3, reticle_color);

        // Zoom readout
        const std::string zoom_str = "MAG: " + std::to_string(static_cast<int>(hud.zoom_magnification * 10.0f) / 10) + "X";
        HudFont::draw_string(color_.data(), w, h, cx - r_int + 16, cy - r_int + 16, zoom_str, 0xFF40D060u, 1, true);
    } else {
        // Standard First-Person Crosshair
        const std::uint32_t cross = hud.hit_marker_active
            ? (hud.is_headshot ? 0xFFFFD000u : 0xFFFF3030u)
            : 0xFFE8FFE8u;
        fill_rect(cx - 8, cy, cx - 3, cy + 1, cross);
        fill_rect(cx + 4, cy, cx + 9, cy + 1, cross);
        fill_rect(cx, cy - 8, cx + 1, cy - 3, cross);
        fill_rect(cx, cy + 4, cx + 1, cy + 9, cross);
    }

    // 2. Health bar (bottom left) and text
    const int bar_w = w / 4;
    const int bar_h = std::max(6, h / 40);
    const float health = std::clamp(hud.health_fraction, 0.0f, 1.0f);
    fill_rect(margin - 2, h - margin - bar_h - 2, margin + bar_w + 2, h - margin + 2, 0xFF101010u);
    fill_rect(margin, h - margin - bar_h, margin + static_cast<int>(static_cast<float>(bar_w) * health),
              h - margin, health > 0.3f ? 0xFF38B848u : 0xFFD03020u);
    const std::string hp_text = "HP " + std::to_string(hud.health_percent) + "%";
    HudFont::draw_string(color_.data(), w, h, margin, h - margin - bar_h - 11, hp_text, 0xFFE0FFE0u, 1, true);

    // 3. Ammo bar (bottom right) and text
    const float ammo = std::clamp(hud.ammo_fraction, 0.0f, 1.0f);
    fill_rect(w - margin - bar_w - 2, h - margin - bar_h - 2, w - margin + 2, h - margin + 2, 0xFF101010u);
    fill_rect(w - margin - static_cast<int>(static_cast<float>(bar_w) * ammo), h - margin - bar_h,
              w - margin, h - margin, 0xFFE0B030u);
    const std::string ammo_text = hud.weapon_name + " [" + hud.fire_mode + "] " +
                                  std::to_string(hud.rounds_in_magazine) + "/" +
                                  std::to_string(hud.reserve_ammo);
    const int ammo_text_w = HudFont::string_width(ammo_text, 1);
    HudFont::draw_string(color_.data(), w, h, w - margin - ammo_text_w, h - margin - bar_h - 11,
                         ammo_text, 0xFFFFE080u, 1, true);

    // 4. Muzzle flash
    if (hud.muzzle_flash > 0.0f) {
        const int fl = static_cast<int>(static_cast<float>(h) * 0.08f * std::min(1.0f, hud.muzzle_flash));
        fill_rect(cx + w / 8 - fl, h - h / 4 - fl, cx + w / 8 + fl, h - h / 4 + fl, 0xFFFFE890u);
    }

    // 5. Top objective / status banner
    const int banner_y = margin;
    const int banner_h = std::max(18, bar_h + 6);
    if (hud.mission_failed) {
        fill_rect(margin - 2, banner_y - 2, w - margin + 2, banner_y + banner_h + 2, 0xFF101010u);
        fill_rect(margin, banner_y, w - margin, banner_y + banner_h, 0xFFB82020u);
        HudFont::draw_string(color_.data(), w, h, margin + 12, banner_y + 4,
                             "MISSION FAILED: KIA", 0xFFFFFFFFu, 1, true);
    } else if (hud.mission_success) {
        fill_rect(margin - 2, banner_y - 2, w - margin + 2, banner_y + banner_h + 2, 0xFF101010u);
        fill_rect(margin, banner_y, w - margin, banner_y + banner_h, 0xFF28B840u);
        HudFont::draw_string(color_.data(), w, h, margin + 12, banner_y + 4,
                             "MISSION ACCOMPLISHED", 0xFFFFFFFFu, 1, true);
    } else if (!hud.objective_text.empty()) {
        const int banner_x0 = w / 8;
        const int banner_x1 = 7 * w / 8;
        fill_rect(banner_x0 - 2, banner_y - 2, banner_x1 + 2, banner_y + banner_h + 2, 0xFF101010u);
        fill_rect(banner_x0, banner_y, banner_x1, banner_y + banner_h, 0xFF183038u);
        fill_rect(banner_x0, banner_y, banner_x0 + 4, banner_y + banner_h, 0xFF30D0A0u);
        HudFont::draw_string(color_.data(), w, h, banner_x0 + 12, banner_y + 5,
                             hud.objective_text, 0xFFD8F8F0u, 1, true);
    }

    // 6. Security Alarm Banner
    if (hud.is_alarm_active) {
        const std::string alarm_text = "[!] BASE SECURITY ALARM ACTIVE [!]";
        const int alarm_w = HudFont::string_width(alarm_text, 1);
        const int alarm_x = cx - alarm_w / 2;
        const int alarm_y = banner_y + banner_h + 8;
        fill_rect(alarm_x - 6, alarm_y - 2, alarm_x + alarm_w + 6, alarm_y + 11, 0xFF801010u);
        HudFont::draw_string(color_.data(), w, h, alarm_x, alarm_y, alarm_text, 0xFFFFD020u, 1, true);
    }

    // 7. Interaction Prompt (centered on screen)
    if (!hud.interaction_prompt.empty()) {
        const int prompt_w = HudFont::string_width(hud.interaction_prompt, 1);
        const int prompt_x = cx - prompt_w / 2;
        const int prompt_y = cy + 40;
        fill_rect(prompt_x - 6, prompt_y - 3, prompt_x + prompt_w + 6, prompt_y + 11, 0xCC101824u);
        HudFont::draw_string(color_.data(), w, h, prompt_x, prompt_y, hud.interaction_prompt, 0xFFFFFFFFu, 1, true);
    }
}

bool SoftwareRenderer::save_bmp(const std::filesystem::path& path) const {
    if (width_ == 0 || height_ == 0 || color_.empty()) {
        return false;
    }
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    const std::uint32_t row_bytes = width_ * 3;
    const std::uint32_t padding = (4 - (row_bytes % 4)) % 4;
    const std::uint32_t padded_row = row_bytes + padding;
    const std::uint32_t image_size = padded_row * height_;
    const std::uint32_t file_size = 54 + image_size;

    // 14-byte File Header
    const std::uint8_t file_header[14] = {
        'B', 'M',
        static_cast<std::uint8_t>(file_size & 0xFF),
        static_cast<std::uint8_t>((file_size >> 8) & 0xFF),
        static_cast<std::uint8_t>((file_size >> 16) & 0xFF),
        static_cast<std::uint8_t>((file_size >> 24) & 0xFF),
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    out.write(reinterpret_cast<const char*>(file_header), 14);

    // 40-byte DIB Header (BITMAPINFOHEADER)
    const std::uint8_t dib_header[40] = {
        40, 0, 0, 0,
        static_cast<std::uint8_t>(width_ & 0xFF),
        static_cast<std::uint8_t>((width_ >> 8) & 0xFF),
        static_cast<std::uint8_t>((width_ >> 16) & 0xFF),
        static_cast<std::uint8_t>((width_ >> 24) & 0xFF),
        static_cast<std::uint8_t>(height_ & 0xFF),
        static_cast<std::uint8_t>((height_ >> 8) & 0xFF),
        static_cast<std::uint8_t>((height_ >> 16) & 0xFF),
        static_cast<std::uint8_t>((height_ >> 24) & 0xFF),
        1, 0,
        24, 0,
        0, 0, 0, 0,
        static_cast<std::uint8_t>(image_size & 0xFF),
        static_cast<std::uint8_t>((image_size >> 8) & 0xFF),
        static_cast<std::uint8_t>((image_size >> 16) & 0xFF),
        static_cast<std::uint8_t>((image_size >> 24) & 0xFF),
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
    out.write(reinterpret_cast<const char*>(dib_header), 40);

    std::vector<std::uint8_t> row(padded_row, 0);
    for (int y = static_cast<int>(height_) - 1; y >= 0; --y) {
        for (std::uint32_t x = 0; x < width_; ++x) {
            const std::uint32_t argb = color_[static_cast<std::size_t>(y) * width_ + x];
            row[x * 3 + 0] = static_cast<std::uint8_t>(argb & 0xFF);         // Blue
            row[x * 3 + 1] = static_cast<std::uint8_t>((argb >> 8) & 0xFF);  // Green
            row[x * 3 + 2] = static_cast<std::uint8_t>((argb >> 16) & 0xFF); // Red
        }
        out.write(reinterpret_cast<const char*>(row.data()), padded_row);
    }
    return out.good();
}

} // namespace igi::gameplay
