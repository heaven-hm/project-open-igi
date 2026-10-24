# Human player movement

Owner: **Agent C (OpenCode)**. Read this with
[`MVP_NATIVE_REFACTOR_GUIDE.md`](MVP_NATIVE_REFACTOR_GUIDE.md).

The target is the playable demo: `open-igi.exe window levelN` next to
`igi.exe window levelN`, the human walking, running, jumping and crouching over the
ported terrain, with the camera on the retail eye position. The demo is not accepted
until paired retail/candidate observations exist; this file records the configuration
evidence and the exact remaining gaps.

## 1. Evidence

Decoded with `igi1conv` v1.11.0 (`qvm decompile`) and a byte-level read of the raw code
table. Output and hashes:

| File | SHA-256 | Decoded |
| ---- | ------- | ------- |
| `config.qvm` | `9fc8cc579f86db2e456972d0678161a303e84a40857a3f3f1a98e115042dd3b0` | `docs/fidelity/evidence/agent-c-config-20261020/config.decompiled.qsc` |
| `humanplayer/humanplayer.qvm` | `6749062f42121bf66c045aa62cc0d93eaab60f81400a5c687f546d0ab57b15ce` | `.../humanplayer.decompiled.qsc` |
| `LOD.QVM` | `8dc4ffebeec643dcc1a7ea234a8fb484a044451cf99250307a1752a18b53b2ae` | `.../lod.decompiled.qsc` |

The three files are compiled LOOP 8.5 programs. The instruction subset they use to build
constant arguments, recovered by correlating the converter disassembly with the raw code
table at `ctable`:

| Opcode | Instruction | Encoding |
| ------ | ----------- | -------- |
| `00` | `BRK` | 1 byte |
| `02` | `PUSH` | `u32` immediate |
| `03` | `PUSHB` | `u8` |
| `04` | `PUSHW` | `u16` |
| `05` | `PUSHF` | `f32` |
| `09` | `PUSHSIB` | `u8` string-constant index |
| `0A` | `PUSHSIW` | `u16` string-constant index |
| `0D` | `PUSHIIB` | `u8` identifier index |
| `0F` / `10` | `PUSH0` / `PUSH1` | 1 byte |
| `12` | `POP` | 1 byte |
| `14` | `BRA` | `u32` |
| `18` | `CALL` | `u32 count`, then `count * u32` argument offsets |
| `2C` | `MINUS` | 1 byte |

A call is `PUSHIIB <symbol index>` followed by `CALL`. Each argument offset points at a
constant expression block that ends in `BRK`. Symbol index *i* resolves to
`symbol_pool[offset_table[i]]`; the same table holds the callee names and the identifier
arguments (`MoveLeft`, `INPUTPORT_DEVICE_KEYBOARD`, `KEY_LEFT`, …).

## 2. `config.qvm`

The shipped file binds **27 actions**. Movement is on the arrow keys, jump on `J`,
crouch on the right control key, walk/run toggle on `W`:

| Action | Device | Control |
| ------ | ------ | ------- |
| `MoveLeft` | keyboard | `KEY_LEFT` |
| `MoveRight` | keyboard | `KEY_RIGHT` |
| `MoveUp` | keyboard | `KEY_UP` |
| `MoveDown` | keyboard | `KEY_DOWN` |
| `Fire` | mouse | `MOUSE_BUTTON_1` |
| `Reload` | keyboard | `KEY_R` |
| `Activate` | keyboard | `KEY_Q` |
| `Crouch` | keyboard | `KEY_RIGHT_CTRL` |
| `Jump` | keyboard | `KEY_J` |
| `NextWeapon` | mouse | `MOUSE_BUTTON_2` |
| `PrevWeapon` | keyboard | `KEY_O` |
| `AlternateFire` | keyboard | `KEY_F` |
| `Peek` | keyboard | `KEY_NUMPAD1` |
| `Binoculars` | keyboard | `KEY_SPACE` |
| `MapComputer` | keyboard | `KEY_C` |
| `ToggleWalkRun` | keyboard | `KEY_W` |
| `ZoomIn` | keyboard | `KEY_PAGEUP` |
| `ZoomOut` | keyboard | `KEY_PAGEDOWN` |
| `WeaponCategory1..9` | keyboard | `KEY_1` .. `KEY_9` |

