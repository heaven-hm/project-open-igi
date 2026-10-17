#include "igi/audio/animation_trigger_registry.hpp"
#include "igi/audio/sound_definition_registry.hpp"

#include <iostream>
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

        std::cout << "Retail animation trigger registry passed: "
                  << animation_trigger_registry.AnimationTriggerRegistry_Count()
                  << " entries resolve to Level 1 sound definitions\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Animation trigger registry verification failed: " << exception.what() << '\n';
        return 1;
    }
}
