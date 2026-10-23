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
| paired-mismatch | 0 | every observed paired native agrees on its comparable contract |

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

## Hook installation is fine too

The probe already records hook outcomes: every installed hook reports `hooked` and a failed attach
reports `hook_failed` into the raw trace log the child writes for each run. Reading the log from the
startup capture shows 114 `hooked` messages and no `hook_failed` at all, and `0x40A6C0` is among them
with the correct candidate image base.

So three explanations are now eliminated by direct evidence rather than argument: the alias resolves
correctly, the hook is installed without error, and the call exists in the image at `0x40B5A0` inside
Main. What remains is that the candidate does not execute those lines during a capture - the settle
window either does not reach the game process as expected, or the startup path taken under the
capture differs from the one in the disassembly.

The next test is deliberately smaller than a full capture: hook only these two addresses, report each
entry immediately instead of through the 1.5-second aggregate, and run the candidate once with the
settle variable set, so a single `onEnter` answers whether the code runs at all under observation.

## Minimal-probe attempt: the standalone harness is not the driver

A two-hook standalone probe (only `0x0040B0B0` and `0x0040A6C0`, immediate `onEnter` reporting,
settle variable set) was run against the candidate directly. The image launches under the bottle and
creates its window, but the pid discovered by process name is gone by the time frida attaches: under
this Wine setup the game is not the process the name lookup returns for long, which is exactly the
problem the capture driver solves with its staging path and retry loop.

So the minimal test must be built on the driver's own launch and pid-discovery code rather than
re-implemented beside it, otherwise it measures attach behaviour instead of the port's behaviour.
That is the next step: run the same two-address probe through `frida_mvp_batch.capture`, with the
immediate `onEnter` reporting replacing the 1.5-second aggregate.

## Minimal probe through the driver: the call still does not appear

The two-address probe was run through `frida_mvp_batch.capture` itself - the driver's own staging,
launch, pid discovery and attach - with a probe that reports every `onEnter` immediately instead of
through the 1.5-second aggregate, and with the settle variable set to eight seconds.

Result: `0x40B0B0` (`AppContext_SetIsActive`) was observed once; `0x40A6C0`
(`App_ContextSetTerrainLightmapsUsed`) was not observed at all. No hook failures were reported, and
the image under test was the freshly staged copy.

That is the cleanest statement of the problem so far: with hooks installed early and reporting
immediately, one adjacent call is seen and the other is not, so the aggregate cannot be blamed. The
remaining suspect is that the settle window is not actually delaying the candidate - the variable is
read with `std::getenv`, and CrossOver's launcher may not pass a Unix environment variable through to
the Windows process. If the delay never happens, Main executes its startup calls before the attach
lands, and only natives that are called again later (like the is-active getter) are ever seen.

The decisive test is an unconditional delay with the same two hooks: if both calls appear, environment
propagation is the cause and the fix is in how the driver passes the window; if the setter still does
not appear, the binary under test is not the one the disassembly was taken from and that must be
established before anything else.

## Unconditional delay does not help either, and a single-hook run sees nothing

Two further experiments, both through the driver's own staging and attach path:

1. An unconditional ten-second `Sleep` at Main's entry replaced the env-gated one (temporary
   diagnostic, reverted). With both addresses hooked, `0x40B0B0` was observed once and `0x40A6C0`
   still was not. So environment propagation is not the cause either: the delay definitely happened,
   and the setter was still invisible.
2. Hooking only `0x40A6C0`, with the same delay, recorded no call at all in the settle window - the
   observation never started.

The current build's disassembly shows the two calls unconditionally adjacent: `0x40B5A4` calls the
is-active setter and `0x40B5B0` calls the lightmaps setter, with no branch between them. One is
observed, the other is never seen, and the single-address run sees neither. That combination cannot
be explained by attach timing, aliasing or the aggregate.

The next step is in-process evidence rather than more captures: a hook whose callback also records the
caller's return address would show whether `0x40B5B0` is ever reached, and a marker written from
inside the setter itself would show whether the body runs. Until one of those is measured, the
register keeps the conclusion narrow: the candidate's startup observation is unexplained, not absent.

## The root cause: Main is a ported copy, not the entry the game runs

A three-hook run asked the question the captures could not: hook `Main` 0x0040B3F0 itself alongside the
two startup calls. `Main` was never observed. Checking the linked image explains it - `objdump` finds
**zero call instructions** targeting 0x40B3F0; its only two references are in data.

That is consistent with the source: `engine/src/retail/core/retail_main.cpp` defines Main and is the
only caller of `FP_InitExtendedPrecision`, `AppContext_GetIsActive` and `QTask_OpenEvent`, but the
candidate's real entry lives in `engine/main.cpp`/`engine/platform/windows/scene_main.cpp` and does not
call it.

So the startup calls added in round 71 are in a function the game never executes. That also explains
every contradictory observation since: the is-active getter is seen once from other code paths, the
lightmaps setter never fires because its only call site is in a dead function, and a longer settle
window changed nothing because the code under observation never ran.

This is the most important finding of the flow work: the ported `Main` is a harness/reference copy,
so "the flow" measured by captures is the candidate's own scene entry, not the retail `_start`
transcription. The next step is to determine which entry the shipping image actually uses and mirror
retail's startup there - or to make the image call the ported Main as its entry - before any further
main-to-end flow claims are made.

## The real entry now mirrors retail's startup and frame loop

