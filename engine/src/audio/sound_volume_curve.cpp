#include "igi/audio/sound_volume_curve.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace igi::audio
{
namespace
{

constexpr double retail_volume_gain_step = 0.0001;
constexpr double retail_volume_logarithm_scale = 1442.6950408888495;
constexpr long double retail_volume_floor = -10000.0L;

using RetailVolumeTable = std::array<std::int32_t, retail_volume_table_entry_count>;

// Original IGI.EXE sound-table native at VA 0x004952F0 builds the attenuation lookup table.
RetailVolumeTable SoundSystem_BuildRetailVolumeTable()
{
    RetailVolumeTable volume_table{};
    volume_table[0] = retail_volume_table_floor;

    for (std::size_t gain_index = 1; gain_index < retail_volume_table_entry_count; ++gain_index)
    {
        const long double linear_gain = static_cast<long double>(gain_index) *
                                        static_cast<long double>(retail_volume_gain_step);
        const long double base_two_logarithm = std::log2(linear_gain);
        const long double natural_logarithm = base_two_logarithm * std::numbers::ln2_v<long double>;
        const long double logarithmic_attenuation = natural_logarithm *
                                                     static_cast<long double>(retail_volume_logarithm_scale);
        const long double rounded_attenuation = std::nearbyint(logarithmic_attenuation);
        const long double bounded_attenuation = std::clamp(rounded_attenuation, retail_volume_floor, 0.0L);
        volume_table[gain_index] = static_cast<std::int32_t>(bounded_attenuation);
    }

    return volume_table;
}

const RetailVolumeTable& SoundSystem_GetRetailVolumeTable()
{
    static const RetailVolumeTable volume_table = SoundSystem_BuildRetailVolumeTable();
    return volume_table;
}

std::size_t SoundSystem_GetRetailVolumeTableIndex(float normalized_gain)
{
    if (!std::isfinite(normalized_gain))
    {
        throw std::invalid_argument("Retail sound gain must be finite");
    }

    const long double bounded_gain = std::clamp(static_cast<long double>(normalized_gain), 0.0L, 1.0L);
    const long double rounded_index = std::nearbyint(bounded_gain * 10000.0L);
    return static_cast<std::size_t>(rounded_index);
}

} // namespace

// Original IGI.EXE sound-voice native at VA 0x00496E60 quantizes gain before table lookup.
std::int32_t SoundSystem_GetRetailVolumeMillibels(float linear_gain)
{
    const std::size_t table_index = SoundSystem_GetRetailVolumeTableIndex(linear_gain);
    return SoundSystem_GetRetailVolumeTable()[table_index];
}

// Original IGI.EXE sound-voice native at VA 0x00496E60 derives signed stereo pan attenuation.
std::int32_t SoundSystem_GetRetailPanMillibels(float pan, bool reverse_stereo)
{
    if (!std::isfinite(pan))
    {
        throw std::invalid_argument("Retail sound pan must be finite");
    }

    const float bounded_pan = std::clamp(pan, -1.0f, 1.0f);
    const float unattenuated_channel_gain = 1.0f - std::abs(bounded_pan);
    const std::int32_t channel_attenuation = SoundSystem_GetRetailVolumeMillibels(unattenuated_channel_gain);
    const std::int32_t pan_direction = bounded_pan >= 0.0f ? 1 : -1;
    const std::int32_t stereo_direction = reverse_stereo ? -1 : 1;
    return pan_direction * stereo_direction * channel_attenuation;
}

// Original IGI.EXE sound-voice native at VA 0x00496E60 clamps playback rate to [0.5, 2.0].
float SoundSystem_ClampRetailPlaybackRate(float playback_rate)
{
    if (!(playback_rate >= 0.5f))
    {
        return 0.5f;
    }
    if (playback_rate > 2.0f)
    {
        return 2.0f;
    }
    return playback_rate;
}

// Original IGI.EXE voice update at VA 0x00496E60 linearly attenuates distance and hand-computes stereo pan.
RetailSpatialGainAndPan SoundSystem_SpatialiseRetailVoice(
    float linear_gain,
    RetailAudioPosition source_position_metres,
    RetailAudioPosition listener_position_metres,
    double listener_heading_radians,
    float minimum_distance_metres,
    float maximum_distance_metres)
{
    if (!std::isfinite(linear_gain) || !std::isfinite(source_position_metres.x) ||
        !std::isfinite(source_position_metres.y) || !std::isfinite(source_position_metres.z) ||
        !std::isfinite(listener_position_metres.x) || !std::isfinite(listener_position_metres.y) ||
        !std::isfinite(listener_position_metres.z) || !std::isfinite(listener_heading_radians) ||
        !std::isfinite(minimum_distance_metres) || !std::isfinite(maximum_distance_metres))
    {
        throw std::invalid_argument("Retail positional audio inputs must be finite");
    }

    const double delta_x = source_position_metres.x - listener_position_metres.x;
    const double delta_y = source_position_metres.y - listener_position_metres.y;
    const double delta_z = source_position_metres.z - listener_position_metres.z;
    const double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

    float attenuated_gain = linear_gain;
    if (distance > static_cast<double>(maximum_distance_metres))
    {
        attenuated_gain = 0.0F;
    }
    else if (distance >= static_cast<double>(minimum_distance_metres))
    {
        const double distance_span =
            static_cast<double>(maximum_distance_metres) - static_cast<double>(minimum_distance_metres);
        if (distance_span <= 0.0)
        {
            attenuated_gain = 0.0F;
        }
        else
        {
            const double remaining_gain =
                1.0 - (distance - static_cast<double>(minimum_distance_metres)) / distance_span;
            attenuated_gain = static_cast<float>(remaining_gain * static_cast<double>(linear_gain));
            attenuated_gain = std::clamp(attenuated_gain, 0.0F, 1.0F);
        }
    }

    const float distance_scale = minimum_distance_metres + maximum_distance_metres;
    float pan = 0.0F;
    if (distance > 0.0 && distance_scale > 0.0F)
    {
        const double local_lateral_position =
            delta_x * std::cos(listener_heading_radians) + delta_y * std::sin(listener_heading_radians);
        pan = static_cast<float>(local_lateral_position / static_cast<double>(distance_scale));
        pan = std::clamp(pan, -1.0F, 1.0F);
    }

    return {attenuated_gain, pan};
}

} // namespace igi::audio
