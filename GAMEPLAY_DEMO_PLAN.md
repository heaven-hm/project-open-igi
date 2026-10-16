# First playable gameplay demo

## Goal

Build a small native macOS Level 1 gameplay slice around the original game
data. The game core must remain OS-independent. The MVP is shipped on macOS;
Windows and Linux support are planned after this playable slice and will require
separate platform backends and verification. Keep OS APIs out of gameplay code.
The first demo is a playtest build, not the finished `igi.app`. Its scope is
player movement and collision, a usable weapon, and one enemy using the
retail mission's navigation/behavior data. The DirectX 7 era image and update
behavior are the reference. Do not add post-processing or redesign the look.

Main menu, sound, finished animation blending, and packaging are outside this
first demo. Animation state must still follow locomotion state where needed for
movement, but polish and sound are deferred.

## Evidence and implementation order

1. **Application loop:** build the macOS `igi.app` shell with `main.cpp`, a
   C++ SDL2 window/event loop, and a portable fixed-step update clock. Keep the
   game loop independent of SDL2. Feed physical
   input events into the portable input state and consume transitions per tick.
   Verify the bundle, tick scheduling, and input edges before adding gameplay
   systems. The current shell has no level rendering or playable state.
2. **Retail capture:** launch the exact `igi.exe` in the existing CrossOver
   bottle with `window level1`. Capture a fixed input sequence, rendered frames,
   process/module identity, and available player/weapon/AI state. Keep the
   binary digest with every capture. Static-only evidence cannot stand in for
   runtime behavior.
3. **Movement slice:** use REA to map the active player update, input dispatch,
   movement integration, ground/ladder/jump transitions, collision response,
   and animation root-motion inputs. Implement only values and branch order
   supported by binary or runtime evidence. Keep unknown retail tuning values
   explicit until captured.
4. **Weapon slice:** map the active fire/reload functions and weapon QVM/config
   fields; capture shot timing, ammo changes, recoil, projectile/hit behavior,
   and state transitions. Port those observations and replay the same sequence.
5. **AI slice:** map the mission graph, task/sensor/order handlers, patrol,
   detection, combat, movement, and damage responses. Use Level 1 source data;
   compare enemy transitions and coordinates on a fixed timeline.
6. **Native render harness:** render the original geometry/textures with the
   game's resolution, camera, texture, depth, blending, lighting, and frame
   order. First capture the legacy output under CrossOver; then map observed
   state to the macOS MVP backend. Keep rendering rules in the OS-independent
   core and graphics APIs in platform backends. Windows and Linux backends come
   after the MVP. No backend is accepted on API similarity alone.
7. **Parity gate:** feed identical tick-stamped inputs to the original and the
   prototype. Compare player transforms/velocities/state, weapon state and
   events, AI state/positions, and frame captures. Report exact matches,
   measured numeric tolerances, unexplained differences, and uncovered paths.

## Commit slices

Keep each commit reviewable and local after its checks pass. Do not push unless
the owner explicitly requests it:

- converter-backed retail asset scan and provenance;
- movement/collision behavior with its REA evidence;
- weapon behavior with its REA evidence;
- Level 1 AI behavior with its REA evidence;
- fixed-function renderer mapping and comparison captures;
- playable macOS demo and app packaging;

- later Windows platform backend and build verification;
- later Linux platform backend and build verification.

Commit messages remain short and German as requested. Do not call the game
"100% verified" until every claimed subsystem has runtime comparison coverage;
unobserved behavior stays marked unknown.
