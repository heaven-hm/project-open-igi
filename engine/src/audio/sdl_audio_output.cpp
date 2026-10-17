#include "igi/audio/sdl_audio_output.hpp"

#include <SDL.h>

#include <atomic>
#include <cstring>
#include <exception>
#include <string>

namespace igi::audio
{

struct SdlAudioOutput::State final
{
    explicit State(AudioMixer& audio_mixer) : mixer(audio_mixer) {}

    AudioMixer& mixer;
    SDL_AudioDeviceID device = 0;
    bool owns_audio_subsystem = false;
    std::string failure_reason;
    std::atomic<std::uint64_t> callback_count = 0;
    std::atomic<bool> callback_failed = false;
};

void SdlAudioOutput::AudioSystem_MixSdlCallback(void* user_data, unsigned char* output_bytes, int byte_count) noexcept
{
    auto* state = static_cast<SdlAudioOutput::State*>(user_data);
    if (state == nullptr || output_bytes == nullptr || byte_count <= 0 ||
        byte_count % static_cast<int>(sizeof(std::int16_t)) != 0)
    {
        if (output_bytes != nullptr && byte_count > 0)
        {
            std::memset(output_bytes, 0, static_cast<std::size_t>(byte_count));
        }
        if (state != nullptr)
        {
            state->callback_failed.store(true, std::memory_order_relaxed);
        }
        return;
    }

    const std::size_t sample_count = static_cast<std::size_t>(byte_count) / sizeof(std::int16_t);
    auto* output_samples = reinterpret_cast<std::int16_t*>(output_bytes);
    try
    {
        state->mixer.MixFrames(std::span<std::int16_t>(output_samples, sample_count));
        state->callback_count.fetch_add(1, std::memory_order_relaxed);
    }
    catch (...)
    {
        std::memset(output_bytes, 0, static_cast<std::size_t>(byte_count));
        state->callback_failed.store(true, std::memory_order_relaxed);
    }
}

void SdlAudioOutput::AudioSystem_CloseDevice(State& state) noexcept
{
    if (state.device != 0)
    {
        SDL_CloseAudioDevice(state.device);
        state.device = 0;
    }
    if (state.owns_audio_subsystem && (SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) != 0)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    state.owns_audio_subsystem = false;
}


SdlAudioOutput::SdlAudioOutput(AudioMixer& mixer, std::uint16_t callback_frame_count)
    : state_(std::make_unique<State>(mixer))
{
    if (callback_frame_count == 0)
    {
        state_->failure_reason = "SDL audio callback frame count must be positive";
        return;
    }

    if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0)
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        {
            state_->failure_reason = "SDL audio initialization failed: " + std::string(SDL_GetError());
            return;
        }
        state_->owns_audio_subsystem = true;
    }

    SDL_AudioSpec requested_specification{};
    requested_specification.freq = static_cast<int>(AudioMixer::retail_output_sample_rate);
    requested_specification.format = AUDIO_S16SYS;
    requested_specification.channels = static_cast<Uint8>(AudioMixer::output_channel_count);
    requested_specification.samples = callback_frame_count;
    requested_specification.callback = SdlAudioOutput::AudioSystem_MixSdlCallback;
    requested_specification.userdata = state_.get();

    SDL_AudioSpec obtained_specification{};
    state_->device = SDL_OpenAudioDevice(nullptr, 0, &requested_specification, &obtained_specification, 0);
    if (state_->device == 0)
    {
        state_->failure_reason = "SDL could not open the retail 22050 Hz stereo output: " +
                                 std::string(SDL_GetError());
        SdlAudioOutput::AudioSystem_CloseDevice(*state_);
        return;
    }

    if (obtained_specification.freq != requested_specification.freq ||
        obtained_specification.format != requested_specification.format ||
        obtained_specification.channels != requested_specification.channels)
    {
        state_->failure_reason = "SDL audio device did not preserve the requested retail output format";
        SdlAudioOutput::AudioSystem_CloseDevice(*state_);
        return;
    }

    SDL_PauseAudioDevice(state_->device, 0);
}

SdlAudioOutput::~SdlAudioOutput()
{
    if (state_)
    {
        SdlAudioOutput::AudioSystem_CloseDevice(*state_);
    }
}

bool SdlAudioOutput::IsAvailable() const noexcept
{
    return state_ != nullptr && state_->device != 0;
}

std::string_view SdlAudioOutput::FailureReason() const noexcept
{
    return state_ == nullptr ? std::string_view("SDL audio output state is unavailable") :
                               std::string_view(state_->failure_reason);
}

std::uint64_t SdlAudioOutput::CallbackCount() const noexcept
{
    return state_ == nullptr ? 0 : state_->callback_count.load(std::memory_order_relaxed);
}

bool SdlAudioOutput::ConsumeCallbackFailure() noexcept
{
    return state_ != nullptr && state_->callback_failed.exchange(false, std::memory_order_relaxed);
}

} // namespace igi::audio
