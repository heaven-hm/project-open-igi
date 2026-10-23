# Level 1 Gameplay MVP

## Active branch and checkout

- Integration branch: `mvp/level1-integration`.
- Current executable: `igi1.exe`; the retail reference remains `igi.exe`. The older `open-igi.exe` is a separate diagnostic output.
- Work only in `/Users/haseeb-mir/Documents/Code/project-open-igi` for this plan. Do not create another worktree.
- Preserve and integrate the existing Codex and DeepSeek work, including captured uncommitted changes. Existing worktrees are retained for recovery; they are not additional execution lanes under this plan.
- One coordinator owns writes, Git staging/commits, shared manifests and the candidate build. Subagents may investigate independently, but only one implementation writer may modify this checkout at a time. Runtime capture is serialized through the shared lease. Do not let separate harnesses switch branches, overwrite the executable, or edit the same files concurrently.
- Existing source/evidence statuses are provisional after integration until their bindings are checked. No push, release or deployment is implied by branch creation or a diagnostic build.

This owner-directed single-checkout decision supersedes the earlier two-worktree recommendation in the historical audit below.

## Product goal: keep Level 1 gameplay

The MVP remains one **playable Level 1 mission** on Windows x86, C++20 and DirectDraw 7 / Direct3D 7, using the original local assets and retail `igi.exe` as the behavior authority. CrossOver is the local test environment, not another product platform.

Do not shrink the product goal to a scene viewer or call a walking demo the completed MVP. Instead, deliver the full goal through small accepted milestones. Audio, menus, movies/cutscenes, vehicles and decorative smoke remain deferred unless a required Level 1 gameplay dependency is demonstrated.

## Immediate target: the Level 1 player loop

The next playable milestone is: launch Level 1, load its required world/tasks, spawn the player, look around, walk/run, crouch, jump, land, collide with the required terrain/building geometry, and complete one authored building interaction. Controls, root motion, physics and task behavior must be recovered from retail evidence rather than replaced with guessed FPS rules.

This milestone does **not** include accepting AI/combat or the full mission. It is the first integrated gameplay dependency to close after the build/workflow baseline.

## Milestones and exit criteria

| Milestone | Deliverable | Acceptance evidence |
|---|---|---|
| M0 — Integration/build baseline | Consolidated source and required workflow helpers; one reproducible Windows x86 candidate. Fix missed-source selection and empty-scope acceptance before trusting automation. | Source snapshot, PE32 architecture, compiler/build log, executable hash and candidate receipt. An ungated build is diagnostic only. Reconcile the inherited staged rename/deletion work before claiming focused feature acceptance. |
| M1 — Level 1 scene foundation | Required models, terrain, textures and placements loaded through the supported script/task path. | Bound asset/object/transform/material observations and matched game-window captures; known visual/task omissions remain explicit. Existing diagnostic rendering is a starting point, not a reason to redo all levels or declare parity. |
| M2 — Player loop (next gameplay milestone) | Config/player parameters, simulation tick, spawn, view, locomotion/root motion, jump/landing and one required building interaction. | Scripted retail/candidate traces for input, tick, position, velocity, stance, ground/contact and interaction state. Missing clips, collision or runtime mappings block the relevant scenario. |
| M3 — One combat encounter | Required guard task/navigation/perception, one Level 1 weapon's equip/fire/reload/ammo path, hit resolution, health/damage/death and synchronized gameplay animations. | Repeatable encounter with source-bound native checks, typed paired runtime observations and explicit handling of death/failure. Then extend to the remaining Level 1-required actors/weapons. |
| M4 — Level 1 mission completion | Required objectives, interactions, AI/combat and mission success/failure operate together. | A documented start-to-completion scenario and failure/death/restart scenario on the exact integrated candidate, plus regression coverage for M1–M3 and unresolved-path limitations. |

All milestones start **OPEN** unless current integrated evidence demonstrates their exit criteria. Imported status labels and earlier screenshots do not automatically close them.

## Native testing scope: dependency closure, not the entire catalog

There is no defensible fixed native count for Level 1 yet. Derive each milestone's required closure from source bindings, startup/task dependencies and observed execution. The 3,050-entry name inventory and reference-only candidate totals are not a testing quota or completion denominator.

For each implementation slice:

