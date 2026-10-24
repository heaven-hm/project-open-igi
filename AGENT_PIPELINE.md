# Single-Agent Pipeline Command (copy-paste prompt for agents)

> **Audit 2026-10-23:** Read [workflow blockers and two-worker ownership](MVP.md#workflow-audit-2026-10-23) before using this prompt. Required helpers are absent in the main checkout but present in the named DeepSeek worktree. Automatic discovery can miss normally named C++ files; the default command does not require fresh paired intake. Do not treat its label as MVP acceptance.


Use this single prompt for any agent contributing to the autonomous Project IGI reverse-engineering pipeline.

---

You are operating the Project IGI autonomous reverse-engineering pipeline.

MANDATORY COMMAND — USE THIS, DO NOT INVENT ALTERNATIVES:

  ./scripts/automatic_reverse_engineering.sh

SCOPED STRICT INTAKE — USE THIS WHEN WORKING ON ONE NATIVE:

  ./scripts/automatic_reverse_engineering.sh --address 0xXXXXXXXX --probe <reviewed-probe> --level <N>

THAT IS THE PIPELINE. THAT IS THE FILE. THAT IS THE COMMAND.

PURPOSE
Drive the current increment (igi1.exe inspection demo — DirectInput mouse look) through:
  static gate -> angr semantic analysis vs original igi.exe -> strict intake -> paired runtime -> typed comparison.

RULES
- Fail closed. Never suppress checks. Structural-only is not acceptance.
- One native at a time (--max-natives default 1); defer extras as not_solved_in_this_run.
- Never mark a native VERIFIED without build + behavioral + semantic + CFG structural + angr exact entry.
- Do not reconstruct new natives unless explicitly instructed.
- Preserve all pre-existing uncommitted work; do not touch unrelated files.

START
Run the full pipeline now. If it blocks, resolve blockers through the mandated order before any
further native work:
  1. behavioral-harness DAT_ declarations + window-procedure live-runner support
  2. diagnostic build receipt regeneration
  3. Binary Ninja MCP health
  4. semantic/behavioral evidence refresh -> re-run strict gate

> **Tool inventory:** every installed static/runtime/automation tool, its exact path, version and usage (Binary Ninja MCP, Radare2, angr, Ghidra, Frida + frida-server, CUA/CuaDriver, CrossOver/Wine, build and gate scripts) is documented in `TOOLS.md`. Read it before invoking a tool; if `python3 scripts/igi_workflow.py doctor` disagrees with it, the doctor output wins and `TOOLS.md` must be corrected.

## Matching-decompilation rule (owner instruction, 2026-10-24)

This pipeline reconstructs `igi.exe` as matching decompilation; it is not a re-implementation. Apply to every native and every pipeline stage:

- **Fidelity gate:** each line of engine code maps to a retail instruction and its evidence; unresolved blocks stay blocked and documented, never guessed or simplified.
- **No invented helpers/methods** in `engine/`: no wrapper types, adapters or convenience APIs. Routines retail calls must be transcribed as their own natives.
- **Only these additions are allowed:** logging that cannot change flow/values, and an isolated dev-mode command-line switch for testing. No other port-only logic inside natives.
- **Verification lives in `scripts/` and `build/`**, as static analysis plus paired sequential retail/candidate runtime evidence. No unit or contract tests, and no test code in engine sources.
- **Structure:** one native per function and file, retail boundaries and call order preserved; comments carry the provenance block (VA, artifact hash, tools, CFG/behavior, status/limits).
- **Status discipline:** a successful build proves nothing; status advances only on the evidence the protocol requires.

## Naming and strings (owner instruction, 2026-10-24)

- Native names use the retail-derived project form with the `NATIVE_DECL` macro: `NATIVE_DECL HumanPlayer_Load_Params()`. Never `Retail*`, `Host*`, `Port*`, `Wrapper*`, `Adapter*` in any function, type, macro or symbol name.
- No helper methods in `engine/`: no conversion/clamp/logging/memory/string utilities. If retail calls it, transcribe it as its own native; otherwise leave the block unresolved.
- Strings and messages byte-identical to `igi.exe` (option names, titles, file names, MessageBox texts, formats). No rewording.
- Preserve retail statement order and branch shape even when less idiomatic.
- Only allowed differences: the documented IGI_TESTING_MUTEX testing build and diagnostic logging, both absent from production builds.
