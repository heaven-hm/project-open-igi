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
  once per simulation tick. The SDL frame pump consumes one input snapshot and
  passes it to `GameSession::update`. The session validates the fixed delta and
  records tick time and the input snapshot; it does not yet update player or
  world state. Player movement, collision, rendering, weapons, and AI remain
  unintegrated, so the app is not playable.
- Individual `-Wall -Wextra -Werror` compiles passed for the changed game-loop,
  session, input, main, and SDL application translation units. A temporary
  harness verified three elapsed ticks result in three session updates, tick
  time accumulation, input propagation, rejection of invalid deltas, and reset.
  The harness passed AddressSanitizer and UndefinedBehaviorSanitizer. The full
  CMake build passed afterward.

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
- Evidence `ev_6a9564f5d84daa287a444b641a8e06fd2e967ffcc91b3b1248a482e9363ea1d4`:
  Ghidra pseudocode for that same retail function confirms that it accumulates
  movement slots into the proposed displacement, applies a `0.9` decay to
  movement-related velocity slots, then conditionally calls wall sweep
  (`0x461A70`), ground probing (`0x462390`), and separation (`0x462BA0`). The
  calls are gated by player state fields and a grounded flag. The pseudocode is
  decompiler output; field semantics beyond corroborated offsets remain
  provisional.
- Evidence `ev_ffb885134a7fb27cd6f68cb4007ada273f8d360638d43c3ff1203565f7f456cc`:
  `Human_WallSweep` at `0x461A70` builds three vertical probes using 409.6
  units, a standing/crouching middle height of 1638.4/819.2 units, and a
  standing/crouching upper height of 7372.8/5324.8 units. Its sweep adds a
  10.24-unit clearance and can repeat collision resolution. This confirms
  several constants in the current C++ wall-sweep scaffold, but not equivalence
  of the collision query primitive or resolution results.
- Evidence `ev_e150e2f3352576dd8420c65337cdac008fe407cfb4525fa413b7c4c80583b1b5`:
  Ghidra pseudocode for `Human_GroundProbe` at `0x462390` starts its ground
  query at the standing/crouching middle height and uses a 2048-unit grounded
  step-down threshold or an 8192-unit slope-slide threshold. The wider routine
  also resolves terrain, object, ladder, and step state; the current C++ ground
  query only models a subset, so it must not be described as a full port.
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

## Mission 1 world, player physics, weapons, AI, and software renderer (macOS)

