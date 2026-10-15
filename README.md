# Project Open I.G.I.

A native macOS reimplementation of the Project I.G.I. game engine, built in
C++20. The first implementation milestone focuses on player locomotion,
physics, weapons, and enemy AI. Original game data is loaded from the retail
game folder.

## Current state

The repository currently contains a buildable gameplay library with a
49-state human locomotion graph, initial movement equations, and a
converter-backed retail asset catalog probe. It is not yet a playable game and
does not yet produce `igi.app`. See [the first gameplay demo plan](GAMEPLAY_DEMO_PLAN.md)
and [verification notes](engine/VERIFICATION.md). Runtime parity against the
original game remains to be measured.

## Build

Requirements: CMake 3.20+ and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
```

The core is C++20 because the platform, renderer, and gameplay systems need a
native ABI and deterministic low-level control. C and Rust can be used for
specific components when their ABI or performance benefits are concrete; the
current gameplay core is C++.

## Reverse-engineering rules

`RE.md` defines the evidence and confidence requirements for analysis. Treat
related-game symbols as clues, never as IGI 1 ground truth. Keep observations,
inferences, and unknowns separate. A function is not runtime-verified merely
because it exists in a catalog or decompiles cleanly.

The `assets/` and `scratch/` trees are retained as local research material from
the source checkout. Do not add retail executables, licensed game data, or
large generated scratch artifacts to Git commits.

## Gameplay priority

1. Match player movement, locomotion transitions, and collision physics.
2. Match weapon selection, aiming, firing, recoil, and reload behavior.
3. Match Level 1 enemy sensing, orders, patrol, movement, and combat.
4. Compare recorded input and state traces against `igi.exe` under CrossOver.
5. Implement the original fixed-function rendering behavior and compare frames.

Main menu, audio, animation polish, and app packaging are deferred until after
the first gameplay demo.
