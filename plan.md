# Level 1 Playable Demo Plan

This is the execution sequence for MVP.md, in the agreed build order:

1. **Load the 3-D models and their exact positions** (task one).
2. **Load the terrain with the correct textures** (task two).
3. **Screenshot each building / each version, add short comments, hand the packet to a human to review and test.**
4. **Verify every object is at its exact position and orientation.**
5. **Verify every texture is loaded correctly.**
6. Only then make it a **playable game** — engine tick, physics, player, enemies.
7. Finally **weapons, physics movement, AI graph navigation, health, movement, jump, weapon reload/shoot, animations, magic objects, and building interaction (stairs, doors)**.

Only after the whole thing works do we treat it as the playable demo. Nothing is skipped ahead.

> **Scope update:** Level 1 sound is part of the playable demo. Implement and verify its file loading, sound definitions, event dispatch, mixer, and audible gameplay output after the core gameplay path is integrated. Main-menu audio, movies/cutscenes, vehicles, and non-gameplay smoke/particle effects remain deferred.

## Repeatable work loop

Each task is one small, reviewable increment:

1. Inspect the current source, assets, and dirty Git state. Do not overwrite unrelated user changes.
2. Establish native behavior from retail `igi.exe` with REA/Ghidra. Cross-check boundaries or control flow with Binary Ninja/Radare2 as needed. Use RE-Agent for bounded reconstruction/parity work.
3. Use C# `open-igi` only to find candidate logic. Validate candidates against the retail binary before using them.
4. Implement one narrow C++20 slice with readable retail-native names/addresses, structured timestamped logs, and explicit errors for unsupported or unresolved cases.
5. Add focused tests for that slice, build the Windows x86 executable, and run the relevant scenario under CrossOver.
6. For each visible milestone, capture the actual game window and retain its run/build identity and logs.
7. Make a small local commit after verification. Do not push unless explicitly asked.
8. At each visual checkpoint, provide the screenshot and test record for owner manual review before advancing.

## Native verification rule (every task)

A native counts as verified only when `bn_function_info <address>` (Binary Ninja MCP) returns that exact name from the retail `igi.exe`. The catalog's own gate/score is recorded alongside.

- Machine-readable status for the demo scope: `assets/BinaryNinja/natives/CLEAN/VERIFICATION.md` — **3,050 unique natives, 3,035 name-verified, 15 name conflicts, 0 lookup errors.**
- **15 name conflicts** are known and must be used by **address only**, never by the catalog label: `0x00413A90`, `0x0043DCF0`, `0x00443840`, `0x00443A00`, `0x00443A10`, `0x004468F0`, `0x00446CE0`, `0x00447A80`, `0x0045DCB0`, `0x00475CC0`, `0x00475D80`, `0x00476EA0`, `0x00476F70`, `0x00482650`, `0x00482700`.
- Names that fail the CFG gate are marked **NAME UNVERIFIED** (e.g. `0x00510010`, gate FAIL 0.667) and must not be relied on beyond their address.
- A catalog match, a parser test, or a visually similar screenshot alone is **not** proof of retail parity.

## Task 0 — Build the Windows x86 baseline

**Deliverable:** A reproducible CMake build of `open-igi.exe` and a verified DirectDraw 7 window startup under CrossOver.

**Checks:** Build and CTest; confirm file/PE headers identify x86; record executable hash, invocation, CrossOver bottle/version, stdout/stderr, and application log. Confirm the captured screenshot is from the game window. This is only the build baseline, not a playable scene.

## Task 1 — Render Level 1 models at their exact positions

**Deliverable:** The first in-game 3-D demo. Execute the verified QVM/task/object-creation path for Level 1 and render the actual model instances at their retail positions, orientations, and rotations with their resolved original materials/textures.

**Checks:** REA evidence for the QVM/task/native call path and model/render data; independent boundary checks for selected natives; tests against Level 1 scripts and model assets; logs for every instantiated object (task/native identity, model/resource ID, source transform, resulting world transform, material/texture resolution); CrossOver run and game-window screenshot. Do not hard-code scene coordinates or substitute a metadata-only window for rendered geometry. Do **not** implement the `XTRV`/`DNER` stride from the unverified document claim (see `NATIVES_CALL_INFO.md` §17).

**Exit gate:** The demo visibly contains Level 1 3-D models placed from the original script path. Submit the screenshot, logs, test/build results, and known gaps for manual review.

## Task 2 — Load and render Level 1 terrain with textures

**Deliverable:** Actual Level 1 terrain rendered with original texture/material bindings and verified lighting inputs — not a placeholder plane.

