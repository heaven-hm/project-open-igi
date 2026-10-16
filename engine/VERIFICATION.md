# Gameplay verification notes

## Platform coverage

The game core must remain OS-independent: no operating-system APIs or headers
belong in gameplay, physics, AI, weapons, scripting, or original-format parsing.
Only platform backends may call OS APIs. The current product MVP targets macOS;
Windows and Linux backends are later milestones and are not currently supported
or verified. Keep future support claims tied to builds and runtime evidence on
each operating system.

The current CMake source has no OS-specific includes in the non-vendored engine
sources, and the core builds with the macOS toolchain. This source inspection and
build establish a portable code boundary only; they do not establish successful
Windows or Linux builds.

## Native application shell and fixed-step loop

- The macOS CMake target `igi_game` produces
  `engine/igi.app/Contents/MacOS/igi` and uses the C++ SDL2 backend in
  `engine/platform/sdl/sdl_application.cpp`. The portable `GameLoop` lives in
  `engine/src/game_loop.cpp` and currently advances at a provisional 60 Hz,
  caps catch-up at eight steps per frame, and reports discarded backlog. The
  60 Hz interval is scaffolding, not a retail timing measurement; measure the
  original loop before using it to drive gameplay physics.
- Configure/build command: `cmake -S . -B /tmp/project-open-igi-build` followed
  by `cmake --build /tmp/project-open-igi-build -j4`. AppleClang 16 configured
  C++20 with SDL2; all targets, including `igi_game`, built successfully for
  arm64.
- A direct launch with SDL_VIDEODRIVER=dummy logged that the C++ SDL loop
  started and remained active until stopped. `open -n` also launched the built
  macOS app bundle, and its exact executable remained alive until the smoke
  process was stopped. The macOS SDL window title displays frame and simulation
  counters; the app does not yet run gameplay updates or render a level.
- A temporary external C++ harness passed checks for sub-tick accumulation,
  60 Hz advancement, six-step catch-up, the eight-step cap, dropped backlog,
  non-finite elapsed time, and reset behavior. The harness was not added to the
  repository.
- Physical key and mouse events map to portable input codes in the macOS
  backend. The shared `InputState` retains held controls and one-tick press/release
  edges; the window consumes those edges once per fixed simulation step. The
  visible IGI gameplay bindings remain unverified and are not assigned.
- A temporary input harness passed checks for invalid input codes, held state,
  duplicate key-down events, one-tick press/release edges, a quick press-and-
  release between ticks, and reset behavior. Platform key-event mapping has
  compile coverage but has not been manually exercised in the app window.
- The portable fixed-step scheduler dispatches the fixed delta to a callback
  once for each simulation step. The macOS frame pump consumes one input
  snapshot in that callback. It does not yet call a gameplay-session update.
  `GameSession` updates, assets, player physics, level rendering, weapons, and
  AI are not integrated. The app is a loop shell, not a playable demo. The next
  implementation step is to call one central session update from this callback
  before adding more gameplay subsystems.
- The changed `game_loop.cpp`, `input_state.cpp`, `main.cpp`, and SDL C++
  translation unit each passed an individual compile or syntax check. A separate
  temporary callback harness confirmed three elapsed ticks produce exactly three
  fixed-delta callback invocations and consume a press edge only on the first
  tick; the harness also passed AddressSanitizer and UndefinedBehaviorSanitizer.
  The full CMake build then passed.

## Locomotion graph

- CMake builds `igi_gameplay` with AppleClang in C++20 mode.
- A native smoke program checked the idle transition count/order, walk/run
  pairing, jump/fall reachability, invalid-state handling, and that every edge
  targets one of the 49 states.
- A row-by-row comparison against
  `open-igi/src/OpenIGI.Game/Player/HumanLocomotionGraph.cs` found 49/49 rows
  identical. That is a source cross-check of the port, not an independent
  replay comparison against `igi.exe`.

## Retail binary observations

REA opened `/Users/haseeb-mir/Downloads/IGI/igi.exe` as PE32 x86 with SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, using
Ghidra 12.1.2.

