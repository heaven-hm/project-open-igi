#pragma once

#include "igi/vector_math.hpp"

#include <array>

namespace igi::retail_natives
{

class GunPickup final
{
  public:
    static constexpr double world_units_per_metre = 4096.0;
    static constexpr double minimum_pickup_distance_units = 2867.2;
    static constexpr double maximum_vertical_distance_units = 4096.0;

    // IGI.EXE 0x0046D230: port of the retail pickup-distance gate.
    // Axis dimensions are the native fields at +0x98, +0x9C, and +0xA0;
    // parent_scale is the result of the retail helper at 0x004D0950.
    [[nodiscard]] static bool GunPickup_Action(
        gameplay::Vector3 pickup_position_metres,
        gameplay::Vector3 human_feet_position_metres,
        const std::array<float, 3>& axis_dimensions,
        float parent_scale);
};

} // namespace igi::retail_natives
