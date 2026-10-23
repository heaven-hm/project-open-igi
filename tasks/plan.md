# Execution plan — Level 1 MVP on mvp/level1-integration

## Operating contract

[MVP.md](../MVP.md) defines product scope, milestones and evidence requirements. This file defines execution order. [todo.md](todo.md) is the live task state. All tasks begin open; baseline source presence is not task completion.

Only checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`. One captain owns Git/index, shared configuration/manifests/build and grants one implementation token. Preserve existing worker changes; create no worktree. Other agents research or review concurrently and return packets. The runtime operator owns the global capture lease. No source/probe/candidate changes during a paired capture.

This is a plan to implement and validate the mission; task completion requires the evidence specified below. Do not create artificial expected gameplay outputs just to write a test. Derive fixtures and comparison expectations from recorded retail behavior.

## Dependency order

```text
P00 preservation
  -> P01 path normalization -> P02 evidence reconciliation
  -> P03 tool/build/scope alignment -> P04 complete verification pilot
                                      |
P05 Level 1 closure research ----------+
  -> P06–P10 required QVM/config/player linkage
  -> P11–P13 scene/task/tick/spawn foundation
  -> P14–P17 player controls/movement/collision/interaction
  -> P18–P21 weapon/damage/AI/animation encounter
  -> P22 authored objectives -> P23 failure/restart
  -> P24 integrated mission/performance/regressions -> P25 final evidence packet
