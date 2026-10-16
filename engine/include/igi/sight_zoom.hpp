#pragma once

#include <cmath>
#include <string>

namespace igi::gameplay {

enum class ZoomMode {
    None,
    IronSights,
    Mp5Sight,
    DragunovScope,
    Binoculars,
};

// Implements weapon sight and sniper scope field-of-view zooming.
// Conforms to retail MP5SD (0x473AF0) and Dragunov scope tangent glide formulas.
class SightZoom final {
public:
    static constexpr float kDefaultFovRadians = 1.2217305f; // 70 degrees
    static constexpr float kDefaultTanX = 0.700207538f;    // tan(35 deg)

    void set_weapon(const std::string& weapon_name) noexcept;
    void reset() noexcept;

    // Advances zoom state by one 30 Hz simulation tick.
    void tick(bool toggle_pressed, bool hold_down) noexcept;

    [[nodiscard]] bool is_active() const noexcept { return is_active_; }
    [[nodiscard]] ZoomMode mode() const noexcept { return mode_; }
    [[nodiscard]] float current_tan_x() const noexcept { return current_tan_x_; }
    [[nodiscard]] float current_fov_radians() const noexcept;
    [[nodiscard]] float magnification() const noexcept;

private:
    ZoomMode mode_{ZoomMode::None};
    bool is_active_{false};
    float current_tan_x_{kDefaultTanX};
    float min_tan_x_{kDefaultTanX};
    float zoom_in_factor_{0.8f};
    float zoom_out_factor_{1.25f};
};

} // namespace igi::gameplay
