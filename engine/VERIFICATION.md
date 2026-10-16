# Gameplay verification notes

## Current native gameplay MVP

The current application loads original Levels 1 and 2, including recursive attachments and skeletal hand/body rigs. Current results, bug corrections, known limits and the failed retail gate are in [MVP_STATUS.md](MVP_STATUS.md). The active scope and acceptance contract are [MVP_PLAN.md](MVP_PLAN.md) and [FIDELITY_REQUIREMENTS.md](FIDELITY_REQUIREMENTS.md). The sections below are historical component checks; statements that the app is only a shell or lacks world providers describe that earlier revision. They are not current status. No historical startup or synthetic check establishes original-versus-port parity.

## Level 1 object placement and retail Euler matrix

REA/Ghidra opened the retail PE32 x86 executable at SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Evidence `ev_9c8e04022868d6ffb013950ca6abd6d3ff76a927550ef6fbd6ff0bb169f3a6cc`
records the routine at `0x4B38E0` as a 640-byte, one-basic-block function. Its
calls to `0x4B3BE0`, `0x4B3BA0`, and `0x4B3B60` build Z, Y, and X axis rotations
in that order; the routine right-multiplies the running orientation matrix.
Their evidence IDs are `ev_9024950e1c7fe4a1b5525989963ff6e78dc9e49e26cececb00b97e4a32097b92`,
`ev_1132773151d7254656bcd520f86c6b68523893ce0ecc61cfc6a26bb98c735eee`, and
`ev_aba3c45ff6dc8e22559073e6d0513c0f2e436c49d28ad03af38b6ca81675ae40`.
Radare2 6.2.0 independently reports the routine as 640 bytes and one basic
block. This agrees with vanilla OpenIGI's `Matrix3f.FromEngineEulerAngles`
implementation (`Z(gamma) * Y(beta) * X(alpha)`) and row-vector `Transform`.

The C++ `rotate_euler` now uses that row-matrix convention. Level 1 placement
verification independently calculates the expected transform from the closed-
form matrix, then compares it with the positions of emitted mesh vertices. Its
per-object numerical tolerance is `max(0.000001, 2 * float_epsilon * coordinate_scale)`;
the real-scene probe rejects tolerances above `0.001` scene units, non-finite
vertices, duplicate source calls, missing audits, or mismatches. Actors and
doors are checked against the runtime transform state; they are not counted as
emitted static geometry checks.

The focused C++20 Debug build passed for the Level 1 scene probe, and 5/5
focused CTests passed: Level 1 load/move/jump/fire/render, movement/weapon/AI,
collision, renderer depth clipping, and navigation/inventory. Level 1 loaded
413 object records and 28 soldiers. This validates the port's own loaded scene
against the `objects.qvm` values and the retail matrix convention; no paired
retail runtime transform capture or pixel comparison was made, so it does not
prove full placement or graphics parity.

## AI target point used for sight checks

REA/Ghidra evidence `ev_164003cd4b7a63ba28fac9384ce6fe4b714e1001ff3033b44b02ca64a0c2378b` and its instruction listing
`ev_fe8785a0d7d95647db2602a46dc07dd010bd8a230360cf40006e7d6d506f82e4` cover
retail `0x465820`. The function copies the human position and adds one of two
encoded double constants to its Z coordinate. The instructions initialize
`0x40a6147ae147ae14` for standing and replace it with `0x4096666666666666` when
`0x463180` reports crouching. Decoded in the game's 4096-units-per-metre scale,
these are `0.69 m` standing and `0.35 m` crouched.

REA evidence `ev_2d149c40fd645df3e6316075ba5c660cf01755df94dfc35a040a4d28a42775a6`
for `0x489960` confirms the cone test consumes the candidate point after
transforming it to the observer's local axes; `0x4898D0` calls that cone test
before the sight query at `0x489B20` (evidence
`ev_155cf31c35ef30d3fc03eb1f13037c857c48c859ec6f84967085e394522a65b6`). The
current vanilla C# reference uses `0.7 m` and `0.319 m` for the corresponding
raises, which differ from the instruction immediates. The C++ port uses the
retail values for its sight target and keeps the camera eye for shot direction.

