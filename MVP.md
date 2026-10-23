# Project Open IGI — Level 1 Gameplay MVP

## 1. Goal and authority

Deliver one playable Project I.G.I. Level 1 mission in the reconstructed Windows x86 C++20 executable `igi1.exe`, using the original local assets and DirectDraw 7 / Direct3D 7. The player must load the level, move and interact, fight the required enemies, complete the actual authored objectives, and reach the mission success state. Death/failure and a repeatable restart path must also work.

**Keep Level 1 gameplay as the full MVP.** Deliver it through the milestones below. The first gameplay milestone is the player loop; a model viewer or walking demo does not complete the mission MVP. The number of required natives is determined by the Level 1 dependency closure, not by an arbitrary quota or the whole catalog.

- Branch: `mvp/level1-integration`.
- Only implementation checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`.
- Integration baseline: local merge `730ce9bfd4209954758f1644776f4a60829def15`; obtain the current HEAD again before work.
- Create no new worktree. Existing agent worktrees and recovery refs are retained as references.
- Retail authority: `/Users/haseeb-mir/Downloads/IGI/igi.exe`.
- Retail SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
- Candidate build: `build-win32-no-verify-gate/igi1.exe`.
- Local diagnostic copy: `/Users/haseeb-mir/Downloads/IGI/igi1.exe`.
- CrossOver is the local execution environment; the product target remains Windows x86.

Read `AGENTS.md` for technical and evidence rules. This owner-requested mission scope supersedes the older inspection-only scope in its sections 10.10–10.12 and historical roadmaps. Its single-decompiler, ABI, evidence, runtime sequencing and source-preservation rules continue to apply. Complete the player milestone before promoting AI/combat into the active implementation slice.

Execution plan: [tasks/plan.md](tasks/plan.md). Work checklist: [tasks/todo.md](tasks/todo.md). Ready-to-paste captain instructions: [DEEPSEEK_HANDOFF.md](DEEPSEEK_HANDOFF.md). The prior mixed roadmap/audit is preserved in [the historical document](docs/fidelity/MVP-before-captain-plan.md).

## 2. Scope

| Required for this MVP | Deferred unless a required Level 1 dependency is demonstrated |
|---|---|
| Retail-supported startup, configuration, resource loading and simulation lifecycle | Other complete missions and whole-catalog native transcription |
| Level 1 terrain, buildings, models, materials, original textures and authored placements | Menus, launcher polish, save/load UI and unrelated editor features |
| Required QVM/task execution, native dispatch and entity state | Movies, cutscenes, decorative smoke and optional effects |
| Player spawn, camera, controls, walk/run/crouch, jump/landing and required world collision | Audio, music and sound fidelity |
| Authored doors, ladders, stairs and other interactions used by the actual mission route | Vehicles and optional actor/weapon types absent from the required route |
| Required weapons, ammunition/reload, hit/damage/health/death, enemy navigation/perception/combat | Additional platforms and renderer modernization |
| Gameplay animation/event synchronization and required inventory/mission items | Extra tools or upstream integrations without a demonstrated blocker |
| Authored objectives, success/failure and repeatable restart | Universal behavior-equivalence claims beyond captured scenarios |

A deferred subsystem's name is not permission to skip a required callee. Record its observable effects and prove the chosen exclusion is safe for the declared scenario. Developer free camera remains explicit diagnostic tooling; it cannot substitute for the player's camera or collision.

## 3. Current baseline and known limits

These observations describe the integration audit, not accepted gameplay. Recompute them after source, candidate or evidence changes.

| Item | Observed state | Meaning |
|---|---|---|
| Integrated source | Main, DeepSeek, Codex and the captured later Codex source updates were combined in local merge `730ce9bf` | Integration progress; no native acceptance implied |
| Candidate | Built and launched as PE32 x86 through `build_no_verify.sh` | Diagnostic executable works at startup; gameplay parity remains open |
| Last built candidate SHA-256 | `de2f8e81edaa52c37c09c4941014cfb4a6991a5950d2ef0a177456decdb8fb33` | Historical build identity; hash again before reuse |
| Manifest | 885 records | Inventory, not accepted coverage |
| BN + Radare2 artifact checks | 757 records had matching artifact hashes, image identity and entries; 684 also matched current source | Partial static provenance audit; not a full semantic/behavioral pass |
| Paired runtime | Five referenced comparison artifacts; one matched current source; none matched the last built candidate | **0 demonstrated natives with both current-source static evidence and paired runtime evidence for that candidate** |
| Infrastructure | 31 CTest bridge regression checks passed; sample scope fixture passed | Validator infrastructure only; these are not 31 verified game natives |
| Compiled native mapping | 144 linked mapped natives + 5 compile-only mapped natives; 28 compiled source files unbound | Resolve classification and linkage before claiming full compiled-scope coverage |
| Performance | Roughly 7 FPS observed in the diagnostic scene against its configured 30 FPS target | Performance and simulation timing need investigation |
| Git paths | 7,033 groups of tracked paths differ only by case | A case-insensitive checkout can overwrite one variant with another; inspect Git blobs before editing/staging |
| Remaining dirty evidence | 273 evidence JSON paths were modified; all are members of case-collision groups; four tracked Python caches were also dirty | These are not automatically fresh valid evidence or safe bulk-staging candidates |
| Evidence integrity | 257 of those modified artifacts were manifest-referenced; none matched its recorded file hash in that worktree audit | Repair provenance and aliases together; never just rehash an unsupported pass |

The disputed evidence set also contained reduced static records, 27 failed-to-passed semantic promotions without adequate supporting observations, and build reports for other source hashes. Some edits correctly retract prior tests. Compare original blobs, worktree variants and raw observations individually; preserve useful retractions and detailed evidence. Do not equate a casing collision with proof of who authored a bad artifact.

### Binary Ninja connection: now available

- URL: `http://127.0.0.1:24642/mcp`.
- Authorization: disabled; clients send no `Authorization` header.
- A live MCP handshake and tool listing succeeded; 75 tools were exposed.
- The already-open database is `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/igi.exe.bndb`.
- Its PE view `view_1` was selected: Windows x86, image base `0x400000`.
- A 16-byte sample at `0x0048F0C0` matched the local retail executable.
- Local connection record: `build/binary-ninja-health-latest.json`.
- One fresh existing pipeline capture at `0x0048F0C0` completed with `analysis_collected`, BN/Radare2 CFG score `1.0`, matching entry bytes and exact angr recovery. Report: `build/native-analysis/bn-reconnected-0048F0C0.json`; it retains `verified=false`. This is a startup sample, not player or mission acceptance.

