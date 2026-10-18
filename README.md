# Project Open IGI.

Project Open IGI is a native implementation of the original game engine. Engine routines are ported from reverse-engineering evidence taken directly from the retail `igi.exe`, with `retail names`, `virtual addresses`, `calling conventions`, and `evidence references` retained where known.

This repository is the game-engine implementation. It is not an editor, converter, helper library, or gameplay rewrite based on another project. The retail executable is the behavior authority. open-igi may identify candidate implementations, but REA/static/runtime evidence from retail igi.exe must verify behavior used here. Unknown behavior remains explicitly unresolved rather than guessed.

## Current status

The active Level 1 MVP is documented in MVP.md, with its required implementation sequence in plan.md: build the Windows x86 baseline; render models at transforms from OBJECTS.QVM; render terrain and textures; verify all visual assets and placements; then implement the playable simulation, player, physics/interactions, AI/navigation, health/damage, weapons, gameplay animation/effects, QVM flow, with audio deferred.

### Current milestone: level loading, 3D models and terrain

The Windows 32-bit executable loads level scenes with textured 3D models and terrain
through DirectDraw 7 / Direct3D 7. These are diagnostic loading scenes. Playable
missions, combat, AI, collision and complete player simulation are not implemented.
Retail camera, placement, material and texture parity remain under verification.

```console
open-igi.exe window level1
open-igi.exe window devmode level1
```

Place the executable in your original game directory. The screenshots below are
existing scene captures, not evidence of playable gameplay or complete retail parity.
Visible vegetation, placement and terrain/material artifacts still need correction.

#### Level 1: models and terrain from four inspection views

![Level 1 diagnostic scene with textured 3D models and terrain, four camera views](screenshots/level1_terrain_4grid.png)

#### Additional level-loading views

![Diagnostic model and terrain loading captures from several levels](screenshots/terrain_4grid.png)

The earlier model-only loading record remains available in
[Task 1 evidence](docs/TASK1_MODEL_LOADING.md). These newer terrain views show the
current visual milestone; they do not mark Tasks 1–2 as behaviorally verified.

Primary native anchors: `Qvm_Load` (`0x004B80B0`), `Resource_Load` (`0x004B5F00`),
`DynObj_LoadModelTexturePackage` (`0x004CF080`), `DynObj_LoadModelMesh` (`0x004CF390`) and
`DisplayRender_DrawModel` (`0x0049DF60`). Names and addresses identify the research sources;
complete retail behavioral parity is not established by compilation or these screenshots.

Movies/cutscenes, vehicles, smoke effects, and the main menu are outside this playable-demo scope unless a Level 1 dependency is proven.

## Build

The target is a 32-bit Windows executable using the original DirectX 7 / DirectDraw platform APIs.
The game target rejects non-Windows and 64-bit toolchains. `./run.sh` launches
`open-igi.exe` after the production verification gate; `./run.sh --status` only
reports paths. Wine/CrossOver may run the Windows executable for local testing;
there is no macOS application target or `.app` deployment.
Launch also checks the executable hash against its verified build receipt and
recorded sources/compiler configuration. Missing or stale receipts block launch.

Requirements: CMake 3.20+, a C++20 compiler, and the Windows SDK or MinGW toolchain with DirectDraw 7 headers and libraries.

Use `./build.sh` for the enforced build. It checks every production native at
`VERIFIED`, requires CFG similarity of at least `0.85`, runs fresh angr analysis
and configured behavioral comparisons, then builds and tests. A blocked gate
does not authorize deployment, even if a separate host build passes.

For one method during development:

```sh
python3 scripts/native_gate.py --root "$PWD" \
  --file engine/src/retail/model/mef_native_attachments.cpp \
  --address 0x004D0D10 --threshold VERIFIED --cfg-threshold 0.85 \
  --report build/native-method-004D0D10.json
```

Comment formatting produces warnings; missing or stale binary-bound evidence,
logic disagreements and behavioral mismatches block verification. A method pass
does not qualify the whole application for release. The following commands run
host compilation and unit tests only:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

## Reverse-engineering workflow

All behavioral implementation claims must trace back to `igi.exe`. `Static analysis`, `dynamic analysis` and `memory inspection` confirm behavior where possible; focused tests record verified conditions and known limits. A catalog entry or clean decompilation alone is not runtime proof.

Tools used for analysis and verification:

- `Ghidra` — disassembly, decompilation, and data-structure analysis.
- `Binary Ninja` — independent disassembly, decompilation, and control-flow analysis.
- `radare2` — independent disassembly, control-flow, and binary inspection.
- `angr` — independent CFG recovery and bounded symbolic analysis; supplementary evidence.
- `IGI-Probe` — runtime instrumentation where supported; availability and coverage must be recorded.
- `RE-Agent` — orchestration of reverse-engineering analysis against the retail executable.

Per-routine evidence and implementation limits stay attached to native source and its analysis records. Tool output is evidence; it is not a substitute for the native engine implementation.

## Source layout

- engine/include/igi/retail/ — declarations for integrated retail routines.
- engine/src/retail/ — routine implementations, named and annotated with their retail addresses.
- engine/tests/ — focused tests for individual implementation slices.
- engine/REVERSE_ENGINEERING.md — provenance, evidence, and unresolved behavior.

## Windows runtime

The retail executable and this project target 32-bit Windows x86 and DirectX 7 / DirectDraw. The game is intended to run as a normal Windows game with its original assets. Keep Windows APIs in the Windows platform layer; shared engine routines retain retail behavior established from igi.exe.

## Developer inspection controls

Run `open-igi.exe window devmode level1`. F12 toggles the diagnostic free camera; F10 loads
the next level (Level 14 wraps to Level 1). F9 toggles **player-only cheat requests** for god mode,
unlimited ammunition and clips. This model scene has no playable health/inventory integration yet,
so F9 logs that limitation and does not grant functional gameplay cheats. All three shortcuts are
inactive without `devmode`; key auto-repeat is ignored. These are host tools, not retail-native claims.

The model-and-terrain grids above record the current loading milestone.
Gameplay screenshots from the retired SDL/OpenGL prototypes are not used here. See [AGENTS.md](AGENTS.md) for the evidence/review rules.

Current development checkpoint: Windows x86 model-and-terrain loading (not a playable or stable gameplay release).
[Windows test record](docs/DEVELOPER_CONTROLS_TEST.md) · [Terrain/ATTA evidence](docs/RETAIL_TERRAIN_ATTACHMENT_STATUS.md) · [Retail byte identities](docs/native-source-identity.json).

Evidence-backed names and historical aliases: [native naming record](docs/RETAIL_NATIVE_NAMING.md).

## License

Project-authored source is distributed under the [MIT license](LICENSE), copyright
2026 Heaven HM. Existing third-party notices remain applicable. The retail game,
its executable and its assets are not licensed by this repository. See
[license scope](LICENSE_SCOPE.md), including the status of screenshots.
