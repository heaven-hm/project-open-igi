# Retail alias-table observation — 0x004B12F0, 0x004B11A0, 0x004B1020, 0x00943A18

Captured: 2026-10-25T20:24:04Z (UTC), retail `igi.exe` only, spawned suspended,
cwd and image base verified before resume, all three hooks installed before
resume, owned PID killed after capture, no game process before or after.

## Identity and hygiene

- Retail `igi.exe` SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- Probe SHA-256: `b0dba3edfd8ae5f2822e311266679edc93ddfc1aba0d8da206733c74413fa9f9`
- Frida 17.24.0, remote Windows x86 server on `127.0.0.1:27042`
- Observed cwd: `Z:\Users\haseeb-mir\Downloads\IGI`
- Image base: `0x400000`; image size: `8605696`
- Hooks: 3/3 installed; 0 failures: `0x004B11A0`, `0x004B12F0`, `0x004B1020`
- Arguments: `window level1`; capture window 30 s; game left stationary
- `game_processes_before`: `[]`; `game_processes_after`: `[]`
- Raw: `capture.json`, `frida.jsonl`, `probe.js` in this directory.

This is retail-only observation. It is not candidate parity, not a status
promotion, and it does not settle untested branches.

## 1. 0x004B12F0 registrar calls (seen)

`0x004B12F0` was reached exactly 6 times during startup + level-1 load. In every
call the **first argument is the alias key** and the **second argument is the
device-prefixed target path** (cdecl order, read from `[esp+4]`/`[esp+8]` and
bounded to 0x100 bytes with a NUL stop):