- Evidence `ev_8052e7790e7178e7660cb649098e80d5c4ac7508ed25481bcfb1c640d50a6f9f`:
  body at `0x4614B0`, 1,468 bytes, 31 basic blocks, seven direct callees.
  This is a static observation of `Human_MovementIntegrator` as named by the
  local catalog; it does not establish the generated C translation is correct.
- Evidence `ev_bc129a3f925f0b0e4622e2cc92d7c84a5826ad0574fd19c3aa5d0c3f429046cb`:
  cataloged `HumanPlayer_UpdateMotion` at `0x408320` is a six-byte body with no
  direct calls in Ghidra's function analysis.
- Evidence `ev_efe7173636efc6f5820dd959dff90d3b1fb5e848fc7b29a209f85a5b54b787a0`:
  cataloged `HumanPlayer_ApplyPhysics` at `0x408420` is a 59-byte body with four
  blocks and one direct call.
- Evidence `ev_ca47b794e915c94e573d496819cfb0af31e173c8edb30e7882dfd60ae2cb8a92`:
  the 694-byte, 27-block function at `0x408460` subtracts `84.74169` from
  vertical velocity and adds movement-slot steering through the body basis.
  Steering magnitude comes from a runtime global, so the port accepts it as an
  argument instead of inventing a retail default.
- Evidence `ev_cabc8c58e5b97087e2544062c195e1782eec8ab45722dbda5d9271ff3c323a07`:
  the 175-byte, five-block ladder-slide routine at `0x40DBF0` multiplies X/Y
  velocity by `0.99` and evaluates `(Z - 44.600887) * 0.99`, in that order.
- Evidence `ev_178401a834b6929403a4049b09024de136b0422bd014a3605fbe8f9dfa6bf8b1`:
  the on-disk initialized bytes for the steering global at `0x56E1F4` are zero.
  That does not establish its runtime value after script setup.

The native smoke program also checked airborne gravity and steering, ladder
gravity/damping order, and root-motion scaling/suppression against the current
ported equations. Root-motion transformation is cross-checked against the
OpenIGI source; it has not yet been independently matched to a complete REA
instruction trace.

An AddressSanitizer build of `igi_gameplay` and the same smoke harness completed
with exit code 0 and no reported memory errors. LeakSanitizer is not supported
by this macOS toolchain, so leak detection was not verified.

The native catalog has 5,732 verified rows representing 100.00% coverage of the
Binary Ninja binary view (5,732 / 5,732) and 93.72% of radare2 functions (5,732 / 6,116):
5,244 `PASS` (CFG score >= 0.85), 487 `FAIL` (single-block thunks, accessors, and null
virtual stubs), and one `CANDIDATE`. The catalog covers all 10 engine subsystems:
Sound (215), Physics (297), Weapon (172), Weather/Environment (206), Animation (38),
Script/QVM (335), AI (534), Render/Display (457), UI/Menu (225), and System/Math (453).
Zero rows currently have `L5_runtime=true`. Catalog size and CFG gates are static
structural evidence, not proof of runtime behavior.

## re-agent source-only check

The local `auto-re-agent` Python source tree ran a source-only parity check of
the generated C at `0x4614B0`. It returned `RED`: its body contains decompiler
stub markers. This check had no live Ghidra backend input and is not a binary
parity result. The full native runner also remains an isolated harness: all
five representative calls fault without initialized engine state, so it does
not provide gameplay runtime evidence.

CrossOver replay of the original remains necessary before movement, weapon,
collision, animation, or sound behavior can be called runtime-verified.

## Retail asset catalog probe

`igi_asset_probe <game-folder> [mission-number]` builds and scans the retail
install in place; it does not copy or modify game assets. On the local IGI
install, mission 1 discovery selected
`MISSIONS/location0/LEVEL1`. The converter streamed 92 RES archives and
45187 resources, including 6794 MEF model entries; it found 998 QVM files and
468 standalone TEX/SPR/PIC files. The level-specific graph filter parsed 11
DAT graphs. Counts without a mission filter (archives, QVM, and standalone
textures) cover the full install. These parser results verify file access and
format recognition, not visual correctness or engine behavior.

