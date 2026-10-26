# Project Open IGI — Playable Level 1 MVP and decompilation roadmap

## 1. Goal and authority

**Current MVP: load Level 1 with its original 3D models, terrain and textures, spawn the player, and make the original player camera, controls, movement, physics and required collision/interactions work.** Launch directly into the level through a retail-supported path. Menus, loading bars, loading text, sound, enemies and weapons are not prerequisites for this first playable delivery.

The later roadmap is explicitly ordered: **playable world/player → sound → enemies → weapons → mission completion → main menu, loading presentation and text**. Sound is a later stage, not permanently excluded. Enemies precede weapons: first validate their spawn, patrol, navigation and perception; integrate a complete armed encounter only once the weapon stage is reached. Full mission success/failure remains a later deliverable rather than the acceptance definition of the initial MVP.

This is **decompilation reconstruction of retail IGI1**, following its original game methods and flow. The target is the original algorithms, data layouts, ABI, calculations, quirks, calls and state transitions recovered from the executable. Independent replacement gameplay, guessed FPS physics, convenient rewritten algorithms and source-port behavior are not acceptable evidence of decompilation fidelity. The implementation is Windows x86 C++20 `igi1.exe`, with original local assets and DirectDraw 7 / Direct3D 7.

- Branch: `mvp/level1-integration`.
- Only implementation checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`.
- Integration baseline: local merge `730ce9bfd4209954758f1644776f4a60829def15`; obtain the current HEAD again before work.
- Create no new worktree. Existing agent worktrees and recovery refs are retained as references.
- Retail authority: `/Users/haseeb-mir/Downloads/IGI/igi.exe`.
- Retail SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
- Candidate build: `build-win32-no-verify-gate/igi1.exe`.
- Local diagnostic copy: `/Users/haseeb-mir/Downloads/IGI/igi1.exe`.
- CrossOver is the local execution environment; the product target remains Windows x86.

Read `AGENTS.md` for technical and evidence rules. This owner-requested staged scope supersedes both the earlier immediate-full-mission MVP and the older inspection-only scope in sections 10.10–10.12 and historical roadmaps. Its single-decompiler, ABI, evidence, runtime sequencing and source-preservation rules continue to apply. Complete the player milestone before promoting AI/combat into the active implementation slice.

Execution plan: [tasks/plan.md](tasks/plan.md). Work checklist: [tasks/todo.md](tasks/todo.md). Ready-to-paste captain instructions: [DEEPSEEK_HANDOFF.md](DEEPSEEK_HANDOFF.md). The prior mixed roadmap/audit is preserved in [the historical document](docs/fidelity/MVP-before-captain-plan.md).

## 2. Scope

| Stage | What to build | What is not required to accept that stage |
|---|---|---|
| **First playable MVP (M0–M2)** | Reliable native verification; retail direct-level startup; Level 1 models, terrain, textures/placements; required scripts/tasks; player spawn/camera, walk/run/crouch, jump/landing, physics/collision, one actual required interaction | Sound, enemies, weapons, mission completion, main menu, loading bar/text and decorative UI |
| M3 — Sound | Original sound subsystem and currently reachable world/player sound events | Weapon-specific events before the weapon stage |
| M4 — Enemies | Authored enemy spawn, patrol/navigation, perception and relevant animation/state behavior | A complete armed combat encounter before weapon support exists |
| M5 — Weapons | Required weapon equip/fire/reload/ammunition, hit/damage/health/death and armed encounters; integrate corresponding sounds/animations | Menus/loading presentation/text |
| M6 — Mission completion | Authored objective/item/trigger progression to success, failure and repeatable restart | Presentation polish |
| M7 — Presentation | Original main menu, loading bar, loading text and other required text/UI flow | Additional missions, new platforms or renderer modernization |

Movies, vehicles, decorative smoke, save/load UI and unrelated tools remain outside this roadmap unless an actual required dependency is demonstrated. An omitted presentation feature must be bypassed by the same supported branch of the original flow, not by silently skipping a required native or replacing it with a no-op. Required initialization/state must still be reconstructed even if its optional visual presentation is deferred. Developer free camera is explicit diagnostic tooling and cannot substitute for the player camera.

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
| M0 — Verification prerequisites within the first delivery | Preserved source variants, unambiguous active paths, correct local config, current x86 candidate and working native checks | Source/native/link map; independent static evidence; a complete paired native proof pilot; stale/missing/empty inputs block. Bound this work to what is needed for the first playable delivery. |
| M1 — Level 1 world and flow | Original models, terrain, textures and placements load through the supported direct-level entry/QVM/task path | Startup-to-spawn call-flow map, task/resource/transform/material comparisons and actual-window captures. No menu/loading UI requirement. |
| M2 — **Accept the first playable MVP here** | Player spawn/camera/controls, walk/run/crouch, jump/land, original physics and required terrain/building collision/interaction | Paired input, tick, position, velocity, stance/contact and interaction observations; every required implemented native has static and focused Frida runtime evidence. Repeat direct launch and the playable route. |
| M3 — Sound | Original sound behavior for the supported world/player events | Per-native static/runtime checks, event order, sample/resource identity, voice/playback/spatial parameters and supported audio output observations. |
| M4 — Enemies | Required enemy spawn/task/graph navigation, patrol and perception | Paired entity/graph/state/animation transitions for the selected repeatable encounter. Weapon-dependent behavior stays pending until M5. |
| M5 — Weapons and combat | Required equip/fire/reload/ammo, hit/damage/health/death and integrated armed encounters | Typed paired observations of weapon/state/events/effects, corresponding sounds and animation callbacks; cover the required variants. |
| M6 — Mission completion | Actual objectives, items and triggers lead to success; failure/restart works | Cold-to-success route and failure/restart route on one candidate, preserving prior stage checks. |
| M7 — Menus/loading/text | Original menu and loading presentation/text follow the recovered UI flow | Resource/string identities, screen/state transitions, input actions, draw/progress state and real rendered captures compared with retail. |

M0 is verification groundwork inside the first delivery, not permission to implement all future systems before the player works. Researchers may map later dependencies early. Implementation proceeds in the order above; only a demonstrated required native dependency may pull a narrow initialization step forward, and that does not accept its later feature.

### Gameplay scenario matrix

Derive concrete inputs, spawn/configuration and expected outcomes from retail. The labels below are test intentions, not invented expected values.

| Scenario | Observe in both programs | Stage |
|---|---|---|
| S01 direct cold Level 1 load | Entry branch, resources/scripts, task/native order, object identities and spawn state | M1 |
| S02 world inspection | Models/terrain, transforms, original textures/materials, representative buildings | M1 |
| S03 controls and locomotion | Input/camera, simulation tick/time, position/velocity, stance and animation | M2 |
| S04 physics and collision | Jump/fall/landing, grounding/slopes, walls/buildings and required steps | M2 |
| S05 authored interaction/repeat launch | Actual trigger/task/player state and interaction; startup/cleanup/relaunch | M2 |
| S06 sound | Actual world/player sound events, sample/voice/playback/spatial state and output | M3 |
| S07 enemy behavior | Authored task/graph movement, patrol, perception and state transitions | M4 |
| S08 weapon/armed encounter | Equip/fire/reload/ammo, hit/health/death, sound/animation and enemy effects | M5 |
| S09 mission route | Actual objective prerequisites, item/trigger state, success, failure/restart | M6 |
| S10 menu/loading/text | Original UI entry branches, strings/resources, progress/draw/state and user actions | M7 |

Recover concrete inputs and expected values from retail. Use the same scenario, game directory, assets, config, scripts and focused typed probe for both programs. Align semantically equivalent events; compare deterministic call order and state transitions, not arbitrary wall-clock scheduling. Predetermine numeric tolerances from ABI, precision and retail repeats; do not loosen them after observing a candidate mismatch. Exact integer/state observations must compare exactly; tolerance-based float checks must be explicitly identified rather than described as byte-exact.

For existing AGENTS.md cross-level regression requirements, select 3–5 supported representative levels including Level 1 for the shared subsystem under test. This does not expand the first delivery to implementing those missions. Recover actual Level 1 objectives for M6; do not invent them during M2.

### Original entry-to-exit method flow

Maintain the original game's reachable call graph from the **actual retail entry point**, through startup, configuration/resources, direct-level/task/player initialization, frame/event dispatch and shutdown/error paths. Do not assume the source's host `main` is the retail entry point. Separate platform/compiler startup scaffolding from reconstructed game methods.

For each supported scenario record: retail VA → candidate symbol/RVA, ABI/data layout, ordered calls and returns, branch conditions, indirect dispatch targets, required globals/effects and cleanup. Record parent/child call relationships and simulation/event ordering. Include all required callees and transitively required helpers. Unknown required branches or indirect targets block acceptance. Compare early startup using probes installed before those calls; a late attach cannot prove them.

The direct-level route must correspond to an observed retail route with the same supported launch/config branch. Deferred UI/audio/enemy/weapon branches remain explicitly uncovered until their stages. The final goal is matching the full recovered game flow, but a partial-stage capture cannot be reported as proof that every main-to-last-method path in the whole game matches.

## 5. Every implemented native: static and focused runtime verification

**Coverage requirement: 100% of the declared implemented native set must have both static and original-versus-candidate runtime evidence before that set is described as fully verified.** Publish the denominator and every missing native. This includes implemented helper/native callees, not just high-level entry functions. Keep a registry of all linked native implementations as well as the current supported-path closure; untested off-route linked methods stay visible and provisional. Do not claim the entire implementation is 100% checked while those remain untested, or hide them by changing labels.

Each native gets a small, focused, reviewed **Frida/IGI-Probe typed probe** for its actual inputs, return, affected state/memory and relevant calls/side effects. A shared capture may compose those probes only if every native's individual contract and complete observations remain available. At least one hit or a matching EAX return is insufficient. Exercise the known branches, boundaries, failure cases and stateful behavior supported by static analysis and retail observations; unexercised paths remain an explicit coverage gap. Complete native-set coverage is not mathematical proof of all possible executions.

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

**Hard limits: three active agents total — one captain plus two worker slots — and at most three simultaneous model calls.** All team members and nested subagents count toward the same limit; rotate the roles below through those slots. Four concurrent agents are not permitted.

**One shared cap of 30 model API request attempts per rolling 60 seconds** covers captain, workers, subagents and retries together. Reserve capacity in a central queue before each attempt, pace aggregate dispatches at least 2.1 seconds apart, enforce the rolling window, and respect Retry-After with central backoff on 429. No per-agent 30 RPM budgets or independent retry loops. The captain must configure or verify these limits in the real harness; instructions in Markdown alone do not enforce them. Native tool calls retain their own safety/resource locks.


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

## 8. First playable MVP completion checklist

- [ ] M0–M2 pass on an identified source snapshot and current x86 candidate; later stage work is not required to close this MVP.
- [ ] Retail-supported direct Level 1 startup loads the actual 3D models, terrain, original textures/materials and authored placements without requiring menu, loading bar or loading text implementation.
- [ ] Player spawn, camera, controls, walk/run/crouch, jump/fall/landing and required world/building collision/interactions work through the original game methods, parameters and task/tick flow.
- [ ] S01–S05 repeat on the same candidate with current paired observations and rendered captures. Required shared-system regressions are recorded.
- [ ] Every required implemented native/helper has independent static/ABI/semantic review and its focused typed Frida comparison against retail; implemented/linked/observed/accepted denominators and uncovered paths are explicit.
- [ ] Entry-to-spawn, frame/event and cleanup/error call-flow maps preserve recovered native calls/order/branches/state effects for the supported route. No required method is silently skipped, stubbed or replaced with guessed gameplay.
- [ ] Build receipts bind source/compiler/image/probe/scenario hashes. The independent critic has reviewed implementations and evidence; failures remain visible.
- [ ] Frame timing and simulation behavior are measured against retail on a fixed machine/configuration. Investigate the observed ~7 FPS diagnostic scene and report any unmet configured 30 FPS target.
- [ ] The completion packet clearly says **first playable Level 1 MVP**; sound, enemies, weapons, mission completion and UI each retain their own later-stage status.

After accepting this checklist, proceed to M3 sound, M4 enemies, M5 weapons/combat, M6 full authored mission and M7 menus/loading/text in that order. Each later stage repeats the same native evidence and flow requirements. Byte-identical source or executables are not established by decompiler output, recompilation, a CFG score or finite Frida scenarios; do not claim byte matching without an actual binary comparison and a declared matching criterion.

The captain may progress autonomously between accepted slices. No separate human gameplay approval gate is added. Pushes/releases and unrelated external integrations remain outside this handoff.