1. Select one behavior and the exact changed native/source set. Discover address identities through the source/manifest mapping, not hexadecimal filenames. Unknown required identities are blockers.
2. Collect/reuse current evidence from one selected decompiler (Binary Ninja preferred; Ghidra fallback only), Radare2 CFG and angr exact-entry recovery. Keep semantic/ABI/side-effect review separate from structural similarity.
3. Run the appropriate focused behavioral comparison and compile the current x86 candidate. Keep required semantic and behavioral checks enabled for acceptance.
4. Capture retail then candidate sequentially with the same level, assets, configuration, reviewed typed probe and scripted actions. Bind the observations to the current source, binary, candidate and probe hashes; retain complete ordered calls and required state effects.
5. Compare every required address and observable from the gate's declared feature scope. Unobserved required paths, stale source hashes, unknown ABI or mismatches remain blocked/inconclusive. Reuse unaffected accepted evidence only while its source/binary/probe/scenario bindings remain valid.
6. Replay the integrated milestone to catch interactions between individually checked functions. Capture actual-window screenshots for visible increments. Finite scenario coverage is scoped evidence, not universal equivalence.

The standard feature gate remains:

```sh
python3 scripts/native_gate.py --root . --changed \
  --threshold VERIFIED --cfg-threshold 0.85 \
  --require-semantic-evidence --require-behavioral-tests \
  --report build/native-verification.json
```

Do not narrow that required changed-feature gate or remove semantic/behavioral requirements to get a green status. Whole-catalog audits are separate. A dirty integration snapshot can have many unrelated changes; reconcile its provenance and staging deliberately instead of pretending its full changed set is a single native.

## Diagnostic build versus accepted gameplay

The owner requested `./build_no_verify.sh` for the integration build. It disables native verification and may copy the diagnostic executable into the game directory after backing up the previous executable. Preserve the build log and PE hash and label the output **UNVERIFIED DIAGNOSTIC**. Do not use it to promote natives, seal a verified build, or mark a milestone accepted. The retail `igi.exe` is not replaced.

## Automation coverage observed before integration

On 2026-10-23, Codex's checkout had 885 manifest records and five referenced paired-runtime comparison files whose hashes matched the manifest. DeepSeek's checkout had 881 records and 95 referenced runtime reports whose hashes matched, all reporting observed retail and candidate calls. Only 56 of those 95 reports' implementation hashes matched the then-current source files. This is partial coverage, not verification of every native; all counts are pre-integration snapshots and require rechecking after merge.

The automatic pipeline also has known missed-path discovery, optional fresh-intake and empty-scope concerns recorded below. Do not confuse the existence of automation with evidence that every required native was successfully tested.

## Automated checks on this branch

CMake now exposes `verify-native-static` and `verify-native-runtime`. With `BUILD_TESTING=ON`, CTest registers a scope fixture, one candidate-build receipt fixture and separate static/runtime tests for every selected native. The default Windows test scope comes from the configured `igi` target sources, with static coverage for compile-only object dependencies too; this is not all catalog entries. Compile-only natives explicitly block runtime acceptance until linked. Explicit addresses select a declared smaller investigation.

For example, configure host-side checks for two startup natives (these are not gameplay coverage):

```sh
cmake -S . -B build-native-checks \
  -DIGI_VERIFICATION_ONLY=ON -DBUILD_TESTING=ON \
  '-DIGI_NATIVE_TEST_ADDRESSES=0x0048F0C0;0x0048F0D0'
ctest --test-dir build-native-checks -L infrastructure --output-on-failure
ctest --test-dir build-native-checks -L static --output-on-failure
```

For paired runtime, configure `IGI_NATIVE_TEST_PROBE` with a reviewed typed probe, or `IGI_NATIVE_TEST_PROBE_MAP` with a JSON address-to-probe-path mapping. Explicit `verification.runtime_probe.path` entries in the manifest are also supported. A probe's hash must match that native's reviewed semantic contract; no generic raw-word probe is substituted. Set `IGI_NATIVE_TEST_LEVEL` to the scenario's level, then run:

```sh
ctest --test-dir build-native-checks -L runtime --output-on-failure
```

