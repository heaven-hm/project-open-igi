# Captain task board — Level 1 MVP

Branch: `mvp/level1-integration`. Only checkout: `/Users/haseeb-mir/Documents/Code/project-open-igi`.

Read [the execution plan](plan.md) and [MVP.md](../MVP.md). The checkboxes below start open. Assign an owner/reviewer and a packet/report before changing state. Use READY / IN_PROGRESS / REVIEW / BLOCKED / ACCEPTED annotations; accepted items need current evidence.

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

## M3 — required combat

- [ ] P18 Mission-required weapon equip/fire/ammo/reload, then remaining required variants.
- [ ] P19 Hit/damage/health/death and effects.
- [ ] P20 Required guard task/navigation/perception/combat.
- [ ] P21 Animation/task event synchronization and integrated encounter coverage.

## M4 — playable mission

- [ ] P22 Actual authored objectives, items/triggers and success transition.
- [ ] P23 Failure/death/cleanup and repeatable restart.
- [ ] P24 Full route, regressions and retail/candidate frame-time/simulation measurements.
- [ ] P25 Independent review of final source/build/scenario/native evidence packet.

## Current connection observation

Binary Ninja MCP handshake/tool listing succeeded at `http://127.0.0.1:24642/mcp` without authorization; the open retail PE `view_1` was activated and a 16-byte sample matched retail. Record: `build/binary-ninja-health-latest.json`. A fresh existing static capture for `0x0048F0C0` also completed with `analysis_collected`, CFG score 1.0 and `verified=false` (`build/native-analysis/bn-reconnected-0048F0C0.json`). These checks resolve the earlier BN connection observation only; P03 is not wholly complete.

## Captain's next update

Record timestamp, active packet, owner/reviewer, source-write token holder, build/runtime owner, accepted scope, exact blocker and next ready task here. Do not start a competing source writer or game capture.
