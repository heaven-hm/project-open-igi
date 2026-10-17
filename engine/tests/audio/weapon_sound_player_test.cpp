#include "igi/asset_io.hpp"
#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/audio/sound_definition_registry.hpp"
#include "igi/audio/sound_system.hpp"
#include "igi/mission_scene.hpp"
#include "igi/weapon_sound_player.hpp"

#include <filesystem>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace
{

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        throw std::runtime_error(std::string(message));
    }
}

std::uint64_t sum_channel_magnitude(const std::vector<std::int16_t>& interleaved_samples, std::size_t channel_index)
{
    std::uint64_t total_magnitude = 0;
    for (std::size_t sample_index = channel_index; sample_index < interleaved_samples.size(); sample_index += 2)
    {
        total_magnitude += static_cast<std::uint64_t>(
            std::abs(static_cast<std::int32_t>(interleaved_samples[sample_index])));
    }
    return total_magnitude;
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Usage: weapon_sound_player_test <original-game-directory>");
        }

        const std::filesystem::path game_directory(arguments[1]);
        const std::filesystem::path common_sound_directory =
            igi::assets::resolve_path(game_directory, "common/sounds");
        const std::filesystem::path mission_sound_directory =
            igi::assets::resolve_path(game_directory, "missions/location0/level1/sounds");

        igi::audio::AudioMixer audio_mixer;
        igi::audio::SoundBank sound_bank(audio_mixer);
        igi::audio::SoundDefinitionRegistry sound_definitions;
        require(sound_definitions.SoundSystem_LoadForMission(game_directory, 1) == 431,
                "Level 1 retail sound definitions did not load");
        require(sound_bank.SoundSystem_MountDirectory(common_sound_directory) > 300,
                "The common retail sound bank did not mount");
        require(sound_bank.SoundSystem_MountDirectory(mission_sound_directory) > 0,
                "The Level 1 retail sound bank did not mount");

        igi::audio::SoundSystem sound_system(audio_mixer, sound_bank, sound_definitions);
        igi::gameplay::WeaponSoundPlayer weapon_sound_player(sound_system);
        const igi::audio::SoundEntry* ak47_loop_sample = nullptr;
        require(sound_bank.SoundSystem_TryGet("ak47_loop", ak47_loop_sample) && ak47_loop_sample != nullptr,
                "The original AK-47 loop sample is missing from the retail sound bank");
        require(ak47_loop_sample->header.channel_count == 1,
                "The retail AK-47 loop should be mono for the exact DirectSound pan test");

        igi::gameplay::SceneWeapon automatic_weapon;
        automatic_weapon.identifier = "WEAPON_ID_AK47";
        automatic_weapon.parameters.maximum_rounds_per_burst = 30;
        automatic_weapon.parameters.shots_per_minute = 600.0F;
        automatic_weapon.fire_sound_name = "ak47_loop";
        automatic_weapon.fire_loop_end_sound_name = "ak47_loop_e";

        igi::audio::RetailVoiceParameters right_side_parameters;
        right_side_parameters.linear_gain = 0.7F;
        right_side_parameters.pan = 0.8F;
        weapon_sound_player.WeaponSoundPlayer_Tick(true, &automatic_weapon, right_side_parameters);
        require(audio_mixer.ActiveVoiceCount() == 1,
                "The retail AK-47 automatic-fire loop did not start");

        std::vector<std::int16_t> mixed_samples(8192);
        audio_mixer.MixFrames(mixed_samples);
        const std::uint64_t right_position_left_channel = sum_channel_magnitude(mixed_samples, 0);
        const std::uint64_t right_position_right_channel = sum_channel_magnitude(mixed_samples, 1);
        require(right_position_right_channel > right_position_left_channel,
                "An AI weapon sound to the right must be louder in the right channel");

        igi::audio::RetailVoiceParameters left_side_parameters;
        left_side_parameters.linear_gain = 0.7F;
        left_side_parameters.pan = -0.8F;
        weapon_sound_player.WeaponSoundPlayer_Tick(true, &automatic_weapon, left_side_parameters);
        audio_mixer.MixFrames(mixed_samples);
        const std::uint64_t left_position_left_channel = sum_channel_magnitude(mixed_samples, 0);
        const std::uint64_t left_position_right_channel = sum_channel_magnitude(mixed_samples, 1);
        require(left_position_left_channel > left_position_right_channel,
                "A moving AI weapon sound must update its loop pan in the left channel");

        weapon_sound_player.WeaponSoundPlayer_Tick(false, &automatic_weapon);
        weapon_sound_player.WeaponSoundPlayer_Tick(false, &automatic_weapon);
        require(audio_mixer.ActiveVoiceCount() == 1,
                "The automatic-fire loop stopped during its retail cadence gap");
        weapon_sound_player.WeaponSoundPlayer_Tick(false, &automatic_weapon);
        require(audio_mixer.ActiveVoiceCount() == 1,
                "The AK-47 loop-end sample did not replace the stopped fire loop");

        weapon_sound_player.WeaponSoundPlayer_Reset();
        audio_mixer.StopAll();

        igi::gameplay::SceneWeapon semi_automatic_weapon;
        semi_automatic_weapon.identifier = "WEAPON_ID_GLOCK";
        semi_automatic_weapon.parameters.maximum_rounds_per_burst = 1;
        semi_automatic_weapon.fire_sound_name = "glock_shot_1";
        semi_automatic_weapon.fire_loop_end_sound_name = "glock_shot_2";

        weapon_sound_player.WeaponSoundPlayer_Tick(false, &semi_automatic_weapon);
        require(audio_mixer.ActiveVoiceCount() == 0,
                "An idle semi-automatic weapon played a sound");
        weapon_sound_player.WeaponSoundPlayer_Tick(true, &semi_automatic_weapon);
        require(audio_mixer.ActiveVoiceCount() == 1,
                "The retail Glock single-shot sound did not play");

        std::cout << "Level 1 weapon audio passed: retail AK-47 loop cadence and tail, Glock single shot\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Level 1 weapon audio verification failed: " << exception.what() << '\n';
        return 1;
    }
}
