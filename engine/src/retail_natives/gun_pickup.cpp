#include "igi/retail_natives/gun_pickup.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace igi::retail_natives
{

bool GunPickup::GunPickup_Action(
    gameplay::Vector3 pickup_position_metres,
    gameplay::Vector3 human_feet_position_metres,
    const std::array<float, 3>& axis_dimensions,
    float parent_scale)
{
    if (!std::isfinite(pickup_position_metres.x) || !std::isfinite(pickup_position_metres.y) ||
        !std::isfinite(pickup_position_metres.z) || !std::isfinite(human_feet_position_metres.x) ||
        !std::isfinite(human_feet_position_metres.y) || !std::isfinite(human_feet_position_metres.z) ||
        !std::isfinite(parent_scale) ||
        !std::all_of(axis_dimensions.begin(), axis_dimensions.end(), [](float dimension)
        {
            return std::isfinite(dimension);
        }))
    {
        throw std::invalid_argument("GunPickup_Action received non-finite retail state");
    }

    const float largest_axis_dimension =
        std::max({axis_dimensions[0], axis_dimensions[1], axis_dimensions[2]});
    const double scaled_distance_limit_units =
        static_cast<double>(largest_axis_dimension) * static_cast<double>(parent_scale);
    const double pickup_distance_limit_units =
        std::max(scaled_distance_limit_units, minimum_pickup_distance_units);

    const double difference_x_units =
        (static_cast<double>(pickup_position_metres.x) - human_feet_position_metres.x) * world_units_per_metre;
    const double difference_y_units =
        (static_cast<double>(pickup_position_metres.y) - human_feet_position_metres.y) * world_units_per_metre;
    const double difference_z_units =
        (static_cast<double>(pickup_position_metres.z) - human_feet_position_metres.z) * world_units_per_metre;
    const double absolute_vertical_distance_units = std::abs(difference_z_units);
    const double three_dimensional_distance_units =
        std::sqrt((difference_x_units * difference_x_units) +
                  (difference_y_units * difference_y_units) +
                  (difference_z_units * difference_z_units));

    return three_dimensional_distance_units < pickup_distance_limit_units &&
           absolute_vertical_distance_units < maximum_vertical_distance_units;
}

} // namespace igi::retail_natives
