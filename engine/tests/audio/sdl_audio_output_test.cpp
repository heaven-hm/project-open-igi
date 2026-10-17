#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/sdl_audio_output.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>

int main()
{
    try
    {
        igi::audio::AudioMixer mixer;
        const std::array<std::int16_t, 2> sample{1000, -1000};
        const std::uint32_t buffer_handle = mixer.CreateBuffer({22050, 1}, sample);
        static_cast<void>(mixer.Play(buffer_handle, {.looping = true}));
        igi::audio::SdlAudioOutput output(mixer, 256);
        if (!output.IsAvailable())
        {
            throw std::runtime_error("SDL dummy audio device did not open: " + std::string(output.FailureReason()));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        if (output.CallbackCount() == 0)
        {
            throw std::runtime_error("SDL audio callback did not request any mixer frames");
        }
        if (output.ConsumeCallbackFailure())
        {
            throw std::runtime_error("SDL audio callback failed while filling its output buffer");
        }

        std::cout << "SDL audio output callback ran " << output.CallbackCount() << " time(s)\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "SDL audio output check failed: " << exception.what() << '\n';
        return 1;
    }
}
