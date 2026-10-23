# Level 1 paired retail vs candidate reachability — 2026-10-23

**Scope:** the first paired retail-versus-candidate reachability capture for the 120-address
MVP native set (`scripts/mvp_native_targets.json`), level 1 (`window level1`), working
directory `Z:\Users\haseeb-mir\Downloads\IGI`, both processes spawned suspended and
terminated only by their own PIDs.

**Status: evidence capture only. No native verification status was changed.** The paired
RVA protocol was executed as requested; the retail side is valid, the candidate RVA column
is not attributable (Section 5), and a clearly separated symbol-resolved supplement gives
the candidate reachability that the RVA probe could not (Section 6).

---

## 1. Result in one paragraph

Retail executed **105 of 120** MVP addresses in a 120 s spawn-gated window
(3,652,554 hook hits). The same probe installed **100 of 120** hooks in the candidate, could
not install the other 20, and recorded **zero** hits and **zero** snapshots; a retry with an
independent ticker reproduced an agent stall after two ticks. The candidate is a normally
linked image and does **not** place these functions at retail RVAs: retail `0x00401000`
(`QTask_CreateTask`) is a bare `retl` in the candidate, retail `0x0048F530` (`Retail_Main`)
is padding inside `_d_print_comp_inner`, and retail `0x004B80B0` (`Qvm_Load`) is a NOP run.
The RVA hooks therefore landed on unrelated code and the retail-RVA hook set stopped the agent; the
candidate 0-hit column is an instrumentation artifact and is **not** evidence that the
candidate omits those paths. A supplemental capture that hooks the candidate's own symbols
resolved **111 of 120** native names (111/111 hooks installed, agent alive for 59 ticks,
12 snapshots) and observed **19** of them (5,860 hits). Corrected logical reachability is
therefore: **both 16, retail-only 89, candidate-only 3, neither 12**, with 9 of the 89
retail-only rows unmeasurable because no candidate symbol could be defensibly mapped.

---

## 2. Image, probe, and target identity

| Item | Value |
|------|-------|
| Retail image | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| Retail SHA-256 | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| Candidate image | `build-clean-win32/igi1.exe` |
| Candidate SHA-256 | `936c8f7f7bc16c12800dc4aa781ed009815602a0c904f663240fc3eb7c6af72e` |
| Candidate receipt | `build-clean-win32/candidate-receipt.json` — `0e84163b03bc712376bb457773957aee806a6baf928f7951b37515df1cf7be22` (`diagnostic-candidate-build`, `built`) |
| MVP target list | `scripts/mvp_native_targets.json` — `bbb22a22e4dc293c3fa03c00d89c5401dbc9477cb4dac0fb957907aaae21a576` (120 entries) |
| Canonical probe | `scripts/frida_level_probe.js` — `ede626093a3c84272bb7a1ff02f87b16ae377dd37fe7679c67bc41280a1398e8` |
| Requested paired probe | `build/runtime-traces/20261023T1330-level1-paired-reachability/reachability_probe_120.js` — `ead76b7651512672c4a877c17f5f1f0858c83d4f96f7590db5bd35a140b947ac` (120 targets, derived from the canonical probe; derivation recorded in `probe-derivation.json`) |
| Supplemental symbol probe | `build/runtime-traces/20261023T1357-candidate-symbol-reachability/symbol_probe.js` — `2e5785f9890e24a0820e370c817537b9fb9a0d840a15b4838000b053ae4a3ed2` (111 targets) |
| Symbol address map | `.../symbol-map.json` — `63b5bc2251f9d8d1cab0488fff5e9b48b15ea643430447a5cd65371fa75843dc` |
| Runtime | Frida 17.24.0 Python API, remote endpoint `127.0.0.1:27042` (CrossOver/Wine) |

The canonical probe was **not edited**. The requested pair used one derived probe,
byte-identical for both sides; the derivation only adds a per-hit timestamp and first-hit
sequence counter and an `rpc.exports.results` accessor, and is listed field by field in
`probe-derivation.json`. The symbol probe is a separate instrument with a different target
list and digest; it is not presented as the same probe.

---

## 3. Protocol actually followed

- **Sequential only.** Retail ran 13:30:07–13:32:10; the candidate RVA run started at
  13:40:01 after retail was gone; the RVA retry ran 13:49:19–13:51:35; the symbol supplement
  ran 13:57:00–13:59:00 and, after the manifest-name correction, 14:18:48–14:20:50. No two
  game processes overlapped.
- **Preflight and foreign processes.** No `igi.exe`, `igi1.exe`, `open-igi.exe`, or
  `open-igi-probe-*.exe` was running when each run started. Two sibling-worktree
  `open-igi-probe-*.exe window level5` captures were observed *between* runs (one exited on
  its own at ~13:27, one ran ~13:51–13:56); neither was attached to, killed, or otherwise
  touched. The symbol run waited for the second to exit before spawning.
- **Suspended spawn.** `Device.spawn(program, argv=[program, "window", "level1"], cwd=...)`
  creates the process suspended. The probe and its hooks were loaded **before**
  `Device.resume`, so CRT/static-init and pre-`Main` calls are visible; this is the gap in
  the older late-attach `early-L1.json` baseline.
- **Own PIDs only.** Retail Windows PID 1584 / host PID 87719; candidate RVA Windows 1716 /
  host 90378; RVA retry Windows 1852 / host 91899; symbol runs Windows 2008, 2076 and 668 /
  host 94133, 94840 and 98417. Every one was spawned by these drivers and terminated by them.
- **Termination note.** Frida `Device.kill` hung on this Wine setup in the retail run; the
  host PID was SIGTERM'd to unblock it, after which the Windows process was confirmed gone.
  The candidate RVA final RPC also hung; the driver killed its own Windows PID and host PID
  (Section 5).
- **No status writes.** This capture did not write `docs/fidelity/native-verification.json`
  or any other native evidence. That manifest already carried uncommitted corrections when
  the capture began (mtime 13:20:26, before the 13:30 retail run); those edits are not from
  this task and were left untouched.

### Hook counts and observation windows

| Run | Hooks installed | Failed | Observation | Snapshots | Observed | Total hits |
|-----|----------------:|-------:|------------:|----------:|---------:|-----------:|
| Retail (requested) | 120/120 | 0 | 120.235 s | 12 | 105 | 3,652,554 |
| Candidate RVA (requested) | 100/120 | 20 | 120.124 s | 0 | 0 | 0 |
| Candidate RVA retry | 100/120 | 20 | 120 s | 0 | 0 | 0 |
| Candidate symbol (supplement) | 111/111 | 0 | 120.0 s | 12 | 19 | 5,860 |

The candidate RVA probe reported `retailAddressMode: true`. That flag only compares the
module **base** with the retail image base (`0x400000`); it does **not** assert that the
candidate preserves retail RVAs. It does not.

---

## 4. Retail side (valid)

Retail loaded all 120 hooks at 13:30:09.866 and was resumed at 13:30:09.868. In 120.235 s it
executed **105/120** addresses; the per-address first-hit sequence runs 0–104.

The first hits are pre-`Main` startup and camera setup, then the retail entry point:

| Seq | Address | Native | Hits |
|----:|---------|--------|-----:|
| 0 | `0x004AA670` | Mem_AllocChunk | 74,305 |
| 1 | `0x004AA682` | Mem_AllocWithRetry | 74,305 |
| 2 | `0x004AA6AE` | Mem_AllocateRawBlock | 74,305 |
| 3 | `0x004A7157` | Camera_ApplyBandFilter_10 | 1 |
| 4 | `0x004AC655` | Camera_ApplyLineFilter_24 | 1 |
| 5 | `0x004AC620` | Camera_SetFieldOfView_24 | 1 |
| 6 | `0x004AC66B` | Camera_ApplyBandFilter_24 | 1 |
| 7 | `0x004AC6FD` | Camera_ApplyWideScreen_24 | 1 |
| 8 | `0x0048F530` | Retail_Main | 1 |
| 9 | `0x0048F140` | AppContext_SetIsActive | 1 |
| 10 | `0x0048F260` | AppContext_SetTerrainLightmapsUsed | 2 |
| 11 | `0x00498460` | Display_ClearSurfaces_16 | 84,606 |
| 12 | `0x00401000` | QTask_CreateTask | 1 |
| 13 | `0x00402930` | QTask_RegisterClasses | 1 |

The full 105-row flow order is in Section 7. The 15 addresses not reached in this window are
listed in Appendix B.

