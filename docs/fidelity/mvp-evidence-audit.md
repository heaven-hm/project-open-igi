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

One record remains unbacked by design of the pipeline, not for lack of
attention: `Retail_Main 0x0048F530`. Its reviewed implementation changed after
its evidence was written, in commit `8027a34c`, which renames the candidate
single-instance mutex string from `IGI1MUTEX` to `IGI2MUTEX` so retail and the
candidate can run side by side for paired observation. That is a deliberate,
documented divergence, but the record's build and semantic artifacts still bind
the old source digest, and no differential runner covers a `main` entry point.
The honest state is therefore "evidence stale, re-review needed", and it is
recorded as such rather than re-stamped.

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
