#include "igi/audio/footstep_sound_rules.hpp"

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace igi::audio
{
namespace
{

constexpr std::array<std::int32_t, FootstepSoundRules::cursor_cycle_length> retail_cursor_cycle{
    0, 1, 2, 3, 4, 5, 3, 1, 4, 2, 3, 5};
constexpr std::size_t material_name_argument_index = 0;
constexpr std::size_t material_hearing_radius_argument_index = 5;
constexpr std::size_t first_walk_sound_argument_index = 6;
constexpr std::size_t expected_material_argument_count = 36;
constexpr std::string_view rain_sound_prefix = "walk_rain_";

// The retail callback reads its walk table at material + 0x54 and its hearing radius at +0x50.
// These source-argument indices follow the C# candidate parser; the callback's field stores remain unverified.

const assets::ScriptValue&
FootstepSoundRules_RequireMaterialArgument(const assets::GameMaterialSourceDefinition& material_definition,
                                           std::size_t argument_index)
{
    if (argument_index >= material_definition.source_arguments.size())
    {
        throw std::invalid_argument("Footstep material is missing a source argument");
    }
    return material_definition.source_arguments[argument_index];
}

std::string FootstepSoundRules_ReadSoundName(const assets::GameMaterialSourceDefinition& material_definition,
                                             std::size_t argument_index)
{
    const assets::ScriptValue& argument =
        FootstepSoundRules_RequireMaterialArgument(material_definition, argument_index);
    if (argument.call_index != std::numeric_limits<std::size_t>::max() || !std::isfinite(argument.number))
    {
        throw std::invalid_argument("Footstep material contains an invalid sound name argument");
    }
    return argument.text;
}

} // namespace

const std::int32_t* FootstepSoundRules::FootstepSoundRules_CursorCycle() noexcept
{
    return retail_cursor_cycle.data();
}

FootstepSoundCue FootstepSoundRules::FootstepSoundRules_Resolve(
    const assets::GameMaterialSourceDefinition& material_definition,
    std::int32_t cursor,
    float retail_random_value,
    bool crouched,
    bool weather_enabled)
{
    if (material_definition.source_arguments.size() != expected_material_argument_count)
    {
        throw std::invalid_argument(
            "Footstep material does not match the retail DefineGameMaterial argument layout");
    }
    if (cursor < 0 || cursor >= cursor_cycle_length)
    {
        throw std::invalid_argument("Footstep material cursor is outside the retail 12-step cycle");
    }
    if (!std::isfinite(retail_random_value) || retail_random_value < 0.0F || retail_random_value >= 1.0F)
    {
        throw std::invalid_argument("Footstep pitch input is outside the retail random range");
    }

    const assets::ScriptValue& material_name =
        FootstepSoundRules_RequireMaterialArgument(material_definition, material_name_argument_index);
    if (material_name.text.empty() || material_name.call_index != std::numeric_limits<std::size_t>::max())
    {
        throw std::invalid_argument("Footstep material has no valid retail material name");
    }

    const assets::ScriptValue& hearing_radius = FootstepSoundRules_RequireMaterialArgument(
        material_definition, material_hearing_radius_argument_index);
    if (!hearing_radius.text.empty() ||
        hearing_radius.call_index != std::numeric_limits<std::size_t>::max() ||
        !std::isfinite(hearing_radius.number))
    {
        throw std::invalid_argument("Footstep material has an invalid AI hearing radius");
    }

    const std::int32_t sound_slot = retail_cursor_cycle[static_cast<std::size_t>(cursor)];
    const std::string material_sound = FootstepSoundRules_ReadSoundName(
        material_definition, first_walk_sound_argument_index + static_cast<std::size_t>(sound_slot));

    FootstepSoundCue cue;
    cue.sound_slot = sound_slot;
    cue.next_cursor = (cursor + 1) % cursor_cycle_length;
    cue.material_sound_definition_name = material_sound;
    if (weather_enabled)
    {
        cue.rain_sound_definition_name = std::string(rain_sound_prefix) + std::to_string(sound_slot + 1);
    }
    cue.playback_rate = retail_random_value * 0.5F + 1.0F - 0.2F;
    cue.volume_scale = crouched ? crouched_volume_scale : 1.0F;
    cue.ai_hearing_radius_units = static_cast<float>(hearing_radius.number) * world_units_per_metre;
    cue.should_notify_ai = !crouched;
    return cue;
}

} // namespace igi::audio
