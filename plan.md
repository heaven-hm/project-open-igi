# Level 1 Playable Demo Plan

This plan implements the scope in [MVP.md](MVP.md) as sequential, reviewable slices. Each task stays small enough to build and verify before the next begins. The first deliverable is a scene visibly populated from retail `OBJECTS.QVM`; later slices make that scene playable.

## Working loop for every task

1. Inspect the current C++ implementation and the exact Level 1 asset/script path.
2. Use REA on the retail `igi.exe` to establish native boundaries, call flow, fields, and behavior. Use Binary Ninja/Radare2 as independent static checks and RE-Agent for bounded reconstruction/parity where useful. Use CrossOver plus `igi-probe`/Frida for runtime observations when the scenario can be reproduced.
3. Consult C# `open-igi` only for candidate logic. Keep the candidate identified and record what retail evidence confirms or contradicts it.
4. Implement one narrow vertical slice in C++20. Add focused unit/integration tests, failure handling, and timestamped subsystem logs.
5. Build the Windows x86 target and run its focused tests. Run the matching retail and port scenario through CrossOver.
6. For visible changes, capture a screenshot of the port and the matching retail view, record the scenario/build identifiers, and save the captures under `screenshots/level1/`.
7. Review the diff and make one small local commit only after its required checks pass. Never claim a runtime check or screenshot that was not actually captured.

## Ordered tasks

### Task 0 — Establish the reproducible Level 1 test case

**Deliverable:** A repeatable way to launch retail and port Level 1 with the same assets and record build identity, logs, and screenshots.

**Acceptance:** The documented CrossOver invocation launches the retail game and the current Windows x86 prototype; the binary hashes, arguments, bottle, output logs, and screenshot paths are recorded. This task establishes a baseline; it does not claim the current diagnostic window is a playable game.

**Verification:** Confirm each process/window and its logs. Save one retail reference capture and one prototype baseline capture. If CrossOver or screen capture is unavailable, record the exact failure and leave the runtime gate open.

**Dependencies:** None.

### Task 1 — Verify the retail object-script startup path

**Deliverable:** A retail-evidence map from loading `OBJECTS.QVM` through its QVM/task/native registration path to renderable object creation.

**Acceptance:** Every native selected for the path has a real retail entry/body, its callers/callees and relevant data fields are recorded, and provisional catalog labels are marked as such. The script's object count and task types can be compared with retail runtime observations.

**Verification:** REA/Ghidra evidence plus independent Radare2/Binary Ninja boundary checks; runtime trace of the original under CrossOver/`igi-probe` where observable. Do not use handwritten coordinate lists as a substitute for script execution.

**Dependencies:** Task 0.

### Task 2 — Execute the Level 1 object-creation closure

**Deliverable:** The minimum verified QVM/task/native runtime needed to instantiate the renderable objects in `OBJECTS.QVM`.

**Acceptance:** The C++ runtime loads the original script path and creates the same renderable task set as retail. Structured logs identify each task/native and report model ID, raw position/orientation inputs, and transformed world values. Unknown opcodes or natives fail with explicit diagnostics instead of being silently skipped.

**Verification:** Unit tests for the verified opcode/task cases, a corpus run over Level 1 `OBJECTS.QVM`, and runtime count/field comparison with retail. Commit this slice separately.

**Dependencies:** Task 1.

### Task 3 — Resolve model packages and render textured Level 1 objects

**Deliverable:** Original RES/MTP/MEF/TEX resources resolved for the instantiated Level 1 object tasks and drawn in the Direct3D 7 window.

**Acceptance:** The model/material identifiers resolve through the retail resource path; mesh and texture decoding follows layouts established from the retail binary. Objects appear at the positions and orientations produced by Task 2. Transparent materials retain their observed blend/depth behavior.

**Verification:** Focused parser and asset-corpus tests; assert resolved asset names and counts; compare native field/stream observations; run under CrossOver and capture a scene screenshot against the retail reference. Do not implement an XTRV/DNER stride or topology from an unverified document claim.

**Dependencies:** Tasks 0–2.

### Task 4 — Load and draw Level 1 terrain with original textures

**Deliverable:** Level 1 terrain, its texture/lightmap inputs, and the verified camera-visible terrain region rendered through Direct3D 7.

**Acceptance:** Height, tile/region boundaries, texture lookup, lighting inputs, and world scale come from confirmed file/native behavior. The terrain is not a placeholder plane.

**Verification:** Tests over the actual Level 1 terrain corpus; logs include source file, dimensions/counts, and loaded texture references; CrossOver screenshot comparison from a matched retail camera view.

**Dependencies:** Task 3.

### Checkpoint A — Visible Level 1 scene

