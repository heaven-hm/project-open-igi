# Execution plan — first playable Level 1, then staged decompilation

## Operating contract

[MVP.md](../MVP.md) defines product scope, milestones and evidence requirements. This file defines execution order. [todo.md](todo.md) is the live task state. All tasks begin open; baseline source presence is not task completion. **The current playable MVP ends at P17/M2.** P18 onward is the ordered later roadmap, not prerequisites for the first delivery.

Only checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`. One captain owns Git/index, shared configuration/manifests/build and grants one implementation token. Preserve existing worker changes; create no worktree. Other agents research or review concurrently and return packets. The runtime operator owns the global capture lease. No source/probe/candidate changes during a paired capture.

This is a plan to implement and validate the mission; task completion requires the evidence specified below. Do not create artificial expected gameplay outputs just to write a test. Derive fixtures and comparison expectations from recorded retail behavior.

## Dependency order

```text
P00 preservation -> P01 path normalization -> P02 evidence reconciliation
                 -> P03 tool/build/scope alignment -> P04 native proof pilot
P05 required direct-Level1 closure research -----------+
  -> P06–P10 required QVM/config/player linkage
  -> P11–P13 models/terrain/textures/task/tick/spawn foundation
  -> P14–P17 player controls/physics/collision/interaction
       = FIRST PLAYABLE MVP ACCEPTANCE (M0–M2; P25 evidence packet)
  -> P18 sound (M3)
  -> P19–P20 enemies: spawn/patrol/navigation/perception (M4)
  -> P21–P22 weapons/damage/armed encounter (M5)
  -> P23 authored mission success/failure/restart (M6)
  -> P24 main menu/loading bar/loading text (M7)
P25 = repeat the evidence/completion packet for each accepted stage
```

P05 and static research for later tasks may run in parallel with M0. Source implementation follows dependencies. Addresses below are known candidates to investigate, not claims that their bodies or identities are accepted.

## Task packet and completion rules

Before editing, the captain creates a packet with:

```yaml
task_id: Pxx-slice-N
owner: assigned-agent
reviewer: different-agent
milestone: M0|M1|M2|M3|M4|M5|M6|M7
baseline_commit: actual-current-HEAD
baseline_worktree_inputs: exact-source-and-manifest-hashes
behavior: one observable outcome
retail_call_flow: entry/parent/child/return/branch/cleanup mapping
implemented_set_and_coverage: explicit-linked-required-static-runtime-counts
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

The common verification requirement for native tasks P06–P24 is the native definition of done in MVP.md: static/ABI/semantic/behavioral checks, current linked x86 candidate receipt, typed sequential retail/candidate comparison and independent review. The last column below adds the task-specific observations. Static-only research returns facts/unknowns and remains open for implementation acceptance.

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
| P05 / task/asset researcher | Read actual Level 1 task/object scripts, config and `humanplayer/humanplayer.qvm`. First map the minimum retail-supported direct-level world/player route, assets, entity lifecycle, required native calls, dispatch and unknowns. Record later objective/enemy/weapon dependencies as research without making their complete gameplay prerequisites for the first delivery. Consult converter/editor/OpenIGI references for discovery with revision/license provenance. | P00; research may run early | A reviewed first-delivery closure and entry-to-exit/call-order map with source/retail observations; keep later mission-objective research separate. Unknown opcodes/calls explicit. Do not guess objective names or count all catalog entries as required. |
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

## M3 — sound after the playable MVP

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P18 / sound implementer | Recover original sound initialization, resource/sample handling, voice lifecycle and reachable world/player audio events. Select source/native addresses from retail; split init, playback and spatial/event slices. | Accepted M2 | S06 compares required per-native arguments, sample identity, play/stop/loop state, spatial parameters and event ordering; retain actual audio observations where supported. No synthetic sound behavior. Weapon sound events are integrated later with P21/P22. |

## M4 — enemies after sound

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P19 / enemy implementer | Recover required enemy spawn/task lifecycle and authored patrol/graph navigation. First select exact VAs/ABIs from the actual mission. | P18 | S07 compares spawn/task/entity state, graph choices, positions and movement/event order. A rendered guard or parsed graph alone does not close it. |
| P20 / enemy implementer | Recover perception, detection/loss/pursuit and associated animation/state transitions. Split independently observable native paths. | P19 | S07 focused probes compare original state decisions and effects. Weapon-dependent firing/damage behavior remains explicitly pending for M5; do not implement the whole weapon system early to close enemy perception. |

## M5 — weapons and combat after enemies

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P21 / weapon implementer | One mission-required weapon's equip/fire/ammo/reload and original shot behavior; then remaining required variants. Recover the original native flow and animation/sound callbacks. | P20 | S08 compares equip/ammo/reload state, actual shot events and required effects with per-native static/runtime evidence. No guessed replacement shooting algorithm. |
| P22 / damage/encounter implementer | Recover hit resolution, health/damage/death, remaining enemy armed states, animation/task callbacks and associated sound events. Split by native behavior. | P21 | S08 compares hit/miss/state/effect/event ordering and a repeatable armed encounter on one candidate. Then cover the required actor/weapon variants. |

