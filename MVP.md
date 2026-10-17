# Level 1 Playable Demo MVP

## Goal

Build a Windows x86 open-igi.exe that loads the original Level 1 assets and becomes a playable mission, implementing retail IGI behavior from evidence in igi.exe. The retail binary is the behavior authority. C# open-igi may identify candidate implementations, but each behavior used in this C++ engine must be checked against the retail executable with REA and runtime evidence where observable.

## Target

- Windows x86, C++20, Win32, DirectDraw 7 / Direct3D 7, and original game assets.
- CrossOver on this Mac is the local test environment for the Windows executable, not a macOS product target.
- Preserve original native names and addresses in source annotations when supported by evidence. Record tool, evidence ID, confidence, and whether runtime parity is established.
- Do not guess file layouts, coordinates, physics constants, or behavior. Mark unresolved facts as unknown until evidence resolves them.
- Keep each implementation slice small and independently buildable, tested, logged, and reviewable.

## Required implementation order

1. **Build baseline.** Produce the Windows x86 executable, verify its PE32 architecture and DirectDraw 7 startup, and establish reproducible test/log capture.
2. **Level 1 3D scene.** Execute the retail OBJECTS.QVM task/object path, resolve the original model and texture resources, and render the Level 1 3D models at their script/runtime positions, orientations, and rotations.
3. **Level 1 terrain.** Load and render the actual terrain with its original textures and required lighting inputs.
4. **Asset placement and appearance verification.** Compare every instantiated Level 1 object transform and asset binding with retail observations. Verify that required model and terrain textures resolve and display correctly.
5. **Player and simulation foundation.** Add the retail game tick/task flow, player spawn, input, camera, and movement state.
6. **Physics and world interaction.** Add verified collision, ground contact, jumping, falling, building entry, stairs/ladders, doors, and relevant object interaction.
7. **AI and graph navigation.** Load Level 1 AI tasks and their graph IDs; verify patrol, perception, movement, and combat behavior.
8. **Health and damage.** Implement player/enemy health initialization, damage, hit regions, death transitions, and difficulty-dependent behavior.
9. **Weapons and combat.** Implement weapon selection, firing, hit resolution, ammunition, reloading, and the required weapon state transitions.
10. **Gameplay animation and effects.** Add the player, enemy, and weapon animations and gameplay-critical magic-object effects needed by Level 1.
11. **Level scripting and mission flow.** Connect the required QVM execution and task transitions so these systems operate together in the Level 1 mission.
12. **Sound engine.** Implement Level 1 sound loading, definitions, event triggers, mixing, and the Windows audio boundary, then verify audible gameplay events against retail.
13. **Playable Level 1 acceptance.** Run the integrated mission scenario and record measured parity and unresolved differences across every included subsystem.

Main menu presentation, movies/cutscenes, vehicles, and non-gameplay smoke effects are outside this playable-demo scope unless a Level 1 dependency is proven.

## Build, test, screenshot, and review gate

For every numbered implementation slice:

1. Build the Windows x86 target and run its focused automated tests.
2. Run the relevant retail and port scenarios under CrossOver; preserve logs, binary identity, arguments, and environment details.
3. Capture a screenshot of each visible build milestone from the actual game window. Keep screenshots with their run/build records; a terminal capture does not count.
4. Compare retail and port object/model/texture data where relevant. Record exact matches, measured deltas, and unknowns.
5. Make one small local commit only after its stated gates pass. Do not push unless explicitly requested.
6. Present the build, test result, screenshot, and remaining gaps for the owner's manual review before moving past a visual checkpoint.

## Acceptance

The MVP is complete only when Level 1 loads the original world and all included systems operate together: object placement, models and textures, terrain, player movement, physics and interaction, AI and navigation, health and damage, weapons and combat, gameplay animations/effects, mission scripting, and sound. Focused tests and compilation alone are insufficient. Runtime evidence and the owner's manual review are required; no subsystem is marked verified without evidence appropriate to its claim.
