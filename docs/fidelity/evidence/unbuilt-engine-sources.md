# Engine sources that are not in the build

Audited 2026-10-25 against `CMakeLists.txt`. **52 of the 466 engine `.cpp` files are not compiled or linked** - 414 are listed, 52 are not.

## Why this matters

An unbuilt native is not a transcription in any useful sense: it cannot be checked against retail at runtime, it violates the no-dead-code rule, and it silently
shrinks every "how much is left" count. This audit exists because chain-worker reported `0x00498460` as "transcribed but unlinked" and the claim needed checking
against the whole tree rather than one file.

## Classification

- **Duplicate retail VA with a linked file: 0.** No rule-13 violation hide here - every unlisted file carries a VA that no linked file covers, or no VA at all.
- **Pending integration: 52.** These are unbuilt, not duplicated. They split into two kinds: transcribed retail natives, and port scaffolding that has no retail VA
  (`input/input_port.cpp`, `input/dinput_globals.cpp`, `input/dinput_image_data.cpp`).

## First real integration test, and where it stopped

Adding 42 of them to the build (all except the ten `native_419xxx` files the BackgroundFX wave is still writing) compiles cleanly and fails only at link, with
**10 unresolved symbols**: `InputPort::Run`, `SetKeyDown`, `SetMouseButtonDown`, `AddMouseDelta`, `Remap`, `ActionPressed`, `ActionDown`, plus `g_54875c` and
`g_548638`. So the backlog is close to linkable: the input subsystem is missing the other half of its own class, not an unknown subsystem.

## The 52 files

