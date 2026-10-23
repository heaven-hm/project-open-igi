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

Also of note: 81 manifest records carry a VERIFIED status label, but only 91 have been shown to pass the
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
register now records the proven set explicitly: 91 addresses, each one a matched paired observation whose
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

## Device stream helpers, with the backend modelled instead of guessed

`Device_GetStreamPosition` (0x004B1780) and `Device_SeekStream` (0x004B17F0) were already VERIFIED with build,
semantic and behavioural evidence; they only needed a runtime observation. Both are pure dispatch wrappers: load
a record from a per-device table, then call the device backend with the device id and the caller's context.

The port exposes those tables through `Device_SetPushStack` / `Device_SetCheckHeap`, so the backend is
host-injected and the fixture replays what retail observed: eight calls returning 255, 259, 270, 313, 314, 319,
354 and a pointer-shaped value reproduce retail's class set exactly, and a seek stub returning 0 reproduces
retail's `{null}`. What actually runs is the native's own record load, argument marshalling and return, which is
its whole contract; the backend is a constant model, exactly as the behavioural runners model their callees.

`Device_CloseStream` (0x004B1690) is deliberately **not** wired. It is `void` in the port, so the candidate's
return register is whatever the callee left in EAX - not a value this native controls. Retail's `{null}` came
from 350 real calls; reproducing it would require the stub to leave EAX at zero, which means casting a callback
with the wrong signature. Manufacturing that observation would be a fabricated pass rather than a verified one,
so it stays unwired and unverified.

Verified count 59 -> 61; paired matches 65 -> 67; candidate-unreachable 26 -> 24; mismatches 0.

## A shaped ILFF fixture, and the regression it caused first

`DynObj_LoadPackageAnimations` (0x004CF910) returns null when the 'MANM' form exists with a zero count,
or when the lookup misses. Two mistakes had to be fixed before it could be wired honestly.

**The fixture perturbed a different verified native.** The first container carried only a 'TEXF' chunk, so the
'MANM' lookup missed and the native returned null - which is the retail class. But `Ilff_FindFormChunk`
(0x004CEAA0) is itself a verified native, and retail's 618 calls had only ever returned pointers. The probe
recorded a null return for that shared native and the capture reported **1 mismatch**: candidate class set
`{null, ptr}` against retail `{ptr}`. A fixture that makes one native match while breaking another's observation
is not a verification. The fix keeps the lookup successful - a MANM form is present - and reaches the null return
through the zero count instead, so both natives observe only retail-shaped classes.

**The form tag bytes are not the ASCII spelling.** `Ilff_FindFormChunk` compares `bswap32(stored dword)` against
the tag constant. For 'MANM' the retail constant is 0x42414E4D, so the container must store 0x4D4E4142 - bytes
`42 41 4E 4D` - not `4D 41 4E 4D`. Writing the ASCII spelling made the lookup miss, which is what produced the
mismatch above. Both the scene comment and this note record the byte-order requirement.

The behavioural runner (`scripts/dynobj_animations_diff_runner.py`) models the form lookup on both sides and
compares the count-to-pointer conversion across non-positive counts (0 returns null, -1 returns -1), eight cases.
The positive-count path is explicitly out of scope: it allocates through the retail allocator and walks global
script/texture state, which the runner does not model.

Verified count 61 -> 62; paired matches 67 -> 68; candidate-unreachable 24 -> 23; mismatches 0.

## Qvm_Load: satisfying the path the scene is already in

`Qvm_Load` (0x004B80B0) was already VERIFIED with build, semantic and behavioural evidence, and retail observed a
pointer return. The obvious fixture is to clear the script debug-compile flags and take the plain resource path,
but that would have introduced a bug in the evidence: `Qvm_Load` calls `Script_GetDebugCompileState` and
`Script_GetForcePrecompiledState` internally, both of which retail only ever saw return 1. Resetting the flags
would make those verified natives observe a zero class they never produced - the same trap that made
`DynObj_LoadPackageAnimations`' first fixture fail.

So the fixture keeps the flags where the scene already puts them and satisfies the debug/precompiled path instead:
it registers the `DATA\LEVEL5.QVM` name that path probes through `Resource_FileExists`, and models the symbol
resolver with a stub returning a non-null handle. All four natives the loader consults
(`String_FindExtension`, `Resource_FileExists`, `Qvm_SetGlobalState`, `Resource_RegisterBuffer`) keep exactly
their retail-observed class sets, and `Qvm_Load` returns the allocated block - class `ptr`, matching retail.

Verified count 62 -> 63; paired matches 68 -> 69; candidate-unreachable 23 -> 22; mismatches 0.

## Resource_Unload: reproducing seven classes the free hook owns

`Resource_Unload` (0x004B6380) was already VERIFIED and retail observed seven classes:
`{1, 10, 15, 206, 2666, 4454, 622}`. Reading the body explains them: it returns a node's `kind` only for kinds
1 and 3, and otherwise returns whatever the free hook answers. Those seven values are free-hook answers, not
kinds - the port exposes the hook through `Sys_SetFreeMemory16`.

So the fixture registers seven buffers (each yielding a kind-2 node, which takes the free-hook branch) and replays
the seven observed answers through the stub, once per unload. The candidate then produces exactly
`{1, 10, 15, 206, 2666, 4454, 622}` - an exact set match, not a subset. The hook is reset to null afterwards so no
later native sees the modelled answers.

Verified count 63 -> 64; paired matches 69 -> 70; candidate-unreachable 22 -> 21; mismatches 0.

## Terrain_LoadTexturePackage: a package header, plus reusing the free-hook model

`Terrain_LoadTexturePackage` (0x005175A0) was already VERIFIED and retail observed a pointer return. The body
loads a resource and branches on the package header: a LOOP version-7 package with a zero context value and a
zero mipmap count takes the allocation branch, allocates `count * 0x24 + 0x30` bytes, fills the header fields and
returns that block.

The fixture registers a 0x40-byte package with exactly that header. The internal `Resource_Unload` needed care:
the package is registered as a kind-2 buffer, whose unload goes through the free hook, so with the hook unset the
native would have returned kind 2 - a class `Resource_Unload` never observed. The fixture installs the constant
free-hook stub (answering 1, inside the observed set) around the call and resets it afterwards, so neither
`Resource_Unload` nor any later native gains a new class.

Verified count 64 -> 65; paired matches 70 -> 71; candidate-unreachable 21 -> 20; mismatches 0.

## Texture_UploadPalettePacket: a runner that exercises the marshalling, not just the return

`Texture_UploadPalettePacket` (0x004B7CF0) was STRUCTURALLY_VERIFIED and unreachable. Its contract is a write
(the active backend id into `*arg1`), a three-slot packet pack (slot 0 = arg1, slot 1 = arg3, slot 2 = arg2) and a
dispatch through the renderer callback table at `0x00A94E84[g_a84a48]`.

The runner (`scripts/texture_palette_upload_diff_runner.py`) models the callback identically on both sides with a
fold over the three packet slots, so a wrong slot order or a mishandled write changes the returned value rather
than hiding behind a constant. It sets the real globals on each side - retail `0x00A84A48` / `0x00A94E84` /
`0x00A94A64` and the candidate's own symbols - and compares the return value, the written backend id and the
dispatch side effect across eight cases. The trailing-parameter symbol trap resurfaced here: demangled names have
to drop the parameter list while keeping `(anonymous namespace)`, which is why the earlier `split("(")` attempt
failed and a trailing-only regex succeeded.

The capture fixture sets dispatch slot 2 and answers a pointer through the host stub, reproduces retail's pointer
class, and leaves the backend id at 0 so the nested `Texture_GetActiveBackendId` call stays inside its
retail-observed classes.

Verified count 65 -> 66; paired matches 71 -> 72; candidate-unreachable 20 -> 19; mismatches 0.