The C++ gameplay probe passed synthetic standing and crouched low-cover cases;
the original Level 1 probe also passed with 413 objects and 28 soldiers loaded.
This verifies the C++ target point against REA's static instruction evidence
and those port-side scenarios. It does not verify a retail runtime trace. The
surrounding C++ AI still uses its existing simplified view cone, visibility
range, event timing, hearing, and combat policy; those are not claimed as
retail-equivalent by this change.

## Terrain quality presets and renderer cost

`--graphics ultra` now selects a separate terrain LOD curve. The real Level 2
probe loaded all five settings and verified strictly increasing source terrain
triangle counts: low `41,299`, medium `101,049`, high `351,509`, enhanced
`2,596,305`, and ultra `6,181,579`. The same probe passed patrol movement,
enemy firing, doorway traversal, animation, and rendering checks. The registered
Level 1 `igi_ultra_graphics_preset` CTest also passed; a direct Level 1 run
reported `5,655,922` original terrain triangles and exited cleanly after one
headless frame.

Single-run Level 1 headless measurements from the application's elapsed-time
log were low `0.216 s` at `28,614` terrain triangles, medium `0.319 s` at
`72,934`, high `0.345 s` at `261,144`, enhanced `1.047 s` at `2,087,084`, and
ultra `2.453 s` at `5,655,922`. These one-frame readings are diagnostic only,
not a stable benchmark. They show that the current CPU rasterizer cannot make
the high-density settings playable; adding detail maps or lighting before an
accelerated renderer would worsen that bottleneck. Ultra remains an explicit
opt-in quality setting and is not evidence of enhanced-renderer parity.

## Texture mip chains and filtering

REA/Ghidra evidence `ev_4a1cc36fa9597db8351b58d3d5317f9ac0e6d46c064f923aa848635799051503`
for retail function `0x51ECB0` shows the `MipMapControl` setting registering
`GR_MIPMAP_DISABLE`, `GR_MIPMAP_NEAREST`, and `GR_MIPMAP_NEAREST_DITHER`, then
initializing the setting to value `2`. This establishes the option names and
initialized default in the shipped binary; it does not establish the full
driver sampling algorithm or prove the runtime value cannot be changed.

The C++ asset loader now preserves authored mip levels from TEX v11 images and
builds a generated chain for base textures that lack one. The software renderer
derives texture-coordinate gradients from its perspective-correct interpolation
and selects mip levels. `--texture-filter nearest` selects authored levels only;
`--texture-filter trilinear` also uses generated levels and blends between
adjacent levels. Generated mip colors are alpha-premultiplied to reduce dark
fringes around transparent texels. These are sampling modes, not claims of a
complete Vanilla or Enhanced preset implementation.

`--texture-anisotropy 1|2|4|8|16` selects the maximum anisotropic tap ratio for
trilinear mode. Without an override, the C++ terrain preset selects 1x for low
and medium, 4x for enhanced, 8x for high, and 16x for ultra. These numbers follow
the local C# OpenIGI preset table; they are a rendering reference, not retail
binary evidence. Long pixel footprints are filtered with multiple samples
along the major texture-gradient axis and the mip level from the minor axis.

The focused renderer CTest passes generated-level selection, authored-level
selection, anisotropic detail retention, mode separation, and serial/parallel
pixel equality. A one-frame Level 1 run loaded 426 textures (102 authored and
2,496 generated mip levels) and saved captures in each mode. The captures have
different SHA-256 digests, confirming the mode reaches the real-asset render
path. No paired retail frame or C# frame comparison was made. Retail's default
dithered-nearest mode is not reproduced by the current nearest-level sampler,
so Vanilla pixel parity remains open. There is no Direct3D 7 backend yet.