| file | retail VA | provenance status | bytes |
| --- | --- | --- | --- |
| `engine/src/retail/audio/native_004ebc60.cpp` | 0x004EBC60 | TRANSCRIBED | 2091 |
| `engine/src/retail/core/native_00530a80.cpp` | 0x00530A80 | TRANSCRIBED | 2101 |
| `engine/src/retail/core/native_005310e0.cpp` | 0x005310E0 | TRANSCRIBED | 1581 |
| `engine/src/retail/core/native_4192e0.cpp` | 0x004192E0 | TRANSCRIBED | 3843 |
| `engine/src/retail/core/native_419330.cpp` | 0x00419330 | TRANSCRIBED | 11004 |
| `engine/src/retail/core/native_419540.cpp` | 0x00419540 | TRANSCRIBED | 7247 |
| `engine/src/retail/core/native_419630.cpp` | 0x00419630 | TRANSCRIBED | 8846 |
| `engine/src/retail/core/native_419760.cpp` | 0x00419760 | TRANSCRIBED | 6094 |
| `engine/src/retail/core/native_4197f0.cpp` | 0x004197F0 | TRANSCRIBED | 2068 |
| `engine/src/retail/core/native_419800.cpp` | 0x00419800 | TRANSCRIBED | 2145 |
| `engine/src/retail/core/native_419820.cpp` | 0x00419820 | TRANSCRIBED | 4746 |
| `engine/src/retail/core/native_419860.cpp` | 0x00419860 | TRANSCRIBED | 7026 |
| `engine/src/retail/core/native_419950.cpp` | 0x00419950 | TRANSCRIBED | 4438 |
| `engine/src/retail/dynobj/native_0051a1d0.cpp` | 0x0051A1D0 | TRANSCRIBED | 2152 |
| `engine/src/retail/input/dinput_globals.cpp` | (no retail VA in provenance) | - | 1587 |
| `engine/src/retail/input/dinput_image_data.cpp` | (no retail VA in provenance) | - | 3532 |
| `engine/src/retail/input/input_port.cpp` | (no retail VA in provenance) | - | 3438 |
| `engine/src/retail/input/native_48f090.cpp` | 0x0048F090 | TRANSCRIBED | 779 |
| `engine/src/retail/input/native_48f0a0.cpp` | 0x0048F0A0 | TRANSCRIBED | 796 |
| `engine/src/retail/input/native_48f270.cpp` | 0x0048F270 | TRANSCRIBED | 857 |
| `engine/src/retail/input/native_48fa80.cpp` | 0x0048FA80 | TRANSCRIBED | 3696 |
| `engine/src/retail/input/native_48fd90.cpp` | 0x0048FD90 | TRANSCRIBED | 1961 |
| `engine/src/retail/input/native_48fed0.cpp` | 0x0048FED0 | TRANSCRIBED | 2454 |
| `engine/src/retail/input/native_490120.cpp` | 0x00490120 | TRANSCRIBED | 2038 |
| `engine/src/retail/input/native_490190.cpp` | 0x00490190 | TRANSCRIBED | 2601 |
| `engine/src/retail/input/native_490230.cpp` | 0x00490230 | TRANSCRIBED | 3477 |
| `engine/src/retail/input/native_4ae730.cpp` | 0x004AE730 | TRANSCRIBED | 895 |
| `engine/src/retail/input/native_4af740.cpp` | 0x004AF740 | TRANSCRIBED | 889 |
| `engine/src/retail/input/native_4ed510.cpp` | 0x004ED510 | TRANSCRIBED | 2346 |
| `engine/src/retail/input/native_507ec0.cpp` | 0x00507EC0 | TRANSCRIBED | 2239 |
| `engine/src/retail/input/native_509cc0.cpp` | 0x00509CC0 | TRANSCRIBED | 1232 |
| `engine/src/retail/input/native_509e90.cpp` | 0x00509E90 | TRANSCRIBED | 1901 |
| `engine/src/retail/input/native_509f00.cpp` | 0x00509F00 | TRANSCRIBED | 1256 |
| `engine/src/retail/model/native_4b6ce0.cpp` | 0x004B6CE0 | TRANSCRIBED | 1695 |
| `engine/src/retail/model/native_4b6d20.cpp` | 0x004B6D20 | TRANSCRIBED | 1654 |
| `engine/src/retail/render/native_00491db0.cpp` | 0x00491DB0 | TRANSCRIBED | 892 |
| `engine/src/retail/render/native_00491dc0.cpp` | 0x00491DC0 | TRANSCRIBED | 894 |
| `engine/src/retail/render/native_00491dd0.cpp` | 0x00491DD0 | TRANSCRIBED | 888 |
| `engine/src/retail/render/native_00491de0.cpp` | 0x00491DE0 | TRANSCRIBED | 890 |
| `engine/src/retail/render/native_00491df0.cpp` | 0x00491DF0 | TRANSCRIBED | 896 |
| `engine/src/retail/render/native_00491e00.cpp` | 0x00491E00 | TRANSCRIBED | 886 |
| `engine/src/retail/render/native_00491e10.cpp` | 0x00491E10 | TRANSCRIBED | 890 |
| `engine/src/retail/render/native_00491e20.cpp` | 0x00491E20 | TRANSCRIBED | 894 |
| `engine/src/retail/render/native_00491e30.cpp` | 0x00491E30 | TRANSCRIBED | 894 |
| `engine/src/retail/render/native_00491e40.cpp` | 0x00491E40 | TRANSCRIBED | 902 |
| `engine/src/retail/render/native_00491e50.cpp` | 0x00491E50 | TRANSCRIBED | 892 |
| `engine/src/retail/render/native_00491e60.cpp` | 0x00491E60 | TRANSCRIBED | 894 |
| `engine/src/retail/render/native_004da1d0.cpp` | 0x004DA1D0 | TRANSCRIBED | 1983 |
| `engine/src/retail/render/native_004e7c50.cpp` | 0x004E7C50 | TRANSCRIBED | 1642 |
| `engine/src/retail/render/native_490340.cpp` | 0x00490340 | TRANSCRIBED | 743 |
| `engine/src/retail/render/native_490350.cpp` | 0x00490350 | TRANSCRIBED | 772 |
| `engine/src/retail/script/native_5186c0.cpp` | 0x005186C0 | TRANSCRIBED | 2884 |
