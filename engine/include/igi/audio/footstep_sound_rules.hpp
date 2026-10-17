#pragma once

#include "igi/game_material_registry.hpp"

#include <cstdint>
#include <string>

namespace igi::audio
{

struct FootstepSoundCue final
{
    std::int32_t sound_slot{};
    std::int32_t next_cursor{};
    std::string material_sound_definition_name;
    std::string rain_sound_definition_name;
    float playback_rate = 1.0F;
    float volume_scale = 1.0F;
    float ai_hearing_radius_units{};
    bool should_notify_ai{};
};

class FootstepSoundRules final
{
  public:
    static constexpr std::int32_t sound_slot_count = 6;
    static constexpr std::int32_t cursor_cycle_length = 12;
    static constexpr float crouched_volume_scale = 0.6666667F;
    static constexpr float world_units_per_metre = 4096.0F;

    // IGI.EXE 0x00463920 reads this table at 0x0053F318; REA evidence ev_e6ec59af8083f39f5881533b539bd775d04c904bb089cc11e9b21865e1823297.
    [[nodiscard]] static const std::int32_t* FootstepSoundRules_CursorCycle() noexcept;

    // IGI.EXE 0x00463920 selects one of six material sounds using a per-human 12-step cursor.
    // The mapping from DefineGameMaterial source arguments to record fields remains C#-derived.
    [[nodiscard]] static FootstepSoundCue
    FootstepSoundRules_Resolve(const assets::GameMaterialSourceDefinition& material_definition,
                               std::int32_t cursor,
                               float retail_random_value,
                               bool crouched,
                               bool weather_enabled);
};

} // namespace igi::audio
