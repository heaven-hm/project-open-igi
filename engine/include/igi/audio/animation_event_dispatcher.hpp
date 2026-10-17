#pragma once

#include "igi/audio/animation_trigger_registry.hpp"
#include "igi/skeletal_animation.hpp"

#include <vector>

namespace igi::audio
{

struct AnimationSoundEvent final
{
    const gameplay::SkeletalAnimationEvent* animation_event{};
    const AnimationTriggerDefinition* trigger_definition{};
};

// The open-igi C# candidate uses open-old/closed-new event intervals and splits loop wraps.
// This schedule is derived until a matching retail runtime trace confirms its exact timing.
[[nodiscard]] std::vector<AnimationSoundEvent> AnimationEventDispatcher_CollectCrossedSounds(
    const std::vector<gameplay::AnimationEventRecord>& animation_events,
    float previous_time,
    float current_time,
    bool wrapped,
    const AnimationTriggerRegistry& animation_trigger_registry);

} // namespace igi::audio
