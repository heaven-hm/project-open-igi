#pragma once

#include "igi/audio/audio_mixer.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

namespace igi::audio
{

class SdlAudioOutput final
{
  public:
    explicit SdlAudioOutput(AudioMixer& mixer, std::uint16_t callback_frame_count = 512);
    ~SdlAudioOutput();

    SdlAudioOutput(const SdlAudioOutput&) = delete;
    SdlAudioOutput& operator=(const SdlAudioOutput&) = delete;
    SdlAudioOutput(SdlAudioOutput&&) = delete;
    SdlAudioOutput& operator=(SdlAudioOutput&&) = delete;

    [[nodiscard]] bool IsAvailable() const noexcept;
    [[nodiscard]] std::string_view FailureReason() const noexcept;
    [[nodiscard]] std::uint64_t CallbackCount() const noexcept;
    [[nodiscard]] bool ConsumeCallbackFailure() noexcept;

  private:
    struct State;
    static void AudioSystem_MixSdlCallback(void* user_data, unsigned char* output_bytes, int byte_count) noexcept;
    static void AudioSystem_CloseDevice(State& state) noexcept;
    std::unique_ptr<State> state_;
};

} // namespace igi::audio
