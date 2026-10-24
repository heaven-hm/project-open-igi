# Changelog

## Release policy

A version is published **only** when the built `igi1.exe` starts, keeps running,
and a screenshot of the running game is captured and attached to that version.
A build that fails closed at a documented boundary (for example the registrar
chain throw at `QTask_RegisterClasses 0x402930`) is an **internal build**: it is
recorded under "Unreleased" with its evidence, and it never gets a version
number and never counts as a release.

## Unreleased

### 2026-10-25 - T0/T1 start: flow order tool, dead-code and port-constant fixes (internal build, no release)

- **Task state**: T1 (3D objects + terrain visible) is **not** started because T0 blocks it: the candidate still fails closed at
  `QTask_RegisterClasses (0x00402930)`, which is the retail startup init sequence, not a registrar. Nothing in this entry is a release.
- **Added**: `scripts/flow_order.py` and `docs/fidelity/evidence/chronological-native-order.md` - the retail call flow from the PE entry
  point in call order: 3022 functions, 678 implemented, 2344 missing. This is the work order for not missing any method in the flow.
- **Added**: `docs/fidelity/MVP-task-breakdown.md` - the owner's small stacked tasks T0-T7 (startup chain, 3D objects, terrain,
  human player, weapons, AI, sound, menu) with the static + runtime + screenshot + changelog acceptance rule for each.
- **Fixed**: `flow.cpp` per-frame dispatch called a hardcoded `phase = 1U`; retail `0x00402462` calls `0x004E7530`, which is already
  transcribed as `Core_GetEventWord_4E7530`, so the dispatch now calls it. One port-only constant removed from the dispatch path.
- **Fixed**: removed the unused `link_handler` helper from `task_types.cpp` (dead code, rule 16); no native called it.
- **Launch attempt**: the candidate was copied to the game directory and started through the CrossOver wine launcher; no running game
  window and no 3D output were verified, and the capture `screenshots/internal/2026-10-25-igi1-t0-launch-attempt.png` is recorded as a
  launch attempt only, not as game evidence. No screenshot claim is made for T1.


### 2026-10-24 — startup-prefix internal build (no release)
- **State**: builds and starts, then terminates by design at `QTask_RegisterClasses`
  (`0x00402930`): the AI-graph registrar is not transcribed, so class ids after it
  would not match retail. Not runnable as a game, therefore **not a release**.
- **Achieved internally**: `WinMain` at retail `0x0048F530` as the only entry;
  startup prefix reproduced (mutex/duplicate path, `CoInitialize`, WNDCLASSA
  `"Innerloop"` style 3, 16 startup setters in retail order, four parser
  registrations, `CreateWindowExA` style `0x37C80000`, 640x480 at X=520/Y=285,
  `ShowCursor(0)`, `ShowWindow(10)`, `StoreWindowHandle`); B3 WinProc
  handler-chain container init (`0x48F674-0x48F6A6`) with the retail 12-byte
  `{next,+prev_link,+handler}` entry layout.
- **How**: matching decompilation of retail `dea34892…` with Binary Ninja (single
  decompiler), Radare2 CFG, angr exact entry; paired Frida prefix probes
  (retail `PREFIX_RETAIL_MATCH 178/179`; the chain side effect was the only real
  divergence and B3 now implements it).
- **Internal candidate**: `igi1.exe` sha256 `30c39039…` (deployed by
  `build_no_verify.sh`).
- **Evidence screenshot (internal, not a release)**: `screenshots/internal/2026-10-24-igi1-startup-prefix.png`.
- **Blocking next step**: transcribe the registrar chain starting at
  `0x00508F80` so `QTask_CreateTask` returns and the window stays alive.


### Planned

- Follow the Level 1 playable-demo sequence in MVP.md and plan.md: Windows x86 build, OBJECTS.QVM model placement, textured terrain, complete visual verification, then playable simulation and gameplay systems including sound.

### Current implementation state

- The Windows x86 target now enters at the retail WinMain (`0x0048F530`) and reproduces the retail startup prefix; it still terminates at the documented registrar boundary (`QTask_RegisterClasses 0x402930`) and is therefore not a playable build.
- Resource, MTP, MEF container, and LOOP texture parsing have focused tests.
- Level 1 QVM object execution, 3D/terrain rendering, player and AI simulation, combat integration, and sound playback are not yet connected into a playable mission.