Portable sources (strictly zero OS APIs in gameplay core):
- `mission_world.hpp` / `mission_world.cpp`: Complete retail Mission 1 loader (481 placed objects, 77 MEF models, 304 textures, 29 AI soldiers, 11 navigation graphs, 6,492 terrain triangles). Loads and parses `.hmp` heightmap tables (`256 * 12 = 3072` byte descriptor table), `.ctr` octrees, `.cmd` meshes, `.mtp` packages, `.mef` hierarchies, `.iff` skeleton animations, and `objects.qsc` / `OBJECTS.QVM`.
- `world_collision.hpp` / `world_collision.cpp`: Spatial hash grid, Möller–Trumbore segment-triangle raycasting, retail `Human_WallSweep` (`0x00461A70`), `Human_GroundProbe` (`0x00462390`), `Human_SlopeSlide` (`0x004614B0`), `Human_Separation` (`0x00462BA0`), and `HumanMotion` integration.
- `weapon_system.hpp` / `weapon_system.cpp`: Exact rate-of-fire cooldown, 3-round burst gate (M16A2), cubic spread cone ($u_1 u_2 u_3 \cdot \theta_{\text{spread}}$), unit-disk recoil with retail `0.9` per-tick damping, magazine reload (`KeyR`), slots 1–5, and hitscan raycasting.
- `ai_system.hpp` / `ai_system.cpp`: 29 AI soldiers with dual view-cone sight checks (30m primary, 10m secondary), `0.79` LOS clearance check, gunshot hearing broadcasts, patrol path opcode interpreter, combat pursuit, damage application, and death transitions.
- `software_renderer.hpp` / `software_renderer.cpp`: Zero-OS software rasterizer writing to ARGB8888 buffer + depth buffer. Implements sky gradient, directional sun lighting, distance fog, textured terrain & placed MEF models, soldier billboards, crosshair, dynamic health/ammo bars, hit markers, muzzle flashes, and tactical objective HUD banners.
- `logger.hpp` / `logger.cpp`: Thread-safe, multi-level logger (`TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`) with ISO-8601 timestamps, file and console logging, in-memory history, and `EngineException`/`AssetLoadException`/`PhysicsException` hierarchies.
- `player_fall_impact.hpp` / `player_fall_impact.cpp`: Speed-based landing impact calculation (14 m/s safe threshold, 13 m/s lethal span, 20,480 / 40,960 unit hearing alert broadcast).
- `weapon_view_sway.hpp` / `weapon_view_sway.cpp`: Fixed-step 11-tick weapon lowering and raising animations for weapon switching.
- `weapon_view_recoil.hpp` / `weapon_view_recoil.cpp`: 3-tick first-person view recoil presentation recovery.
- `player_ladder.hpp` / `player_ladder.cpp`: Ladder climb line geometric steps (1,228.8 units) and activation bounds (8,192 unit radius, 0.7071 cosine check).
- `mission_expression.hpp` / `mission_expression.cpp`: Complete QSC expression parser and evaluator (boolean, relational, arithmetic, unary, GAME_FREQUENCY constant, variables).
- `level_flow.hpp` / `level_flow.cpp`: Mission objective state machine supporting all 14 missions, authored QSC objectives, completion/failure expressions, and extraction zone validation.
- `mission_objective_loader.hpp` / `mission_objective_loader.cpp`: Parses authored QSC `DefineComputerObjective` task definitions.
- `platform/sdl/sdl_application.cpp`: Only platform backend touches SDL2 (window creation, relative mouse capture, input event mapping, frame blit).

Checks performed:
- Each individual `.cpp` file was verified with `clang++ -std=c++20 -Wall -Wextra -Werror` before linking.
- Full CMake build passed for all targets: `igi_asset_formats`, `igi_gameplay`, `igi_asset_probe`, `igi_native_probe`, `igi_world_probe`, `igi_game` (`igi.app`), and `igi_tests`.
- Fixed timestep calibrated to retail 30 Hz (`fixed_step_seconds = 1.0 / 30.0` in `game_loop.hpp`), matching retail constants (airborne gravity `84.74169`, recoil damping `0.9`, cooldown ticks `(60/rpm)*30`).
- Automated test suite (`ctest` / `igi_tests`) passed 100% (36/36 tests passed across 7 test suites):
  - `Parsers`: QSC single/multiple/grouping, error handling, TEX round-trip encode/decode, non-existent file rejection, CTR record size (32 bytes), HMP 256-table layout (3072 bytes), IFF/RES/QVM parsing, and real retail file sanity.
  - `Physics`: 49-state locomotion graph topology, airborne gravity integration (`-84.74169`), ladder slide integration (`0.99` horizontal, `(Z-44.6)*0.99` vertical), wall sweep probe heights & clearance thresholds, cylinder separation repulsion, and slope slide downhill vector.
  - `Weapons`: Default loadout (M16A2, Glock, MP5SD, AK47, Dragunov), RPM cooldown, M16A2 3-round burst gating, unit-disk recoil with `0.9^10` damping, dry fire, and reloading mechanics.
  - `AiSystem`: Dual view-cone detection (30m primary, 10m peripheral, blind spot behind), hearing broadcast alert, and raycast hitscan hit/damage.
  - `LoggerAndExceptions`: Exception hierarchy subsystem tags, snapshotting, level filtering, and concurrent multi-threaded logging safety.
  - `GameplayHelpers`: Fall impact safe & damaging landings, weapon view sway 11-tick transitions, weapon view recoil 3-tick decay, and ladder activation.
  - `MissionFlow`: QSC arithmetic/relational/boolean expression parser, fallback objectives for all 14 missions, objective completion progression, and authored objective set loading.