**Cross-check against the older baseline.** `build/runtime-traces/early-L1.json` observed 30
addresses after a 0.367 s late attach. All 30 are a strict subset of this spawn capture's
105; the spawn capture adds 75 addresses, including the earliest allocation and camera hits
that a late attach cannot see. There is no address that the late-attach baseline saw and the
spawn capture missed, so the new retail side does not contradict the old one.

---

## 5. Candidate side under the requested probe — the RVA convention is invalid here

The candidate RVA run installed 100 hooks and failed on 20, then ran the full 120 s window
with **0 snapshots and 0 hits**. After the window, the final `rpc.exports.results()` call
hung for 63 s; the driver killed its own Windows PID 1716 and host PID 90378 at 13:43:04,
and the call then failed with `InvalidOperationError('script has been destroyed')`. The
driver's `result` field says `exited_early`; that is its fallback label for “no final
export”, not an observed early exit. The capture window itself completed.

The retry used the identical probe plus a **separate** ticker script
(`setInterval` every 2 s, `rpc.exports.ping`) in the same agent. The ticker emitted two
ticks (2 s and 4 s after load) and then stopped; at the end of the 120 s window the ping
timed out and the probe results call errored after the process was killed. This was
reproduced from a clean preflight. The symbol run, by contrast, kept the same kind of
ticker alive for **59 ticks** with 111 hooks installed at candidate-correct addresses. The
stall is therefore caused by the retail-RVA hook set, not by spawn, resume, or the candidate
shell being unable to host an agent.

### Why the RVA column cannot be read as reachability

| Retail address | Retail native | What is actually at that address in the candidate |
|----------------|---------------|---------------------------------------------------|
| `0x00401000` | QTask_CreateTask | `c3` (`retl`) — start of `___mingw_invalidParameterHandler`; Interceptor refuses the hook |
| `0x0048F530` | Retail_Main | padding (`leal %cs:(%esi,%eiz),%esi`) inside `_d_print_comp_inner` |
| `0x004B80B0` | Qvm_Load | a run of `nop` bytes |
| `0x004B0C60` | Mem_Alloc | mid-function libstdc++ code (`movw %cx,0x2(%esp)` …) |
| `0x00401050` | QTask_OpenEvent | candidate symbol is at `0x0041F770` |
| `0x004B80B0` | Qvm_Load | candidate symbol is at `0x0043EC70` |
| `0x0048F530` | Retail_Main | candidate symbol `_Main` is at `0x0041FBF0` |
| `0x00512C30` | Terrain_Draw | candidate symbol is at `0x00452980` |

The candidate is a normally linked `i686-w64-mingw32-g++ -O3 -DNDEBUG -mwindows -static`
image; `build-clean-win32/CMakeFiles/igi.dir/link.txt` contains no `/ORDER` or other
section-order directive. `nm`/`c++filt` resolve 111 of the 120 MVP names at addresses that
differ from retail. The 20 hook failures (`unable to intercept function at ...`, `no
instruction could be decoded there`) are consistent with hooking padding, NOP runs, and
mid-instruction addresses; they are layout artifacts, not candidate absence.

**Consequence.** The requested same-probe comparison, read literally as “retail RVA vs
candidate RVA”, yields 0 both / 105 retail-only / 0 candidate-only / 15 neither. Those
numbers are reported for completeness in Section 8, but the 105 “retail-only” rows are not
attributable: the candidate may well execute the same logic at a different address. The
candidate 0-hit column must not be used to count missing candidate call sites. This is
exactly the failure mode the canonical probe warns about: “an installed hook that never
fires is not evidence of absence.”

---

## 6. Supplemental candidate capture by symbol (valid instrumentation)

To get a candidate measurement that is not defeated by the layout, the supplement resolves
each retail address to a candidate symbol and hooks that address. The retail address is the
identity; the label is only the join key. Resolution order:

1. the **manifest name at the same retail address** (the implementation catalog the
   candidate is built from). This matters: 10 MVP labels differ from the manifest name at
   the same address;
2. exact or normalized (case/underscore) demangled basename of that name;
3. the source-identified alias `Retail_Main` → `Main`;
4. the MVP label itself when the address is not in the manifest.

The MVP label is retained as the comparison key, so every row still joins to the requested
120-address set. The ten label conflicts:

| Retail VA | MVP label (join key) | Manifest name | Candidate VA |
|-----------|----------------------|---------------|--------------|
| `0x004AC66B` | Camera_ApplyBandFilter_24 | FP_ControlWordToFlags | `0x0041F150` |
| `0x004AC6FD` | Camera_ApplyWideScreen_24 | FP_FlagsToControlWord | `0x0041F210` |
| `0x004AC620` | Camera_SetFieldOfView_24 | FP_MergeControlWord | `0x0041F2D0` |
| `0x004AC655` | Camera_ApplyLineFilter_24 | FP_MergeControlWordNoDenormal | `0x0041F440` |
| `0x004A7157` | Camera_ApplyBandFilter_10 | FP_SetDefaultPrecision | `0x0041F5B0` |
| `0x00498460` | Display_ClearSurfaces_16 | FP_InitExtendedPrecision | `0x0041F720` |
| `0x004AF7B0` | Error_Show | Debug_Error | `0x0041FB60` |
| `0x0048F530` | Retail_Main | Main | `0x0041FBF0` |
| `0x004F1160` | Script_ParseEnumReal32_5 | Script_GetParseMode | `0x004522F0` |
| `0x004F82B0` | TerrainLightMap_Init_2 | TerrainLightMap_PushEntry | `0x004588E0` |

111 of 120 names were mapped, with 0 hook failures and 59 liveness ticks. The 9 names with
no candidate symbol are **unmeasured**, not “absent”: `QTask_CreateTask`, `Config_Read`,
`DefineHumanPlayerGeneral`, `HumanPlayer_LoadParameters`, `AnimIff_LoadAnimationFile`,
`Iff_FindFormChunk`, `Iff_FindChunkBelowForm`, `Iff_FindChunk`, `Iff_GetChunkPayload`.

Some retail addresses are inner points rather than function entries. `0x004F82B0` is a
`call` to `0x004F1160` that matches the `Script_GetParseMode` call site in the candidate's
`TerrainLightMap_PushEntry`; `0x004A7157` is inside the FP helper sequence. The candidate symbol entry is the closest equivalent hook point, not a
byte-identical site, so per-address counts are not a direct like-for-like measure; the
reachability question (“did this function’s code path run?”) is still answered.

**Candidate result.** 19 of 111 hooked symbols executed (5,860 hits). The frame-loop cluster
(`AppContext_GetIsActive`, `QTask_OpenEvent`, `Human_CameraEyePosition`,
`Human_TaskViewReset`, `Transcontext_BuildViewBlock`, `TransContext_SetActiveTransContext`)
each fired **823** times, i.e. the candidate ran on the order of 823 loop iterations in the
window and then stopped or idled; retail's corresponding counts over 120 s were 84,602,
84,602, 243,155, 1, 1,846, and 3,797.

The manifest-name correction changed the result. `Display_ClearSurfaces_16` is actually
`FP_InitExtendedPrecision` and fired **825** times (retail 84,606), and
`Camera_ApplyBandFilter_10` is `FP_SetDefaultPrecision` and fired once (retail 1); both are
now “both”. `TerrainLightMap_Init_2` (`TerrainLightMap_PushEntry`, retail 151,302) and
`Script_ParseEnumReal32_5` (`Script_GetParseMode`, retail 154,289) were mapped but stayed
at 0, as did four of the five FP helper labels. `Retail_Main`/`Main` and
`Error_Show`/`Debug_Error` remain at 0.

### Candidate first-hit order (symbol probe)

