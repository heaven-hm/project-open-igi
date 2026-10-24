# Changelog

## Release policy

A version is published **only** when the built `igi1.exe` starts, keeps running,
and a screenshot of the running game is captured and attached to that version.
A build that fails closed at a documented boundary (for example the registrar
chain throw at `QTask_RegisterClasses 0x402930`) is an **internal build**: it is
recorded under "Unreleased" with its evidence, and it never gets a version
number and never counts as a release.

## Unreleased


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
