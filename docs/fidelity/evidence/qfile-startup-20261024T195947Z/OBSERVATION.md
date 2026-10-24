# Retail startup/level-load native observation

Captured 2026-10-24T19:59:47Z (UTC) by `tester`; retail-only. This is evidence about **retail `igi.exe`**, not the candidate, and not a paired differential.

## Verdict

The Frida spawn-suspended probe reached all five hooked natives during startup + level 1 load. Three are selected below as the primary observations; `0x004B13A0` and `0x004B0E00` are supplemental because the same probe also caught them. The probe establishes call counts, the cdecl first argument, the one return value, and raw before/after table states. It does **not** establish candidate parity, and the candidate cannot currently reach these addresses because it fails closed at `QTask_RegisterClasses 0x00402930`.

## Identity and run conditions

| Item | Value |
|---|---|
| Retail executable | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| Retail SHA-256 | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| Retail size | 1,384,448 bytes |
| Probe | `scripts/tester/frida_qfile_startup_probe.js` |
| Probe SHA-256 | `63c9c669033c5270f26dd45475e3a9ddbce35991c4e9f04f0488be2dac4d7bc3` |
| Frida client/server | 17.24.0 / 17.24.0 Windows x86 (`igi-frida-server-17.24.0-windows-x86.exe`) |
| Arguments / cwd | `window level1`; requested and observed `Z:\Users\haseeb-mir\Downloads\IGI` |
| Module | `igi.exe`, base `0x400000`, size 8,605,696 bytes |
| Level script | `MISSIONS/location0/LEVEL1/OBJECTS.QVM`, SHA-256 `90acf17a8af76ddc24d40f061e796fc5964feed9298f321cb02b160ceaa89971` |
| Duration | 30 s after resume, stationary; no movement input |
| Hooks before resume | 5/5 installed, 0 failures; cwd and image base checked before `resume` |
| Process hygiene | no game before spawn; owned PID 1068 killed after capture; no `igi.exe`/`igi1.exe`/`open-igi.exe` remained |
| Candidate | never attached or run; no candidate evidence claimed |

The five hooks were installed while the process was suspended. `game_processes_before` and `game_processes_after` are both empty in `capture.json`. The shared Frida server was already running and was left alone; it is not a game process.

## Selected native 1 — `0x004B1620`

Placeholder name `QFile_LookupDeviceByName_4B1620`; cdecl, first argument at `[esp+4]`, return in EAX.

- **Reached:** yes. **351 calls**.
- **First caller:** `0x4B151E` (the call at `0x4B1519`).
- **Arguments:** every one of the 351 calls passed pointer `0x53B604`, which reads as the C string **`r`**.
- **Return:** **1** on all 351 calls.
- **Table `0x00546F04` (read before first call, after first call, and at 30 s):** four valid pointers, then null at index 4:
  - index 0 `0x53B604` → `r`
  - index 1 `0x546F18` → `r+`
  - index 2 `0x538354` → `w`
  - index 3 `0x543A60` → `w+`
  - index 4 → null
- **Table `0x00534A70`:** `[1, 3, 2, 3]` for those four entries; unchanged before/after.
- **Mutation by this native:** none observed.
- **Fact:** both tables are populated in retail, not zeroed. The strings are stream modes, not device names; the placeholder label/semantics need review. The candidate files declare 16 entries, but retail reads four valid entries followed by null in this capture.

## Selected native 2 — `0x004B1A90`

Placeholder name `QFile_FindRecordAtAddress_4B1A90`; cdecl, one 32-bit address argument at `[esp+4]`, no return value.

- **Reached:** yes. **135 calls**.
- **First caller:** `0x4B1A3C` (the call at `0x4B1A37`).
- **Arguments:** 32-bit addresses; first `0x40C003C` (67895356 decimal), last `0x9554CEC` (156585196 decimal).
- **Match result:** all 135 calls matched a record's `+0x18` word within the 512-node read bound; no unmatched call and no `Debug_Error` path observed.
- **Head at `0x00943970`:** non-null. First call: `0x3E9C61C`, with two records:
  - record 0: address `0x3E9C61C`, next `0x943974`, `+0x18` = `0x40C003C` (the first argument)
  - record 1: address `0x943974`, next null, `+0x18` = 0
- **List growth:** first call length 2; observed call-time lengths 2–41; final state at 30 s has **69 records**, head `0x3E9CCAC`, last record at `0x943974` with next null and `+0x18` = 0.
- **Mutation by this native:** none observed; the head/list are populated and grow during startup/load through other code.
- **Fact:** `0x00943970` is a real non-null head and the records are real; the first call's argument is present at a record `+0x18`. The final node at `0x943974` is near the head global, so its role (sentinel/hidden node) is not established by this probe.

## Selected native 3 — `0x004B0E70`

Placeholder name `Sys_AddSlot_4B0E70`; cdecl, one name pointer at `[esp+4]`, no return value.

