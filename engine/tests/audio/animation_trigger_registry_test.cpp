#include "igi/asset_io.hpp"
#include "igi/audio/animation_event_dispatcher.hpp"
#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/animation_trigger_registry.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/audio/sound_definition_registry.hpp"
#include "igi/audio/sound_system.hpp"
#include "igi/skeletal_animation.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
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
            throw std::runtime_error("Usage: animation_trigger_registry_test <original-game-directory>");
        }

        igi::audio::AnimationTriggerRegistry animation_trigger_registry;
        require(animation_trigger_registry.AnimationTriggerRegistry_LoadForGame(arguments[1]) == 50,
                "Retail animation trigger count changed");

        const igi::audio::AnimationTriggerDefinition* reload_trigger = nullptr;
        require(animation_trigger_registry.AnimationTriggerRegistry_TryResolve(1000, reload_trigger),
                "Retail Glock reload trigger 1000 is missing");
        require(reload_trigger->task_type_identifier == "TASKTYPE_ANIMSOUND" &&
                    reload_trigger->sound_definition_name == "glock_reload_1" &&
                    reload_trigger->fourth_argument_text == "TRUE" &&
                    reload_trigger->fourth_argument_number == 1.0,
                "Retail Glock trigger source fields were not preserved");

        const igi::audio::AnimationTriggerDefinition* fence_trigger = nullptr;
        require(animation_trigger_registry.AnimationTriggerRegistry_TryResolve(1025, fence_trigger),
                "Retail fence-walk trigger 1025 is missing");
        require(fence_trigger->sound_definition_name == "walk_fence_1" &&
                    fence_trigger->fourth_argument_text == "FALSE" &&
                    fence_trigger->fourth_argument_number == 0.0,
                "Retail fence-walk trigger source fields were not preserved");

        const igi::audio::AnimationTriggerDefinition* missing_footstep = nullptr;
        require(!animation_trigger_registry.AnimationTriggerRegistry_TryResolve(1010, missing_footstep),
                "The absent event 1010 must not be fabricated into the retail trigger table");
        require(missing_footstep == nullptr, "A failed trigger lookup must clear its output pointer");

        const std::filesystem::path game_directory(arguments[1]);
        const auto weapon_animations = igi::gameplay::load_skeletal_animations(
            game_directory / "common" / "ANIMS" / "005.IFF");
        const auto reload_clip_iterator = weapon_animations->clips.find(17);
        require(reload_clip_iterator != weapon_animations->clips.end(),
                "Retail Glock reload clip 17 is missing");

        const auto crossed_reload_sounds = igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
            reload_clip_iterator->second.events, 2560.0F, 2577.0F, false, animation_trigger_registry);
        require(crossed_reload_sounds.size() == 1 &&
                    crossed_reload_sounds.front().animation_event->event_identifier == 1000 &&
                    crossed_reload_sounds.front().trigger_definition->sound_definition_name == "glock_reload_1",
                "Retail Glock reload event did not resolve at its authored animation time");

        const auto repeated_reload_sounds = igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
            reload_clip_iterator->second.events, 2577.0F, 2720.0F, false, animation_trigger_registry);
        require(repeated_reload_sounds.empty(),
                "An animation event at the open interval boundary was dispatched twice");

        igi::gameplay::SkeletalAnimationClip loop_probe_clip;
        loop_probe_clip.events = {
            {1025, 0, 495.0F, {0.0F, 0.0F, 0.0F}},
            {1000, 0, 5.0F, {0.0F, 0.0F, 0.0F}},
        };
        const auto crossed_loop_sounds = igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
            loop_probe_clip.events, 480.0F, 20.0F, true, animation_trigger_registry);
        require(crossed_loop_sounds.size() == 2,
                "Animation sound events were lost at a wrapped clip boundary");

        const auto animation_restart_sounds = igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
            loop_probe_clip.events, 480.0F, 20.0F, false, animation_trigger_registry);
        require(animation_restart_sounds.empty(),
                "A same-clip animation restart was mistaken for a natural duration wrap");

        bool invalid_animation_time_rejected = false;
        try
        {
            static_cast<void>(igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
                loop_probe_clip.events,
                std::numeric_limits<float>::quiet_NaN(),
                0.0F,
                false,
                animation_trigger_registry));
        }
        catch (const std::invalid_argument&)
        {
            invalid_animation_time_rejected = true;
        }
        require(invalid_animation_time_rejected,
                "Animation sound dispatch accepted a non-finite animation time");

        igi::audio::SoundDefinitionRegistry sound_definition_registry;
        static_cast<void>(sound_definition_registry.SoundSystem_LoadForMission(arguments[1], 1));
        for (const auto& trigger : animation_trigger_registry.AnimationTriggerRegistry_Definitions())
        {
            const igi::audio::SoundDefinition* sound_definition = nullptr;
            require(sound_definition_registry.SoundSystem_TryResolve(
                        trigger.sound_definition_name, sound_definition),
                    "A retail animation trigger references a sound missing from the Level 1 sound registry");
            require(sound_definition->SoundSystem_IsPlayable(),
                    "A retail animation trigger resolves to a non-playable sound node");
        }

        igi::audio::AudioMixer audio_mixer;
        igi::audio::SoundBank sound_bank(audio_mixer);
        const auto common_sound_directory = igi::assets::resolve_path(game_directory, "common/sounds");
        require(sound_bank.SoundSystem_MountDirectory(common_sound_directory) > 300,
                "The common Level 1 sound assets did not mount");
        igi::audio::SoundSystem sound_system(audio_mixer, sound_bank, sound_definition_registry);
        const std::uint32_t reload_voice = sound_system.SoundSystem_PlayDefinition(
            crossed_reload_sounds.front().trigger_definition->sound_definition_name);
        require(reload_voice != 0 && sound_system.SoundSystem_IsPlaying(reload_voice),
                "The crossed retail reload event did not create an active sound voice");
        std::array<std::int16_t, 512> mixed_samples{};
        audio_mixer.MixFrames(mixed_samples);
        require(std::any_of(mixed_samples.begin(), mixed_samples.end(), [](std::int16_t sample)
        {
            return sample != 0;
        }), "The crossed retail reload event produced no PCM samples");

        std::cout << "Retail animation trigger registry passed: "
                  << animation_trigger_registry.AnimationTriggerRegistry_Count()
                  << " entries resolve to Level 1 sound definitions; authored Glock reload event dispatch passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Animation trigger registry verification failed: " << exception.what() << '\n';
        return 1;
    }
}
