# Retail player fall-impact sound cues

## Ground truth

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`, PE32 x86, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

REA/Ghidra 12.1.2 analyzed native `0x00408460` (694 bytes, body `0x00408460–0x00408715`), evidence `ev_63600d5cab4a5c2a4978fbb351941a3e746a0a3ae2811039431b93fb1fdca044`. The routine applies gravity (`84.74169` units/tick), computes impact speed as `abs(v) * 1/4096 * 30`, subtracts the safe threshold (`14 m/s`), and scales positive damage by `1/13 * maximum health`. It selects `player_fall_1` below 10% maximum-health damage, `player_fall_2` from 10% to below 20%, and `player_fall_3` at or above 20%. The strings are present in the retail executable at `0x00538D5C`, `0x00538D4C`, and `0x00538D3C`, respectively. For positive damage, the routine passes the selected key and player task to `FUN_004E6B00`.

REA analyzed `FUN_004E6B00` as a 246-byte function at `0x004E6B00–0x004E6BF5`, evidence `ev_1148119312a94c77ec368da02eabf057c0b59dbe298fa2fb3f057b1ad120e75b`. It resolves a named event through registered task handlers and creates/updates the sound task. This is static evidence from the shipped executable, not a runtime trace.

## C# reference

- Formula and cue thresholds: `/Users/haseeb-mir/Documents/Code/open-igi/src/OpenIGI.Game/Player/HumanFallImpact.cs`.
- Event consumer: `/Users/haseeb-mir/Documents/Code/open-igi/apps/OpenIGI.Desktop/DesktopGame.Mission.cs`, `ResolvePlayerFallImpact`; it plays a non-empty fall key through the sound definition registry.

The C++ port preserves the strict threshold comparisons and queues one cue for the landing tick. The game loop consumes it once and routes it through `SoundSystem_PlayDefinition`.

## C++ implementation and verification

- Calculation: `engine/src/human_motion.cpp`, `engine/include/igi/human_motion.hpp`.
- Landing event: `engine/src/game_simulation.cpp`, consumed by `engine/src/main.cpp`.
- Tests: `mvp_movement_weapon_ai` checks safe, light, medium, and heavy impacts plus one-shot delivery; `retail_level_one_sound_playback` resolves and starts all three named sounds from the actual Level 1 sound bank.

These tests verify formula selection, event lifecycle, resource resolution, and mixer voice creation. They do not prove audible output or timing parity with DirectSound on the retail executable.