- **Reached:** yes. **64 calls**.
- **First caller:** `0x49172E` (the call at `0x491729`).
- **First argument / table transition:**
  - first call argument `0x542430` → `DisplayOpen`
  - table `0x00A94A80` before: all 128 slots have null name and flag `+0x04` = 0
  - after the first call: slot 0 name `DisplayOpen`, flag `+0x04` = 1; all other slots still free
- **Last call argument:** `AfterburnerPrimitive`, placed in slot 63.
- **Final state at 30 s:** **64 used slots** (0–63), all with name pointers and flag 1; slots 64–127 still free.
- **Mutation by this native:** yes; it fills the first free slot and sets the flag, exactly the table effect the candidate describes.
- **Fact:** the 128-entry table is real and starts zeroed; 64 registrations occur in this startup/load window.

## Supplemental — `0x004B13A0` (clear-by-name)

7 calls in the startup window; no match was observed, and the table was zero at the first call but populated by the 30 s snapshot:

- first caller `0x4B1314`; arguments were `INTROSCREEN:`, `STATUSSCREEN:`, `COMPUTER:`, `MENUSYS:`, `LANGUAGE:` (twice), `MISSION:`.
- **All 7 calls: no record matched** (`matchIndex = -1`); first-call before/after table `0x00942330` was entirely zero (names empty, `+0x90` = 0, parallel `0x009423C0` = 0).
- Final state at 30 s: six records are populated — `INTROSCREEN:`, `STATUSSCREEN:`, `COMPUTER:`, `MENUSYS:`, `LANGUAGE:`, `MISSION:` — each with `+0x90` = 1 and parallel word = 1.
- **Fact:** the table is real, but no clear call matched. The first call's before/after state was zero; the six records appear by the 30 s snapshot. The population ordering relative to the calls is unresolved: the capture proves no match at the calls, not that every call preceded population. `0x00942330` is not "starts zeroed for the whole run"; it is zero at the first call and populated later.

## Supplemental — `0x004B0E00` (RenderMode registrar)

2 calls, first caller `0x49358B`:

- first argument `Direct3D`, second `OffscreenBuffer`.
- table `0x00A950A0` before first call: all eight slots null/flag 0; after first call slot 0 `Direct3D`/flag 1; final state has slots 0–1 used, flag 1.
- **Fact:** the table starts zeroed and receives two render-mode registrations.

## Which limitations this resolves, and which remain

**Resolved (for this 30 s startup + level-1 window):**

- The "runtime UNOBSERVED" limitation for these five addresses is resolved: all five are reached, with exact call counts and first arguments.
- `0x00546F04` and `0x00534A70` are not empty; retail holds four real C strings and four paired words.
- `0x00943970` is a non-null head; loaded-file records exist, and the walker's first argument matched a real `+0x18` value; all 135 calls matched under the 512-node bound.
- `0x00942330` is a real record table populated by 30 s, with the six names above and `+0x90`/parallel = 1; the first clear call saw it zero, all seven clear calls report no match, and no clear side effect was observed.
- `0x00A94A80` is a real 128-slot table that starts zeroed and receives 64 startup registrations; `0x00A950A0` starts zeroed and receives 2.
- The candidate's "these tables start zeroed" limitation is therefore a **candidate/static** limitation, not a retail fact; retail populates them in this path.

**Remain unproven:**

- Meaning of the `0x00534A70` values; why `0x00546F04` holds stream modes rather than device names; the placeholder name/semantics.
- The actual retail table extent (four valid entries plus null observed; source declares 16) and whether it changes later.
- Full layout/meaning of the linked-file records; the role of the `0x943974` node; list lifecycle, teardown, and whether all later calls outside this 30 s window match.
- What populates `0x00942330` after the seven clear calls, and why clear runs first; whether the parallel word is ever cleared in another path.
- Record fields beyond name/flag/`+0x18`; complete 32-record / 8-render-mode / 128-slot capacities over the full game.
- Any other level, menu, map, or teardown path; no movement input, no multiple levels.
- Candidate parity or a runtime contract: the candidate cannot reach these addresses, and no candidate was run. This is retail observation only and does not promote any native status.

## Evidence

Raw evidence directory: `docs/fidelity/evidence/qfile-startup-20261024T195947Z/`

- `capture.json` — identity, cwd/base proof, 5/5 hooks, process cleanup, full pre-resume and final snapshots, per-call records.
- `frida.jsonl` — 567 raw Frida messages, including every call's argument, return, match result, and first-call before/after tables.
- `probe.js` — exact probe copy; SHA-256 `63c9c669033c5270f26dd45475e3a9ddbce35991c4e9f04f0488be2dac4d7bc3`.
- Probe scripts: `scripts/tester/frida_qfile_startup_probe.js`, `scripts/tester/capture_qfile_startup.py`.

This report is intentionally separate from native verification: no engine, manifest, CMake, or status change was made.