| call | argument 1 (key) | argument 2 (target) | return EAX | caller |
|---:|---|---|---:|---|
| 0 | `INTROSCREEN:` | `LOCAL:screens/intro/` | `0x15` | `0x4150cf` |
| 1 | `STATUSSCREEN:` | `LOCAL:screens/game/status/` | `0x1b` | `0x486994` |
| 2 | `COMPUTER:` | `LOCAL:computer/` | `0x10` | `0x46abda` |
| 3 | `MENUSYS:` | `LOCAL:menusystem\` | `0x12` | `0x4170b9` |
| 4 | `LANGUAGE:` | `LOCAL:language/ENGLISH/` | `0x18` | `0x405b8f` |
| 5 | `MISSION:` | `LOCAL:missions/location0/level1/` | `0x21` | `0x415b94` |

One full target string, exactly as copied into the record:

```
LOCAL:missions/location0/level1/
```

The second argument is therefore device-prefixed (`LOCAL:` in all six calls),
not a bare relative path. The return EAX values are recorded as raw leftovers;
no semantics are inferred from them here.

## 2. Table 0x00942330 before and after the calls (seen)

The initial table was entirely zero (32 records, zero occupied). The records
fill in order 0..5. Each record is 0x94 bytes. The observed record contents are:

| index | pointer | +0x00 key | +0x10 target | +0x90 word |
|---:|---|---|---|---:|
| 0 | `0x942330` | `INTROSCREEN:` | `LOCAL:screens/intro/` | 1 |
| 1 | `0x9423c4` | `STATUSSCREEN:` | `LOCAL:screens/game/status/` | 1 |
| 2 | `0x942458` | `COMPUTER:` | `LOCAL:computer/` | 1 |
| 3 | `0x9424ec` | `MENUSYS:` | `LOCAL:menusystem\` | 1 |
| 4 | `0x942580` | `LANGUAGE:` | `LOCAL:language/ENGLISH/` | 1 |
| 5 | `0x942614` | `MISSION:` | `LOCAL:missions/location0/level1/` | 1 |

Table state before each call: 0, 1, 2, 3, 4, 5 occupied respectively. After each
call, the newly filled index is recorded in `capture.json`
(`populate-entry.tableBefore`, `populate-exit.tableAfter`,
`populate-exit.filledIndices`).

The `0x004B11A0` hook observed its return for the argument passed by
`0x004B12F0`: **return 1** for all six targets. The matching record's `+0x90`
word is **1** in all six calls, and `handleMatchesDevice` is true for all six.
So in this capture the `+0x90` word is exactly the `0x004B11A0` device index
(1, the first device), not a separate boolean. That is a *seen* fact for this
run; whether another device index can appear was not exercised.

The full 16-byte key and 32-byte target bytes are in the snapshots; e.g. index 5
key bytes are `4d495353494f4e3a00000000000000` and target bytes begin
`4c4f43414c3a6d697373696f6e732f6c6f636174696f6e302f6c6576656c312f`.

## 3. Reader 0x004B1020 contract observed (seen, partial)

`0x004B1020` was called at least 4096 times; the probe's reader-event cap was
reached, so the total above 4096 is **unknown**. The first 24 calls were:

- Arguments (cdecl): `destination` at `[esp+4]`, `alias` at `[esp+8]`, and
  `depth` at `[esp+0xc]`.
- All first 24 calls had `depth = 0`.
- Destinations were stack buffers (e.g. `0x22f994`, `0x22f4b8`) or the fixed
  buffer `0x9435b0`; both produced the same string.
- Every first-24 output was byte-identical to the alias argument; return EAX
  was `0` on all of them.
- Examples:

| call | destination | alias | depth | output | return |
|---:|---|---|---:|---|---:|
| 0 | `0x22f994` | `LOCAL:screens/intro/` | 0 | `LOCAL:screens/intro/` | 0 |
| 1 | `0x22f4b8` | `LOCAL:screens/intro/EIDOS.WAV` | 0 | `LOCAL:screens/intro/EIDOS.WAV` | 0 |
| 2 | `0x9435b0` | `LOCAL:screens/intro/EIDOS.WAV` | 0 | `LOCAL:screens/intro/EIDOS.WAV` | 0 |
| 21 | `0x22f99c` | `LOCAL:missions` | 0 | `LOCAL:missions` | 0 |
| 23 | `0x22ef2c` | `LOCAL:missions/location0/COMMON` | 0 | `LOCAL:missions/location0/COMMON` | 0 |

**No reader call matched a table key.** The table's `+0x00` values are alias keys
such as `INTROSCREEN:` and `MISSION:`; the observed reader inputs are
device-prefixed paths starting `LOCAL:`. `matchedRecord` was null in all 4096
observed reader calls. Consequently:

- The "resolve then upper-case" behaviour assumed by the old file was **not
  observed** here. The no-match branch copies the alias unchanged, preserving
  case, and returns 0.
- The table-match branch (copy target + remainder, depth handling, any case
  transformation) was **not exercised** in this window. It remains unknown from
  runtime evidence.
- The 4096-call reader cap was reached; calls after the cap are unobserved.

This is consistent with the table being populated with alias keys while the
level-1 startup calls the reader with already-resolved device paths, but the
match path itself is not proven either way.

## 4. Fixed buffer 0x00943A18 (seen; boundary inferred 0x80)

`0x00943A18` was read after every reader call up to the cap. It holds the last
destination string produced by `0x004B1020`, not the alias key:

- Final buffer value after the capture window:
  `LOCAL:missions/location0/level1/sounds/m1_heliblad02.wav` (56 bytes).
- Last fully captured reader call (stored call 95) had alias
  `LOCAL:missions/location0/LEVEL13/MISSION.qvm` (44 bytes), output identical.
  The final 56-byte value came from a later, unrecorded reader call; because the
  reader cap was reached, the alias for that final value is not known.
- Maximum observed length: 77 bytes, at stored call 35:
  `LOCAL:missions/location0/LEVEL1/lightmaps/missions/location0/level1/lightmaps`.
- Every observed reader call was a no-match, so the buffer held the alias
  unchanged in all observed cases.
- The 0x100-byte read limit was never hit (`hitReadLimit` false).
- It did not overflow or truncate in this capture.

The static extent was narrowed by the retail reference at `0x004B1AC0`:

- `0x004B1AC0` calls `0x004B1020` with destination `0x00943A98`, then reads and
  scans `0x00943A98` (including `0x00943A99 + edx`). That proves `0x00943A98`
  is a separate string object immediately following the reader's fixed buffer.
- No code or data reference to any address between `0x00943A18` and `0x00943A98`
  was found in the image scan.
- The difference is `0x80` bytes. `0x004B1200` clears the first byte at
  `0x00943A18`, consistent with clearing a fixed buffer, but does not itself
  encode the extent.

Therefore the fixed buffer's capacity is **inferred as `0x80` bytes**
(`0x00943A18..0x00943A97`, next object at `0x00943A98`), not the old candidate's
`char[0x100]`. The observed maximum 77 and final 56 are both below 0x80; there
were 51 bytes of headroom at the observed maximum. The retail copy is still
unbounded in code, so a longer path could overflow; no bound check was observed.
The precise extent remains an inference from the next object rather than a
retail size symbol.

## Tags

- **seen**: 6 registrar calls, argument order, all six key/target strings,
  fill order 0..5, `+0x00`/`+0x10`/`+0x90` values, device return 1 matching
  `+0x90`, first 24 reader calls and outputs, no reader match in 4096 calls,
  fixed-buffer final/max lengths, process/cwd/base/hash hygiene.
- **inferred**: the `+0x90` word is the device index for these calls (the values
  coincide exactly and all six match; other devices were not exercised); the
  fixed buffer capacity is `0x80` bytes because the next referenced object is
  `0x00943A98` and no object is referenced in between. That is a boundary
  inference, not a retail size symbol.
- **unknown**: reader table-match branch, depth > 0 behaviour, case handling on
  a match, total reader calls above 4096, any call after the 30 s window, and
  whether a longer path is ever passed to `0x004B1020` at runtime.

No status promotion was made.