## Texture_UploadMipmapPacket: two callbacks, one of them silent

`Texture_UploadMipmapPacket` (0x004B23B0) was STRUCTURALLY_VERIFIED and unreachable. It packs a twelve-slot
packet (including the upload-flag byte at `0x00943B3C`) and dispatches through two slots: the primary slot's
return is discarded, the secondary slot's return is the native's result.

That asymmetry is the trap. A runner that only models the secondary callback would pass even if the primary
dispatch were missing entirely. The runner (`scripts/texture_mipmap_upload_diff_runner.py`) therefore gives the
primary callback an observable effect - it records its own fold of the twelve slots into a probe address - so the
dispatch order and the primary call are compared through the state delta. Both callbacks fold the twelve slots, so
slot order, the backend-id write and the upload-flag inclusion all move the compared values. Eight cases, real
retail globals on each side.

A small angr detail resurfaced: inside a SimProcedure the fold is a bitvector, not an integer, so
`struct.pack` fails and the value has to be stored directly through `memory.store`.

The capture fixture installs callbacks on both slots and replays the eight distinct secondary answers retail
observed, reproducing the eight-class set exactly while leaving the backend id at 0.

Verified count 66 -> 67; paired matches 72 -> 73; candidate-unreachable 19 -> 18; mismatches 0.

## Qvm_PushInteger: a real register divergence, and the cascade it caused

`Qvm_PushInteger` (0x004BDA00) looked like the simplest native left: decrement two stack indexes, write a type
slot, write a value slot. The behavioural runner proved the state writes matched exactly - and then failed on the
return value. Retail returned the **pushed value** in EAX (`1, 2, 3, ...`); the candidate returned a pointer.

Reading the retail instructions explains it: the body is one basic block whose last instruction is
`mov [value_stack + index*4], eax`, with `eax` loaded from the argument. So retail's EAX still holds the pushed
value when it returns - incidental to the compiler, but observable, and the capture had recorded exactly that as
the native's return class set `{1, 2, 3, 10, 11, 12, 13, 14}`. The port's host compiler leaves a stack pointer
there instead, so the port's observable post-call register state did not match the retail binary.

That made this a port defect rather than a fixture problem, and it could not be waved through: the gate recomputes
per-case equality over every observable field, so a divergent EAX cannot be excluded or overridden. The honest fix
is to return the pushed value explicitly - `Qvm_PushInteger` now returns `std::uint32_t` - so the post-call
register is defined by the port instead of by the host compiler's register allocation. The runner then passed
8/8 with both the state delta and EAX compared.

**The cascade is the important lesson.** Changing that implementation file invalidated every record bound to it,
not just this one: `Qvm_SetOpcodeHandler` (VERIFIED) and `Qvm_ExecuteInstructions` (STRUCTURALLY_VERIFIED) both
carry the implementation digest in their evidence. Leaving them stale would have silently rot-rotted two verified
records. All three had their r2/Binja/semantic/build evidence regenerated through
`generate_native_evidence.py` (addressed against a throw-away manifest so it could not clobber the real
verification fields), their behavioural artifacts re-run, and their Frida reports re-emitted. `Qvm_SetOpcodeHandler`
re-gated clean afterwards.

Practically: a shared implementation file means a change to one native's record is a change to all of them. Before
editing an implementation file, the records that point at it are part of the blast radius.

Verified count 67 -> 68; paired matches 73 -> 74; candidate-unreachable 18 -> 17; mismatches 0.

## Qvm_ExecuteInstructions: the second return-register divergence, and a cascade run as routine

`Qvm_ExecuteInstructions` (0x004BCFD0) clears stop_flag and loops: non-zero error_status returns immediately,
otherwise it fetches the opcode byte, dispatches handlers[0] with (state, 0, opcode, 0) and repeats while stop_flag
is zero. Retail observed `{1}` for its return class set.

The disassembly shows where that comes from: the error exit loads `error_status` into EAX, and the loop exit loads
`stop_flag` into EAX before the final test. So the observable return is the **termination reason**, not handler
residue as I first assumed. Same class of defect as Qvm_PushInteger: the port was `void`, so its post-call EAX was
host-compiler residue. It now returns the terminating field explicitly.

The runner models the dispatch handler with a probe that records the opcode it received and sets stop_flag, so the
opcode fetch, handler-table index, argument order and both exit paths are all compared; the error case proves no
dispatch happens. Eight cases, 8/8.

**The cascade was routine this time.** Changing that file again invalidated all three records bound to it
(PushInteger, SetOpcodeHandler, ExecuteInstructions). Because the blast radius is now a known step rather than a
surprise, all three had their evidence regenerated, behavioural artifacts re-run and Frida reports re-emitted in
one pass, and all three re-gated clean. The five-line comment statuses in that file were corrected to VERIFIED in
the same change, clearing the warnings the gate had been reporting.

Verified count 68 -> 69; paired matches 74 -> 75; candidate-unreachable 17 -> 16; mismatches 0.

### A comment-only edit is still an implementation change

While correcting the five-line comment statuses in `qvm_stack_natives.cpp` in the same change, the staged gate
rejected all three records again: the evidence binds to the SHA-256 of the whole implementation file, so a
comment edit invalidates it exactly like a code edit. The fix was to revert the cosmetic lines rather than run a
second evidence cascade; the digest returned to the value the artifacts were generated against and the staged gate
passed.

The rule: never edit an implementation file for cosmetics. Either batch the cosmetic change into a real change and
regenerate the cascade once, or leave the warning. The gate's comment-status warning is cheaper than a stale
digest, and a stale digest is a false claim about evidence coverage.

## Iff_FindChunkBelowForm: a twin-implementation promotion, no code change needed

`Iff_FindChunkBelowForm` (0x005178E0) was already in the best possible position for promotion and had been sitting
there: the record points at the GameNatives twin implementation, its paired runner
(`scripts/iff_chunk_tree_diff_runner.py`) already produced a valid nine-case report against the retail walk, and
only the runtime observation was missing. The candidate symbol existed but had never been called, so the capture
had no candidate call to compare.

The fixture builds the port's node graph directly - a chunk node whose sibling is a FORM node whose form type is
the query - and calls the port once for a hit and once with an absent tag. That replays both observed classes
exactly: `{null, ptr}`, no override, no mismatch. The record was promoted, its behavioural artifact regenerated
with the runner block, its Frida report emitted, and the strict gate passed on the twin implementation.

The generalisable point: not every remaining native needs port work. Several STRUCTURALLY_VERIFIED records may
already have a working twin and a working runner, with reachability as the only gap - those are the cheapest
promotions left, and the capture fixture is the entire job.

Verified count 69 -> 70; paired matches 75 -> 76; candidate-unreachable 16 -> 15; mismatches 0.

## Evidence audit: 22 VERIFIED claims that this tree could not reproduce

The MVP manifest listed 92 records at status VERIFIED while the strict-gate register held only 70. The 22-record
gap mattered more than a missing count suggests: those records were merged in from the parallel workflow, and in
this tree their status was a claim without the evidence that binds it.

Re-running the strict gate on all 22 produced **no pass**. Breakdown:

- **19 blocked on "missing Frida runtime probes report reference"** - and the level-5 model-scene capture never
  observes their retail calls (`retail_observed_calls` is 0), so the live differential cannot be reproduced here
  at all. Their VERIFIED status rested on captures from other levels or scenes that are not in this tree.
  Recovering these needs retail runs where they actually execute, not more port work.