Run with:

```sh
cmake -S . -B build
cmake --build build
./build/engine/igi_asset_probe /path/to/IGI 1
```

An AddressSanitizer build of `igi_asset_probe` also parsed the retail install
with exit code 0 and no AddressSanitizer report (`detect_leaks=0`). Leak
detection was disabled; this is not a leak check.

## DirectDraw / Direct3D static findings from REA

REA's connected session analyzed the same retail PE32 x86 binary and SHA-256
above with Ghidra 12.1.2. The user's linked local `re-agent` 6.4.0 CLI
independently reproduced the procedure identity and referenced string with
Ghidra 12.1.3 under Evidence
`ev_af4ac08834f8c3ed10783ac055b58cbb96e996e210c29b6cfca3d578a4603db7`.
Static Evidence `ev_587dd2c9088f58d96bf6489ed93ac3d23a3f705ab521aca10d0d5c805ff3d1cc`
links the literal `Direct3D` to `FUN_00493580` at `0x493580`. Its 663-byte
body registers Direct3D callbacks and sets render-related globals. The
function name is a derived interpretation from the string/reference and
pseudocode, not a recovered source symbol. `ev_1c752bcbb63ba8d72adf588d2884bcea94ef652e9306dd27fc753860d68f9ba6`
records imports for `DirectDrawEnumerateExA` and `DirectDrawCreateEx`;
`FUN_00491EF0` calls enumeration with flag 7, while `FUN_00498480` resolves
DirectDraw entry points dynamically and proceeds through DirectDraw surface
and DirectMusic setup paths.

These are static observations. The copied native catalog's
`Display_InitDirectDraw` label at `0x4924D0` does not resolve as a Ghidra
function entry in this analysis session, so it is not treated here as a
verified procedure identity. Runtime API/state capture and pixel comparison
against CrossOver are still required before selecting and validating a macOS
render backend. No enhanced rendering effects are part of the target.

## CrossOver runtime attempt

The linked re-agent 6.4.0 `capture_process_scenario` launched CrossOver's
`cxstart --bottle project-igi --no-update --wait --workdir C:\IGI
C:\IGI\igi.exe window level1` in the bottle containing the same executable
digest. The sample observed `igi.exe` and CrossOver processes, but provided no
interactive gameplay state or input trace. At its 35-second limit, REA reported
`cleanup_incomplete`: process ownership for a live `explorer.exe` was ambiguous,
so it retained the temporary capture root and did not claim cleanup. A later
read-only process check found no `igi.exe` or matching CrossOver PIDs. The
partial run is not a verified level-load or gameplay capture; do not infer
movement, weapon, AI, or rendering behavior from it.


## Mission navigation graph catalog and world transform

`GameAssetCatalog` now retains every valid `GRAPH<number>.DAT` file under the selected mission's `GRAPHS` directory, keyed by its retail graph ID. Each entry preserves node positions, node criteria, radius/material metadata, node links, and typed edges. The previous first-valid-graph behavior exposed only `GRAPH1.DAT` and silently dropped the graph selected by mission AI tasks.

On the local mission 1 assets, the C++20 `igi_asset_probe` build loaded 11 graphs. It reports graph 4019 with 264 nodes and 876 edges, alongside the other ten graphs. This is evidence of parser/catalog integration only; it does not verify AI route selection or retail pathfinding behavior.

Command used:

```sh
cmake --build /tmp/project-open-igi-build -j4
/tmp/project-open-igi-build/engine/igi_asset_probe /Users/haseeb-mir/Downloads/IGI 1
```


The portable `AiNavigationTransform` implements the observed graph-position transform at retail address `0x004F9770`: add the graph task offset; when the parent-orientation flag is active, multiply by the parent basis rows; then add the parent world position. REA/Ghidra evidence `ev_4616cc984ea6e431129942a1ec95613e7a7735a416a8f9397c8a21c2f313833b` records the function body and operand order. This is a static translation of one transform helper, not verification of graph selection, AI routing, or in-level behavior.
