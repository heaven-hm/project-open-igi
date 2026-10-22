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

## The dominant gap: the candidate does not call 74 of the natives retail calls

The candidate is not stuck. It runs its frame loop and calls the dispatch more
often than retail does (`QTask_OpenEvent`: 2420 candidate calls vs 1610
retail). It simply never executes 74 of the 100 linked natives that retail
executes during the same level, for example:

| native | retail calls | candidate calls |
| --- | --- | --- |
| `Human_IsCrouched` | 11664 | 0 |
| `Human_CameraEyePosition` | 11664 | 0 |
| `Script_GetForcePrecompiledState` | 6160 | 0 |
| `Game_GetContext` | 3222 | 0 |
| `Flow_RunHandler` | 1610 | 0 |
| `Resource_FileExists` | 1439 | 0 |
| `String_FindExtension` | 167 | 0 |
| `Script_GetDebugCompileState` | 121 | 0 |

Those natives are linked and hookable in the candidate, so this is not a probe
limitation: the ported flow does not execute the script VM, the human-player
update, the resource lookup or the flow handler chain that retail runs every
frame. No amount of additional runtime evidence can verify a native the
candidate never calls; the candidate flow has to reach it first.

## The five return-class divergences

Each was checked against the retail disassembly before being called a defect,
because a function that returns nothing leaves an arbitrary EAX.

1. `0x004B0BA0 Mem_InitHeapState` — **not a defect, not comparable.**
   The retail body ends `mov dword [0xa960e0], 0x10; ret` and never writes EAX;
   the observed `small:8` is the return value of its `call 0x004B0D40`. The
   candidate explicitly returns 0. Return-class comparison is meaningless for
   this native and must not be counted as a pass or a failure.

2. `0x004037C0 GOInRemap` — **real risk, argument mapping.**
   Retail masks the third script argument (`and eax, 0x3fffffff` after
   `call 0x004B8A20`) and the observed class is `ptr`. The candidate returns
   `entry[2] & 0x3FFFFFFF` but reads the argument from its own `QvmFrameView`
   layout, and produced `small:9…24`. The mask matches the retail; the value fed
   into it does not, so the candidate's frame view does not represent the retail
   argument source.

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
- The next lever is candidate flow coverage, not more probing: 74 natives need
  the candidate to execute them before they can be verified.
