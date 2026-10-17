#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::audio {

inline constexpr std::size_t sound_attenuation_table_entry_count = 10001U;
using SoundAttenuationTable = std::array<std::int32_t, sound_attenuation_table_entry_count>;

// Port of the attenuation-table portion of IGI.EXE FUN_004952F0.
// Its native name is unresolved; the catalog's Display_ClearSurfaces label conflicts with REA's
// observed DirectSound initialization and x87 attenuation-table instructions.
[[nodiscard]] SoundAttenuationTable Sound_BuildAttenuationTable_004952F0();

} // namespace igi::retail::audio
