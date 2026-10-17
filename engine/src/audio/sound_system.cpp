#include "igi/audio/sound_system.hpp"

#include "igi/audio/sound_volume_curve.hpp"
#include "igi/diagnostics.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <string>
#include <utility>

namespace igi::audio
{
namespace
{

constexpr std::string_view audio_log_category = "audio";

} // namespace

SoundSystem::SoundSystem(
    AudioMixer& mixer,
    SoundBank& bank,
    SoundDefinitionRegistry& definitions) noexcept
    : mixer_(mixer),
      bank_(bank),
      definitions_(definitions)
{
}

// Original IGI.EXE game native Sound_EventTrigger at VA 0x004E7200 resolves a named sound and
// rejects non-Sound definition types before delegating voice creation to 0x004E6DB0.
// REA/Ghidra dossiers: ev_cf822779295241012541b99ee029020c2c524cea8595954f58895e84ab955089 and
// ev_2c100bb34ddbbf8879ff76512df711502d70fce10153cb23e31a5c5026183ca2.
std::uint32_t SoundSystem::SoundSystem_PlayDefinition(
    std::string_view definition_name,
    RetailVoiceParameters parameters,
    std::optional<bool> looping_override)
{
    if (definition_name.empty())
    {
        return 0;
    }

    const SoundDefinition* definition = nullptr;
    if (!definitions_.SoundSystem_TryResolve(definition_name, definition) || definition == nullptr)
    {
        SoundSystem_LogMissingDefinitionOnce(definition_name);
        return 0;
    }

    // Original IGI.EXE Sound_EventTrigger rejects group, graph, and trigger definitions on direct play.
    if (!definition->SoundSystem_IsPlayable())
    {
        return 0;
    }

    parameters.linear_gain *= definition->volume;
    parameters.playback_rate = SoundSystem_ClampRetailPlaybackRate(parameters.playback_rate);
    parameters.looping = looping_override.value_or(definition->looping);
    return SoundSystem_PlayResolvedBankSound(definition->bank_key, parameters);
}

std::uint32_t SoundSystem::SoundSystem_PlayBankSound(
    std::string_view bank_key,
    RetailVoiceParameters parameters)
{
    parameters.playback_rate = SoundSystem_ClampRetailPlaybackRate(parameters.playback_rate);
    return SoundSystem_PlayResolvedBankSound(bank_key, parameters);
}

bool SoundSystem::SoundSystem_IsPlaying(std::uint32_t voice_handle) const
{
    return mixer_.IsPlaying(voice_handle);
}

void SoundSystem::SoundSystem_Stop(std::uint32_t voice_handle)
{
    mixer_.StopVoice(voice_handle);
}

void SoundSystem::SoundSystem_StopAll()
{
    mixer_.StopAll();
}

// Original IGI.EXE voice-task native at VA 0x004E6DB0 allocates or updates the sound task and voice.
// REA/Ghidra evidence: ev_2c100bb34ddbbf8879ff76512df711502d70fce10153cb23e31a5c5026183ca2.
std::uint32_t SoundSystem::SoundSystem_PlayResolvedBankSound(
    std::string_view bank_key,
    RetailVoiceParameters parameters)
{
    const std::uint32_t buffer_handle = bank_.SoundSystem_GetOrCreateAudioBuffer(bank_key);
    if (buffer_handle == 0)
    {
        return 0;
    }

    try
    {
        return mixer_.Play(buffer_handle, parameters);
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Sound '" + std::string(bank_key) + "' could not start: " + exception.what());
        return 0;
    }
}

void SoundSystem::SoundSystem_LogMissingDefinitionOnce(std::string_view definition_name)
{
    std::string normalized_name(definition_name);
    std::transform(
        normalized_name.begin(),
        normalized_name.end(),
        normalized_name.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); });
    if (reported_missing_definitions_.insert(normalized_name).second)
    {
        diagnostics::log(
            diagnostics::Severity::debug,
            audio_log_category,
            "No sound definition named '" + std::string(definition_name) + "'");
    }
}

} // namespace igi::audio
