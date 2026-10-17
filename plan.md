# Level 1 Playable Demo Plan

This is the execution sequence for MVP.md. Keep the order: establish the build, show the 3D models at retail object transforms, add textured terrain, verify the complete visual scene, and only then build the playable systems. Sound is included in the full Level 1 MVP and comes after the gameplay path is integrated.

## Repeatable work loop

Each task is one small, reviewable increment:

1. Inspect the current source, assets, and dirty Git state. Do not overwrite unrelated user changes.
2. Establish native behavior from retail igi.exe with REA/Ghidra. Cross-check boundaries or control flow with Binary Ninja/Radare2 as needed. Use RE-Agent for bounded reconstruction/parity work.
3. Use C# open-igi only to find candidate logic. Validate candidates against the retail binary before using them.
4. Implement one narrow C++20 slice with readable retail-native names/addresses, structured timestamped logs, and explicit errors for unsupported or unresolved cases.
5. Add focused tests for that slice, build the Windows x86 executable, and run the relevant scenario under CrossOver.
6. For each visible milestone, capture the actual game window and retain its run/build identity and logs.
7. Make a small local commit after verification. Do not push unless explicitly asked.
8. At each visual checkpoint, provide the screenshot and test record for owner manual review before advancing.

## Task 0 — Build the Windows x86 baseline

**Deliverable:** A reproducible CMake build of open-igi.exe and a verified DirectDraw 7 window startup under CrossOver.

**Checks:** Build and CTest; confirm file/PE headers identify x86; record executable hash, invocation, CrossOver bottle/version, stdout/stderr, and application log. Confirm the captured screenshot is from the game window. This is only the build baseline, not a playable scene.

## Task 1 — Render Level 1 models from OBJECTS.QVM

**Deliverable:** The first in-game 3D demo. Execute the verified QVM/task/object-creation path for Level 1 and render the actual model instances at their retail positions, orientations, and rotations with their resolved original materials/textures.

**Checks:** REA evidence for the QVM/task/native call path and model/render data; independent boundary checks for selected natives; tests against Level 1 scripts and model assets; logs for every instantiated object (task/native identity, model/resource ID, source transform, resulting world transform, material/texture resolution); CrossOver run and game-window screenshot. Do not hard-code scene coordinates or substitute a metadata-only window for rendered geometry.

**Exit gate:** The demo visibly contains Level 1 3D models placed from the original script path. Submit the screenshot, logs, test/build results, and known gaps for manual review.

## Task 2 — Load and render Level 1 terrain with textures

**Deliverable:** Actual Level 1 terrain rendered with original texture/material bindings and verified lighting inputs.

**Checks:** Derive layouts and transforms from retail native behavior; test the Level 1 terrain/texture corpus; log source files, dimensions, tile/material identifiers, and texture resolution; run under CrossOver; capture the actual game window.

**Exit gate:** Terrain and its textures are visible with Task 1 models still placed correctly. Submit this build and screenshot for manual review.

## Task 3 — Verify all Level 1 visual assets and transforms

**Deliverable:** A reconciliation report for every object emitted by the Level 1 object/task path, including model identity, position, orientation, rotation, material, and texture.

**Checks:** Compare the port's structured log and runtime state against retail igi.exe/igi-probe observations and source script execution. Check missing, duplicate, or unresolved objects and texture/material bindings. Capture representative scene views and retain the complete per-object log. Record unknowns rather than inferring a pass.

**Exit gate:** Every expected renderable object is accounted for and visual differences are listed. Owner reviews the build before gameplay-system work begins.

## Task 4 — QVM/task runtime and simulation tick

**Deliverable:** The verified Level 1 script/task execution closure and retail tick order needed to drive the mission.

**Checks:** Opcode/task unit tests, Level 1 script corpus run, task-count/state logs, native call-flow evidence, and matched retail runtime observations. Unknown opcodes or unresolved native calls must fail visibly rather than being skipped.

## Task 5 — Player creation, input, camera, and movement

**Deliverable:** The retail player instance and Level 1 input/camera/movement state path.

**Checks:** Verify native boundaries and configuration-driven controls; deterministic input/tick tests; per-tick player position, velocity, stance, and camera logs; matched movement/jump scenarios in CrossOver. Keep animation state synchronized with the proven movement state.

## Task 6 — Physics, collision, and world interactions

**Deliverable:** Grounding, slopes, edges, walls, buildings, stairs/ladders, doors, and relevant interactive world objects used by Level 1.

**Checks:** Retail-derived collision query tests and matched runtime traces for walking, jumping, landing, entering a building, stairs/ladders, and door interaction. Report position/state deltas and health effects. No generic FPS constants or guessed collision geometry.

## Task 7 — AI and Level 1 graph navigation

**Deliverable:** Level 1 guard task creation, graph selection by original graph ID, path following, perception, and combat-state transitions.

**Checks:** Graph node/link integrity and AI state tests; source/runtime position reconciliation; CrossOver scenarios for patrol, detection, attack, and target loss. Verify guards move on graph paths rather than sliding or orbiting.

## Task 8 — Health, damage, and death

**Deliverable:** Retail health initialization and player/enemy damage, hit regions, fall damage, difficulty, and death-state handling.

**Checks:** Boundary and fatal/nonfatal tests; before/after health and state logs; verify player-only debug effects do not modify enemy state; compare corresponding retail observations.

## Task 9 — Weapon selection, firing, and reloading

**Deliverable:** Level 1 weapon switching, aiming, fire cadence, spread/recoil, hit/projectile resolution, ammunition, reload, and damage dispatch.

**Checks:** Deterministic input/shot tests and CrossOver shot sequences. Compare tick, ammo, direction/spread, recoil, target hit, damage, and death transition with retail traces. Capture the visible weapon/target result.

## Task 10 — Gameplay animations and magic-object effects

**Deliverable:** Player locomotion, enemy attack/hit/death, weapon equip/fire/reload, and gameplay-critical magic-object effects.

**Checks:** Verify animation state IDs, transition conditions, completion behavior, and object attachment/update routines against retail. Run repeated movement/combat sequences and capture representative frames. Main-menu and cutscene animation are excluded.

## Task 11 — Sound engine

**Deliverable:** Level 1 audio resource loading, sound definitions, event dispatch, voices/mixing, attenuation, and the original Windows audio API boundary.

**Checks:** Unit tests for decoded formats and mixer/event behavior; runtime event traces for footsteps, weapon fire/reload, impacts, alarms, and AI cues; compare retail timing/parameters and audible output under CrossOver. Sound is required for the full MVP, but it is not part of the initial graphics demo.

## Task 12 — Integrated playable Level 1 run

**Deliverable:** A repeatable run from executable launch into the Level 1 mission exercising the integrated world, player, physics, AI/navigation, health/damage, weapons, interactions, animations/effects, scripts, and sound.

**Checks:** Clean Windows x86 rebuild, full automated test suite, CrossOver end-to-end run, retained logs and screenshots, and a subsystem-by-subsystem retail parity report. Owner manually reviews the result. Any unobserved branch or unresolved difference remains open.

## Current verified limits

The current Windows bootstrap is a DirectDraw 7 diagnostic window that loads an explicitly supplied RES archive and reports resource/MEF metadata. The source has RES, MTP, MEF-container, and LOOP texture parsing tests. It does not yet execute the Level 1 object QVM, render MEF geometry or terrain, instantiate script objects, or provide a playable game loop. A successful parser test or window startup is not proof of a loaded 3D scene or retail parity.