- [ ] Level 1 QVM/task instances are loaded.
- [ ] Textured 3D models and terrain are visible in the game window.
- [ ] Position/orientation/model/texture logs reconcile with retail observations.
- [ ] Focused tests and Windows x86 build pass.
- [ ] Retail and port screenshots are saved with provenance.

### Task 5 — Add the Level 1 player and input path

**Deliverable:** Retail player creation, input dispatch, camera state, and movement state updates.

**Acceptance:** The player can move, turn, crouch, and jump using the original Level 1 controls and state transitions. Control mappings and fixed-tick behavior come from the retail config/script and binary.

**Verification:** Native boundary/call-flow evidence, input replay tests, per-tick position/velocity/state logs, and matched CrossOver movement traces/screenshots. Test a short jump and ordinary movement before broad scenarios.

**Dependencies:** Checkpoint A.

### Task 6 — Match player physics and world collision

**Deliverable:** Ground contact, slopes, edges/building entry, ladders where used in Level 1, jump arcs, landing, and damage-producing falls.

**Acceptance:** Collision queries use the actual Level 1 world geometry. Player motion, landing transitions, and health effects are driven by the verified retail path; no generic FPS constants are introduced.

**Verification:** Focused deterministic cases plus matched retail/port input traces for movement, jump, slope, doorway, and fall scenarios. Report per-tick state and position deltas.

**Dependencies:** Task 5 and Checkpoint A.

### Task 7 — Load Level 1 navigation and bring up enemy AI

**Deliverable:** Level 1 graph data and the verified AI/task chain required for guards to patrol, detect, move, and attack.

**Acceptance:** Enemy start positions and graph/task identities match Level 1 source/runtime values. Guards follow connected paths, respond to the player as retail does, and do not slide, panic-loop, or remain inert without a corresponding retail state.

**Verification:** Graph-link integrity tests, AI state transition tests, and matched runtime observations for patrol, detection, attack, and loss of target. Capture visible enemy positions at the same scenario ticks.

**Dependencies:** Tasks 1, 2, and 6.

### Task 8 — Add health and damage for player and enemies

**Deliverable:** Retail health initialization, hit/fall damage, death transitions, and difficulty-dependent behavior used by Level 1.

**Acceptance:** Health changes are limited to the intended entity; fatal state remains fatal; body-region and fall behavior follow verified retail branches.

**Verification:** Boundary tests from retail-derived inputs, tests for normal and fatal damage, and runtime comparisons of before/after health and entity state. Verify that player-only debug/cheat behavior cannot affect enemy health.

**Dependencies:** Tasks 6–7.

### Task 9 — Add weapon selection, firing, reloading, and hit resolution

**Deliverable:** Level 1 player weapon state, switching, ammo, fire cadence, spread/recoil, projectile or hit resolution, and damage dispatch.

**Acceptance:** The player can select available Level 1 weapons, fire at enemies, reload, and see ammo/damage/death state update. Any visible weapon animation uses original animation/state data where required for the playable path.

**Verification:** Focused weapon tests plus matched shot sequences under CrossOver; compare tick of shot, ammo, direction/spread, recoil, target hit, damage, and death transition. Capture an in-game screenshot of the weapon and target result.

**Dependencies:** Tasks 5, 7, and 8.

### Task 10 — Integrate the Level 1 mission script and playable loop

**Deliverable:** The required Level 1 QVM/task flow connects world loading, player, AI, weapons, health, and mission progress into one continuous run.

**Acceptance:** Launch directly into the playable mission path, interact with required world objects, engage guards, and complete the Level 1 demo scenario without menus/cutscenes/vehicles.

**Verification:** End-to-end CrossOver run from process launch to the chosen Level 1 endpoint; retain logs, test output, binary/build identity, and before/after screenshots. Repeat after a clean build.

**Dependencies:** Tasks 2–9.

### Checkpoint B — Playable Level 1 demo

- [ ] Object placement, terrain, player, physics, AI/navigation, health/damage, weapon/shooting, and required QVM flow work together.
- [ ] Every visible implementation increment has a screenshot comparison.
- [ ] Unit, integration, and end-to-end tests pass on the Windows x86 build.
- [ ] Remaining differences and unverified branches are listed explicitly.

## Current evidence and known limits

- The `game-engine-impl` target currently has a DirectDraw 7 diagnostic window and parsers for RES, MTP, MEF container metadata, and TEX headers/data. It is not yet a playable Level 1 renderer or game loop.
- The current MEF code exposes D3DR/DNER/VRTX chunks but does not decode their verified vertex/draw layouts or render them. The Level 1 object QVM/task execution path is not connected.
- Some native catalog labels are not confirmed by REA. A catalog match, parser test, or visually similar result alone is not proof of retail parity.
- Current screenshot/runtime capture limitations must be reported against the affected task; do not substitute a terminal screenshot for a game-window capture.