CTest builds/binds one candidate receipt fixture and reuses it with per-native `--skip-build` checks. Runtime remains serialized and uses the existing cross-process lease. The bridge checks for active games before any behavioral backend and blocks known live Frida runners without their own lease; it never kills another game. Missing probes, tools, receipts, observations or matching identities fail the tests rather than becoming passes.

Static collection means Binary Ninja/Radare2/angr observations were gathered; it does not mean semantic equivalence or native acceptance. Full runtime acceptance still goes through the existing strict source-semantic/behavioral gate, fresh paired capture and typed comparator. A pinned test-scope/source/manifest fingerprint prevents CTest silently missing new changes: rerun CMake after edits. `changed` scope blocks shared-header/build changes unless the complete affected closure is explicitly supplied; prefer compiled scope for those.

Infrastructure checks are generated validator fixtures, never retail observations. Their passing count is not a count of accepted natives. The diagnostic script still uses `BUILD_TESTING=OFF`; verification is a separate, explicit action, not a hidden claim made by `build_no_verify.sh`.

### Integration and check status (2026-10-23)

- Source from the captured main/Codex/DeepSeek snapshots is combined in this checkout; three subsequent Codex source changes and their four updated native records were also included. Recovery refs and the original staged/unstaged stash are retained. No worktree was created.
- `Qvm_Execute` / QVM argument helpers and the human-player/config definitions are compile-only objects, not linked gameplay. Their unresolved retail callees/state remain a player-loop blocker; static compilation cannot approve their runtime wiring.
- The combined `igi1.exe` built successfully with `build_no_verify.sh` and was copied to `/Users/haseeb-mir/Downloads/IGI/igi1.exe`. This is **UNVERIFIED DIAGNOSTIC** output.
- Thirty-one CTest-bridge infrastructure regressions pass. The configured scope fixture passed for the two startup example addresses.
- An actual static CTest ran Radare2 and angr successfully but blocked because the configured Binary Ninja HTTP service at `127.0.0.1:24642` refused the calls. This is not a static-pass claim.
- The newly launched combined diagnostic writes `igi.log` and showed roughly 7 FPS in the observed interval against its 30 FPS target; playable performance remains open.
- The initial runtime fixture blocked without a configured reviewed typed probe. No new paired-runtime acceptance is claimed. An older diagnostic game session was present during setup; it was not terminated by the test bridge.
- The normal merge-commit hook requires `VERIFIED` and blocks on incomplete/stale native evidence. For this integration only, `IGI_ALLOW_UNVERIFIED_DIAGNOSTIC_MERGE=1` runs the structural-only gate and explicitly allows the diagnostic integration commit even when that gate blocks. It saves the machine-readable report and full log under `build/native-gate-diagnostic-report.json` and `build/native-gate-diagnostic.log`, prints the failure, and never labels the result verified. This exception records integration progress only; it does not verify native behavior or gameplay.

## Full MVP acceptance

Level 1 is accepted only when M0–M4 operate together on the current Windows candidate and the declared scenarios pass their source-bound, scoped retail comparisons. A separate human gameplay sign-off is not required under the 2026-10-21 acceptance update, but screenshots, raw evidence, limitations and reproducible results remain required. Sound remains deferred. Model activity, a successful diagnostic build, catalog labels and single-native tests are insufficient.

---

The following is the **historical pre-integration audit**. Its checkout counts, old branch names and earlier worktree recommendation describe the snapshot at that time; the active single-branch instructions above now take precedence.

## Workflow audit 2026-10-23

Snapshot: 2026-10-23. Root checkout: `game-engine-impl`, HEAD `80fe859624e9b1ac03d56af385c1417a70c8bed1`. This is a source/workflow audit, not a new gameplay acceptance run. Existing changes, running agents, game processes, branches, index and evidence manifests were not modified.

### Decision

The current product is a Windows x86 model/terrain inspection scene, not an accepted playable Level 1 mission. More model calls will not by themselves close the integration and runtime-evidence gaps. Keep Codex and DeepSeek working on separate owned slices, with one designated integration owner and one serialized retail/candidate capture lane.

Two model workers are not the problem. Unclear ownership, different checkout state, unreliable work discovery and evidence becoming stale are the problems.

### Checkout boundaries

