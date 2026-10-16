# Project Open I.G.I.

**Version 0.1 ALPHA**

An open-source native implementation of the Project I.G.I. game engine, built
in C++20 for all major desktop operating systems — Windows, macOS, and Linux.
This is not a from-scratch re-implementation in the style of the C#
[OpenIGI](https://github.com/OpenIGI/OpenIGI) project: original game data is
loaded from the retail game folder and the runtime reproduces the original game
byte-for-byte. The first implementation milestone focuses on player
locomotion, physics, weapons, and enemy AI.

## Current state

The repository builds a C++20 gameplay library, a fixed-step application loop,
asset and native diagnostic probes, and a C++ SDL2 application shell packaged
as `igi.app` on macOS (native binaries on Windows and Linux). The shell opens a window and runs a provisional 60 Hz loop;
player control, level rendering, collision queries, weapons, and AI are not
integrated yet, so it is not a playable game. See [the first gameplay demo plan](GAMEPLAY_DEMO_PLAN.md)
and [verification notes](engine/VERIFICATION.md). Runtime parity against the
original game remains to be measured.

## Screenshots

Level spawn renders captured from the engine (1280x960):

![Level 1 spawn](screenshots/screenshot_01_level1_spawn.png)

![Level 2 spawn](screenshots/screenshot_02_level2_spawn.png)

![Level 3 spawn](screenshots/screenshot_03_level3_spawn.png)

## Derived from

This project builds on earlier open-source projects:

- [project-igi-editor](https://github.com/heaven-hm/project-igi-editor) — 3D
  level editor for Project I.G.I. 1 (renderer attachment transforms, format
  and level knowledge).
- [project-igi-converter](https://github.com/heaven-hm/project-igi-converter) —
  IGI1Conv/IGI2Conv asset converter (the runtime format parsers).
- [OpenIGI](https://github.com/OpenIGI/OpenIGI) — C# reference implementation,
  used as a cross-check reference only; this project is a native,
  byte-for-byte game reproduction, not a re-implementation.
- [Project-IGI-Terrain](https://github.com/hjcminus/Project-IGI-Terrain) —
  the Terrain Editor by hjcminus that project-igi-editor is based on.

Retained upstream license and provenance notices live in
`engine/third_party/licenses/`.

## Platform scope

The game core is designed to be OS-independent: gameplay, physics, AI, weapons,
scripting, and file formats must not call operating-system APIs. Windowing,
graphics, input, filesystem, and audio APIs belong behind platform interfaces.
The project targets all major desktop operating systems — Windows, macOS, and
Linux — and is not tied to any single platform.

## Branch and release policy

`develop` is the integration branch for experimental work. Version tags such as
`v0.1.0-alpha.1`, `v0.1.0-beta.1`, `v0.1.0-rc.1`, or
`v0.1.0-experimental.1` create GitHub prereleases only when their commit is part
of `develop`. Stable version tags such as `v0.1.0` create GitHub releases only
when their commit is part of `main`. Feature branches are not release sources.

The release workflow currently publishes source snapshots only. This repository
does not yet build a playable game; its current `igi.app` is only a window and
fixed-step-loop shell, so releases must not claim to contain a playable macOS port. See [release policy](docs/RELEASE_POLICY.md).

## Build

Requirements: CMake 3.20+, a C++20 compiler, SDL2, and either an SDL2 CMake
package or pkg-config metadata.

```sh
cmake -S . -B build
cmake --build build
```

The core is C++20 because the platform, renderer, and gameplay systems need a
native ABI and deterministic low-level control. C and Rust can be used for
specific components when their ABI or performance benefits are concrete; the
current gameplay core is C++.

## Reverse engineering

The original game behavior was established through reverse engineering of the
retail `igi.exe` (PE32, Innerloop Studios, 2000). Tools used:

- **Binary Ninja** (plus its MCP service) — disassembly, decompilation, and the
  verified native-function catalogs.
- **Ghidra** — decompilation and independent cross-checks of control flow,
  constants, and object layouts.
- **Radare2** — independent disassembly and CFG cross-checks.
- **REA / re-agent** — automated reverse-engineering agent for function,
  call-site, and data-structure analysis.
- **CrossOver** — runs the retail `igi.exe` for reference captures
  (`igi.exe window level1`).
- **Frida-style instrumentation, debuggers, and memory inspection** — used when
  installed and available for runtime evidence.
- **Cross-checked format documentation** from
  [igipy](https://github.com/artiom-rotari/igipy) and the derived projects
  listed above.

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

## License

MIT — see [LICENSE](LICENSE). Copyright © 2026 Heaven HM. Retained
third-party license and provenance notices live in
`engine/third_party/licenses/`. Game assets remain subject to their
respective owners' rights.
