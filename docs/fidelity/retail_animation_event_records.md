# Retail animation-event record preservation

## Scope and status

The IFF parser already decoded `BOEH`/`BOED` records but the gameplay animation layer discarded them. `SkeletalAnimationClip` now retains each event identifier, bone identifier, timestamp, and three raw position components in source order. Level 1 regression coverage loads the retail player and first-person animation sets and checks their event totals and representative reload/fence-walk payloads.

This is data preservation only. It does not claim that animation events are dispatched, that their coordinates are transformed into world space, or that the fourth `DefineAnimTrigger` argument has a known meaning. No sound trigger is played by this change.

## Retail evidence

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

- REA/Ghidra 12.1.2 at `0x004ebe70`, evidence `ev_2d7b47a46b932c80c0620b200ed3e400c3eaa76f39a706b6d6f489db6e77e037`: a 59-byte function registers the `DefineAnimTrigger` native and passes callback address `0x004ebeb0`.
- Ghidra does not recognize `0x004ebeb0` as a function entry; REA's `analyze_function` correctly rejects it. Independent radare2 analysis after decoding the retail image identifies a 264-byte routine at that address (3 basic blocks, 71 instructions), beginning after the registering routine's `RET` and five alignment NOP bytes. The boundary is independently supported, but callback parameter semantics still need runtime confirmation.
- The original `animtrigger/animtrigger.qvm` is LOOP 8.5 and decodes with `igi1conv` to 50 `DefineAnimTrigger` registrations. This proves the retail script contains the entries; it does not prove callback behavior.
- The original `common/ANIMS/000.IFF` contains 122 clips and 73 BOED events; `005.IFF` contains 78 clips and 52 events. A temporary C++ inventory probe linked directly against the repository's `iff_parser.cpp` observed those counts and the payloads checked by `mvp_original_animation_event_records`.

The C# implementation is not used as evidence for the stored field layout or dispatch behavior. Retail binary callback behavior and matched runtime traces remain the authority.

## Trigger table port

`AnimationTriggerRegistry` now reads the original trigger QVM into a source-value table. It preserves the event ID, task-type symbol, sound-definition name, and the final argument's text and numeric value. It does not allocate a guessed numeric `TASKTYPE_ANIMSOUND` constant and does not interpret the final argument. The test confirms all 50 entries map to playable Level 1 sound definitions, including Glock reload event 1000 and fence-walk event 1025; ID 1010 remains absent.

The initial table-only implementation did not connect events to playback. The next subsection records the current first-person dispatch slice and its verification limits.

## Port playback slice: first-person weapon animation events

The Level 1 runtime now compares consecutive first-person weapon animation times, collects mapped BOED events crossed by that interval, and resolves each event through the retail `DefineAnimTrigger` source table and `SoundSystem_PlayDefinition`. An event on the old interval boundary is excluded; an event on the new boundary is included. When a clip wraps, the interval is split at the duration boundary. Events without a `DefineAnimTrigger` entry are not assigned an invented sound; this includes event 1010.

This schedule is explicitly **C#-derived / not retail-runtime verified**. The C# `BoneClip.CrossedEvent` uses an open-old/closed-new interval and handles wraparound as two ranges. REA/Ghidra evidence `ev_372d81b9077615fe3d7fdf6a2480e59ffe4ce1c7e1397426f407b39a94fe65ff` confirms retail `0x004d65b0` writes an animation-time value to a clip record at offset `+0x2c`; evidence `ev_e4d0847e485a7159b606774a303e0fbf12e37d88823ee971a2d9096ecbdbe670` shows a retail caller computes and stores a scaled clip time. REA evidence `ev_f442484e179522795381640837f926566ff75ebb3e6bcafee4b7f5465729ca85` reads the callback bytes at `0x004ebeb0`; the callback routes a task type through two indirect handler tables. These observations support the animation clock and task-registration path but **do not verify the interval inequalities, sound start tick, spatialization, or fourth QVM argument semantics**.

The focused Level 1 test uses the original `005.IFF` Glock reload clip 17: crossing timestamp 2577 dispatches event 1000 to `glock_reload_1`, creates an active voice from the original sound bank, produces non-zero mixed PCM, does not duplicate an open-boundary event, preserves both sides of a synthetic wrap interval, and rejects non-finite times. This verifies the port's scheduler, asset linkage, and mixer path, not matching retail output. A paired CrossOver/igi-probe trace is still required before marking timing or sound placement as retail parity.

REA/Ghidra evidence `ev_2d7b47a46b932c80c0620b200ed3e400c3eaa76f39a706b6d6f489db6e77e037` confirms native registration at `0x004ebe70` and callback pointer `0x004ebeb0`. REA could not classify the callback as a Ghidra function entry. Radare2 independently identified its 264-byte extent, but callback effects still need a controlled runtime trace before implementing dispatch semantics.

CTest: `retail_level_one_animation_trigger_registry`.

## Port files and regression

- `engine/src/formats/parsers/iff_parser.cpp` decodes the source `BOED` fields.
- `engine/include/igi/skeletal_animation.hpp` and `engine/src/skeletal_animation.cpp` preserve the fields without coordinate conversion.
- `engine/tests/mvp_skeletal_probe.cpp` checks 73 player events, 52 weapon-rig events, three Glock reload IDs/timestamp/bone/position fields, and ten ordered fence-walk events.
- CTest: `mvp_original_animation_event_records` (requires `IGI_GAME_PATH`).

Focused Release verification on 2026-10-17: `mvp_skeletal_animation` and `mvp_original_animation_event_records` passed (2/2).

## Next required work

Establish the `DefineAnimTrigger` callback argument contract and event interval using retail disassembly and a controlled Level 1 runtime capture. Then compare the port's first-person event timing and sound parameters with the retail trace. Extend the same verified dispatcher to player-body and AI animation clips only after their task and coordinate contracts are established. Do not mark animation sounds as complete until those steps pass.
