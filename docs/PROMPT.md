# Project I.G.I. Native Engine — Full Working Prompt

> **Current owner decision:** Work on `mvp/level1-integration` in the existing main checkout without creating worktrees. Follow the updated [MVP.md](MVP.md): full Level 1 gameplay, implemented through M0–M4; the next gameplay slice is M2 player movement/interactions after the baseline. Older branch/worktree instructions below are historical.

> **Audit 2026-10-23:** This brief contains stale sound requirements, manual-review language and implementation status. Current scope is [MVP.md](MVP.md); current workflow rules are [AGENTS.md](AGENTS.md). Read the [dated gap/workflow audit](MVP.md#workflow-audit-2026-10-23). Audio is deferred; the scene now renders models/terrain diagnostically, while playable mission acceptance remains open.


This document consolidates the project owner's instructions and the current working context for future contributors and coding agents. It is the project brief, not proof that any described feature or analysis is complete. Confirm current source, build output, tool availability, binary identity, and runtime evidence before making claims about status.

## 1. Main objective

Build a native Windows x86 open-igi.exe for the original Project I.G.I. using the owner's local game installation:

- Game assets and retail executable: /Users/haseeb-mir/Downloads/IGI/
- Retail behavior authority: /Users/haseeb-mir/Downloads/IGI/igi.exe
- Current implementation branch: game-engine-impl

The goal is to implement Level 1 engine behavior from the retail executable, using C++20 and the game's DirectDraw 7 / Direct3D 7 platform path. C# open-igi is a candidate source for implementation details only; verify every behavior used against igi.exe through REA/static analysis and runtime observations where possible. CrossOver on this Mac is only the local Windows runtime test environment.

The complete Level 1 MVP includes models and terrain loaded at verified placements with original textures, QVM/task flow, player movement and physics, AI and graph navigation, health/damage, weapons and combat, gameplay animations/effects, interactions; the sound engine remains deferred. Its strict order and acceptance gates are maintained in MVP.md and plan.md. Do not substitute a similar-looking game or claim parity without corresponding evidence. Use the original game assets locally and do not redistribute them.

## 2. Priorities and prototype scope

Follow the canonical sequence in MVP.md and plan.md:

1. Build and verify the Windows x86 DirectDraw 7 baseline.
2. Execute the verified Level 1 OBJECTS.QVM task path and render all Level 1 3D models at retail positions, orientations, and rotations.
3. Load and render Level 1 terrain with original textures.
4. Verify all object transforms and model/terrain texture bindings, capture screenshots, and hand the visual build to the owner for manual review.
5. Integrate the required QVM/task runtime and retail simulation tick.
6. Add player, physics, collision, building/stair/door interactions, AI and graph navigation, health/damage, weapons, gameplay animations/effects, mission flow; keep sound deferred.
7. Run the integrated Level 1 mission, compare it against retail, and keep every unverified difference explicit.

Every increment needs focused tests, timestamped logs, a Windows x86 build, and CrossOver runtime verification when applicable. Capture the actual game window for visual increments. Make small local commits after gates pass; do not push unless explicitly requested.

## 3. Language and platform choices

This branch targets Windows x86 only. The product executable is a normal Windows game; CrossOver on macOS is used only for local testing.

- Use C++20 for this implementation. Add C or Rust only when a concrete, verified subsystem benefit justifies the extra language boundary.
- Use the retail DirectDraw 7 / Direct3D 7 path on Windows. Keep Win32/DirectX calls in the Windows platform boundary; retail engine logic remains traceable to original native behavior.
- Do not add SDL, OpenGL, Vulkan, macOS APIs, or a macOS application target to this Windows-only branch.
- Preserve x86 calling conventions, float behavior, and data widths where evidence requires them. The retail executable is PE32 x86.
- Any future macOS/Linux port is a separate later effort and must not alter the current Windows vanilla target.

## 4. Source projects and research references

Use these local projects as references when relevant. Inspect their current state before relying on details:

- Main working repository: `/Users/haseeb-mir/Documents/Code/project-open-igi`
- Reverse-engineering catalog and C prototype reference: `/Users/haseeb-mir/Documents/Code/project-igi-natives`
- Open-source macOS port reference: `/Users/haseeb-mir/Documents/Code/open-igi`
  - The owner says this project has a `develop` branch and/or `macos-build` work. Use it for architectural ideas and platform integration. It is known by the owner to match only part of the retail game; do not copy behavior uncritically or inherit its known fidelity gaps.
- Game asset format converter: `/Users/haseeb-mir/Documents/Code/project-igi-converter`
- Converter format documentation: `/Users/haseeb-mir/Documents/Code/project-igi-converter/docs`
- Converter utility: `/Users/haseeb-mir/Documents/Code/project-igi-converter/igi1conv` (check the actual path/build and platform before use)
- REA / re-agent checkout: `/Users/haseeb-mir/Documents/Code/rea`
- Requested REA upstream: `https://github.com/heaven-hm/re-agent.git`
- Binary Ninja exports and catalogs: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/`
- Verified native catalog requested by the owner: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/IGINatives_VERIFIED.json`
- Current C prototype workspace: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/cproj/`

The repository also contains native catalogs, exports, and prior research under `assets/`. Treat catalog entries as leads or static evidence, not as implementations of whole engine subsystems. A large number of native functions does not mean the game engine has been ported.

### Primary native implementation source — owner update 2026-10-16

Use `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/`
as a primary source of existing decompiled gameplay bodies. Inspect the relevant
`.c`, `.h` and `.md` exports before inventing a replacement algorithm. Reuse those
bodies and the existing `cproj/src/` translations wherever applicable; adapt them
into readable portable C++ under the corresponding engine subsystem. Preserve
source addresses and provenance in fidelity notes. The game project remains
`/Users/haseeb-mir/Documents/Code/project-open-igi`.

Some exports contain decompiler syntax, inferred pointer types, unresolved globals
or misleading semantic names. Normalize those against original instructions and
object layouts before compiling; do not treat compiler acceptance as fidelity.
Reuse converter/editor parsers and the C# vanilla implementation where needed,
then verify against retail rather than inheriting candidate behavior blindly.

For each increment: add focused regressions, check invalid-input/error handling,
keep timestamped diagnostics, compile and run, perform applicable static/runtime/
memory comparisons, and save a small local commit. Report exactly what the owner
can manually test in the delivered build and what remains unverified.

Current acceptance target is the complete Level 1 playable MVP in MVP.md and plan.md. Sound is deferred for this MVP. Movies/cutscenes, subtitles, vehicles, smoke and menu presentation remain outside this playable-demo scope unless a Level 1 dependency is proven; gameplay animation remains required.

## 5. Reverse-engineering and evidence workflow

Use static, runtime, memory, and verification analysis where available and safe for this owned game copy. Make evidence reproducible and record the target executable's identity (path, architecture, size, and SHA-256) with analysis results.

### Static analysis

- Use REA/re-agent, Ghidra, Radare2, and Binary Ninja to inspect functions, call sites, control flow, strings, data references, object layouts, constants, and calling conventions.
- Prefer corroboration across disassembly, decompilation, callers/callees, and data structures. A decompiler's guessed names or types are hypotheses until checked.
- Record evidence identifiers, addresses, tool versions, query/command details, and confidence or unresolved ambiguity in research notes.
- When additional natives are discovered, update the requested verified catalog and relevant Binary Ninja method names, variables, and identifiers. Preserve provenance and do not label a function verified solely because it exists in the binary.

### Runtime and memory analysis

- The original executable may be launched using the installed CrossOver environment when that environment is functioning. The owner's known launch form is `igi.exe window level1`; fullscreen has known Direct3D issues under CrossOver. Confirm the current CrossOver launch command and prefix before use.
- Collect comparable in-game state for the retail executable and native prototype: player position, velocity/acceleration, flags, collision results, animation state, weapon state, AI state, and per-tick transitions as applicable.
- Use Frida-like instrumentation, REA runtime capabilities, debuggers, or memory inspection tools only when actually installed and usable. Report the exact tool and invocation used; never state that runtime or memory analysis happened when it did not.
- Keep runtime sampling controlled and repeatable. Identify the mission, starting state, input sequence, frame/tick timing, and build used.

### Verification

Separate evidence levels clearly:

1. **Static existence/signature evidence**: function and call-site facts from the binary.
2. **Build/syntax evidence**: source compiles and links for the stated target.
3. **Unit or synthetic harness evidence**: isolated behavior with mocked callbacks and synthetic memory.
4. **Runtime integration evidence**: actual native build runs through a real level and engine providers.
5. **Behavioral parity evidence**: controlled retail-versus-port comparisons for equivalent inputs and states.

Never describe a compile, symbol export, mock harness, or static analysis as proof of gameplay parity. For parity, specify measured values, tolerances, sample size, and known platform floating-point differences. Validate failures, invalid pointers, missing assets, malformed files, and unavailable providers as well as successful paths.

## 6. Tool locations and current-session inventory

Known local tool locations (availability may change; re-check with `command -v`, version commands, and project configuration):

| Tool | Location / endpoint | Intended use |
|---|---|---|
| REA/re-agent checkout | `/Users/haseeb-mir/Documents/Code/rea` | Static/runtime reverse-engineering workflow; use the checkout's documented runner and commands. |
| REA upstream | `https://github.com/heaven-hm/re-agent.git` | Source/project reference; do not assume network or a fresh pull is available. |
| Binary Ninja MCP | `http://127.0.0.1:24642/mcp` | Binary Ninja analysis and symbol/identifier updates; authorization was specified as disabled. Confirm the MCP service is live before use. |
| Radare2 | `/opt/homebrew/bin/radare2`, `/opt/homebrew/bin/r2` | Independent disassembly and CFG cross-checks. |
| Ghidra launcher | `/opt/homebrew/bin/ghidraRun` | Ghidra GUI/analysis; determine headless installation path from local setup when needed. |
| CMake | `/opt/homebrew/bin/cmake` | Configure and build native C/C++ projects. |
| LLVM Clang | `/opt/homebrew/opt/llvm/bin/clang` (also `/opt/homebrew/bin/clang`) | C compiler and strict syntax checks. |
| LLVM Clang++ | `/opt/homebrew/opt/llvm/bin/clang++` | C++ compiler. |
| GCC command | `/usr/bin/gcc` | macOS toolchain alias; check its actual compiler/version before describing it as GNU GCC. |
| Rust compiler | `/opt/homebrew/bin/rustc` | Optional Rust component; use only when justified. |
| Cargo | `/opt/homebrew/bin/cargo` | Rust build tool. |
| CrossOver | `/Applications/CrossOver.app` | Runs the retail Windows executable for reference analysis, subject to a functioning bottle/Wine server. |
| Game binary | `/Users/haseeb-mir/Downloads/IGI/igi.exe` | Retail reference; verify digest before collecting evidence. |
| Game assets | `/Users/haseeb-mir/Downloads/IGI/` | Local assets used by the original game and eventually the native app. |
| `gh-axi` | `~/.local/bin/gh-axi` | Preferred ergonomic wrapper for GitHub CLI operations when needed. Never push unless the owner explicitly requests it. |

This table lists tools and endpoints known in the current working context; it is not a claim that every tool was invoked during every task. At the start of each substantial task, record which tools were actually used and their versions. Do not expose credentials, authorization tokens, private memory, or unrelated local files in logs or this document.

## 7. Current repository and engineering status

Repository: /Users/haseeb-mir/Documents/Code/project-open-igi (branch game-engine-impl).

The target is a Windows x86 C++20 executable using DirectDraw 7 / Direct3D 7. The Windows diagnostic scene now loads and draws textured models and terrain, with placement data decoded from the original scripts and partial character/input/host movement paths. This is not complete execution of the retail QVM/task lifecycle or an accepted playable mission. Motion can be disabled following a root-motion/update exception; AI/combat, health/ammunition and mission completion remain open. See MVP.md#workflow-audit-2026-10-23 for the inspected source, evidence gaps and two-worker workflow.

REA currently has the retail PE32 x86 target /Users/haseeb-mir/Downloads/IGI/igi.exe open with SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; re-check the digest before relying on target identity. The current inspection reports 3,926 procedures and 2,444 strings. This proves analysis availability, not behavior parity.

The working tree may contain user changes and uncommitted subsystem work. Inspect git status before every edit, do not revert unrelated changes, and do not stage files outside the focused task. Keep commits small and local; do not push unless explicitly requested.

## 8. Immediate engineering direction

Use MVP.md and plan.md as the single authoritative Level 1 sequence.

1. First build and verify the Windows x86 DirectDraw 7 executable.
2. Execute the verified Level 1 OBJECTS.QVM path and render its 3D models at the original object transforms.
3. Add the real Level 1 terrain and original textures.
4. Reconcile every object's placement/material/texture against retail logs and runtime observations; capture the game window and obtain owner manual review.
5. Only after the visual gate, integrate simulation/QVM tasks, player movement and physics, world interactions, AI/graph navigation, health/damage, weapons, gameplay animation/effects, and sound.
6. For every slice, retain focused tests, logs, build identity, CrossOver run evidence, screenshot when visible, and explicit unresolved differences.

Use REA as the retail static-analysis source and runtime tooling/igi-probe for observations. Use C# open-igi only to locate candidate logic, then verify the candidate against igi.exe. Never treat generated catalog names, C# behavior, or synthetic tests as proof of retail behavior.

## Branch and release policy

- `develop` is the experimental integration branch. Use prerelease tags such as `v0.1.0-alpha.1`, `v0.1.0-beta.1`, `v0.1.0-rc.1`, or `v0.1.0-experimental.1` only for commits reachable from `develop`.
- `main` is the stable release branch. Use plain semantic version tags such as `v0.1.0` only for commits reachable from `main`. The repository currently has `main`; do not create or use `master` as a parallel stable branch.
- Feature branches are not release sources. Never publish a stable release from `develop` or an experimental release from `main`.
- Current GitHub releases are source snapshots only. A non-playable `igi.app` shell exists; do not claim a playable game until gameplay is implemented and verified.
- Do not push, tag, publish, or merge unless the owner explicitly asks for that operation.

## 9. Coding and maintenance standards requested by the owner

- Use full, descriptive names for variables, functions, methods, types, files, and identifiers. Prefer `int index = 0;` over `int i = 0;`; avoid unexplained abbreviations and one-letter names except conventional mathematical notation with clear context.
- Format code consistently with spaces around operators and after commas, readable indentation, and sensible blank lines. Keep punctuation and line wrapping clean.
- Write maintainable, modular code with clear ownership of state and explicit interfaces. Avoid needless duplication and opaque generated-looking code.
- Keep comments minimal and useful. Explain non-obvious behavior, reverse-engineered constants, and evidence references; do not narrate obvious syntax.
- Handle expected failures explicitly. Use C error/status returns or C++ exceptions only where appropriate to the interface; do not allow exceptions to cross C ABI boundaries. Validate pointers, sizes, ranges, asset paths, and provider availability before use.
- Add useful structured/timestamped logs for initialization, key state transitions, asset/provider failures, and diagnostics. Make verbose logging controllable so normal gameplay is not flooded. Never log secrets or dump unrelated memory.
- Use assertions for internal invariants during development, not as the only validation for untrusted files or runtime input.
- Keep Windows platform code separate from shared game rules where the retail architecture permits. Keep low-level recovered ABI details documented and isolated.
- Prefer stable language/toolchain versions over chasing the newest release. State compiler and language standard in build reports.

## 10. Git and delivery practices

- Keep changes small, focused, and reviewable. Maintain `.gitignore` so build products and temporary analysis outputs do not swamp source control; do not ignore needed fixtures, evidence manifests, or source research.
- The owner previously requested locally initialized project work, branches such as `main`/`master`, `develop`, and feature work, and a private GitHub destination under `heaven-hm`. Check actual repository and remote state before changing it; branch naming and remote setup are not permission to overwrite history or push.
- Local commits are allowed when explicitly requested. Use a concise German commit message if the owner repeats the earlier commit-message request. Do not push without a direct, current instruction.
- Before reporting a change, include paths changed, rationale, commands run and outcomes, and remaining limitations. Never imply that an artifact was copied to Downloads, committed, pushed, or launched unless that operation actually succeeded.

## 11. Required progress reporting

For each substantial work session, report:

- **Current project location** and exact files being worked on.
- **Immediate objective** and how it fits the gameplay-first priority.
- **Tools actually used**, with their executable paths/versions or MCP endpoint, and what each was used for.
- **Evidence level**: static, build, synthetic, runtime integration, or parity.
- **Commands/build outcomes**, including failures and environment blockers.
- **What remains** before the native game can launch and before movement/weapon/AI fidelity can be claimed.

Do not invent a minute-by-minute activity history. When the owner asks for detailed status, provide a timestamped, factual session log based on recorded actions; distinguish current-session activity from prior notes and plans.

## 12. Definition of meaningful milestones

### Visual-scene checkpoint

- Windows x86 build starts through DirectDraw 7 under CrossOver.
- Level 1 OBJECTS.QVM creates expected 3D model instances at verified positions, orientations, and rotations.
- Level 1 terrain and original textures render in the same scene.
- Structured object/material logs reconcile with retail observations; an actual game-window screenshot is supplied for owner review.

### Playable Level 1 checkpoint

- The verified simulation and QVM/task flow runs with the player and real world collision.
- Player movement, buildings/stairs/doors interactions, AI graph navigation, health/damage, weapons, gameplay animations/effects, mission flow, and sound work together in Level 1.
- Focused and end-to-end tests pass; CrossOver runtime evidence and screenshots are retained; owner manually reviews the result.

### Fidelity claim

Claim parity only for the specifically measured subsystem and scenarios. A “100%” claim requires comprehensive retail runtime comparisons across relevant states, inputs, collision cases, weapons, AI, assets, scripts, audio, and platform conditions, not just static coverage or compilation.
