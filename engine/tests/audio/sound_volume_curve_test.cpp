#include "igi/audio/sound_volume_curve.hpp"

#include <cmath>
#include <exception>
#include <iostream>
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

} // namespace

int main()
{
    try
    {
        using igi::audio::SoundSystem_ClampRetailPlaybackRate;
        using igi::audio::SoundSystem_GetRetailPanMillibels;
        using igi::audio::SoundSystem_GetRetailVolumeMillibels;
        using igi::audio::retail_volume_table_floor;

        require(SoundSystem_GetRetailVolumeMillibels(1.0f) == 0,
                "Retail full-scale gain must map to zero attenuation");
        require(SoundSystem_GetRetailVolumeMillibels(0.0f) == retail_volume_table_floor,
                "Retail silence must map to the table floor");
        require(SoundSystem_GetRetailVolumeMillibels(0.5f) == -1000,
                "Retail FISTP rounds the half-scale attenuation to negative one thousand");
        require(SoundSystem_GetRetailVolumeMillibels(0.25f) == -2000 &&
                    SoundSystem_GetRetailVolumeMillibels(0.125f) == -3000,
                "Retail logarithmic table entries must use x87 nearest rounding");
        require(SoundSystem_GetRetailVolumeMillibels(0.00096f) == -9966,
                "Retail voice update rounds gain times ten thousand before table lookup");
        require(SoundSystem_GetRetailVolumeMillibels(0.01f) == -6644 &&
                    SoundSystem_GetRetailVolumeMillibels(0.1234f) == -3019,
                "Retail table checkpoints must retain the recovered log scale and rounding");

        require(SoundSystem_GetRetailPanMillibels(0.0f) == 0,
                "A centered retail sound must apply no pan attenuation");
        require(SoundSystem_GetRetailPanMillibels(0.5f) == -1000 &&
                    SoundSystem_GetRetailPanMillibels(-0.5f) == 1000,
                "Retail pan must sign the attenuation of the opposite stereo channel");
        require(SoundSystem_GetRetailPanMillibels(0.5f, true) == 1000,
                "Retail reverse-stereo must invert the pan attenuation sign");

        require(SoundSystem_ClampRetailPlaybackRate(0.25f) == 0.5f &&
                    SoundSystem_ClampRetailPlaybackRate(1.0f) == 1.0f &&
                    SoundSystem_ClampRetailPlaybackRate(4.0f) == 2.0f,
                "Retail playback rate must clamp to the native half-to-double range");
        require(SoundSystem_ClampRetailPlaybackRate(std::nanf("")) == 0.5f,
                "Retail unordered playback-rate comparison takes the lower clamp branch");

        bool rejected_non_finite_gain = false;
        try
        {
            static_cast<void>(SoundSystem_GetRetailVolumeMillibels(std::nanf("")));
        }
        catch (const std::invalid_argument&)
        {
            rejected_non_finite_gain = true;
        }
        require(rejected_non_finite_gain, "Non-finite gains must fail with a controlled exception");

        bool rejected_non_finite_pan = false;
        try
        {
            static_cast<void>(SoundSystem_GetRetailPanMillibels(std::nanf("")));
        }
        catch (const std::invalid_argument&)
        {
            rejected_non_finite_pan = true;
        }
        require(rejected_non_finite_pan, "Non-finite pan must fail with a controlled exception");

        std::int32_t previous_attenuation = retail_volume_table_floor;
        for (int gain_index = 0; gain_index <= 10000; ++gain_index)
        {
            const float gain = static_cast<float>(gain_index) / 10000.0f;
            const std::int32_t attenuation = SoundSystem_GetRetailVolumeMillibels(gain);
            require(attenuation >= previous_attenuation,
                    "Retail attenuation table must be monotonic with gain");
            previous_attenuation = attenuation;
        }
        require(previous_attenuation == 0, "The retail table must end at full-scale zero attenuation");

        std::cout << "Retail IGI.EXE sound volume, pan, and playback-rate checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail sound volume curve check failed: " << exception.what() << '\n';
        return 1;
    }
}