Also decoded: mouse sensitivity `0.3`, mouse inversion `0`, player name `Haseeb`,
active mission `13`, `GOStart(0)`, empty content-control password, 1024x640 display
mode, 16-bit depth, English, difficulty `GD_2`, blood on.

## 3. `humanplayer.qvm`

`DefineHumanPlayerGeneral` takes **43 positional arguments**. Only the leading four have
an established meaning from the movement natives
(`Human_RunUpdate` 0x004109A0, the airborne integrator 0x00408460):

| Index | Value | Meaning |
| ----- | ----- | ------- |
| 0 | `1.75` | root-motion scale |
| 1 | `17.5` | jump horizontal speed, km/h |
| 2 | `27` | jump vertical speed, km/h |
| 3 | `0.5` | air-control speed, km/h |
| 4..5 | `0.85`, `0.85` | kept, not named |
| 6 | `0.25` | kept, not named |
| 7 | `3.0` | kept, not named |
| 8..41 | per-state triples, e.g. `(0.5, 0.75, 1)` | kept, not named |
| 42 | `100000` | kept, not named |

The remaining indices are preserved in order by the reader; they are not named because
the evidence does not establish their meaning yet. The file also declares the weapon
cycle, weapon categories and ammo limits; those calls are decoded by the generic reader
but are outside the movement scope.

## 4. `LOD.QVM`

`LOD.QVM` is a **model** LOD task table, not terrain LOD: `Task_DeclareParameters`
`ModelLODSettings` with `Model`, `Distance to LOD 1..4` and `Distance to cutoff`, then
**462** `Task_New` rows:

```
Task_New(-1, "ModelLODSettings", <task name>, <model>, d1, d2, d3, d4, cutoff)
```

First rows: `300_01_1` → `7, 17, 30, 35, 50`; `300_02_1` → `7, 18, 30, 35, 50`;
`301_01_1` → `25, 45, 45, 45, 80`; `302_01_1` → `5, 15, 25, 35, 200`;
`313_01_1` → `5, 7, 9, 9, 9`.

Agent C owns decoding the table. Agent A's model renderer consumes it; the terrain
renderer does not.

## 5. The code

| File | Role |
| ---- | ---- |
| `engine/include/igi/retail/config/retail_config.hpp`, `engine/src/retail/config/retail_config.cpp` | Reads a compiled configuration program into a constant-argument call list and decodes `config.qvm`, `DefineHumanPlayerGeneral` and the `ModelLODSettings` table. Unsupported opcodes and non-constant arguments fail the read. |
| `engine/include/igi/retail/config/retail_input_bindings.hpp`, `engine/src/retail/config/retail_input_bindings.cpp` | Binds the decoded rows to `input::InputPort`: action name → `Action`, device name → `Device`, control name → Win32 virtual-key code (or mouse button). Unknown names fail instead of binding nothing. |
| `engine/tests/config/retail_config_test.cpp` | Asserts the shipped values above against the real files, including the port latching `move_left`, `jump`, `crouch` and `fire` from the raw device codes. |

Run it against the installed game data:

```sh
cmake -S engine -B build-agent-c -DIGI_ENABLE_NATIVE_GATE=OFF -DBUILD_TESTING=ON
cmake --build build-agent-c --target igi_retail_config_tests -j8
./build-agent-c/igi_retail_config_tests
```

This is a host configuration reader, not a native transcription and not a QVM
interpreter. It does not execute script logic and it does not replace the retail script
VM; it only recovers the constant configuration the shipped files pass to the natives.

