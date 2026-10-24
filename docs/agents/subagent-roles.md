# Subagent roles, handoffs and the 3-agent rule

Owner instruction (2026-10-25): divide work among subagents - one for research/testing/verification, one for the actual implementation and
build - and when a subagent is idle, use the three available slots on the same task, in parallel or sequentially depending on the
dependency shape. This file records how that is done in this repository so any session can follow it.

## Bound by the repository limits

`AGENTS.md` 10.13 fixes the hard limits: **at most 3 active agents total (Lead + 2 worker slots)** and one implementation writer at a time.
The owner's "3 agents" is therefore the Lead plus two workers; when the Lead is not writing code it acts as one of the three workers
(analysis, verification or measurement). Do not spawn a fourth agent and do not run two implementation writers in one increment.

## Roles

| Role | Owns | Never does |
| --- | --- | --- |
| **Lead** (integrator) | Shared history, `CMakeLists.txt`, `docs/fidelity/native-verification.json`, `docs/fidelity/native-database.md`, commits, builds, the final gate run | Transcribes a native it has not verified the evidence for; edits a worker's file while the worker is running |
| **Implementer** (worker slot 1) | One native per file, provenance block, no helpers; writes only the files named in its brief | Commit, build, manifest/CMake edits, another worker's file, renaming a placeholder |
| **Verifier / Tester** (worker slot 2) | Measurement: `scripts/build_native_records.py` CFG, angr entry check, semantic review against retail bytes, paired Frida runtime capture; report file only | Editing implementation source, or approving a native whose evidence it produced itself |
| **Research / flow analyst** (either slot when idle) | Call-order and dependency derivation (`scripts/flow_order.py`, `scripts/landability_scan.py`), extent measurement, ABI questions | Writing engine code, changing statuses |

Every agent is fresh-context: it receives a brief, and it writes a report. Chat history is not an interface; files are.

## Handoff artifacts

- Brief: `.superpowers/sdd/<task>-brief.md` or the evidence path named in the dispatch. The brief is the only source of exact values.
- Report: `docs/fidelity/evidence/<topic>-report.md` (or the path named in the dispatch). Every claim in it must be reproducible from
  a command, a file offset, or a capture path.
- The Lead copies accepted findings into `CHANGELOG.md` and the evidence tree; nothing is accepted from chat text alone.

## Parallel or sequential - choose by dependency shape

- **Parallel (two workers)**: both items are independent native VAs with disjoint files whose dependencies are already implemented. Example:
  one worker transcribes `0x00424C10` while the other verifies a previously landed native.
- **Sequential (one writer)**: the second item needs the first item's symbol, global, or argument convention. Implementer writes, then the
  verifier measures, then the Lead integrates and builds - in that order, never overlapped.
- **Never parallel**: two writers in the same file, a writer and a build, or two runtime captures (both games take `IGIMUTEX`).

## Verification is never self-verification

An implementer may not mark its own native `STRUCTURALLY_VERIFIED`. The verifier re-derives the instruction stream from
`build/retail-dis.txt` and runs `python3 scripts/build_native_records.py <VA> --force`; only that measurement (CFG >= 0.85) plus a semantic
read of the operands can move a status. Runtime statuses additionally need the paired sequential capture from the same level argument,
and `RUNTIME_TESTED` needs the candidate to actually reach the native.

## Current assignment board

| Slot | Agent | Assignment |
| --- | --- | --- |
| Lead | `lead` | integrate, build, measure, commit; own `task_types.hpp/cpp` corrections and the startup sequence integration |
| Worker 1 | `chain-worker` | implement Wave A: handlers `0x424C10`, `0x424CA0`, `0x424280`, then Cursor registrar `0x424B90`; Wave B: `0x46DFB0` -> `0x46DF70` -> `0x46DF10` |
| Worker 2 | `handler-abi-worker` | ABI facts for the handler corpus + the `0x42DB00`/`0x42DCE0`/`0x42FE80`/`0x42FEF0` handlers and the two registrars |
| either, when idle | verifier duty | measure landed natives, re-read corrected natives (`0x00401400`, `0x00401530`), write the verification report |
