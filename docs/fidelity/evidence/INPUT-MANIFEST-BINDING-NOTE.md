# Input-layer natives: manifest binding is the current gate blocker

Run 2026-10-24 (round 10), repo state after commit `30282cc5`:

```
$ python3 scripts/native_gate.py --root . --changed --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
native_gate: v4 catalog consistency checked for 0 selected entries
native_gate: OK - 0 changed native(s) passed ...
```

**That OK is an empty scope, not a pass.** The changed-native gate selects
natives through `docs/fidelity/native-verification.json`; the input-layer natives
landed this session (`0x4AF740`, `0x4AE730`, `0x48F090`, `0x48F0A0`, `0x48FA80`,
`0x48FD90`, `0x48FED0`, `0x490120`, `0x490190`, `0x48F270`, `0x509CC0`,
`0x48F0A0`, `0x509E90`, `0x509F00`, `0x490340`, `0x490350`, `0x509CC0`) have **no
manifest records**, so the gate cannot see them and cannot report CFG >= 0.85 or
semantic/behavioral evidence for them.

`scripts/build_native_records.py` cannot create the records yet either:

```
FileNotFoundError: docs/fidelity/evidence/0x0048F090-binaryninja.json
```

So each record requires a per-native Binary Ninja evidence artifact at
`docs/fidelity/evidence/0x<VA>-binaryninja.json` in the schema that script
expects, plus the semantic/behavioral evidence the gate demands at
`STRUCTURALLY_VERIFIED` (CFG >= 0.85) and beyond.

## Consequence

- The natives are honestly at TRANSCRIBED. No CFG/angr/runtime claim exists for
  them, and the empty gate result must never be cited as one.
