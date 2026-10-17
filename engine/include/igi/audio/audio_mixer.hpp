#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <span>
#include <unordered_map>
#include <vector>

namespace igi::audio
{

struct AudioBufferFormat final
{
    std::uint32_t sample_rate = 0;
    std::uint16_t channel_count = 0;
};

struct RetailVoiceParameters final
{
    float linear_gain = 1.0F;
    float pan = 0.0F;
    float playback_rate = 1.0F;
    bool looping = false;
    bool reverse_stereo = false;
};

// Original IGI.EXE initializes 32 DirectSound voices and a 22050 Hz primary output at VA 0x004952F0.
class AudioMixer final
{
  public:
    static constexpr std::size_t retail_voice_count = 32;
    static constexpr std::uint32_t retail_output_sample_rate = 22050;
    static constexpr std::uint16_t output_channel_count = 2;

    [[nodiscard]] std::uint32_t CreateBuffer(
        AudioBufferFormat format,
        std::span<const std::int16_t> interleaved_samples);
    void DestroyBuffer(std::uint32_t buffer_handle);

    // Retail allocates the first free voice and drops the sound when all 32 voices are occupied.
    [[nodiscard]] std::uint32_t Play(std::uint32_t buffer_handle, RetailVoiceParameters parameters = {});
    [[nodiscard]] bool IsPlaying(std::uint32_t voice_handle) const;
    void SetVoiceParameters(std::uint32_t voice_handle, RetailVoiceParameters parameters);
    void StopVoice(std::uint32_t voice_handle);
    void StopAll();
    [[nodiscard]] std::size_t ActiveVoiceCount() const;

    // SDL calls this with signed 16-bit interleaved stereo output frames.
    void MixFrames(std::span<std::int16_t> output_samples);

  private:
    struct AudioBuffer final
    {
        AudioBufferFormat format;
        std::vector<std::int16_t> interleaved_samples;
        std::size_t frame_count = 0;
    };

    struct Voice final
    {
        bool active = false;
        std::uint32_t handle = 0;
        std::shared_ptr<const AudioBuffer> buffer;
        RetailVoiceParameters parameters;
        double source_frame_position = 0.0;
        double volume_amplitude = 1.0;
        double left_pan_amplitude = 1.0;
        double right_pan_amplitude = 1.0;
    };

    static void ValidateVoiceParameters(const RetailVoiceParameters& parameters);
    static void SoundSystem_UpdateVoiceGains(Voice& voice);
    [[nodiscard]] static double MillibelsToAmplitude(std::int32_t millibels) noexcept;

    mutable std::mutex mutex_;
    std::array<Voice, retail_voice_count> voices_{};
    std::unordered_map<std::uint32_t, std::shared_ptr<const AudioBuffer>> buffers_;
    std::uint32_t next_buffer_handle_ = 1;
    std::uint32_t next_voice_handle_ = 1;
};

} // namespace igi::audio