## M6 — authored mission after combat

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P23 / mission-task implementer | Recover actual objective/item/trigger prerequisites and success; then authored failure/death/cleanup and repeatable restart. Split per transition. | P22 | S09 follows the actual authored route to success and failure/restart with typed state/call observations. No synthetic mission-completion toggle. No menus needed to accept the supported direct-launch mission route. |

## M7 — main menu, loading presentation and text last

| Task / owner | Work and likely files | Dependencies | Completion / verification |
|---|---|---|---|
| P24 / UI decompilation implementer | Recover original main-menu branch/input/state flow, loading progress presentation, font/string/resource lookup and text draw behavior. Split into distinct original-native packets. | P23 | S10 compares original UI state transitions, resources/strings, progress/draw observables and rendered output with focused native probes. No custom replacement menu or invented progress bar. Earlier required initialization was already reconstructed when needed, without claiming this presentation stage complete. |

## P25 — acceptance packet for the current stage

| Task / owner | Work | Dependencies | Completion / verification |
|---|---|---|---|
| P25 / captain + independent critic | Freeze the current source/candidate, replay the stage's scenarios, relevant earlier-stage regressions, frame/simulation measurements and representative shared-system cross-level checks. Assemble source/tree/build receipt, image/probe/scenario identities, native/call-flow/coverage matrix, raw evidence and review findings. Split profiling fixes into owned source packets. | **P17 for the first playable MVP**; repeat after P18, P20, P22, P23 and P24 for the later stages | For first delivery, S01–S05 and M0–M2 pass; later systems stay open. For later stages, add only their declared scenarios and regressions. Every claimed implemented native has static and focused paired Frida evidence; untested linked methods and unexercised branches remain explicit. No universal byte-identical claim from finite tests. |

## Mandatory decompilation and flow rules

Preserve the original retail method bodies' recovered semantics, ABI, memory/data layout, integer/float behavior, branches, callees, event/tick order, side effects and error/cleanup paths. This is decompilation reconstruction; existing ports and generated pseudocode are references/hypotheses, never a substitute for the retail machine instructions and observations.

Create a retail VA to candidate symbol/RVA map from actual retail entry through the supported route and exit. Record call parents/children, deterministic order, branch conditions and indirect dispatch. Use early hooks for startup and per-native focused typed Frida probes for actual inputs/returns/changed state/effects. Retain raw traces and compare the first divergence. Every required implemented helper participates, not only named top-level functions.

Publish separate denominators for all linked implementations, current-path required implementations, static-checked, runtime-observed and accepted natives. The 100% requirement is complete static+runtime evidence for the declared implemented set; do not report 100% for the whole implementation while linked native methods remain untested. A CFG threshold or a single successful hit cannot establish exact behavior across all paths.

The original supported direct-Level1 branch should avoid deferred UI presentation if retail does so. Never skip an unavoidable original native call or add a no-op to force that shortcut. If an enemy/audio/weapon-related initialization method is genuinely required before player spawn, reconstruct and verify that narrow dependency now; record why it is required, while its later gameplay stage remains open. This preserves actual call flow without turning the first delivery into every feature.

## Scheduling and blocker policy

**Hard harness limits:** at most **3 active agents total: 1 captain + 2 worker slots**. Researcher, implementer, reviewer and runtime-operator are roles rotated through those slots, not permission to start one agent for every role. Team members and nested subagents consume the same slots; queue additional work. Maximum simultaneous model requests is 3, never 4.

The provider limit is **30 model API request attempts per rolling 60 seconds shared by the entire harness**, including captain, workers, nested delegation and retries. One shared limiter must reserve capacity immediately before each attempt; never give each agent a separate 30 RPM allowance. Maintain a global request queue and in-flight count. A pacing interval of at least 2.1 seconds between aggregate dispatches stays below the 30 RPM ceiling; also enforce the rolling-window cap. On 429 respect Retry-After and apply central backoff/jitter; retries consume capacity. No worker-local immediate retry loop. Tool requests have their own tool health/locks and do not grant extra model request capacity. This document specifies these limits; the captain must configure or verify them in the actual harness before launching workers.


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
Static / semantic / behavioral / per-native paired-Frida outcome:
Implemented/linked/required/static/runtime/accepted counts and missing VAs:
Entry-to-exit flow/call-order comparison and uncovered branches:
Earliest mismatch or blocker:
Accepted scope (or BLOCKED/INCONCLUSIVE):
Next ready task / unblock condition:
```

The captain reports first-delivery progress by accepted M0–M2 scenarios, then later-stage progress in the declared order. Native counts support provenance. More model calls, growing status counters and compilation alone do not complete the playable MVP or any later feature.