| # | Native | Category | Candidate VA | Candidate hits | Retail hits |
|---|--------|----------|--------------|---------------:|------------:|
| 1 | Camera_ApplyBandFilter_10 | CTR / Camera | 0x0041F5B0 | 1 | 1 |
| 2 | Display_ClearSurfaces_16 | Flow Engine | 0x0041F720 | 825 | 84606 |
| 3 | Config_GraphicOptionsGetPerfFlagsFromLevel | config.qvm | 0x0040C6C0 | 1 | 0 |
| 4 | Config_GraphicOptionsSetPerformanceFlags | config.qvm | 0x0040C5B0 | 1 | 0 |
| 5 | Loop_SetStatusByte | Flow Engine | 0x0041F760 | 1 | 0 |
| 6 | AppContext_SetIsActive | Flow Engine | 0x0041F7C0 | 1 | 1 |
| 7 | GOInRemap | humanplayer.qvm | 0x0040C7C0 | 81 | 81 |
| 8 | Mem_InitHeapState | Memory & System | 0x00422E70 | 1 | 1 |
| 9 | Terrain_LoadTexturesAndLightmaps | Terrain | 0x00460910 | 1 | 1 |
| 10 | Resource_CanonicalizePath | RES | 0x00452560 | 1 | 13426 |
| 11 | Resource_CalculatePathHash | RES | 0x004525D0 | 1 | 13426 |
| 12 | Terrain_Load | Terrain | 0x0045F3F0 | 1 | 1 |
| 13 | Terrain_CreateMaterial | Terrain | 0x00458600 | 6 | 6 |
| 14 | AppContext_GetIsActive | Flow Engine | 0x0041F7F0 | 823 | 84602 |
| 15 | QTask_OpenEvent | Flow Engine | 0x0041F770 | 823 | 84602 |
| 16 | Human_CameraEyePosition | humanplayer.qvm | 0x004224D0 | 823 | 243155 |
| 17 | Human_TaskViewReset | humanplayer.qvm | 0x00422520 | 823 | 1 |
| 18 | Transcontext_BuildViewBlock | CTR / Camera | 0x00433DA0 | 823 | 1846 |
| 19 | TransContext_SetActiveTransContext | CTR / Camera | 0x00433C10 | 823 | 3797 |

**Variant note (two model/MTP names).** The strict map prefers the symbol in the manifest
implementation namespace (`dynobj`). Those free-function entries did not fire. An earlier
run of the same supplement (archived as `candidate-symbols-ambiguousmap.json`) hooked the
`model` namespace overloads instead and saw `DynObj_LoadModelMesh` 138 hits and
`DynObj_LoadModelTexturePackage` 2 hits. So the model/MTP path was exercised through the
overloads, while the manifest-namespace entry points were not entered. The strict counts
below therefore list both names as retail-only; the archived run is retained so this is not
hidden. No hook set combined both variants, and no claim is made that the overloads are the
same implementation as the retail natives.

---

## 7. Reachability matrix in retail first-hit flow order

Column meanings: **Retail hits** from the valid spawn capture; **Candidate symbol VA** and
**Candidate hits** from the symbol supplement. `-` means the name had no defensible symbol
(unresolved). A “retail only” verdict means the candidate did not enter the mapped symbol;
Section 10 explains why that is not automatically “the logic is absent”. Rows 1–105 are in
retail first-hit order; the last 15 are retail-unobserved in this window.