```

P05 and static research for later tasks may run in parallel with M0. Source implementation follows dependencies. Addresses below are known candidates to investigate, not claims that their bodies or identities are accepted.

## Task packet and completion rules

Before editing, the captain creates a packet with:

```yaml
task_id: Pxx-slice-N
owner: assigned-agent
reviewer: different-agent
milestone: M0|M1|M2|M3|M4
baseline_commit: actual-current-HEAD
baseline_worktree_inputs: exact-source-and-manifest-hashes
behavior: one observable outcome
retail_addresses: exact-declared-address-list
required_dependency_closure: observed-and-statically-justified-dependencies
allowed_files: explicit-source/header-allowlist
source_write_token: required-before-editing
selected_decompiler: binary_ninja-or-recorded-target-fallback
scenario_and_probe: reviewed-paths-and-hashes-or-explicitly-not-ready
acceptance: observable-pass-conditions
verification_command_and_report: actual-command-and-unique-output
blocking_dependencies: explicit-list
```

If a task requires more than about five implementation files or several independent native behaviors, split it before editing. Each packet is one reviewable increment. The captain serializes shared headers and manifest/config edits and calculates their affected closure. Source changes do not become accepted because a later task needs them.

The common verification requirement for native tasks P06–P23 is the native definition of done in MVP.md: static/ABI/semantic/behavioral checks, current linked x86 candidate receipt, typed sequential retail/candidate comparison and independent review. The last column below adds the task-specific observations. Static-only research returns facts/unknowns and remains open for implementation acceptance.

## M0 — reliable checkout and verification

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P00 / captain | Inventory branch, staged/unstaged/untracked state, existing game processes, current builds and recovery refs. Preserve index/worktree/case-variant blobs and SHA identities without modifying other worktrees. Record a recovery manifest in a unique audit directory. | None | Every variant has a recoverable identity; exact allowed dirty set and rollback route recorded. Audit 273 evidence differences individually; do not assume they are fresh edits. |
| P01 / captain + path researcher + critic | Resolve case-only path aliases in bounded batches. Use Git tree/blob data, not only the case-insensitive filesystem. Handle identical aliases first; preserve distinct contents under unambiguous archive names with original-path/blob maps. Update references together. | P00 | No active-path alias can overwrite another required file. Full-tree collision audit documents remaining archives/exclusions; required sources/evidence resolve unambiguously. Independent critic verifies no distinct artifact/source variant was lost. Do not mass-stage alias paths. |
| P02 / evidence researcher + captain | Reconcile `docs/fidelity/native-verification.json` and referenced artifacts after P01. Retain detailed raw static evidence, failures/retractions and actual provenance. Reopen unsupported semantic promotions. Verify file hashes, source hashes, retail/candidate IDs and scope. | P01 | Every active required record resolves; current-source/current-candidate counts are computed reproducibly. Stale/missing artifacts are explicitly blocked. No status is promoted by replacing hashes alone. Partition evidence batches by task rather than rewriting all 885 statuses. |
| P03 / workflow implementer | Reconcile `scripts/igi-workflow.json`, candidate/build defaults, reference catalog and CMake selection with this checkout. BN URL already correct; auth is disabled. Audit consumers such as `bn_mcp_call.py` rather than assuming the config controls all clients. Fix genuine path/config drift only. | P00; final scope after P01/P02 | Actual doctor checks report each provider separately; selected BN PE bytes match retail; correct configured target builds. Full compiled inventory explains 144 linked/5 compile-only/28 unbound baseline and resolves or blocks each needed item. Missing scope, tools or probes remain failures. |
| P04 / implementer + runtime operator + critic | Choose one required, small native with understood ABI/effects and a reviewed probe. Run the complete existing verification pipeline against the fresh candidate. Candidate source must be linked. Use `native_ctest.py`, existing semantic/behavioral gate and paired comparator. | P02/P03 | One actual native passes the entire declared contract, with retail and candidate observations, source/build/probe bindings and independent review. Negative controls show stale hash, missing candidate call, missing probe and empty scope fail. Existing validator fixtures are separate. |

**M0 checkpoint:** the captain closes M0 only when the workflow can distinguish a real native comparison pass from the diagnostic merge/build. The connected BN endpoint fixes the former reachability blocker, not the remaining evidence/linkage/runtime issues.

P01 must preserve variants before any deletion/rename. It may canonicalize paths within this existing checkout; a new worktree or a new filesystem is not the default plan. If platform constraints prevent safe materialization, retain the Git blob map and report the exact blocker rather than silently overwriting a variant.

## M1 — world, scripts and required startup closure

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P05 / task/asset researcher | Read actual Level 1 task/object scripts, config and `humanplayer/humanplayer.qvm`. Map assets, objectives, entity lifecycle, required native calls, dispatch and unresolved dependencies. Consult converter/editor/OpenIGI references for discovery with revision/license provenance. | P00; research may run early | A reviewed Level 1 closure and actual objective list with source/retail observations. Unknown opcodes/calls explicit. Do not guess objective names or count all catalog entries as required. |
| P06 / QVM implementer | Resolve `0x004B85B0` QVM execution candidate, its required opcode/callee/state dependencies and linkage. Likely `engine/src/retail/script/qvm_execute.cpp`, its header and explicit CMake link inputs. | P04/P05 | Required script dispatch executes in candidate; instruction/stack/state transitions agree for the selected script path. No silent unsupported-opcode skip. Receipt/link map proves body inclusion. |
| P07 / QVM implementer | Resolve `0x004B8A50` float-argument candidate and typed argument/return boundary. Likely QVM stack native source/header. | P06 or an independently justified call boundary | Paired relevant argument decoding/stack effects; required source linked. Calling convention and float representation supported. |
| P08 / configuration implementer | Resolve `0x00405850` Config_Read candidate and actual loader flow. Locate current source from manifest rather than guessing a filename; align required config/global lifecycle. | P05/P06/P07 where required | Actual Level 1 config values and load order reach consumers, with correct failure behavior. Receipt proves this is not a compile-only dependency. |
| P09 / player-configuration implementer | Resolve `0x00410070` general human-player definition candidate in `humanplayer_defines.cpp/.hpp`. | P08 | Observed player parameters/state transferred through actual QVM/native ABI, including required defaults/overrides supported by retail. |
| P10 / player-configuration implementer | Resolve `0x004137A0` human-player parameter loading candidate and known evidence retraction. Preserve explicit not-run findings until compared. | P09 | Player parameter data layout/assignments and effects compared; source linked; rejected historical report cannot count as runtime evidence. |
| P11 / scene implementer | Confirm required model/resource/object loading and authored transforms; likely `local_models_mef.cpp`, associated model data and narrow scene wiring. | P05 and required loader closure | S01/S02 paired entity/transform/material observations; missing resources fail explicitly. Avoid heuristic placement substitutions. |
| P12 / terrain implementer | Close required Level 1 terrain/texture/material decoding and geometry/collision agreement, in bounded terrain/texture source packets. | P11 and required resource closure | S02 checks of representative terrain, material identity/coordinates and building alignment; screenshots and typed data agree for declared scope. |
| P13 / lifecycle implementer | Integrate actual task/tick/entity/player spawn lifecycle through `core/flow.cpp`, `core/frame_natives.cpp` and narrowly owned scene call sites. Split shared code changes. | P06–P12 required closure | S01 observes task ordering, stable spawn, required tick calls and entity state. A rendered scene does not substitute for executing the task lifecycle. |

**M1 checkpoint:** Level 1 loads through the supported retail-derived path, required objects/materials/terrain are present in authored positions, config is live and required startup calls are mapped. The exact required closure is versioned with its evidence.

## M2 — player loop, the next gameplay deliverable

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P14 / player implementer | Bind retail input/controls and player camera; candidate sites include config input bindings, `player_controller.cpp/.hpp` and narrow scene input wiring. Split camera and dispatch if independent. | P13/P09/P10 | S03 compares input, camera orientation/state and tick behavior. Diagnostic free camera is excluded from player acceptance. |
| P15 / locomotion implementer | Walk/run/crouch, actual movement parameters, animation/root-motion effects and simulation timing. | P14 | S03 paired positions, velocities, stance and animation state for supported inputs; no guessed speed fallback or swallowed motion error. |
| P16 / collision implementer | Grounding, jump, fall/landing, walls/slopes/building collision and required stairs. Split into separate contact/jump/building packets. | P12/P15 | S04 ground/contact and state transitions match retail within declared measured tolerances; compare building geometry as well as terrain. |
| P17 / interaction implementer | Implement one actual authored building interaction from P05: task trigger, player state and required door/ladder/other transition. | P16 and actual task dependency | S05 completes the same interaction in retail and candidate; missing required calls/state remain blocked. Repeat movement checks around the interaction. |

**M2 checkpoint:** cold launch -> player spawn -> look -> walk/run/crouch -> jump/land -> collide -> authored interaction works on one frozen candidate. Repeat the route; capture actual output. Keep combat outside the active implementation slice until this checkpoint passes.

## M3 — the mission's combat dependency closure

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P18 / weapon implementer | One mission-required weapon's equip/fire/ammo/reload and observable shot behavior. Locate exact native implementations and ABI using P05 before assigning files. | M2; required inventory/task lifecycle | S06 compares ammo, shot events, reload/equip state and effects. Start with one weapon, then enumerate remaining required Level 1 variants. |
| P19 / damage implementer | Required hit resolution, health, damage/death state and effects. Separate player/enemy paths where necessary. | P18; collision/entity state | S07 known hit/miss cases and resulting state/side effects agree; no health/ammo log placeholder counts as behavior. |
| P20 / AI implementer | One required guard's authored task/graph navigation, perception, detection/loss and combat state. Split graph, perception and encounter state into packets. | M2; P19 for integrated encounter | S07 compares authored state/graph decisions and transitions for a repeatable encounter; then cover remaining required actor variants. Rendering guards or decoding a graph alone cannot close this task. |
| P21 / animation/task implementer | Synchronize required gameplay animation events/root motion with locomotion, weapon, damage/death and task callbacks; complete the first encounter, then remaining required variants. | P18–P20 | S06/S07 event order and entity state match on one candidate; no visual-only animation acceptance or duplicate task events. |

**M3 checkpoint:** a repeatable guard encounter works with the player's actual controller, required weapon/ammo/damage/death and synchronized task/animation behavior. Required Level 1 actor/weapon variants have a declared tested closure.

## M4 — mission acceptance

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P22 / mission-task implementer | Implement actual objective prerequisites, mission items, triggers, progression and success state recovered in P05. Split per authored transition. | M3 and required interaction closure | S08 follows the real mission route and compares objective/task state and success event; no synthetic completion toggle. |
| P23 / lifecycle implementer | Failure/death and repeatable restart/cleanup using the supported diagnostic launch or authored path; menus remain deferred. | P19/P22 | S09 reproduces failure, releases required task/resources and restarts with correct state. No residual state falsely completes objectives. |
| P24 / captain + runtime operator + performance researcher | Freeze a candidate and run S01–S10, including cold-to-success and failure/restart; measure frame timings and simulation rates with a fixed retail baseline. Run 3–5 supported representative levels for shared-system regressions. Split profiling from implementation fixes. | P22/P23 | Exact successful mission route plus failure route, integrated native closure, paired evidence/captures and timing measurements. Fix earliest divergence and rerun affected routes. Report unmet configured 30 FPS target. Other level missions need not be implemented. |
| P25 / captain + independent critic | Assemble accepted source/tree ID, build receipt/image hashes, scenario/probe identities, scope/native matrix, raw artifacts, review outcomes and remaining limitations. Update MVP/task statuses. | P24 | Every MVP checklist item has a current evidence reference; unsupported statuses remain open. Full Level 1 MVP may be declared complete only for the tested scope with the actual authored success/failure paths working. |

## Scheduling and blocker policy

1. Captain grants a task to the implementation writer only when its dependencies and packet are ready. Give other members read-only dependency research, ABI/probe design or review tasks.
2. Assign the independent critic before implementation starts; it must not be the code author or manufacture missing observations.
3. Acquire build ownership before changing CMake/config or replacing `igi1.exe`. Freeze tree/candidate/probe/scenario and transfer ownership to the runtime operator for capture.
4. Run retail then candidate under the shared lease. Validate receipt and hashes before and after. A busy or unavailable game is a blocker; preserve the active process.
5. Accept only the complete declared feature closure. A one-address pilot cannot approve a multi-native shared-file edit.
6. After each packet, record accepted scope or the exact earliest divergence. On the same unchanged failure twice, stop identical reruns, open the missing dependency and choose another ready research task. New inputs or an actual repair justify retrying.
7. Keep queues explicit: READY, IN_PROGRESS, REVIEW, BLOCKED, ACCEPTED. BLOCKED has an owner and a concrete unblock condition; ACCEPTED has a report and reviewer.
8. No blanket source/evidence staging, forced cleanup, gate weakening or globally exported diagnostic override. Preserve unreviewed variants for later reconciliation. Local commits are small; no push/release is authorized by this plan.

## Task completion report

```text
Task / owner / independent reviewer:
Baseline revision + source/manifest fingerprints:
Files and exact native closure changed:
Retail facts and unresolved assumptions:
Probe/scenario and candidate receipt/hash:
Commands + report/raw-artifact paths:
Static / semantic / behavioral / paired-runtime outcome:
Earliest mismatch or blocker:
Accepted scope (or BLOCKED/INCONCLUSIVE):
Next ready task / unblock condition:
```

The captain reports progress by accepted milestones and scenarios, with native counts as supporting provenance. More model calls, a growing status counter or repeated successful compilation are not substitutes for a working mission.
