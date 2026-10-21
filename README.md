# Project Open IGI: clean retail-flow branch

This branch starts the Windows x86 reconstruction from a minimal diagnostic Win32 window and debugger logger. It does not yet implement retail WinMain, a game window, DirectDraw 7, a level, or gameplay. The placeholder is not a retail transcription or a verified game.

The committed engine from `game-engine-flow` was removed only from this branch. The tracked `scripts/` and `docs/` trees remain as research and verification references. Local untracked assets are not included in this branch. Their historical manifests, candidate paths, build receipts, screenshots, and status labels describe the earlier branch; they do not verify this new baseline. The previous README and build script are retained as `docs/fidelity/README-legacy-game-engine-flow.md` and `scripts/legacy-game-engine-flow-build.sh`.

## Diagnostic build

Run `./build.sh --diagnostic` with CMake and `i686-w64-mingw32-g++` installed. The diagnostic command builds `build-clean-win32/igi1.exe`, a PE32 diagnostic shell, and does not deploy it or attest any native. The existing `run.sh` and legacy verification flows refer to the earlier engine and must be adapted before use on this branch.

## Next work

Identify the retail entry and MVP call flow using Binary Ninja (Ghidra only as fallback), Radare2 and angr; reconstruct one required native at a time. Bind the new implementation and candidate to fresh semantic, build, and paired retail/OpenIGI runtime evidence before accepting it. An empty changed set or a successful placeholder build is not verification. See `AGENTS.md` and `docs/fidelity/native-feature-verification.md` for the evidence requirements.

## Workflow commands

- `python3 scripts/igi_workflow.py doctor` lists local tools and reference paths; it does not test MCP connections.
- `python3 scripts/igi_workflow.py native 0x00405850` finds historical records and GameNatives candidates.
- `python3 scripts/igi_workflow.py static 0x00405850` runs the strict per-address native gate.
- `python3 scripts/igi_workflow.py verify 0x00405850 --level 1` invokes the existing paired-verification workflow. Its old candidate paths may need adaptation; missing data is a blocker.

The debugger logger uses Windows `OutputDebugStringW`; it is diagnostic infrastructure, not retail behavior. The MVP is the retail startup flow required to load 3D models, MEF/ILFF, textures, terrain, QVM configuration, player movement and collision. Follow `AGENTS.md` for the tool selection and per-native evidence requirements.

## Current inspection increment

The requested first scene is Level 1 positioned 3D models with original textures and the previous developer free camera/mouse look, named `igi1.exe`. Terrain rendering and human physics are later increments. The executable currently remains a diagnostic window; the model scene is not implemented on this branch.

Run `python3 scripts/igi_workflow.py doctor --check` for actual tool checks, `python3 scripts/igi_workflow.py analyze 0x0048F530` to collect the retail main entry analysis, and `python3 scripts/igi_workflow.py self-check` for verification-system regression checks. The latter uses labeled validator fixtures, never game observations. Configure local paths in `scripts/igi-workflow.json`; `IGI_ANGR_PYTHON` and `FRIDA_HOST` override their settings.

`verify` builds and binds the current x86 candidate before the strict VERIFIED native gate and typed paired runtime comparison. A reviewed typed gameplay probe is required via the manifest's `verification.runtime_probe.path` or the underlying verifier's `--probe`. Missing implementations and evidence block acceptance. `--static-only` remains partial and never prints `True`.

The v4 catalog was copied as historical inventory from the existing scope-gate worktree; it does not confer verification status. RE-Agent's optional Frida 17.23.2 JavaScript module was installed in the shared local `/Users/haseeb-mir/Documents/Code/project-open-igi/.tools/re-agent-frida-17.23.2` tooling directory and linked into its `node_modules/frida` without source/lockfile changes. RE-Agent and upstream REA remain distinct. The repair was confirmed through an actual MCP process-list call.

Default `./build.sh [--probe path --level N]` builds a diagnostic candidate first, binds its compiler inputs, then runs `native_gate.py --changed --threshold VERIFIED --cfg-threshold 0.85 --require-semantic-evidence --require-behavioral-tests` and the scoped paired runtime checker. It blocks empty scopes. It never deploys. `./build.sh --diagnostic` is the explicit shell-only build command.

`python3 scripts/igi_workflow.py smoke` spawns the diagnostic candidate suspended, injects a small architecture/module probe, and terminates only that owned process before main resumes. This checks runtime plumbing without executing game code or disturbing another agent’s session. It never verifies a native.
