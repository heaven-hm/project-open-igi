> **Current owner scope and limits:** `MVP.md`, `tasks/plan.md`, and `DEEPSEEK_HANDOFF.md` govern `mvp/level1-integration`: first deliver original Level 1 models/terrain/textures and playable player controls/physics/interactions; then sound, enemies, weapons/combat, full mission, and menus/loading/text. Treat both older inspection-only and immediate-full-mission priorities as historical. This is retail decompilation reconstruction: preserve original native semantics/ABI/call flow, with independent static and focused per-native original/candidate Frida evidence; complete native-set coverage is distinct from universal/byte-identical proof. Use this existing checkout, one implementation writer, no new worktree, at most 3 active agents total (captain + 2 worker slots), and a shared 30 model requests per rolling minute including retries. Existing evidence and sequential-runtime rules apply.

## 10. AI-Agent Native Transcription and Verification Protocol

The goal is to reconstruct native functions from the original retail Project IGI binary (`igi.exe`) as faithfully as possible. The executable is the source of truth. AI-generated code is only a reconstruction until supported by static analysis, structural verification, and runtime evidence.

> **Tool inventory:** every installed static/runtime/automation tool, its exact path, version and usage (Binary Ninja MCP, Radare2, angr, Ghidra, Frida + frida-server, CUA/CuaDriver, CrossOver/Wine, build and gate scripts) is documented in `TOOLS.md`. Read it before invoking a tool; if `python3 scripts/igi_workflow.py doctor` disagrees with it, the doctor output wins and `TOOLS.md` must be corrected.

### 10.1. Required Tools and Responsibilities

