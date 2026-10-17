#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/audio/sound_definition_registry.hpp"
#include "igi/audio/sound_system.hpp"
#include "igi/asset_io.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
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

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Usage: sound_system_test <original-game-directory>");
        }

        const std::filesystem::path game_directory(arguments[1]);
        const auto common_sounds = igi::assets::resolve_path(game_directory, "common/sounds");
        const auto mission_sounds = igi::assets::resolve_path(
            game_directory,
            "missions/location0/level1/sounds");

        igi::audio::AudioMixer mixer;
        igi::audio::SoundBank bank(mixer);
        igi::audio::SoundDefinitionRegistry definitions;
        require(definitions.SoundSystem_LoadForMission(game_directory, 1) == 431,
                "Level 1 must load its retail sound definitions");
        require(bank.SoundSystem_MountDirectory(common_sounds) > 300,
                "The common retail sound bank did not load");
        require(bank.SoundSystem_MountDirectory(mission_sounds) > 0,
                "The Level 1 retail sound bank did not load");

        igi::audio::SoundSystem sounds(mixer, bank, definitions);
        const std::uint32_t group_voice = sounds.SoundSystem_PlayDefinition("GUN SHOTS");
        require(group_voice == 0, "A retail sound group must not play as a direct sound");
        require(mixer.ActiveVoiceCount() == 0, "Rejecting a sound group must not allocate a voice");

        const std::uint32_t missing_voice = sounds.SoundSystem_PlayDefinition("missing_level_one_sound");
        require(missing_voice == 0, "An absent sound definition must not allocate a voice");

        igi::audio::RetailVoiceParameters looping_parameters;
        looping_parameters.looping = false;
        const std::uint32_t rifle_voice = sounds.SoundSystem_PlayDefinition(
            "ak47_loop",
            looping_parameters);
        require(rifle_voice != 0, "The retail AK-47 definition did not create a mixer voice");
        require(sounds.SoundSystem_IsPlaying(rifle_voice), "The retail AK-47 voice is not active");
        for (const std::string_view fall_sound_name : {"player_fall_1", "player_fall_2", "player_fall_3"})
        {
            const std::uint32_t fall_sound_voice = sounds.SoundSystem_PlayDefinition(fall_sound_name);
            require(fall_sound_voice != 0 && sounds.SoundSystem_IsPlaying(fall_sound_voice),
                    "A retail player fall sound must resolve to an active Level 1 bank voice");
        }

        std::array<std::int16_t, 512> mixed_samples{};
        mixer.MixFrames(mixed_samples);
        require(std::any_of(mixed_samples.begin(), mixed_samples.end(), [](std::int16_t sample)
        {
            return sample != 0;
        }), "The retail AK-47 voice produced no PCM output");
        require(sounds.SoundSystem_IsPlaying(rifle_voice), "The looping retail voice stopped at its buffer end");

        sounds.SoundSystem_Stop(rifle_voice);
        require(!sounds.SoundSystem_IsPlaying(rifle_voice), "Stopping the retail voice did not release it");

        std::cout << "Level 1 retail definition playback passed: direct sound lookup, non-sound rejection, "
                     "PCM mix, looping, and stop lifecycle\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Level 1 sound-system verification failed: " << exception.what() << '\n';
        return 1;
    }
}