The nearest and trilinear 320x240 captures have SHA-256 values
`eaaf3830fd3b4d5b9bbc8f9a88052f6a808699571cecdf6937b077fcb11c1f0b` and
`913770f213831262a6c99b90825a08103b8273ede5dc200d9be7482514140838`.
Those initial runs rendered for screenshot capture after the loop and are not
performance measurements. Both initialized 413 objects, 28 soldiers, and one
Level 1 tick.

On the same build and Level 1 asset cache, a separate 320x240 low-terrain
`--benchmark` render took `0.020591 s` at 1x and `0.032188 s` at 16x. Capture SHA-256 values were
`913770f213831262a6c99b90825a08103b8273ede5dc200d9be7482514140838` (1x) and
`a56148f0b72bdc35db983f74895ee5882ab38558f10c6d14618efac54ef594c1` (16x).
At 320x240 with enhanced terrain (2,087,084 triangles), the automatic 4x
setting completed a benchmarked render in `0.175893 s`. These are single-frame
diagnostic timings, not stable averages. The CPU 16x path is visibly more
expensive and needs high-resolution profiling before it can be called playable.
At 160x120, Ultra loaded 5,655,922 terrain triangles, selected 16x by default,
and completed one benchmarked render in `0.295876 s`.

## Native application loop

- `igi_game` is a C++20 executable using SDL2 2.32.72 through SDL's C API. It
  initializes, processes quit/escape events, clears a frame, and shuts down.
- The bounded timer-only loop passed through CTest, and a three-frame SDL dummy
  video run exercised window and software-renderer setup in this environment.
- A normal macOS window could not be exercised from the current shell because
  SDL reported that the video driver had no displays. This is an environment
  limitation, not evidence of successful desktop rendering.
- CrossOver 26.3.0 launched the retail `igi.exe` (SHA-256 recorded below) with
  `window level1`; Wine logged the `igi.exe` process starting. The shell still
  had no capturable display, so no scene, input, or movement was observed. The
  task-launched process was closed after this startup check.
- The application loop does not yet invoke player simulation, collision,
  animation, assets, or other game systems. It is an executable shell, not a
  playable prototype or an IGI gameplay parity result.

## Locomotion graph

- CMake builds `igi_gameplay` with AppleClang in C++20 mode.
- A native smoke program checked the idle transition count/order, walk/run
  pairing, jump/fall reachability, invalid-state handling, and that every edge
  targets one of the 49 states.
- A row-by-row comparison against
  `open-igi/src/OpenIGI.Game/Player/HumanLocomotionGraph.cs` found 49/49 rows
  identical. That is a source cross-check of the port, not an independent
  replay comparison against `igi.exe`.

## Human movement integrator

- The source translation is `assets/BinaryNinja/cproj/src/004614B0-Human_MovementIntegrator.c`.
  REA/Ghidra pseudo-code evidence `ev_6a9564f5d84daa287a444b641a8e06fd2e967ffcc91b3b1248a482e9363ea1d4`
  and the call sequence recorded in `assets/BinaryNinja/cproj/PLAYER_MOVEMENT_PARITY.md`
  are the static basis for `HumanMovementIntegrator` in the C++20 gameplay library.
- The C++ translation preserves slope-slide thresholds, the 0.9 velocity and slide
  damping, position adjustment order, roof-contact flag clearing, wall/ground/
  separation query order, ground-query budgets, and transform-commit ordering.
- The source-field map is retained: world position `+0x20`, movement flags `+0x2F4`,
  animation movement `+0x520`, base movement `+0x664`, velocity `+0x670`, slide
  acceleration `+0x67C`, committed movement `+0x688`, collision controls
  `+0x694..+0x698`, slope normal `+0x6D8`, slope-enable value `+0x6F8`,
  position adjustment `+0x728`, and collision radius `+0xA0`. The interface uses
  named fields and leaves byte-offset marshaling to the future native-state adapter.
- `human_movement_integrator_probe` checks movement composition, collision call
  order, and the extended query budget for slope sliding with synthetic state
  and a recording mock world. It is a component check, not retail gameplay evidence.