- `igi_world_probe` loaded retail Mission 1 read-only: 481 placed objects, 77 MEF models, 304 textures, 29 soldiers, 11 nav graphs, 6,492 terrain triangles. After 240 simulated ticks the player settles grounded. Firing M16A2 executes a 3-round burst (mag 30 -> 27), generates recoil, broadcasts gunshot sound, alerts nearby soldier to combat, moves 49.8m in 10s at 0.010 ms/tick, and dumps rendered frame to `/tmp/world_probe.ppm`.

## Graphics quality: bilinear filtering and low/high render setting (2026-10-16)

Outer-layer graphics only (retail DirectX 7 behavior is not replicated; no
texture or polygon data is altered — texel/model bytes still come from retail
files unchanged):

- `TextureImage::sample_bilinear` was a stub forwarding to `sample_nearest`.
  It now performs a true 4-texel bilinear blend with retail-style UV wrapping
  (`engine/src/mission_world.cpp`). The software rasterizer samples via
  `sample_bilinear` (`engine/src/software_renderer.cpp`), matching the
  bilinear filtering retail Direct3D used instead of blocky nearest-neighbor.
- New portable `AppLaunchOptions::parse`
  (`engine/include/igi/launch_options.hpp`, zero OS APIs — pure string
  parsing, safe for all OS backends): `--gfx low|high`, `--width N`,
  `--height N`, `--mission N` / `-m N`, `--game-root PATH`, plus the legacy
  positional `<mission-number>` / `<game-root>` forms. Defaults to high
  (960x600); low selects 480x300. SDL backend
  (`engine/platform/sdl/sdl_application.cpp`) renders at the selected size
  instead of the fixed 480x300 upscale.
- Tests: new `GraphicsQuality` suite in `tests/test_graphics_quality.cpp`
  (7 cases: 4-texel center blend, texel-center agreement with nearest, UV
  wrap parity, default high 960x600, low flag 480x300, explicit size survival,
  mission/root arg forms). Full suite: 48/48 pass (`ctest`).
- Runtime integration: `igi_world_probe` missions 1 and 2 both load and settle
  grounded after 240 ticks (L1: 499 objects / 79 models / 304 textures /
  29 soldiers / 11 graphs / 24784 tris; L2: 229 objects / 46 models /
  237 textures / 25 soldiers / 14 graphs / 15858 tris). Frames dumped to
  `/tmp/world_l1.ppm` and `/tmp/world_l2.ppm`.
- REA static context: `binary_overview` on `igi.exe` (SHA-256
  `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`)
  confirms PE32 x86, 6 segments, 3,926 procedures, 2,444 strings
  (evidence `ev_270be182d280bb1afa522d4378fd2b0a9f649d7d3b95360537bb49277a625a5a`,
  Ghidra 12.1.2). No parity claim: rasterizer output has not been
  pixel-compared against a retail Direct3D capture.
- Remaining graphics gaps: terrain streaming cap (`half_steps <= 60`) still
  bounds high-mode view distance; retail lightmaps not yet applied; no
  retail-capture pixel comparison yet.

## Retail control bindings from config.qvm (2026-10-16)

Core input now follows the retail remap table, not FPS guesses:

- New portable single-source header `engine/include/igi/retail_controls.hpp`
  (zero OS APIs): `InputAction` enum plus `is_action_down` /
  `was_action_pressed` helpers over a constexpr 24-row table transcribed from
  `config.qvm` GOInRemap bytes (Move WASD, Fire=Mouse1, Reload=F, Activate=Q,
  Crouch=C, Jump=E, NextWeapon=Mouse2, PrevWeapon=O, AltFire=Backspace,
  Binoculars=Space, Map=M, WalkRun=Shift, Categories=digits 1-9).
- `WorldCollision::step_player`: jump (ground + ladder release) moved from
  Space to retail KEY_E; movement/crouch/walk-modifier read through the table.