| Checkout inspected | Branch observed | Audit observation |
|---|---|---|
| Main checkout | `game-engine-impl` | 1,232 changed paths: 754 staged modifications, 450 staged deletions, 26 staged additions and two unstaged modifications at initial inspection. |
| `.worktrees/codex-smart-workflow` | `codex-smart-workflow` | Separate Git worktree; its work is not automatically present in the main checkout. |
| `.worktrees/deepseek-smart-workflow` | `integration-both` | 610 changed paths at inspection; workflow helpers exist here that are absent from the main checkout. |

There is also a `codex-f0f0-runtime` worktree; a directory name is not proof of which live agent owns a process. The user's statement establishes that Codex and DeepSeek are working. This audit did not inspect credentials, change their configuration, stop them, or certify that either live process is reading the documents updated here. Markdown changes in this checkout require a reviewed handoff before another worktree consumes them.

### What is missing for the Level 1 MVP

| Order | Missing deliverable | Observed evidence / limit | Exit condition |
|---|---|---|---|
| 0 | Reproducible integrated x86 candidate | Main checkout and agent worktrees differ; evidence records are stale or missing. | One identified source snapshot, build receipt and candidate hash with the appropriate strict feature gates passing. |
| 1 | Accepted scene placement/material/terrain path | Models and terrain are rendered, but README explicitly says visual artifacts and parity remain unresolved. Drawing decoded task data is not complete execution of the retail task lifecycle. | Paired Level 1 object transforms, material/texture resolution and actual-window captures tied to the same candidate. |
| 2 | Retail QVM/task execution and tick closure | `docs/fidelity/input-retail-gap-20261020.md` distinguishes decoded config calls from the retail load/execute/cleanup path. | Required Level 1 tasks and native dispatch execute in supported retail order; unknown required calls block instead of being skipped. |
| 3 | Reliable player movement, root motion and world collision | `PlayerController` and `TerrainCollisionGrid` are host-side implementations. `retail_model_scene.cpp` catches motion exceptions, restores player state and sets `player_motion_available=false`. Missing root-motion clips can therefore leave a rendered scene with motion disabled. | Paired input/position/velocity/stance/ground traces for walking, jumping, landing, walls, building entry and required stairs/doors/ladders. No substituted movement constants. |
| 4 | Live AI and graph navigation | Current README excludes playable AI; rendering a character or decoding a graph is not patrol/perception/combat integration. | Level 1 guards spawn from tasks, follow authored graphs, detect/lose the player and enter combat in a repeatable scenario. |
| 5 | Health, damage, weapons and inventory | The scene explicitly logs `health/ammunition native integration pending`; F9 is only a request. Historical macOS weapon checkpoints are not current Windows evidence. | Equip/fire/reload/ammo/hit/damage/death work together in the current Windows candidate with measured retail comparisons. |
| 6 | Gameplay animations and interactions | Animation assets and attachments do not establish synchronized locomotion, reload, death or task callback behavior. | Animation events, root motion and interactions advance through the actual gameplay state/task loop. |
| 7 | Mission objectives and completion/failure | No integrated playable mission acceptance is established by current scene captures or native inventory. | One reproducible Level 1 path from spawn through its required objectives to success, plus a failure/death/restart scenario. Retain observable coverage and limitations. |

Audio, menus, movies/cutscenes, vehicles and decorative smoke remain deferred under the current `MVP.md` / `plan.md` scope. Contradictory older statements in `PROMPT.md` do not demonstrate implementation or acceptance. Do not silently expand to every native or all levels to satisfy the Level 1 MVP.

### Major workflow issues

#### P0 — The advertised pipeline is incomplete in the main checkout

`AGENT_PIPELINE.md` tells an agent to run `scripts/automatic_reverse_engineering.sh`. Its Python driver invokes `igi_workflow.py`, `workflow_self_check.py`, `frida_shell_smoke.py` and the `igi-workflow.json` configuration. Those files are absent from the main checkout but present in the DeepSeek worktree. The intake dependency `smart_end_to_end_verification.py` also exists in that worktree.

Do not call this a uniformly available repository-wide pipeline. Integrate the workflow dependency closure as a small reviewed change, with imports and command paths checked in the checkout that will actually build the game. Do not copy all dirty worktree changes indiscriminately.

#### P0 — Automatic change discovery can omit real native edits

