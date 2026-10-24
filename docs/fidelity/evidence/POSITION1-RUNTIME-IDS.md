# Position-1 registrar: runtime id boundary (retail capture)

Source of truth for this note: `docs/fidelity/evidence/runtime/tasktype-registration-trace.json`
(sha256 `e7d06f88e89c480a34c5a50ec897b19f44f6d4c854e05cbb61bd9cc6d1c2b5e6`), captured by the
Tester from retail `igi.exe` (sha256 `dea34892…5a01`) with a **spawn-suspended** typed Frida
probe (`probe_tasktype_register.js` sha256 `76a5d970…6a3d`) hooking `0x00402930` and
`0x00401900` before the first instruction. 279 calls, none before entry, none after return.
Launch: `igi.exe window level1`, cwd the game data root (proved via `Process.getCurrentDir()`
at registrar entry). Limitation: one idle level-1 run, no gameplay input.

## Settled by observation

- **279 registrations, ids contiguous 0..278, one per call.** This agrees with the engine's
  existing 279-class capture and disproves the older assumption that position 1 alone
  assigns "ids 0..43".
- Every callsite is an `E8` decoding to `0x00401900` (279 distinct sites, 279 distinct
  return addresses), so each id maps one-to-one onto a transcription site.
- First: id 0 `QTask`, size 32, arg3 `0x181`, site `0x401130` (return `0x401135`).
- Last: id 278 `MenuLoader`, size 292, arg3 `0x0`, site `0x4190FA` (return `0x4190FF`).
- **`AIGraph` is id 122** at site `0x508FBF` (inside `0x508FA0`/AIGraph_Init). That is the
  decisive number: the AI-graph registrar (chain position 1) does **not** end before id 43;
  it finishes at or after 122.
- Registration is not ordered by chain position: two ids (0 `QTask`, 125 `Flow`) come from
  sites below `0x402930` (`0x401130`, `0x401FD0`), while `AIGraph` lands at 122 inside
  position 1. So the call graph, not the flat chain list, determines ids.

## Consequence for the transcription plan

The position-1 subtree must produce ids 0..122 (123 classes) before `0x489560` (position 2)
runs. The inventory counted **127 unconditional `0x00401900` sites** in that subtree, so
roughly four sites are conditional/duplicate/dormant on this path. That gap is now a
precise, checkable target: transcribe `0x508F80` → `0x5183D0` group → `0x508FA0` and the
observed ids must fall 0..122 in the observed order.

## Still open

- Which exact sites are the four non-executing ones among the 127 (compare ordered id list
  against the subtree site list).
- Testers' capture does not include exit code (driver terminated the spawn after 30 s);
  irrelevant to id attribution, but it means this trace cannot support lifetime claims.
