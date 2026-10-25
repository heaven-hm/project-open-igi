# Captain task board — first playable Level 1, then later stages

Branch: `mvp/level1-integration`. Only checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`.

Read [the execution plan](plan.md) and [MVP.md](../MVP.md). The checkboxes below start open. Assign an owner/reviewer and a packet/report before changing state. Use READY / IN_PROGRESS / REVIEW / BLOCKED / ACCEPTED annotations; accepted items need current evidence.

**Hard harness limits:** 3 active agents total (captain + 2 worker slots), maximum 3 concurrent model requests, and a shared 30 model requests per rolling minute including retries. Enforce centrally; nested/team agents consume the same slots and budget.

**Current MVP ends at P17/M2, closed by P25.** Sound, enemies, weapons, full mission objectives and menus/loading/text follow later. Every implemented native needs static and focused original/candidate Frida evidence; publish missing methods and original entry-to-exit flow gaps.

## M0 — baseline and workflow

- [ ] P00 Preserve current Git/index/worktree variants, hashes, running-session ownership and recovery route.
- [ ] P01 Resolve case-only aliases in reviewed batches while preserving all distinct blobs and updating references.
- [ ] P02 Reconcile required evidence with source/images; retract unsupported passes; recompute valid counts.
- [ ] P03 Align tool/build/catalog/scope configuration with the actual checkout and confirm each provider's health.
- [ ] P04 Demonstrate one complete current-source, linked-candidate native comparison with independent review.

## M1 — world and script foundation

- [ ] P05 Recover actual Level 1 dependencies, scripts, assets, entity lifecycle and objectives.
- [ ] P06 Resolve/link required QVM execution and dispatch (`0x004B85B0` candidate).
- [ ] P07 Resolve/link QVM float-argument ABI (`0x004B8A50` candidate).
- [ ] P08 Resolve/link actual Config_Read lifecycle (`0x00405850` candidate).
- [ ] P09 Resolve/link human-player general definitions (`0x00410070` candidate).
- [ ] P10 Resolve/link human-player parameter loading (`0x004137A0` candidate); preserve test retraction.
- [ ] P11 Compare required model/resource/entity placements and materials.
- [ ] P12 Compare Level 1 terrain/texture/material and collision geometry foundation.
- [ ] P13 Integrate and compare task/tick/player spawn lifecycle.

## M2 — player loop

- [ ] P14 Player input and camera.
- [ ] P15 Walk/run/crouch, simulation timing and root motion.
- [ ] P16 Grounding, jump/landing and required world/building collision.
- [ ] P17 One actual authored building interaction, then repeat the player route.

- [ ] P25 First playable acceptance: freeze candidate, replay S01–S05, compare native/call-flow evidence and close only M0–M2.

## M3 — sound after playable MVP

- [ ] P18 Original sound lifecycle, samples/voices/spatial playback and current world/player sound events.

## M4 — enemies after sound

- [ ] P19 Original enemy spawn, task/patrol/graph navigation.
- [ ] P20 Perception/detection/loss/pursuit and associated state/animation; armed behavior pending M5.

## M5 — weapons after enemies

- [ ] P21 Original equip/fire/ammo/reload and shot behavior, with sound/animation callbacks.
- [ ] P22 Hit/damage/health/death and integrated enemy armed encounter.

## M6 — full authored mission after combat

- [ ] P23 Actual objectives/items/triggers and success/failure/cleanup/restart.

## M7 — menus/loading/text last

- [ ] P24 Original main menu, loading bar, loading text and UI/resource flow.
- [ ] Repeat P25 evidence packet for each later stage before accepting it.

## Current connection observation

Binary Ninja MCP handshake/tool listing succeeded at `http://127.0.0.1:24642/mcp` without authorization; the open retail PE `view_1` was activated and a 16-byte sample matched retail. Record: `build/binary-ninja-health-latest.json`. A fresh existing static capture for `0x0048F0C0` also completed with `analysis_collected`, CFG score 1.0 and `verified=false` (`build/native-analysis/bn-reconnected-0048F0C0.json`). These checks resolve the earlier BN connection observation only; P03 is not wholly complete.

## Captain's next update

Record timestamp, active packet, owner/reviewer, source-write token holder, build/runtime owner, accepted scope, exact blocker and next ready task here. Do not start a competing source writer or game capture.