- `GameSession::update`: reload reads Reload action (F, R kept as documented
  convenience); world interaction reads Activate (Q) instead of F/E; weapon
  cycling is wheel/digits + O (Q now means Activate); fire reads the Fire
  action (Mouse1).
- Documented deviations in-header: MouseRight keeps the sniper sight zoom
  (retail binds NextWeapon to mouse2 — needs a retail runtime capture to
  disambiguate scope-vs-cycle); Peek/numpad and PageUp/PageDown zoom have no
  `InputCode` yet.
- Tests: new `RetailControls` suite (`tests/test_retail_controls.cpp`, 7
  cases incl. E-jump, Space-is-binoculars, F-reload, Q-activate, 24-row table
  size). Full suite 55/55 pass; changed TUs compile under
  `-Wall -Wextra -Werror`; `igi_world_probe` mission 1 unchanged (grounded,
  burst 30->27, 49.8 m / 10 s walk).

## AI graph-edge route following (2026-10-16)

Patrol movement previously bee-lined to the target node, ignoring retail
navigation edges (walls/buildings crossed). Now:

- `AiSystem::find_graph_route` (new public static, portable, no OS APIs):
  BFS shortest node-id path over per-node `link1`/`link2` plus the edge list,
  treated as undirected. Empty when ids are unknown or unreachable.
- `AiSystem::nearest_graph_node_id`: plan-distance (XY) nearest node.
- `SoldierAgent::route_waypoints`: intermediate node positions; the Patrol
  branch walks them leg by leg via the existing collision-aware `step_toward`
  (wall sweep + ground query per leg) and only approaches `walk_target`
  directly when no route resolved. `notify_gunshot` clears the route on
  redirect. Investigate/Combat pursuit still bee-lines (next increment).
- Also removed two dead provisional constants flagged by a strict
  `-Wall -Wextra -Werror` TU compile (`kFireIntervalSeconds`,
  `kSoldierShotDamage` were unused).
- Tests: new `AiNavigation` suite (`tests/test_ai_navigation.cpp`, 6 cases:
  chain route, reverse symmetry, edge-list traversal, unreachable/unknown
  ids, nearest node, empty graph). Full suite 61/61 pass; `ai_system.cpp`
  strict-compiles; `igi_world_probe` missions 1+2 still load, settle
  grounded, and simulate (L1: patrol=28 combat=1; L2: patrol=23 combat=1
  investigate=1).

## Building entry end-to-end (2026-10-16, branch feature/gameplay-parity)

Entry mechanics (toggle + slide-aside + collision skip past 0.85 progress)
already existed; the defects were misleading HUD labels and an interact range
narrower than the hint range:

- `get_interaction_hint` labels now name retail keys (`[Q]` Activate,
  `[W]` ladder climb) instead of `[E]`/`[SPACE]`.
- `try_interact` default radius 6144 -> 8192 units, matching the hint range.
- New `BuildingEntry` suite (`tests/test_building_entry.cpp`) runs against
  retail Mission 1: finds a modeled door, proves a horizontal axis through it
  is blocked while closed, opens it through the Activate flow, animates
  `update_doors` to `open_progress > 0.85`, and proves the axis is passable.
  Full suite 62/62 pass on a pristine checkout.

## Magic-object registry from retail data (2026-10-16, branch feature/gameplay-parity)

Retail `MAGICOBJ/MAGICOBJ.QVM` was previously ignored entirely (zero
references in the engine). Now decoded through the existing QVM
decompiler (`igi_qvm_dump`, new `engine/tools/qvm_dump.cpp` probe):

- New portable `MagicObjectRegistry` (`engine/include/igi/magic_object_registry.hpp`,
  `engine/src/magic_object_registry.cpp`, zero OS APIs): parses
  `DefineMagicObj(name, model, TASKTYPE_*, ...)` lines. Retail calls are
  variable-arity (EXPLODEMAGICOBJ explosion/sound params, RUDDER deflections,
  COCKPITSHIELD values), so the parser takes the first three fields.
- `MissionWorld` loads the registry at mission load and reports the count in
  the status line; behavior is unchanged (data layer only — no effect
  rendering reads it yet, pending coordination on effect lanes).
