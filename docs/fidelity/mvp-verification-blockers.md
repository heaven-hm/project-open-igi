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
| paired-mismatch | 3 | both sides observed, observed classes disagree |

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

## GOInRemap verified: the tenth native

`GOInRemap` 0x004037C0 had read as a paired mismatch on every capture. Radare2 showed why:
the native writes the three frame arguments into the remap table, then at 0x0040382E loads
its destination into EDI, builds a six-dword block (four zeros and 0x00567C74), copies it and
returns that destination in EAX. It also increments the action slot it read (0x00403824). The
port returned the third argument masked to 30 bits.

Landing it needed the evidence the gate demands, so this round added the missing pieces:

- `scripts/goinremap_diff_runner.py` executes the retail instruction bytes under Unicorn with
  the true PE mapped, replaces the frame reader 0x004B8A20 with a hook returning the fixture
  argument, supplies the caller's destination slot ([esp+0x28], where the retail reads it),
  and runs the candidate translation unit natively against the same table and frame
  fixtures. It compares the remap entry, the incremented action slot, the returned block's
  defined dwords and whether the destination came back: 8/8.
- A semantic review records function identity, argument mapping (frame reader), the 345-dword
  action stride, the (count-1)*115 slot arithmetic, the remap stride, the 30-bit mask, the
  0x567C74 descriptor, the single return path and the block-ownership deviation.
- The paired capture then observed `ptr` on both sides with 81 calls each.

The strict gate passes every pillar, which makes this the tenth native verified end to end.
Two honest caveats are recorded rather than hidden. The retail caller supplies the destination
buffer, while this port returns a private static block because its config decoder discards the
result. And the canonical declaration `scripts/GameNatives/go/004037C0-GOInRemap.hpp` exists and
is bound by hash, but the gate refuses to stage a GameNatives file for an address whose
implementation lives under engine/src (it requires the canonical source to *be* the
implementation), so the declaration is not yet tracked.

## A return value that is not a contract

`DynObj_LoadModelTexturePackage` 0x004CF080 had read as a paired mismatch: retail returned null
on the shipped data, the candidate returned a package pointer. The obvious reading - that the
port's return is wrong - is not supported by the evidence. The native has a single `ret`
(0x004CF2F2) reached after loop counters are decremented, so what EAX holds there is
path-dependent: the live capture saw null, while the empty-fixture differential in
`scripts/mtp_package_diff_runner.py` derives a non-null value for the same entry.

I tried making the port return null to match the live observation; the differential then failed
0/8, which is the point. Neither value is "the contract" - the return is a leftover of the loop
structure, and comparing it across two independently timed runs is not meaningful.

So the comparison basis for this address is now reachability, with that reasoning recorded in
`scripts/mvp_runtime_comparability.json` and in the emitted report. The native passes the strict
gate on the pillars that apply - CFG, angr, its dedicated behavioural differential and paired
probes on both sides - making it the eleventh native verified end to end.

## The terrain result block: fix identified, landing blocked by the file's own gate

`Terrain_CreateMaterial` and `Terrain_CreateTileMap` mismatch for one reason: the retail dispatcher
supplies a 24-byte result block that each native fills and returns, while the port's script
emulation calls `Terrain_CreateMaterial(nullptr, args)` and `Terrain_CreateTileMap(nullptr, args)`.
Both natives then faithfully return the null they were given, so retail read `ptr` and the
candidate read `null` on every capture.

I implemented the correct fix at the two call sites - an aligned 24-byte block, as a retail caller
supplies - rebuilt, and captured it. It works: the paired capture then observed `ptr` on both sides
for both natives, matches rose from 13 to 15 and mismatches fell from 3 to 1. The differentials
kept passing throughout because they exercise the natives with a real context and never covered the
caller's omission, which is precisely why this stayed invisible.

The change was then reverted, because the repository gate refuses to accept it. Editing
`terrain_loaders.cpp` stages ten natives at once, and the pre-commit gate requires every native in a
staged source to satisfy its requirement set: build report, behavioural differential, semantic
review and (at VERIFIED) paired probes. Eight of those ten sit at ANALYZED or STRUCTURALLY_VERIFIED
with no semantic review and, for several, no runner that can drive their address. So the commit is
blocked by the other eight natives in the file, not by anything wrong with the fix.