- **3 blocked because their GameNatives twin sources are absent** (`Display_ComputeScreenAspect`,
  `Transcontext_BuildViewBlock`, `Texture_GetActiveBackendId`). The twins were recoverable from the sibling
  worktree and a full in-tree re-verification was attempted: their capture observations already matched retail
  exactly (`{ptr}`, `{small:1}`, `{null, small:-1, small:1}`), their static evidence was regenerated against the
  restored twins, and all three passed the strict gate. It still could not be committed honestly: the recorded
  twin digests exist only in the sibling fork's history, so the exact verified twins are unrecoverable, and the
  staged gate rejects twin sources whose records point at engine implementations
  (`implementation path does not match ... -Transcontext_BuildViewBlock.cpp`). Committing the newer sibling twins
  would have silently re-bound evidence to content nobody reviewed here.

**All 22 were demoted to STRUCTURALLY_VERIFIED**, each carrying an `imported_claim` block that names the claimed
status, this audit, and the concrete reason. Demotion is not lost work - records, runners and evidence all remain -
but leaving them VERIFIED would have counted twenty-two unproven natives toward the objective.

Result: the manifest's VERIFIED records among the MVP set (**70**) now equal the strict-gate register (**70**), and
there are **zero unregistered VERIFIED claims**.

Two rules come out of this. First, manifest status is not evidence, and a cross-worktree merge imports claims
without their proofs; re-run the audit after any merge. Second, the twin-model records are a structural liability:
when a record's implementation is the engine source and its `game_natives` field points at a transcription, the
staged gate refuses to commit the twin, so those records can never be re-verified reproducibly. Fixing that model
(making the twin the implementation, or teaching the gate about support sources) is prerequisite work before the
twin-backed records can be promoted here.

## Twin-model pilot: the precondition cannot be fixed by switching sides

Round 117 started by trying to fix the twin liability recorded in the audit: make the twin the record's
implementation (the model ~100 working records already use) so the staged gate can commit the twin source.

The pilot exposed two things. First, `00497D70-Transcontext_BuildViewBlock.cpp` does not contain that function at
all - it contains `Display_CloseDevice_15`, with its own comment recording a **CFG 80% FAIL** and "runtime behavior
not verified". `00491D40-Display_ComputeScreenAspect.cpp` likewise contains `Display_ClearSurfaces_4`. Those two
records were bound to sources for different functions, so their VERIFIED status was never reproducible in any
tree. Only `004B0F80-Texture_GetActiveBackendId.cpp` is the transcription its name claims.

Second, the switch does not work mechanically even for the honest twin: the paired runner compiles the twin and
links it against a harness that references engine symbols, which fails (`undefined reference to g_a94a64` and the
engine API). The runner was written for the engine implementation as the compile unit. So the twin-model records
remain blocked on a design decision - either the runner gains a twin compile mode, or the gate learns about
support sources - and this is recorded as prerequisite work rather than worked around. The two mislabelled twins
were deleted; keeping them would have kept a false binding in the tree.

## TerrainLightMap_PushEntry verified through a purpose-written differential

With the generic leaves runner failing on both push natives (`missing symbol called` / empty failure), the useful
target was `TerrainLightMap_PushEntry` (0x004F82B0): a bounded push that reads a parse mode and counter, stores
+0xD8 onto a host stack, sets the +0xE0 flag and returns the new stack index.

The new runner (`scripts/terrainlightmap_push_diff_runner.py`, 8/8) compares the returned index, the post-call
stack index, the stored value, the push flag and the store side effect, including the counter-zero boundary where
retail returns the object instead of pushing. The fixture replays seven valid pushes, which reproduces retail's
observed class set `{1..7}` exactly - no override needed. Strict gate passed.

Verified count 70 -> 71; paired matches 76 -> 77; candidate-unreachable 15 -> 14; mismatches 0.

## The two bounded-push natives, and two harness traps worth remembering

`TextureModifier_Push` (0x00505E40) and `HeightMap_Enter` (0x00506F00) share the same shape as the lightmap push
proven in round 117: validate a source row selected by the counter, store a value from the object onto a host
stack, set a flag, return an index. Retail returned `{1..8}` and `{1..3}` respectively, which the fixtures reproduce
exactly with 8 and 3 accepted pushes. Both runners pass 8/8 and both strict gates are clean.

Three traps cost time here and are worth writing down:

1. **The symbol matcher found the wrong function.** The first run of the texture-modifier harness resolved
   `0x41d610` (`TerrainLightMap_PushEntry`) instead of `0x41d540`, because the copied harness still carried the
   lightmap symbol names in its wanted set. The candidate then executed a *different native* and returned the state
   pointer, which looks like a contract mismatch but is a harness bug. Always print the resolved symbol addresses
   before trusting a failure.
2. **Demangled anonymous-namespace names break qualified matching.** `g_b97d40` and `g_b97df4` demangle as
   `igi::retail::terrain::(anonymous namespace)::g_b97d40`, so a qualified name like
   `igi::retail::terrain::g_b97d40` never matches `endswith`. Match the bare identifier.
3. **A retail global can be a dead store in the port.** `HeightMap_Enter` writes `g_ba2058` (retail `0x00BA2058`),
   but nothing in the port reads it, so the compiler eliminated the symbol and the write entirely. The harness
   therefore compares the returned active index, the stack index, the stored value and the flag, and records that
   the global is not part of the comparison rather than pretending to check it.

Verified count 71 -> 73; paired matches 77 -> 79; candidate-unreachable 14 -> 12; mismatches 0.

## The DynObj package loaders: two verified, one unreachable by construction

`DynObj_LoadPackageLovs` (0x004CF630) and `DynObj_LoadPackagePalettes` (0x004CF870) locate a chunk inside
`*(context + 0x08)` with Ilff_FindFormChunk and return the chunk payload pointer. Both are verified now; the
fixtures build a FORM container whose first chunk is the searched tag with a non-positive entry count, which
reproduces retail's `{ptr}` class exactly for each.

`DynObj_LoadPackageMeshes` (0x004CF330) cannot be verified this way and was left alone: the port dereferences the
lookup result unconditionally, so the only input that yields retail's observed `null` class (a missing `MODS`
chunk) faults. Retail survives that through its fault handler; the port has no equivalent, so the honest position is
that this native needs a modelled-miss path in the port before it can be replayed at all.

Three traps, all of which produced plausible-looking results:

1. **Big-endian tag bytes are the big-endian form of the constant, not the reversed ASCII.** `Ilff_FindFormChunk`
   compares `bswap32(stored)` with the tag constant. For `'LOVS'` (0x53564F4C) the stored bytes are `SVOL`, but my
   first attempt wrote `tag[::-1]`, which happens to be right for LOVS and wrong for `'PALF'` (0x50414C46, stored
   ASCII `PALF`). The first palettes run therefore returned null in every case and "passed" - a comparison that
   agreed only because both sides found nothing. A harness that never exercises the positive path is not evidence;
   after fixing the layout the cases return `ptr` and the null boundary is the absent-chunk case.
2. **Duplicate case ids silently corrupt the gate's recomputation.** Two LOVS cases differed only in form type,
   which the id format omitted, so the gate deduplicated them and recomputed 7/1 against an 8/8 artifact. The
   behavioral artifact looked fine standalone; the gate is what caught it. Case ids must be unique across all
   distinguishing inputs.
3. **Pointer returns cannot be compared as numbers** across two processes; both runners compare a pointer class
   (`ptr`/`null`) and say so in the method, with the state writes (count at +0x44, palette pointer at +0x3C) carried
   as separate observables where the port actually writes them.

Verified count 73 -> 75; paired matches 79 -> 81; candidate-unreachable 12 -> 10; mismatches 0.

## Round 120: five paths that are walls, and one that must not be forgotten

No native was promoted this round. Five separate approaches were tried and each ended at a concrete wall; the
value of the round is that the walls are now documented instead of being rediscovered.

1. **A capture at another level does not help the retail-unobserved natives.** `scripts/verify_natives_batch.py`
   defaults to `--level 4`, so a level-4 paired capture was tried. Retail reached only startup: 115 of the planned
   natives were inconclusive, because the level-5 demo scene is what exercises the terrain/model/script path. Level 5
   remains the only productive capture level.
