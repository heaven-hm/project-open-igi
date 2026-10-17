# Retail sound volume curve

## Scope and status

This C++ slice ports the retail attenuation lookup curve, stereo-pan attenuation, playback-rate clamp, a fixed 32-voice PCM mixer, and an SDL2 output device requested at the retail 22050 Hz stereo format. The Level 1 sound bank and QVM definitions are connected to mission music, player jump/fall cues, and player/AI weapon fire. Other gameplay event routes and direct comparison to retail audio output remain incomplete; end-to-end sound parity remains `UNKNOWN`.

## Reference binary

- File: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- Architecture: PE32 x86

## Retail evidence

Ghidra/REA analyzed the same SHA-256 with Ghidra 12.1.2.

| Retail function | Observed body | Evidence | Relevant behavior |
|---|---:|---|---|
| `FUN_004952F0` | 768 bytes, 23 basic blocks | `ev_045da563dcccf0041b8283b3778575c9bb911f60abeb72679def1d52c642f9b8`, `ev_276f257ce1e5b9cde80efce3db4f8dd94d3a2b5095da53de3657034c174d9829`, `ev_b4af29d2b28ab7c22e6b6d7ed54e618c6130d820f529121056737b3bf3ed7f10` | Initializes entry zero to `-10000`, then fills the 10,000 remaining values from the gain index and clamps to `[-10000, 0]`. The instruction sequence uses `FLDLN2`, `FYL2X`, multiply by `1442.6950408888495`, and `FISTP qword`. |
| `FUN_00496E60` | 1,102 bytes, 47 basic blocks | `ev_325c6253ba9e2e2623de9259cf4bb6d1ca5dbf8b7ee6938117609e4ca6ec3b00`, `ev_eaf627906b6ba16171468863340666b1d9ece8c696014ed279d59cabbf2d34f5`, `ev_e8fd4ee7e488ffc30b88ffe84e1b82837cfd28dacc9ee10297a0063c8f9693ea` | Updates active voices: linearly attenuates by distance, derives stereo pan from listener-space lateral offset, rounds table indices, skips pan when the corresponding flag is set, and clamps playback rate to `[0.5, 2.0]`. Incoming calls at `0x490D8B` and `0x4967CC` support the voice-update interpretation. |

Radare2 independently analyzed the retail executable. `af @ 0x004952F0` recovered size `768`, `nbbs=23`; `af @ 0x00496E60` recovered size `1102`, `nbbs=47`. Both match the Ghidra procedure extents and block counts. Radare2's automatic `aa` pass did not recognize `0x4952F0`; the entry was manually analyzed after disassembly showed the previous routine's `ret`, alignment NOPs, and the new routine's stack allocation at `0x4952F0`. No bytes in the executable were modified.

## C# comparison

The C# reference is `src/OpenIGI.Audio/AudioVolumeCurve.cs` in `/Users/haseeb-mir/Documents/Code/open-igi`. Its table builder casts the computed attenuation to `int`, truncating toward zero, and its tests assert `-999`, `-1999`, and `-2999` for exact powers of two. That disagrees with retail `FISTP`, which rounds according to the x87 control word (nearest-even under the normal game startup state). The C++ port follows the retail instruction's rounding behavior and uses nearest-rounded gain/pan table indices, as observed in `FUN_00496E60`.

## C++ implementation and verification

- Implementation: `engine/src/audio/sound_volume_curve.cpp`
- Public interface: `engine/include/igi/audio/sound_volume_curve.hpp`
- Test: `engine/tests/audio/sound_volume_curve_test.cpp`
- CMake target: `igi_audio`
- Test name: `retail_sound_volume_curve`

The focused test checks full scale, silence, exact halving checkpoints, nearest-rounded table indexing, pan sign/reverse stereo, pitch clamps, non-finite input handling, and monotonicity over the complete 10,001-input range.

**Verification status:** static algorithm evidence is available from Ghidra and radare2; the C++ unit test passes. The table has not yet been captured from a live retail process, and there is no direct audio-device or audible-output comparison. `std::log2` uses the host math library, so cross-platform last-bit identity with the x87 `FYL2X` implementation remains unproven.
