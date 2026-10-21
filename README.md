# Project Open IGI: clean retail-flow branch

This branch starts the Windows x86 reconstruction from a single diagnostic entry file. It does not yet implement retail WinMain, a game window, DirectDraw 7, a level, or gameplay. The placeholder is not a retail transcription or a verified game.

The committed engine from `game-engine-flow` was removed only from this branch. The tracked `scripts/` and `docs/` trees remain as research and verification references. Local untracked assets are not included in this branch. Their historical manifests, candidate paths, build receipts, screenshots, and status labels describe the earlier branch; they do not verify this new baseline. The previous README and build script are retained as `docs/fidelity/README-legacy-game-engine-flow.md` and `scripts/legacy-game-engine-flow-build.sh`.

## Diagnostic build

Run `./build.sh` with CMake and `i686-w64-mingw32-g++` installed. It builds `build-clean-win32/open-igi.exe`, a PE32 placeholder, and does not deploy it or attest any native. The existing `run.sh` and legacy verification flows refer to the earlier engine and must be adapted before use on this branch.

## Next work

Identify the retail entry and MVP call flow using Binary Ninja (Ghidra only as fallback), Radare2 and angr; reconstruct one required native at a time. Bind the new implementation and candidate to fresh semantic, build, and paired retail/OpenIGI runtime evidence before accepting it. An empty changed set or a successful placeholder build is not verification. See `AGENTS.md` and `docs/fidelity/native-feature-verification.md` for the evidence requirements.
