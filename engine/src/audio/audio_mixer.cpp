#include "igi/audio/audio_mixer.hpp"

#include "igi/audio/sound_volume_curve.hpp"
#include "igi/exceptions.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>

namespace igi::audio
{
namespace
{

std::int16_t SoundSystem_ClampMixedSample(double mixed_sample) noexcept
{
    const double rounded_sample = std::nearbyint(mixed_sample);
    const double bounded_sample = std::clamp(
        rounded_sample,
        static_cast<double>(std::numeric_limits<std::int16_t>::min()),
        static_cast<double>(std::numeric_limits<std::int16_t>::max()));
    return static_cast<std::int16_t>(bounded_sample);
}

} // namespace

std::uint32_t AudioMixer::CreateBuffer(
    AudioBufferFormat format,
    std::span<const std::int16_t> interleaved_samples)
{
    if (format.sample_rate == 0 || format.channel_count == 0 || format.channel_count > output_channel_count)
    {
        throw logging::AudioException("Audio buffer format must have a positive rate and one or two channels");
    }
    if (interleaved_samples.empty() || interleaved_samples.size() % format.channel_count != 0)
    {
        throw logging::AudioException("Audio buffer must contain complete, non-empty sample frames");
    }

    auto buffer = std::make_shared<AudioBuffer>();
    buffer->format = format;
    buffer->interleaved_samples.assign(interleaved_samples.begin(), interleaved_samples.end());
    buffer->frame_count = interleaved_samples.size() / format.channel_count;

    std::lock_guard<std::mutex> lock(mutex_);
    const std::uint32_t first_candidate_handle = next_buffer_handle_;
    std::uint32_t buffer_handle = first_candidate_handle;
    while (buffer_handle == 0 || buffers_.contains(buffer_handle))
    {
        ++buffer_handle;
        if (buffer_handle == 0)
        {
            buffer_handle = 1;
        }
        if (buffer_handle == first_candidate_handle)
        {
            throw logging::AudioException("No free retail sound-buffer handle remains");
        }
    }
    next_buffer_handle_ = buffer_handle + 1U;
    if (next_buffer_handle_ == 0)
    {
        next_buffer_handle_ = 1;
    }
    buffers_.emplace(buffer_handle, std::move(buffer));
    return buffer_handle;
}

void AudioMixer::DestroyBuffer(std::uint32_t buffer_handle)
{
    if (buffer_handle == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    const auto buffer_iterator = buffers_.find(buffer_handle);
    if (buffer_iterator == buffers_.end())
    {
        return;
    }

    for (Voice& voice : voices_)
    {
        if (voice.active && voice.buffer == buffer_iterator->second)
        {
            voice = Voice{};
        }
    }
    buffers_.erase(buffer_iterator);
}

std::uint32_t AudioMixer::Play(std::uint32_t buffer_handle, RetailVoiceParameters parameters)
{
    ValidateVoiceParameters(parameters);

    std::lock_guard<std::mutex> lock(mutex_);
    const auto buffer_iterator = buffers_.find(buffer_handle);
    if (buffer_iterator == buffers_.end())
    {
        return 0;
    }

    for (std::size_t voice_index = 0; voice_index < voices_.size(); ++voice_index)
    {
        Voice& voice = voices_[voice_index];
        if (voice.active)
        {
            continue;
        }

        const std::uint32_t first_candidate_handle = next_voice_handle_;
        std::uint32_t voice_handle = first_candidate_handle;
        while (voice_handle == 0 || std::any_of(voices_.begin(), voices_.end(), [voice_handle](const Voice& candidate)
                                                { return candidate.active && candidate.handle == voice_handle; }))
        {
            ++voice_handle;
            if (voice_handle == 0)
            {
                voice_handle = 1;
            }
            if (voice_handle == first_candidate_handle)
            {
                throw logging::AudioException("No free retail sound-voice handle remains");
            }
        }
        next_voice_handle_ = voice_handle + 1U;
        if (next_voice_handle_ == 0)
        {
            next_voice_handle_ = 1;
        }

        voice.active = true;
        voice.handle = voice_handle;
        voice.buffer = buffer_iterator->second;
        voice.parameters = parameters;
        voice.source_frame_position = 0.0;
        SoundSystem_UpdateVoiceGains(voice);
        return voice_handle;
    }

    return 0;
}

bool AudioMixer::IsPlaying(std::uint32_t voice_handle) const
{
    if (voice_handle == 0)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    return std::any_of(voices_.begin(), voices_.end(), [voice_handle](const Voice& voice)
                       { return voice.active && voice.handle == voice_handle; });
}

void AudioMixer::SetVoiceParameters(std::uint32_t voice_handle, RetailVoiceParameters parameters)
{
    ValidateVoiceParameters(parameters);
    if (voice_handle == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    const auto voice_iterator = std::find_if(voices_.begin(), voices_.end(), [voice_handle](const Voice& voice)
                                            { return voice.active && voice.handle == voice_handle; });
    if (voice_iterator == voices_.end())
    {
        return;
    }

    voice_iterator->parameters = parameters;
    SoundSystem_UpdateVoiceGains(*voice_iterator);
}

void AudioMixer::StopVoice(std::uint32_t voice_handle)
{
    if (voice_handle == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    const auto voice_iterator = std::find_if(voices_.begin(), voices_.end(), [voice_handle](const Voice& voice)
                                            { return voice.active && voice.handle == voice_handle; });
    if (voice_iterator != voices_.end())
    {
        *voice_iterator = Voice{};
    }
}

void AudioMixer::StopAll()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (Voice& voice : voices_)
    {
        voice = Voice{};
    }
}

std::size_t AudioMixer::ActiveVoiceCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<std::size_t>(std::count_if(
        voices_.begin(), voices_.end(), [](const Voice& voice) { return voice.active; }));
}

void AudioMixer::MixFrames(std::span<std::int16_t> output_samples)
{
    if (output_samples.size() % output_channel_count != 0)
    {
        throw logging::AudioException("Audio output buffer must contain complete stereo frames");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::fill(output_samples.begin(), output_samples.end(), 0);
    const std::size_t output_frame_count = output_samples.size() / output_channel_count;

    for (Voice& voice : voices_)
    {
        if (!voice.active || !voice.buffer)
        {
            continue;
        }

        const AudioBuffer& buffer = *voice.buffer;
        const double source_frame_step =
            static_cast<double>(buffer.format.sample_rate) * voice.parameters.playback_rate /
            static_cast<double>(retail_output_sample_rate);

        for (std::size_t output_frame_index = 0; output_frame_index < output_frame_count; ++output_frame_index)
        {
            if (voice.source_frame_position >= static_cast<double>(buffer.frame_count))
            {
                if (voice.parameters.looping)
                {
                    voice.source_frame_position = std::fmod(
                        voice.source_frame_position, static_cast<double>(buffer.frame_count));
                }
                else
                {
                    voice = Voice{};
                    break;
                }
            }

            const std::size_t first_source_frame = static_cast<std::size_t>(voice.source_frame_position);
            const std::size_t following_source_frame = first_source_frame + 1U < buffer.frame_count
                ? first_source_frame + 1U
                : (voice.parameters.looping ? 0U : first_source_frame);
            const double interpolation_fraction = voice.source_frame_position - first_source_frame;
            const std::size_t source_left_index = first_source_frame * buffer.format.channel_count;
            const std::size_t source_right_index = source_left_index +
                                                   (buffer.format.channel_count == 2 ? 1U : 0U);
            const std::size_t following_left_index = following_source_frame * buffer.format.channel_count;
            const std::size_t following_right_index = following_left_index +
                                                       (buffer.format.channel_count == 2 ? 1U : 0U);

            const double first_left_sample = buffer.interleaved_samples[source_left_index];
            const double second_left_sample = buffer.interleaved_samples[following_left_index];
            const double first_right_sample = buffer.interleaved_samples[source_right_index];
            const double second_right_sample = buffer.interleaved_samples[following_right_index];
            const double source_left_sample = first_left_sample +
                                              ((second_left_sample - first_left_sample) * interpolation_fraction);
            const double source_right_sample = first_right_sample +
                                               ((second_right_sample - first_right_sample) * interpolation_fraction);

            const std::size_t output_left_index = output_frame_index * output_channel_count;
            const std::size_t output_right_index = output_left_index + 1U;
            const double mixed_left_sample = output_samples[output_left_index] +
                (source_left_sample * voice.volume_amplitude * voice.left_pan_amplitude);
            const double mixed_right_sample = output_samples[output_right_index] +
                (source_right_sample * voice.volume_amplitude * voice.right_pan_amplitude);
            output_samples[output_left_index] = SoundSystem_ClampMixedSample(mixed_left_sample);
            output_samples[output_right_index] = SoundSystem_ClampMixedSample(mixed_right_sample);

            voice.source_frame_position += source_frame_step;
        }
    }
}

void AudioMixer::ValidateVoiceParameters(const RetailVoiceParameters& parameters)
{
    if (!std::isfinite(parameters.linear_gain) || !std::isfinite(parameters.pan) ||
        !std::isfinite(parameters.playback_rate))
    {
        throw logging::AudioException("Retail voice gain, pan, and playback rate must be finite");
    }
}

// Original IGI.EXE voice update at VA 0x00496E60 sets DirectSound volume, pan, and rate.
void AudioMixer::SoundSystem_UpdateVoiceGains(Voice& voice)
{
    voice.parameters.playback_rate = SoundSystem_ClampRetailPlaybackRate(voice.parameters.playback_rate);
    const float bounded_gain = std::clamp(voice.parameters.linear_gain, 0.0F, 1.0F);
    const std::int32_t volume_millibels = SoundSystem_GetRetailVolumeMillibels(bounded_gain);
    const std::int32_t pan_millibels = SoundSystem_GetRetailPanMillibels(
        voice.parameters.pan, voice.parameters.reverse_stereo);

    voice.volume_amplitude = MillibelsToAmplitude(volume_millibels);
    const double pan_amplitude = MillibelsToAmplitude(-std::abs(pan_millibels));
    // DirectSound positive pan attenuates left; negative pan attenuates right.
    if (pan_millibels > 0)
    {
        voice.left_pan_amplitude = pan_amplitude;
        voice.right_pan_amplitude = 1.0;
    }
    else if (pan_millibels < 0)
    {
        voice.left_pan_amplitude = 1.0;
        voice.right_pan_amplitude = pan_amplitude;
    }
    else
    {
        voice.left_pan_amplitude = 1.0;
        voice.right_pan_amplitude = 1.0;
    }
}

double AudioMixer::MillibelsToAmplitude(std::int32_t millibels) noexcept
{
    if (millibels <= retail_volume_table_floor)
    {
        return 0.0;
    }

    // DirectSound interprets its SetVolume/SetPan values as hundredths of a decibel.
    return std::pow(10.0, static_cast<double>(millibels) / 2000.0);
}

} // namespace igi::audio
