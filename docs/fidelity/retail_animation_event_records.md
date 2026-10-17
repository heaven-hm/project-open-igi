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

This table is not yet connected to per-tick animation dispatch. Passing this test proves QVM parsing and asset-name linkage only, not that the retail callback plays a sound at a particular event time or position.

REA/Ghidra evidence `ev_2d7b47a46b932c80c0620b200ed3e400c3eaa76f39a706b6d6f489db6e77e037` confirms native registration at `0x004ebe70` and callback pointer `0x004ebeb0`. REA could not classify the callback as a Ghidra function entry. Radare2 independently identified its 264-byte extent, but callback effects still need a controlled runtime trace before implementing dispatch semantics.

CTest: `retail_level_one_animation_trigger_registry`.

## Port files and regression

- `engine/src/formats/parsers/iff_parser.cpp` decodes the source `BOED` fields.
- `engine/include/igi/skeletal_animation.hpp` and `engine/src/skeletal_animation.cpp` preserve the fields without coordinate conversion.
- `engine/tests/mvp_skeletal_probe.cpp` checks 73 player events, 52 weapon-rig events, three Glock reload IDs/timestamp/bone/position fields, and ten ordered fence-walk events.
- CTest: `mvp_original_animation_event_records` (requires `IGI_GAME_PATH`).

Focused Release verification on 2026-10-17: `mvp_skeletal_animation` and `mvp_original_animation_event_records` passed (2/2).

## Next required work

Establish the `DefineAnimTrigger` callback argument contract using retail disassembly and a controlled Level 1 runtime capture. Then load the QVM trigger map, dispatch events at the retail animation timeline, resolve the associated sound through the existing sound registry, and compare event timing and sound parameters with the retail trace. Do not mark animation sounds as complete until those steps pass.