- The movement source and probe pass individual AppleClang C++20 checks with
  `-Wall -Wextra -Wpedantic -Werror`. CTest passes both the bounded app-loop
  check and movement probe; a separate AddressSanitizer + UndefinedBehaviorSanitizer
  build also passes the movement probe. Leak detection was not independently verified.
- The real mission/world query and transform providers are not implemented, and
  `igi_game` does not invoke this component yet. Floor following, real collision,
  and retail-versus-port movement parity remain unverified.

## Retail binary observations

REA opened `/Users/haseeb-mir/Downloads/IGI/igi.exe` as PE32 x86 with SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, using
Ghidra 12.1.2.

The CrossOver bottle copy at
`/Users/haseeb-mir/Library/Application Support/CrossOver/Bottles/project-igi/drive_c/IGI/igi.exe`
was checked and had the same SHA-256. The direct CrossOver launch is only a
process-start observation; it did not produce a movement trace or a visual
retail-versus-port comparison.

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

The native catalog has 4,814 rows: 4,337 `PASS`, 476 `FAIL`, and one
`CANDIDATE`. Zero rows currently have `L5_runtime=true`. Catalog size and CFG
gates are not proof of runtime behavior.

## re-agent source-only check

The local `auto-re-agent` Python source tree ran a source-only parity check of
the generated C at `0x4614B0`. It returned `RED`: its body contains decompiler
stub markers. This check had no live Ghidra backend input and is not a binary
parity result. The full native runner also remains an isolated harness: all
five representative calls fault without initialized engine state, so it does
not provide gameplay runtime evidence.

CrossOver replay of the original remains necessary before movement, weapon,
collision, animation, or sound behavior can be called runtime-verified.

## Gameplay update: attachments, animation, portals and firing

Reference hash was rechecked on 2026-10-16 and still matches the SHA-256 above. This update uses C++20 in the native engine; references to the separate OpenIGI C# implementation are cross-checks, not a new runtime dependency.

- Recursive ATTA transforms and rest-bone skinning were adapted from the editor and OpenIGI references listed in `third_party/licenses/igi-editor-ref/PROVENANCE.md`; runtime format decoders live in `engine/src/formats/`.
- Original `magicobj/magicobj.qvm` declares `TASKTYPE_GUNFLAME` for models `1000_01_1`, `1005_01_1`, `1007_01_1`, `1008_01_1`, and `1010_01_1`. These attachments retain firing-only visibility and no longer contribute collision. The renderer regression checks idle, shot and expiry states. The timing remains provisional until aligned original captures exist.
- Original TAMC materials retain `portal != 0xffff` non-solid metadata. The previous adapter ignored it and treated every ECFC face as a wall. Capsule and generic ray queries now skip those openings. A failing synthetic crossing test passes after the change; the original Level 2 integration probe crosses at least one portal in the full scene and counts 148 such faces. This is an asset/integration result, not complete original movement verification. A follow-up correction loads original movement collision mesh level 1 separately from level 0 sight/projectile geometry. Dynamic doors, complete native query dispatch and special portal visibility rules still need verification.
- Original soldier table `0x0053ed10` entry 6 contains animation 56, mode 1, independently read with radare2. The renderer now selects that clip for attacking guards and exposes shot/muzzle events instead of showing only stand/walk. Ghidra has no procedure at `0x0045b060`; REA returned an explicit error. radare2 identifies a 289-byte, 14-block candidate there. No dual-provider translation of that handler or complete combat-state equivalence is claimed.
- Quaternion blend `0x004d4500`: REA evidence `ev_33955caea00f4397dd4d7933d75461a1f515a22287aa1276a5b39c27f3a98e67`. radare2 reports 391 bytes / 13 blocks. Rotation sampler `0x004d5180`: Binary Ninja and radare2 both report 1,394 bytes / 53 blocks; Ghidra has no procedure at that address. That missing-provider limitation remains open. Tangent direction, quaternion sign, short-angle path and loop cases have synthetic tests; bit-exact poses do not yet have matched retail captures.
- Input registrar evidence: `ev_d231cf4ffcaa1d0c797c2f365c91618d1fcc80e9f55bfa36ccdc84d1cea9aebd`, containing function `0x00402ff0`; GOInRemap string `0x0053324c` and xrefs are preserved in earlier local evidence. Numeric keyboard/mouse bindings are read from original config data.
- Weapon cycling research only: REA/Ghidra identifies `0x0040f760`, 451 bytes / 37 blocks, dossier `ev_e20fcff877d48d2e50c82089bae9e0bb62121571716862f48f5c6097755cfb66`, pseudocode `ev_09a00b71a702c9dd7892a504c9f89deddd8a16560bf4bd12f7b5c2703226f49e`. radare2 independently reports the same boundary/size/block count (count score 1.0, not semantic proof). It cycles the original global weapon list and skips unowned/unselectable records. This investigation has not yet replaced the prototype inventory-order cycling.

