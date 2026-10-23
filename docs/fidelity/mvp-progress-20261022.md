# MVP progress audit: 2026-10-22

Worktree: `codex-smart-workflow`. Read `AGENTS.md`; ran `python3 scripts/verify_demo_natives.py` at the default VERIFIED / CFG 0.85 requirements, with semantic and behavioral checks enabled. Raw artifacts are under `build/demo-progress-20261022/` and `build/demo-progress-20261022-rerun/`.

- Targets: 120. Independent angr batch recovered all 120 exact retail entries.
- Recorded BN/r2 CFG scores: all 120 at least 0.85. This is retail graph evidence, not proof that each candidate implements retail behavior.
- Existing engine implementation paths: 116. Four remaining records point only into GameNatives: 0x00401000, 0x0048F230, 0x0048F250 and 0x0048F260.
- Manifest labels: 114 STRUCTURALLY_VERIFIED, 6 VERIFIED. These are not fresh acceptance results.
- Fresh strict gate: 0/120 passed. The first batch aborted on the four reference-only addresses. After fixing selection, the gate checked 116 engine targets and retained a complete report with 325 failures.
- Global blockers include v4 catalog names Debug_Error versus Error_Show at 0x004AF7B0 and Mem_AllocFn versus Mem_Alloc at 0x004B0C60. The gate's own angr subprocess returned unparsable/empty output, despite the separate 120-entry batch succeeding. Unsupported behavioral runners and statuses below VERIFIED are additional blockers.
- Terrain_TraverseContourQuadtree (0x004D14E0), Terrain_DecodeChunkCommands (0x00510010), and Terrain_Draw (0x00512C30) still throw not-transcribed errors in engine code. Their STRUCTURALLY_VERIFIED labels do not establish implementation completeness.
- The custom OpenIGI diagnostic string is absent from current engine sources; current window identity is Innerloop.

The runner now isolates reference-only entries rather than aborting all engine checks; missing report rows and global gate errors block instead of passing. It preserves the gate report/stdout/stderr. A subsequent reporting fix preserves reference blockers when engine results are merged and labels manifest statuses separately from acceptance. The raw full-run report predates those reporting-only refinements; no native status was promoted. Invalid angr output diagnostics now retain child exit status and stderr for the next gate run. Infrastructure fixture checks and integrity pins passed.

The engine/manifest source snapshot stayed unchanged during the two complete audit runs. Existing changes by other agents were preserved. No new retail/candidate runtime parity, playable-MVP acceptance or screenshot was established by this audit.
