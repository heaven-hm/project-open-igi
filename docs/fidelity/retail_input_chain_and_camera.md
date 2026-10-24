# Retail input chain and human camera

Status: `STRUCTURALLY_VERIFIED` for the port's own input port, camera placement and
movement model. The retail ground speed remains provisional because the port does not
play the human animation.

## What the retail executable does

The retail game binds its in-game actions to the devices in `config.qvm`, not in code:

```
GOInMouInv(0)
GOInMouSens(0.4)
GOInRemap("MoveLeft",  "INPUTPORT_DEVICE_KEYBOARD", "KEY_A")
GOInRemap("MoveRight", "INPUTPORT_DEVICE_KEYBOARD", "KEY_D")
GOInRemap("MoveUp",    "INPUTPORT_DEVICE_KEYBOARD", "KEY_W")
GOInRemap("MoveDown",  "INPUTPORT_DEVICE_KEYBOARD", "KEY_S")
GOInRemap("Jump",      "INPUTPORT_DEVICE_KEYBOARD", "KEY_E")
GOInRemap("Crouch",    "INPUTPORT_DEVICE_KEYBOARD", "KEY_C")
GOInRemap("ToggleWalkRun", "INPUTPORT_DEVICE_KEYBOARD", "KEY_LEFT_SHIFT")
GOInRemap("Fire",      "INPUTPORT_DEVICE_MOUSE", "MOUSE_BUTTON_1")
...
```

The configuration carries a later `GOInMouSens`/`GOInMouInv` pair, and the last call
wins: the running game reports mouse sensitivity 0.3 with the invert flag set.

The natives that implement the chain:

| Address    | Name                 | Role |
| ---------- | -------------------- | ---- |
| 0x004037C0 | `GO_InRemap`         | adds one binding to the current profile |
| 0x00403870 | `GO_InMouseInvert`   | sets the inverted mouse-look flag for the profile |
| 0x004038E0 | `GO_InMouseSens`     | sets the mouse look sensitivity for the profile |
| 0x00490230 | `Keyboard_Update`    | latches the keyboard state |
| 0x00509CF0 | `Joypad_Update`      | latches the joystick state |
| 0x00507EC0 | `Input_Run`          | latches the device state into the actions |

`GO_InRemap` (0x004037C0) writes three values per binding into the current profile's
remap table: the action id, the device id and the key code with the top two bits
cleared (`Script_EvalArgInt(arg2, 2) & 0x3fffffff`). The profile block is indexed by
`g_bc2380`, the active configuration profile.

The retail look rate is 0.0035 radians per mouse count (`PlayerController::look_radians_per_count`).

## What the port does

`engine/src/retail/input/input_port.cpp` holds the port's input port:

- `InputPort::Remap` records a binding, `SetKeyDown`/`SetMouseButtonDown` report the
  raw device state and `Run` publishes the press edges, mirroring the retail native's
  state array and changed mask.
- `Host_ApplyRetailInputConfig` reads the retail `config.qvm` with the port's own QVM
  reader, applies every `GOInRemap`, `GOInMouSens` and `GOInMouInv` call, and maps the
  retail key names (`KEY_LEFT_SHIFT`) and device names (`INPUTPORT_DEVICE_MOUSE`) onto
  the host codes. Nothing about the key map is hardcoded in the port.

`engine/platform/windows/retail_model_scene.cpp` wires the port to the scene:

- The scene window procedure turns `WM_KEYDOWN`/`WM_KEYUP`/`WM_SYSKEY*` and
  `WM_MOUSEMOVE` into the input port calls, so the port receives the same messages a
  real device sends.
- `Scene::update_player` reads the action state, applies the retail look rate through
  `PlayerController::update`, and places the camera on the human's eye
  (`Human_CameraEyePosition`, 0x00465820) looking along the human's yaw and pitch.
- The collision grid is rebuilt from the terrain mesh the frame draws, so the human
  stands on the surface the player can see.

## Ground locomotion

The retail ground displacement is the animation root-motion at `human+0x520`, which
`Human_RunUpdate` (0x004109A0) scales by `g_human_gen_1` (1.75) and feeds to
`Human_MovementIntegrator` (0x004614B0) alongside the velocity. The port does not play
the human animation yet, so it stands in with the per-state factors the retail
configuration carries: `humanplayer.qvm` `DefineHumanPlayerGeneral` repeats
`(1, 0.5, 0.75)` per movement state, and the 17.5 km/h base of the same file is the
factor 1.0.

| State | Retail factor | Port displacement |
| ----- | ------------- | ----------------- |
| walk  | 0.5           | 331.85 units/tick (8.75 km/h) |
| run   | 0.75          | 497.78 units/tick (13.125 km/h) |

The retail starts in run and `ToggleWalkRun` (LSHIFT) switches to walk. This stand-in is
provisional: the retail value comes from the animation, and the port will read it from
the clip once the animation pipeline lands.

## Runtime evidence

`open-igi.exe window level4 inputdemo` posts the retail key sequence to the scene
window from a per-frame timeline, so the chain runs without a physical keyboard. The
scene log reports the human state once a second:

```
HumanPlayer feet=112858424.000/-54600292.000/175465520.000 eye=0.000/0.000/0.690 yaw=0.000 pitch=0.000 on_ground=1 ...
HumanPlayer feet=112862028.934/-54568638.841/175461717.333 eye=0.880/7.728/-0.578 yaw=0.113 pitch=0.038 on_ground=1 ...
```

Observed in one run:

- W moved the human 4.15 m along +Y while it stayed on the ground (`on_ground=1`).
- The mouse moved yaw to 0.113 rad and pitch to 0.038 rad.
- E put the human airborne (`on_ground=0`) and the terrain query brought it back down.
- C toggled the crouch flag.
- The camera eye followed the human from 0.000 m to 8.010 m.

`scripts/frida_drive_openigi_input.js` does the same from outside the process through
`PostMessageW`, for the retail and the candidate side by side.

## What is still missing

- The human animation and the root-motion track, so the ground speed is the stand-in
  above rather than the retail value.
- The retail animation event records and the weapon, sound and AI subsystems.
- The retail `InputPort` native itself is not transcribed yet: the port implements the
  observable behaviour of the chain, and `0x004037C0`, `0x00403870`, `0x004038E0` and
  `0x00507EC0` remain at `STRUCTURALLY_VERIFIED` in the gate.
