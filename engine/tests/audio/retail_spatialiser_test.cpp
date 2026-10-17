#include "igi/audio/sound_volume_curve.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace
{

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        throw std::runtime_error(std::string(message));
    }
}

void require_near(float actual_value, float expected_value, float tolerance, std::string_view message)
{
    require(std::abs(actual_value - expected_value) <= tolerance, message);
}

} // namespace

int main()
{
    try
    {
        using igi::audio::RetailAudioPosition;
        using igi::audio::SoundSystem_SpatialiseRetailVoice;

        const auto near_right = SoundSystem_SpatialiseRetailVoice(
            0.8F, {5.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0.0, 5.0F, 15.0F);
        require_near(near_right.linear_gain, 0.8F, 0.00001F,
                     "Retail gain must remain unchanged at the minimum distance");
        require_near(near_right.pan, 0.25F, 0.00001F,
                     "Retail pan must use lateral distance divided by minimum plus maximum range");

        const auto mid_range = SoundSystem_SpatialiseRetailVoice(
            1.0F, {0.0, 10.0, 0.0}, {0.0, 0.0, 0.0}, 0.0, 5.0F, 15.0F);
        require_near(mid_range.linear_gain, 0.5F, 0.00001F,
                     "Retail gain must fall linearly between minimum and maximum distance");
        require_near(mid_range.pan, 0.0F, 0.00001F,
                     "A source directly ahead must remain centred");

        const auto turned_listener = SoundSystem_SpatialiseRetailVoice(
            1.0F,
            {0.0, 10.0, 0.0},
            {0.0, 0.0, 0.0},
            1.5707963267948966,
            5.0F,
            15.0F);
        require_near(turned_listener.pan, 0.5F, 0.00001F,
                     "Retail pan must rotate with the listener eye basis");

        const auto beyond_range = SoundSystem_SpatialiseRetailVoice(
            1.0F, {0.0, 0.0, 16.0}, {0.0, 0.0, 0.0}, 0.0, 5.0F, 15.0F);
        require_near(beyond_range.linear_gain, 0.0F, 0.00001F,
                     "Retail positional audio must become silent beyond its maximum range");

        bool rejected_non_finite_position = false;
        try
        {
            static_cast<void>(SoundSystem_SpatialiseRetailVoice(
                1.0F,
                {std::numeric_limits<double>::infinity(), 0.0, 0.0},
                {0.0, 0.0, 0.0},
                0.0,
                1.0F,
                50.0F));
        }
        catch (const std::invalid_argument&)
        {
            rejected_non_finite_position = true;
        }
        require(rejected_non_finite_position,
                "Invalid positional audio coordinates must fail with a clear exception");

        std::cout << "Retail spatial audio passed: attenuation, pan, listener rotation, range cutoff, validation\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail spatial audio verification failed: " << exception.what() << '\n';
        return 1;
    }
}
