#include "igi/audio/audio_mixer.hpp"
#include "igi/exceptions.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{

void SoundSystem_Require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void SoundSystem_TestRetailMixAndPan()
{
    igi::audio::AudioMixer mixer;
    const std::array<std::int16_t, 2> mono_samples{12000, -12000};
    const std::uint32_t buffer_handle = mixer.CreateBuffer({22050, 1}, mono_samples);
    const std::uint32_t voice_handle = mixer.Play(buffer_handle, {.looping = true});
    std::array<std::int16_t, 4> output_samples{};
    mixer.MixFrames(output_samples);
    SoundSystem_Require(output_samples == std::array<std::int16_t, 4>{12000, 12000, -12000, -12000},
                        "Unity-gain mono voice must be duplicated to the two retail output channels");

    mixer.StopVoice(voice_handle);
    const std::uint32_t right_panned_voice = mixer.Play(
        buffer_handle, {.linear_gain = 1.0F, .pan = -1.0F, .looping = true});
    std::array<std::int16_t, 2> panned_output{};
    mixer.MixFrames(panned_output);
    SoundSystem_Require(panned_output[0] == 0 && panned_output[1] == 12000,
                        "Retail positive DirectSound pan must attenuate the left channel");

    mixer.StopVoice(right_panned_voice);
    const std::uint32_t left_panned_voice = mixer.Play(
        buffer_handle, {.linear_gain = 1.0F, .pan = 1.0F, .looping = true});
    mixer.MixFrames(panned_output);
    SoundSystem_Require(panned_output[0] == 12000 && panned_output[1] == 0,
                        "Retail negative DirectSound pan must attenuate the right channel");
}

void SoundSystem_TestVolumeAndPlaybackRate()
{
    igi::audio::AudioMixer mixer;
    const std::array<std::int16_t, 4> mono_samples{10000, 20000, 30000, 4000};
    const std::uint32_t buffer_handle = mixer.CreateBuffer({22050, 1}, mono_samples);
    const std::uint32_t quiet_voice = mixer.Play(
        buffer_handle, {.linear_gain = 0.5F, .looping = true});
    std::array<std::int16_t, 2> output_samples{};
    mixer.MixFrames(output_samples);
    SoundSystem_Require(output_samples[0] >= 3161 && output_samples[0] <= 3163,
                        "Half gain must pass through retail attenuation before DirectSound amplitude scaling");
    mixer.StopVoice(quiet_voice);

    const std::uint32_t fast_voice = mixer.Play(buffer_handle, {.playback_rate = 4.0F});
    std::array<std::int16_t, 6> fast_output{};
    mixer.MixFrames(fast_output);
    SoundSystem_Require(fast_output == std::array<std::int16_t, 6>{10000, 10000, 30000, 30000, 0, 0},
                        "Retail voice playback rate must clamp at 2.0 before resampling");
    SoundSystem_Require(!mixer.IsPlaying(fast_voice), "A completed one-shot voice must be released");
}

void SoundSystem_TestVoicePoolAndStaleHandles()
{
    igi::audio::AudioMixer mixer;
    const std::array<std::int16_t, 1> sample{100};
    const std::uint32_t buffer_handle = mixer.CreateBuffer({22050, 1}, sample);
    std::vector<std::uint32_t> voice_handles;
    voice_handles.reserve(igi::audio::AudioMixer::retail_voice_count);
    for (std::size_t voice_index = 0; voice_index < igi::audio::AudioMixer::retail_voice_count; ++voice_index)
    {
        const std::uint32_t voice_handle = mixer.Play(buffer_handle, {.looping = true});
        SoundSystem_Require(voice_handle != 0, "The 32 retail voice slots must accept one sound each");
        voice_handles.push_back(voice_handle);
    }
    SoundSystem_Require(mixer.Play(buffer_handle, {.looping = true}) == 0,
                        "A full retail voice pool must reject an additional sound");
    mixer.StopVoice(voice_handles.front());
    const std::uint32_t replacement_voice = mixer.Play(buffer_handle, {.looping = true});
    SoundSystem_Require(replacement_voice != 0 && replacement_voice != voice_handles.front(),
                        "Voice handles must not be immediately reused after a stop");
    mixer.StopVoice(voice_handles.front());
    SoundSystem_Require(mixer.IsPlaying(replacement_voice),
                        "A stale voice handle must not stop a replacement sound");
    SoundSystem_Require(mixer.ActiveVoiceCount() == igi::audio::AudioMixer::retail_voice_count,
                        "The voice pool must report all active voices");
}

void SoundSystem_TestInvalidInputs()
{
    igi::audio::AudioMixer mixer;
    bool rejected_invalid_format = false;
    try
    {
        const std::array<std::int16_t, 1> sample{0};
        static_cast<void>(mixer.CreateBuffer({0, 1}, sample));
    }
    catch (const igi::logging::AudioException&)
    {
        rejected_invalid_format = true;
    }
    SoundSystem_Require(rejected_invalid_format, "A zero-rate audio buffer must fail with an audio exception");

    bool rejected_invalid_frame = false;
    try
    {
        std::array<std::int16_t, 1> incomplete_stereo_frame{};
        mixer.MixFrames(incomplete_stereo_frame);
    }
    catch (const igi::logging::AudioException&)
    {
        rejected_invalid_frame = true;
    }
    SoundSystem_Require(rejected_invalid_frame, "Incomplete stereo output must fail with an audio exception");
}

} // namespace

int main()
{
    try
    {
        SoundSystem_TestRetailMixAndPan();
        SoundSystem_TestVolumeAndPlaybackRate();
        SoundSystem_TestVoicePoolAndStaleHandles();
        SoundSystem_TestInvalidInputs();
        std::cout << "Retail audio mixer checks passed: mono/stereo mix, DirectSound pan, attenuation, pitch, voice pool, handles, and errors\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail audio mixer check failed: " << exception.what() << '\n';
        return 1;
    }
}
