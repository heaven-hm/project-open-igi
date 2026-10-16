#include "igi/software_renderer.hpp"

#include <algorithm>
#include <cmath>

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
            const float light = a.light;

            std::uint32_t texel = 0xFF8C9088u;
            if (texture != nullptr) {
                texel = texture->sample_nearest(u, v);
                if (texture->has_alpha && (texel >> 24) < 128u) {
                    continue;
                }
            }
            std::uint32_t shaded = modulate(texel, light);
            if (z > kFogStart) {
                shaded = blend(shaded, kFogColor, (z - kFogStart) / (kFogEnd - kFogStart));
            }
            depth_[index] = z;
            color_[index] = shaded;
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
            m.light = cur.light;
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
        const float light = light_for_normal(tri.normal.x, tri.normal.y, tri.normal.z);
        a.light = b.light = c.light = light;
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
        }

        const auto draw_range = [&](std::size_t start, std::size_t count, const TextureImage* tex) {
            const std::size_t end = std::min(start + count, mesh->triangles.size());
            for (std::size_t t = start; t < end; ++t) {
                const auto& idx = mesh->triangles[t];
                if (idx[0] >= verts.size() || idx[1] >= verts.size() || idx[2] >= verts.size()) {
                    continue;
                }
                ClipVertex a = verts[idx[0]];
                ClipVertex b = verts[idx[1]];
                ClipVertex c = verts[idx[2]];
                if (a.z < kNearPlane && b.z < kNearPlane && c.z < kNearPlane) {
                    continue;
                }
                const Vector3& p0 = world_pts[idx[0]];
                const Vector3& p1 = world_pts[idx[1]];
                const Vector3& p2 = world_pts[idx[2]];
                const float e1x = p1.x - p0.x, e1y = p1.y - p0.y, e1z = p1.z - p0.z;
                const float e2x = p2.x - p0.x, e2y = p2.y - p0.y, e2z = p2.z - p0.z;
                const float light = light_for_normal(
                    e1y * e2z - e1z * e2y, e1z * e2x - e1x * e2z, e1x * e2y - e1y * e2x);
                a.light = b.light = c.light = std::max(light, 0.55f);
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
            draw_range(0, mesh->triangles.size(), texture_for_slot(0));
        } else {
            for (const MeshSubBlock& block : mesh->sub_blocks) {
                draw_range(block.triangle_start, block.triangle_count,
                           texture_for_slot(block.material_slot));
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

    for (const SoldierRenderData& s : soldiers) {
        const float half_w = s.is_alive ? 1000.0f : 1500.0f;
        const float h = s.is_alive ? 7372.8f : 800.0f;
        const double rx = static_cast<double>(basis.right.x) * half_w;
        const double ry = static_cast<double>(basis.right.y) * half_w;
        ClipVertex bl = world_to_clip(s.position.x - rx, s.position.y - ry, s.position.z, 0.0f, 1.0f);
        ClipVertex br = world_to_clip(s.position.x + rx, s.position.y + ry, s.position.z, 1.0f, 1.0f);
        ClipVertex tl = world_to_clip(s.position.x - rx, s.position.y - ry, s.position.z + h, 0.0f, 0.0f);
        ClipVertex tr = world_to_clip(s.position.x + rx, s.position.y + ry, s.position.z + h, 1.0f, 0.0f);
        bl.light = br.light = tl.light = tr.light = 1.0f;
        const TextureImage* tex = !s.is_alive ? &kDeadTexture : (s.is_combat ? &kCombatTexture : &kPatrolTexture);
        draw_clipped_triangle(bl, br, tr, tex, focal);
        draw_clipped_triangle(bl, tr, tl, tex, focal);
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

    // Crosshair
    const std::uint32_t cross = hud.hit_marker_active ? 0xFFFF3030u : 0xFFE8FFE8u;
    fill_rect(cx - 8, cy, cx - 3, cy + 1, cross);
    fill_rect(cx + 4, cy, cx + 9, cy + 1, cross);
    fill_rect(cx, cy - 8, cx + 1, cy - 3, cross);
    fill_rect(cx, cy + 4, cx + 1, cy + 9, cross);

    // Health (bottom left) and ammo (bottom right) bars
    const int bar_w = w / 4;
    const int bar_h = std::max(6, h / 40);
    const int margin = 12;
    const float health = std::clamp(hud.health_fraction, 0.0f, 1.0f);
    const float ammo = std::clamp(hud.ammo_fraction, 0.0f, 1.0f);
    fill_rect(margin - 2, h - margin - bar_h - 2, margin + bar_w + 2, h - margin + 2, 0xFF101010u);
    fill_rect(margin, h - margin - bar_h, margin + static_cast<int>(static_cast<float>(bar_w) * health),
              h - margin, health > 0.3f ? 0xFF38B848u : 0xFFD03020u);
    fill_rect(w - margin - bar_w - 2, h - margin - bar_h - 2, w - margin + 2, h - margin + 2, 0xFF101010u);
    fill_rect(w - margin - static_cast<int>(static_cast<float>(bar_w) * ammo), h - margin - bar_h,
              w - margin, h - margin, 0xFFE0B030u);

    if (hud.muzzle_flash > 0.0f) {
        const int fl = static_cast<int>(static_cast<float>(h) * 0.08f * std::min(1.0f, hud.muzzle_flash));
        fill_rect(cx + w / 8 - fl, h - h / 4 - fl, cx + w / 8 + fl, h - h / 4 + fl, 0xFFFFE890u);
    }

    // Top objective banner
    if (hud.mission_failed) {
        fill_rect(margin, margin, w - margin, margin + bar_h + 2, 0xFFB82020u);
    } else if (hud.mission_success) {
        fill_rect(margin, margin, w - margin, margin + bar_h + 2, 0xFF28B840u);
    } else if (!hud.objective_text.empty()) {
        const int banner_x0 = w / 4;
        const int banner_x1 = 3 * w / 4;
        fill_rect(banner_x0 - 2, margin - 2, banner_x1 + 2, margin + bar_h + 2, 0xFF101010u);
        fill_rect(banner_x0, margin, banner_x1, margin + bar_h, 0xFF183038u);
        fill_rect(banner_x0, margin, banner_x0 + 4, margin + bar_h, 0xFF30D0A0u);
    }
}

} // namespace igi::gameplay