| # | Retail VA | Native | Category | Retail hits | Candidate symbol VA | Candidate hits | Verdict |
|---|-----------|--------|----------|------------:|---------------------|---------------:|---------|
| 1 | 0x004AA670 | Mem_AllocChunk | Memory & System | 74305 | 0x00422FE0 | - | retail_only |
| 2 | 0x004AA682 | Mem_AllocWithRetry | Memory & System | 74305 | 0x00422F30 | - | retail_only |
| 3 | 0x004AA6AE | Mem_AllocateRawBlock | Memory & System | 74305 | 0x00422EE0 | - | retail_only |
| 4 | 0x004A7157 | Camera_ApplyBandFilter_10 | CTR / Camera | 1 | 0x0041F5B0 | 1 | both |
| 5 | 0x004AC655 | Camera_ApplyLineFilter_24 | CTR / Camera | 1 | 0x0041F440 | - | retail_only |
| 6 | 0x004AC620 | Camera_SetFieldOfView_24 | CTR / Camera | 1 | 0x0041F2D0 | - | retail_only |
| 7 | 0x004AC66B | Camera_ApplyBandFilter_24 | CTR / Camera | 1 | 0x0041F150 | - | retail_only |
| 8 | 0x004AC6FD | Camera_ApplyWideScreen_24 | CTR / Camera | 1 | 0x0041F210 | - | retail_only |
| 9 | 0x0048F530 | Retail_Main | Flow Engine | 1 | 0x0041FBF0 | - | retail_only |
| 10 | 0x0048F140 | AppContext_SetIsActive | Flow Engine | 1 | 0x0041F7C0 | 1 | both |
| 11 | 0x0048F260 | AppContext_SetTerrainLightmapsUsed | Flow Engine | 2 | 0x0041E7C0 | - | retail_only |
| 12 | 0x00498460 | Display_ClearSurfaces_16 | Flow Engine | 84606 | 0x0041F720 | 825 | both |
| 13 | 0x00401000 | QTask_CreateTask | Flow Engine | 1 | - | - | retail_only (symbol unresolved) |
| 14 | 0x00402930 | QTask_RegisterClasses | Flow Engine | 1 | 0x005996E8 | - | retail_only |
| 15 | 0x004B0BA0 | Mem_InitHeapState | Memory & System | 1 | 0x00422E70 | 1 | both |
| 16 | 0x004B0D40 | QHash_ValueSet | RES | 229 | 0x00422E60 | - | retail_only |
| 17 | 0x004B0C60 | Mem_Alloc | Memory & System | 73926 | 0x004230A0 | - | retail_only |
| 18 | 0x004AF8F0 | HeapList_Insert | Memory & System | 106814 | 0x00422DC0 | - | retail_only |
| 19 | 0x004B26B0 | Texture_DisableUpload | TEX | 685 | 0x00466C10 | - | retail_only |
| 20 | 0x004BD3D0 | Qvm_SetOpcodeHandler | QVM | 472 | 0x00452270 | - | retail_only |
| 21 | 0x004CEA80 | Render_GetActiveBackendId | Terrain | 1209 | 0x00433880 | - | retail_only |
| 22 | 0x004B59F0 | Resource_CreateNode | RES | 5636 | 0x0043B640 | - | retail_only |
| 23 | 0x004B5AF0 | Resource_CanonicalizePath | RES | 13426 | 0x00452560 | 1 | both |
| 24 | 0x004AD73D | String_ToUpperChar | RES | 1444700 | 0x00452540 | - | retail_only |
| 25 | 0x004B5B30 | Resource_CalculatePathHash | RES | 13426 | 0x004525D0 | 1 | both |
| 26 | 0x004AF960 | HeapList_Remove | Memory & System | 101706 | 0x00422DE0 | - | retail_only |
| 27 | 0x004A53B3 | sprintf | Memory & System | 14272 | 0x00422370 | - | retail_only |
| 28 | 0x004AAD10 | Resource_MatchPrefix | RES | 50942 | 0x0043B590 | - | retail_only |
| 29 | 0x004B1E90 | String_FindExtension | QVM | 167 | 0x0043EAF0 | - | retail_only |
| 30 | 0x004B5F00 | Resource_Load | RES | 4682 | 0x0043AFF0 | - | retail_only |
| 31 | 0x004B1780 | Device_GetStreamPosition | RES | 203 | 0x00433EC0 | - | retail_only |
| 32 | 0x004B17F0 | Device_SeekStream | RES | 203 | 0x00433EE0 | - | retail_only |
| 33 | 0x004B1690 | Device_CloseStream | RES | 359 | 0x00433F10 | - | retail_only |
| 34 | 0x004B5F80 | Resource_RegisterBuffer | RES | 203 | 0x0043BE50 | - | retail_only |
| 35 | 0x004B7580 | Ilff_GetChunkTag | ILFF & IFF | 21501 | 0x004524C0 | - | retail_only |
| 36 | 0x004B6380 | Resource_Unload | RES | 1214 | 0x0043AF80 | - | retail_only |
| 37 | 0x004B5C90 | Resource_FreeNode | RES | 1153 | 0x0043B7E0 | - | retail_only |
| 38 | 0x0048F340 | Script_GetForcePrecompiledState | QVM | 6160 | 0x0043EA90 | - | retail_only |
| 39 | 0x004B80B0 | Qvm_Load | QVM | 121 | 0x0043EC70 | - | retail_only |
| 40 | 0x0048F1D0 | Script_GetDebugCompileState | QVM | 121 | 0x0043EA80 | - | retail_only |
| 41 | 0x004B5B60 | Resource_FileExists | QVM | 1439 | 0x0043EAA0 | - | retail_only |
| 42 | 0x004B8A10 | Qvm_SetGlobalState | QVM | 121 | 0x0043EAE0 | - | retail_only |
| 43 | 0x004BCFD0 | Qvm_ExecuteInstructions | QVM | 44656 | 0x00452290 | - | retail_only |
| 44 | 0x004BDA00 | Qvm_PushInteger | QVM | 32517 | 0x00452240 | - | retail_only |
| 45 | 0x004B83D0 | Qvm_Cleanup | QVM | 83 | 0x0043EC20 | - | retail_only |
| 46 | 0x00402800 | Flow_CreateHandler | Flow Engine | 1 | 0x0041F110 | - | retail_only |
| 47 | 0x00405850 | Config_Read | config.qvm | 1 | - | - | retail_only (symbol unresolved) |
| 48 | 0x004037C0 | GOInRemap | humanplayer.qvm | 81 | 0x0040C7C0 | 81 | both |
| 49 | 0x004137A0 | HumanPlayer_LoadParameters | humanplayer.qvm | 1 | - | - | retail_only (symbol unresolved) |
| 50 | 0x00410070 | DefineHumanPlayerGeneral | humanplayer.qvm | 1 | - | - | retail_only (symbol unresolved) |
| 51 | 0x004ED5C0 | InputPort_Process | humanplayer.qvm | 1 | 0x00422D70 | - | retail_only |
| 52 | 0x004028A0 | Flow_SetState | Flow Engine | 1 | 0x0041F750 | - | retail_only |
| 53 | 0x0048F170 | AppContext_GetIsActive | Flow Engine | 84602 | 0x0041F7F0 | 823 | both |
| 54 | 0x00401050 | QTask_OpenEvent | Flow Engine | 84602 | 0x0041F770 | 823 | both |
| 55 | 0x00402880 | Game_GetContext | Flow Engine | 169204 | 0x0041F740 | - | retail_only |
| 56 | 0x00402260 | Flow_RunHandler | Flow Engine | 84602 | 0x0041EEB0 | - | retail_only |
| 57 | 0x004B0F80 | Texture_GetActiveBackendId | TEX | 4578 | 0x00466BF0 | - | retail_only |
| 58 | 0x004B7570 | Ilff_GetFormType | ILFF & IFF | 45 | 0x004524A0 | - | retail_only |
| 59 | 0x004B60F0 | Resource_RegisterIlffContainer | RES | 29 | 0x0043BAC0 | - | retail_only |
| 60 | 0x004B7490 | Ilff_GetFirstChunk | ILFF & IFF | 9493 | 0x00452340 | - | retail_only |
| 61 | 0x004B74D0 | Ilff_GetChunkPayload | ILFF & IFF | 19054 | 0x004523C0 | - | retail_only |
| 62 | 0x004B7470 | Ilff_GetNextChunk | ILFF & IFF | 71703 | 0x00452310 | - | retail_only |
| 63 | 0x004B7540 | Ilff_ReadContainerHeader | ILFF & IFF | 5464 | 0x00452470 | - | retail_only |
| 64 | 0x004B23B0 | Texture_UploadMipmapPacket | TEX | 1407 | 0x00466C20 | - | retail_only |
| 65 | 0x004C5F00 | Terrain_LoadTexturesAndLightmaps | Terrain | 1 | 0x00460910 | 1 | both |
| 66 | 0x005175A0 | Terrain_LoadTexturePackage | Terrain | 1 | 0x00466EE0 | - | retail_only |
| 67 | 0x004B26A0 | Texture_EnableUpload | TEX | 684 | 0x00466C00 | - | retail_only |
| 68 | 0x004B7CF0 | Texture_UploadPalettePacket | TEX | 4 | 0x00466CE0 | - | retail_only |
| 69 | 0x0048F250 | App_ContextGetTerrainLightmapsUsed | Flow Engine | 386 | 0x0041E7D0 | - | retail_only |
| 70 | 0x004C6240 | Terrain_Load | Terrain | 1 | 0x0045F3F0 | 1 | both |
| 71 | 0x004C6670 | Terrain_CreateMaterial | Terrain | 6 | 0x00458600 | 6 | both |
| 72 | 0x004C6760 | Terrain_CreateTileMap | Terrain | 8 | 0x00458710 | - | retail_only |
| 73 | 0x004CF080 | DynObj_LoadModelTexturePackage | MTP | 2 | 0x00421DA0 | - | retail_only |
| 74 | 0x004CF910 | DynObj_LoadPackageAnimations | 3D Models & MEF | 2 | 0x004219A0 | - | retail_only |
| 75 | 0x004CEAA0 | Ilff_FindFormChunk | ILFF & IFF | 618 | 0x004215F0 | - | retail_only |
| 76 | 0x004D67A0 | AnimIff_LoadAnimationFile | MTP | 6 | - | - | retail_only (symbol unresolved) |
| 77 | 0x00517890 | Iff_FindFormChunk | MTP | 406 | - | - | retail_only (symbol unresolved) |
| 78 | 0x00517940 | Iff_FindChunk | MTP | 1638 | - | - | retail_only (symbol unresolved) |
| 79 | 0x005179C0 | Iff_GetChunkPayload | MTP | 30042 | - | - | retail_only (symbol unresolved) |
| 80 | 0x005178E0 | Iff_FindChunkBelowForm | MTP | 388 | - | - | retail_only (symbol unresolved) |
| 81 | 0x004CF790 | DynObj_LoadModelTextures | TEX | 2 | 0x00467360 | - | retail_only |
| 82 | 0x004B24C0 | Texture_LoadLoopTextureFile | TEX | 683 | 0x00466D30 | - | retail_only |
| 83 | 0x004CF870 | DynObj_LoadPackagePalettes | TEX | 2 | 0x00421B60 | - | retail_only |
| 84 | 0x004CF330 | DynObj_LoadPackageMeshes | 3D Models & MEF | 2 | 0x00421C40 | - | retail_only |
| 85 | 0x004CF390 | DynObj_LoadModelMesh | 3D Models & MEF | 604 | 0x00421660 | - | retail_only |
| 86 | 0x004B7590 | Ilff_FindChunk | ILFF & IFF | 9080 | 0x004524E0 | - | retail_only |
| 87 | 0x004CF630 | DynObj_LoadPackageLovs | 3D Models & MEF | 2 | 0x00421CE0 | - | retail_only |
| 88 | 0x004CF300 | DynObj_LookupHashItem | 3D Models & MEF | 604 | 0x004215A0 | - | retail_only |
| 89 | 0x004EDB60 | LODSettings_LoadParameters | 3D Models & MEF | 462 | 0x00433B70 | - | retail_only |
| 90 | 0x00505E40 | TextureModifier_Push | TEX | 12314 | 0x004587E0 | - | retail_only |
| 91 | 0x00506F00 | HeightMap_Enter | Terrain | 2105 | 0x00458880 | - | retail_only |
| 92 | 0x004F82B0 | TerrainLightMap_Init_2 | Terrain | 151302 | 0x004588E0 | - | retail_only |
| 93 | 0x004F1160 | Script_ParseEnumReal32_5 | QVM | 154289 | 0x004522F0 | - | retail_only |
| 94 | 0x004D0D10 | Model_GetAttachmentCount | 3D Models & MEF | 1075 | 0x0042E440 | - | retail_only |
| 95 | 0x004D0CF0 | Model_GetAttachment | 3D Models & MEF | 668 | 0x0042E420 | - | retail_only |
| 96 | 0x0048F230 | App_ContextGetLightmapsUsed | Flow Engine | 22 | 0x0041E7B0 | - | retail_only |
| 97 | 0x00465820 | Human_CameraEyePosition | humanplayer.qvm | 243155 | 0x004224D0 | 823 | both |
| 98 | 0x00463180 | Human_IsCrouched | humanplayer.qvm | 243245 | 0x004224C0 | - | retail_only |
| 99 | 0x00497D70 | Transcontext_BuildViewBlock | CTR / Camera | 1846 | 0x00433DA0 | 823 | both |
| 100 | 0x00491D40 | Display_ComputeScreenAspect | CTR / Camera | 3798 | 0x00433D80 | - | retail_only |
| 101 | 0x00497E70 | TransContext_SetActiveTransContext | CTR / Camera | 3797 | 0x00433C10 | 823 | both |
| 102 | 0x004D14E0 | Terrain_TraverseContourQuadtree | Terrain | 1846 | 0x004630C0 | - | retail_only |
| 103 | 0x00510010 | Terrain_DecodeChunkCommands | Terrain | 863 | 0x00458930 | - | retail_only |
| 104 | 0x00512C30 | Terrain_Draw | Terrain | 1845 | 0x00452980 | - | retail_only |
| 105 | 0x004659E0 | Human_TaskViewReset | humanplayer.qvm | 1 | 0x00422520 | 823 | both |
| - | 0x0048F480 | Loop_SetStatusByte | Flow Engine | 0 | 0x0041F760 | 1 | candidate_only |
| - | 0x00404590 | Config_GetActiveGraphicOptions | config.qvm | 0 | 0x0040C590 | - | neither |
| - | 0x00404F00 | Config_GraphicOptionsSetPerformanceFlags | config.qvm | 0 | 0x0040C5B0 | 1 | candidate_only |
| - | 0x00404F60 | Config_GraphicOptionsGetPerformanceFlags | config.qvm | 0 | 0x0040C610 | - | neither |
| - | 0x00405530 | Config_GraphicOptionsGetPerfLevelFromFlags | config.qvm | 0 | 0x0040C720 | - | neither |
| - | 0x004055C0 | Config_GraphicOptionsGetPerfFlagsFromLevel | config.qvm | 0 | 0x0040C6C0 | 1 | candidate_only |
| - | 0x00405620 | Config_GraphicOptionsNumPerfLevels | config.qvm | 0 | 0x0040C670 | - | neither |
| - | 0x004B7E10 | Qvm_ReportError | QVM | 0 | 0x0043EBC0 | - | neither |
| - | 0x004A2600 | Camera_ResetMatrix | CTR / Camera | 0 | 0x004339D0 | - | neither |
| - | 0x004B7450 | Matrix_InvalidateSlot | CTR / Camera | 0 | 0x004338A0 | - | neither |
| - | 0x004A26C0 | Camera_UpdateFogPalette | TEX | 0 | 0x00433950 | - | neither |
| - | 0x004A5702 | String_VFormat | Memory & System | 0 | 0x00422340 | - | neither |
| - | 0x004AD722 | Mem_InvokeNewHandler | Memory & System | 0 | 0x00422EB0 | - | neither |
| - | 0x004AF7B0 | Error_Show | Memory & System | 0 | 0x0041FB60 | - | neither |
| - | 0x0048F1B0 | App_ContextGetDebugged | Flow Engine | 0 | 0x0041E780 | - | neither |

