# TextureModifier_Push replay, 2026-10-21

The adjacent report records a bounded Unicorn PE32 comparison of retail
`0x00505E40` and a separately compiled candidate from
`engine/src/retail/terrain/terrain_loaders.cpp`. The report binds the retail
image, candidate source, runner and dependencies by SHA-256. All 12 recorded
fixtures matched their captured return and state observations.

The candidate is a test DLL, not the installed or newly built game. The test
uses controlled, nonaliasing contexts and models `Error_Show` as a boundary;
it does not execute that callee. It does not establish natural level behavior,
terrain rendering, or paired Frida parity. Verification remains provisional.
An independent review found no mismatch within the captured fields. Table and
row memory, other globals, stack balance, preserved registers and all transitive
compiler headers are not covered by this report.

On this macOS host the same command exited with SIGILL in Unicorn's `mem_map`
under the execution sandbox before any comparison. Running it outside that
sandbox produced the adjacent report. A runner crash must remain a blocking
failure; this observation does not authorize suppressing it.

The current shared verification manifest points this address at the
`scripts/GameNatives` reference rather than the engine source tested here.
Do not bind this report to that different source or promote its status without
fresh matching static, semantic, build and paired runtime evidence.