`scan_changed_native_addresses()` in `scripts/advanced_reverse_engineering_pipeline.py` extracts eight-digit hex tokens from changed **paths**, then looks those addresses up in the manifest. Normally named engine files have no such token.

Isolated reproduction, using the real manifest and mocked Git command output only:

```text
changed implementation: engine/src/retail/core/frame_natives.cpp
manifest address:       0x00401050
pipeline selected:      []
```

The main and DeepSeek copies of this pipeline were byte-identical when checked. Resolve changed source paths through manifest/source-native mappings, as the strict native gate does; do not rely on address-bearing filenames. An empty selection must be explicitly diagnostic/blocked, never feature acceptance. The final-result branch currently has no independent nonempty-scope requirement; healthy infrastructure plus no selected natives can reach a `passed` result without feature work.

#### P0 — Fresh paired intake is optional in the top-level command

The driver invokes intake only when `not args.skip_intake and args.probe`. Its advertised default invocation supplies no probe. Strict per-native gates can themselves require behavioral runners, so this is not a claim that all behavioral checks are disabled. It means a top-level `passed` label does not by itself establish that this invocation built the current candidate and captured a fresh reviewed retail/candidate scenario.

Require explicit acceptance mode with nonempty scope and a reviewed typed probe, or report gate-only work as diagnostic. Retain source/candidate/probe/scenario hashes and the final typed comparison. Never weaken the existing gate or manufacture observations to achieve a green status.

#### P1 — Unlimited reruns can repeat the same native without advancing the MVP

The driver sorts changed addresses and selects `addresses[:max_natives]`, default one. It does not itself keep a persistent completed/blocked scheduler or choose the next gameplay dependency. The strict gate is run before that invocation's additional angr analysis, and its earlier failure is not cleared by a later successful analysis.

An external harness may already schedule explicit addresses; that live scheduling was not established here. Without such a scheduler, repeated default runs can revisit the same first native. Existing `native_verification_queue.py` already has fingerprint-bound checkpoint/resume logic; evaluate connecting that machinery rather than adding another unrelated queue.

Use a dependency-aware Level 1 queue with `ready`, `in_progress`, `blocked`, `needs_review`, `accepted_for_scope`, owner, source snapshot, last failure fingerprint and next action. Repeated identical failures should produce a blocker/handoff, not more model-generated status text. Unlimited token allowance does not remove runtime/tool contention or disk limits.

#### P1 — Evidence/source drift and source binding are material blockers

A fresh inventory-only run of `scripts/mvp_scope_report.py` found:

- 112 address-bound engine native identities in its source scope.
- 14 identities without verification records.
- 29 existing records whose implementation is missing or whose source hash no longer matches.
- 19 source files with no native identity discovered by this inventory.
- 1,862 reference-only candidates, which are neither missing gameplay features nor proven runtime dependencies.

The 112 status labels were 70 `STRUCTURALLY_VERIFIED`, 20 `VERIFIED`, three `VERIFIED_WITH_DOCUMENTED_SCOPE`, five `ANALYZED` and 14 without a status. These are inventory labels, **not a freshly revalidated acceptance count**; stale records can overlap them. Inventory excludes audio and diagnostics and does not establish runtime reachability.

The saved `build/native-verification.json` contains 1,544 failure lines, while narrower saved reports have different scopes and failure counts. These are retained artifacts, not a freshly rerun gate. Do not compare those totals as native completion percentages. Fix source bindings, ABI and evidence for the current feature closure, then rerun the strict check on the exact snapshot. Keep the rename/deletion campaign separate from gameplay fixes.

#### P1 — Two workers share one runtime capture resource

`frida_capture_lease.py` already provides a host-keyed exclusive capture lease, and several pair runners use it. Other runners, such as `frida_lightmap_pair.py`, only preflight whether a game is active. A saved lightmap report is blocked by `a game is already active; refusing to overlap captures`.

Use the shared lease consistently for paired capture, including across Codex and DeepSeek worktrees. Run retail then candidate sequentially according to current `AGENTS.md`; do not terminate another worker's game or assume separate Git branches imply separate Frida servers, asset directories, installed executables or mutexes. Build candidate binaries in separate build directories and pass their exact paths/receipts to the serialized capture lane.

#### P1 — Documents disagree about scope and acceptance

