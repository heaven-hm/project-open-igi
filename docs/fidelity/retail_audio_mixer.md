# Retail audio mixer and SDL output

## Scope and status

The C++ engine now has a bounded 32-voice mixer and an SDL2 callback device requesting 22050 Hz, stereo, signed 16-bit output. Resident PCM is mixed with linear interpolation, gain and pan are converted through the retail attenuation table, and playback rate is clamped to the retail range. The SDL callback catches failures and emits silence rather than allowing an exception to cross the audio thread.

This device is not yet connected to the game loop's sound bank or QVM sound definitions. The app therefore does not yet play Level 1 sounds. The interpolation/resampling and SDL device timing have not been compared directly with DirectSound; this work is a tested audio foundation, not a claim of end-to-end parity.

## Retail evidence

Target executable: `/Users/haseeb-mir/Downloads/IGI/igi.exe` (PE32 x86), SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

- REA/Ghidra 12.1.2 at `0x004952F0`: 768 bytes, 23 blocks; evidence `ev_4240c272ee1c373a6409bb33387f6a77c5e4c0f91d3f46aeaa4f45b5e4136c90`. Pseudocode evidence `ev_1fb1bd1433efcce21bfb436e786a22441e9d0006bdd8f31d99d7222a0809f682` shows the attenuation table initialization, the `0x20` voice count, and the DirectSound format setup.
- REA/Ghidra 12.1.2 at `0x00496E60`: 1102 bytes, 47 blocks; evidence `ev_81ea43739dddbb92f578229c0e6ac65cb06e5c2d4529227cd616fd0bbc50f136`. Pseudocode evidence `ev_c8b3f5fbbc4ee51b2f656c5b142a0a18246fc04abf779493216cc83bc6e554af` shows volume/pan table indices, DirectSound buffer vtable calls, and the `[0.5, 2.0]` rate clamp.
- The current REA session verified the same retail file hash listed above. The two procedures still have Ghidra-generated names; this port does not present its C++ helper names as recovered retail symbols.
- Microsoft DirectSound documents signed `SetPan` values from `-10000` to `10000`: positive values attenuate the left channel, negative values attenuate the right. See [IDirectSoundBuffer::SetPan](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/mt708938%28v%3Dvs.85%29). `SetVolume` uses hundredths of a decibel; see [IDirectSoundBuffer::SetVolume](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/mt708939%28v%3Dvs.85%29).

## Port files and tests

- `engine/include/igi/audio/audio_mixer.hpp`, `engine/src/audio/audio_mixer.cpp`
- `engine/include/igi/audio/sdl_audio_output.hpp`, `engine/src/audio/sdl_audio_output.cpp`
- `engine/tests/audio/audio_mixer_test.cpp`, `engine/tests/audio/sdl_audio_output_test.cpp`
- CMake library: `igi_audio`; tests: `retail_audio_mixer`, `sdl_audio_output_callback`

Mixer checks cover mono duplication, both hard-pan endpoints, the retail half-gain point, sample-rate clamping, one-shot completion, all 32 voices, full-pool rejection, stale voice handles, invalid formats, and incomplete output frames. The SDL callback test runs against SDL's dummy audio driver and verifies callback execution.

Focused command:

```sh
cmake --build /tmp/project-open-igi-manual-build --target audio_mixer_test sdl_audio_output_test igi_game -j 1
ctest --test-dir /tmp/project-open-igi-manual-build -R 'retail_audio_mixer|sdl_audio_output_callback|retail_ilsf_sound_parser|retail_level_one_ilsf_samples|mvp_original_level_gameplay|igi_game_headless_loop' --output-on-failure
```

Observed result: 6/6 passed after correcting pan attenuation. The dummy output test proves the SDL callback path only; it does not produce an audible game or compare the port to a live retail capture.

## Remaining audio work

1. Connect Level 1 sound-file loading, streamed reads, and resident ILSF decoding to mixer buffers.
2. Port the sound bank and `sounds.qvm` definition registry used by original script names.
3. Add event-triggered sound playback for player movement, weapons, impacts, AI, and mission objects.
4. Compare sample positions, voice updates, pan/gain, and event timing against retail runtime captures.
