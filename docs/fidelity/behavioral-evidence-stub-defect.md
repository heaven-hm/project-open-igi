# The evidence generator writes a behavioural stub, not evidence

`scripts/generate_native_evidence.py` is the canonical way to produce a native's
evidence set. It writes the decompiler and Radare2 CFG capture, the semantic
review, the build report and the manifest record correctly. What it writes for
the behavioural artifact is not a behavioural report.

`behavioral_evidence()` (line 354) returns:

```json
{
  "address": "0x0048F530",
  "binary_sha256": "dea34892…",
  "cases": [{"id": "static-structure", "categories": ["structure"],
             "input": {"args": [], "initial_state": {}},
             "candidate_output": {"return_value": 0, "side_effects": [],
                                  "state_delta": {"chunks_read": 0}},
             "note": "The candidate Main reproduces the retail chunk walk: …"}]
}
```

It has no `schema_version`, no `kind`, no `result`, no `implementation_sha256`
and no `runner`. The single case invents a `return_value` of 0 and a
`chunks_read` counter of 0 for any native, and its note asserts behaviour in
prose. It cannot pass `validate_report_artifact`, and it is not a measurement of
anything.

## Why this matters

Every manifest record produced by the generator points
`verification.behavioral` at one of these stubs. Under
`--require-behavioral-tests` the gate ignores the saved artifact and runs the
configured `behavioral_runner` live, so a record can still be formally correct —
but the stored evidence does not support the claim, and any audit that reads the
artifact alone is reading fiction. Under the default (non-strict) gate the saved
artifact is what gets validated, which is why this has gone unnoticed.

## What was done here

For the five MVP records regenerated on this branch, the stub was replaced with
the runner's real output: a `schema_version` 1, `kind:
"behavioral-test-report"` report bound to the current implementation digest,
with 8 of 8 cases passing for each of `0x00402880`, `0x004028A0`, `0x0048F480`,
`0x0048F140` and `0x0048F170`.

`0x0048F530` (the retail entry point) has no runner case at all, so its record
keeps no behavioural reference rather than a stub, and its limitations say so.

## The fix

`behavioral_evidence()` should either invoke the configured runner and store its
report, or refuse to write a behavioural artifact at all and let the record carry
an explicit gap. Writing a fixed-shape JSON object that looks like a report is
the worst of the three options, because it survives schema-light validation while
carrying no information.
