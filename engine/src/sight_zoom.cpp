#include "igi/sight_zoom.hpp"

#include <algorithm>

namespace igi::gameplay {

void SightZoom::set_weapon(const std::string& weapon_name) noexcept {
    if (weapon_name == "Dragunov") {
        mode_ = ZoomMode::DragunovScope;
        min_tan_x_ = 0.1051f; // ~12 degrees FOV
        zoom_in_factor_ = 0.96f;
        zoom_out_factor_ = 1.04f;
    } else if (weapon_name == "MP5SD") {
        mode_ = ZoomMode::Mp5Sight;
        min_tan_x_ = 0.35f; // ~38 degrees FOV
        zoom_in_factor_ = 0.80f;
        zoom_out_factor_ = 1.25f;
    } else if (weapon_name == "Binoculars") {
        mode_ = ZoomMode::Binoculars;
        min_tan_x_ = 0.0875f; // ~10 degrees FOV
        zoom_in_factor_ = 0.94f;
        zoom_out_factor_ = 1.06f;
    } else {
        mode_ = ZoomMode::IronSights;
        min_tan_x_ = 0.55f; // ~57 degrees FOV
        zoom_in_factor_ = 0.92f;
        zoom_out_factor_ = 1.09f;
    }
    is_active_ = false;
    current_tan_x_ = kDefaultTanX;
}

void SightZoom::reset() noexcept {
    is_active_ = false;
    current_tan_x_ = kDefaultTanX;
}

void SightZoom::tick(bool toggle_pressed, bool hold_down) noexcept {
    if (mode_ == ZoomMode::None) {
        is_active_ = false;
        current_tan_x_ = kDefaultTanX;
        return;
    }

    if (mode_ == ZoomMode::Mp5Sight) {
        // MP5 toggles on rising edge of alternate fire
        if (toggle_pressed) {
            is_active_ = !is_active_;
        }
    } else {
        // Dragunov, binoculars, and iron sights engage while alternate fire is held
        is_active_ = hold_down;
    }

    if (is_active_) {
        if (current_tan_x_ > min_tan_x_) {
            current_tan_x_ = std::max(min_tan_x_, current_tan_x_ * zoom_in_factor_);
        }
    } else {
        if (current_tan_x_ < kDefaultTanX) {
            current_tan_x_ = std::min(kDefaultTanX, current_tan_x_ * zoom_out_factor_);
        }
    }
}

float SightZoom::current_fov_radians() const noexcept {
    return 2.0f * std::atan(current_tan_x_);
}

float SightZoom::magnification() const noexcept {
    if (current_tan_x_ <= 1.0e-5f) {
        return 1.0f;
    }
    return kDefaultTanX / current_tan_x_;
}

} // namespace igi::gameplay