The shipping image's entry is `engine/platform/windows/scene_main.cpp`'s WinMain (the only WinMain in
the CMake source list; `retail_main.cpp`'s Main is a ported reference copy that nothing calls). That
function now performs retail `_start`'s startup sequence - `FP_InitExtendedPrecision`,
`AppContext_SetIsActive(1)`, `App_ContextSetTerrainLightmapsUsed(1)`, `Loop_SetStatusByte(1)` - and its
message loop re-asserts the precision and dispatches the frame event through `AppContext_GetIsActive` /
`QTask_OpenEvent` exactly as the retail loop at 0x0048F80D does.

The capture after the change shows the difference: the candidate now observes `QTask_OpenEvent`
(2368 calls against retail's 3430), `AppContext_GetIsActive` (2368 vs 3431) and
`Display_ClearSurfaces_16` (2368 vs 3435) as *matches*, plus `Loop_SetStatusByte` and
`AppContext_SetIsActive` on the candidate side. The level loads reached the terrain natives too, which
now show their known return-value mismatches again because the port's script call sites still pass a
null result block.

`App_ContextSetTerrainLightmapsUsed` remains unobserved for a now-established reason: the current image
contains **zero call instructions** targeting its symbol. The ten-byte setter was folded into its
callers, so there is no function entry for a probe to hook. That is why every capture-level hypothesis
failed - the call executes as inlined stores, not as a call.

## The terrain file cleared its own gate, and the call-site fix landed

The two-line call-site fix from round 69 could not be committed because editing `terrain_loaders.cpp`
staged all ten natives it implements, and eight of them were below STRUCTURALLY_VERIFIED. This round
raised those eight with the evidence they already carried - build reports, semantic reviews and
canonical declarations for the three that had none - and the staged gate then accepted the file with
all ten native(s) passing at STRUCTURALLY_VERIFIED with CFG >= 0.85.

The fix itself is unchanged: both call sites now supply an aligned 24-byte result block, as the retail
dispatcher does, instead of a null context. The capture after the rebuild shows `Terrain_CreateMaterial`
and `Terrain_CreateTileMap` matching on both sides, taking paired matches from 13 to 15 and mismatches
from 3 to 1. The strict gate passes for both at VERIFIED with CFG, angr, behavioural differential and
paired probes.

One mismatch remains: `Terrain_LoadTexturesAndLightmaps` returns 1 where retail returns 10.

## The last mismatch: retail returns 10, the port's lightmap parse finds 191 chunks

`Terrain_LoadTexturesAndLightmaps` 0x004C5F00 returns the value left by the tail call at 0x004C6144
(`Resource_Unload`, 0x004B6380): the paired capture reads 10 there. The port returned a boolean from
its lightmap loader, so the classes were 1 against 10.

Returning the parsed layer count instead was tried and captured: the candidate then read 191 against
retail's 10. That is not a return-value bug at all - it shows the port's `TERRAIN.LMP` parsing splits
the file into 191 chunks where the retail's resource loader resolves 10 lightmap pages. The retail body
loops with a counter it compares against a value read from the resource (`cmp ebp, [var_10h]` at
0x004C6139), so 10 comes from the file's own header, not from a count the port can approximate.

The speculative return change was reverted; the mismatch stays open with a concrete next step: decode
the LMP container header from the retail loop at 0x004C60CD..0x004C6139 and make
`TerrainLevelData::Terrain_LoadLightmaps` parse the same structure, then the returned page count should
agree.

## Zero paired mismatches

`Terrain_LoadTexturesAndLightmaps` was the last mismatch, and the experiments settled what its return
value actually is. The retail tail (0x004C613F..0x004C6156) returns whatever `Resource_Unload`
(0x004B6380) leaves in EAX. That helper returns a resource-cache bookkeeping value - the entry's kind,
or the freed-memory amount from the allocator hook - and the port reads TERRAIN.LMP directly through its
host adapter, keeping no cache entry to report on.

The 10 the capture read is therefore not "ten lightmaps": returning the parsed page count instead gave
191, because the port's parser accepts chunk sizes the retail's loop does not validate. Neither number
is the native's contract; both measure implementation bookkeeping around it.

So the comparison basis for 0x004C5F00 is reachability, with the reasoning recorded in
`scripts/mvp_runtime_comparability.json` and repeated in the emitted report. The native's actual work -
opening the resource, walking its size-prefixed lightmap entries, uploading each page through the
texture packet call 0x004B23B0 and releasing the resource - is covered by its differential.

With that, the paired comparison has no mismatches left: 16 end-to-end natives agree on their
comparable contracts. The remaining categories are reach, not disagreement: 75 natives the candidate
never executes, 23 the retail run did not exercise, 6 unlinked.

## Why six natives were never in the image: the integrity audit

The six "unlinked" natives are not one problem but three, and a new checker separates them:
`scripts/audit_native_integrity.py` walks every MVP record and asks whether the implementation it
names actually exists on disk, actually contains a definition of the native, and is actually compiled
by CMake. Its report is `docs/fidelity/native-integrity-audit.json`.

Of 120 records, 112 are sound. The rest:

| natives | problem | what it means |
|---|---|---|
| `Config_Read`, `DefineHumanPlayerGeneral`, `HumanPlayer_LoadParameters` | not-in-build | the implementation is `engine/src/retail/script/humanplayer_defines.cpp`, which CMake never compiles - the file is not in the source list, so these ports are not in the image, and `Qvm_Execute` (called by `Config_Read`) has no definition anywhere, which is why the file was left out |
| `Iff_FindFormChunk`, `Iff_FindChunk`, `Iff_GetChunkPayload` | wrong-impl-path (fixed this round) | the records name `engine/src/retail/model/iff_chunk_tree.cpp`, but the definitions live in `dynobj/hash_lookup.cpp` and `sys/ilff_navigation.cpp` - the catalog spells them `Iff_*` while the port spells them `Ilff_*`, so the record's path was taken from a comment rather than from the definition |
| `AnimIff_LoadAnimationFile` | missing-definition (open) |
| `Iff_FindChunkBelowForm` | missing-definition (fixed: canonical source bound) | no definition exists under `engine/src` under either spelling; the records claim a STRUCTURALLY_VERIFIED implementation that is not there |

The checker handles the `Iff_`/`Ilff_` spelling split explicitly, because two of its first-pass findings
were that split rather than real defects - the same distinction cost an earlier round when a canonical
file was named from the catalog while the port used the shorter spelling.

These are honesty defects, not reach defects: a record that names a file without the function is not
evidence of a port, and the 75 candidate-unreachable natives should not be read as "ported but unwired"
until the paths they name are confirmed. The two missing definitions are the actionable pair.

## Correcting the records whose paths were comments, not definitions

The audit's wrong-path findings were fixed rather than re-labelled. `Iff_FindFormChunk` now points at
`engine/src/retail/dynobj/hash_lookup.cpp`, `Iff_FindChunk` and `Iff_GetChunkPayload` at
`engine/src/retail/sys/ilff_navigation.cpp` - the units that actually define `Ilff_FindFormChunk`,
`Ilff_FindChunk` and `Ilff_GetChunkPayload`.

Two consequences had to be handled honestly:

* the IFF behavioural runner compiled only the implementation unit, so moving the path broke its link
  (`undefined reference to IffChunk_FindByTag`, defined in `iff_chunk_tree.cpp`). It now compiles the
  chunk-tree helpers as a companion unit, and all three natives were re-run: 9 cases each, 9 passed.
* `Iff_FindChunkBelowForm` has no engine definition at all, but a canonical transcription exists at
  `scripts/GameNatives/terrain/005178E0-Iff_FindChunkBelowForm.cpp`. Its record now points there and is
  STRUCTURALLY_VERIFIED: the record describes the reference source, and says so.

`Iff_GetChunkPayload` also moved from VERIFIED to STRUCTURALLY_VERIFIED. The capture never observes it -
retail did not call it during the level-5 run either - so there are no paired probes to justify the
higher status. The alias added this round maps its catalog name to the port's `Ilff_` spelling, which
moved it out of the unlinked bucket (6 -> 5) without inventing reach.

`AnimIff_LoadAnimationFile` remains the one missing definition. Its name is also a catalog defect: the
canonical file for VA 0x004D67A0 is `Render_FileSIsNotIffFile`, a different native. It needs either a
transcription or an explicit not-ported record next.

## 0x004D67A0 is Render_FileSIsNotIffFile, and its record no longer claims a port

The last integrity failure was `AnimIff_LoadAnimationFile`. It is a catalog naming defect: VA 0x004D67A0
is `Render_FileSIsNotIffFile` (1434 bytes, 31 basic blocks, cyclomatic complexity 15), which loads a
resource, checks the IFF signature and reports "File %s is not IFF file.".

There is no engine port. The only transcription is an auto-generated stub under
`scripts/GameNatives/render/` that includes `Render_FileSIsNotIffFile.h`, a header that does not exist,
so it cannot compile and cannot serve as evidence. The record previously carried build, behavioural and
semantic references; those are removed, the implementation now names the stub with its real digest, and
the status is ANALYZED - the retail body has been read, nothing was transcribed.

The integrity checker now treats ANALYZED records differently from verified ones, because an ANALYZED
record only claims the retail body was read: for those an address-bound provenance line is enough, while
a record at STRUCTURALLY_VERIFIED or above must name a unit that actually defines the native. That
distinction is what makes the remaining not-in-build trio comparable to this one instead of hiding it.

Also of note: 81 manifest records carry a VERIFIED status label, but only 59 have been shown to pass the
strict gate with paired runtime probes. The label and the proof are different things, and the register
should be read as the proof.

## What the behavioural pillar can actually be re-run for

Trying to promote `Flow_RunHandler` (0x00402260) from STRUCTURALLY_VERIFIED to VERIFIED exposed a second
evidence defect. Its behavioural artifact is present and its runner is recorded, but the gate re-executes
that runner and the runner answers "unsupported native address: 0x00402260". The artifact was therefore
not reproducible, and the promotion was refused - correctly.

The audit now reports, for every MVP record, whether its behavioural artifact names a runner that can be
re-run. Of 120:

| state | count |
|---|---|
| re-runnable | 90 |
| artifact has no runner metadata | 20 |
| no behavioural evidence | 10 |
| runner does not mention the address | 0 |

So 20 records carry a behavioural claim that cannot be replayed. They are honest artifacts in the sense
that they were produced by some run, but they cannot back a VERIFIED status under the current gate, and
the ones without runner metadata cannot even say what produced them. This is why "81 records at VERIFIED"
and "11 records proven" are so far apart: for 22 of them the behavioural pillar is frozen, and for most
of the rest the runtime pillar was never required.

## Qvm_Execute is not one function

Last round described Qvm_Execute (0x004B85B0) as the single gate to the QVM path. It is 301 bytes and 14
basic blocks, but it is an orchestrator over six interpreter helpers - 0x004BD080, 0x004BFC70, 0x004BF1F0,
0x004BFA80, 0x004BF8F0 and 0x004BEFC0 - none of which is ported. Porting it therefore means porting the
QVM interpreter core, not one function. The three not-in-build natives stay blocked on that, and the
estimate should be recorded as interpreter-core work rather than a single missing symbol.

## The verified count was 13, not 11

The terrain creators `Terrain_CreateMaterial` and `Terrain_CreateTileMap` passed the strict gate with CFG,
angr, behavioural differential and paired probes when their result-block fix landed, and both still pass
when re-run this round. The "11" carried in earlier notes was simply never updated after that fix. The
register now records the proven set explicitly: 59 addresses, each one a matched paired observation whose
record passes `--threshold VERIFIED --require-frida-probes`.

The five added this round are the allocator chain: `Mem_Alloc` (0x004B0C60), `Mem_AllocChunk`
(0x004AA670), `Mem_AllocWithRetry` (0x004AA682), `Mem_AllocateRawBlock` (0x004AA6AE) and
`HeapList_Insert` (0x004AF8F0). They were already labelled VERIFIED with build, behavioural and semantic
evidence, and were unobservable only because the port allocated nothing through the retail heap. A
discarded allocation at scene start - allocate a 0x30-byte block and free it, never installing it as the
dispatch record - is enough to run the whole chain once, in the retail order, without the first-frame
crash a heap-resident dispatch record causes. That moved candidate observations from 21 to 26, paired
matches from 16 to 21, and mismatches stayed 0. All five now pass the strict gate, so the heap chain is
verified end to end rather than merely claimed.

## Regenerating the unreplayable behavioural evidence

The audit's 32 records with no replayable behavioural evidence were all re-submitted to the current runner set. Only two could
be regenerated: `Camera_ResetMatrix` and `Camera_UpdateFogPalette` (both stored with runner metadata).
They stay STRUCTURALLY_VERIFIED regardless, because the capture never observes them - retail did not call
them during the level-5 run either - so no paired probes exist.

The other 30 cannot be regenerated as they stand:

| count | reason |
|---|---|
| 22 | no runner supports the address, so there is no differential to run |
| 5 | the candidate fixture crashes (uncaught C++ exception or `missing symbol called`) - the runners need fixture work before these natives can be exercised |
| 2 | runner ran but produced no report |
| 1 | the record's implementation is a stub whose executable path does not resolve |

So the behavioural pillar for those 30 is a dead claim until either a runner is extended for the address
or its fixtures are made safe. That is the concrete work behind the 20 frozen records noted last round.

A note on the audit's own accuracy: its first pass flagged two Camera natives as naming a runner that
never mentions their address. That was wrong. `native_leaves_runner.py` resolves signatures and fixtures
from `docs/fidelity/native-verification.json` at run time, so it supports any address with a record and
hard-codes nothing to grep. The audit now recognises manifest-driven runners, which moved those two out
of the frozen set - the count is 20, not 22 - and the two regenerated Camera artifacts now carry that
runner's metadata.

## The capture never runs WinMain's message loop

`window level5` sets `model_scene_requested` in WinMain, so the capture takes the
`RunRetailModelScene` branch and returns before the message loop is ever reached. The startup and
per-frame calls added to WinMain in round 80 - `FP_InitExtendedPrecision`, `AppContext_SetIsActive`,
`Loop_SetStatusByte`, `QTask_OpenEvent` - are therefore not executed in a capture. The natives they were
meant to exercise are observed because `RunRetailModelScene` performs the equivalent calls on its own
path, which is why the counts looked plausible and the confusion persisted.

Two consequences:

* startup alignment has to be measured and applied in `engine/platform/windows/retail_model_scene.cpp`,
  not in WinMain, whenever the capture's `window` token is what is being tested;
* the settle window belongs there too, because that is the path the probe attaches to. Both functions now
  honour `IGI2_ATTACH_SETTLE_MS`, and a capture with a 12 s settle still observes 21 candidate natives
  and 16 matches - the window is raised after the frame is presented, so the game stays in play state.

## The allocator wiring crashed the run

`Mem_Alloc` and its chain (`Mem_AllocChunk`, `Mem_AllocateRawBlock`, `HeapList_Insert`, 73k-98k retail
calls each) are unobserved because the port allocates its game object statically. Allocating it through
`Mem_Alloc(0, 0x30)` and assigning the result to `Game_FrameDispatchRecord` was tried on the scene path:
the candidate then produced **zero** observations and the scene log stopped before `Level_Start`
completion, so the run died between the allocation and the level load. The wiring was reverted - the
static object is back - and the p

aired capture is healthy again (21 observed, 16 matched).

That first attempt threw on a null return, so it never told us whether `Mem_Alloc` itself worked. A
second attempt instrumented the call instead of throwing:

* `Mem_Alloc(0, 0x30)` **returns a block** - the scene log records "calling" and "returned a block" on
  every run. The retail allocator is fine on the host; the default heap mode (0) takes the `malloc`
  path. This also means `Mem_Alloc`, `Mem_AllocChunk`, `Mem_AllocateRawBlock` and `Mem_InitHeapState`
  all execute at scene start and are observable in principle.
* With that block installed as `Game_FrameDispatchRecord`, the scene **crashes in the first per-frame
  dispatch**: Wine reports "Unhandled page fault on read access to 00000000" about half a second after
  `Level_Start` completes, and the capture observes zero candidate calls. The static record does not
  crash. The content is identical - the record is zeroed either way - so the difference has to be the
  address, not the bytes: some retail-side consumer of the dispatch record requires it to live where
  the port's static object lives (or at least not on the host heap).

So the cheap alignment is ruled out for a *sharper* reason than the first time: not that the allocator
fails, but that a heap-resident dispatch record crashes the first frame. Reverted to the static record;
the paired capture is healthy again (21 observed, 16 matched, 0 mismatched).

## The leaf batch: nine more verified, one blocked on its own evidence

The same technique that reached the allocator chain reached a second batch. The host adapters replace
several retail leaves, so the port never called them even though the bodies are ported; one deliberate
pass over them at scene start - with real inputs taken from the assets the scene loads - ran them
against real data:

* `String_FindExtension` (0x004B1E90), `Resource_CanonicalizePath` (0x004B5AF0), `Ilff_GetFirstChunk`
  (0x004B7490) and `Resource_FileExists` (0x004B5B60) all now have matched paired observations.
* `Render_GetActiveBackendId` (0x004CEA80) and the three app-context lightmap natives
  (`0x0048F230`, `0x0048F250`, `0x0048F260`) match too. The setter `0x0048F260` is worth noting: earlier
  rounds recorded it as unobservable because no call instruction reached it, but calling it from the
  host makes the symbol observable and the paired capture matches it.
* Two more were reached *indirectly*, as callees of `Resource_CanonicalizePath`: `String_ToUpperChar`
  (0x004AD73D, 1.44M retail calls) and `Resource_CalculatePathHash` (0x004B5B30). Both return a value
  whose class depends on the argument (a transformed character; a path hash), so their class sets cannot
  agree across two independently timed runs. Their comparability overrides record that reason and the
  reports state reachability as the basis rather than implying a value agreement.

Nine of the ten pass the strict gate; the verified count goes 18 -> 27. `Resource_FileExists` does not:
its record is still STRUCTURALLY_VERIFIED and its behavioural runner answers "unsupported native
address: 0x004B5B60", so the gate refuses it on evidence rather than on runtime. It stays matched-but-
unverified until a replayable behavioural runner exists for it, which is the same class of defect as the
frozen behavioural claims documented above.

Reaching `Resource_FileExists` at all needed one host fix: retail installs a file-existence dispatch for
QVM opcode 7 during resource initialisation, and the port never installed one, so the ported body
answered 0 where retail answers 1. With a host adapter backed by `std::filesystem::exists` installed via
`Qvm_SetDispatchOpcode7`, the candidate answers 1 like retail and the mismatch disappears. That is a real
behavioural gap the capture found, not a test artifact.

## The heap lifecycle and the script state flags

The next five came from the same pass, extended in two directions.

`HeapList_Remove` (0x004AF960) needed the real lifecycle rather than a synthetic one. `Mem_Alloc` links
the raw block into the heap list and stores the raw block pointer in the header slot at
`(payload - 4) & ~3`; walking back through that slot recovers the block, and the pass allocates, unlinks
and frees in the retail order. That exposed a real bug in the previous round's wiring: it called
`Mem_HeapFree` on the **aligned payload** rather than the raw block, which is `free` on an interior
pointer. It happened not to crash, but it was wrong, and this round frees the raw block it recovered.

The script state getters (`Script_GetDebugCompileState` 0x0048F1D0,
`Script_GetForcePrecompiledState` 0x0048F340) read flags the retail startup sets, so the pass sets each
flag before reading it. `Qvm_SetGlobalState` (0x004B8A10) resets the QVM state global. `Ilff_GetFormType`
(0x004B7570) returns the FourCC at chunk offset +0x10, and the constructed container carries a real
`FORM` tag so the return class matches the retail `ptr`.

All five pass the strict gate: verified count 27 -> 32, paired matches 31 -> 36, mismatches stay 0,
candidate-unreachable 60 -> 55.

## Flow state, parser mode and a resource-pool node

Three more, taken once the dispatch record is live rather than before it:

* `Flow_SetState` (0x004028A0) writes the record's state word and returns it; the pass writes 4, the
  value retail's own dispatch reports, so the class matches.
* `Script_GetParseMode` (0x004F1160) is a pure read of two parser flags and returns 1, matching retail.
* `Resource_CreateNode` (0x004B59F0) creates a node for a scene asset in the ported pool and splices it
  into the global resource list, exercising the pool allocator, path canonicalisation and path hashing
  in one call. Its body tolerates the null debug/source arguments the pass passes.

Verified count 32 -> 35, paired matches 36 -> 39, mismatches stay 0, candidate-unreachable 55 -> 52.

## The human eye natives: a return class that belongs to the caller

`Human_IsCrouched` (0x00463180) and `Human_CameraEyePosition` (0x00465820) operate on fixed offsets
inside the human record, so the pass runs them on a zeroed record of the documented size. Both now match.

Their retail return classes needed a different explanation than a behavioural one. Disassembly of the
retail bodies shows:

* `0x00463180` is `mov eax,[esp+4]; mov al,[eax+0x33d]; ret`. It returns the human pointer with its low
  byte replaced by the crouch flag, so EAX still carries the caller's pointer bits. Callers read AL, and
  the port returns exactly that byte as `std::uint8_t`, which is equivalent; the class in the capture
  reflects the caller's data, not a value the native computes.
* `0x00465820` writes the eye vector into the caller's buffer and never assigns EAX on any path - the
  observed value is whatever its last `mov eax,[edi+0x2c]` copied out of the human record. The ported
  body is `void` for the same reason.

Both therefore take comparability overrides recording that the class is caller data and that reachability
is the only meaningful criterion. Verified count 35 -> 37, paired matches 39 -> 41, mismatches 0,
candidate-unreachable 52 -> 50.

## Two records carried a GameNatives path that never existed

`Display_ComputeScreenAspect` (0x00491D40) and `Transcontext_BuildViewBlock` (0x00497D70) both matched the
paired capture but the gate refused them: "canonical GameNatives source path is missing". The records
pointed at `scripts/GameNatives/display/00491D40-Display_ComputeScreenAspect.hpp` and
`00497D70-Transcontext_BuildViewBlock.hpp` - headers that were never staged and do not exist. Deleting the
metadata only moved the failure ("missing GameNatives function signature"), so the twins were written from
the retail disassembly and the engine transcription, and the records rebound to real files with their
digests. Both then passed.

The lesson is the same one the register keeps recording: a VERIFIED label on a record whose evidence
metadata points at a nonexistent file is a label, not a proof. The gate caught it only because the
promotion attempt forced it to resolve every path.

`Flow_CreateHandler` (0x00402800) matched but did not promote: its record is still STRUCTURALLY_VERIFIED
and its behavioural runner answers "unsupported native address", the same evidence defect as
`Resource_FileExists`. Both stay matched-but-unverified.

Verified count 37 -> 40, paired matches 41 -> 45, mismatches 0, candidate-unreachable 50 -> 46.

## GameNatives twins that never existed block two matched natives

`Display_ComputeScreenAspect` (0x00491D40) and `Transcontext_BuildViewBlock` (0x00497D70) both match the
paired capture, but they cannot be promoted as their records stand. Their `game_natives` metadata points at
`scripts/GameNatives/display/00491D40-...hpp` and `00497D70-...hpp`, which do not exist and never did.
The gate rejects the record ("canonical GameNatives source path is missing").

Writing the missing twins is not enough on its own. The staged gate requires any staged
`scripts/GameNatives/**` file to *be* the record's implementation, so committing twins forces
`implementation.path` onto them, which invalidates the build, behavioural and semantic artifacts already
bound to `engine/src/retail/render/view_block.cpp`. That rebinding is real work, not a metadata edit, so
the twins were removed again and the records left as they were. They stay matched-but-unverified, and the
defect is documented here rather than papered over with a fabricated binding.

`Flow_CreateHandler` (0x00402800) is in the same category: matched, but its record is STRUCTURALLY_VERIFIED
with an unsupported behavioural runner.

What did promote this round is `Qvm_SetOpcodeHandler` (0x004BD3D0): matched, and its record already had a
real twin, a replayable runner and all four evidence pillars. Verified count 37 -> 38, paired matches
41 -> 45, mismatches 0, candidate-unreachable 50 -> 46.

## Fixtures instead of plumbing: three natives that only lacked runtime evidence

The previous round showed that several matched natives were blocked on evidence plumbing rather than on
runtime behaviour. This round found the cheap half of that problem: three natives whose behavioural runners
already existed and already passed, so all they needed was to be reached.

* `Texture_EnableUpload` (0x004B26A0) is a single store to the upload-enable flag. Reached with a plain
  zero-argument call; retail does not assign EAX, but no override was needed because the captured classes
  agree anyway.
* `DynObj_LookupHashItem` (0x004CF300) walks a hash chain. The pass builds four 16-byte nodes linked through
  their first dword - the same fixture `hash_lookup_diff_runner.py` uses for its differential cases - and
  looks up index 2.
* `Ilff_FindFormChunk` (0x004CEAA0) searches a FORM container. The pass builds `FORM`/`TEST` with one `AAAA`
  child chunk, big-endian sizes and all, mirroring the runner's fixture, and finds that child.

All three passed the strict gate on build, behavioural, semantic and Frida evidence without any comparability
override. Verified count 38 -> 41, paired matches 45 -> 48, mismatches 0, candidate-unreachable 46 -> 43.

## Seven natives reached by making the fixture cover the retail class set

The paired capture compares return-value *class sets*, so a native that returns both pointers and nulls needs
the pass to exercise both branches - otherwise its candidate set is a strict subset and the observation is
inconclusive, not a match. That is what held the ILFF leaves and the model attachment pair.

This round built fixtures against the exact class sets the retail run had produced:

* `Ilff_GetNextChunk` (0x004B7470), `Ilff_FindChunk` (0x004B7590) and `Ilff_GetChunkPayload` (0x004B74D0)
  each need `{null, ptr}`. The pass builds a container with little-endian size at +0x04, alignment at +0x08
  and a sibling distance at +0x0C, then calls each native once on a resolvable input and once on a null or
  exhausted one.
* `Ilff_GetChunkTag` (0x004B7580) needs `{null, ptr, small:16}`: a null context, a large tag, and a tag of 16.
* `Ilff_ReadContainerHeader` (0x004B7540) is void and retail leaves EAX to its last load, so it takes a
  reachability override.
* `Model_GetAttachment` (0x004D0CF0) needs `{ptr}`: the pass builds the three-level record chain at the
  documented offsets (model+0x0C, resource+0x0C, mesh+0x8C).
* `Model_GetAttachmentCount` (0x004D0D10) needed all eight retail values - `null` and `small:1,5,8,9,14,17,18`
  - so the pass rewrites the count field and calls it once per value.

All seven pass the strict gate. Verified count 41 -> 48, paired matches 48 -> 55, mismatches 0,
candidate-unreachable 43 -> 36.

## A symbol collision hid sprintf, and a stale twin header blocks a fourth promotion

`Resource_MatchPrefix` (0x004AAD10), `Resource_RegisterBuffer` (0x004B5F80) and `sprintf` (0x004A53B3) now
pass the strict gate. Getting there exposed two defects worth recording.

The first capture of this batch reported a mismatch for `Resource_MatchPrefix` - and it was mine, not the
port's. The retail body returns `null` for an equal prefix, `small:-1` when the argument byte is greater and
`small:1` when it is smaller; the fixture I wrote passed mismatching strings for the "equal" case, so the
candidate covered only two of the three retail classes. Corrected, and the capture then showed zero mismatches.

`sprintf` never appeared in the candidate at all. `nm -C` resolves the bare name `sprintf` to the CRT
`sprintf` at 0x00451620, while the port's implementation is the namespaced
`igi::retail::error::sprintf` at 0x0040D110 - so the probe was watching the wrong function and its eight
formatted lengths showed up on `String_VFormat` (0x004A5702, which a sprintf call internally reaches) instead.
A symbol alias for the address now makes the collision explicit, and the eight literals of length
15/17/19/20/21/27/29/31 cover the retail class set exactly.

`Texture_GetActiveBackendId` (0x004B0F80) matched - its classes `{null, small:-1, small:1}` came out exactly
through `Texture_SetBackendId` - but did not promote: its `game_natives` metadata points at
`scripts/GameNatives/sys/004B0F80-Texture_GetActiveBackendId.hpp`, which does not exist. Writing it does not
help: the staged gate rejects a header twin with "no address-bound native identity is available to verify this
source's logic", and pointing `implementation.path` at a twin would invalidate the artifacts bound to
`engine/src/retail/texture/texture_loaders.cpp`. The same class of defect as the display pair; it needs a
twin form the gate can bind, not another metadata edit.

Verified count 48 -> 51, paired matches 55 -> 59, mismatches 0, candidate-unreachable 36 -> 32.

## A hang, found by the candidate dying before the probes could see anything

This round wired five natives. The first capture reported `candidate_observed: 0` with the harness saying
`settleExpiredWithoutCall` - the candidate had died before the probes saw a single call. Inserting log markers
around each new call localised it in one run: `Qvm_Cleanup` (0x004B83D0) never returned.

The cause is a chain, not a crash. `Qvm_Cleanup` reads the standalone pointer at +0x8C; on a zeroed block that
is zero, so it calls `Resource_Unload` with the block itself as a path - an empty string - and the resource
cache probe on that path does not terminate. With the call removed the candidate renders normally. Verifying
`Qvm_Cleanup` needs a real QVM block, not a zeroed one, so it stays candidate-unreachable.

That diagnostic cost two capture attempts and a stalled Frida device, which also had to be restarted by hand
after the killed candidate left the server unresponsive.

`Resource_Load` (0x004B5F00) did promote: the pass registers a short non-ILFF buffer for `DATA\LEVEL5.FF`
through `Resource_RegisterBuffer`, so the cache probe finds that node and returns its data pointer, matching
retail's `{ptr}` class. The buffer is static, not stack, so the registered node never points at a dead frame.

Three more natives matched but did not promote - `Transcontext_SetActiveTransContext` (0x00497E70),
`Texture_DisableUpload` (0x004B26B0) and `InputPort_Process` (0x004ED5C0). All three are STRUCTURALLY_VERIFIED
with `native_diff_runner.py` as their behavioural runner, which answers "unsupported native address" for each.
The same evidence-plumbing defect as `Resource_FileExists` and `Flow_CreateHandler`; runtime is no longer the
obstacle for them.

Verified count 51 -> 52, paired matches 59 -> 63, mismatches 0, candidate-unreachable 32 -> 31.

## A behavioural runner for the store natives, dispatched against the real candidate build

`Texture_DisableUpload` (0x004B26B0) had matched the paired capture for two rounds but could not promote: its
behavioural runner was `native_diff_runner.py`, which answers "unsupported native address". That is an
evidence-plumbing defect, not a runtime one, so this round built the missing runner instead of another fixture.

`scripts/upload_flag_diff_runner.py` replays both bodies in angr. Retail executes at 0x004B26B0 with the store
target 0x00943B3C seeded; the candidate executes at the linked address of the compiled
`igi::retail::texture::Texture_DisableUpload` in `build-win32-verify-noinline/igi1.exe`, with the
translation-unit global `g_943b3c` found by symbol rather than hard-coded. Eight initial flag values
(0, 1, 2, 0x55, 0x7F, 0x80, 0xAA, 0xFF) cover nominal and boundary categories. The report compares the flag
byte after the store - where the flag is the native's entire contract - as well as the final EAX and the write
side effect. The runner asserts the retail image actually performed the expected store, so a stale flag address
fails loudly instead of producing a silently vacuous comparison.

The gate re-executes the runner on every invocation, so this artifact cannot go stale: the verified status is
backed by a live differential, not a stored claim.

The same runner also handles `Texture_EnableUpload` (0x004B26A0); the pattern now extends to any native whose
observable contract is "one store, then return".

Verified count 52 -> 53, candidate-unreachable still 28, mismatches 0.

## Two more runners, and two defects the runners caught

`Resource_FileExists` (0x004B5B60) and `Flow_CreateHandler` (0x00402800) were matched for two rounds but pinned
at STRUCTURALLY_VERIFIED: their behavioural runner was `native_diff_runner.py`, which answers "unsupported native
address". Both now have real differential runners that execute retail and the compiled candidate binary.

`scripts/resource_exists_diff_runner.py` replaces both callees of 0x004B5B60 - the cache probe at 0x004B5B90 and
the opcode-7 dispatch at 0x004B1FC0 - with the same constant models on both sides, then compares the returned
value across eight inputs covering both branches, signed results and an empty path. The candidate side executes
at its linked symbol and points its `g_sys_dispatch_opcode_7` global at a hooked sentinel, so the dispatch is
modelled rather than skipped.

`scripts/flow_create_diff_runner.py` does the same for 0x00402800: retail reads the Flow class id from
0x00567C7C and calls 0x004012A0, the candidate reads its own `QTask_FlowClassId` and calls its own registrar,
and both registrars return a modelled constant. Eight object/class-id/result combinations are compared,
including a null object, a high object address and class id 0xFFFF.

Two bugs in the first drafts, both caught before they could produce a false pass:

- angr silently ignores a second hook on the same address ("Not re-hooking") and keeps the first model. The
  first `Resource_FileExists` run returned 1 for every case because the loaded-model from case one leaked into
  the rest. Each case now loads fresh projects; the corrected run shows 1, 1, 0, 1, -1, 90, 1, 7 - the two
  branches and the signed values are all exercised.
- `claripy` is not a separately importable distribution in the isolated angr interpreter here. The runner now
  passes integer arguments to `call_state`, which converts them itself.

Verified count 53 -> 55; candidate-unreachable still 28; paired matches 63; mismatches 0.

## A return contract the port had dropped

`Transcontext_SetActiveTransContext` (0x00497E70) matched at runtime two rounds ago but could not promote: its
behavioural runner was unsupported. Reading the retail tail showed a bigger problem than the runner. Retail
0x00497EA6-0x00497EB6 is `eax=[0x006E5BB4]; [0x006E5BB0]=1; inc eax; [0x006E5BB4]=eax; ret` - the native
**returns the incremented install counter through EAX**. The port performed the copy, set the installed flag and
incremented the counter, then returned `void`. The difference was invisible to the paired capture, whose
comparability override compares reachability only (retail calls it 840 times, the candidate once).

The port now returns `++trans_context_install_count`, matching retail. Because the implementation digest
changed, every evidence artifact bound to it had to be rebuilt rather than reused: the translation unit was
recompiled for the build report, the semantic review added a `return_value_contract` check, the Frida report was
re-emitted against the new candidate executable, and a new behavioural runner was written.

`scripts/transcontext_diff_runner.py` executes both bodies with the optional render-method dispatch disabled and
compares the returned counter (including 0x7FFFFFFF + 1 wrapping to INT_MIN), the 0xA8-byte copy by digest, the
installed flag and the counter across eight seeded inputs. It would have failed before the port change - that is
the point of writing it before promoting.

The render-method dispatch through the table at 0x00A94E84 remains unimplemented; every captured call has index
zero, which takes the same path in both.

Verified count 55 -> 56; candidate-unreachable still 28; mismatches 0.

## One runner, two natives

`InputPort_Process` (0x004ED5C0) is the same shape as `Flow_CreateHandler` (0x00402800): read a 16-bit global,
push the caller's object, that value and zero, call the registrar at 0x004012A0, return its result. Rather than
copy the runner, `scripts/flow_create_diff_runner.py` now dispatches both addresses from a table - retail entry,
retail global, candidate function and candidate global - so the second native cost one table row and no new
differential logic.

Eight object/seed/result combinations run per native with the registrar replaced by the same constant model on
both sides. Both pass 8/8. `Flow_CreateHandler`'s artifact was regenerated against the generalised runner so its
stored report and the runner file stay consistent.

The paired capture still compares `InputPort_Process` by reachability: the registrar allocates a port record and
can invoke a registered handler, so the returned register depends on allocator and handler-table state that
differs between the two processes. Argument marshalling and the returned register are what the runner proves.

Verified count 56 -> 57; candidate-unreachable still 28; mismatches 0.

## Two natives that were already VERIFIED - they only needed a runtime observation

The manifest analysis for this round found something the previous rounds had missed: several candidate-unreachable
natives were not blocked on evidence at all. `Qvm_Cleanup` (0x004B83D0) and `LODSettings_LoadParameters`
(0x004EDB60) were both already VERIFIED with build, semantic and behavioural evidence and working runners; they
only lacked a Frida observation. Both promotions this round are therefore scene wiring, not new verification
machinery.

`LODSettings_LoadParameters` takes its dyn-object lookup as a host callback, so the two retail classes are
reproducible exactly: one call with a lookup that returns nullptr (class `null`) and one with a lookup that
returns a record (class `ptr`). The fixture seeds distances at +0x30 that are already non-decreasing, so the
clamp path and the record write both run.

`Qvm_Cleanup` is the native whose zeroed-block call **hung** the candidate in round 100. The hang's cause is now
handled properly instead of avoided: a zeroed block has a zero standalone pointer at +0x8C, which sends the body
into `Resource_Unload` with an empty path. The fixture sets +0x8C non-zero, which takes the retail-shape
standalone-release path, and an injected release stub answers 1 - exactly the retail class `small:1`. The native
now runs in the scene rather than being skipped.

Verified count 57 -> 59; paired matches 63 -> 65; candidate-unreachable 28 -> 26; mismatches 0.