---

## 8. Counts per category

### 8.1 Requested RVA protocol (candidate column invalid)

| Class | Count |
|-------|------:|
| Both | 0 |
| Retail only | 105 |
| Candidate only | 0 |
| Neither | 15 |
| Total | 120 |

These are the literal numbers from `comparison.json` in the paired capture directory. The
0/105 split is an artifact of the RVA mismatch (Section 5) and is **not** a finding that the
candidate omits 105 paths.

### 8.2 Corrected logical comparison (retail RVA vs candidate symbol)

| Class | Count | Notes |
|-------|------:|-------|
| Both | 16 | retail executed it and the candidate entered its mapped symbol |
| Retail only | 89 | 80 have a candidate symbol that never fired; 9 have no defensible symbol |
| Candidate only | 3 | candidate entered the symbol; retail did not in this window |
| Neither | 12 | neither side entered the address/symbol in this window |
| Total | 120 | |

Of the 89 retail-only rows, 80 are “mapped but not entered” and 9 are “unmeasured”. The 9
are a subset of retail-only, not an extra category.

### Per-category breakdown (corrected logical comparison)

| Category | Total | Both | Retail only | Candidate only | Neither | Symbol unresolved |
|----------|------:|-----:|------------:|---------------:|--------:|------------------:|
| Flow Engine | 16 | 4 | 10 | 1 | 1 | 1 |
| RES | 14 | 2 | 12 | 0 | 0 | 0 |
| QVM | 12 | 0 | 11 | 0 | 1 | 0 |
| Memory & System | 11 | 1 | 7 | 0 | 3 | 0 |
| Terrain | 11 | 3 | 8 | 0 | 0 | 0 |
| CTR / Camera | 10 | 3 | 5 | 0 | 2 | 0 |
| TEX | 10 | 0 | 9 | 0 | 1 | 0 |
| 3D Models & MEF | 8 | 0 | 8 | 0 | 0 | 0 |
| ILFF & IFF | 8 | 0 | 8 | 0 | 0 | 0 |
| config.qvm | 7 | 0 | 1 | 2 | 4 | 1 |
| humanplayer.qvm | 7 | 3 | 4 | 0 | 0 | 2 |
| MTP | 6 | 0 | 6 | 0 | 0 | 5 |

Category notes: QVM has **0 both** (11 retail-only, 1 neither, 0 unresolved). TEX, ILFF &
IFF, 3D Models & MEF, and MTP have **0 both**. The three candidate-only rows are all in
Flow Engine (1) and config.qvm (2).

---

## 9. Divergences and the most important missing candidate call sites

### 9.1 Candidate-only

| Retail VA | Native | Category | Candidate hits | Candidate VA |
|-----------|--------|----------|---------------:|--------------|
| `0x0048F480` | Loop_SetStatusByte | Flow Engine | 1 | `0x0041F760` |
| `0x00404F00` | Config_GraphicOptionsSetPerformanceFlags | config.qvm | 1 | `0x0040C5B0` |
| `0x004055C0` | Config_GraphicOptionsGetPerfFlagsFromLevel | config.qvm | 1 | `0x0040C6C0` |

These are real candidate-only reachability in this window: the candidate entered each once
at startup, while retail did not enter them at all in 120 s. This is a divergence in startup
flow, not proof of a bug; it is also a reminder that retail's same names were reachable in
other contexts (the config functions are UI/options paths that level 1 startup need not
touch).

### 9.2 Most important retail-only call sites

The list is ordered by retail hit volume and grouped by subsystem. “Mapped” means the
candidate symbol was installed and never fired; “unresolved” means the row is unmeasured.

1. **QVM execution is not reached at all.** Every linked `Qvm_*` native that retail
   executed has 0 candidate hits: `Qvm_Load` (retail 121), `Qvm_Cleanup` (83),
   `Qvm_SetGlobalState` (121), `Qvm_ExecuteInstructions` (44,656), `Qvm_SetOpcodeHandler`
   (472), `Qvm_PushInteger` (32,517), plus the flag helpers. `Script_GetParseMode` — the
   function behind the MVP `Script_ParseEnumReal32_5` label — is also 0 (retail 154,289), and
   its caller `TerrainLightMap_PushEntry` is 0 (retail 151,302). Retail's QVM category totals
   240,146 hits across 11 observed natives; candidate: 0. `Qvm_ReportError` is in the
   neither set. `Retail_Main` itself is 0, and `QTask_CreateTask` is unresolved (no candidate
   symbol and its `scripts/GameNatives` implementation is not linked). This is the single
   largest functional gap the capture exposes.
2. **ILFF/resource loading is not exercised.** `Ilff_GetNextChunk` (71,703),
   `Resource_MatchPrefix` (50,942), `Ilff_GetChunkTag` (21,501), `Ilff_GetChunkPayload`
   (19,054), `Ilff_GetFirstChunk` (9,493), `Ilff_FindChunk` (9,080),
   `Resource_CreateNode` (5,636), `Resource_Load` (4,682), `Resource_FileExists` (1,439),
   `String_ToUpperChar` (1,444,700), and the rest of RES/ILFF are all 0 candidate hits. The
   MVP `Iff_*`/`AnimIff_*` rows are separate and unmeasured (Section 9.3). The candidate
   canonicalizes one path (`Resource_CanonicalizePath` 1 vs 13,426) and computes one hash
   (1 vs 13,426) in the window.
3. **The retail memory wrapper layer is bypassed.** `Mem_AllocChunk` / `Mem_AllocWithRetry`
   / `Mem_AllocateRawBlock` (74,305 each), `Mem_Alloc` (73,926), `HeapList_Insert`
   (106,814), `HeapList_Remove` (101,706), and `sprintf` (14,272) are 0 in the candidate,
   while `Mem_InitHeapState` fires once. The candidate's allocations, if any, did not pass
   through the retail-shaped wrappers.
4. **Texture work is absent.** All nine retail-observed TEX natives are 0, including
   `TextureModifier_Push` (12,314) and `DynObj_LoadModelTextures` (2). Terrain's texture
   entry point fires once, but the texture pipeline it feeds is not entered.
5. **Terrain draw/quadtree path is absent.** `Terrain_Draw` (1,845), `HeightMap_Enter`
   (2,105), `Terrain_TraverseContourQuadtree` (1,846), `Render_GetActiveBackendId` (1,209),
   `Terrain_DecodeChunkCommands` (863), `Terrain_CreateTileMap` (8), and
   `TerrainLightMap_PushEntry` (151,302) are all 0, although `Terrain_Load` (1),
   `Terrain_CreateMaterial` (6), and `Terrain_LoadTexturesAndLightmaps` (1) do fire. The
   candidate loads terrain and creates materials, then does not enter the tile-map, quadtree,
   lightmap-push, or draw entry points it would need to draw it.
