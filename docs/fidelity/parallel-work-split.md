# Parallel work split: this branch vs `codex-smart-workflow`

Two branches are reconstructing the same retail binary with different pipelines. This file records
what each side owns so the same native is not verified twice, and what can be pulled as-is.

## State at the time of writing

| | this branch (`deepseek-smart-workflow`) | `codex-smart-workflow` |
| --- | --- | --- |
| registered records | 883 | 883 |
| status distribution | 91 VERIFIED, rest lower | 795 STRUCTURALLY_VERIFIED, 2 BUILD_VERIFIED, 1 RUNTIME_TESTED, 84 ANALYZED, 1 INCONCLUSIVE |
| runtime evidence | paired Frida capture, differential runners, strict gate | structural only; no paired capture |
| MVP natives verified | 91 of 120 | none of the same 120 at VERIFIED |

The two pipelines are complementary rather than redundant: this branch produces *evidence*
(executed differentials, live Angr CFG, captured probes), the parallel branch produces *transcriptions*.
Neither branch has verified a native the other one needs - there is no verification duplication.

## Pull list (generated)

`scripts/codex_sync_report.py` recomputes this from both manifests; see `docs/fidelity/codex-sync.md`.

| action | count | meaning |
| --- | --- | --- |
| evidence-only | 16 | implementation file is byte-identical on both branches; only runtime evidence remains here |
| adopt-file | 3 | file is new here, or holds no native verified here; safe to adopt, then re-gate |
| hunk-merge | 10 | file also holds verified natives here; only the one function body may be pulled |

The evidence binding is what makes whole-file adoption unsafe: every record here binds to the
implementation file's SHA-256, so swapping a file that also implements a verified native silently
invalidates that record. `scripts/codex_sync_report.py` reports exactly that overlap per row.

## Ownership: what the parallel branch is actively changing

Determined from its working tree (files being edited right now) and its recent commits:

| lane | natives | evidence of activity |
| --- | --- | --- |
| terrain transcription (quadtree, draw, loaders, storage, mipmaps, scene state) | `0x004D14E0`, `0x00510010`, `0x00512C30`, `0x004C5F00`, `0x004C6240` | commits `efc3b24b`, `f87da55a`, `419495df`; `terrain_*.cpp` 1000-2100 lines |
| terrain renderch cluster | `0x00517890`, `0x005178E0`, `0x00517940`, `0x005179C0` | uncommitted evidence edits right now |
| QTask creation and class registration | `0x00401000`, `0x00402930` | commit `5f2a1ba7`; new `qtask_create_task.cpp` |
| QVM error path | `0x004B7E10` | commit `5f2a1ba7` |
| startup accessors and WinMain | `0x0048F0D0`, `0x0048F260`, `0x0048F530` | uncommitted evidence edits; commit `3d2cd36c` |
| AnimIff loading | `0x004D67A0` | uncommitted evidence edits |
| humanplayer defines link | `0x00405850`, `0x00410070`, `0x004137A0` | uncommitted `humanplayer-defines-link-blockers.json` |

**Do not work these lanes here.** Pull them with the script when the parallel branch is idle.

## Owned here: natives the parallel branch is not touching

These are blocked on *observation*, not on transcription: the implementation files are already
byte-identical on both branches, and no branch has ever observed the retail side calling them.

| native | VA | why retail is silent |
| --- | --- | --- |
| `Config_GetActiveGraphicOptions` | `0x00404590` | config command table, not entered by a gameplay run |
| `Config_GraphicOptionsSetPerformanceFlags` | `0x00404F00` | registered as a pointer by `fcn.00402ff0`, not called |
| `Config_GraphicOptionsGetPerformanceFlags` | `0x00404F60` | same registration path |
| `Config_GraphicOptionsGetPerfLevelFromFlags` | `0x00405530` | same registration path |
| `Config_GraphicOptionsGetPerfFlagsFromLevel` | `0x004055C0` | same registration path |
| `Config_GraphicOptionsNumPerfLevels` | `0x00405620` | same registration path |
| `Human_TaskViewReset` | `0x004659E0` | task/map view only |
| `Camera_ResetMatrix` | `0x004A2600` | single caller, level/camera transition |
| `Matrix_InvalidateSlot` | `0x004B7450` | called from `0x00495060`, `0x0049B260`, `0x004A0110` |
| `Camera_UpdateFogPalette` | `0x004A26C0` | single caller, weather/fog transition |
| `Loop_SetStatusByte` | `0x0048F480` | caller `0x00401050` takes the branch only on a status change |
| `String_VFormat` | `0x004A5702` | reachable only through error formatting |
| `Error_Show` | `0x004AF7B0` | reachable only on an error path |
| `Mem_InvokeNewHandler` | `0x004AD722` | out-of-memory path |