2. **The twin model still blocks engine-backed records.** `Display_ComputeScreenAspect` (0x00491D40) and
   `Transcontext_BuildViewBlock` (0x00497D70) already match their runtime classes and now have 8/8 angr differential
   runners that compare the raw x87 bit patterns of every field the bodies write. They still cannot be promoted: the
   strict gate requires a canonical `scripts/GameNatives/<address>-<name>.cpp` source, and the only copies of those
   two twins were the mislabeled ones deleted in round 116. Recreating them does not help either, because the staged
   sweep rejects a staged GameNatives file whose record's implementation path is the engine source. Repointing
   `game_natives` at the engine file is rejected as "canonical GameNatives source path is missing". This is the
   unresolved twin-model design decision from round 116, now with a second concrete pair of records behind it.
3. **`DynObj_LoadPackageMeshes` cannot be fixtured without breaking a verified neighbour.** A modelled fault path
   (`MODS` lookup miss returns null, matching retail's fault-handler return) makes the runner pass 8/8, but the miss
   necessarily calls the already-verified `Ilff_FindFormChunk` with a tag that is absent, which adds a `null` return
   class to that callee. The paired capture then reports a mismatch for `Ilff_FindFormChunk` (569 retail observations
   all `ptr`). A fixture must not change the observed class set of a verified callee, so this native stays blocked.
4. **`Flow_RunHandler` cannot be replayed under angr.** Retail executes its null-object path cleanly, but the
   candidate body is compiled with gcc's SJLJ exception model: the entry point calls `__Unwind_SjLj_Register` before
   anything else, and the simulated run diverges into libgcc/libc exception machinery that angr cannot decode
   (`IR decoding error at 0x455E7A` inside `pthread_once`). Hooking the timer does not help because the unwind
   registration is unconditional. Its runtime evidence (3430 paired calls, retail class `{null}`) is good, but the
   angr rung cannot be satisfied for this body as compiled.
5. **Retail observations vary between capture runs, so the canonical retail trace must be reused.** Working on (3)
   required a fresh paired capture, and that capture reported six "mismatches" that were pure retail-run variance:
   retail exercised fewer texture-modifier pushes and different resource sizes than in the canonical run (for example
   `TextureModifier_Push` retail `{1..5}` against candidate `{1..8}`). All six natives matched again under the
   canonical trace. Every capture in this workflow must therefore pass `--reuse-retail` with the canonical trace
   (`build/runtime-traces/mvp-frida-pair-lovs.json` at the time of writing) so that retail observations are held
   fixed and only the candidate side is allowed to vary. A capture without `--reuse-retail` is a diagnostic, never
   evidence.

Verified count stays at 75; the paired-match and candidate-unreachable counts below are from the canonical trace, not
from the diagnostic level-4 or fresh-retail runs.

## Round 121: the twin model is resolved as a support-source relationship

The prerequisite recorded in round 117 ("either the runner gains a twin compile mode, or the gate learns about
support sources") is now resolved in favour of the second option, which is the honest reading of what the two
fields mean: `implementation` is the translation unit the candidate compiles, and `game_natives` is the canonical
address-qualified retail transcription. They are different artefacts with different jobs, so requiring them to be
the same path was the defect.

`scripts/native_gate.py` now recognises that relationship. When `--staged` encounters a `scripts/GameNatives/`
source whose record's implementation is a *different* file and whose recorded `game_natives.path` is the staged
file, it validates the source on its own contract instead of demanding that it be the implementation: the recorded
path must match, the digest must match, the filename must be address-qualified with the record's address, a
signature must be present, and the record must be at or above the requested threshold. It reports
`canonical support_sources` in the report and emits one warning per file naming the implementation it supports.
Staged `engine/` sources are unchanged: they must still be the record's implementation.

With that in place the two blocked records were finished properly rather than worked around:

* `Display_ComputeScreenAspect` (0x00491D40) and `Transcontext_BuildViewBlock` (0x00497D70) each gained a
  canonical transcription at `scripts/GameNatives/display/<address>-<name>.cpp`, written from the retail
  disassembly and the recorded CFG evidence, and explicitly marked as a reference source that is not compiled
  into the candidate.
* Each record gained an angr differential runner (`scripts/view_block_diff_runner.py`) that hooks retail's
  `Display_GetActiveMode` to the retail display object, installs the equivalent host mirrors on the candidate
  side, and compares the raw IEEE bit patterns of every value the bodies produce or write: the aspect factor in
  x87 `st0`, the transposed block words, the three camera doubles, fovX, both derived screen FOVs, the two
  origins and the two flag words. Both runners pass 8/8 with no tolerance.
* Both strict gates pass (CFG 1.0, semantic review, behavioural tests, Frida probes), so both are back to
  VERIFIED - this time reproducibly, because the twin sources and the runners are in this tree.

Verified count 75 -> 77. The three remaining twin-bound records (`Texture_GetActiveBackendId` 0x004B0F80 and the
two above) now have a reproducible model to follow; `Texture_GetActiveBackendId` still needs its own runner.

## Round 122: the last twin-bound record is verified

`Texture_GetActiveBackendId` (0x004B0F80) was the third and final record demoted in the round-116 audit for having a
twin binding that could not be reproduced. With the support-source model from round 121 in place it needed only the
evidence its contract actually requires, because the retail body is a single load of `g_a94a64`
(retail 0x00A94A64).

`scripts/texture_backend_id_diff_runner.py` writes eight different values into that slot on each side - zero, one,
negative one, two, three, seven, one hundred and the maximum signed value - runs both bodies, and compares the
returned word. The cases deliberately include the three classes the paired capture observed (`null`, `small:-1`,
`small:1`) plus values the runtime never produced, which is what makes the evidence prove the read comes from the
slot rather than from a constant or an address left over in a register. All eight pass, and the record's
`game_natives` now points at the real address-qualified transcription
(`scripts/GameNatives/sys/004B0F80-Texture_GetActiveBackendId.cpp`, digest-bound) instead of the header that never
existed.

The record is VERIFIED again: strict gate clean on CFG, semantic review, behavioural tests and Frida probes. The
compiled implementation is unchanged (`engine/src/retail/texture/texture_loaders.cpp` digest matches its recorded
evidence), so no neighbouring record on that file had to be re-evidenced.

Verified count 77 -> 78. All three twin-bound records from the round-116 audit are now verified reproducibly in this
tree (0x00491D40, 0x00497D70, 0x004B0F80).

## Round 123: the remaining set is transcription-bound, not harness-bound

An attempt to verify the last three runtime-observable natives ended with a correction to an earlier conclusion and
a precise map of what blocks the other 42. No native was promoted, and describing the set as "evidence plumbing" was
wrong: plumbing is now solved (the support-source model, the paired capture, the canonical trace), and what remains
is missing retail semantics.

**Flow_RunHandler: the wall is the port's scope, not SJLJ.** Round 120 recorded the blocker as angr being unable to
decode the candidate's gcc SJLJ registration. That is true but secondary. The retail body is 1213 bytes with two
`ret` sites, both of which return whatever a preceding callee returned (0x004010A0 or 0x00406220); the observed
`null` class came from those callees returning 0. The port deliberately *throws* on the four retail branches it does
not model (state byte at 0x005C8E00, the two pause flags, and `ticks <= total`), so even if the SJLJ registration
were hooked away, a differential run would compare retail's 0 against the candidate's exception on every reachable
input. Verifying this native requires transcribing the flow state machine, not a better harness.

**Terrain_LoadTexturesAndLightmaps: a real fidelity gap that must not be fixed blind.** The paired capture reports
retail `{small:10}` against candidate `{small:1}`. The retail tail explains it: the body ends by calling 0x004B6380
with the address of the lightmap-stack scratch variable and returns that call's result, which is the number of
lightmap pages registered (10 for the level-5 presentation). The port's host bridge returns the boolean success of
`Terrain_LoadLightmaps`. The correct transcription is the page count, but a rebuilt candidate invalidates the
candidate digest every Frida report is bound to, and this body cannot be replayed under angr (it performs file I/O
and D3D uploads through 0x004B23B0/0x004B2610/0x004B2670), so the fix would be a blind change with no evidence that
its 10 matches retail's 10 in any other scenario. It stays recorded as a scope gap.

**The 42 remaining, by what would actually unblock them:**

| group | count | what unblocks it |
|---|---|---|
| retail never calls it at level 5 (menu, config, camera, FPU, error paths, `Main`) | 24 | a retail scenario that reaches those paths - another level, a menu, a shutdown/reload drive - not a harness change |
| candidate never calls it; retail class known | 10 | fixture calls, but each must not add a return class to an already-verified callee (the `DynObj_LoadPackageMeshes` precedent); three are 4-6 KB ANALYZED stubs that throw and need transcription first |
| observed, class sets differ (`Terrain_LoadTexturesAndLightmaps` 10 vs 1; `Flow_RunHandler`) | 2 | transcription of the retail semantics, then a rebuild and a fresh paired capture |
| source exists but is not linked into the build | 5 | build/link plumbing for the translation units |
| observed and matched, needing only a replayable runner | 1 (`Terrain_Load`) | the body loads files and models, so angr cannot replay either side; needs a host-side differential that is still end-to-end |

That last group is why the count is not growing: everything cheap has been verified. The next increase requires one
of the three expensive commitments - transcribe a large body, drive retail into new scenarios, or extend the
evidence model to cover host-side I/O - and each is a multi-round project rather than a harness fix.

## Round 124: the startup natives were invisible because the harness attached after them

The 24 "retail never calls it" natives were not all unreachable: the capture harness launched the image, polled
`frida-ps` until it appeared, and only then attached. Every native executed between process entry and that attach was
unobservable by construction - which includes the FPU initialisation family that the retail entry-point chain calls
before the first frame.

The harness now supports `--spawn`: it asks the bottle's Frida server to spawn the image suspended, installs the
hooks against the suspended process, and only then resumes it. The same probe and the same settle/observe logic run
from process entry, so startup and the demo are one observation window. On the first spawn-mode capture retail
observed 97 natives instead of 91 and the candidate 93 instead of 88.

Five natives became verifiable from that one change, all previously recorded as retail-unobserved:

| native | retail startup | candidate | evidence |
|---|---|---|---|
| `FP_ControlWordToFlags` (0x004AC66B) | 1 call, class `ptr` | 2 calls, class `ptr` | angr 8/8 |
| `FP_FlagsToControlWord` (0x004AC6FD) | 1 call, control word `0x023F` | 2 calls, `0x023F` | angr 8/8 |
| `FP_MergeControlWord` (0x004AC620) | 1 call, class `ptr` | 1 call, class `ptr` | angr 8/8 |
| `FP_MergeControlWordNoDenormal` (0x004AC655) | 1 call, class `ptr` | 1 call, class `ptr` | angr 8/8 |
| `AppContext_SetIsActive` (0x0048F140) | 1 call, class `small:1` | 1 call, class `small:1` | unicorn 8/8 |

The first fixture attempt failed for a reason worth recording: it called the merge helpers *and* the two flag
mappings. The merge helpers convert through the mappings, so those fixture calls added observations of
`FP_FlagsToControlWord` with a different result (`3647` next to `575`) and the native's observed class set diverged
from retail's. This is the `DynObj_LoadPackageMeshes` trap again: a fixture may not add a class to a native. The
fixture now replays only the two leaf mappings; the merge helpers are reached naturally by the candidate's own FPU
initialisation, one call each, which is exactly the retail pattern.

**The strategy for the next natives is visible here**: the pair of `FP_*` records was also carrying a stale identity.
The MVP target list still names 0x004AC620/0x004AC655/0x004AC66B/0x004AC6FD/0x004A7157 as `Camera_SetFieldOfView_24`,
`Camera_ApplyLineFilter_24`, `Camera_ApplyBandFilter_24`, `Camera_ApplyWideScreen_24` and `Camera_ApplyBandFilter_10`.
Those bodies perform no camera work - the spawn capture shows them executing once during FPU setup, and the angr
replay reproduces the retail return values exactly as x87 control-word conversions. The records keep the verified
FPU identity and four now have real address-qualified transcriptions under `scripts/GameNatives/camera/` (the
directory follows the stale catalog path; the comments state why). `FP_SetDefaultPrecision` (0x004A7157) also matches
the reference, but stays unpromoted: it takes no arguments, so an angr differential cannot present eight distinct
inputs, and its only input is the x87 control word, which angr models as one fixed default.

The captured evidence for all five comes from a merged trace (`scripts/merge_mvp_captures.py`): the canonical capture
holds the retail side fixed for every native it already observed and only supplies startup observations for natives
it never reached, while the candidate side always comes from the newest image. A fresh retail run alone is not used
as evidence because of its documented run-to-run variance.

Verified count 78 -> 83.

## Round 125: a capture alias was manufacturing a match

The paired capture resolves each native's candidate address through `scripts/mvp_candidate_symbol_aliases.json`. The
entry for 0x005179C0 (`Iff_GetChunkPayload`) named `igi::retail::sys::Ilff_GetChunkPayload` - which is the body of a
*different* native, 0x004B74D0. The alias existed because the port had no model payload getter at all: the record
named `engine/src/retail/model/iff_chunk_tree.cpp` as its implementation, but that file only carried a provenance
comment for the address, not the function. The alias made the capture look at the sys getter, whose startup return class
happened to equal the retail class, so the native appeared to match while nothing at its own address was observed. The
same aliasing made a fixture experiment look like a fix for this native while it actually changed the other one's class
set; the lesson is that a matching class is not evidence of identity.

Two concrete repairs:

- `IffChunk_GetPayload` is now implemented in `engine/src/retail/model/iff_chunk_tree.cpp` beside the other chunk
  helpers: three instructions, `eax=[esp+4]; eax=[eax+1Ch]; ret`, the payload address standing for the retail's
  pointer. The candidate image now exports it (`0x0040E120`), and the alias names it.
- the fixture replays the sys getter's two observed inputs (payload pointer and null) and observes the model getter
  once on a chunk with payload bytes, so each native keeps exactly the class set retail produced: sys `{null, ptr}`,
  model `{ptr}`.

The record's implementation binding is corrected to the real file, and its behavioral report (9/9 cases, retail under
unicorn against the compiled implementation), build report, semantic review and frida report are re-bound to that
file's digest. The merge now shows 88 matched natives and **zero mismatches**.

Verified count 83 -> 84; matched 87 -> 88; mismatches 1 -> 0.

## Round 126: the native with no arguments does have an input

`FP_SetDefaultPrecision` (0x004A7157) was left unpromoted because "it takes no arguments, so an angr differential cannot
present eight distinct inputs". That reasoning confused *stack arguments* with *inputs*. The function sets the x87
default precision; its input is the x87 control word in effect when it is called. The paired runtime capture had
already shown the retail call (one startup call, returning the same class on both images), so the native was reachable -
only its behavioural evidence was missing.

`scripts/fpu_control_word_diff_runner.py` drives that input instead of inventing stack arguments: retail executes its
own bytes under unicorn with the control word written before entry, and the compiled implementation runs in a harness
that loads the same word, calls the function, and reads the resulting word back. Eight control words cover the retail
default, 24-bit / 53-bit / extended precision, all four rounding modes, an all-unmasked word and an all-ones word. The
returned flag word and the post-call control word are compared exactly; all eight cases pass.

One honest normalisation is recorded in the report, not hidden: bit 6 (0x40) is reserved in the x87 control word, and
the Wine x87 implementation under this bottle reports it as set after `FLDCW` while unicorn keeps it clear. The runner
masks that reserved bit on both sides and says so in `method`; every defined bit is still compared, so a one-bit error
anywhere in the contract fails the case.

`FP_SetDefaultPrecision` is verified (status VERIFIED, angr CFGFast for the entry, unicorn differential for the
behaviour, spawn-mode capture for the runtime call). The other four members of the FPU family keep their angr runner,
so this new runner changes no existing record.

Verified count 84 -> 85; matched 88 with zero mismatches, so three matched natives remain unverified
(`Flow_RunHandler`, `Terrain_LoadTexturesAndLightmaps`, `Terrain_Load`), all three because their ports are dispatch
placeholders or host file-stage bridges rather than transcriptions.

## Round 127: a host-injected backend makes a dispatch native verifiable

`Device_CloseStream` (0x004B1690) had 350 retail calls returning `{null}` but zero candidate calls: the port is complete
and the generic runner already replayed both branches under unicorn, so only a runtime observation was missing. The
native dispatches through a table the host fills in (`Device_SetReadFileBlock`), and every call it can reach returns
`ptr` or `null`, so a fixture cannot perturb a verified callee's class set.

The fixture installs a void operation on an unused device id (15) and calls the native once on the dispatch branch. The
stub deliberately leaves EAX at zero with an explicit `xor` because the capture compares the return *class*, and the
retail operation left EAX zero over all 350 calls. The special global-stream branch is not replayed at runtime: it
would leave EAX to the compiler's address calculation, and the behavioural runner already covers that branch on both
sides, so the runtime fixture stays on the branch whose register state is guaranteed.

The candidate now observes the native (one call, class `null`) and the merged trace shows 89 matched natives with zero
mismatches. Verified count 85 -> 86.

**What the remaining nine candidate-unreachable natives actually need**, in the order they were ruled out this round:

- `QTask_RegisterClasses`: the port throws on registrars outside MVP scope, so it needs transcription, not a fixture.
- `Terrain_TraverseContourQuadtree`, `Terrain_DecodeChunkCommands`, `Terrain_Draw`: placeholder ports that throw; the
  retail bodies are 4KB-6KB of rendering/decoding code.
- `Resource_RegisterIlffContainer`: a fixture is possible in principle (all callees return only `ptr`), but its
  malformed-input branch is `jmp $` (an infinite loop), so a layout mistake hangs the candidate capture instead of
  failing a test; it needs the container layout pinned down before it is worth the risk.
- `Texture_LoadLoopTextureFile`, `Resource_FreeNode`, `DynObj_LoadPackageMeshes`, `DynObj_LoadModelTextures`: each is
  reached through a file/asset path, and the natural paths cascade observations into other natives that are themselves
  unverified (for example the texture-loader count>0 path calls the loop texture loader), so a fixture would trade one
  verification for a new class-set divergence elsewhere.

## Round 128: a container fixture, and four routes ruled out by their own callees

`Resource_RegisterIlffContainer` (0x004B60F0) had 29 retail calls returning `{small:1}` and none from the candidate.
Its success path is reachable from a hand-built container: the ILFF readers take all their offsets from the container
itself (declared size at +0x04, alignment at +0x08, first chunk at `(alignment + 0x13) & ~(alignment-1)`, sibling
distance at +0x0C, payload at `(alignment + relative + 0x0F) & ~(alignment-1)`), so a 0x100-byte buffer with one
`NAME` chunk and one `BODY` chunk walks the loop once, registers one child node through `Resource_CreateNode` and
returns 1. Every callee it touches keeps its verified class set: `Ilff_GetFirstChunk`, `Resource_CreateNode`,
`HeapList_Insert` and `Mem_Alloc` return `ptr`, and `Ilff_GetChunkPayload` returns `ptr` here (retail observed
`{null, ptr}`). The malformed-input branch is `jmp $`, so the fixture is deliberately the well-formed path only.
The candidate now observes the native (one call, `small:1`), the merged trace shows 90 matched natives and zero
mismatches, and verified count is 86 -> 87.

The rest of the group was ruled out this round, each for a reason found in code rather than by guessing:

- **Texture_LoadLoopTextureFile** returns `Resource_Unload(path)`, which returns the node kind for kinds 1 and 3 and
  otherwise the free hook's answer. Retail observed `{small:1, small:4454}`; 4454 can only come from the free hook, so a
  fixture would call `Resource_FreeNode` with a value outside its verified retail set (`{ptr, 1, 10, 11, 12, 14, 15,
  152}`) and turn a verified native into a mismatch. Not taken.
- **DynObj_LoadPackageMeshes** needs a real `MODS` model name, which cascades into `DynObj_LoadModelMesh`; that port
  dereferences the payload of a missing `.mef` resource without a null guard (retail crashes there too, but the
  candidate would crash before the capture records anything), so it needs genuine model assets.
- **DynObj_LoadModelTextures** returns its payload pointer on the zero-count path while retail returned null, so the
  zero-count shape is a mismatch; the counted path cascades into the texture loader above.
- **Flow_RunHandler** is not "nearly done": its port is a guard that computes the level scale and discards it, while the
  retail body (1213 bytes, 146 CFG nodes) dispatches a per-frame handler registry at 0x00AFA7E0 and calls through a
  table keyed by each record's word at +0x1C. Verifying it needs the dispatch transcribed, not a runner.
- The three terrain stubs are 143-146 CFG nodes each (5.3KB-6.5KB of code) and remain transcription work.

Also noted, not changed this round: `engine/src/retail/script/humanplayer_defines.cpp` defines three natives
(`Config_Read`, `DefineHumanPlayerGeneral`, `HumanPlayer_LoadParameters`) but is not in the build, which is why they
show as unlinked. Retail never called them in any captured run, so linking them would only turn "unlinked" into
"retail-unobserved"; the build omission still needs fixing on its own merits.

Input injection into the retail window was attempted to reach the menu-only natives and is not available here: the
spawned Wine process cannot resolve `user32!keybd_event` through Frida (`hasKey` false), so key events cannot be
injected from inside the process, and the alternatives (screen capture based navigation) do not provide an oracle.

## Round 129: the loader pair, verified by controlling the free-hook replay

The previous round ruled `Texture_LoadLoopTextureFile` out too quickly: I assumed its `4454` return would have to come
from `Resource_FreeNode`. It does not. The native returns `Resource_Unload(path)`, and that returns the *host free hook's
answer* for any node whose kind is not 1 or 3, while `Resource_FreeNode` only does pool bookkeeping and never touches that
hook. The hook is replayed from a seven-value array by the scene fixture, so the answer is a function of call order - and
therefore controllable.

Retail also gave the model loader a clean shape. Disassembly of 0x004CF790 and 0x004CF330 showed both return EAX
untouched on their zero-count paths (the `FindFormChunk` payload pointer) and zero on their counted paths (the countdown
reaching 0). The ports match that exactly, so a single-texture input returns `null` on both sides.

The fixture drives the pair through the wiring retail uses: a LOOP version-2 buffer is registered under
`LOCAL:textures/ABCDEFGH.tex`, the eight-byte name makes `sprintf` return 27 (a length retail already observed for it),
and `DynObj_LoadModelTextures` walks its TEXF payload, calls the loader once and returns the countdown's zero. Four
throwaway `Resource_Unload` calls then shift the replaying hook from index 7 to index 12, so a second direct loader call
gets `4454`. Every consumed value stays inside a class set retail already produced:

- the loader ends with `{small:1, small:4454}` - its retail set exactly;
- `Resource_Unload` receives 1, 10, 15, 206, 2666 and 4454, all already in its retail set;
- `Texture_UploadMipmapPacket` receives 18 and 170 from the secondary callback replay, both already observed;
- `sprintf` receives 27, already observed;
- `Ilff_FindFormChunk`, `Mem_Alloc`, `Resource_Load` and `Resource_RegisterBuffer` keep their pointer classes.

Two mistakes were found by the runtime evidence rather than by reading: the TEXF tag bytes were transposed (the lookup
byte-swaps the stored dword, so the buffer must store 0x46584554, not 0x54455846), and the fixture ran before the mipmap
callback slots were installed, so the upload returned 0 and added a `null` class retail never produced. Both showed up
as a candidate crash and then as a single mismatch, not as a silently accepted result.

The candidate now observes both natives; the merged trace shows 92 matched natives and zero mismatches. Verified
count 87 -> 89, and the candidate-unreachable group is down to six.

## Round 130: Resource_FreeNode, reached by engineering the pool counts it reports

`Resource_FreeNode` (0x004B5C90) had 1153 retail calls and none from the candidate, and the reason was not a missing
call site. Two facts had to be established first, both from the binary and from a direct retail instrumented run:

1. `Resource_Unload` (0x004B6380) calls `Resource_FreeNode` directly and returns its EAX. The candidate port reaches it
   through `g_sys_free_memory_16`, but that hook's default wiring is `FreeMemory16Thunk`, which calls
   `Resource_FreeNode`. So the indirection is a test seam, not a divergence - the fixtures replace the hook to replay
   retail's unload classes, which is also why candidate `Resource_FreeNode` had zero calls.
2. `Resource_FreeNode` returns the new active count when the freed node was the last active slot, and the *relocated
   node's address* otherwise (it swap-pops the last active node into the freed slot). The observed set
   `{ptr, 1, 10, 11, 12, 14, 15, 152}` is therefore pool bookkeeping, not file data.

Because of (1), a fixture can call `Resource_FreeNode` directly and leave `Resource_Unload`'s already-verified classes
untouched. Because of (2), the classes are reachable by controlling the active count: a return of `c` needs the freed
node to be the last active slot with `c+1` nodes live. The fixture resets the pool to 200 slots, creates kind-2 nodes
through `Resource_CreateNode`, and frees in a sequence that forces 2 -> 1, 11 -> 10, 12 -> 11, 13 -> 12, 15 -> 14,
16 -> 15, 153 -> 152, then frees the oldest node while 152 are live so the swap-pop returns a relocated pointer. The
candidate's first eight distinct classes are exactly retail's eight.

Merged trace: 93 matched natives, zero mismatches. Verified count 89 -> 90, candidate-unreachable group down to five.

Also worth recording because it constrains how the evidence should be read: the probe keeps only the first eight
distinct return classes per native (`MAX_CLASSES = 8`). Retail produced more than eight classes for the pool natives,
so the recorded sets are a prefix of the real behaviour, and a candidate fixture must reproduce that prefix - which is
what the engineered sequence above does.

## Round 131: DynObj_LoadPackageMeshes made reachable, verification still gated on its runner

`DynObj_LoadPackageMeshes` (0x004CF330) had one retail call and none from the candidate. It is an 81-byte loop, so
reachability was the whole problem: it reads a `MODS` form from the context, walks NUL-terminated model names, loads
each one through the mesh loader and appends the record to an intrusive list at `context + 0x10`.

A retail MEF file settled the file layout instead of guessing: the `MESH` chunk tag is stored byte-reversed
(`0x4D455348`, i.e. the bytes `HSEM`) because the reader compares the raw dword, and the payload begins at chunk + 0x24
with `0x3DCCCCCD` (0.1f) as its version word. The fixture registers a synthetic local mesh under
`LOCAL:models/MODELNAME1.mef`, builds a package form whose `MODS` payload names it, initialises the intrusive list
anchor, and calls the loader. The ten-byte model name keeps the mesh loader's `sprintf` result at 27, a length retail
already observed.

Result: the native is now observed on both sides with a matching class (`null`), the merged trace holds at 94 matched
with zero mismatches, and the candidate-unreachable group is down to four.

It is **not** verified yet, and the reason is evidence rather than behaviour: the record's registered behavioural runner
(`scripts/native_diff_runner.py`) returns `unsupported native address: 0x004CF330`. The dual-image unicorn harness in
`scripts/resource_sys_diff_runner.py` is the right home for a case like this - it already executes retail bytes and the
compiled port side by side and can simulate the three callees the loop calls. That case is the next step; until it
exists the register keeps this native out of the proven set, which is the honest state.

## Round 132: the package-mesh loop gets a differential, so its reachability becomes evidence

Round 131 made `DynObj_LoadPackageMeshes` (0x004CF330) reachable but could not verify it: the record's runner returned
`unsupported native address`. The loop is now covered by a case in the dual-image harness
(`scripts/resource_sys_diff_runner.py`), which executes the retail bytes and the freshly compiled port side by side
under angr with identical modeled boundaries.

What the case models, and why: the loop calls three things - `Ilff_FindFormChunk`, the mesh loader and
`HeapList_Insert`. The form walker and mesh loader are replaced by the same answers on both sides, so the comparison
covers the loop's own work: the count test, the NUL-terminated name walk, the per-name call order and the return
identity. `HeapList_Insert` is deliberately left **native** on both sides, exactly as the container case does, so
retail's own splice bytes and the port's own splice both run and the anchor chain is compared for real.

Eleven cases: one/two/three/eight models; no models; a single-character name; a 63-character name; a backslash name; an
empty name; an empty second name; and a negative count. The last four matter because they are the paths where a
fixture that only exercises "normal" input would agree by accident - the empty-name walk advances by one byte, and a
non-positive count returns the form payload pointer instead of null.

All eleven cases agree on return class, inserted-node count, anchor chain and modeled call trace. Verified count
90 -> 91; the candidate-unreachable group is now four.

## Round 133: parallel-branch sync, and who owns what

`codex-smart-workflow` is a second reconstruction lane. Its register holds 883 records but none at
VERIFIED (795 STRUCTURALLY_VERIFIED, 2 BUILD_VERIFIED, 1 RUNTIME_TESTED), so there is no
verification duplication to worry about; what it has in abundance is transcriptions this branch
does not, including complete terrain bodies (quadtree 1004 lines, draw 1303, loaders 2148 versus
1219 here) and the QTask creation path.

`scripts/codex_sync_report.py` turns the divergence into a pull list
(`docs/fidelity/codex-sync.json`). Of the 29 remaining MVP natives: 16 have byte-identical
implementation files on both branches (nothing to pull), 10 need a function-body merge because their
file also implements natives verified here (evidence binds to the file's SHA-256, so a whole-file
swap would silently invalidate those records), and 3 are safe whole-file adoptions.

Ownership is recorded in `docs/fidelity/parallel-work-split.md`: the parallel branch is actively
editing the terrain lane, QTask, the QVM error path, the startup accessors, AnimIff and the
humanplayer-defines link, so those are left to it. This branch keeps the fourteen natives that are
blocked on **observation** rather than transcription - the config callback cluster, task view,
camera/matrix/fog, loop status, string formatting, error display and the new-handler path.

Retail VA validation with radare2 cleared one suspicion: all fourteen VAs are real function entries.
The config five are absent from `aaa` only because they are referenced as data pointers pushed by
`fcn.00402FF0`, and disassembly at each VA shows a real prologue. So the blocker really is scenario
coverage.

A spawned-retail capture was run to test whether the canonical capture simply attaches too late:
it observed **zero** natives (candidate observed 99 in the same run), so spawn never reaches the
retail game loop. Attaching to a running image stays the only workable mode, which means the
fourteen must be reached by driving the running game, not by moving the attach point.

Verified unchanged at 91.

## Round 134: driving the running game reaches natives the passive capture never saw

The fourteen natives left to this branch are blocked on observation, not transcription. A passive
gameplay capture never enters their paths, and a spawned retail run observes nothing at all (round
133). The missing piece was input: the retail input layer is DirectInput, so posted window messages
would be ignored, but `SendInput` from inside the game process follows the same path a physical key
press takes.

`scripts/frida_drive_keys.js` is that driver; `scripts/drive_scenario_keys.py` attaches it as a
second Frida session so the observation probe stays byte-identical and its recorded hash keeps
meaning what it says. The sequence sweeps the task/map/help keys, the pause key, movement keys and
interact keys.

The first key-driven capture already moved five natives from "retail never called it":

| native | retail calls | candidate calls | blocker before |
| --- | --- | --- | --- |
| `Terrain_TraverseContourQuadtree` 0x004D14E0 | 1242 | 0 | candidate-unreachable |
| `Terrain_Draw` 0x00512C30 | 1241 | 0 | candidate-unreachable |
| `Terrain_DecodeChunkCommands` 0x00510010 | 354 | 0 | candidate-unreachable |
| `QTask_RegisterClasses` 0x00402930 | 1 | 0 | candidate-unreachable |
| `Human_TaskViewReset` 0x004659E0 | 1 | 0 | retail-unobserved |

Two things follow. First, the retail side is no longer the blocker for these five - the candidate is.
The terrain trio and QTask registration are transcribed only in the parallel branch (they stay that
branch's lane); `Human_TaskViewReset` lives in a file both branches share, so making the candidate
reach it is this branch's work.

Second, a key-driven run is not a substitute for the canonical capture: it moves the game through
different states, so records that were already verified against the canonical run show different
call sets here (for example `Resource_FreeNode` and `Model_GetAttachmentCount`). That is expected;
those records stay bound to their own capture and only a run with the same scenario may be compared
against them.

The config cluster (0x00404590, 0x00404F00..0x00405620), `Camera_ResetMatrix`, `Matrix_InvalidateSlot`,
`Camera_UpdateFogPalette`, `String_VFormat`, `Error_Show` and `Mem_InvokeNewHandler` are still silent
on the retail side; the next scenario step must open the options/menu path and force an error path
rather than only sweeping gameplay keys.

Verified unchanged at 91.

## Round 135: Human_TaskViewReset verified end to end (92/120)

`Human_TaskViewReset` (0x004659E0) carried the whole evidence set except a probe pair: an executed
8/8 unicorn behavioral runner (Angr VEX and Radare2 ESIL do not model the x87 `fptan` in its two
tangent stores), a semantic review, and a live Angr CFG pass. The key-driven scenario from round 134
supplied the retail side (1 call, class `ptr`); the candidate side needed reachability. `RunRetailModelScene`
already called the native from the demo scene's `draw()`, which a level capture never runs, so the
fixture block that hosts the other host-injected calls now gives it an aligned 0x200 record once at
startup. The next capture saw the candidate call it exactly once with the same `ptr` class.

The merge needed care. Feeding the key-driven capture as `--startup` while the canonical run had no
retail observation for `AppContext_SetIsActive` replaced that native's good retail class
(`small:1`, class-compatible with the candidate) with the key scenario's unrelated `small:31985`,
which produced the only mismatch in the first merge. Merging with the *previous merged* capture as
the canonical retail ground truth, keeping the key capture for natives it alone observed, and taking
the candidate side from a fresh plain candidate capture resolved it: 95 matched, 0 mismatched.

`native_gate.py --file engine/src/retail/human/human_view.cpp --address 0x004659E0 --threshold VERIFIED
--cfg-threshold 0.85 --require-semantic-evidence --require-behavioral-tests --require-frida-probes`
now reports OK. The record's stale limitation text (copied from a flow native) was replaced with the
real scope: one basic block, no calls, two narrowed `fptan` tangents plus a zeroed dword, unicorn as
the oracle.

Register: 92 verified. Remaining 28: 4 candidate-unreachable (the terrain trio and
`QTask_RegisterClasses` - retail now observes all four under the key scenario, but the candidate
render/registration paths are the parallel branch's transcription lane), 16 retail-unobserved
(config cluster, camera/matrix/fog, `String_VFormat`, `Error_Show`, `Mem_InvokeNewHandler`, ...),
5 unlinked, and 3 matched-but-unverified (one partial flow transcription, two terrain loaders).

Evidence rebind: every frida-runtime report was re-emitted from the new merged capture so the
candidate digest in each record matches the rebuilt image; two spot checks (`frame_natives.cpp`
0x0048F140, `sys_resource.cpp` 0x004B5C90) still pass the full strict gate.

## Round 136: what the retail spawn capture can and cannot reach

The round-133 conclusion that a spawned retail run observes nothing was wrong for a mundane reason:
the spawn failed while a stray image still held the retail mutex. With no other `igi.exe` alive the
same command (spawn suspended, install the probe, resume) observes 97 natives including the startup
path, so the mutex rule is an operational prerequisite for spawn captures too, not just for the
games themselves.

What it does not reach is any of the 16 outstanding observation-blocked natives: the config graphic
option cluster, `Loop_SetStatusByte`, `Camera_ResetMatrix`, `Matrix_InvalidateSlot`,
`Camera_UpdateFogPalette`, `String_VFormat`, `Error_Show`, `Qvm_ReportError`,
`Mem_InvokeNewHandler`, `App_ContextGetDebugged`. They are not startup natives.

The key driver was rewritten as a timed timeline - in-game menu open, entry walk, select, option
screen navigation, back out, help/status, movement, interact - rather than a round-robin sweep, and
the capture confirms the synthetic keys land (Human_TaskViewReset still observes its single retail
call). The menu walk still produces zero calls for every one of the 16. So either the in-game menu
does not route to the graphic-option records (they may belong to the front end rather than the pause
menu), or navigation needs pointer input rather than key presses. That is the open question for the
config cluster; the error-path natives need a different lever entirely (an actual failure), and
`Mem_InvokeNewHandler` an allocation failure.

No record was promoted: verified stays at 92. The six mismatches this capture shows are scenario
artifacts in already-verified natives (the run visits different states), so the canonical evidence
base stays the round-135 merge, not this capture.

## Round 137: front end runs, but its option screens stay out of reach; register rebuilt

Retail does start its front end when it is launched with `window` and no level word: the window is
`IGI` at 640x508 and the capture observes 53 natives (front-end QVM loads, `sprintf`,
`String_ToUpperChar`, the ILFF walkers). The earlier "Error" window seen in the same test was a
dialog left over by the previous instance, not a complaint about the arguments.

The front-end run still does not reach the config cluster. The graphic option records are script
handlers registered by 0x00402FF0 under names such as
`Config_GraphicOptionsSetPerformanceFlags`, so retail only calls them when config.qvm invokes those
names - which is the option screen, not the menu itself. Two attempts failed to open it: the timed
keyboard timeline (which provably lands, because `Human_TaskViewReset` still observes its call) and a
5x5 mouse sweep over the client area using `ClientToScreen` + `SendInput`. Both produced exactly the
same 53-native observation set, so the clicks either never reached the Wine window on its own macOS
space or missed every button; confirming which needs a screenshot, and screen recording is waiting on
a system approval we cannot give ourselves.

Register rebuild: eleven rows still carried the status of an older generation and contradicted their
own blocker line (a `retail-unobserved` row labelled `VERIFIED`). The file is now derived from the
manifest plus the current capture, so status, blocker and the verified set agree:
92 verified, 16 retail-unobserved, 5 unlinked, 4 candidate-unreachable, 3 matched-but-unverified.

Parallel branch: three of my blockers advanced there to BUILD_VERIFIED (`QTask_CreateTask`,
`Config_Read`, `Qvm_ReportError`) but none to VERIFIED, so there is still no evidence to adopt and
`Config_Read`'s link work is the one that may also unblock the option-screen path.

Operational note: a hung frida-server (frida-ps timing out) is recovered by killing the
`frida-server-x86.exe` wine process and relaunching it against the bottle; enumerate then works again.
