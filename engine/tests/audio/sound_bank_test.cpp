#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/asset_io.hpp"

#include <array>
#include <cstdint>
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

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Usage: sound_bank_test <original-game-directory>");
        }

        const std::filesystem::path game_directory(arguments[1]);
        const auto common_sounds = igi::assets::resolve_path(game_directory, "common/sounds");
        const auto mission_sounds = igi::assets::resolve_path(
            game_directory,
            "missions/location0/level1/sounds");
        const auto menu_archive = igi::assets::resolve_path(
            game_directory,
            "menusystem/sound/sounds.res");

        igi::audio::AudioMixer mixer;
        igi::audio::SoundBank bank(mixer);
        const std::size_t common_count = bank.SoundSystem_MountDirectory(common_sounds);
        const std::size_t mission_count = bank.SoundSystem_MountDirectory(mission_sounds);
        const std::size_t menu_count = bank.SoundSystem_MountArchive(menu_archive);
        require(common_count > 300, "The common sound bank did not load the retail effects");
        require(mission_count > 0, "Level 1 sound assets were not mounted");
        require(menu_count > 0, "The menu sound archive did not load its assets");

        const igi::audio::SoundEntry* rifle_entry = nullptr;
        require(bank.SoundSystem_TryGet("ak47_loop", rifle_entry), "AK-47 loop sound is missing");
        require(rifle_entry != nullptr && !rifle_entry->SoundSystem_IsStreamed(),
                "AK-47 loop must be a resident retail sound");

        const igi::audio::SoundEntry* music_entry = nullptr;
        require(bank.SoundSystem_TryGet("game_music", music_entry), "Level 1 score is missing");
        require(music_entry != nullptr && music_entry->SoundSystem_IsStreamed(),
                "Level 1 score must retain its streamed ILSF storage mode");
        require(music_entry->encoded_file.empty(), "Streamed directory music must remain unopened in the bank");

        const std::uint32_t rifle_buffer = bank.SoundSystem_GetOrCreateAudioBuffer("AK47_LOOP");
        require(rifle_buffer != 0, "Resident AK-47 sound did not decode into the mixer");
        const std::uint32_t rifle_voice = mixer.Play(rifle_buffer);
        require(rifle_voice != 0, "Mixer rejected a loaded retail weapon sound");
        std::array<std::int16_t, 512> mixed_frames{};
        mixer.MixFrames(mixed_frames);
        require(mixer.IsPlaying(rifle_voice), "Retail weapon sound stopped before its samples mixed");
        require(bank.SoundSystem_Count() > common_count, "Mission and menu mounts did not extend the bank");

        const std::string sample_key = igi::audio::SoundSystem_ExtractSoundKey(
            "LOCAL:common/sounds/ak47_1.wav");
        require(sample_key == "AK47_1", "Original path-to-key extraction changed");
        require(igi::audio::SoundSystem_ExtractSoundKey("ak47_1.wav").empty(),
                "A bare filename must retain the retail empty-key behavior");

        std::cout << "Retail sound bank passed: common=" << common_count
                  << ", mission=" << mission_count << ", menu=" << menu_count
                  << ", total=" << bank.SoundSystem_Count() << '\n';
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail sound bank verification failed: " << exception.what() << '\n';
        return 1;
    }
}