### Retail VA validation (radare2 on the retail image)

All fourteen VAs are real function entries, so the blocker is scenario coverage and not a bad
mapping:

- `0x00404590` - `fcn.00404590`, 28 bytes, called from 18 sites inside `0x0040452E..0x00404F71`.
- `0x00404F00`/`0x00404F60`/`0x00405530`/`0x004055C0`/`0x00405620` - not discovered by `aaa`
  because they are only referenced as *data pointers*: `fcn.00402FF0` pushes each address at
  `0x004032FC`, `0x00403361`, `0x00403374`, `0x00403387`, `0x0040339A`. Disassembly at each VA is a
  real prologue (`sub esp, ...`), so these are config callbacks installed into a table, invoked
  only when the corresponding config command runs.
- `0x004659E0` - 47 bytes, eight callers in `0x0040EA18..0x0040F098` plus `0x00472F04`.
- `0x004A2600` - 188 bytes, one caller at `0x004A3C3E`.
- `0x004B7450` - 17 bytes, callers in `fcn.00495060`, `fcn.0049B260`, `fcn.004A0110`.
- `0x004A26C0` - 86 bytes, one caller at `0x004A3FA2`.
- `0x004A5702` - 81 bytes, callers include `fcn.004AF7B0` (`Error_Show`) and `fcn.004B7E10`.
- `0x004AF7B0` - 89 bytes, many callers, including `fcn.004B7E10` and `0x004137A0`.
- `0x0048F480` - 10 bytes, called from `0x00401089` (inside the verified `0x00401050`).

The rule this branch must keep applying: a native is only **PASSED** when the retail side has
actually been observed calling it, the Angr CFG is at least 0.85, and an executed differential
runner backs the behavior. A transcription, however good, is not a pass - so these fourteen stay
unverified until the retail scenario reaches them, which is the work owned here.

## Coordination rules

- One `igi.exe` at a time: captures take the lease in `scripts/frida_capture_lease.py`
  (lock file keyed by `FRIDA_HOST` under the temp directory) and refuse to start while another
  game is alive.
- The parallel branch pushes its own transcription commits; this branch pushes evidence commits.
  Pull with `python3 scripts/codex_sync_report.py --write` before starting a native, so a pull that
  already exists is adopted instead of re-transcribed.
- Re-run `docs/fidelity/mvp-verification-blockers.md` counts after any pull, because a pull can
  invalidate a record's file-SHA binding.

## Observation experiment: does a spawned retail run reach the startup natives?

A first hypothesis for the fourteen observation-blocked natives was that the canonical retail
capture attaches after startup, so natives called during startup would never be seen. The harness
supports that test directly (`frida_mvp_batch.py --spawn`, hooks installed before the entry point):

```
mvp-frida-pair-startupspawn.json:
  planned 115, retail_observed 0, candidate_observed 99, matched 0, inconclusive 115
  retail: "spawn observation timed out" (0 calls observed in the settle window)
```

The negative result rules out the simple explanation: retail under Frida spawn does not reach the
game loop at all in this harness (the candidate does, 99 natives observed). Retail therefore has to
be launched normally and attached to, which is what the canonical capture does - and that
necessarily misses startup. Reaching the fourteen natives needs a *scenario* that invokes them
while the game runs (a config command, a task-view open, a pause/status change, an error path), not
a different attach point.

Next experiment: drive the task-view and pause paths in the running image and re-capture, because
`Human_TaskViewReset`, `Loop_SetStatusByte` and the camera trio sit on those paths. This is held
here rather than in the parallel branch because it is observation work - the parallel branch owns
the transcription lanes listed above.