**Checks:** Derive layouts and transforms from retail native behavior (the `.CTR` layout and the quadtree-renderer name are **unverified** — decode from the binary consumer, do not implement the documented guess); test the Level 1 terrain/texture corpus; log source files, dimensions, tile/material identifiers, and texture resolution; run under CrossOver; capture the actual game window.

**Exit gate:** Terrain and its textures are visible with the Task 1 models still placed correctly. Submit this build and screenshot for manual review.

## Task 3 — Screenshot each building / version and hand it to a human

**Deliverable:** A review packet: for each building and each asset version, a port capture (and, where possible, a matched retail reference), each with a short comment describing what it is, where it comes from, and what to look for.

**Checks:** Captures saved under `screenshots/level1/<building>/` with the run/build identity; a `screenshots/level1/REVIEW.md` that links every capture to its build and test record. A capture that was not actually taken is not listed.

**Exit gate:** A human walks the packet and signs off before any gameplay-system work begins. This is the human review/test gate requested for this stage.

## Task 4 — Verify every object is at its exact position and orientation

**Deliverable:** A reconciliation report for every object emitted by the Level 1 object/task path — model identity, position, orientation, rotation, material.

**Checks:** Compare the port's structured log and runtime state against retail `igi.exe` / `igi-probe` observations and source script execution. Check missing, duplicate, or unresolved objects. Every object either matches within a stated tolerance or is listed as a known difference with the affected evidence. Record unknowns rather than inferring a pass. Positions come from script execution, not handwritten coordinate lists.

**Exit gate:** Every expected renderable object is accounted for and visual differences are listed.

## Task 5 — Verify every texture is loaded correctly

**Deliverable:** A texture reconciliation report for every material in the scene — source archive/`.TEX`, dimensions, format/palette, and whether it bound and displayed.

**Checks:** Corpus test asserting resolved texture names and counts; logs of bind calls and formats; visual check against the Task 3 packet. No missing or misbound texture; every fallback/placeholder is explicitly listed as a difference.

**Exit gate:** Texture report reconciles; differences listed.

## Checkpoint A — Level 1 visual scene signed off

- [ ] Level 1 QVM/task instances are loaded (Task 1).
- [ ] Terrain and models are visible with original textures (Task 2).
- [ ] Screenshot packet for every building/version reviewed by a human (Task 3).
- [ ] Object position/orientation report reconciles (Task 4).
- [ ] Texture report reconciles (Task 5).
- [ ] Focused tests and the Windows x86 build pass.

**Gameplay-system work (Tasks 6+) does not start until this checkpoint is signed off.**

## Task 6 — QVM/task runtime and simulation tick

**Deliverable:** The verified Level 1 script/task execution closure and retail tick order needed to drive the mission.

**Checks:** Opcode/task unit tests, Level 1 script corpus run, task-count/state logs, native call-flow evidence, matched retail runtime observations. Unknown opcodes or unresolved native calls must fail visibly rather than being skipped.

## Task 7 — Player creation, input, camera, and movement

**Deliverable:** The retail player instance and Level 1 input/camera/movement state path.

**Checks:** Verify native boundaries and configuration-driven controls; deterministic input/tick tests; per-tick player position, velocity, stance, and camera logs; matched movement/jump scenarios in CrossOver. Keep animation state synchronized with the proven movement state.

## Task 8 — Physics, collision, and world interactions

**Deliverable:** Grounding, slopes, edges, walls, buildings, stairs/ladders, doors, and the interactive world objects used by Level 1.

**Checks:** Retail-derived collision query tests and matched runtime traces for walking, jumping, landing, entering a building, stairs/ladders, and door interaction. Report position/state deltas and health effects. No generic FPS constants or guessed collision geometry. Doors must use the authored parameters (Level 1 doors are **sliding**, `Max angle = 0`) and the retail task state machine, not hardcoded motion.

## Task 9 — AI and Level 1 graph navigation

**Deliverable:** Level 1 guard task creation, graph selection by original graph ID, path following, perception, and combat-state transitions.

**Checks:** Graph node/link integrity and AI state tests; source/runtime position reconciliation; CrossOver scenarios for patrol, detection, attack, and target loss. Verify guards move on graph paths rather than sliding or orbiting, and their start positions match Level 1 source/runtime values.

## Task 10 — Health, damage, and death

**Deliverable:** Retail health initialization and player/enemy damage, hit regions, fall damage, difficulty, and death-state handling.

**Checks:** Boundary and fatal/non-fatal tests; before/after health and state logs; verify player-only debug effects do not modify enemy state; compare corresponding retail observations.

## Task 11 — Weapon selection, firing, and reloading