6. **Human/flow/script/display paths are absent.** `Human_IsCrouched` (243,245),
   `Game_GetContext` (169,204), `Flow_RunHandler` (84,602), `Flow_CreateHandler` (1),
   `QTask_RegisterClasses` (1), `InputPort_Process` (1), `Script_GetForcePrecompiledState`
   (6,160), `Script_GetDebugCompileState` (121), and `Display_ComputeScreenAspect` (3,798)
   are all 0. `Game_GetContext`, `Human_IsCrouched`, and `Display_ComputeScreenAspect` are
   plausible inline candidates, so the 0 at their symbol entries is weaker evidence than the
   QVM/ILFF 0s; the capture cannot distinguish “inlined into callers” from “not called”.
7. **FP helper cluster.** The six MVP camera/display labels are manifest FP control-word
   helpers. Only `FP_SetDefaultPrecision` (1) and `FP_InitExtendedPrecision` (825) fire.
   `FP_MergeControlWord`, `FP_MergeControlWordNoDenormal`, `FP_ControlWordToFlags`, and
   `FP_FlagsToControlWord` are 0 at their entries (retail 1 each); they are plausible
   candidates for inlining into the two that do fire.
8. **Entry/config natives.** `Retail_Main` is 0 at `_Main`; the candidate's `WinMain` does
   not enter it. `Config_Read`, `DefineHumanPlayerGeneral`, and `HumanPlayer_LoadParameters`
   are unresolved because `engine/src/retail/script/humanplayer_defines.cpp` is **not in the
   link object list** (`build-clean-win32/CMakeFiles/igi.dir/objects1.rsp` contains no
   `humanplayer_defines` object). `QTask_CreateTask` has no candidate symbol, and its
   `scripts/GameNatives/qtask/00401000-QTask_CreateTask.cpp` implementation is not linked
   into `igi1.exe`. These are the strongest “missing candidate call site” cases in the strict
   sense: the named implementation is not in the image, rather than merely unhooked.

### Top retail-only mapped symbols by retail hits

| Retail VA | Native | Category | Retail hits | Candidate symbol VA |
|-----------|--------|----------|------------:|---------------------|
| 0x004AD73D | String_ToUpperChar | RES | 1444700 | 0x00452540 |
| 0x00463180 | Human_IsCrouched | humanplayer.qvm | 243245 | 0x004224C0 |
| 0x00402880 | Game_GetContext | Flow Engine | 169204 | 0x0041F740 |
| 0x004F1160 | Script_ParseEnumReal32_5 | QVM | 154289 | 0x004522F0 |
| 0x004F82B0 | TerrainLightMap_Init_2 | Terrain | 151302 | 0x004588E0 |
| 0x004AF8F0 | HeapList_Insert | Memory & System | 106814 | 0x00422DC0 |
| 0x004AF960 | HeapList_Remove | Memory & System | 101706 | 0x00422DE0 |
| 0x00402260 | Flow_RunHandler | Flow Engine | 84602 | 0x0041EEB0 |
| 0x004AA670 | Mem_AllocChunk | Memory & System | 74305 | 0x00422FE0 |
| 0x004AA682 | Mem_AllocWithRetry | Memory & System | 74305 | 0x00422F30 |
| 0x004AA6AE | Mem_AllocateRawBlock | Memory & System | 74305 | 0x00422EE0 |
| 0x004B0C60 | Mem_Alloc | Memory & System | 73926 | 0x004230A0 |
| 0x004B7470 | Ilff_GetNextChunk | ILFF & IFF | 71703 | 0x00452310 |
| 0x004AAD10 | Resource_MatchPrefix | RES | 50942 | 0x0043B590 |
| 0x004BCFD0 | Qvm_ExecuteInstructions | QVM | 44656 | 0x00452290 |
| 0x004BDA00 | Qvm_PushInteger | QVM | 32517 | 0x00452240 |
| 0x004B7580 | Ilff_GetChunkTag | ILFF & IFF | 21501 | 0x004524C0 |
| 0x004B74D0 | Ilff_GetChunkPayload | ILFF & IFF | 19054 | 0x004523C0 |
| 0x004A53B3 | sprintf | Memory & System | 14272 | 0x00422370 |
| 0x00505E40 | TextureModifier_Push | TEX | 12314 | 0x004587E0 |
| 0x004B7490 | Ilff_GetFirstChunk | ILFF & IFF | 9493 | 0x00452340 |
| 0x004B7590 | Ilff_FindChunk | ILFF & IFF | 9080 | 0x004524E0 |
| 0x0048F340 | Script_GetForcePrecompiledState | QVM | 6160 | 0x0043EA90 |
| 0x004B59F0 | Resource_CreateNode | RES | 5636 | 0x0043B640 |
| 0x004B7540 | Ilff_ReadContainerHeader | ILFF & IFF | 5464 | 0x00452470 |
| 0x004B5F00 | Resource_Load | RES | 4682 | 0x0043AFF0 |
| 0x004B0F80 | Texture_GetActiveBackendId | TEX | 4578 | 0x00466BF0 |
| 0x00491D40 | Display_ComputeScreenAspect | CTR / Camera | 3798 | 0x00433D80 |
| 0x00506F00 | HeightMap_Enter | Terrain | 2105 | 0x00458880 |
| 0x004D14E0 | Terrain_TraverseContourQuadtree | Terrain | 1846 | 0x004630C0 |

### 9.3 Unmeasured rows

The unmeasured set is now 9. The hottest are `Iff_GetChunkPayload` (30,042),
`Iff_FindChunk` (1,638), `Iff_FindFormChunk` (406), `Iff_FindChunkBelowForm` (388), and
`AnimIff_LoadAnimationFile` (6); `QTask_CreateTask`, `Config_Read`,
`DefineHumanPlayerGeneral`, and `HumanPlayer_LoadParameters` have 1 retail hit each. The
IFF/AnimIff translation units are linked (`iff_chunk_tree.cpp.obj`,
`iff_skeleton.cpp.obj`) but the named free functions are absent from the symbol table,
consistent with inlining or removal; that is not proof that the logic is missing.

The earlier label mismatch mattered: `FP_InitExtendedPrecision` (MVP
`Display_ClearSurfaces_16`, 84,606 retail hits) is now measured at 825 candidate hits
(both), and `FP_SetDefaultPrecision` (`Camera_ApplyBandFilter_10`, 1 retail hit) at 1
(both). `TerrainLightMap_PushEntry` (MVP `TerrainLightMap_Init_2`, 151,302) and
`Script_GetParseMode` (MVP `Script_ParseEnumReal32_5`, 154,289) are mapped but 0
(retail-only), not unmeasured.

### Retail-observed names with no candidate symbol (unmeasured)

| Retail VA | Native | Category | Retail hits |
|-----------|--------|----------|------------:|
| 0x005179C0 | Iff_GetChunkPayload | MTP | 30042 |
| 0x00517940 | Iff_FindChunk | MTP | 1638 |
| 0x00517890 | Iff_FindFormChunk | MTP | 406 |
| 0x005178E0 | Iff_FindChunkBelowForm | MTP | 388 |
| 0x004D67A0 | AnimIff_LoadAnimationFile | MTP | 6 |
| 0x00401000 | QTask_CreateTask | Flow Engine | 1 |
| 0x00405850 | Config_Read | config.qvm | 1 |
| 0x004137A0 | HumanPlayer_LoadParameters | humanplayer.qvm | 1 |
| 0x00410070 | DefineHumanPlayerGeneral | humanplayer.qvm | 1 |

---

## 10. Honest limitations

1. **Reachability is not behaviour.** This capture records that an entry was entered and how
   often; it says nothing about arguments, return values, side effects, ordering semantics,
   rendering, collision, or correctness. No behavioral claim is made, and no status is
   changed.
2. **The requested candidate RVA column is invalid.** The candidate does not preserve retail
   RVAs. Its 0 hits and 0 snapshots are an instrumentation failure, not evidence of absence.
   The 20 unhookable addresses are layout artifacts. Any use of the RVA `retail_only = 105`
   count as a missing-path list would be wrong.
3. **The RVA hook set stalls the candidate agent.** Two independent runs reproduce it (no
   snapshots; two ticks then a dead ping). The symbol run proves the same spawn/attach/resume
   path and agent timers work when the hooks are at candidate-correct addresses, but this
   does not identify which retail-RVA hook caused the stall; no bisection was attempted.
4. **Hooks may miss inlined, optimized-away, or dead code.** `Game_GetContext`,
   `Human_IsCrouched`, and `Display_ComputeScreenAspect` are plausible inline cases. A 0 at a
   symbol entry means “this entry was not entered”, not “this logic never ran”. Conversely,
   one hit at a wrapper or overload does not prove the native's behaviour ran identically.
