# Gameplay engine work

The game core is OS-independent: gameplay rules and original-format parsers
must not call operating-system APIs. Keep window, graphics, input, audio, and
filesystem APIs inside platform backends behind narrow interfaces. The MVP
backend targets macOS; Windows and Linux backends are later work and are not
currently supported or verified.

`igi_gameplay` is the C++20 gameplay layer. It contains the retail human
locomotion transition table, observed movement equations, and a catalog for
original game files. Converter parsers are vendored under `formats/vendor`
with their upstream license notices in `formats/vendor/notices`.

This is still an engine prototype; it does not yet implement grounded movement,
collision response, weapon state, AI, renderer, or an app bundle. Those require
matching native body semantics and runtime traces from the retail executable;
do not infer them from a graph.

Configure from the repository root with `cmake -S . -B build`.

Run `build/engine/igi_native_probe --stats` to inspect the 5,732 verified native
functions (100.00% coverage of Binary Ninja binary view) across all 10 engine subsystems:
Sound, Physics, Weapon, Weather, Animation, Script/QVM, AI, Render, UI, and System/Math.
You can query by address (`build/engine/igi_native_probe 0x401000`), by name
(`build/engine/igi_native_probe --name Config_SaveSoundOptions`), or by subsystem
(`build/engine/igi_native_probe --subsystem Sound`).

Run `build/engine/igi_asset_probe /path/to/IGI 1` to scan retail resources and the
selected mission graph. The probe reads in place and never copies game assets.
