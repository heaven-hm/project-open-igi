#pragma once

#include <string>

namespace igi::gameplay {

struct PlayerFallImpact {
    float speed_meters_per_second = 0.0f;
    float damage = 0.0f;
    std::string sound_name;
    float view_kick_units = 0.0f;
    float hearing_radius_units = 0.0f;
};

// Calculates the complete set of effects produced by a vanilla landing.
// Follows verified OpenIGI HumanFallImpact equations calibrated for 30 Hz ticks.
PlayerFallImpact calculate_vanilla_fall_impact(
    float vertical_velocity_units_per_tick,
    float maximum_health);

} // namespace igi::gameplay
