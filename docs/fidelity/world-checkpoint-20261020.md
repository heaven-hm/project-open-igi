# Object verification checkpoint, 2026-10-20

Task 4 (every object's placement/material/texture) is **incomplete**. Task 5
(player/task loop/physics/interactions) is not accepted by this checkpoint.
The new verification architecture does not supply missing game methods.

## Reproduce the diagnostic

Run the existing Frida server in the `open-igi` CrossOver bottle on
`127.0.0.1:27042`, then from the C++ repository use (pass `--host` if testing an independently owned server):

```sh
"/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python" \
  scripts/capture_world_checkpoint.py --level 1 --seconds 25 \
  --candidate build-win32-no-verify/engine/platform/windows/open-igi.exe
```

The driver starts retail suspended, verifies its actual working directory is
`Z:\Users\haseeb-mir\Downloads\IGI`, installs read-only native hooks, resumes,
captures, closes its owned process and confirms termination before starting
the candidate. Both receive `window level1`. It refuses existing game sessions.
Levels 1..14 are selectable. The candidate defaults to the existing diagnostic
`build-win32/engine/platform/windows/open-igi.exe`; staging it does not authorize
a release or claim it contains uncompiled workspace changes.

The driver follows the user's cutscene instruction after observing the retail
input loop: find the `IGI` window owned by the captured PID, bring it to the
foreground, send Escape down/up, and await release. Keyboard_Update and
LevelFlow_GetBreakCutSceneKey observations record consumption separately from
successful OS delivery. Cleanup releases a pending key before terminating the
owned process. This is test input, not a substitute game pause/cutscene method.

`scripts/frida_world_checkpoint_probe.js` is frozen once and supplied to both
processes; raw source, hashes, launch parameters, script errors, native state,
opaque position records and candidate scene-log additions are retained under
`build/runtime-traces/world-level*/`. Frida's `context` invocation property is
reserved: this probe uses `nativeContext`. Callback errors, failed reads and
overflow must not count as a successful capture.

## Observed captures

| Capture folder under build/runtime-traces | Retail observations | Candidate result |
| --- | --- | --- |
| world-level1-20261020T012015Z | 656 opaque world-object addresses; position bytes captured; heightmap callback errored, so heightmap values are invalid | Scene presented; no HeightMap_Enter call observed |
| world-level4-20261020T012447Z | 190 opaque world-object addresses; terrain calls observed | Scene presented; selected native calls unobserved |
| world-level7-20261020T012602Z | 1,158 opaque world-object addresses; terrain calls observed | Scene presented; selected native calls unobserved |
| world-level1-20261020T012833Z | Corrected heightmap probe: 1,599 returns, six distinct inputs, no callback failures; 664 opaque objects | Selected native calls unobserved |
| world-level1-20261020T013156Z | Escape queued to owned IGI window; 977 heightmap returns; 664 opaque objects; no callback failures | Selected native calls unobserved |

The first capture predates driver error propagation; its raw message file
contains the heightmap errors even though its old summary says observed.
Keep the failed observations as history, not passing evidence. One additional
repeat (`world-level1-20261020T012723Z`) observed no retail calls and is
inconclusive. Variable observation windows and cutscene phases mean call counts
cannot establish cross-level or cross-program parity.

## Latest paired transform checkpoint

The final diagnostic executable has SHA-256
`e0faef4c781b85e96b4aa76d77641fcb5756f06c9af59005d761917903a72c63`.
The identical frozen probe was used sequentially against retail and candidate.
The candidate's existing scene log supplies its transform values; its native
world-object entry is still unobserved. This is an observable-data comparison,
not a claim of matching native execution paths.

| Level | Capture suffix on 20261020 | Retail rigid objects | Candidate rigid objects | Exact matches |
| --- | --- | ---: | ---: | ---: |
| 1 | T052212Z | 380 | 380 | 380 |
| 4 | T052417Z | 101 | 101 | 101 |
| 7 | T052453Z | 667 | 667 | 667 |

Comparison uses a multiset of model instance name, all three position double
bit patterns, and all nine orientation float bit patterns. Duplicate records
retain their multiplicity. These are the objects explicitly identified through
the observed EditRigidObj path, not every object in each level. Artifact hashes
and executable identities are retained in
`evidence/fpu-state-20261020/paired-transform-matrix.json`.
Recompute this comparison with `python3 scripts/compare_world_transforms.py`
followed by the capture directories. Empty, errored, or overflowed captures fail
as inconclusive; differences return a failing exit status.

The remaining orientation discrepancies were traced to x87 precision/rounding
initialization and matrix accumulation order. Six retail FPU helpers were
reconstructed from Binary Ninja, independently checked with Radare2 and angr,
and exercised in 64 paired retail/candidate x86 fixtures. Their status remains
TRANSCRIBED; finite fixtures do not establish all floating-point behavior.
The scene adapter now retains the observed retail x87 accumulation sequence.
No guessed rotation correction or per-model exception was added.

## What is still missing

Materials, texture bindings, attachments, other object classes, and complete
task identities remain unverified. No paired game-window screenshots or player
movement/collision acceptance were obtained. The instrumented scene has not
established smooth 30 FPS operation. Five terrain entry points still report
untranscribed behavior; host rendering cannot serve as evidence that those
native methods are implemented.

The latest captures used an independently owned server on `127.0.0.1:27043`
because the existing shared 27042 listener did not complete a bounded health
query. The driver now accepts `--host` and bounds initial connection checks.
No shared server was terminated. The actual commands, working directories,
Escape observations, raw probe source, errors, and per-run candidate log bytes
are preserved in each capture directory.

`build_no_verify.sh` compiles a separate Release diagnostic tree with CMake's
native gate explicitly disabled, as authorized by the user. It is not release
acceptance. The normal `build.sh` gate and commit integrity checks must pass
before any verified release claim. The current gate working tree has unrelated
changes under review, so no new checkpoint commit is claimed here.
