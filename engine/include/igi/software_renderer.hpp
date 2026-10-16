#pragma once

#include "igi/human_motion.hpp"
#include "igi/mission_world.hpp"

#include <cstdint>
#include <vector>

namespace igi::gameplay {

struct CameraView {
    NavigationPosition eye{};
    float yaw{};
    float pitch{};
    float vertical_fov_radians{1.2217305f}; // 70 degrees
};

struct HudState {
    float health_fraction{1.0f};
    int health_percent{100};
    float ammo_fraction{1.0f};
    int rounds_in_magazine{30};
    int reserve_ammo{120};
    std::string weapon_name{"M16A2"};
    std::string fire_mode{"BURST"};
    bool hit_marker_active{};
    bool is_headshot{};
    float muzzle_flash{};
    std::string objective_text{};
    std::string interaction_prompt{};
    bool mission_success{};
    bool mission_failed{};
    bool is_alarm_active{};
    bool is_sniper_scope{};
    float zoom_magnification{1.0f};
};

struct SoldierRenderData {
    NavigationPosition position{};
    float yaw{};
    bool is_alive{true};
    bool is_combat{false};
};

// Portable software rasterizer: writes ARGB8888 into an owned framebuffer.
// No OS or graphics API dependencies; a platform backend only blits pixels().
class SoftwareRenderer final {
public:
    void resize(std::uint32_t width, std::uint32_t height);

    [[nodiscard]] std::uint32_t width() const noexcept { return width_; }
    [[nodiscard]] std::uint32_t height() const noexcept { return height_; }
    [[nodiscard]] const std::vector<std::uint32_t>& pixels() const noexcept { return color_; }

    void render_world(
        const MissionWorld& world,
        const CameraView& camera,
        const std::vector<NavigationPosition>& soldier_positions);
    void render_world(
        const MissionWorld& world,
        const CameraView& camera,
        const std::vector<SoldierRenderData>& soldiers);
    void render_hud(const HudState& hud);
    void clear_with_sky(const CameraView& camera);

private:
    struct ClipVertex {
        float x{}, y{}, z{}; // camera space: x right, y up, z forward
        float u{}, v{};
        float light{1.0f};
    };

    void draw_triangle(
        const ClipVertex& a,
        const ClipVertex& b,
        const ClipVertex& c,
        const TextureImage* texture,
        float focal_px);
    void draw_clipped_triangle(
        ClipVertex a,
        ClipVertex b,
        ClipVertex c,
        const TextureImage* texture,
        float focal_px);
    void fill_rect(int x0, int y0, int x1, int y1, std::uint32_t argb);

    std::uint32_t width_{};
    std::uint32_t height_{};
    std::vector<std::uint32_t> color_;
    std::vector<float> depth_;
};

} // namespace igi::gameplay
