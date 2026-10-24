# MVP in small stacked tasks, in the order the game itself runs them

The owner's rule for this stage: complete one small task at a time, 100% tested - static evidence **and** runtime evidence - take a
screenshot of the result, record it in `CHANGELOG.md`, and only then start the next task. Each task builds on the previous one and the
previous one must stay working.

Chronological native order is generated, not guessed: `scripts/flow_order.py` walks retail `igi.exe` from its PE entry point in **call
order** (the order the call sites appear in each caller), expands compiler/CRT code without listing it, and marks each function with the
implementation the manifest records. Output: `docs/fidelity/evidence/chronological-native-order.md`. That is the list to work through so no
method in the flow is skipped.

## Task list

| Task | Deliverable | Done when |
| --- | --- | --- |
| **T0** | Retail startup chain `0x00402930` transcribed and the candidate boots into the level-load flow | candidate runs past `0x00402930`, reaches the level-load call and stays alive |
| **T1** | 3D objects of Level 1 loaded from original assets (MEF/ILFF models, original textures bound) | the objects are visible in the candidate and a screenshot is attached |
| **T2** | Level 1 terrain with its original textures, placed under the objects already loaded | terrain visible with correct textures in the same screenshot set |
| **T3** | HumanPlayer: spawn, camera, mouse look, walk/run/crouch, jump, gravity and ground collision | the player moves through the level under the retail camera and physics |
| **T4** | Weapons: equip, fire, reload, ammunition, hit/damage | a weapon fires and damages through the retail path |
| **T5** | AI enemies: spawn, patrol, navigation, perception | an enemy patrols and reacts |
| **T6** | Sound: original sound subsystem and world/player events | sound plays for the events the earlier tasks produce |
| **T7** | Main menu / loading presentation | the menu starts the level |

T0 is not a detour: every later task's natives are reached through that startup sequence, which is why the candidate currently fails closed
there. The task is therefore split and executed in the retail call order from that sequence, not by rewriting the guard.

## Acceptance for every task

1. **Static**: each involved native has retail VA, exact provenance block, control-flow similarity >= 0.85 recorded by
   `scripts/build_native_records.py`, and no invented logic/helpers.
2. **Runtime**: paired retail/candidate Frida evidence for the natives the task changes, sequential runs, same level argument, hashes bound.
3. **Visual**: a screenshot of the candidate showing the task's deliverable.
4. **Changelog**: an entry under `CHANGELOG.md` stating what was added and the evidence paths.

## Task order note

T0 is stretchy - it is the 155-call startup sequence plus closures. It is worked top-to-bottom in
`docs/fidelity/evidence/chronological-native-order.md`, a few natives per round, with the gate run after each landing.