REA's original CrossOver process-capture attempt failed process-ownership cleanup validation and produced no accepted retail capture. Reported PIDs were checked as pre-existing Chrome processes; none was terminated. The available host Frida autonomy workflow is arm64 Mach-O only and cannot establish original PE32-under-Wine gameplay behavior. Existing IGIProbe HOOK startup lines prove setup only. The retail gate consequently remains failed.

Functional, sanitizer, analyzer and failing-retail logs are retained locally under ignored `dist/verification/gameplay-bugs/`. Current counts are in [MVP_STATUS.md](MVP_STATUS.md). Sound/cutscene/menu/text work is explicitly deferred; the gate requires 17 gameplay scenarios and accepts no synthetic replacement for original traces.

### Grounded building-entry follow-up

A stricter floor-aware portal test initially failed. Projecting an isolated capsule through a portal plane did not reproduce the full movement loop's step/ground behavior. The corrected test uses original root tracks, the complete translated scene, and 15 forward-input ticks in each direction; both directions finish across the opening and grounded. It does not teleport the live application or substitute a fabricated doorway.

Original wall-query assembly at `0x00461a70` records `PUSH 1` at `0x00461d3d` before dispatch to `0x004cdb20` at `0x00461d71`; REA assembly evidence `ev_169b6ae80e97eb4a42b6eb2ed048dada0bf768fda9e2f2343a4bb7d3c248099e`, pseudocode `ev_db777e56eb27416e6d076dcf16534daac1152765101d622a664dc663f04290c2`. radare2 independently decodes the containing function and same call sequence. The dispatcher has nine arguments and preserves its final argument in the collision query state: REA `ev_a7fe91b77f871e9aafedf10495012009c25d2aa246d454dbebf2132065266621`. The separate reference implementation identifies this slot as the movement mesh level. The helper at `0x00416850` is only a six-byte global-byte getter in both providers; nested-looking pseudocode arguments are not evidence that helper consumes the pre-pushed dispatcher arguments.

The engine now keeps original ECFC/XTVC/TAMC level 1 movement geometry separate from level 0 generic traces. A synthetic regression also checks that a default trace hull can remain opaque while the movement mesh allows traversal. This preserves query separation; it is not a translation or runtime verification of the full original dispatcher.

### Original executable runtime observations, 2026-10-16

A subsequent direct Windows x86 IGIProbe capture succeeded where the earlier REA process wrapper had failed ownership validation. This is a local original-executable observation, not an accepted original-versus-port comparison and not a replacement REA evidence ID.