- 2026-10-25 06:48 UTC / Codex / P03–P04 REVIEW: infrastructure self-check 12/12;
  BN, r2, angr, compiler and retail identity healthy. Shared Frida port 27042 timed
  out and the RE-Agent checkout is absent; the verifier's private Frida service
  passed RPC health and cleaned up successfully. Run
  `build/native-auto/runs/20261025T064528Z-c23776a4/0x0048F0D0/attempt-1/`
  passed CFG 1.0, semantic checks and 11/11 controlled paired Frida cases. Natural
  startup capture blocked on compiler-input drift after the build receipt;
  overall pending, no native acceptance. Engine implementation remains with the
  existing writer; this packet changed documentation only. Next acceptance step:
  run the same single-command native verifier during a coordinated stable-source
  window. No game/server from this run remains owned by this packet. Ready MVP
  dependency research: the recorded GOInRemap ABI/output-buffer gap in
  `docs/fidelity/evidence/current-config-20261025/observation.json`.

<!-- progress log (appended by Lead; not a status promotion) -->
- 2026-10-25 / Codex / P08–P14 dependency ANALYZED: retail GOInRemap ABI capture
  completed 81 calls and 243 ordered reader calls, confirming caller-buffer
  ownership and nonzero copied stack words that the candidate incorrectly zeros.
  Evidence and concrete correction requirements:
  `docs/fidelity/evidence/remap-abi-20261025/`. No candidate acceptance or engine
  write; owned runtime resources released. Next research dependency is the script
  dispatcher/caller output layout, before correcting the native and decoder ABI.
- 2026-10-24: P05 flow-closure work is evidenced by build/audit/p05b..p05e packets and the ordered walk in docs/fidelity/main-call-flow.md; retail-vs-candidate evidence lives under build/tester/20261023T220230Z/.
- Linked + retail-verified, candidate UNOBSERVED: startup globals 0x48F120/130/200/210/280/290/2A0/2B0/300, SetAppTitle 0x48F100, SetDebugCompileState 0x48F1C0, QTaskList 0x4C1790/17C0/1800, parser 0x48F360, tokenizer 0x4A5A0C, CLI handlers 0x48F350/920/930/950/960. Current deployed build: 102fe97ef2a6cc3ad846cdfd7fbf80718308d3e1ae08416349a8fccb0a2e9a72.
- Next blocking step: P13 main.cpp transcription + entry switch per the 10-step checklist in docs/fidelity/main-call-flow.md, then Tester six-scenario paired runtime comparison. Known divergence: QTask_RegisterClasses throw-stub suppresses the Level handler's second 0x48F210(0) call.
- 2026-10-24 / Codex sol / REVIEW: corrected the earliest WinMain mutex divergence (missing NULL check and collapsed GetLastError calls). Fresh BN/r2 CFG=1.0, exact angr recovery; isolated diagnostic candidate `1f2290efe0ad6067534da524e32aff9e1763ec0ec0d65edb8f7ff28c547b0c13` passed 12 typed successful-prefix checks with retail. Packet: `docs/fidelity/evidence/main-mutex-20261024-sol/`. Full WinMain, failure branches and independent critic review remain pending; no native status promoted. Build/capture ownership released after both owned processes exited. The shared installed candidate was not replaced by this task.
- 2026-10-24 / Codex sol / ANALYZED: BN/r2 CFG=1.0 and two retail frame-save calls establish the count=0 behavior of 0x004A5000 and buffer 0x0054F8E0. Matching restore instructions at 0x004A507C independently support jump-buffer semantics. Packet: `docs/fidelity/evidence/setjmp-frame-20261024-sol/`. Candidate frame linkage/runtime and nonlocal resume remain open. Continue the ordered WinMain walk; do not accept the whole frame native from these retail-only calls.
- 2026-10-24 / Codex sol / REVIEW: removed startup-comparator exclusions for missing natives, helper substitution and a collapsed GetLastError count. Replay of the real historical green capture now blocks on a missing final summary and records the call mismatches. Packet: `docs/fidelity/evidence/startup-comparator-20261024-sol/`. This is a checker correction, not current native acceptance.
- 2026-10-24 / Codex sol / ANALYZED: next required pre-window native 0x00498480 has fresh BN/r2 CFG=1.0 (38 blocks) and exact angr recovery. Retail typed import/COM capture observed 48 ordered events, seven local interface identities, capability transitions 0x100/200/300/500/600/601/700 and output (0x700,2), EAX=1. Required DirectMusic COM initialization is part of this path despite deferred gameplay sound. Packet: `docs/fidelity/evidence/directx-probe-20261024-sol/`. No engine native added; candidate transcription/paired capture, error coverage and independent semantic critique remain open. Runtime lease released after owned-process cleanup.
