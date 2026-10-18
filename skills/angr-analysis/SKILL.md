---
name: angr-analysis
description: Use angr for independent static analysis of native binaries, especially function CFGs, disassembly, decompilation, and bounded symbolic execution. Use alongside the project's established reverse-engineering evidence; do not treat angr output alone as proof of behavioral equivalence.
---

# angr analysis

Use angr as an additional evidence source when analyzing a native function or validating control flow. The target executable remains the source of truth, and angr's recovered function boundaries, indirect branches, and decompilation can be incomplete or incorrect.

## Native gate integration

`scripts/native_gate.py` launches `scripts/angr_analyze.py` once for the addresses selected by the current gate mode. The helper runs `CFGFast` and returns machine-readable per-function block and edge data. The gate binds each result to the retail executable SHA-256, helper SHA-256, and current implementation SHA-256, then includes the validated output in its JSON report. It prefers `RE_AGENT_ROOT/.venv/bin/python`; set `IGI_ANGR_PYTHON` when angr is installed in another isolated environment.

An exact function recovery is only an angr structural pass. Missing dependencies, a timeout, an unrecovered address, or invalid output is inconclusive or failed and blocks the gate. This does not replace the required Binary Ninja/Radare2 comparison, semantic review, or live differential tests; `VERIFIED` still depends on those behavioral checks.

The same guidance applies when using re-agent's optional `angr` backend. Configure `backend.type: angr` and `backend.binary_path` to enable it. Keep Ghidra configured separately when the task needs Ghidra evidence.

## IGI workflow

1. Confirm the exact `igi.exe` path, architecture, image base, and SHA-256 before analysis. Record the hash with the results.
2. Load the executable with `auto_load_libs=False` unless a specific library model is needed. Start with `CFGFast`; it is a static approximation and may miss indirect targets.
3. Check the requested address against angr's recovered function map and the executable's mapped base. Do not silently analyze a nearby function if the address is missing or appears to be mid-instruction.
4. Collect function disassembly, block addresses, branch/call edges, direct callers/callees, and decompiler output when useful. Preserve the angr version and analysis options.
5. Compare angr findings independently with Binary Ninja, Ghidra, and Radare2 CFG evidence. Investigate differences in boundaries, branches, calls, and returns.
6. Use symbolic execution only for a specific question with bounded inputs, state count, and step count. Treat its results as path evidence under the stated model, not a complete runtime observation.
7. Record tool output, assumptions, unresolved indirect control flow, timeouts, and missing runtime evidence. Keep native verification status provisional until the project's full evidence requirements are met.

## Example

```python
import angr

project = angr.Project("/path/to/igi.exe", auto_load_libs=False)
cfg = project.analyses.CFGFast(normalize=True)
address = 0x004CF390
function = cfg.kb.functions.get(address)
if function is None:
    raise RuntimeError(f"angr did not recover a function at {address:#x}")

print(function.name, sorted(function.block_addrs))
```

For usage and API details, consult the [angr CFG recovery guide](https://docs.angr.io/en/latest/analyses/cfg.html), [angr decompiler guide](https://docs.angr.io/en/latest/analyses/decompiler.html), and [upstream repository](https://github.com/angr/angr).