- Staging: `/tmp/igi-retail-gameplay-capture-full/`; retained logs, source and manifest: ignored `dist/verification/gameplay-bugs/original-runtime/`.
- The staged `igi.exe` hash equals the unchanged original reference hash. The original Downloads executable and its pre-existing `igi-probe.log` hashes were checked again and remained unchanged. Existing probe-log SHA-256: `f975c7eeb74ba8aa0a03b0ef273643b2b2f9fe7e30894c5e59b8b15638a09982`.
- Existing OpenIGI x86 probe sources were staged and built with installed `i686-w64-mingw32-g++`, C++20, static compiler runtimes. The injector was adapted to retain the process handle, explicitly set the hook selection and sample rate, use the original asset working directory, wait 30 seconds and terminate only that created game process. Both resulting tools were verified PE32/x86.
- Launch: CrossOver bottle `project-igi`, staged executable, arguments `window level2`. No synthetic keyboard/mouse inputs were injected.
- Eight expected hooks armed, zero SKIP records, sample rate one. The file contains **63 animation-state observations** and **46 first-call movement/ground observations**. Named gameplay guards including 2000, 2001, 2003, 2005, 2007, 2008, 2011, 2012, 2014, 2015, 2017, 2019 and 2020 enter state 3 / animation 57 / mode 1. This differs from the prototype's current moving-guard clip 34 choice, but the task/input/phase contexts are not yet aligned for a differential claim.
- Runtime compatibility difference: the existing probe patches joystick enumeration at `0x005099c6` in memory to avoid its acquisition failure. Original disk bytes are untouched. The probe uses its fallback file log directly; no ring tailer comparison or overrun audit was performed.
- **Unobserved:** requested player task-0 per-tick snapshots (zero rows), combat events, round events, controlled building entry, and a validated cutscene-to-gameplay boundary. The movement/ground handlers mostly log their first call per human. These observations therefore do not prove player trajectories or enemy-fire behavior.
- The launcher reports its owned process closed at the 30-second boundary. That closure is deliberate capture cleanup, not natural mission completion. Concurrent development work and instrumentation affect timing; this is not a native-Windows performance oracle.

The manifest retains file hashes, tool/source hashes, command line, hook/sample settings, raw event counts and limitations. Next: establish the gameplay boundary, capture controlled original inputs and corresponding per-tick state, then align a native replay and identify the first divergence. The 17-scenario retail gate stays failed until genuine matching captures and reviewed provenance exist.

### Controlled original-input attempt

A second owned 30-second capture added a player-update hook at `0x004109a0`, verified as a 1,480-byte entry by radare2. REA/Ghidra pseudocode evidence: `ev_da3795453a2b6965f86a10ea60c2209bfec2024cc3147338e77adcad94ac90c0`. Its first argument is the human instance; this hook observes state at entry, before that tick's movement.

The launcher sent ten logged key/mouse edges for Escape, W, E, left mouse and F, only after verifying its own Windows process was foreground. Win32 SendInput reported acceptance for all ten. Nine expected hooks armed, no SKIP. The capture produced only **one player-update snapshot and one player-integrator snapshot**, showing task 0, position `(126131752, -37800692, 175432800)`, zero velocity, flags zero, state zero, accumulated damage zero and maximum-health field 3. No combat or round observations followed.

This attempt does **not** verify walking, jumping, firing or reload: API acceptance does not prove input consumption during gameplay, and the cutscene/menu/focus or instrumentation stall has not been resolved. Preserve this as an incomplete experiment, not a passing end-to-end scenario. Raw logs, staged observer/launcher source and manifest are under ignored `dist/verification/gameplay-bugs/original-controlled-attempt/`. The original executable and earlier probe log remain untouched.


### Retail soldier hit-zone damage and Level 1 mesh trace

