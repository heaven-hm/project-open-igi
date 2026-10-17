#pragma once

#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/audio/sound_definition_registry.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

namespace igi::audio
{

class SoundSystem final
{
  public:
    SoundSystem(AudioMixer& mixer, SoundBank& bank, SoundDefinitionRegistry& definitions) noexcept;

    // Original IGI.EXE Sound_EventTrigger resolves a definition name and starts its sound voice.
    [[nodiscard]] std::uint32_t SoundSystem_PlayDefinition(
        std::string_view definition_name,
        RetailVoiceParameters parameters = {},
        std::optional<bool> looping_override = std::nullopt);

    [[nodiscard]] std::uint32_t SoundSystem_PlayBankSound(
        std::string_view bank_key,
        RetailVoiceParameters parameters = {});
    [[nodiscard]] bool SoundSystem_IsPlaying(std::uint32_t voice_handle) const;
    void SoundSystem_SetVoiceParameters(std::uint32_t voice_handle, RetailVoiceParameters parameters);
    void SoundSystem_SetDefinitionVoiceParameters(
        std::string_view definition_name,
        std::uint32_t voice_handle,
        RetailVoiceParameters parameters);
    void SoundSystem_Stop(std::uint32_t voice_handle);
    void SoundSystem_StopAll();

  private:
    [[nodiscard]] std::uint32_t SoundSystem_PlayResolvedBankSound(
        std::string_view bank_key,
        RetailVoiceParameters parameters);
    void SoundSystem_LogMissingDefinitionOnce(std::string_view definition_name);

    AudioMixer& mixer_;
    SoundBank& bank_;
    SoundDefinitionRegistry& definitions_;
    std::unordered_set<std::string> reported_missing_definitions_;
};

} // namespace igi::audio