This replaces the earlier connection-refused observation for Binary Ninja. Recheck health before a batch and confirm the active view; the GUI view is shared state. This connection check does not mark a native verified. Frida/IGI-Probe, compiler, angr, Radare2 and optional RE-Agent components have separate health results.

## 4. Milestones and acceptance

All milestones start **OPEN**. Close them only with evidence for the current integrated source and candidate.

| Milestone | Product outcome | Required exit evidence |
|---|---|---|
| M0 — Reliable branch and verification workflow | One usable checkout, preserved source variants, canonical noncolliding paths, correct local config, working native verification pipeline | Collision/provenance reconciliation; source-to-native/link map; tool health; current x86 build receipt; one complete native proof pilot; missing/stale/empty inputs demonstrably block acceptance |
| M1 — Level 1 world and task foundation | Required world/resources/materials load in authored positions through the supported startup/QVM/task lifecycle | Level 1 dependency inventory; object transforms and task state compared to retail; terrain/material checks; known missing calls/assets explicit; actual-window captures |
| M2 — Player loop | Spawn, correct player camera/input, walk/run/crouch, jump/land, collision and one authored building interaction | Matched scenarios with input, simulation time, position, velocity, stance, grounding/contact and interaction state; no guessed movement replacement or silent motion fallback |
| M3 — Required combat and actors | Mission-required weapons and guards, equip/fire/reload/ammo, navigation/perception, hit/health/death and synchronized animation | Isolated weapon/damage/AI scenarios followed by one integrated encounter; all required outputs and effects compared to retail; then remaining Level 1-required variants covered |
| M4 — Playable mission completion | Actual authored objectives and interactions lead to success; failure/death/restart also work | Repeatable start-to-success route; failure/restart route; prior milestone regressions; exact candidate receipt/hashes; remaining limitations; no missing required native or unsupported required behavior |

