# MVP evidence audit: what the labels are actually backed by

`scripts/audit_mvp_records.py` walks the 120 MVP natives and validates each
record with the gate's own `validate_report_artifact`, so a record it passes is
a record the gate would pass. It does no angr, radare2 or game work, so it is
cheap enough to run over the whole set. `docs/fidelity/mvp-record-audit.json`
holds the per-native result.

Run on the current branch:

| measure | count |
| --- | --- |
| MVP natives | 120 |
| labelled `VERIFIED` in the manifest | 81 |
| saved evidence the gate's validators accept | 119 (53 when the audit was written) |
| runner configured, so the strict gate runs it live | 119 (upper bound) |
| paired Frida evidence with calls in both images | 8 |
| records that pass the full strict gate today | 6 |

## Why one record is still not backed

The audit started with 67 of 120 records unbacked. The causes were all in the
saved evidence rather than in missing work, and each was fixed at its source:

| cause | records | fix |
| --- | --- | --- |
| behavioural report did not identify the test runner | 58 | runners re-run; the orchestrator attests the runner path, digest and argv it actually executed |
| runner command was not an argv array | 8 | same attestation, and invalid runner blocks are replaced only when the orchestrator knows the real invocation |
| runner digest no longer matched the runner | 4 | the refresh re-ran the current runner |
| implementation digest stale | 1 | still open, see below |

A guard was added after a full refresh briefly made things worse: it stages each
new report under `docs/fidelity/evidence`, validates it with the gate's own
`validate_report_artifact`, and only then stores it. A runner that emits a
report the gate would reject can no longer overwrite a good artifact with a bad
one.

`main 0x0048F530` needed a different kind of repair. Its saved semantic
review did not describe `main` at all: it asserted chunk-tag constants, a
sibling/child walk and a payload pointer, which is an IFF chunk-tree review. Its
implementation comment was also truncated mid-sentence, and its block referenced
build and behavioural artifacts bound to a source digest that no longer existed.

The record was re-reviewed against the actual retail body instead of re-stamped.
Radare2 over `0x0048F530..0x0048F970` shows the WinMain shape: `CreateMutexA`,
`CoInitialize`, `GetForegroundWindow`, `LoadIconA`/`LoadCursorA`,
`RegisterClassA`, ~20 startup getter/registration calls, a DirectX version check
with a MessageBox, and then the per-frame loop. The port's `Main` is a scoped
transcription of that startup order plus the frame loop.

The corrected review states exactly what is and is not claimed: identity,
argument mapping, constants and return/error paths pass; memory layout and
floating point are not applicable; and the skipped subsystems (DirectX check,
script setup, the startup registrations, movie/subtitle, particles, debug keys,
sound/joystick) are enumerated rather than implied. Two divergences are recorded
openly: the class name registered by that unit is `IGI` while the executable's
real entry registers the retail `Innerloop`, and the mutex is `IGI2MUTEX` by
design for paired observation. The stale build and behavioural references were
removed rather than left pointing at artifacts for a source that no longer
exists; at STRUCTURALLY_VERIFIED the gate does not require them.

That makes all 120 records self-consistent. It does not make `Main` a verified
1:1 transcription, and the record now says so.

## What the end-to-end number means

8 records have a paired Frida report with positive observations on both sides.
6 of them also satisfy every other pillar and the status label, and were
re-verified with the strict gate this session:

`0x00401050` QTask_OpenEvent, `0x004CF390` DynObj_LoadModelMesh,
`0x00402880` Game_GetContext, `0x0048F170 AppContext_GetIsActive`,
`0x004B0BA0` Mem_InitHeapState, `0x004B0D40` QHash_ValueSet.

The other two are blocked by something other than evidence: Flow_RunHandler has
no behavioural runner case for a 1213-byte function with four untracked
callees, and Terrain_Load is a host-backed bridge held at ANALYZED.

## The honest summary

- "78 VERIFIED" is a label count, not an evidence count. 53 of those records
  are backed under the default definition.
- 6 natives are verified end-to-end under the definition this project actually
  needs: CFG >= 0.85, angr, a live behavioural runner and a paired Frida
  observation with calls in both images.
- The remaining work is mechanical for most records (regenerate the behavioural
  artifact from the runner) and harness work for the rest (addresses the runner
  does not support, and natives the candidate never executes).

## A runner edit invalidates every saved behavioural report

Each saved report records the runner's digest, because the report is only
trustworthy if the code that produced it is identified. That binding has a
consequence worth stating: editing the runner makes every previously saved
report stale at once. During this work the audit count visibly dropped from 68
to 50 the moment `scripts/native_unicorn_runner.py` gained the Ilff adapters,
then recovered to 70 once the refresh was re-run against the final runner.

The lesson for the campaign: finish runner changes before refreshing evidence,
or budget a full re-run (about forty addresses take a few minutes) after any
runner change. The alternative — loosening the digest binding — would destroy
exactly the property that makes these reports worth anything.