The two sources and two headers are listed in `HOST_ONLY_SOURCES` in
`scripts/native_gate.py` so the changed-native gate does not demand an address-bound
identity for host tooling; the gate still verifies the six `Config_*` records in the
same directory. After the gate change the trust anchor
`docs/fidelity/gate-integrity.sha256` was updated; `scripts/check_gate_integrity.sh`
reports OK. The directory gate result is:

```sh
python3 scripts/native_gate.py --root . --tree engine/src/retail/config
# native_gate: OK — 6 changed native(s) passed ...
```

## 6. Gaps, in order

### 6.1 Bindings are applied at scene load (wired and runtime-observed)

`engine/platform/windows/retail_model_scene.cpp` now reads `config.qvm` in `Scene::load`,
after the HumanPlayer spawn is installed and before terrain loading, and installs every
decoded binding on the scene's `input_port`. The temporary blocking throw is removed.
On 2026-10-20 the user authorized Agent C to make this change in Agent A's scene; the
ownership rule in the guide records the exception. An unknown action, device or control
throws with the failing name, so a binding cannot silently disappear.

```cpp
config::ScriptProgram config_program;
config::InputConfig input_config;
std::string input_config_error;
if (!config::read_script_file(root / "config.qvm", config_program, input_config_error)
    || !config::decode_input_config(config_program, input_config, input_config_error)
    || !config::apply_input_config(input_config, input_port, input_config_error)) {
    throw std::runtime_error("config.qvm: " + input_config_error);
}
```

`root` is the game directory (`Scene::load` receives `executable_path().parent_path()`),
and `input_port` is a member. The same block reads
`humanplayer/humanplayer.qvm` and calls `player.set_parameters(...)` with the decoded
`DefineHumanPlayerGeneral` values; see §6.2.

Runtime evidence (2026-10-20, CrossOver bottle `open-igi`, `window level1`):

```
config.qvm: applied 27 action bindings
humanplayer.qvm: root-motion scale 1.750000, jump 17.500000/27.000000 km/h, air control 0.500000 km/h
Relative mouse input registered; config.qvm sensitivity and inversion remain active.
First 3D frame presented: terrain_batches=14 objects=919 model_draw_batches=4112 uploaded_textures=398
```

With the synthetic `inputdemo` sequence the port delivered 5 key and 10 mouse messages;
the demo looks up the configured code for each action before posting it, so the jump
used the `Key J` binding. Look changed yaw/pitch and the jump action left the ground.
Ground motion then failed at the root-motion check in §6.2, as designed. Commands, executable and input-file
hashes, the raw excerpt and the limitations are in
`docs/fidelity/evidence/agent-c-runtime-20261020/runtime-notes.md`.

This is a host adapter, not script execution: it decodes the constant rows of the shipped
file and does not run `config.qvm` in the retail QVM interpreter. Handling an edited
config with new script logic remains the VM work; the binding path must keep its current
behaviour when that lands.

### 6.2 Ground locomotion uses the retail clip root motion (implemented 2026-10-20)

`PlayerController::update` refuses to run until `set_parameters()` is called:
`"HumanPlayer parameters are not set; read humanplayer.qvm before the first update."`
`HumanPlayerParameters` carries the root-motion scale, jump horizontal, jump vertical
and air-control values decoded from `DefineHumanPlayerGeneral`; the unit test installs
the shipped values as its fixture and asserts the vertical jump is exactly `1024.0`
retail units per tick (`27 km/h * 4096000/108000`).

The grounded branch now consumes the retail animation clip instead of throwing. The
static evidence is:

| Evidence | Address / value |
| -------- | --------------- |
| Human state → animation table | `0x00538870`, written by `0x004104B7`; entries 0..16 are `{2,4,37,35,64,70,68,11,10,57,74,72,75,77,76,81,80}` |
| State selection / table read | `Human_StateAnimation` `0x00489E70` indexes the table with the state at `+0x10` |
| Clip time step | `0x004D2D30` stores `4800.0 / ticks_per_second`; `0x00534B14 = 4800.0` and `0x00533488 = 30.0`, so **160 ms per tick** |
| Root sample | `0x004D2FB0` / `0x004D3020`; `0x004109A0` scales by `DefineHumanPlayerGeneral` argument 0 (`1.75`) when `+0x52C` is clear, rotates by the body matrix |
| Per-tick integration | `0x004614B0` adds the per-tick velocity and the scaled root delta to the position |