M0 is an acceptance dependency. Researchers may investigate later milestones while M0 is open, but the captain must not close gameplay milestones on a broken evidence workflow.

### Gameplay scenario matrix

Derive concrete inputs, spawn/configuration and expected outcomes from retail. The labels below are test intentions, not invented expected values.

| Scenario | Observe in both programs | Milestone |
|---|---|---|
| S01 cold Level 1 load | Resources/scripts, task order, entity identities, spawn state, relevant failures | M1 |
| S02 world inspection | Object/terrain transforms, materials/textures, clipping/occlusion and representative building views | M1 |
| S03 look and locomotion | Input state, camera state, tick/delta, position/velocity, stance and animation state | M2 |
| S04 ground, jump and collision | Takeoff/landing, ground contact, slopes, wall/building collision and required steps | M2 |
| S05 authored interaction | Actual trigger/task, player state and the required door/ladder/building transition | M2 |
| S06 weapon lifecycle | Equip, trigger events, shot result, ammunition, reload state and animation events | M3 |
| S07 guard encounter | Task/graph state, perception decisions, movement, hits, health, death and side effects | M3 |
| S08 objective progression | Actual objective prerequisites, item/trigger state, progression order and success event | M4 |
| S09 failure and restart | Authored failure/death state, resource/task cleanup and repeatable restart | M4 |
| S10 integrated regression | S01–S09 on one frozen candidate plus representative cross-level regressions | M4 |

Use the same scenario, game directory, assets, config, scripts and reviewed probe for retail and candidate. Where paths require different input events, align semantically equivalent events and explain the mapping. Predetermine numeric tolerances from ABI, measurement precision and retail repeat runs; never loosen them after seeing a candidate mismatch. Screenshots assess rendered output; typed observations assess behavior.

For the existing AGENTS.md cross-level requirement, select 3–5 representative supported levels including Level 1 for regressions of shared systems. This is regression sampling, not an expansion to completing those missions. Recover the actual authored Level 1 objective list before naming or implementing objectives.

## 5. Native definition of done

For every required native in the declared feature closure:

1. Identify the exact retail VA, image hash/base, function boundaries, ABI, inputs, returns, callees, global/state effects and unknowns.
2. Use exactly one selected decompiler: Binary Ninja first; Ghidra only if Binary Ninja is unavailable for that target. Retain tool/version, disassembly/pseudocode and raw provenance. Independently compare Radare2 CFG and angr exact-entry recovery; unresolved required flow blocks structural acceptance.
3. Bind the implementation and canonical reference to the current source hashes. Review semantics, data layout and side effects independently of the CFG score. A score of 0.85 or more alone does not prove correct calculations.
4. Define a reviewed typed runtime contract and probe hash. Specify required input/output/side-effect fields, coverage and any proven absence of effects. Unknown pointer layouts remain unknown.
5. Run meaningful focused behavioral comparisons supported by retail observations. Synthetic validator fixtures test infrastructure and must never become native observations.
6. Compile/link the required implementation into the current x86 candidate; retain its build receipt, compiler inputs and symbols. An object that only compiled as a dependency has no candidate runtime proof.
7. Capture retail first, then the candidate, sequentially. Bind raw observations to source, retail, candidate, probe and scenario hashes. Require actual candidate calls for every required mapped native and compare complete ordered observations.
8. Independently review the result; fix the earliest divergence and repeat affected checks. Keep failures and retractions. Set a status only for the scope actually demonstrated.

A helper failure, missing ABI, unresolved required method, absent call, dropped sequence, stale hash, different probe or failed comparison blocks acceptance. A successful launch, model response, name match, return-only probe or emulated run cannot replace paired runtime evidence for the current game build.