The retail executable was `/Users/haseeb-mir/Downloads/IGI/igi.exe`, PE32 x86, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`. REA/Ghidra 12.1.2 returned shipped-artifact evidence for the relevant routines:

- `0x45F930`, evidence `ev_6846e08c154e0c699d75e12fa17dc18efee843b12fa8547ff137188ef3dd79b8`, is a 150-byte body. Its pseudocode calls `0x489C60`, stores the selected zone, reads the zone row through `0x489E10`, multiplies the incoming damage by the row multiplier, accumulates zone and total damage, and sets the death flag when total damage reaches the maximum-health field.
- `0x489C60`, evidence `ev_bf2aae2dcd0aa4edc128c7ba8ff699acebe7a46c8869f7cfc97741af3c436dc0`, is a 417-byte / 20-basic-block resolver. Ghidra and radare2 independently report the same byte size and block count. It bounds the body-part index at 30, indexes `0x5412A4`, refines default zone 12 using the forward/side components, and applies the 614.4-world-unit proximity check against bone 4 before selecting zone 15. This is evidence about the retail resolver only; it is not a CFG parity score for the C++ port.
- `0x489C30`, evidence `ev_315541156235666e17ba5633a817df2204b8504a62fe3fb3e3a8c15c6bd44029`, initializes the human maximum-health field to `1.0` and invokes the 17-zone initializer. `0x489E30`, evidence `ev_ade258cf58da530fbdf59ac8057ad9388a13a864512efb6f53fe9e56c9c5f7e7`, copies 17 zone rows.
- REA read the complete 272-byte zone table at `0x53F1D0` (evidence `ev_da44eec0055e063d4ae2abe95d0a1e4b6e2e1a3adf3bfcf087ac51f93390ba8e`) and the complete 120-byte body-part map at `0x5412A4` (evidence `ev_bb6593ca64fba9012bc4279b404ae59de4c99888a4ff7a480a6e1315d151b29e`). The multiplier sequence in the compiled soldier table is `0.5, 0.75, 1.0` for each limb triplet, `1.0` for zones 12–14, and `100.0` for zones 15–16. The body-part table has 30 entries. These are read-only bytes from the shipped executable, not live process memory.

The C++ port now retains `XTVC` bone indices for each `ECFC` collision triangle, skins those vertices with the active soldier pose, ray-traces player projectiles against the triangles, maps the struck bone through the observed body-part table, and applies the observed soldier multiplier. The first-triangle-vertex rule, with the C# implementation's two-matching-vertices exception, is currently sourced from vanilla OpenIGI `HumanHitTest.TryCastMesh`; the original trace caller that supplies `0x489C60`'s hit record has not been fully recovered here, so that triangle-to-bone selection is not independently proven from REA. The C# source is a port reference, not evidence that behavior exists in retail.

Validation on 2026-10-17:

- Debug C++ build: `cmake --build build --target mvp_gameplay_probe mvp_real_scene_probe -j 4` succeeded.
- `mvp_gameplay_probe` passed its movement, jump, crouch, automatic-fire, damage, death, reload, occlusion, and restart checks.
- `mvp_real_scene_probe /Users/haseeb-mir/Downloads/IGI /tmp/igi-l1-hit-zone-cache` loaded Level 1 and verified 12,718 soldier collision triangles with bone mappings. Its controlled shot hit active gameplay guard 2017 on bone 14 / zone 5, applied the zone multiplier, and registered seven hits before defeat. The probe aims through a mesh-intersecting scripted point; it is not a player-controlled retail replay.
- Focused CTest: 5/5 passed (`mvp_renderer_depth_clipping`, `mvp_collision_world`, `mvp_movement_weapon_ai`, `mvp_navigation_inventory`, `mvp_original_level_gameplay`).
- Clang static analyzer: `game_combat.cpp`, `mission_assets.cpp`, `mvp_real_scene_probe.cpp`, and `mvp_gameplay_probe.cpp` completed with no diagnostics.
- No matched original/port memory snapshot, player damage-zone test, hit/death reaction parity, headshot test, or all-zone coverage was produced. The retail acceptance gate remains failed.


### macOS developer-mode shortcut replacement

The desktop event loop now toggles developer mode on `G` or `F9`; it no longer advertises or binds F11. `G` avoids macOS's function-key/media-key handling. Startup help and the runtime HUD instructions were updated together.

Validation on 2026-10-17:

- `igi_game` and `mvp_navigation_inventory_probe` built successfully with C++20.
- `mvp_navigation_inventory` passed, including developer mode granting the mission weapons, reserve ammunition, and maximum health. This probe tests simulation behavior, not SDL key delivery.
- Manual keyboard verification of `G` is pending; no live GUI key event was injected or claimed.