The host implementation is:

- `engine/platform/windows/retail_animation.{hpp,cpp}` reads the animation IFF by tag
  (`BOAH` clip header, `BOTH` key count, `BOTD` 40-byte `{x,y,z,time,tangent_in,
  tangent_out}` keys). The FORM sizes in these files are not trustworthy. It samples
  the root translation track.
- `PlayerController::select_locomotion_state` maps forward/back/strafe pairs to states
  1..8 and adds 8 for crouch, matching the table order. `advance_root_motion` advances
  160 ms per tick, wraps at the clip duration without adding the end-to-start
  discontinuity, scales by `parameters_.root_motion_scale`, and `tick_step` rotates the
  local `+X`/`+Y` delta by the human yaw before adding it to the feet position.
- `engine/platform/windows/retail_model_scene.cpp` loads `COMMON/ANIMS/000.IFF` and
  installs the clips. The player model logged at runtime is `000_01_1` (rig object id
  0), so `000.IFF` is its animation file.

Runtime observation (2026-10-20 15:57Z, `window level1 inputdemo`, executable
`a09c5d5e...`): the scene loaded 122 clips (`forward=3360 ms`) and the player model
was `000_01_1`. The `inputdemo` sequence (standing forward, mouse look, jump, crouch,
walk toggle) moved the feet 43,275 native units in 4.26 s (about 8.9 km/h wall-clock);
the final sample is crouched. The two relevant clips are: forward clip 4, a 3,360 ms
loop with 10,035.7 units (21.15 km/h), and crouch-forward clip 57, a 5,600 ms loop
with 6,123.7 units (8.07 km/h) at 160 ms/tick and the 1.75 scale. The displacement
magnitude is consistent with the sequence, but the 4.5 s log interval does not split
the phases, so this is not a per-tick speed match. The ground-locomotion throw did
not fire. The excerpt is
`docs/fidelity/evidence/agent-c-runtime-20261020/level1-root-motion-excerpt.log`; the
full run notes record the bottle and the limitations.

Remaining gaps:

- The root sampler interpolates between key positions linearly. The retail spline
  tangent sign/order convention is not established, so the sub-key profile is not
  proven; the displacement over a loop (and therefore the average ground speed) is
  unchanged. The editor's reference sampler is also linear.
- The ten `(0.5, 0.75, 1)` triples in `DefineHumanPlayerGeneral` and the
  `ToggleWalkRun` binding are not decoded. The same clip is used for walking and
  running; no multiplier is guessed.
- The 15:57Z run used the `project-igi-2` bottle because a retail `level4` run held
  `IGIMUTEX` in `open-igi`, and it used the synthetic `inputdemo`. It is a
  candidate-only smoke test; the paired retail/candidate comparison is still
  outstanding. See §7.

### 6.3 Mouse look

The port applies the decoded sensitivity and inversion. `HumanView`'s look rate is still
the documented hardcoded `0.0035` radian per count; that native is Agent A's input/camera
work.

## 7. Acceptance

The demo is accepted only with paired runs, retail first:

1. `cd /Users/haseeb-mir/Downloads/IGI && igi.exe window level7`, run the five steps,
   close it, confirm the process is gone.
2. `cd /Users/haseeb-mir/Downloads/IGI && open-igi.exe window level7`, same steps.
3. Steps: stand still; walk forward into a wall; walk up a slope; jump on flat ground
   and land; crouch and stand.
4. Compare feet position, eye position, yaw, pitch, ground contact, crouch state and the
   rendered frame, with the same config, level and scripted inputs.
5. Retain the logs, Frida traces, executable hashes and level-script paths under
   `build/runtime-traces/` or `docs/fidelity/evidence/`.

A successful build or a passing unit test does not accept the demo; the paired
observation does.