Use the strict feature gate on the complete changed feature scope. After a commit, do not treat an empty `--changed` set as regression acceptance: retain the feature's gate report/address closure and rerun its declared scope explicitly. Shared headers/build changes require the affected native closure, not merely one convenient address.

## 6. Captain, team and shared resources

| Role | Responsibility | Write ownership |
|---|---|---|
| Captain / integrator | Read baseline, choose active slice, own task board, resolve dependencies, integrate reviewed work, assign status and next task | Git/index, shared manifests/config, CMake, candidate build/deployment; may grant the implementation token |
| Static researcher | Recover required retail behavior with the selected decompiler, Radare2 and angr; flag ABI/control-flow unknowns | Return findings or an assigned unique research note; do not overwrite accepted artifacts |
| Task/asset researcher | Decode QVM/config/task and asset references; map Level 1 dependencies and observations | Assigned unique note/packet only |
| Implementer | Implement one assigned native or narrow feature from evidence | Only the granted source/header allowlist while holding the single implementation token |
| Runtime operator | Design/run approved typed scenarios, maintain shared lease and raw trace identities | One assigned capture directory; sole game-capture owner |
| Independent critic | Review implementation, source/binary bindings, semantic assertions, probes and comparisons | Findings only; no implementation edits or self-approval |

The harness can use multiple agents and team members, but this shared checkout has **one implementation writer at a time**. Parallelize read-only research, review and independent analysis. A captain may delegate disjoint notes, but source changes and shared evidence/manifest writes require explicit ownership. Do not create worktrees or let subagents switch branches, stage, commit, rebuild or replace the candidate independently.

Treat the Binary Ninja active view as shared state. One analyst/broker selects the retail view; other agents coordinate before changing view, symbols or types. Use bounded independent Radare2/angr jobs rather than flooding the host. Runtime capture is globally serialized with `frida_capture_lease.py`; CTest's process-local resource lock is an additional guard, not a substitute.

During a paired runtime run, freeze source, manifest, probe, scenario and candidate. No builds, source edits or candidate copies may run until the operator releases the capture lease. Never kill someone else's game to obtain a run. A busy game/server is a task blocker with a retry condition.

## 7. Commands and evidence locations

Run from this checkout with the local working locale:

```sh
cd /Users/haseeb-mir/Documents/Code/project-open-igi
export LC_ALL=C LANG=C PYTHONDONTWRITEBYTECODE=1
git branch --show-current
git rev-parse HEAD
python3 scripts/igi_workflow.py doctor --check
python3 scripts/igi_workflow.py self-check
```

Tool health alone is not native acceptance. `scripts/igi-workflow.json` already names the supplied BN URL; its inherited candidate/build and external catalog paths must be reconciled in task P03 before trusting default verification commands. The existing BN clients send no authorization header. Do not add a key requirement for this local provider.

Capture static analysis for a real selected native (address shown is a known startup example, not the player-loop closure):

```sh
python3 scripts/igi_workflow.py analyze 0x0048F0C0 \
  --report build/native-analysis/0x0048F0C0/capture.json
```

Diagnostic build, under the captain's build ownership:

```sh
./build_no_verify.sh
```

That script disables native gates/tests, builds the candidate and copies it into the game directory after backing up the previous candidate. Label it **UNVERIFIED DIAGNOSTIC**. Preserve retail `igi.exe`. Build before testing changed code; final acceptance/deployment claims require the paired checks.

Configure infrastructure/static checks for a declared pilot (two startup examples only):

```sh
cmake -S . -B build-native-checks \
  -DIGI_VERIFICATION_ONLY=ON -DBUILD_TESTING=ON \
  '-DIGI_NATIVE_TEST_ADDRESSES=0x0048F0C0;0x0048F0D0'
ctest --test-dir build-native-checks -N
ctest --test-dir build-native-checks -L infrastructure --output-on-failure
ctest --test-dir build-native-checks -L static --output-on-failure
```

