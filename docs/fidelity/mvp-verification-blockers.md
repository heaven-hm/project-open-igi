# What blocks each MVP native from end-to-end verification

Generated from the level-5 paired capture (`build/runtime-traces/mvp-frida-pair-level5.json`),
the manifest and the record audit. The machine-readable table is
[`mvp-verification-blockers.json`](mvp-verification-blockers.json); this file explains the
categories and what each one needs.

| blocker | natives | meaning |
| --- | --- | --- |
| candidate-unreachable | 75 | retail calls it, the candidate program never does: the subsystem is not built into the scene |
| retail-unobserved | 23 | neither run reached it |
| end-to-end | 12 | both sides observed; 9 pass every pillar, 3 fail another pillar |
| unlinked | 6 | no candidate symbol at all |
| paired-mismatch | 4 | both sides observed, observed classes disagree |

An earlier pass had 20 unlinked natives. Fourteen of them were not missing code:
the catalog name and the port's name differ at the same retail address (the port
implements the x87 control-word helpers at the addresses the catalog labels
`Camera_*`, for example). `scripts/mvp_candidate_symbol_aliases.json` now records
those mappings with the reason each address is authoritative, and the capture
driver resolves through it. Linked coverage rose from 100 to 114 and the paired
capture then matched 10 natives instead of 8 - the two new ones are
`Display_ClearSurfaces_16` (the x87 init body) and `Iff_FindChunk`.

The six that remain unlinked are honest gaps: `QTask_CreateTask` and
`AnimIff_LoadAnimationFile` have no implementation, `Iff_GetChunkPayload` has no
function because the port holds the payload by value, and the three
`humanplayer_defines.cpp` natives cannot link until its five unimplemented
config/QVM dependencies exist.

## The flow fact that frames everything

Retail `main` (0x0048F530) itself calls only 7 of the 120 MVP natives:
`QTask_OpenEvent`, `AppContext_SetIsActive`, `AppContext_GetIsActive`,
`AppContext_SetTerrainLightmapsUsed`, `Display_ClearSurfaces_16`, `sprintf` and
`Error_Show`. The candidate observes three of those; the other four are startup calls
the MVP transcription skips. So "1:1 from main to the end of the frame" cannot be
established by transcribing main alone: 113 of the 120 natives sit below it in
subsystems the candidate does not run.

## Order of work implied by the table

1. `paired-mismatch` (5) - each is a concrete behavioural difference against a known
   retail body, so each is a real bug with a fix and a re-capture. `GOInRemap
   0x004037C0` is the clearest: retail returns the destination buffer it fills, the
   candidate returns a masked argument.
2. `unlinked` (20) - link the already-ported units into the executable (five IFF
   natives and `AnimIff_LoadAnimationFile` exist under other names without the
   `extern "C"` binding; `humanplayer_defines.cpp` is not in the root CMake list).
3. `candidate-unreachable` (69) - porting work in subsystem order: script/QVM, then
   resource, then animation/human/weapon.
4. `retail-unobserved` (18) - widen the retail run or record them as not exercised at
   MVP levels.

## Decoded contract for the first mismatch: GOInRemap 0x004037C0

Read from the full retail body (161 bytes, `0x004037C0..0x00403861`), not from the
current implementation comment, which understates it. The retail signature is
`uint32_t* __cdecl GOInRemap(void* destination, void* frame)` - two stack pointers,
not a single QVM context:

1. `count = [0x00BC2380]`.
2. `action = [0x00BC2358 + count * 345 * 4]` (stride 345 dwords), `slot = (count - 1) * 115 + action`,
   `entry = 0x00BC25BC + slot * 12` (three dwords).
3. `entry[0] = ReadArgument(frame, 0)`, `entry[1] = ReadArgument(frame, 1)`,
   `entry[2] = ReadArgument(frame, 2) & 0x3FFFFFFF`, through retail 0x004B8A20.
4. `[0x00BC2358 + count * 345 * 4] += 1` - the increment lands on the same slot the read
   came from, not on the remap row.
5. A 24-byte (six dword) record is copied to `destination` with `rep movsd`. Four of its
   dwords are written first: three zeros and the constant `0x00567C74`; two dwords are
   never written, so the retail copies whatever those stack slots held.
6. Returns `destination`, not a masked argument.

Two consequences for the port: the signature and the caller convention have to change
(the QVM dispatcher passes three script arguments today), and the record copy cannot be
bit-exact because two of its dwords are uninitialized on the retail side - the port must
either define them and document the divergence or the comparison must scope them out.
That makes this a transcription task with a scope decision, not a one-line repair.

## Return classes that depend on the arguments

The default paired criterion is the set of return-value classes each image produced. That
is invalid for a lookup: `Iff_FindFormChunk` returns the node whose tag matches the
argument or null, so its class depends on which call it is. The two runs made different
calls (retail 406, candidate 18), so comparing class sets compares two different samples.

`scripts/mvp_runtime_comparability.json` now holds curated exceptions with a reason per
address, and the emitted report states reachability as its basis instead of implying a
return-value agreement. With that recorded honestly, `Iff_FindFormChunk` passes the strict
gate on the pillars that do apply - CFG, angr, the live behavioural differential and a
paired observation in both images - making it the ninth native verified end to end.

## GOInRemap: the contract is decoded, the fix is not landed yet

`GOInRemap` 0x004037C0 has read as a paired mismatch on every capture: retail returns a
pointer, the candidate returns a series of small integers. Radare2 shows why. The native
does write the three frame arguments into the remap table, but that is not its return: at
0x0040382E it loads its first argument into EDI, builds a six-dword stack block holding four
zeros and 0x00567C74, copies the block with `rep movsd`, and leaves that destination in EAX.
Its signature is `uint32_t* __cdecl(void* destination, void* frame)`, and it also increments
the action slot it read (0x00403824). The port returns the third argument masked to 30 bits
and never writes a destination block.

A port of that contract was written and captured: with it in place the paired capture
observed `ptr` on both sides, 81 calls each, and matches rose from 11 to 12 while mismatches
fell to 4. It was then reverted rather than pushed, for a concrete reason. The repository's
integrity gate refuses to accept a changed native source without a full evidence set for
that address - a build report, a behavioural differential from a runner, a semantic review
and the paired probes. Two pieces are missing: `scripts/native_diff_runner.py` has no
adapter for this address (its retail-side fixtures assume simple global setters, while this
native reads a QVM frame and writes a destination block), and the record's canonical
GameNatives path `scripts/GameNatives/go/004037C0-GOInRemap.hpp` does not exist.

So the change is held at the gate, not waved through. Landing it needs, in order: a
differential runner that can drive this native's frame reader, a canonical GameNatives
transcription at that path, and a semantic review of the destination-block contract. The
decoded body, signature and return are recorded here and in the record's status line so the
next pass starts from the facts rather than rediscovering them.