`HEAVEN.md` targets another checkout/branch and contains historical push, no-tests, structural-only and simultaneous-runtime instructions. These conflict with current `AGENTS.md` / per-native feature acceptance. `PROMPT.md` requires sound and describes an older metadata-only status. `plan.md` contains a name-equality verification rule, old terrain status and older manual-review gates; its acceptance banner already supersedes the manual-review language. `AGENT_PIPELINE.md` still calls the current increment an inspection/mouse-look demo.

Treat `AGENTS.md` as the current workflow rules, `MVP.md` as product scope, and source-bound evidence as completion truth. The dated audit links added during this review identify drift; old numeric claims and older records must not become acceptance by repetition. Gate-integrity pins passed with `LC_ALL=C LANG=C`; do not re-pin already-correct files just because an old prompt says they need repair.

#### P1 — Some workflow instructions do not travel through Git

`git check-ignore -v` identifies `.gitignore:6:/*` as ignoring `AGENT_PIPELINE.md`, `HEAVEN.md` and newly created files under `docs/`. Existing tracked files remain tracked, but editing these ignored handoffs or adding a new audit file does not make them part of normal branch integration. This can leave Codex and DeepSeek following different local instructions despite a successful code handoff.

The canonical version of this audit is therefore included in tracked `MVP.md`, linked by the tracked README, plan and project prompt. The separate `docs/` copy is a local reading convenience. No ignore rules or Git index entries were changed by this review. Treat future workflow/prompt handoffs as explicit versioned artifacts, not undocumented local files.

### Recommended Codex / DeepSeek division

This is a proposed ownership plan, not a claim that either running harness has been reconfigured.

| Lane | Suggested owner | Scope | Handoff |
|---|---|---|---|
| Build/gate/integration | Codex | Repair missed-source discovery, empty-scope acceptance, workflow dependency integration, candidate receipts and source/evidence binding. | Small reviewed commits/patches with exact files, hashes, checks and remaining blockers. |
| First playable dependency | DeepSeek | One bounded Level 1 task/config/player/root-motion dependency at a time, selected from the actual earliest divergence. | Candidate source plus evidence, typed probe, precise scenario and unresolved dependencies. |
| Shared runtime capture | One lease holder at a time | Retail then exact candidate, same level/config/actions; no competing launches. | Raw traces, candidate/build/probe identities, typed comparison and actual-window capture where visible. |
| Acceptance/integration | One designated integrator | Review and integrate one completed slice, rebuild, invalidate stale evidence and replay the integrated scenario. | Update `MVP.md` with the accepted behavior and reproducible evidence, not just native counts. |

Do not give both workers simultaneous ownership of the shared manifest, gate scripts, branch-wide rename campaign or installed game executable. Keep the full catalog as inventory and research input; prioritize the dependency closure of the next visible/playable behavior.

### Next steps

1. Pin the integration checkout and confirm what each live harness reads; hand off this audit to both without overwriting their dirty trees.
2. Fix missed-path discovery and empty-scope success; integrate the missing workflow helpers; keep semantic/behavioral requirements enabled.
3. Separate rename/path reconciliation from behavior changes and refresh only the evidence invalidated by the selected slice.
4. Complete one Level 1 loop: load tasks/config/player parameters, spawn, walk/jump/land against real terrain and one building interaction, with typed paired traces. This is an intermediate demo, not a redefinition of the full MVP.
5. Add AI/combat/health/ammo, gameplay animations and objective transitions in the canonical order; replay the integrated Level 1 mission after every accepted slice.
6. Stop treating model activity, catalog size, successful parsing or single-native verification as proof of a playable mission.

### Checks actually performed

Read-only source/document/worktree inspection; inventory generation to `/tmp/project-open-igi-mvp-audit-20261023.json`; isolated change-discovery reproduction with mocked Git output and the real manifest; gate-integrity hashes with `LC_ALL=C LANG=C`; free-space check (about 11 GiB available, 95% used at inspection).

No engine implementation was changed. No new tests were written, no full strict/native/host build was executed, no gameplay was launched, no external agent was stopped, and no push/merge/staging/commit occurred during this audit. Live tool health, the running harness configuration, and retail parity were not established. Recheck counts after either worker changes the relevant snapshot.
