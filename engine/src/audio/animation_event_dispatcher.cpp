#include "igi/audio/animation_event_dispatcher.hpp"

#include <cmath>
#include <stdexcept>

namespace igi::audio
{

std::vector<AnimationSoundEvent> AnimationEventDispatcher_CollectCrossedSounds(
    const gameplay::SkeletalAnimationClip& animation_clip,
    float previous_time,
    float current_time,
    bool wrapped,
    const AnimationTriggerRegistry& animation_trigger_registry)
{
    if (!std::isfinite(previous_time) || !std::isfinite(current_time) || previous_time < 0.0F ||
        current_time < 0.0F)
    {
        throw std::invalid_argument("Animation sound dispatch received an invalid clip time");
    }

    std::vector<AnimationSoundEvent> crossed_sounds;
    if (wrapped)
    {
        if (current_time >= previous_time)
        {
            throw std::invalid_argument("Animation sound dispatch marked a non-wrapping interval as wrapped");
        }
    }
    else if (current_time < previous_time)
    {
        return crossed_sounds;
    }

    crossed_sounds.reserve(animation_clip.events.size());
    for (const gameplay::SkeletalAnimationEvent& animation_event : animation_clip.events)
    {
        const bool crossed = wrapped
                                 ? animation_event.time > previous_time || animation_event.time <= current_time
                                 : animation_event.time > previous_time && animation_event.time <= current_time;
        if (!crossed)
        {
            continue;
        }

        const AnimationTriggerDefinition* trigger_definition = nullptr;
        if (!animation_trigger_registry.AnimationTriggerRegistry_TryResolve(
                static_cast<std::int32_t>(animation_event.event_identifier), trigger_definition) ||
            trigger_definition == nullptr)
        {
            continue;
        }

        crossed_sounds.push_back({&animation_event, trigger_definition});
    }

    return crossed_sounds;
}

} // namespace igi::audio