5. **Label and symbol mapping is an approximation.** 111 addresses were mapped by the
   manifest name at the same address first, then exact/normalized basename, with one
   source-identified alias. Ten MVP labels conflict with the manifest (Section 6), and some
   retail addresses are inner points rather than function entries, so the candidate symbol
   entry is the nearest equivalent hook site, not a byte-identical one; per-address counts
   are therefore not directly comparable. Overloads and wrappers can share a basename;
   `sprintf` has two candidate symbols; `DynObj_LoadModelMesh` /
   `DynObj_LoadModelTexturePackage` have model-namespace overloads that did fire in the
   archived run while the manifest-namespace entries did not. The supplement captures *a*
   candidate code path per address, not necessarily the exact retail implementation.
6. **Nine names are unmeasured.** They are not counted as absent. `Config_Read`,
   `DefineHumanPlayerGeneral`, and `HumanPlayer_LoadParameters` belong to a translation unit
   that is not linked; `QTask_CreateTask` is not linked; the IFF/AnimIff names are absent
   from linked translation units, consistent with inlining or removal. The rows are listed
   in Section 9.3.
7. **One retail run, several candidate runs.** There is no repetition/variance estimate. The
   candidate symbol supplement was repeated three times (initial, MVP-label map,
   manifest-resolved map); only the last is the primary result, and the earlier two are
   archived alongside it. The retail side is a single spawn capture.
8. **The capture window is 120 s and level 1 only.** Paths taken later (menus, other levels,
   save/load, error handling, AI/weapons/sound outside MVP) are not covered. The candidate
   ran ~823 frame-loop iterations and then stopped or idled; retail's loop ran far longer, so
   per-address totals are not directly comparable.
9. **Interceptor overhead can change timing** and, at 100+ hooks, possibly the frame rate.
   Counts are not performance measurements.
10. **Diagnostic shell, not the full game.** The candidate is the `igi1.exe` diagnostic
    candidate. Missing QVM/ILFF/texture/terrain-draw execution is a real reachability
    finding for this image, but it may be expected for an incomplete MVP shell; it is not a
    verdict that the native ports are wrong.

---

## 11. Artifacts and reproduction

| Path | Contents |
|------|----------|
| `build/runtime-traces/20261023T1330-level1-paired-reachability/` | requested pair: `retail-capture.json`, `candidate-capture.json`, `comparison.json`, `matrix.md`, both probes/derivation, driver, preflight and Frida logs, modules |
| `build/runtime-traces/20261023T1348-candidate-reachability-retry/` | identical-probe candidate retry with independent ticker: `candidate-retry.json`, `probe.log`, `ticker.log`, `retry.log` |
| `build/runtime-traces/20261023T1357-candidate-symbol-reachability/` | symbol supplement: `symbol_probe.js`, `symbol-map.json`, `candidate-symbols.json`, `comparison.json`, `matrix.md`, `summary.json`, `report-tables.md`; archived earlier symbol probes/maps/captures (`symbol_probe-mvpnames.js`, `symbol-map-mvpnames.json`, `candidate-symbols-mvpnames.json`, `candidate-symbols-ambiguousmap.json`) |
| `docs/fidelity/evidence/20261023-level1-paired-reachability.md` | this report |

Reproduction (no status changes):

```bash
PY="/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python"
cd build/runtime-traces/20261023T1330-level1-paired-reachability
"$PY" capture_pair.py              # requested sequential pair (RVA probe)
cd ../20261023T1348-candidate-reachability-retry
"$PY" retry_candidate.py           # identical RVA probe + independent ticker
cd ../20261023T1357-candidate-symbol-reachability
python3 make_symbol_probe.py       # regenerate map + symbol probe (checksum-stable)
"$PY" capture_symbols.py          # candidate symbol supplement
python3 analyze_symbols.py
python3 summarize.py
python3 render_report.py
```

`sha256sums.txt` in each capture directory binds the raw files. The retail and candidate
images and the canonical probe are pinned in Section 2; any rerun with different hashes is
not this evidence.

---

## Appendix A — retail-only mapped symbols that never fired (all rows)

| Retail VA | Native | Category | Retail hits | Candidate symbol VA |
|-----------|--------|----------|------------:|---------------------|
| 0x004AD73D | String_ToUpperChar | RES | 1444700 | 0x00452540 |
| 0x00463180 | Human_IsCrouched | humanplayer.qvm | 243245 | 0x004224C0 |
| 0x00402880 | Game_GetContext | Flow Engine | 169204 | 0x0041F740 |
| 0x004F1160 | Script_ParseEnumReal32_5 | QVM | 154289 | 0x004522F0 |
| 0x004F82B0 | TerrainLightMap_Init_2 | Terrain | 151302 | 0x004588E0 |
| 0x004AF8F0 | HeapList_Insert | Memory & System | 106814 | 0x00422DC0 |
| 0x004AF960 | HeapList_Remove | Memory & System | 101706 | 0x00422DE0 |
| 0x00402260 | Flow_RunHandler | Flow Engine | 84602 | 0x0041EEB0 |
| 0x004AA670 | Mem_AllocChunk | Memory & System | 74305 | 0x00422FE0 |
| 0x004AA682 | Mem_AllocWithRetry | Memory & System | 74305 | 0x00422F30 |
| 0x004AA6AE | Mem_AllocateRawBlock | Memory & System | 74305 | 0x00422EE0 |
| 0x004B0C60 | Mem_Alloc | Memory & System | 73926 | 0x004230A0 |
| 0x004B7470 | Ilff_GetNextChunk | ILFF & IFF | 71703 | 0x00452310 |
| 0x004AAD10 | Resource_MatchPrefix | RES | 50942 | 0x0043B590 |
| 0x004BCFD0 | Qvm_ExecuteInstructions | QVM | 44656 | 0x00452290 |
| 0x004BDA00 | Qvm_PushInteger | QVM | 32517 | 0x00452240 |
| 0x004B7580 | Ilff_GetChunkTag | ILFF & IFF | 21501 | 0x004524C0 |
| 0x004B74D0 | Ilff_GetChunkPayload | ILFF & IFF | 19054 | 0x004523C0 |
| 0x004A53B3 | sprintf | Memory & System | 14272 | 0x00422370 |
| 0x00505E40 | TextureModifier_Push | TEX | 12314 | 0x004587E0 |
| 0x004B7490 | Ilff_GetFirstChunk | ILFF & IFF | 9493 | 0x00452340 |
| 0x004B7590 | Ilff_FindChunk | ILFF & IFF | 9080 | 0x004524E0 |
| 0x0048F340 | Script_GetForcePrecompiledState | QVM | 6160 | 0x0043EA90 |
| 0x004B59F0 | Resource_CreateNode | RES | 5636 | 0x0043B640 |
| 0x004B7540 | Ilff_ReadContainerHeader | ILFF & IFF | 5464 | 0x00452470 |
| 0x004B5F00 | Resource_Load | RES | 4682 | 0x0043AFF0 |
| 0x004B0F80 | Texture_GetActiveBackendId | TEX | 4578 | 0x00466BF0 |
| 0x00491D40 | Display_ComputeScreenAspect | CTR / Camera | 3798 | 0x00433D80 |
| 0x00506F00 | HeightMap_Enter | Terrain | 2105 | 0x00458880 |
| 0x004D14E0 | Terrain_TraverseContourQuadtree | Terrain | 1846 | 0x004630C0 |
| 0x00512C30 | Terrain_Draw | Terrain | 1845 | 0x00452980 |
| 0x004B5B60 | Resource_FileExists | QVM | 1439 | 0x0043EAA0 |
| 0x004B23B0 | Texture_UploadMipmapPacket | TEX | 1407 | 0x00466C20 |
| 0x004B6380 | Resource_Unload | RES | 1214 | 0x0043AF80 |
| 0x004CEA80 | Render_GetActiveBackendId | Terrain | 1209 | 0x00433880 |
| 0x004B5C90 | Resource_FreeNode | RES | 1153 | 0x0043B7E0 |
| 0x004D0D10 | Model_GetAttachmentCount | 3D Models & MEF | 1075 | 0x0042E440 |
| 0x00510010 | Terrain_DecodeChunkCommands | Terrain | 863 | 0x00458930 |
| 0x004B26B0 | Texture_DisableUpload | TEX | 685 | 0x00466C10 |
| 0x004B26A0 | Texture_EnableUpload | TEX | 684 | 0x00466C00 |
| 0x004B24C0 | Texture_LoadLoopTextureFile | TEX | 683 | 0x00466D30 |
| 0x004D0CF0 | Model_GetAttachment | 3D Models & MEF | 668 | 0x0042E420 |
| 0x004CEAA0 | Ilff_FindFormChunk | ILFF & IFF | 618 | 0x004215F0 |
| 0x004CF390 | DynObj_LoadModelMesh | 3D Models & MEF | 604 | 0x00421660 |
| 0x004CF300 | DynObj_LookupHashItem | 3D Models & MEF | 604 | 0x004215A0 |
| 0x004BD3D0 | Qvm_SetOpcodeHandler | QVM | 472 | 0x00452270 |
| 0x004EDB60 | LODSettings_LoadParameters | 3D Models & MEF | 462 | 0x00433B70 |
| 0x0048F250 | App_ContextGetTerrainLightmapsUsed | Flow Engine | 386 | 0x0041E7D0 |
| 0x004B1690 | Device_CloseStream | RES | 359 | 0x00433F10 |
| 0x004B0D40 | QHash_ValueSet | RES | 229 | 0x00422E60 |
| 0x004B1780 | Device_GetStreamPosition | RES | 203 | 0x00433EC0 |
| 0x004B17F0 | Device_SeekStream | RES | 203 | 0x00433EE0 |
| 0x004B5F80 | Resource_RegisterBuffer | RES | 203 | 0x0043BE50 |
| 0x004B1E90 | String_FindExtension | QVM | 167 | 0x0043EAF0 |
| 0x004B80B0 | Qvm_Load | QVM | 121 | 0x0043EC70 |
| 0x0048F1D0 | Script_GetDebugCompileState | QVM | 121 | 0x0043EA80 |
| 0x004B8A10 | Qvm_SetGlobalState | QVM | 121 | 0x0043EAE0 |
| 0x004B83D0 | Qvm_Cleanup | QVM | 83 | 0x0043EC20 |
| 0x004B7570 | Ilff_GetFormType | ILFF & IFF | 45 | 0x004524A0 |
| 0x004B60F0 | Resource_RegisterIlffContainer | RES | 29 | 0x0043BAC0 |
| 0x0048F230 | App_ContextGetLightmapsUsed | Flow Engine | 22 | 0x0041E7B0 |
| 0x004C6760 | Terrain_CreateTileMap | Terrain | 8 | 0x00458710 |
| 0x004B7CF0 | Texture_UploadPalettePacket | TEX | 4 | 0x00466CE0 |
| 0x0048F260 | AppContext_SetTerrainLightmapsUsed | Flow Engine | 2 | 0x0041E7C0 |
| 0x004CF080 | DynObj_LoadModelTexturePackage | MTP | 2 | 0x00421DA0 |
| 0x004CF910 | DynObj_LoadPackageAnimations | 3D Models & MEF | 2 | 0x004219A0 |
| 0x004CF790 | DynObj_LoadModelTextures | TEX | 2 | 0x00467360 |
| 0x004CF870 | DynObj_LoadPackagePalettes | TEX | 2 | 0x00421B60 |
| 0x004CF330 | DynObj_LoadPackageMeshes | 3D Models & MEF | 2 | 0x00421C40 |
| 0x004CF630 | DynObj_LoadPackageLovs | 3D Models & MEF | 2 | 0x00421CE0 |
| 0x004AC655 | Camera_ApplyLineFilter_24 | CTR / Camera | 1 | 0x0041F440 |
| 0x004AC620 | Camera_SetFieldOfView_24 | CTR / Camera | 1 | 0x0041F2D0 |
| 0x004AC66B | Camera_ApplyBandFilter_24 | CTR / Camera | 1 | 0x0041F150 |
| 0x004AC6FD | Camera_ApplyWideScreen_24 | CTR / Camera | 1 | 0x0041F210 |
| 0x0048F530 | Retail_Main | Flow Engine | 1 | 0x0041FBF0 |
| 0x00402930 | QTask_RegisterClasses | Flow Engine | 1 | 0x005996E8 |
| 0x00402800 | Flow_CreateHandler | Flow Engine | 1 | 0x0041F110 |
| 0x004ED5C0 | InputPort_Process | humanplayer.qvm | 1 | 0x00422D70 |
| 0x004028A0 | Flow_SetState | Flow Engine | 1 | 0x0041F750 |
| 0x005175A0 | Terrain_LoadTexturePackage | Terrain | 1 | 0x00466EE0 |

