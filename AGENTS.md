## 10. AI-Agent Native Transcription and Verification Protocol

The goal is to reconstruct native functions from the original retail Project IGI binary (`igi.exe`) as faithfully as possible. The executable is the source of truth. AI-generated code is only a reconstruction until supported by static analysis, structural verification, and runtime evidence.

### 10.1. Required Tools and Responsibilities

| Tool                                                   | Responsibility                                                                                                                                                                                                  |
| ------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Binary Ninja MCP**                                   | Analyze disassembly, decompiled pseudocode, function boundaries, cross-references, arguments, and data types.                                                                                                   |
| **Ghidra MCP**                                         | Independently analyze the same function, validate decompilation, inspect references, and identify disagreements with Binary Ninja.                                                                              |
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

2. **Analyze independently with Binary Ninja, Ghidra, and angr**

   - Compare the decompilers with angr CFGFast recovery of the same exact entry and image base.
   - Compare pseudocode, control flow, types, function boundaries, and cross-references.
   - Investigate disagreements rather than choosing one decompiler automatically.

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
- Disassembly and decompiler references from Binary Ninja and Ghidra.
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
4. **Use independent analysis.** Compare Binary Ninja and Ghidra results, then use CFG and runtime evidence to resolve disagreements.
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

**Core principle:** Binary Ninja, Ghidra, angr, Radare2 CFG MCP, and IGI-Probe provide complementary evidence. RE-Agent coordinates the project-specific reconstruction and review process; REA is a separate upstream project whose capabilities may be evaluated for integration. The original executable and reproducible observations—not AI confidence—determine whether a native is verified.

### 10.7. Gate modes and candidate reuse

- `build.sh` calls `scripts/native_gate.py`; binary-analysis logic belongs in Python helpers. `scripts/angr_analyze.py` owns angr imports, PE loading and CFGFast execution.
- Build and launch acceptance requires the full production tree at `VERIFIED` with a CFG threshold of at least `0.85`. A single-method or changed-file pass does not qualify an entire release.
- A helper timeout, missing angr, wrong binary/hash/base, unrecovered entry, unresolved required control flow, semantic failure or behavioral mismatch must block verification. Keep semantic and behavioral checks enabled.
- Use `IGI_ANGR_PYTHON` to select an isolated interpreter containing angr. The default is `$ROOT_DIR/.venv-angr/bin/python`, independent of the RE-Agent checkout.
- Provenance comment format, comment names, and comment/status disagreements are warnings. Missing comments must not hide methods: use manifest addresses to discover them and retain blocking logic/evidence checks.
- For startup/load-only development checks, `./build.sh --runtime-trace <trace.json>` accepts only a complete `igi-native-runtime-trace` (schema 1) captured by IGI-Probe, bound to the exact retail SHA-256 and containing observed native VAs. It checks that observed subset at `STRUCTURALLY_VERIFIED`, then runs the host build and tests; it never creates a release attestation, deploys an executable, or replaces the full production release gate.
- Prefer small native fixes and small commits. Check the changed method, then repeat `./build.sh` as fixes accumulate. Keep current evidence with the code.
- Candidate methods may come from `scripts/GameNatives`, C# OpenIGI, `project-igi-editor`, or `hjcminus/Project-IGI-Terrain`. Retain source revision and applicable license notices. Copied/adapted code remains a candidate until checked against retail `igi.exe`.
- External source shape or a CFG score alone cannot prove equivalent calculations, return values or memory effects. Record separate CFG, angr, semantic and behavioral results; incomplete comparisons stay inconclusive.
