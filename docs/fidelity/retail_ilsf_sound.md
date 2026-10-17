# Retail ILSF sound parsing and decoding

## Scope

The C++ port in `engine/src/formats/ilsf_sound.cpp` recognizes the original ILSF container, validates its 20-byte header, and decodes PCM or ADPCM to signed 16-bit samples. This is a format and decoder slice; it does not yet provide the retail streamed-file reader, voice scheduler, mixer, DirectSound replacement, or sound-trigger runtime. Level 1 streamed music and ambience are therefore validated and decoded here, but are not yet played by the game.

C++ port entrypoints use descriptive native-style names. `SoundSystem_*` is a port naming convention, not a symbol name recovered from the executable. The retail routines currently have Ghidra-generated names `FUN_00495F70`, `FUN_00496280`, and `FUN_00496420`.

## Retail evidence

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`

- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- REA provider: Ghidra 12.1.2
- Loader at `0x00495F70`: 718 bytes, 24 basic blocks; REA procedure evidence `ev_ab711dc698b93f8ca5d3b05777759dacdc4dee8a2d085b49203a6751e188e7c9`. Decompilation evidence `ev_b83d3e432ec4f45b736a09f13db010e527f632434d5f932180b5454612bd86ee` shows the `ILSF` magic check and a switch for storage formats 0 through 3.
- ADPCM decoder at `0x00496280`: 409 bytes, 29 basic blocks; REA procedure evidence `ev_9848ca12e1e431e8dab244ed611ac16b8df65249c38181f7ab8b3cdafcc9497c`. Decompilation evidence `ev_7a60a515ec8a85fe9d561a02113d64ef1ab68ce48dae525481db4d1a2ba289a7` shows a high-nibble-first decoder, channel-interleaved frame loop, separate predictor/index state per channel, clamping to signed 16-bit, and update of the cached step for the following sample.
- Decoder state initializer at `0x00496420`: 34 bytes, 1 basic block; REA procedure evidence `ev_92af21e086ff941e5903fb2cf433257d4917d71a0340e06051535334a44689ad` and decompilation evidence `ev_7df35efa43d57ad4f59ef22afeae1dbb06ac6d349e9120e6793355f7548eebc2` show channel count plus zeroed predictors, step indices, and nibble state.
- Independent Radare2 analysis on the same executable reported matching extents and block counts for all three routines: `718/24`, `409/29`, and `34/1`.

C# cross-reference reviewed in `/Users/haseeb-mir/Documents/Code/open-igi/src/OpenIGI.Formats/Sound/`: `IlsfHeader.cs`, `IlsfFormat.cs`, `IlsfFile.cs`, `IlsfAdpcmDecoder.cs`, and `IlsfStreamReader.cs`. The current C++ decoder follows the retail sample equations and one-shot interleaving; the C# streaming reader remains the reference for the not-yet-ported streamed read/state boundary.

## Level 1 sample census and verification

The retail `MISSIONS/location0/LEVEL1/SOUNDS` directory contains 28 `.wav` files. They are ILSF containers, not RIFF/WAVE. The observed inventory is 26 ADPCM and 2 PCM files. `game_music.wav` is streamed stereo ADPCM at 22050 Hz; `m1_ambience.wav` is streamed stereo PCM at 11025 Hz. Some files have up to one trailing byte beyond the header-declared payload; the parser deliberately ignores those bytes, matching the C# reader and the retail frame-count-driven reads.

`retail_ilsf_sound_parser` checks all storage codes, PCM 8/16 conversion, mono/stereo ADPCM sample checkpoints, malformed headers, unsupported metadata, and truncated payload errors. `retail_level_one_ilsf_samples` parses and decodes all 28 installed Level 1 sounds and checks the two streamed assets' metadata. Both passed in `/tmp/project-open-igi-manual-build`.

## Remaining sound-engine work

This result does not establish sound-engine parity. Next work must port and verify the ILSF streaming reader, retail sound resource registration and resident/streamed load paths, voice allocation/update, the 32-voice mixer, sound positioning/attenuation, and gameplay sound triggers. Runtime comparison against `igi.exe` is still required before any sound behavior is called retail-verified.
