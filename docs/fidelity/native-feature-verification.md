# Per-native feature verification

The default is `./build.sh`, using `native_gate.py --changed`. This selects
staged, unstaged and untracked edited native sources. It does not discover all
runtime calls or prove that an edited function is reached. The resulting
`build/native-verification.json` defines the required feature scope; the runtime
runner and checker must use that same report. Whole-catalog checks are separate
audits, not the default feature gate.

## Required sequence for each native

1. Locate the exact entry in the hashed retail `igi.exe`. Record the signature,
   ABI, globals, callees and assumptions using Binary Ninja first (Ghidra only
   if unavailable), Radare2 CFG and independent angr CFGFast recovery.
2. Review the candidate's semantics against those observations: arithmetic,
   branches, state changes, memory accesses, ordering, errors and returns.
   Keep source and evidence hashes current in
   `docs/fidelity/native-verification.json`; status labels do not create proof.
3. Run the focused retail/candidate behavioral runner, including relevant edge
   cases and a negative case that detects the divergence being fixed. For a
   single-native investigation, use `native_gate.py --root . --file FILE
   --address VA --threshold VERIFIED --cfg-threshold 0.85
   --require-semantic-evidence --require-behavioral-tests --report build/native-focus.json`.
4. Compile the changed candidate for Windows x86 DirectDraw/Direct3D 7. This is
   an unaccepted test executable until the runtime gate passes. The build input
   snapshot must still match at sealing.
5. Run retail and that candidate sequentially from
   `/Users/haseeb-mir/Downloads/IGI`, with `window levelN`, the same assets,
   configuration, task script and recorded actions. Use the same Frida probe
   against both images, resolving candidate symbols from the new executable.
   The runner temporarily copies the exact candidate bytes under a unique
   `open-igi-probe-*.exe` name beside retail assets, because the current host
   derives its data root from the executable directory. It removes this test
   file afterward and never replaces the installed `open-igi.exe` before acceptance.
   Controls and movement values come from `config.qvm` / `humanplayer.qvm`.
6. Compare normalized inputs, outputs and all relevant state changes through
   a typed, evidence-backed per-native comparator. Record the first divergence,
   fix from retail evidence, and repeat steps 1–6. Only then accept the feature,
   seal/deploy, create a small local commit and proceed to the next increment.

The default build performs steps 1–3 via the evidence gate, compiles the
candidate, then invokes these commands before sealing/deployment:

```sh
python3 scripts/native_frida_diff.py \
  --gate-report build/native-verification.json \
  --candidate-exe build-win32/engine/platform/windows/open-igi.exe \
  --level 4 --out build/native-frida-differential.json
python3 scripts/check_frida_differential.py \
  --gate-report build/native-verification.json \
  --candidate-exe build-win32/engine/platform/windows/open-igi.exe \
  --report build/native-frida-differential.json
```

Use `build/native-focus.json` instead for both commands when investigating one
native. Choose the level that actually exercises that method; level 4 is only
the default scenario. Expand to the documented three-to-five-level matrix for
shared terrain, camera, player or world changes. See
[runtime-test-workflow.md](runtime-test-workflow.md) for local tools and recovery.

## Strict new-native intake: static analysis then runtime analysis, then add

For every new native implementation increment, the autonomous workflow may add the
implementation to the codebase only after BOTH analyses pass, in order:

1. **Static analysis**: `python3 scripts/igi_workflow.py static <VA>` (native gate:
   selected decompiler + Radare2 CFG + angr, source-semantic review, behavioral
   tests, threshold `VERIFIED`, CFG threshold `0.85`).
2. **Runtime analysis**: `python3 scripts/igi_workflow.py verify <VA> --probe <reviewed-probe> --level <N>`
   or the strict wrapper `python3 scripts/igi_workflow.py intake <VA> --probe <reviewed-probe> --level <N>`.
   The intake runs the static gate, then the paired Frida capture and the typed
   runtime comparison through `smart_end_to_end_verification.py`, and accepts
   (report `result: passed`, `verified: true`, exit 0) only when every phase passes.

If any phase fails, the native is **rejected**: no implementation is added to the
codebase, no status is promoted, and the failure is retained in the report.

`--static-only` is diagnostic-only and is now **fail closed**: it exits blocked and
cannot be used to accept a native. Static-only evidence is analysis evidence, not
acceptance.

The default `./build.sh` chain (`native_gate.py --changed` + paired runtime +
typed comparison via `accept_changed_feature.py`) remains the strict acceptance
entry point.

## Fail closed and report the actual scope

Missing methods, missing symbols, missing observations, failed tools, unknown
ABI, incomplete comparisons or mismatched image/report hashes block acceptance.
Do not use `--allow-unobserved`, `--limit`, skip flags or status edits to turn
these into success. An empty changed set does not verify a new feature.

The generic `frida_native_diff.js` records raw argument words and EAX. It cannot
validate x87/float returns, 64-bit ABI values, pointer contents, global side
effects or startup calls that occurred before attachment. Its Python driver
preserves every ordered call; a capture exceeding capacity blocks instead of
truncating. These limitations must
remain inconclusive until a typed per-native script captures and compares the
required scenario, call sequence and state. A comparator must produce evidence,
not merely set a success flag. No universal equivalence claim follows from a
finite scenario.

The driver assembles a per-native `comparison` object with a nonempty
`scope` and `inputs`, `outputs`, `sideEffects`, each containing normalized
`retail` and `candidate` observations. The checker recomputes equality from
the complete call sequences; success booleans alone cannot authorize acceptance.
The semantic-review artifact must include a reviewed `runtime_contract` with
`scope`, `probe_sha256`, and `required_observables` lists for `inputs`, `outputs`,
`sideEffects`. Each call must supply dictionaries containing those named fields.
Empty requirement lists need explicit reviewed `no_arguments`, `no_return`, or
`no_side_effects` flags respectively. The native gate copies this source-bound
contract into its report. Empty placeholders or a different probe/scope cannot
pass. The generic probe does not synthesize
this record. Numerical tolerances need a separate justified comparator; do not
round away a discrepancy in order to pass exact comparison.

For integration, set `IGI_NATIVE_PROBE=/absolute/path/to/typed-probe.js` (or pass
`--probe` to the runner). That same script is injected into both executables.
Keep the `hook(address)` RPC and `observed` message interface; include an
evidence-backed `typedState` object in each observation with `scope`, `inputs`,
`outputs`, `sideEffects`. The driver pairs the two normalized observations into
`comparison` and computes acceptance; it never infers missing state. A typed
probe must capture the relevant call sequence as part of that state, not just
the final convenient return value. Its hash and every raw message are saved.

Preserve raw captures and reports under `build/runtime-traces/` or
`docs/fidelity/evidence/`, including retail/candidate/probe hashes, gate report
digest, native addresses, source hashes, level, script, config, inputs, capture
boundaries, comparison tolerances and unresolved behavior. A feature pass is
valid only for that recorded scope. RE-Agent coordinates evidence and review;
REA is a separate upstream tool and must not be conflated with it.
