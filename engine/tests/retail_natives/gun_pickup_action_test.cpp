#include "igi/retail_natives/gun_pickup.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void verify_retail_pickup_distance_gate()
{
    using igi::retail_natives::GunPickup;

    constexpr std::array<float, 3> initialized_axis_dimensions{1.0F, 1.0F, 1.0F};
    require(GunPickup::GunPickup_Action({0.699F, 0.0F, 0.0F}, {}, initialized_axis_dimensions, 1.0F),
            "The retail minimum range must include a position just inside 2867.2 units");
    require(!GunPickup::GunPickup_Action({0.701F, 0.0F, 0.0F}, {}, initialized_axis_dimensions, 1.0F),
            "A position outside the retail minimum range must be rejected");
    require(!GunPickup::GunPickup_Action({0.5F, 0.0F, 0.5F}, {}, initialized_axis_dimensions, 1.0F),
            "Retail pickup range must use full three-dimensional distance, not planar distance");
    require(!GunPickup::GunPickup_Action({0.0F, 0.0F, 1.0F}, {}, initialized_axis_dimensions, 1.0F),
            "The retail vertical-distance comparison must remain strict at 4096 units");
    require(GunPickup::GunPickup_Action({0.0F, 0.0F, 0.5F}, {}, initialized_axis_dimensions, 1.0F),
            "A pickup inside both retail distance checks must be accepted");

    constexpr std::array<float, 3> extended_axis_dimensions{6000.0F, 1.0F, 1.0F};
    require(GunPickup::GunPickup_Action({1.0F, 0.0F, 0.0F}, {}, extended_axis_dimensions, 1.0F),
            "The retail scaled-axis range must exceed its 2867.2-unit lower bound");
    require(!GunPickup::GunPickup_Action({1.5F, 0.0F, 0.0F}, {}, extended_axis_dimensions, 1.0F),
            "The retail scaled-axis range must reject positions beyond its computed bound");
}

void verify_invalid_state_is_reported()
{
    using igi::retail_natives::GunPickup;

    constexpr std::array<float, 3> initialized_axis_dimensions{1.0F, 1.0F, 1.0F};
    bool invalid_position_rejected = false;
    try
    {
        static_cast<void>(GunPickup::GunPickup_Action(
            {std::numeric_limits<float>::quiet_NaN(), 0.0F, 0.0F}, {}, initialized_axis_dimensions, 1.0F));
    }
    catch (const std::invalid_argument&)
    {
        invalid_position_rejected = true;
    }
    require(invalid_position_rejected, "Non-finite pickup state must produce an explicit error");
}

} // namespace

int main()
{
    try
    {
        verify_retail_pickup_distance_gate();
        verify_invalid_state_is_reported();
        std::cout << "Retail GunPickup_Action distance gate passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail GunPickup_Action distance gate failed: " << exception.what() << '\n';
        return 1;
    }
}