A normal Windows CMake configuration defaults to compiled source scope. Host verification-only mode requires an explicit valid scope/address list. Reconfigure after source/manifest/script/config changes because CTest pins that scope. Fix unbound required sources and compile-only linkage rather than masking the full scope with the example addresses.

For runtime, configure `IGI_NATIVE_TEST_PROBE_MAP` with a reviewed JSON mapping of retail addresses to probe paths, then run `ctest --test-dir build-native-checks -L runtime --output-on-failure`. A single explicitly reviewed probe can instead use `IGI_NATIVE_TEST_PROBE`. Missing or unreviewed probes block the fixture. The runtime fixture binds one candidate receipt; runtime tests remain serialized. CMake targets `verify-native-static` and `verify-native-runtime` expose the same bridge.

Strict changed-feature gate:

```sh
python3 scripts/native_gate.py --root . --changed \
  --threshold VERIFIED --cfg-threshold 0.85 \
  --require-semantic-evidence --require-behavioral-tests \
  --report build/native-verification.json
```

The reviewed feature's runtime capture/checker must consume that gate report so every required native is mapped and observed. For one fully declared new-native slice, the existing intake command is:

```sh
python3 scripts/igi_workflow.py intake RETAIL_VA \
  --probe REVIEWED_TYPED_PROBE --level 1 --report build/native-intake/report.json
```

`RETAIL_VA` and `REVIEWED_TYPED_PROBE` above are placeholders to replace from the task packet; do not run them literally. Do not use a one-address command to approve a broader change.

Launch retail and candidate with game cwd `/Users/haseeb-mir/Downloads/IGI` and the same `window level1` arguments, using the established CrossOver/Frida procedure in `docs/fidelity/runtime-test-workflow.md`. Run retail first, confirm it has ended, then run the candidate. Neither program should overlap the other or another user's game.

Retain:

- Native registry and reviewed evidence: `docs/fidelity/native-verification.json`, `docs/fidelity/evidence/`.
- Immutable per-run raw traces/build records: unique directories under `build/runtime-traces/` and `build/native-ctest/`; publish stable references to evidence intended for commits.
- Tool-health observations: `build/tool-health.json` and `build/binary-ninja-health-latest.json`.
- Prior diagnostic merge failure: `build/native-gate-diagnostic-report.json` and `build/native-gate-diagnostic.log`.
- Captain's task state: `tasks/todo.md`, with task packets in `tasks/packets/` as needed.

The pre-commit hook normally requires `VERIFIED`. Its explicit `IGI_ALLOW_UNVERIFIED_DIAGNOSTIC_MERGE=1` mode recorded the historical diagnostic integration despite a failed structural gate. That override is never evidence of a native or milestone pass. Do not export it globally or make it the harness's normal acceptance path.

## 8. Full MVP completion checklist

- [ ] M0–M4 are accepted on one identified source snapshot and candidate, with each required native's scoped evidence current.
- [ ] The authored Level 1 success route runs from a cold start to success; failure/death/restart is repeatable.
- [ ] The M1–M3 scenario suite passes on the integrated candidate; required cross-level shared-system regressions are recorded.
- [ ] No required QVM opcode/native/task is silently skipped and no required movement/collision/combat behavior is replaced with guessed constants or fallbacks.
- [ ] Actual rendered output, object transforms and materials have paired observations and captures for the declared route.
- [ ] Performance is measured on a fixed machine/configuration with retail baseline, candidate frame-time distribution and simulation timing. Investigate the current ~7 FPS scene; target the configured 30 FPS budget for the supported profile and report any unmet budget explicitly.
- [ ] Evidence records bind the exact source, compiler inputs, retail/candidate images, probes and scenario; the critic has resolved or explicitly recorded every finding.
- [ ] The completion packet names tested scope and remaining limitations. Native status totals, screenshots alone and infrastructure test counts are not substituted for these criteria.

The captain may progress autonomously between accepted slices. No separate human gameplay approval gate is added by this plan. Pushes, releases and unrelated external integrations are outside this handoff.
