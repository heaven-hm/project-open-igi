# Project Open IGI: clean retail-flow branch

This branch starts the Windows x86 reconstruction from a minimal diagnostic Win32 window and debugger logger. It does not yet implement retail WinMain, a game window, DirectDraw 7, a level, or gameplay. The placeholder is not a retail transcription or a verified game.

The committed engine from `game-engine-flow` was removed only from this branch. The tracked `scripts/` and `docs/` trees remain as research and verification references. Local untracked assets are not included in this branch. Their historical manifests, candidate paths, build receipts, screenshots, and status labels describe the earlier branch; they do not verify this new baseline. The previous README and build script are retained as `docs/fidelity/README-legacy-game-engine-flow.md` and `scripts/legacy-game-engine-flow-build.sh`.

## Diagnostic build

Run `./build.sh` with CMake and `i686-w64-mingw32-g++` installed. It builds `build-clean-win32/open-igi.exe`, a PE32 diagnostic shell, and does not deploy it or attest any native. The existing `run.sh` and legacy verification flows refer to the earlier engine and must be adapted before use on this branch.

## Next work

Identify the retail entry and MVP call flow using Binary Ninja (Ghidra only as fallback), Radare2 and angr; reconstruct one required native at a time. Bind the new implementation and candidate to fresh semantic, build, and paired retail/OpenIGI runtime evidence before accepting it. An empty changed set or a successful placeholder build is not verification. See `AGENTS.md` and `docs/fidelity/native-feature-verification.md` for the evidence requirements.

## Workflow commands

- `python3 scripts/igi_workflow.py doctor` lists local tools and reference paths; it does not test MCP connections.
- `python3 scripts/igi_workflow.py native 0x00405850` finds historical records and GameNatives candidates.
- `python3 scripts/igi_workflow.py static 0x00405850` runs the strict per-address native gate.
- `python3 scripts/igi_workflow.py verify 0x00405850 --level 1` invokes the existing paired-verification workflow. Its old candidate paths may need adaptation; missing data is a blocker.

The debugger logger uses Windows `OutputDebugStringW`; it is diagnostic infrastructure, not retail behavior. The MVP is the retail startup flow required to load 3D models, MEF/ILFF, textures, terrain, QVM configuration, player movement and collision. Follow `AGENTS.md` for the tool selection and per-native evidence requirements.
