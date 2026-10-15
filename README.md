# Project Open I.G.I.

A native macOS reimplementation of the Project I.G.I. game engine, built in
C++20. The first implementation milestone focuses on gameplay fidelity:
locomotion, physics, player and weapon motion, animation integration, and
gameplay audio. Original game data is loaded from a user-provided game folder.

## Current state

The repository currently contains a small, buildable gameplay library with a
49-state human locomotion graph and initial reverse-engineered movement
equations. It is not yet a complete game engine and does not yet produce
`igi.app`. The graph is cross-checked against OpenIGI source; the movement
equations are recorded against static REA evidence in `engine/VERIFICATION.md`.
Runtime parity against the original game remains to be measured.

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

1. Match player movement, locomotion state transitions, and physics.
2. Match weapon selection, aiming, firing, recoil, and reload timing.
3. Integrate original animation and sound assets through native systems.
4. Compare recorded input and state traces against `igi.exe` under CrossOver.
5. Add presentation systems after gameplay behavior has evidence-backed parity.
