---
name: igi-native-verification
description: Verify Project IGI native transcriptions against the exact retail igi.exe before accepting them into a build or runtime.
---

# IGI Native Verification

The retail `igi.exe` is the behavior authority. A GameNatives export, generated comment, successful build, or passing unit test is not proof that a native matches the game.

## Required workflow

1. Confirm the exact PE32 x86 executable and SHA-256. This project currently expects `Downloads/IGI/igi.exe`; never substitute an IGI2 image or an unrelated executable.
2. Identify the exact native boundary, address, calling convention, parameter/return signature, callers, callees, and relevant state.
3. Analyze the same function independently with Ghidra and Binary Ninja when available. Record the evidence IDs and investigate any disagreement. If Binary Ninja is unavailable, record that limitation and use the strongest available disassembly evidence; do not claim independent BN validation.
4. Check the CFG with Radare2: basic blocks, conditional branches, call sites, loops, and returns. Compare the Ghidra and Radare2 counts using:

   ```text
   score = 1 - (
       abs(B_ghidra - B_r2) / max(B_ghidra, B_r2, 1)
     + abs(I_ghidra - I_r2) / max(I_ghidra, I_r2, 1)
     + abs(C_ghidra - C_r2) / max(C_ghidra, C_r2, 1)
   ) / 3
   ```

   Here `B` is basic-block count, `I` is conditional/loop branch count, and `C` is call count. Structural acceptance requires at least `0.85`; a failed or mid-instruction analysis cannot pass.
5. Use the matching address-qualified source from `scripts/GameNatives` as the transcription source. Preserve its control flow and call structure. If it is incomplete or conflicts with executable evidence, fix the source from the binary first. Label host bridges `HOST HELPER`; never label a host rewrite as a retail native.
6. Add a five-line provenance block and a record to `docs/fidelity/native-verification.json`. The record binds the implementation and GameNatives source hashes to the exact binary hash, exact function address, signatures, Ghidra and Radare2 evidence IDs, CFG counts, recomputed score, and review status. Updating either source invalidates the record.
7. Keep statuses evidence-based: `STRUCTURALLY_VERIFIED` means the interface and CFG were compared and the score passed. Build/runtime parity requires additional build or runtime evidence. Do not promote status because a comment says so.
8. For transformed or compiler-divergent code, use graph alignment tools (QBinDiff, BinKit, BINALIGNER, Diaphora) or source-CFG comparison where available. Symbolic/path equivalence tools (angr, SymCC, Miasm, KLEE) are stronger optional checks; record assumptions and limits. They do not replace the function identity and evidence record.

## Build and run gate

`build.sh` and `run.sh` invoke `scripts/native_gate.py --production-tree --threshold VERIFIED`: every production `engine/src/retail/*.cpp` is checked on release build and launch, independent of Git status. Any native without current evidence blocks compilation or launch. Semantic review and behavioral comparisons remain required; a status label alone cannot authorize release. The CMake engine target uses `--changed --threshold STRUCTURALLY_VERIFIED` for development feedback. `STRUCTURALLY_VERIFIED` means function structure and interface evidence passed; it does not prove behavioral equivalence by itself. The pre-commit hook checks staged native changes as an early commit-time check, not as a substitute for the full production-tree gate.

Use `--production-tree --threshold VERIFIED` for release builds so every production native is checked, not just recently changed files. During development, use `--changed --threshold STRUCTURALLY_VERIFIED` for faster feedback. Remember: `VERIFIED` only helps catch incorrect logic if your gate actually performs semantic checks and behavioral comparisons. A status label alone doesn't prove correctness.

The current gate cannot prove full source-to-binary semantic equivalence. It binds reviewed implementation and GameNatives hashes to the selected executable, compares signatures, and recomputes Ghidra/Radare2 CFG similarity. Do not describe that CFG score as a candidate-vs-retail parity score. A native should be called behaviorally `VERIFIED` only after the evidence record includes appropriate candidate-vs-retail structural or runtime comparison for its declared scope; unresolved parity keeps it provisional.

RE-Agent and upstream REA are separate projects. Record which MCP server/provider produced evidence; do not label an REA/Ghidra result as RE-Agent evidence.