## Appendix B — retail-unobserved addresses

| Retail VA | Native | Category | Candidate status | Candidate hits |
|-----------|--------|----------|------------------|---------------:|
| 0x0048F480 | Loop_SetStatusByte | Flow Engine | observed | 1 |
| 0x00404590 | Config_GetActiveGraphicOptions | config.qvm | symbol_hooked_no_hits | 0 |
| 0x00404F00 | Config_GraphicOptionsSetPerformanceFlags | config.qvm | observed | 1 |
| 0x00404F60 | Config_GraphicOptionsGetPerformanceFlags | config.qvm | symbol_hooked_no_hits | 0 |
| 0x00405530 | Config_GraphicOptionsGetPerfLevelFromFlags | config.qvm | symbol_hooked_no_hits | 0 |
| 0x004055C0 | Config_GraphicOptionsGetPerfFlagsFromLevel | config.qvm | observed | 1 |
| 0x00405620 | Config_GraphicOptionsNumPerfLevels | config.qvm | symbol_hooked_no_hits | 0 |
| 0x004B7E10 | Qvm_ReportError | QVM | symbol_hooked_no_hits | 0 |
| 0x004A2600 | Camera_ResetMatrix | CTR / Camera | symbol_hooked_no_hits | 0 |
| 0x004B7450 | Matrix_InvalidateSlot | CTR / Camera | symbol_hooked_no_hits | 0 |
| 0x004A26C0 | Camera_UpdateFogPalette | TEX | symbol_hooked_no_hits | 0 |
| 0x004A5702 | String_VFormat | Memory & System | symbol_hooked_no_hits | 0 |
| 0x004AD722 | Mem_InvokeNewHandler | Memory & System | symbol_hooked_no_hits | 0 |
| 0x004AF7B0 | Error_Show | Memory & System | symbol_hooked_no_hits | 0 |
| 0x0048F1B0 | App_ContextGetDebugged | Flow Engine | symbol_hooked_no_hits | 0 |

## Appendix C — the 20 retail-RVA hook failures on the candidate

| Retail VA | Native | Install error |
|-----------|--------|---------------|
| 0x00401000 | QTask_CreateTask | Error: unable to intercept function at 00401000; please file a bug |
| 0x00402880 | Game_GetContext | Error: unable to intercept function at 00402880; please file a bug |
| 0x00404590 | Config_GetActiveGraphicOptions | Error: unable to intercept function at 00404590; no instruction could be decoded there |
| 0x00404F00 | Config_GraphicOptionsSetPerformanceFlags | Error: unable to intercept function at 00404F00; please file a bug |
| 0x004055C0 | Config_GraphicOptionsGetPerfFlagsFromLevel | Error: unable to intercept function at 004055C0; no instruction could be decoded there |
| 0x00405850 | Config_Read | Error: unable to intercept function at 00405850; please file a bug |
| 0x00463180 | Human_IsCrouched | Error: unable to intercept function at 00463180; no instruction could be decoded there |
| 0x0048F260 | AppContext_SetTerrainLightmapsUsed | Error: unable to intercept function at 0048F260; no instruction could be decoded there |
| 0x00498460 | Display_ClearSurfaces_16 | Error: unable to intercept function at 00498460; please file a bug |
| 0x004A26C0 | Camera_UpdateFogPalette | Error: unable to intercept function at 004A26C0; please file a bug |
| 0x004A5702 | String_VFormat | Error: unable to intercept function at 004A5702; please file a bug |
| 0x004AA6AE | Mem_AllocateRawBlock | Error: unable to intercept function at 004AA6AE; no instruction could be decoded there |
| 0x004B60F0 | Resource_RegisterIlffContainer | Error: unable to intercept function at 004B60F0; no instruction could be decoded there |
| 0x004B80B0 | Qvm_Load | Error: unable to intercept function at 004B80B0; please file a bug |
| 0x004CF870 | DynObj_LoadPackagePalettes | Error: unable to intercept function at 004CF870; please file a bug |
| 0x004CF910 | DynObj_LoadPackageAnimations | Error: unable to intercept function at 004CF910; no instruction could be decoded there |
| 0x004EDB60 | LODSettings_LoadParameters | Error: unable to intercept function at 004EDB60; no instruction could be decoded there |
| 0x004F1160 | Script_ParseEnumReal32_5 | Error: unable to intercept function at 004F1160; no instruction could be decoded there |
| 0x00505E40 | TextureModifier_Push | Error: unable to intercept function at 00505E40; please file a bug |
| 0x00506F00 | HeightMap_Enter | Error: unable to intercept function at 00506F00; please file a bug |