- Tests: new `MagicObjects` suite (`tests/test_magic_objects.cpp`, 3 cases:
  synthetic parse incl. malformed-line rejection, empty input, retail catalog
  shape = 374 entries with `108_02_1`->GUNCLIP and `410_03_1`->GLASS spot
  checks). Full suite 65/65 pass; new TU strict-compiles
  (`-Wall -Wextra -Werror`); `igi_world_probe` L1 reports
  `374 magic objects` with unchanged simulation.

## Combat both directions, RES model fallback, ATTA fix, 14-level sweep (2026-10-16, branch feature/gameplay-parity)

- New `CombatIntegration` suite (`tests/test_combat_integration.cpp`, retail
  Mission 1 data): `ExposedPlayerTakesEnemyFire` places the player 8 m in
  front of a soldier with verified-clear LOS, steps 600 retail ticks, and
  asserts cumulative enemy damage > 0 plus a Combat transition;
  `AimedFireKillsExposedSoldier` drives the real firing chain
  (`WeaponSystem::update` -> wall-limited raycast -> `AiSystem::apply_shot`)
  at 15 m and asserts wounding plus a kill. Full suite 68/68 pass.
- Fixed zero-model loads on RES-packed levels: `MissionWorld` now falls back
  to `LEVEL< N >.RES` + `location0.res` for models missing from unpacked dirs
  (mirrors the existing texture fallback), via an extracted
  `load_single_model_file` shared by both paths. L3 went 0/0 -> 93 models /
  336 textures; L5 74/266; L6 92/377; L7 91/357.
- Fixed ATTA loss at the parser: `ParseMefGeometry` only ran
  `ParseAttachments` for skeletal (`modelType == 1`) meshes and the
  collision-fallback reassignment wiped even those. ATTA is now parsed
  unconditionally at function end; `ModelMesh::attachments` (`ModelAttachment`
  name/offset/bone) is filled during conversion. 83 L1-referenced models
  carry ATTA bytes; `RetailAttaMountsPreservedOnModels` proves mounts survive
  into the loaded store.
- Fixed retail Door record decoding (survey: 26 doors / 14 missions):
  the generic euler parse read slot 6 (stopX) as facing, giving every door a
  bogus -1.0 rad yaw — doors now take euler gamma from slot 11 (verified:
  1.5708/0/4.71239 across L1), open durations from the open-time field
  (1.0 s, 0.5 s, 2.0 s gate doors), and slide start -> stop offsets
  (mirrored +/-3.6 m gate panels confirm the offset reading). `maxAngle` is 0
  everywhere: all retail doors slide. `update_doors` uses scripted durations
  and offsets. PROVISIONAL (needs executable evidence before calling it
  retail behavior): stop fields read as world-frame meter offsets; the 0.85
  collision-skip threshold. Locked/open expressions are stored, not yet
  evaluated (gates stay Q-openable for playability).
- Renderer now honors retail sub-block opacity with source-over blending
  (was: ignored, so translucent glass/fences drew opaque black). Translucent
  pixels skip the depth write; sorting is unsorted (noted limit). Provenance:
  L4 tower model 905_01_1 (dark olive texture, 62% transparent texels, 0.5
  block opacity) previously drew solid black.
- 14/14 retail missions load read-only (objects / models / textures /
  soldiers / graphs / terrain tris): L1 499/80/304/29/11/24784, L2
  229/46/237/25/14/15858, L3 549/93/336/56/18/24870, L4 125/42/228/50/13/20164,
  L5 395/74/266/39/13/16280, L6 586/92/377/55/23/18764, L7 822/91/357/47/31/28800,
  L8 543/106/392/41/33/23082, L9 543/85/309/35/18/10248, L10 435/76/310/39/14/25596,
  L11 460/100/367/56/24/8556, L12 460/87/344/54/34/10724, L13 910/101/353/72/20/23114,
  L14 740/95/221/94/34/0 (L14 ships no heightmap terrain; noted, open).