| Tool                                                   | Responsibility                                                                                                                                                                                                  |
| ------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Binary Ninja MCP**                                   | Preferred decompiler; analyze disassembly, pseudocode, function boundaries, cross-references, arguments, and data types when available. Do not also run Ghidra on the same target/function.                         |
| **Ghidra MCP**                                         | Fallback decompiler only when Binary Ninja is unavailable for a target. Do not also run Binary Ninja on the same target/function.                                                                                 |
| **Radare2 CFG MCP**                                    | Analyze control-flow graphs, basic blocks, branches, loops, calls, and function structure.                                                                                                                      |
| **angr** | Independently recover exact retail function entries and CFGs with CFGFast; record image identity, assumptions and unresolved flow. Approximate CFG evidence does not prove behavior. |
| **IGI-Probe**                                          | Provide Frida-like runtime instrumentation for observing native behavior, function calls, memory values, and execution paths where supported. Do not assume IGI-Probe is Frida or supports every Frida feature. |
| **[RE-Agent](https://github.com/heaven-hm/re-agent/)** | Act as the project-specific AI-agent layer for evidence-guided reconstruction, tool orchestration, review, and verification workflows, according to its actual implemented capabilities.                        |
| **[REA](https://github.com/morluto/rea)**              | A separate upstream reverse-engineering project that may provide reusable analysis tools, integrations, or capabilities. Evaluate its actual functionality and license before integrating it into the workflow. |

**Important distinction:** RE-Agent and REA are two separate projects. RE-Agent is your project at `heaven-hm/re-agent`; REA is the upstream project at `morluto/rea`. Do not treat them as the same repository, claim they are already integrated, or attribute one project's features to the other.

### 10.2. Native Reconstruction Workflow

1. **Identify the target function**

   - Locate the native function in the retail `igi.exe`.
   - Record its address, calling convention, arguments, return value, callers, callees, and relevant memory references where evidence permits.

2. **Analyze with the selected decompiler and angr**

   - For each target, select exactly one decompiler: Binary Ninja first, Ghidra only if Binary Ninja is unavailable. Do not analyze that target/function with both.
   - Compare the selected decompiler's results with Radare2 CFG and angr CFGFast recovery of the same exact entry and image base.
   - Check pseudocode, control flow, types, function boundaries, and cross-references. Investigate disagreements with CFG, runtime evidence, or another independent method that is not a second decompiler.

3. **Verify the control-flow structure with Radare2 CFG MCP**

   - Check basic blocks, conditional branches, loops, jump targets, call sites, and return paths.
   - Ensure the proposed reconstruction preserves the original control-flow behavior.

4. **Collect runtime evidence with IGI-Probe**

   - When feasible, instrument the original game and observe actual calls, arguments, return values, memory changes, and execution paths.
   - Record the game version, test conditions, and limitations.
   - Static evidence must remain available when runtime testing is not possible.

5. **Use RE-Agent to coordinate evidence and reconstruction**

   - Feed the available analysis results and runtime observations into your RE-Agent workflow.
   - Generate or refine the native implementation, preserve evidence references, identify conflicting observations, and request independent critique.
   - RE-Agent should distinguish observed facts from hypotheses and generated code.

6. **Evaluate REA separately for integration**

   - Inspect `morluto/rea` for relevant capabilities and supported interfaces.
   - Integrate specific components only when they demonstrably improve the workflow and their licensing and dependencies permit the intended use.
   - Do not assume REA is a dependency of RE-Agent.

7. **Build, test, and review**
   - Compile the reconstruction for the correct architecture and calling convention.
   - Compare it with the original using test cases, runtime traces, and relevant state changes.
   - Resolve discrepancies or document them explicitly before assigning a verification status.

### 10.3. Evidence Requirements

Every reconstructed native should have an evidence record containing, where applicable:

- Original executable identity, version, and function address.
- Disassembly and decompiler references from the single selected decompiler, identified by tool and version.
- CFG findings from Radare2.
- Function signatures, calling conventions, argument meanings, return values, and side effects.
- Runtime observations collected through IGI-Probe, including test conditions.
- Differences between the original behavior and the reconstructed implementation.
- Unresolved assumptions, limitations, and reviewer comments.
- Build results and reproducible test results.

Do not invent function signatures, native names, argument meanings, or runtime behavior when the available evidence does not establish them.

### 10.4. Transcription Rules

1. **Preserve behavior, not just appearance.** Match the original branches, loops, calls, return paths, state changes, and side effects.
2. **Preserve the original interface.** Do not change the calling convention, parameter layout, return type, or memory assumptions without evidence.
3. **Separate facts from hypotheses.** Label inferred types, guessed names, and uncertain semantics.
4. **Use independent analysis.** Compare the selected decompiler with CFG and runtime evidence. Do not run both Binary Ninja and Ghidra on the same target/function.
5. **Do not blindly trust generated pseudocode.** Decompilers and AI agents can produce plausible but incorrect interpretations.
6. **Avoid unsupported simplification.** Do not remove unusual branches, error handling, or apparently redundant instructions until their behavior is understood.
7. **Review every meaningful change.** Require an independent critic/reviewer pass where available, address findings, and retain unresolved issues in the evidence record.
8. **Keep original and reconstructed code distinguishable.** Never present a generated implementation as a verbatim transcription of the binary.

### 10.5. Verification Statuses

Assign statuses based on evidence actually collected, not on an agent's confidence.

| Status                           | Meaning                                                                                                                           |
| -------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `ANALYZED`                       | Static analysis has been performed and recorded.                                                                                  |
| `TRANSCRIBED`                    | A candidate implementation has been written from the available evidence.                                                          |
| `STRUCTURALLY_VERIFIED`          | Function structure and control flow have been checked against the binary.                                                         |
| `BUILD_VERIFIED`                 | The candidate compiles successfully for the intended target.                                                                      |
| `RUNTIME_TESTED`                 | The candidate has been exercised under documented runtime test conditions.                                                        |
| `VERIFIED`                       | The relevant behavior has been checked against sufficient original-binary evidence and reproducible tests for the declared scope. |
| `VERIFIED_WITH_DOCUMENTED_SCOPE` | Verification passed for a clearly stated subset of behavior, with limitations explicitly recorded.                                |

A successful build does not prove behavioral equivalence. A runtime test does not prove every execution path. If evidence is incomplete or conflicting, keep the status provisional.

### 10.6. Final Acceptance Criteria

A native reconstruction is ready for acceptance when:

- Its implementation is traceable to the original `igi.exe`.
- Static-analysis findings and control-flow structure have been checked.
- Runtime comparisons have been performed where feasible, or their absence is explicitly documented.
- Important arguments, return values, side effects, and edge cases are supported by evidence.
- Review findings are resolved or recorded as outstanding.
- The verification status accurately describes what was tested.
- RE-Agent and REA are identified separately, and any claimed integration is supported by the actual repository implementation.

**Core principle:** One selected decompiler (Binary Ninja preferred, Ghidra fallback), angr, Radare2 CFG MCP, and IGI-Probe provide complementary evidence. RE-Agent coordinates the project-specific reconstruction and review process; REA is a separate upstream project whose capabilities may be evaluated for integration. The original executable and reproducible observations—not AI confidence—determine whether a native is verified.

### 10.7. Gate modes and candidate reuse

- `build.sh` calls `scripts/native_gate.py`; binary-analysis logic belongs in Python helpers. `scripts/angr_analyze.py` owns angr imports, PE loading and CFGFast execution.
- Default feature verification MUST use `python3 scripts/native_gate.py --root . --changed --threshold VERIFIED --cfg-threshold 0.85 --require-semantic-evidence --require-behavioral-tests`. Do not replace `--changed` with `--production-tree`. `--changed` selects edited native sources, not proof of runtime reachability. Required native addresses come from that gate report and must then be observed and compared in both games; unrelated unused catalog entries are outside this feature check. A scoped pass never qualifies the whole game as equivalent.
- A helper timeout, missing angr, wrong binary/hash/base, unrecovered entry, unresolved required control flow, semantic failure or behavioral mismatch must block verification. Keep semantic and behavioral checks enabled.
- Use `IGI_ANGR_PYTHON` to select an isolated interpreter containing angr. The default is `$ROOT_DIR/.venv-angr/bin/python`, independent of the RE-Agent checkout.
- Provenance comment format, comment names, and comment/status disagreements are warnings. Missing comments must not hide methods: use manifest addresses to discover them and retain blocking logic/evidence checks.
- For startup/load-only development checks, `./build.sh --runtime-trace <trace.json>` accepts only a complete `igi-native-runtime-trace` (schema 1) captured by IGI-Probe, bound to the exact retail SHA-256 and containing observed native VAs. It checks that observed subset at `STRUCTURALLY_VERIFIED`, then runs the host build and tests; it never creates a release attestation, deploys an executable, or replaces the changed-native feature acceptance gate.
- Prefer small native fixes and small commits. Check the changed method, then repeat `./build.sh` as fixes accumulate. Keep current evidence with the code.
- Candidate methods may come from `scripts/GameNatives`, C# OpenIGI, `project-igi-editor`, or `hjcminus/Project-IGI-Terrain`. Retain source revision and applicable license notices. Copied/adapted code remains a candidate until checked against retail `igi.exe`.
- Use a hybrid reference-and-evidence workflow: inspect `/Users/haseeb-mir/Documents/Code/project-igi-converter`, `/Users/haseeb-mir/Documents/Code/project-igi-editor`, and `/Users/haseeb-mir/Documents/Code/open-igi` first to identify file layouts, tags, resource paths, task/level behavior, and candidate control/data flow. Do not copy their implementation as a transcription or claim their behavior is retail-equivalent. Verify the exact selected behavior with Binary Ninja (Ghidra fallback), Radare2 CFG, angr, and sequential retail Frida evidence before accepting it. Prefer the converter's parsers for format discovery (for example MTP `BANM` and animation IFF chunks); use the editor for level/task cross-references; use retail runtime instrumentation for values, ordering, and state transitions.
- External source shape or a CFG score alone cannot prove equivalent calculations, return values or memory effects. Record separate CFG, angr, semantic and behavioral results; incomplete comparisons stay inconclusive.

### 10.8. Required sequential game-runtime workflow

- Use the same retail level argument for both programs, for example `igi.exe window level7` followed by `open-igi.exe window level7`. Run one process at a time: both use the `IGIMUTEX` mutex. Close the first process and confirm it is gone before launching the second. `window` is required for this scene test; omitting it selects OpenIGI's fullscreen path. Launch each program with `/Users/haseeb-mir/Downloads/IGI` as its working directory because retail startup checks `YMBE.AFP` by relative path.
- For each level comparison, capture retail first, then OpenIGI, with matching game directory, level, display mode, and observation steps. Preserve logs, Frida output, executable hashes, and level-script paths under `build/runtime-traces/` or `docs/fidelity/evidence/`; compare call counts, arguments/returns where the ABI is understood, state changes, load failures, and rendered output. Never infer equivalence from attachment or call counts alone.
- Use `docs/fidelity/runtime-test-workflow.md` for exact local paths, CrossOver/Frida startup and recovery steps, scripts, and a three-to-five-level matrix selected from 1–14. `scripts/frida_level_probe.js` is the reusable entry/return probe; it must identify unresolved signatures and avoid dereferencing unknown pointers. A Frida attach after launch misses earlier startup calls; record that limitation. Do not use remote spawn unless its working directory is proven to be the game data root.
- Read `humanplayer/humanplayer.qvm` and `config.qvm` before implementing movement. Record the decoded values and retail observations for input, motion, collision, jump, ground contact, and interaction. Do not treat guessed controller constants as retail facts. Compare the same task script (`OBJECTS.QVM`, loaded through the `.qsc` alias where needed) in each application.
- Run the two games sequentially for three to five representative levels, including the level under active development and levels with different terrain/object content. If the retail binary cannot load a requested level with this command, record the exact failure and select a level it supports; do not substitute a different script silently.
- For each native analysis, select one decompiler: Binary Ninja first, Ghidra only when Binary Ninja is unavailable. Do not analyze the same target/function in both. Radare2 CFG, angr, Frida/IGI-Probe, memory/state observations, and build/host regression checks answer separate questions. A CFG pass cannot establish camera, texture, input, collision, or visual correctness. Keep runtime parity provisional until paired observations support it; keep weapon, AI, damage, and sound outside the current movement/interactions scope.
- The Codex MCP registration and `reverse-engineer-anything` skill from `/Users/haseeb-mir/Documents/Code/re-agent` were refreshed on 2026-10-19. Restart Codex to load that updated MCP server in a fresh session. Treat RE-Agent as an evidence/orchestration layer; do not accept its generated candidate code as retail transcription without static, structural, and runtime evidence. Its health for each requested provider must be reported separately from tools configured by this repository.
- Do not introduce guessed constants, fallback movement, synthetic interaction, or substitute camera behavior. If the source transcription has unresolved blocks or runtime behavior is unknown, keep the feature failing/provisional and record the gap rather than masking it.

### 10.9. Mandatory per-native feature acceptance loop

- Work on one native/feature increment at a time. Follow `docs/fidelity/native-feature-verification.md`: static analysis (Binary Ninja preferred, Ghidra fallback only; Radare2 CFG; angr exact entry), source-semantic review, focused behavioral test, compile the candidate, then paired runtime comparison. Each phase requires recorded evidence before acceptance.
- Scope runtime capture and its checker with `--gate-report build/native-verification.json`. Every required address must appear exactly once in the differential report. Missing mappings, unobserved methods, missing candidate calls, mismatched inputs/outputs/side effects, unknown ABI, stale hashes or failed tools block acceptance. Never suppress these to obtain a green build.
- Semantic-review evidence must authorize a `runtime_contract` (scope, probe SHA-256, required input/output/side-effect observables and any proven absence of effects). Compare every ordered call against that contract; missing fields, placeholders, dropped/truncated sequences or a different probe block acceptance. All sealing modes require paired runtime evidence.
- Use the same probe, level, script, configuration and scripted actions for retail and candidate; read controls/player values from game QVM/configuration. Capture startup natives before startup or mark them unobserved; late attach cannot validate earlier calls. Compare pointer contents and state transitions through evidence-backed typed probes, never pointer addresses across processes.
- Runtime tests must target the newly compiled x86 candidate, with symbols resolved from that binary. Building a candidate is necessary to test changed code; seal/deploy only after runtime acceptance. Never test an old installed `open-igi.exe` and use it to approve new source.
- Inspect the earliest divergence, correct the native from retail evidence, rerun its static/semantic/behavioral/runtime checks, and only then move to the next increment. Retain raw traces, image/source/probe hashes, scenario and limitations. Generic raw-word/EAX probes establish observations only; side effects require a typed comparator. A matching return alone cannot pass.
- `--development` and `--runtime-trace` remain diagnostic builds only; they do not accept a feature. Do not promote a diagnostic pass, a CFG pass, an empty changed set or a reduced observation set to full verification. Record exact tested scope rather than claiming universal 100% equivalence.

### 10.10. Clean-branch starting point

This branch starts with a diagnostic Win32 shell, not the retail entry point or a playable game. Treat its window and debugger logging as infrastructure only. Do not infer native fidelity from them. Follow the MVP call flow from the retail entry point, implementing only the required 3D models, MEF/ILFF, textures, terrain, QVM/configuration, human player movement and collision. Keep AI, weapons, damage and sound outside that scope.

Consult `scripts/GameNatives`, `docs/fidelity/native-verification.json`, and the historical `game-engine-flow` branch for candidates and evidence. Historical status applies to the prior source and tested binary only; inspect the exact implementation, source/image hashes, scope and evidence before reuse. A copied candidate requires fresh checks against retail and the new candidate. Do not invent game logic, constants, bindings, names, signatures or behavior; keep unknown functionality blocked. Standard diagnostic/platform scaffolding must remain explicitly separate from game natives.

Use `python3 scripts/igi_workflow.py --help` for available discovery and verification commands. Use `./build.sh --diagnostic` to build the shell without feature acceptance. Default `./build.sh` requires the strict changed-native gate and paired typed runtime acceptance; an empty native scope blocks it. The inherited verification scripts may depend on paths from the older branch and must fail or report unavailable rather than claim a pass. Do not add invented test cases; use evidence-backed comparisons and existing probes. Never report CFG or an empty gate result as behavioral parity.

### 10.11. First task for this worktree

Before implementing game natives, complete and validate the smart verification workflow. Start with `python3 scripts/igi_workflow.py doctor`; configure an angr interpreter and locate the historical v4 catalog without misrepresenting missing tools as available. Adapt the inherited paired Frida workflow to this worktree's freshly built x86 candidate, retaining the sequential retail/candidate procedure and evidence requirements in sections 10.8–10.9. Existing historical records and scripts are references, not proof for this clean branch. Report unavailable dependencies and incomplete comparisons as blockers; never mark a native verified merely to make a gate pass.

Once the workflow is operational, follow the retail MVP call flow from the entry point one native at a time, maintaining the original ABI, calls, state and behavior supported by evidence. Use small local commits and do not push unless the user explicitly requests it. Keep existing agents' worktrees and changes intact.

### 10.12. Current ordered work: inspection demo

The immediate user-requested increment is `igi1.exe`: Level 1 3D models, original model textures, mouse look, and the previous developer free camera as explicitly authorized diagnostic tooling. Complete tool health and fail-closed workflow checks first. Follow the required retail entry/call flow for scene natives, excluding only proven non-MVP dependencies. Names such as FX or sound in the decompiler are not proof that a method can be skipped. Do not restore the old model scene wholesale: its placement heuristics, player fallbacks and status labels need fresh examination. Terrain and human physics follow this inspection increment; AI, weapons, damage and sound remain deferred.

Commands: `python3 scripts/igi_workflow.py doctor --check`; `python3 scripts/igi_workflow.py self-check`; `python3 scripts/igi_workflow.py analyze <retail-VA>`; `python3 scripts/igi_workflow.py verify <retail-VA> --level 1`; for new native increments the mandatory strict path is `python3 scripts/igi_workflow.py intake <retail-VA> --probe <reviewed-probe> --level <N>` (static gate + paired runtime + typed comparison; accept only on full pass, otherwise reject). Local tool paths are in `scripts/igi-workflow.json`. Analysis capture does not change native statuses. Do not claim all tools healthy from file presence, or native parity from the validator's artificial infrastructure fixtures. Keep paired runtime captures sequential and preserve other agents' active game sessions.

## Agent skills

Issue tracker: 'GitHub Issues are the tracker. See `docs/agents/issue-tracker.md`.'

Domain docs: 'This is a single-context layout. See `docs/agents/domain.md`.'

### 10.13 Owner instruction: matching decompilation only (recorded 2026-10-24)

This work is **matching decompilation of `igi.exe`, not a re-implementation**. The rules below are binding for every native, every file under `engine/`, and every agent:

1. **Byte-faithful flow.** Every function, branch, loop, call, argument, constant, string, ABI and memory reference must be traceable to retail instructions, with the evidence recorded in the provenance block above the native. If the evidence does not establish something, leave it blocked and record the gap; never guess, simplify, substitute, or invent.
2. **No invented helpers or methods.** Do not add helper functions, wrapper types, adapter layers, or convenience APIs to `engine/`. If retail calls a routine, that routine must be its own transcribed native (for example `StoreCommandLine` 0x0048F0C0, `SetWindowHandle` 0x0048F0E0). Port-only scaffolding must never sit inside a native's flow.
3. **Permitted additions only:** (a) logging that never changes control flow, values, or ordering, clearly marked as infrastructure; (b) a dev-mode command-line switch for testing, isolated from retail command-line parsing and documented as diagnostic-only. Nothing else.
4. **Testing stays outside `engine/`.** Static and runtime verification scripts live in `scripts/`/`build/`. No unit tests, no contract tests, no test code inside engine sources.
5. **Engine code stays simple, modular and well-defined:** one native per function/file, retail boundaries preserved, retail order preserved, no cross-native abstraction to tidy things up.
6. **Comments and logs are required.** Every native keeps the five-line provenance block (VA, artifact hash, tools, CFG/behavior, status/limits); comments explain the retail instruction behind non-obvious code.
7. **Nothing is promoted on the strength of a build.** Status only advances with the evidence the protocol requires; candidate runtime parity is never assumed.

A change that adds logic absent from `igi.exe` fails review even if the game behaves well, and a developer or agent finding such a change must remove it rather than keep it for convenience.

### 10.14 Naming, strings and no-helper rule (owner instruction, 2026-10-24)

8. **Retail native naming only.** No function, type, macro or symbol may be named `Retail*`, `*Retail*`, `Host*`, `Host_*`, `Port*`, `Wrapper*`, `Adapter*` or similar. Retail routines are named the way the retail symbol/behavior indicates, in the project's native form: `NATIVE_DECL HumanPlayer_Load_Params()`. The declaration macro is `NATIVE_DECL` (not a `*_RETAIL_*` name); it expands to the retail calling convention (`__cdecl`/`__stdcall`) as the disassembly shows. A name is part of the reconstruction, not a label for its port status.
9. **No helper methods at all.** Do not add helper functions to `engine/` — not for conversion, clamping, logging plumbing, memory access, string handling or convenience. If retail code needs behavior, that behavior is its own transcribed native at its retail address, or the block stays unresolved. Logging and the dev-mode switch (rule 3) are the only non-retail code and must live outside native flows.
10. **Strings and messages are byte-identical.** Every literal — option names, window/class titles, file names, `MessageBoxA` texts such as `"IGI is already running."`, `"Error"`, `"Innerloop"`, `"filesys.cfg"`, format strings, separators — must match the retail bytes exactly, including punctuation, capitalization, spacing and terminators. No reworded or "cleaner" text. Where a string address in retail is a pointer into BSS/static data, use a literal with identical bytes.
11. **Exact behavior over readability.** Do not reorder statements, merge branches, or hoist loops for style. Structure follows the retail control flow, even when that is less idiomatic C++.

The only intentional non-retail differences allowed anywhere are the `IGI_TESTING_MUTEX` build switch (testing build name) recorded in §10.13/§10.14 and diagnostic logging; both must be documented at the exact site and absent from production builds.

12. **Rename only after full verification.** Address-suffixed or decompiler-generated names (for example `SetAppTitle_48F100`, `Display_SetScreenMode_48F120`, `App_ContextSetDebugtextState`) are placeholders while a native is unverified or partially verified. Once a native reaches `VERIFIED` (or `VERIFIED_WITH_DOCUMENTED_SCOPE` for its declared scope), rename it to a clean retail-derived behavior name with no address suffix and no decompiler artifact name, update every call site, the provenance block (`// Native: <name>; retail VA 0x...` must still carry the VA), and the manifest record in the same change. Keep the old placeholder in the record's notes as an alias so evidence and prior captures remain traceable. Never rename a native before its evidence supports the name it is being given: an unverified native keeps its address-suffixed placeholder.
