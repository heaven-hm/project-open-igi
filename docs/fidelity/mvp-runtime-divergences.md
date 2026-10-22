# MVP runtime divergences and the candidate coverage gap

Measured from the paired capture
`docs/fidelity/evidence/mvp-frida-pair-level1.json`
(retail `dea34892…`, candidate `9e4d7cd3…`, level 1, windowed, sequential).

The probe hooks every MVP native in one session per side and reduces each call
to process-independent classes: `null`, `small:<exact value>` for integers
below 0x10000 in magnitude, and `ptr` for anything else. Call counts and
per-call arguments are not compared, because retail and the candidate attach
at different points of level load and advance at different frame rates.

## Result of the level-1 paired run

| measure | value |
| --- | --- |
| MVP targets | 120 |
| resolved to a candidate symbol | 100 |
| observed in the retail run | 83 |
| observed in the candidate run | 13 |
| matched on return-value class | 4 |
| reached by both but different class | 5 |

Only `0x00401050` and `0x004CF390` currently satisfy the full
`CFG >= 0.85` + angr + behavioural + Frida requirement.

## The dominant limit: inlining, not a missing flow

A first reading of the run was that the candidate never calls 74 of the
natives retail calls. That reading was wrong and is retracted here.

The candidate does run its frame loop: it calls the dispatch more often than
retail does (`QTask_OpenEvent` 2420 candidate calls vs 1610 retail). What
differs is how the calls are emitted. The candidate is a Release build with
the retail sources inlined across translation units, so many natives have no
entry that is ever branched to: their bodies exist in the image, but every
call site was inlined into its caller.

Proof from the candidate's own dispatch: `QTask_OpenEvent` at `0x0041f930`
reaches `Game_GetContext` by reading the global directly
(`mov eax, dword [0x5d4be0]`) instead of `call 0x0041f900`. Same for
`Flow_RunHandler` `0x0041f070`, which is inlined into
`(anonymous namespace)::Flow_RunHandler_Handler` at `0x0041ed60`.

Counting direct call and jump sites in the built candidate separates the two
cases:

| | natives |
| --- | --- |
| linked in the candidate | 100 |
| with at least one direct call/jump site | 46 |
| with zero direct call sites, i.e. fully inlined | 54 |
| hook-observable and reached by retail | 38 |
| already observed in both images | 9 |

Natives retail executes at very high frequency that can never be observed by
an entry hook in this build include `String_ToUpperChar` (1443550 retail
calls), `Mem_AllocChunk` (73216), `Qvm_ExecuteInstructions` (42712) and
`Human_IsCrouched` (7382).

So absence of a hook hit does not prove absence of execution, and an
address-entry hook cannot verify an inlined native at all. The ceiling for
this probe against the shipped candidate is 38 of the 100 linked natives; the
other 62 (54 inlined plus 8 never reached at level 1) would need either a
non-inlined verification build of the same sources or a per-native typed probe
of the caller.

## The five return-class divergences

Each was checked against the retail disassembly before being called a defect,
because a function that returns nothing leaves an arbitrary EAX.

1. `0x004B0BA0 Mem_InitHeapState` — **not a defect, not comparable.**
   The retail body ends `mov dword [0xa960e0], 0x10; ret` and never writes EAX;
   the observed `small:8` is the return value of its `call 0x004B0D40`. The
   candidate explicitly returns 0. Return-class comparison is meaningless for
   this native and must not be counted as a pass or a failure.

2. `0x004037C0 GOInRemap` — **real contract mismatch in the candidate.**
   The retail body at `0x004037C0`..`0x00403860` takes its first stack argument
   as a destination buffer, calls the frame reader `0x004B8A20` three times to
   fetch script arguments 0, 1 and 2, stores them at the remap entry, masks only
   the third with `0x3FFFFFFF` before storing it, increments the slot counter at
   `0x00BC2358 + slot*4`, then copies 24 bytes (six dwords) of a stack-local
   struct into the destination with `rep movsd`, and returns the destination
   pointer itself.

   The candidate is declared `std::uint32_t GOInRemap(void* qvm_context)`, reads
   the three values through its own `QvmFrameView` layout instead of the retail
   frame reader, and returns the masked third value. That is why the observed
   classes disagree: retail returns a pointer, the candidate returns a small
   integer. The candidate's comment and manifest note ("returns the third
   argument masked to the low 30 bits") describe behaviour the retail does not
   have, so the disagreement is a defect in the record and in the port, not a
   probe artefact. Fixing it needs the two-argument ABI, the `0x004B8A20` frame
   reader, the 24-byte destination copy and the returned destination pointer.

3. `0x004CF080 DynObj_LoadModelTexturePackage` — **open.**
   Retail's last EAX write is `dec eax` on `[esp+0x1c]`, and the observed class
   is `null` (0), consistent with a zero count. The candidate returns a pointer.
   Either the return type is wrong or retail genuinely returns a count that was
   zero for this run; needs a typed probe of the returned field.

4. `0x004C6670 Terrain_CreateMaterial` — **state, not logic.**
   Both sides return a pointer in principle; the candidate returned `null`
   because its material table `g_c28fe0` is empty at that moment, so
   `&g_c28fe0[0]` on an empty container is null. The candidate is not loading
   the terrain materials retail loads. Compare again once the candidate scene
   populates the table.

5. `0x004C5F00 Terrain_LoadTexturesAndLightmaps` — **known scope.**
   Retail returns status `small:10`; the candidate returns a boolean 1 from a
   host-backed bridge whose own header records `TRANSCRIBED (host-backed)` and
   "retail lightmap stack layout and texture-upload sequencing remain
   unverified". The manifest keeps it at ANALYZED. The runtime evidence
   confirms that recorded limitation rather than contradicting it.

## Consequences for the verification claim

- A native is only end-to-end verified when the Frida probe observes it in both
  images, in addition to CFG, angr and behavioural evidence.
- Reachability alone is not parity. The reports emitted from this capture say
  so explicitly in their `comparison.basis` and `limitations` fields.
- The next lever is a verification build compiled with inlining disabled
  (`-fno-inline`, `-fno-inline-functions-called-once`, `-fno-ipa-cp`) from the
  same sources. Inlining is semantics-preserving, so that build executes the
  same program while giving each native a real call site the probe can hook;
  the report must record that build's digest and state the difference.
  Alternatively, per-native typed probes can observe the inlined body through
  its caller.