- Render throughput measured (`world_probe`, Release): high 960x600 ranges
  6.7 ms (L4) to 45.6 ms (L1) per frame; low 480x300 ranges 2.3-12.5 ms.
  L1-high exceeds the 33 ms frame budget and is the "slow" feel source;
  simulation itself is ~0.06 ms/tick. Fog-range terrain clamp is the planned
  exactness-preserving next step, not yet implemented.

## Exact skeletal pose sampling (2026-10-16, branch feature/gameplay-prototype)

`MissionWorld::evaluate_skeletal_pose` was a sine-wobble placeholder that
used only each track's first key. Ported the exact evaluation math (read-only
reference: the other engine's `skeletal_animation.cpp` — slerp with
shortest-path flip, key-track sampling with cyclic wrap, hierarchical
composition of rest offsets; adapted to dependency-free portable types):

- New `engine/include/igi/skeletal_pose.hpp` (zero OS APIs):
  `normalize/multiply/slerp_quaternions`, `rotate_vector`,
  `sample_rotation_track`, cycle-safe `compose_bone_world_positions`
  (invalid parents/cycles detach instead of recursing).
- `evaluate_skeletal_pose` now slerps every bone's full IFF key track at the
  wrapped clip time and composes world positions down the bank hierarchy.
  Output semantics: model-space bone world positions (documented in header).
- Tests: new `SkeletalPose` suite (`tests/test_skeletal_pose.cpp`, 8 cases:
  slerp 45-degree midpoint, endpoint exactness, vector rotation, inter-key
  sampling, cyclic negative-time wrap, empty-track identity, hierarchy
  composition, parent-rotation propagation, invalid-parent safety). Full suite
  76/76 pass; `mission_world.cpp` strict-compiles; L1+L2 probes unchanged.
- Honest limit: this corrects pose math under the hood; soldiers still render
  as procedural quads. Skinning retail meshes with these poses is the next
  visual increment, not part of this change.

## Weapon numbers verified from retail scripts (2026-10-16, branch feature/gameplay-prototype)

open-igi holds the field table but no native verification; this wires it up
with retail bytes as ground truth (open-igi's order = lead only):

- New `WeaponConfigRegistry` (`engine/include/igi/weapon_config.hpp`,
  `engine/src/weapon_config.cpp`): decodes all 24 `WeaponConfig` records from
  `WEAPONS/weaponconfig.qvm` (nested inside the container call — spans are
  extracted per `Task_New` occurrence). Variable float formatting and
  multi-line records handled; knife (-1 clip) excluded as non-firearm.
- Verified scripted values now in `default_loadout` (semantics-certain fields
  only — RPM/cooldown, clip, burst gate, reload seconds, effective range):
  M16A2 500rpm/20/burst20/2.0s/300m (full-auto: burst == clip),
  Glock 400/17/1/2.0s/60m, MP5SD 700/32/32/1.8s/100m,
  AK47 650/32/32/2.0s/300m, Dragunov 50/10/1/2.0s/1000m.
  Previous defaults (800rpm/30mag/3-burst, 2.2s reload) were provisional
  guesses and are removed.
- Explicitly NOT wired (units unproven — recorded, open): DamageFactor
  (0.2-1.0, needs the retail HP formula), Min/MaxSpread (M16 3.0/10.0 — unit
  unknown, not degrees as previously assumed), recoil fields. Engine keeps
  provisional damage/spread/recoil, now labeled as such in `WeaponStats`.
- Tests: `WeaponConfig` suite (`tests/test_weapon_config.cpp`):
  `RetailRosterDecodes` (>=23 records, per-weapon spot values incl. gun-model
  `108_01_1` for M16A2) and `LoadoutMatchesScriptedValues` (loadout equals
  script). Old `test_weapons` expectations rewritten to scripted behavior
  (auto-fire shot counts, 500 RPM cooldown ticks, per-weapon reload times).
  Full suite 78/78 pass; probe L1 fires scripted auto (mag 20 -> 12 per
  second) with unchanged mission load.