**Deliverable:** Level 1 weapon switching, aiming, fire cadence, spread/recoil, hit/projectile resolution, ammunition, reload, and damage dispatch.

**Checks:** Deterministic input/shot tests and CrossOver shot sequences. Compare tick, ammo, direction/spread, recoil, target hit, damage, and death transition with retail traces. Capture the visible weapon/target result.

## Task 12 — Gameplay animations and magic-object effects

**Deliverable:** Player locomotion, enemy attack/hit/death, weapon equip/fire/reload, and the gameplay-critical magic-object effects the Level 1 script drives.

**Checks:** Verify animation state IDs, transition conditions, completion behavior, and object attachment/update routines against retail. Run repeated movement/combat sequences and capture representative frames. Main-menu and cutscene animation are excluded.

## Task 13 — Level 1 sound engine

**Deliverable:** Retail-derived Level 1 sound resources, sound definitions, event triggers, voice mixing, and audible Windows playback integrated with gameplay.

**Checks:** REA evidence for the retail loader, definition, event, and mixer paths; parser/decoder/mixer tests against Level 1 resources; timestamped logs for loaded sounds, trigger source, voice allocation, and playback failures; CrossOver scenarios for representative footsteps, weapon, impact, and AI sounds; compare trigger timing and decoded output with retail observations. File parsing and decoder tests alone do not establish audible runtime parity.

## Task 14 — Integrated playable Level 1 run

**Deliverable:** A repeatable run from executable launch into the Level 1 mission exercising the integrated world, player, physics, AI/navigation, health/damage, weapons, interactions, animations/effects, and scripts.

**Checks:** Clean Windows x86 rebuild, full automated test suite, CrossOver end-to-end run, retained logs and screenshots, and a subsystem-by-subsystem retail parity report. Owner manually reviews the result. Any unobserved branch or unresolved difference remains open.

## Demo native scope

| Step | Native areas | Unique natives | Name-verified |
|---|---|---|---|
| 1 Models + placement | `dynobj`, `resource`, `magicobj`, `rigid`, `physicsobj`, `origo`, `vec3`, `mat33` | 187 | 187 |
| 2 Terrain + textures | `terrain`, `physicsterrain`, `discardterrain`, `terradrawtask`, `heightmap`, `create` | 187 | 187 |
| 4 Placement verify | `origo`, `vec3`, `mat33`, `splineobj`, `worldobject` | 33 | 33 |
| 5 Texture verify | `dynobj`, `mesh`, `material`, `mipmapcontrol`, `render`, `display` | 514 | 514 |
| 6 Playable script core | `script`, `qsc`, `qvm`, `qtask`, `qobj`, `symbol` | 568 | 568 |
| 7 Player/movement | `humanplayer`, `locomotion`, `body`, `bone`, `humancamera`, `ladder`, `collision` | 221 | 221 |
| 8 Physics | `physics*`, `rigid`, `forceaccumulator` | 190 | 190 |
| 9 AI navigation | `aigraph`, `graph*`, `patrol*`, `splinepath*`, `ai`, `humanai`, `combatstate` | 637 | 637 |
| 10 Health | `hitzone`, `medipack`, `human`, `soldier` | 7 | 7 |
| 11 Weapons | `weapon*`, `gun*`, `ammo*`, `m203`, `grenade`, `mine` | 248 | 248 |
| 12 Animation | `anim*`, `frames`, `*dyncubeobj` | 71 | 71 |
| 12 Interaction | `door`, `doorcabinet`, `switch`, `terminal`, `elevator`, `glass` | 192 | 192 |

Per-native addresses, gate status, BN verification result and CLEAN path: `assets/BinaryNinja/natives/CLEAN/VERIFICATION.md`. Inventory by subsystem: `NATIVES_CALL_INFO.md` §23; build-order mapping: §24.

*Note: 15 catalog names conflict with the binary (see the rule above); they are listed in the manifest and must be used by address.*

## Deferred from this demo

Main menus, movies and cutscenes, subtitles, vehicles, and non-gameplay smoke/particle effects. Level 1 sound, including gameplay event sounds and its mixer/playback path, remains in scope. Music, sirens, and radio are included only where Level 1 execution reaches them.

## Current verified limits

The current Windows bootstrap is a DirectDraw 7 diagnostic window that loads an explicitly supplied RES archive and reports resource/MEF metadata. The source has RES, MTP, MEF-container, and LOOP texture parsing tests. It does not yet execute the Level 1 object QVM, render MEF geometry or terrain, instantiate script objects, or provide a playable game loop. A successful parser test or window startup is not proof of a loaded 3-D scene or retail parity.