What landing it needs, concretely: semantic reviews and behavioural evidence for the seven ANALYZED
natives (Terrain_LoadTexturesAndLightmaps, Terrain_Load, Terrain_TraverseContourQuadtree,
Terrain_DecodeChunkCommands, Terrain_Draw, TextureModifier_Push, HeightMap_Enter), a canonical
GameNatives source for TerrainLightMap_PushEntry, and runners for the addresses whose current
runners do not support them. Once those exist the two-line call-site change goes in unchanged.

## Main-to-end flow: what the candidate's Main covers, measured

Retail `_start` 0x0048F530 calls seven MVP natives. The candidate's Main now covers three of them
in the same shape as retail: `FP_InitExtendedPrecision` (x87 setup), `AppContext_GetIsActive` in the
per-frame condition, and `QTask_OpenEvent` on the active branch, with `Loop_StatusByte` driving the
window teardown exactly as the retail's loop does.

Both retail startup calls are now mirrored. `AppContext_SetIsActive(1)` and
`App_ContextSetTerrainLightmapsUsed(1)` run before the frame loop, and Main's record gained the
build artifact it was missing plus a semantic review re-written against the new body rather than
re-stamped. The capture method cannot see those calls, and the earlier reading that `AppContext_SetIsActive`
was "observed once at startup" was wrong. The driver starts the image to a level and attaches
afterwards, so one-shot startup calls execute before any hook exists. `AppContext_SetIsActive` still
records a hit because other code paths call it later; `App_ContextSetTerrainLightmapsUsed` is called
once, at startup, and is therefore never observed - the alias resolves to the right VA (0x0040A6C0)
and the image contains the call at 0x40B5A0 inside Main, but no hook is installed at the time Main
runs it.

Observing a startup call needs a capture that spawns suspended, installs hooks and only then resumes
the image. That is a change to the capture driver, not to the port, and it would apply to every
startup-window native rather than to this one.

The remaining two retail calls are not flow gaps in the ported sense: `sprintf` and `Error_Show` sit
on retail's startup-error path, which the candidate takes only when a subsystem fails to
initialise.

## The setter's candidate observation still does not land

`App_ContextSetTerrainLightmapsUsed` 0x0048F260 executes in the candidate - the rebuilt image
contains four references to its body at `0x0040A6C0`, including the startup call added to Main - but
the probe still records zero candidate calls for it while recording one for `AppContext_SetIsActive`
on the adjacent line.

A candidate symbol alias was added for the address
(`igi::retail::core::App_ContextSetTerrainLightmapsUsed`, the port's actual name for the setter) and a
fresh capture was taken with the alias in place; the candidate count stayed at zero. The alias table
is loaded and normalised correctly (`load_symbol_aliases` lowercases addresses and the resolver
prefers the alias), so the gap is in hook installation for this internal, non-exported symbol rather
than in the name mapping. Until that is diagnosed the emission leaves the entry inconclusive, which
is the truthful state: the source mirrors the call, the binary contains it, and the runtime
observation has not been captured.

## The attach race was not the cause either

To test the "hooks installed too late" explanation, Main gained an env-gated settle window
(`IGI2_ATTACH_SETTLE_MS`): a Sleep at entry that no normal run sets, so the observation driver can
install its hooks before the startup calls execute. The rebuilt image was captured with a six-second
window, which is far longer than the driver needs to find the pid and attach.

The candidate still records zero calls for `App_ContextSetTerrainLightmapsUsed` while recording one
for `AppContext_SetIsActive`, even though both calls are adjacent in the same function and both are
present in the image. So the race hypothesis is disproven: nothing was installed late. The remaining
explanation is that `Interceptor.attach` on that particular entry point fails or is dropped - the
function is 0x14 bytes at 0x0040A6C0, adjacent to other small setters, and the probe installs hooks
without reporting failures.

The settle window stays in the port as documented instrumentation, since it is exactly what a
future capture needs once the hook path is fixed, and it is inert unless the variable is set.
