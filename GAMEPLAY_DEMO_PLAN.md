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

Main menu, sound, movies/cutscenes, subtitles and vehicles are deferred.
Gameplay skeletal animation, weapon handling and original movement timing are
required for Level 1 acceptance. The packaged application remains experimental
until the recorded retail-versus-port gameplay comparisons pass.

Use this project's `RE.md` as the reverse-engineering workflow. Existing native
exports and vanilla C# implementations are the porting sources; confirm their
semantics against retail instructions and runtime observations. The converter
and editor supply the existing C++ format parsers.

## Current Level 1 checkpoints

- The application loop, original asset loading and playable scene are present.
- [Retail RNG](docs/fidelity/retail_random_generator.md): original constructor,
  integer draws, float bits and mutated state compared against original x86 code.
- [Jump motion](docs/fidelity/jump_motion_fields.md): separate airborne/impact
  velocities, previous-animation launch direction, takeoff tick and landing stop.
- [Weapon accuracy](docs/fidelity/weapon_accuracy.md): original script fields,
  projectile count, cone sampling and recoil samples integrated in the runtime.
- [REA/Frida](docs/fidelity/frida_runtime.md): an owned macOS primitive traced;
  this is separate from retail Windows game instrumentation.

Acceptance is incomplete. Next priorities remain integer weapon cadence and
view recoil; retail player-state/animation transitions; building and edge
collision coverage; AI graph/task/combat script integration; and original
rendering/graphics settings. Capture corresponding original and port state for
each scenario. Passing functional tests does not satisfy the full retail gate.

## Evidence and implementation order

1. **Application loop:** build the macOS `igi.app` shell with `main.cpp`, a
   C++ SDL3 window/event loop, and a portable fixed-step update clock. Keep the
   game loop independent of SDL3. Feed physical
   input events into the portable input state and consume transitions per tick.
   Verify the bundle, tick scheduling, and input edges before extending gameplay
   systems. The current prototype has level rendering and playable state;
   fixed-step scheduling still requires aligned original-game capture evidence.
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
