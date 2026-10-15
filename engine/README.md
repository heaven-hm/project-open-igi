# Gameplay engine work

`igi_gameplay` is the C++20 gameplay layer. It contains the retail human
locomotion transition table, observed movement equations, and a catalog for
original game files. Converter parsers are vendored under `formats/vendor`
with their upstream license notices in `formats/vendor/notices`.

This is still an engine prototype; it does not yet implement grounded movement,
collision response, weapon state, AI, renderer, or an app bundle. Those require
matching native body semantics and runtime traces from the retail executable;
do not infer them from a graph.

Configure from the repository root with `cmake -S . -B build`, then run
`build/engine/igi_asset_probe /path/to/IGI 1` to scan retail resources and the
selected mission graph. The probe reads in place and never copies game assets.
