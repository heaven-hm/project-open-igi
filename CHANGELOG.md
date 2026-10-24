# Changelog

## Release policy

A version is published **only** when the built `igi1.exe` starts, keeps running,
and a screenshot of the running game is captured and attached to that version.
A build that fails closed at a documented boundary (for example the registrar
chain throw at `QTask_RegisterClasses 0x402930`) is an **internal build**: it is
recorded under "Unreleased" with its evidence, and it never gets a version
number and never counts as a release.

## Unreleased

### 2026-10-25 - reverse-engineering skills applied: retail registrar names mined (internal build, no release)

- **Selected skills** (recorded in `docs/agents/re-skills-applied.md`): `igi-auto-re-agent` for registrar push-pair mining,
  `igi-validation` for the dual-engine Gate/Ghidra+radare2 rule, `reverse-engineering` as the technique catalogue. Checked and
  recorded: radare2 present; **Ghidra headless MCP on `127.0.0.1:8771` is not answering**, so no dual-engine score can be claimed yet;
  `igi-validation`'s documented image path does not exist on this machine (this repo's authority image is the Downloads copy).
- **Added**: `scripts/registrar_pairs.py` - for every `call` into the four retail registrars (`0x4B8890`, `0x4BAAC0`, `0x4B88C0`,
  `0x4B88F0`) it back-decodes the preceding pushes (cdecl: the name string is pushed last, so the push nearest the call is the name),
  reads the C string from the image and cross-references the function with the manifest.
- **Result**: **167 call sites, 166 unique registered natives, 88 already implemented here, 78 missing** (19 call sites undecoded).
  Table: `docs/fidelity/evidence/registrar-pairs.md`. This is the only source of authoritative retail names for these routines:
  `Flow_RequestEvent` (`0x00402780`), `GOStart` (`0x00403660`), `Game_SetMission` (`0x00415890`), the `MenuManager_*` family,
  `MovieSubtitle_RegisterFont/RegisterText`, `Game_SetMusicVolume/SetSFXVolume`, and 70 more.
- **Structural finding**: none of the missing named natives is a direct callee of `0x00402930` and none appears in the call-order walk -
  they are script natives invoked indirectly by the QVM through the registry, so static call-graph work can never reach them; they have to
  be landed from the script/mission requirements. This is a gap the call-order list alone cannot show.

### 2026-10-25 - corrected the two QTask link natives from retail operands (internal build, no release)

- **Corrected (fidelity defect, found by `chain-worker`, fixed by Lead)**: `QTask_LinkQTaskEventHandler` (`0x00401400`) had its two
  argument roles swapped. Retail `0x00401408` loads the SECOND argument into `ebp` and takes the phase from its low byte, while
  `0x00401429` loads the FIRST argument and masks it to get the index base; `0x0040145F` compares the type record's first word against
  the FIRST argument and `0x00401474`-`0x0040147A` recurse with the SECOND. The tree body used the first argument for phase/recursion and
  the second for the comparison, which with the registrar call shape `(class id, event id, handler)` made every phase link collide on
  `class + (class & 0xff) * 0x180`. Both bodies now match retail; `0x00401530` also had its record comparison against the wrong argument,
  and retail does not low-byte-mask its stride argument - both corrected.
- **Re-measured**: `0x00401400` and `0x00401530` both rebuild green and score **CFG 1.000** (7 blocks / 10 edges each) against retail.
- **Why it matters**: the class registrars pushed by the startup sequence call these two natives; the wrong operand made the handler
  table land in the wrong phase rows, so the startup-chain transcription could not be validated against retail registration state.
- **Process**: `docs/agents/subagent-roles.md` records the owner's subagent split (Lead integrator, implementer, verifier/tester, research)
  with the mandatory "no self-verification" rule and the parallel-vs-sequential decision rule.

### 2026-10-25 - window-only scene capture: launch, load, Escape, capture (internal build, no release)

- **Fixed**: the earlier captures were full-screen grabs of the desktop (they showed the editor, not the game). New
  `scripts/tester/capture_game_window.py` enumerates windows by `CGWindowID`, raises the game window, captures it with
  `screencapture -l <window id>` only, and reports pid/window id/bounds/image size/scale so the image is provably that window
  (`matches_window: true`), with a region fallback when a window has no image backing.
- **Added**: `scripts/tester/game_scene_capture.py` - the owner's scene-check sequence in one command: launch
  `<exe> window level<N>` from the game directory through the CrossOver wine launcher, poll until the game window exists, wait the
  load time, bring it to the front, send Escape to skip the intro/cutscene, capture only the window, then terminate the process.
- **Verified (retail)**: `igi.exe window level1`, Escape sent (`CGEventPostToPid` + `osascript`), window `igi.exe` title `IGI`
  id `11715` pid `41085` bounds `1024x668` at `328,177`, captured `2048x1336` (scale 2, `matches_window: true`) ->
  `screenshots/pair-2026-10-25/retail-level1-scene.png`.
- **Candidate**: `igi1.exe window level1` through the same sequence produced **no window**: it still fails closed at
  `0x00402930` before a capturable game window exists. No candidate scene screenshot is claimed; T1 remains blocked on T0.

### 2026-10-25 - diagnostic logging + paired same-level capture (internal build, no release)

- **Added**: `engine/include/igi/diagnostic_log.hpp` - diagnostic-only logging infrastructure (owner rule 3(a)): never changes control
  flow, values or ordering, writes `$TMPDIR/igi1-diagnostic.log`, one line per call, flushed. First call site: the fail-closed guard in
  `class_registration.cpp`, which now logs `fail-closed: startup init sequence not transcribed, stopping at AI graph` before throwing.
  Verified live: the candidate ran under CrossOver wine and the log line was produced at
  `[0x00402930] fail-closed: startup init sequence not transcribed, stopping at AI graph`.
- **Captured**: same-level runs of retail `igi.exe` and candidate `igi1.exe` with `window level1`, screenshots by the macOS
  `screencapture` tool: `screenshots/pair-2026-10-25/retail-level1.png` and `screenshots/pair-2026-10-25/candidate-level1.png`.
- **Result**: retail ran (process alive at capture); the candidate reached the guard and terminated, so the candidate capture is not a
  level render. No release; T1 still blocked on T0.

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
