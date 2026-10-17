# Terrain and attachment checkpoint — 2026-10-17

## Evidence identity

Retail PE32 x86 SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Original byte ranges/digests and REA evidence IDs: [native-source-identity.json](native-source-identity.json).
These identify the analyzed input; they do not prove port instruction identity or full behavior parity.

## Measured progress

- Windows x86 attachment-accessor fixture tests passed under CrossOver. `Render_DrawBoneMesh_2`
  (`0x004D0CF0`) follows two +0x0C pointers, then MESH+0x8C and a 68-byte record stride.
  `Render_DrawSplineMesh_2` (`0x004D0D10`) sign-extends the count at MESH+0x7E.
- `Car_GunCreate` (`0x004C4950`, provisional catalog name) consumes attachment records and creates
  child tasks through the magic-object type registry and task/event dispatch. A recursive mesh draw
  is not a faithful substitute. Factory integration and retail replay remain open.
- REA/Ghidra and Binary Ninja agree that the terrain loader starts at `0x004C6240`.
  Binary Ninja reports 25 blocks and 1,069 source bytes. Static research, not an integrated terrain port.
- The terrain loader resolves `.bit`, optional `.hmp`, `.cmd`, `.ctr`, then `.qsc`/compiled QVM.
  BIT and HMP begin with 256 records of 12 bytes. BIT data starts at +0xC00; HMP advances by
  `(dimension + 1)^2` bytes. CTR uses 32-byte records and relocates each +0x1C field against CMD data.
  Terrain material/tile natives are registered before the terrain script runs, then unregistered.
- BIT/HMP state labels `DAT_00B81A20`/`DAT_00B81920` are 256-byte arrays written by the loader.
  These must become owned storage in the port; the original labels are not usable port pointers.
- Seven performance presets were read at VA `0x00537B74`: 414, 430, 13, 96, 77, 65, 0.
  Highest-quality semantics and renderer consumers still require confirmation; no invented ultra mode.

## Remaining before terrain acceptance

Recover loader callees and global initialization, terrain QVM registration/execution, material/tile
layouts, the height/contour consumers, renderer dispatch, and matching placement/camera transforms.
Then implement in dependency order, run corpus/fixture tests and retail comparisons, render the terrain,
and capture four actual back-buffer images for review. No terrain rendering or full native parity is
claimed at this checkpoint. Audio remains deferred.

## Reference policy

C# open-igi, C++ project-igi-editor and project-igi-converter are navigation aids only. Their format
knowledge may locate retail consumers; engine behavior must be transcribed from verified igi.exe
analysis. Developer inspection controls and diagnostic logs are the explicit host-tooling exceptions.

## Independent instruction comparison

The Windows x86 differential test read the two original ATTA leaf bodies from the hash-verified
retail PE, verified the complete instruction bytes, and executed those position-independent bodies
against the same allocated records as the C++ functions. All 16,384 valid record/base combinations
and all 65,536 signed attachment-count values matched. This is instruction-level differential
evidence for the two leaf operations, not a live retail task/factory integration test.

## All-level terrain corpus audit

`python3 scripts/verify_terrain_native_layout.py --game <IGI-directory> --output <report.json>`
performs read-only source-range checks. It does not render terrain and adds no game logic.
The loader's 32-byte CTR relocation stride is observed; the old guessed bounding-box/quadtree
layout remains unsupported. The vertex consumer uses an 8-byte command header and signed
16-bit triangle-byte/base-vertex/extra-vertex fields before 4-byte packed vertex data.

| Level | CTR records | Out-of-range command offsets | Inconsistent command blocks | HMP |
| --- | ---: | ---: | ---: | --- |
| level1 | 11022 | 0 | 0 | present |
| level2 | 11024 | 0 | 0 | absent (optional) |
| level3 | 11070 | 0 | 0 | present |
| level4 | 11267 | 0 | 0 | absent (optional) |
| level5 | 11167 | 0 | 0 | absent (optional) |
| level6 | 11236 | 0 | 0 | present |
| level7 | 12387 | 0 | 0 | absent (optional) |
| level8 | 12706 | 0 | 0 | present |
| level9 | 11595 | 0 | 0 | present |
| level10 | 11053 | 0 | 0 | present |
| level11 | 11267 | 0 | 0 | absent (optional) |
| level12 | 11075 | 0 | 0 | absent (optional) |
| level13 | 12387 | 0 | 0 | absent (optional) |
| level14 | 15202 | 10174 | 4226 | absent (optional) |

All BIT tables and all present HMP tables walk their files exactly under the retail loader's
record/size rules. Eight HMP files are absent in this installation. Level 14 has 14,400 problematic
command references in total; do not silently fabricate geometry for them. Renderer/task reachability
still needs checking before deciding how retail handles that level.

## Further naming correction

REA `ev_7aed9441d2601cd76460f960c36d8580de03f30f95083342ba8e51ae9663898c`
shows that `0x00512040`, catalogued as Terrain_SampleHeightmap, copies vertex/interpolant records and
branches on per-vertex 0x10 flags before calling 0x005122B0. It is not evidence of an HMP sampler.
The signed-byte height reads, bilinear arithmetic, bias 64 and scale 256 are observed inside
`0x00510010` instead (REA `ev_ba370d099419afc676d7082c9828a35e206e9a7de421482b56f8582b67ffb169`).

The next native transcription obligations are the traversal's globals/callbacks at 0x004D14E0,
its output descriptor layout and the 0x00510010 vertex/material consumer. Terrain rendering, highest
profile integration, human camera, movement/collision and jumping remain incomplete.

Verification-tool regression: all six `scripts/tests/test_verify_terrain_native_layout.py` tests passed,
including optional HMP, native BIT truncation, corner-grid sizing and invalid command bounds.