- Objective compliance ("all game natives match CFG >= 0.85 and are runtime
  verified") is blocked on: (1) per-native BN evidence JSONs, (2) manifest
  records generated from them, (3) the runtime path that can observe them.

## Next actions

1. Produce `0x<VA>-binaryninja.json` evidence for each landed input native from
   the already-captured Binary Ninja analysis (function extent, blocks, edges,
   calls, data refs) - no re-decompilation of a different tool.
2. Run `scripts/build_native_records.py <VAs>` then the gate; record per-native
   CFG scores; fix any native below 0.85 before it is used in a body that runs.
3. Only then pursue runtime observation, which still requires the registrar
   chain to reach the input path.

## Round 11 update: evidence-artifact chain, one step further

Created the Binary Ninja disassembly artifacts for the seven single-block natives
(`0x0048F090`, `0x0048F0A0`, `0x0048F270`, `0x004AF740`, `0x004AE730`,
`0x00490340`, `0x00490350`) at `docs/fidelity/evidence/0x<VA>-binaryninja.json`
in the existing schema (schema_version 1, kind native-disassembly, tool
binary_ninja, retail SHA-256, cfg nodes/edges).

`build_native_records.py` then fails on the **next** artifact in the chain:

```
FileNotFoundError: docs/fidelity/evidence/0x0048F090-radare2.json
```

So a record needs at minimum BN + radare2 (+ semantic-review/behavioral/build)
artifacts per VA. None of those can be fabricated: radare2 must be an actual
capture of the retail CFG for that entry, and semantic-review/behavioral records
must reflect review actually performed. The remaining steps are therefore tool
runs, not authorship:

1. radare2 CFG capture per VA -> `0x<VA>-radare2.json` (same schema family as the
   existing `0048F1A0-radare2.json`).
2. semantic-review artifact per VA against the transcription just written.
3. behavioral/build artifacts from the real candidate and probe runs.
4. `build_native_records.py` -> gate -> per-native CFG score.

Until then the input natives stay TRANSCRIBED and the changed-native gate
continues to report an empty scope.

## Round 13: why three VAs produce no record

`build_native_records.py` never reports an error for `0x48F0A0`, `0x4AF740`,
`0x4AE730`; it prints `would build 0 record(s)`. Reading the script explains it:
candidates come from `discover()`, which walks `ng.changed_paths(ROOT,
"production-tree")` and parses each source with `ng.implementation_natives(path,
relative)`. The `--addresses` argument is only a filter applied *after* discovery,
and a `continue` for addresses already in the manifest is silent. So these three
files are not being parsed as natives by `implementation_natives`, and the
builder cannot see them at all - the manifest is not the blocker here, the source
recognizer is.

Next diagnostic (not yet done): dump `ng.implementation_natives` results for
`engine/src/retail/input/native_48f0a0.cpp`, `.../native_4af740.cpp`,
`.../native_4ae730.cpp` and diff the provenance line shape against a file that
was discovered (`native_48f090.cpp` built fine). Candidate causes: the
`// Native: ...` line shape for a getter whose name has no behavior suffix, the
`extern "C"`-only signature (no `NATIVE_DECL` token), or the `IAT thunk` phrase in
the name. Fix belongs in the provenance/signature comment of those three files or
in the recognizer's pattern list - not in a hand-written manifest record.

## Round 14: the three VAs are already in the manifest - with wrong identities

The recognizer is fine: `ng.implementation_natives` returns the expected native for
all four files (`0x0048F090`, `0x0048F0A0`, `0x004AF740`, `0x004AE730`). The
builder skipped three because the manifest already contains those addresses:

| VA | existing manifest name/status | existing implementation |
| --- | --- | --- |
| `0x0048F0A0` | `AppContext_GethWnd`, **VERIFIED** | `scripts/GameNatives/0048f0a0-AppContext_GethWnd.cpp` |
| `0x004AF740` | `Camera_ApplyBandFilter_30`, STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF740-Camera_ApplyBandFilter_30 2.cpp` |
| `0x004AE730` | `Camera_SetFieldOfView_30`, STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AE730-Camera_SetFieldOfView_30 2.cpp` |

So the catalog carries **stale/incorrect identities for these exact VAs**:
`0x004AF740`/`0x004AE730` are the `DirectInputCreateA`/`DirectInputCreateEx` IAT
thunks (`ff 25 38/3c 30 53 00`), not camera functions, and `0x0048F0A0` is the
window-handle getter (`a1 c4 8b 5c 00 c3`), not `AppContext_GethWnd`. One of them
is even marked VERIFIED against a different implementation.

This is a fidelity problem, not a bookkeeping nuisance: a VERIFIED status on the
wrong identity can hide a real native, and it is exactly the failure mode
`--changed` cannot surface because the address is "already covered".

## Required next steps

1. Re-point those three records at the engine transcription (`implementation.path`
   + sha256), correct the names to the behavior the bytes prove, and drop the
   VERIFIED status until this session's evidence supports it - using
   `scripts/apply_manifest_names.py` / the record tooling, not hand edits that
   bypass the evidence schema.
2. Re-run `build_native_records.py` for the three with `--force` so BN+r2
   artifacts are generated from the retail image and the CFG score is recorded.
3. Audit for other catalog entries whose names disagree with their bytes; the
   camera-named thunks suggest the old catalog had address/name drift.

## Round 16: multi-block binding - three natives score below the 0.85 threshold

`build_native_records.py --force` bound the multi-block input natives. CFG scores
(the script's BN-vs-r2 comparison over retail-derived artifacts) are:

| VA | BN nodes/edges | r2 nodes/edges | score | verdict |
| --- | --- | --- | --- | --- |
| `0x00509CC0` | 3/2 | 3/2 | 1.000 | pass |
| `0x00509E90` | 9/11 | 9/11 | 1.000 | pass |
| `0x00509F00` | 1/0 | 1/0 | 1.000 | pass |
| `0x00490120` | 5/6 | 5/3 | **0.675** | BLOCKED (<0.85) |
| `0x0048FED0` | 10/11 | 10/3 | **0.527** | BLOCKED (<0.85) |
| `0x00490190` | 10/10 | 10/2 | **0.480** | BLOCKED (<0.85) |

`0x0048FA80` and `0x0048FD90` produced no record (not reported at all - to
diagnose next; likely not discovered or already present with another path).

The three failures are **tool-disagreement on edges**, not node disagreement:
both tools find the same block count, but Binary Ninja reports many more edges
than radare2 (e.g. 10 nodes 11 BN edges vs 3 r2 edges). That pattern is what a
switch/jump-table function looks like when one tool resolves indirect targets and
the other does not - and all three are exactly jump-table dispatchers
(`0x490120` message table, `0x48FED0` message table, `0x490190` selector table).

Per the gate rules these three must not be treated as CFG-verified and must not be
routed through runtime until the disagreement is resolved. The honest state is:
transcribed, evidence artifacts exist, CFG score below threshold, cause identified
as indirect-branch recovery, not as a suspected transcription error.

## Round 17: root cause of the "missing" records - NATIVE_COMMENT needs the VA on line 1

`ng.implementation_natives` matches `NATIVE_COMMENT`:
`^\s*//\s*Native:\s*(?P<name>.*?)\b(?:retail\s+)?VA\s+0x(?P<address>[0-9A-Fa-f]{8})\b`

`native_48fa80.cpp` and `native_48fd90.cpp` put the extent on the *Artifact* line
instead, so their `// Native:` line had no `VA 0x........` and the recognizer
returned `[]` - silently, with no gate error. Fixed by moving
"; retail VA 0x........-0x........" into the `// Native:` line (BN alias moved to a
parenthesis). Both then bind:

| VA | BN | r2 | score |
| --- | --- | --- | --- |
| `0x0048FA80` DirectInput_ErrorText | 59/64 | 59/58 | **0.939** (pass) |
| `0x0048FD90` Input_MouseGetCaps | 4/4 | 4/4 | **1.000** (pass) |

Note both are the *retail-derived* CFGs the builder writes; the score is
tool-to-tool agreement, not a claim about the C++ transcription.
