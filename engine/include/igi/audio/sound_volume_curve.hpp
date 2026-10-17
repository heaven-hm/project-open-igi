#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::audio
{

inline constexpr std::size_t retail_volume_table_entry_count = 10001;
inline constexpr std::int32_t retail_volume_table_floor = -10000;

// Original IGI.EXE sound initialization at VA 0x004952F0 builds this attenuation table.
// The voice update at VA 0x00496E60 rounds the gain and pan indices before reading the table.
// REA evidence: ev_045da563dcccf0041b8283b3778575c9bb911f60abeb72679def1d52c642f9b8
// and ev_eaf627906b6ba16171468863340666b1d9ece8c696014ed279d59cabbf2d34f5.
std::int32_t SoundSystem_GetRetailVolumeMillibels(float linear_gain);

// Original IGI.EXE voice update at VA 0x00496E60 applies signed table attenuation to stereo pan.
std::int32_t SoundSystem_GetRetailPanMillibels(float pan, bool reverse_stereo = false);

// Original IGI.EXE voice update at VA 0x00496E60 clamps sample playback rate to [0.5, 2.0].
float SoundSystem_ClampRetailPlaybackRate(float playback_rate);

} // namespace igi::audio
