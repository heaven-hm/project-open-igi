# Project Open I.G.I.

Project Open I.G.I. is a native integration of the original Project I.G.I. game engine. Engine routines are ported from reverse-engineering evidence taken directly from the retail `igi.exe`, with each routine retaining its retail name, virtual address, calling convention, and evidence references where those are known.

This repository is the game-engine implementation. It is not an editor, converter, helper library, or a gameplay rewrite based on another project. The retail executable is the behavior authority; disassembly, decompilation, traces, and runtime captures are used to establish and verify each integration. Unknown behavior remains explicitly unresolved rather than guessed.

## Current status

The codebase is being rebuilt incrementally around verified retail routines. The current slice includes a Windows x86 DirectDraw 7 bootstrap and focused resource/MEF loading routines with tests. It is not yet a playable Level 1 implementation, and no full-game parity claim is made.

The current MVP excludes audio, movies and cutscenes, vehicles, smoke effects, and the main menu. Gameplay integration proceeds from the retail execution and loading paths, with each subsystem added and verified in small commits.

## Build

The game target is a 32-bit Windows executable using the original DirectX 7 / DirectDraw platform APIs.

Requirements: CMake 3.20+, a C++20 compiler, and the Windows SDK or MinGW toolchain with DirectDraw 7 headers and libraries.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Reverse-engineering workflow

All behavioral implementation claims must trace back to `igi.exe`. Static analysis identifies candidate routines and data layouts; runtime tracing and memory inspection confirm behavior where possible; focused tests record the verified conditions and known limits. A catalog entry or clean decompilation alone is not runtime proof.

Tools used for analysis and verification:

- [Ghidra](https://ghidra-sre.org/) — disassembly, decompilation, and data-structure analysis.
- [Binary Ninja](https://binary.ninja/) — independent disassembly, decompilation, and control-flow analysis.
- [radare2](https://radare2.com/) — independent disassembly, control-flow, and binary inspection.
- [Frida](https://frida.re/) — runtime hooks, call tracing, and memory inspection.
- [RE-Agent](https://github.com/heaven-hm/re-agent) — orchestration of reverse-engineering analysis against the retail executable.

Per-routine evidence and implementation limits stay attached to the native source and its analysis records. Tool output is evidence; it is not substituted for the native engine implementation.

## Source layout

- `engine/include/igi/retail/` — declarations for integrated retail routines.
- `engine/src/retail/` — routine implementations, named and annotated with their retail addresses.
- `engine/tests/` — focused tests for individual implementation slices.
- `engine/REVERSE_ENGINEERING.md` — provenance, evidence, and unresolved behavior.

## Windows runtime

The retail executable and this project target 32-bit Windows x86 and DirectX 7 / DirectDraw. The game is intended to run as a normal Windows game with its original assets. Keep Windows APIs in the Windows platform layer; shared engine routines retain the retail behavior established from `igi.exe`.
