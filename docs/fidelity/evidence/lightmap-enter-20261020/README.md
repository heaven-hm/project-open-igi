# Lightmap stack correction, 2026-10-20

Retail remains the authority: igi.exe SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Binary Ninja was the only decompiler used for this correction. RE-Agent's
Radare2 disassembly and independent Radare2 CFGs corroborate its six-block
caller and four-block callee. angr recovers both exact entries, with an
additional call-continuation block in the caller.

The previous TerrainLightMap_Init_2 (0x004F82B0) wrote one byte at context
+0xE0 rather than a DWORD, omitted its call to 0x004F1160, and imposed a
20-entry branch absent from retail. The candidate now retains that native
call, both mode branches, the DWORD write, and the observed stack/count/return
operations. The callee retains its existing catalog label
Script_ParseEnumReal32_5; its observed behavior is a predicate on byte globals
0x00A758BE and 0x00A758BD, not established enum or float parsing.

The controlled x86 baseline failed all 24 fixtures. The corrected caller
passes 24/24 and the callee passes eight byte-state fixtures. The actual
candidate source is compiled into a PE32 fixture DLL and executed alongside
retail instructions; no reference implementation is copied into the oracle.
Source hashes and invoked build commands are retained in the replay reports.
The independent critic identified a source-binding check missing from the
mode-only harness; it now refuses an implementation path different from the
one it compiles.

Frida then found an application integration defect: the private, unread stack
array and its writes had been optimized away in the diagnostic Release EXE.
The existing stack/count are now externally visible native globals. This
preserves their compiled writes for other translation units and observation;
it does not establish the original array's complete extent.

The exact same frozen script explicitly executes 24 fixture inputs in retail
and the rebuilt candidate before the main thread resumes, then restores and
verifies the touched global regions. All outputs match in sequential runs
with window level1, window level4 and window level7. frida-matrix.json binds
those executable identities, probe bytes and raw outputs. These are explicit
agent-thread fixture calls. Three levels do not broaden the input coverage or
prove that the candidate's natural terrain loop invokes this native.

The first Frida attempt captured retail but stopped before candidate execution
because its optimized-away stack symbol was absent. It is retained as a failed
integration investigation, not passing evidence. There is no whole-game or
terrain-rendering equivalence claim. No game-window screenshot is claimed for
this native-state correction.

Both records remain TRANSCRIBED. Valid nonaliasing contexts, counters 0 and 19,
and the documented mode/field fixtures are the tested scope. Existing host
storage covers only 20 elements. Negative counters, broader retail storage,
overflow, aliases, invalid pointers, concurrent access, and natural game-loop
integration are unresolved. Other terrain natives still require transcription.

Reproduce controlled execution with scripts/lightmap_enter_diff_runner.py and
the binary/source identity arguments recorded in after.json or mode.json.
For paired diagnostic execution use capture_world_checkpoint.py with
--lightmap-cases, an operational --host, the freshly built --candidate, and the
same level for both processes. Diagnostic compilation uses the separately
user-authorized build_no_verify.sh; strict build.sh acceptance remains separate.
