# Agent C runtime note — config + human-player parameters (2026-10-20)

> **Update 2026-10-20 15:57Z:** the ground-locomotion gap described in the last
> section is now implemented with the retail clip root motion. The candidate moved
> under `inputdemo`; see **Root-motion run** at the end of this file. The earlier
> "what the observations establish" section is retained as the history of the
> config-only run.

## What this records

The first runtime observation of the new host call site in
`engine/platform/windows/retail_model_scene.cpp`:

- `config.qvm` is read at scene load and its action bindings are installed on the
  scene's input port.
- `humanplayer/humanplayer.qvm` `DefineHumanPlayerGeneral` values are installed on
  `PlayerController` before the first update.

This is **not** a claim of retail parity. It records the wiring and the observed
runtime effect, and it states what remains untested.

## Build and executable

| Item | Value |
| ---- | ----- |
| Build | `build-agent-c-win32` (`i686-w64-mingw32-g++`, `-DIGI_ENABLE_NATIVE_GATE=OFF`, Release) |
| Executable | `build-agent-c-win32/engine/platform/windows/open-igi.exe` |
| SHA-256 | `c8f75f951791bbb1240d5c15ab43ab9d678b42de88d5a5d40bbfab5d6bed7571` |
| Game directory | `/Users/haseeb-mir/Downloads/IGI` |
| Level | `missions/location0/level1` |
| Bottle | CrossOver `open-igi`, windowed (`window`) |

The build output was copied to `/Users/haseeb-mir/Downloads/IGI/open-igi-agentc.exe`
only so that the scene resolves `config.qvm`, `humanplayer/` and `missions/` from
`executable_path().parent_path()`. The deployed `open-igi.exe` was not replaced,
and the copy was removed after the runs.

## Input files

| File | SHA-256 |
| ---- | ------- |
| `config.qvm` | `9fc8cc579f86db2e456972d0678161a303e84a40857a3f3f1a98e115042dd3b0` |
| `humanplayer/humanplayer.qvm` | `6749062f42121bf66c045aa62cc0d93eaab60f81400a5c687f546d0ab57b15ce` |
| `LOD.QVM` | `8dc4ffebeec643dcc1a7ea234a8fb484a044451cf99250307a1752a18b53b2ae` |

## Commands

```sh
cd /Users/haseeb-mir/Downloads/IGI
env LC_ALL=C LANG=C \
  /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
  --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\open-igi-agentc.exe' window level1
```

Second run, same command plus `inputdemo`:

```sh
... open-igi-agentc.exe window level1 inputdemo
```

Each process was killed after about 25 s. No `igi.exe` was started, so the
`IGIMUTEX` single-instance rule was not involved. `open-igi-model-scene.log` is
written in the working directory and is append-only; the captured lines below are
the ones added by these runs.

## Observed output

Run 1 (no synthetic input):

```
config.qvm: applied 27 action bindings
humanplayer.qvm: root-motion scale 1.750000, jump 17.500000/27.000000 km/h, air control 0.500000 km/h
Relative mouse input registered; config.qvm sensitivity and inversion remain active.
First 3D frame presented: terrain_batches=14 objects=919 model_draw_batches=4112 uploaded_textures=398
```

Run 2 (`inputdemo`), same startup lines, then:

```
HumanPlayer feet=24578514.000/-55852816.000/174421680.000 eye=0.000/0.000/0.690 yaw=-3.096 pitch=0.000 on_ground=1 crouched=0 sliding=0 ground=174421680.000 key_msgs=0 mouse_msgs=0 ...
[WARN] Player update: Ground locomotion requires retail animation root-motion; no fallback speed is available.
HumanPlayer feet=24578505.355/-55853005.432/174416087.048 eye=-0.002/-0.046/-0.675 yaw=-2.983 pitch=-0.038 on_ground=0 crouched=0 sliding=0 ground=174413141.334 key_msgs=5 mouse_msgs=10 ...
```

The full filtered excerpt is in `level1-runtime-excerpt.log`.

## What the observations establish

- The scene reader found and decoded the shipped files: 27 action bindings and the
  four `DefineHumanPlayerGeneral` values (`1.75`, `17.5`, `27`, `0.5`).
- The input port received the bindings: the `inputdemo` sequence looks up the
  *configured* code for each action before posting it (it does not hardcode a key),
  and the resulting messages produced 5 key and 10 mouse events. Look changed yaw
  (`-3.096` -> `-2.983`) and pitch (`0.000` -> `-0.038`), and the configured jump
  action left the ground (`on_ground=0`, feet z `174421680` -> `174416087`).
- Ground locomotion still refuses to run without a retail animation root-motion
  sample. The warning is the designed failure, not a crash; the controller keeps
  the previous state and no speed is substituted.
- The spawn settles from `174421680` to the terrain surface `174413141` in the
  no-input run. The camera stays one eye height above that surface
  (`eye z = -1.395` in level-relative metres, ground at `-2.083`), so this is
  terrain settling, not a camera below ground.

## Limitations

- The configuration is read by a host constant-call reader, not by executing
  `config.qvm` in the retail QVM interpreter. It reproduces the shipped file's
  values; it is not evidence that arbitrary script logic is supported.
