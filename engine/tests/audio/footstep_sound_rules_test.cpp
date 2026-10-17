#include "igi/audio/footstep_sound_rules.hpp"
#include "igi/game_material_registry.hpp"

#include <array>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

igi::assets::ScriptValue text_argument(std::string text)
{
    igi::assets::ScriptValue argument;
    argument.text = std::move(text);
    return argument;
}

igi::assets::ScriptValue numeric_argument(double number)
{
    igi::assets::ScriptValue argument;
    argument.number = number;
    return argument;
}

igi::assets::GameMaterialSourceDefinition synthetic_ground_material()
{
    igi::assets::GameMaterialSourceDefinition material;
    material.material_index = 1;
    material.source_arguments.resize(36);
    material.source_arguments[0] = text_argument("Ground");
    material.source_arguments[5] = numeric_argument(2.5);
    for (std::size_t sound_index = 0; sound_index < 6; ++sound_index)
    {
        material.source_arguments[6 + sound_index] =
            text_argument("walk_ground_" + std::to_string(sound_index + 1));
    }
    return material;
}

void verify_retail_cursor_cycle()
{
    constexpr std::array<std::int32_t, 12> expected_cycle{0, 1, 2, 3, 4, 5, 3, 1, 4, 2, 3, 5};
    const std::int32_t* actual_cycle = igi::audio::FootstepSoundRules::FootstepSoundRules_CursorCycle();
    for (std::size_t cursor = 0; cursor < expected_cycle.size(); ++cursor)
    {
        require(actual_cycle[cursor] == expected_cycle[cursor],
                "Footstep cursor sequence must match retail data at 0x0053F318");
    }
}

void verify_retained_material_sound_parameters()
{
    const auto material = synthetic_ground_material();
    const auto cue =
        igi::audio::FootstepSoundRules::FootstepSoundRules_Resolve(material, 7, 0.5F, true, true);
    require(cue.sound_slot == 1 && cue.next_cursor == 8,
            "The eighth retail cursor step must select sound slot 1 and advance to cursor 8");
    require(cue.material_sound_definition_name == "walk_ground_2",
            "Footstep sound must use the material QVM's selected walk definition");
    require(cue.rain_sound_definition_name == "walk_rain_2",
            "Retail rain footsteps must layer the matching one-based walk-rain definition");
    require(std::abs(cue.playback_rate - 1.05F) < 0.00001F,
            "Retail footstep pitch must use the 0.5 * random + 1.0 - 0.2 formula");
    require(std::abs(cue.volume_scale - 0.6666667F) < 0.000001F && !cue.should_notify_ai,
            "Crouched retail footsteps must reduce voice gain and suppress AI hearing");
    require(std::abs(cue.ai_hearing_radius_units - 10240.0F) < 0.001F,
            "Footstep AI hearing radius must use the material value multiplied by 4096 units");

    const auto walking_cue =
        igi::audio::FootstepSoundRules::FootstepSoundRules_Resolve(material, 0, 0.0F, false, false);
    require(walking_cue.playback_rate == 0.8F && walking_cue.volume_scale == 1.0F &&
                walking_cue.should_notify_ai && walking_cue.rain_sound_definition_name.empty(),
            "Standing dry footsteps must preserve retail pitch, volume, and AI-hearing branches");
}

void verify_invalid_inputs_are_rejected()
{
    auto material = synthetic_ground_material();
    const auto expect_invalid = [&](std::int32_t cursor, float random_value)
    {
        bool rejected = false;
        try
        {
            static_cast<void>(igi::audio::FootstepSoundRules::FootstepSoundRules_Resolve(
                material, cursor, random_value, false, false));
        }
        catch (const std::invalid_argument&)
        {
            rejected = true;
        }
        require(rejected, "Invalid footstep source state must be rejected");
    };

    expect_invalid(-1, 0.5F);
    expect_invalid(12, 0.5F);
    expect_invalid(0, std::numeric_limits<float>::quiet_NaN());
    expect_invalid(0, 1.0F);

    material.source_arguments.pop_back();
    bool malformed_material_rejected = false;
    try
    {
        static_cast<void>(
            igi::audio::FootstepSoundRules::FootstepSoundRules_Resolve(material, 0, 0.5F, false, false));
    }
    catch (const std::invalid_argument&)
    {
        malformed_material_rejected = true;
    }
    require(malformed_material_rejected, "Incomplete retail material rows must be rejected");
}

void verify_retail_game_material_rows(const std::filesystem::path& game_directory)
{
    igi::assets::GameMaterialRegistry material_registry;
    require(material_registry.GameMaterialRegistry_LoadForGame(game_directory) == 24,
            "The original material QVM must define 24 game material rows");

    const auto* ground_material = material_registry.GameMaterialRegistry_Find(1);
    require(ground_material != nullptr && ground_material->source_arguments.size() == 36,
            "The retail Ground row must expose all 36 DefineGameMaterial arguments");

    std::int32_t cursor = 0;
    constexpr std::array<std::int32_t, 12> expected_cycle{0, 1, 2, 3, 4, 5, 3, 1, 4, 2, 3, 5};
    for (std::size_t step = 0; step < expected_cycle.size(); ++step)
    {
        const auto cue = igi::audio::FootstepSoundRules::FootstepSoundRules_Resolve(
            *ground_material, cursor, 0.25F, false, false);
        const std::string expected_sound = "walk_ground_" + std::to_string(expected_cycle[step] + 1);
        require(cue.sound_slot == expected_cycle[step] &&
                    cue.material_sound_definition_name == expected_sound,
                "Footstep selection must resolve the exact retail Ground QVM definition");
        cursor = cue.next_cursor;
    }
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        verify_retail_cursor_cycle();
        verify_retained_material_sound_parameters();
        verify_invalid_inputs_are_rejected();
        if (argument_count > 1)
        {
            verify_retail_game_material_rows(arguments[1]);
        }

        std::cout << "Footstep sound rule checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Footstep sound rule check failed: " << exception.what() << '\n';
        return 1;
    }
}
