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

std::int32_t SoundSystem_GetRetailVolumeMillibels(float linear_gain)
{
    const std::size_t table_index = SoundSystem_GetRetailVolumeTableIndex(linear_gain);
    return SoundSystem_GetRetailVolumeTable()[table_index];
}

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

} // namespace igi::audio