- `inputdemo` posts a fixed synthetic key/mouse sequence. It proves the binding
  path and the jump action, not a human-driven session.
- No paired retail run was made in this step, so nothing here is a parity or
  `VERIFIED` claim. The paired level comparison in
  `docs/fidelity/human_player_movement.md` §7 is still outstanding.
- Walking, running and landing are unobserved because the root-motion input does
  not exist yet; the controller fails loudly on the first grounded move.

## Root-motion run (2026-10-20 15:57Z)

### Build and executable

| Item | Value |
| ---- | ----- |
| Build | `build-agent-c-win32` (`i686-w64-mingw32-g++`, `-DIGI_ENABLE_NATIVE_GATE=OFF`) |
| Executable | `build-agent-c-win32/engine/platform/windows/open-igi.exe` |
| SHA-256 | `a09c5d5e8148247b318abd86c84153b1847d615367f4d16af9608ccc999e7607` |
| Game directory | `/Users/haseeb-mir/Downloads/IGI` |
| Level | `missions/location0/level1` |
| Bottle | CrossOver `project-igi-2`, windowed (`window`) |

The `open-igi` bottle was occupied by a retail `igi.exe window level4` run from
another agent, which holds `IGIMUTEX`; the candidate was therefore smoke-tested in
the `project-igi-2` bottle. This is a **candidate-only** observation, not the paired
acceptance run.

Excerpt: `level1-root-motion-excerpt.log`.

### What changed

- `engine/platform/windows/retail_animation.{hpp,cpp}`: host reader for the retail
  animation IFF. It scans `BOAH` clips and their `BOTH`/`BOTD` root translation keys
  by tag (the FORM sizes are not trustworthy) and samples the root track.
- `engine/platform/windows/player_controller.{hpp,cpp}`: the grounded branch now
  selects the retail human state, advances the clip by `4800/30 = 160 ms` per tick,
  samples the root delta, multiplies it by `humanplayer.qvm` argument 0 (`1.75`),
  rotates it by the human yaw and adds it to the feet position. The old unconditional
  `"Ground locomotion requires retail animation root-motion"` throw now only fires when
  the state's clip or its root track is missing.
- `engine/platform/windows/retail_model_scene.cpp`: loads `COMMON/ANIMS/000.IFF` and
  installs the clips on the controller.

### Observed output

```
Character skeletons loaded: 6 of 6 animation files
config.qvm: applied 27 action bindings
humanplayer.qvm: root-motion scale 1.750000, jump 17.500000/27.000000 km/h, air control 0.500000 km/h
Animation clips: COMMON/ANIMS/000.IFF clips=122 forward=3360.000000 ms
placement source=OBJECTS.QVM code_offset=144891 type=HumanPlayer model=000_01_1
First 3D frame presented: terrain_batches=14 objects=919 model_draw_batches=4112 uploaded_textures=398
HumanPlayer feet=24578514.000/-55852816.000/174421680.000 ... on_ground=1 crouched=0
HumanPlayer feet=24574742.162/-55895927.621/174413141.334 ... on_ground=1 crouched=1 key_msgs=5 mouse_msgs=10
HumanPlayer feet=24574742.162/-55895927.621/174413141.334 ... on_ground=1 crouched=1 key_msgs=8 mouse_msgs=10
```

There is no ground-locomotion warning in this run. The player moved from
`(24578514, -55852816)` to `(24574742.162, -55895927.621)`: 43,275 native units
(10.57 m) in the 4.26 s between the two log lines, about 8.9 km/h wall-clock. The
final sample is crouched (`crouched=1`), but the `inputdemo` sequence runs a
standing-forward phase first (clip 4), then mouse look, jump, crouch and the
walk-toggle. The two clip speeds from `000.IFF` are:

| State | Clip | Loop | Root translation | Speed at 160 ms/tick, 1.75 |
| ----- | ---- | ---- | ---------------- | -------------------------- |
| forward | 4 | 3,360 ms | 10,035.7 units | 21.15 km/h |
| crouch forward | 57 | 5,600 ms | 6,123.7 units | 8.07 km/h |

The observed displacement magnitude is consistent with the sequence, but the 4.5 s
log interval does not split the standing and crouched phases, so this is not a
per-tick speed match. A per-phase capture remains outstanding.

The placement line independently confirms the clip file: the player model is
`000_01_1` (rig object id 0), so `000.IFF` is its skeleton's animation file. The
previous inference from the model rig and the `BANM` list is now runtime-confirmed.

### Limitations

- The root sampler interpolates the key positions linearly. The retail spline tangent
  sign/order convention is not established, so the sub-key motion profile is not yet
  proven; the displacement over a loop (and therefore the average ground speed) is
  the same.
- The per-weapon-category speed triples in `humanplayer.qvm` and the effect of the
  `ToggleWalkRun` binding are unresolved. The same clip is used for the standing and
  walk/run paths; no multiplier is guessed.
- The run used a different bottle from the retail process and a synthetic
  `inputdemo`; it is not a paired level comparison and does not establish parity.
- The frame rate was about 6-7 FPS, so wall-clock speed includes tick-batching and
  input timing.
