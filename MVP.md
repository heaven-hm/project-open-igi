# Level 1 Playable Demo MVP

## Goal

Build a Windows x86 `open-igi.exe` that loads and presents the original Level 1 world, then lets the player complete a playable mission slice. The first visible milestone is the real Level 1 3D scene: terrain, textured models, and object positions and orientations loaded from the retail `OBJECTS.QVM` path.

This is an implementation of IGI's game engine, not a new game with similar-looking behavior. The retail `igi.exe` is the authority. C# `open-igi` is a source of candidate behavior and implementation details; each candidate used in C++ must be checked against retail static evidence and runtime observations.

## Target and implementation rules

- Target: Windows x86, C++20, DirectDraw 7 / Direct3D 7, and the original game's assets.
- CrossOver on this Mac is a test environment for the Windows executables. It is not a macOS game target.
- Preserve retail native names and addresses in code comments. Record the analysis tool, evidence references, confidence, and whether runtime behavior has been verified. Treat guessed or catalog-only names as provisional.
- Do not substitute guessed formats, coordinates, physics values, or behavior. If retail evidence is incomplete, record the unknown and investigate it before claiming parity.
- Keep each implementation slice focused. Add focused tests and logs, build the Windows x86 executable, run the relevant Level 1 path under CrossOver, capture a screenshot for visible changes, then make a small local commit. Do not push unless explicitly requested.

## Included in the playable demo

1. **Level 1 object execution and placement** — load the retail object script through the required QVM/task path; instantiate renderable objects with their actual model IDs, positions, orientations, and rotations.
2. **World assets and rendering** — load Level 1 RES/MTP/MEF/TEX assets and terrain, bind original textures, and draw them through the Direct3D 7 path.
3. **Player** — Level 1 player creation, input, movement, camera, jump, ground contact, and collision behavior.
4. **Physics and health** — retail-backed movement and collision, fall/damage handling, health, and damage to the player and enemies.
5. **AI and graph navigation** — spawn Level 1 enemies, load their graph data, move and navigate, perceive the player, and engage using the mission's behavior.
6. **Weapons and combat** — select and switch available weapons, fire, reload, update ammunition, trace hits, and apply retail damage.
7. **Level scripting** — execute the Level 1 QVM/task behavior required for object creation and the playable mission flow.

## Deferred from this demo

Main menus, movies and cutscenes, subtitles, vehicles, and smoke/particle effects are outside the first playable demo. Sound implementation is paused until the playable visual/gameplay milestone is accepted; it remains part of the broader Level 1 parity goal.

## Acceptance gates

The demo is accepted only when all of the following are evidenced:

- The Windows x86 executable opens the Level 1 scene using the original game assets.
- Retail renderable objects are instantiated from `OBJECTS.QVM`; logs report the native task/object identity and the loaded model, position, orientation, and rotation. The values are compared with the retail script/runtime path, not manually entered scene coordinates.
- Terrain, models, and textures are visible in the game window. A screenshot is captured after each visible implementation slice and stored with its build/test record.
- Player movement, physics, AI navigation and combat, health/damage, weapon switching and shooting, and required QVM behavior work together in the Level 1 end-to-end run.
- Focused tests, the Windows x86 build, and the corresponding CrossOver runtime scenario pass for each committed slice. Retail-versus-port comparisons list exact matches, measured differences, and unresolved behavior; unknowns are never marked verified.

This MVP does not claim completion of full Level 1 parity until each included subsystem has evidence at the required static and runtime gates.
