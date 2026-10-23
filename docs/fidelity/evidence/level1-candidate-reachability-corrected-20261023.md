# Level 1 candidate reachability: corrected measurement

**Date:** 2026-10-23
**Scope:** measurement only. No native status in `docs/fidelity/native-verification.json`
was changed, and this report does not claim behavioural equivalence for any native.

## 1. What was wrong with the earlier candidate measurement

The previous candidate supplement (`20261023T1357-candidate-symbol-reachability`)
mapped 111 of the 120 MVP names by name alone and reported 19 observed natives. It did
not prove that a mapped address was a real function entry, so an address in the middle
of another function would have been reported as an unexecuted native. It also missed
five functions whose candidate implementation was deliberately renamed (the IFF chunk
tree and the animation IFF reader).

This run rebuilds the retail→candidate map from the candidate's own COFF symbol table,
validates every mapped address as a function entry, and then measures the candidate
with a spawn-gated probe. The candidate image was also rebuilt several times by
concurrent work while this task ran; the measured image is therefore a frozen,
byte-identical snapshot, not a moving build path.

## 2. Image identity

| role | path | SHA-256 |
|------|------|---------|
| Retail game (source of truth for VAs and hits) | `/Users/haseeb-mir/Downloads/IGI/igi.exe` | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| Candidate build path used as the source of the measured snapshot | `build-clean-win32/igi1.exe` at 2026-10-23 14:57:13 | `11f939200afd865bf7286ee2a90ac393a5887b1bb9589b0b1a324dfbbd59a581` |
| Measured frozen image | `build/runtime-traces/20261023T1440-level1-candidate-corrected/frozen/igi1-frozen-11f93920.exe` | `11f939200afd865bf7286ee2a90ac393a5887b1bb9589b0b1a324dfbbd59a581` |

The candidate receipt for this build is `build-clean-win32/candidate-receipt.json`
(SHA-256 `4e839465d6efaaa96f731f71187ffd1bb540b3e133fcc183f91a747c68430648`),
created `2026-10-23T09:27:14Z`, scope `Diagnostic compiler/source binding only; no native
acceptance or deployment`. The frozen image is 16,071,073 bytes, module name
`igi1-frozen-11f93920.exe`, loaded at the expected base `0x400000`.

`build-clean-win32/igi1.exe` changed at 14:46, 14:50, 14:53 and 14:57 during this task
(concurrent agents were editing and rebuilding `engine/src/retail/core/qtask_create_task.cpp`
and other sources). Freezing was necessary: a map built against one hash and a capture
spawned from another would silently mix images. `frozen-image.json` records the source
mtime, copy time, receipt hash and the 61-second stability window before the copy. All
numbers below bind to the frozen SHA-256, not to whatever `build-clean-win32/igi1.exe`
contains later.

## 3. Address map

Source: `scripts/mvp_native_targets.json` (120 retail VAs). Symbols: `llvm-nm` over the
frozen image, demangled with `c++filt` (4,859 code symbols). Resolution order:

1. the MVP name, then the manifest implementation name bound to that retail VA;
2. exact demangled basename match;
3. C-underscore-stripped match;
4. manifest/source-provenance aliases where the implementation was renamed;
5. unique normalized basename match (case/underscore), with ambiguity rejected.

Five mappings are source-provenance aliases, each backed by a comment at the retail VA:

| retail name | retail VA | candidate symbol | binding |
|-------------|-----------|------------------|---------|
| `AnimIff_LoadAnimationFile` | `0x004D67A0` | `igi::retail::model::IffSkeleton_Read` | `engine/src/retail/model/iff_skeleton.cpp` |
| `Iff_FindFormChunk` | `0x00517890` | `igi::retail::model::IffChunk_FindByTag` | `engine/include/igi/retail/model/iff_chunk_tree.hpp` |
| `Iff_FindChunkBelowForm` | `0x005178E0` | `igi::retail::model::IffChunk_FindBelowForm` | same |
| `Iff_FindChunk` | `0x00517940` | `igi::retail::model::IffChunk_FindChildByTag` | same |
| `Iff_GetChunkPayload` | `0x005179C0` | `igi::retail::model::IffChunk_GetPayload` | same |

`AppContext_SetTerrainLightmapsUsed` resolves by unique normalized basename (the candidate
spells it `App_ContextSetTerrainLightmapsUsed`). Some MVP labels and candidate symbols
differ because the implementation uses the manifest/source name, for example
`Display_ClearSurfaces_16` → `igi::retail::core::FP_InitExtendedPrecision`,
`Camera_ApplyBandFilter_10` → `FP_SetDefaultPrecision`,
`Script_ParseEnumReal32_5` → `Script_GetParseMode`,
`TerrainLightMap_Init_2` → `TerrainLightMap_PushEntry`, `Retail_Main` → `_Main`,
`Error_Show` → `_Debug_Error`, and `sprintf` → `igi::retail::error::sprintf`.
The retail VA is the anchor; these label differences are naming, not evidence of
equivalent semantics.

Every mapped address passed six entry checks against the frozen image:

- the symbol is a code symbol (`T/t/W/w`);
- it lies in an executable section;
- a Capstone linear sweep of the whole `.text` section (1,697,280 bytes, 456,213
  instructions, 106 undecodable bytes) has an instruction start exactly at the symbol;
- no instruction that starts up to 15 bytes earlier straddles the entry;
- the first instruction is not filler/padding;
- the gap to the next code symbol is positive.

Result: **117 of 120 names mapped, 117 entry-valid, 0 duplicate candidate addresses,
3 unmapped**. The 3 unmapped names are all functions whose only source definition is
`engine/src/retail/script/humanplayer_defines.cpp`, which is not listed in
`CMakeLists.txt`, so their symbols are not in this image:

| retail name | retail VA | classification |
|-------------|-----------|----------------|
| `Config_Read` | `0x004FBE30` | `definition_in_unlinked_translation_unit` |
| `DefineHumanPlayerGeneral` | `0x004F9A20` | `definition_in_unlinked_translation_unit` |
| `HumanPlayer_LoadParameters` | `0x004FB160` | `definition_in_unlinked_translation_unit` |

`QTask_CreateTask` (`0x00401000`) was among the unmapped names in the earlier supplement.
A concurrent change added `engine/src/retail/core/qtask_create_task.cpp` to the build at
14:45; in the frozen image it is `_QTask_CreateTask` at `0x0041FB60`, entry-validated. It
is therefore measurable here, unlike in the earlier supplement.

## 4. Capture protocol and execution

Files: `capture_candidate.py`, `make_probe.py`, `candidate_probe.js`,
`probe-derivation.json`, `frozen-image.json`, `capture-attempt-4.log`.

- Spawn suspended through Frida (`Device.spawn`, `window level1`), working directory
  `Z:\Users\haseeb-mir\Downloads\IGI` (the retail data root with `YMBE.AFP`).
- 117 hooks attempted after load and before `resume`; **117 installed, 117 byte-verified,
  0 failed**. The probe resolves the module once, checks the runtime base is `0x400000`,
  and compares 24 mapped bytes at each entry before attaching `Interceptor`.
- Window: 120 s. Liveness sampled every 5 s: 24 samples, last at 118.4 s, all
  `windows_alive` and `host_alive` true. 12 probe snapshots (last at 120.0 s) and 60
  ticker ticks (last at 120.09 s).
- Process identity: Windows PID 2060, host PID 21215. Termination was verified for both
  views; only this PID was killed. No other game process was killed by this driver.
- Result record: `result: captured`, `stayed_alive: true`, `early_exit_at_rel: null`,
  `hooks_before_resume: true`, `candidate.sha256 == candidate.sha256_post_run`.

Attempt 1 was cut at 5.2 s with a foreign `igi1.exe` already in the process list
(recorded as `externally_terminated`). Attempts 2 and 3 ended at 15.5 s and 51.6 s during
the same period of repeated concurrent spawn/kill cycles by other agent sessions (each
is followed within seconds by a foreign game in the Wine log). Those artifacts are
preserved under `attempt-1-*`, `attempt-2-exited_early/` and `attempt-3-*`; attempts 2
and 3 did observe 23 partial-window natives each, but neither is used for the numbers. `run_capture.py` waited for a quiet shared runtime and retried until attempt 4
completed a full window. It never killed a foreign process. A separate pre-freeze
attempt on image `936c8f7f…` with the earlier 116-target map reached 23 observed /
4,283 hits before an external kill (preserved under `attempt-1-external-kill/`); it is
consistent with attempt 4 but is not the reported measurement.

## 5. Corrected result

| metric | retail | candidate |
|--------|--------|-----------|
| natives in catalog | 120 | 120 |
| valid measurable entries | 120 | 117 |
| observed in this window | 105 | 23 |
| total entry hits | 3,652,554 | 5,585 |
| capture window | 120.235 s | 120 s |
| snapshots | 12 | 12 |

Matrix labels: **both** = observed on both sides; **retail_only** = retail observed,
candidate had a valid entry hook but 0 hits; **candidate_only** = candidate observed,
retail not; **unmeasured** = no valid candidate address (a measurement gap, not a
behavioural finding); **neither** = both measured validly, 0 hits on both.

| label | count |
|-------|-------|
| both | 20 |
| retail_only | 82 |
| candidate_only | 3 |
| unmeasured | 3 |
| neither | 12 |
| **total** | **120** |

Per category:

| category | both | retail_only | candidate_only | unmeasured | neither | total |
|----------|------|-------------|----------------|-------------|---------|-------|
| 3D Models & MEF | 0 | 8 | 0 | 0 | 0 | 8 |
| CTR / Camera | 3 | 5 | 0 | 0 | 2 | 10 |
| Flow Engine | 4 | 10 | 1 | 0 | 1 | 16 |
| ILFF & IFF | 0 | 8 | 0 | 0 | 0 | 8 |
| MTP | 4 | 2 | 0 | 0 | 0 | 6 |
| Memory & System | 1 | 7 | 0 | 0 | 3 | 11 |
| QVM | 0 | 11 | 0 | 0 | 1 | 12 |
| RES | 2 | 12 | 0 | 0 | 0 | 14 |
| TEX | 0 | 9 | 0 | 0 | 1 | 10 |
| Terrain | 3 | 8 | 0 | 0 | 0 | 11 |
| config.qvm | 0 | 0 | 2 | 1 | 4 | 7 |
| humanplayer.qvm | 3 | 2 | 0 | 2 | 0 | 7 |

### 5.1 Both observed (20)

| retail VA | native | category | retail hits | candidate hits | candidate symbol |
|-----------|--------|----------|-------------|----------------|------------------|
| `0x004A7157` | `Camera_ApplyBandFilter_10` | CTR / Camera | 1 | 1 | `igi::retail::core::FP_SetDefaultPrecision()` |
| `0x0048F140` | `AppContext_SetIsActive` | Flow Engine | 1 | 1 | `_AppContext_SetIsActive` |
| `0x00498460` | `Display_ClearSurfaces_16` | Flow Engine | 84,606 | 778 | `igi::retail::core::FP_InitExtendedPrecision()` |
| `0x004B0BA0` | `Mem_InitHeapState` | Memory & System | 1 | 1 | `igi::retail::mem::Mem_InitHeapState()` |
| `0x004B5AF0` | `Resource_CanonicalizePath` | RES | 13,426 | 1 | `igi::retail::sys::Resource_CanonicalizePath(char*, char*)` |
| `0x004B5B30` | `Resource_CalculatePathHash` | RES | 13,426 | 1 | `igi::retail::sys::Resource_CalculatePathHash(char const*)` |
| `0x004037C0` | `GOInRemap` | humanplayer.qvm | 81 | 81 | `_GOInRemap` |
| `0x0048F170` | `AppContext_GetIsActive` | Flow Engine | 84,602 | 776 | `_AppContext_GetIsActive` |
| `0x00401050` | `QTask_OpenEvent` | Flow Engine | 84,602 | 776 | `_QTask_OpenEvent` |
| `0x004C5F00` | `Terrain_LoadTexturesAndLightmaps` | Terrain | 1 | 1 | `igi::retail::terrain::Terrain_LoadTexturesAndLightmaps(void*, char const*)` |
| `0x004C6240` | `Terrain_Load` | Terrain | 1 | 1 | `igi::retail::terrain::Terrain_Load(void*, char const*)` |
| `0x004C6670` | `Terrain_CreateMaterial` | Terrain | 6 | 6 | `igi::retail::terrain::Terrain_CreateMaterial(void*, float const*)` |
| `0x004D67A0` | `AnimIff_LoadAnimationFile` | MTP | 6 | 6 | `igi::retail::model::IffSkeleton_Read(std::filesystem::__cxx11::path const&)` |
| `0x00517890` | `Iff_FindFormChunk` | MTP | 406 | 12 | `igi::retail::model::IffChunk_FindByTag(igi::retail::model::IffChunkNode*, unsigned int)` |
| `0x00517940` | `Iff_FindChunk` | MTP | 1,638 | 18 | `igi::retail::model::IffChunk_FindChildByTag(igi::retail::model::IffChunkNode*, unsigned int)` |
| `0x005179C0` | `Iff_GetChunkPayload` | MTP | 30,042 | 18 | `igi::retail::model::IffChunk_GetPayload(igi::retail::model::IffChunkNode const*)` |
| `0x00465820` | `Human_CameraEyePosition` | humanplayer.qvm | 243,155 | 776 | `igi::retail::human::Human_CameraEyePosition(void*, void const*)` |
| `0x00497D70` | `Transcontext_BuildViewBlock` | CTR / Camera | 1,846 | 776 | `igi::retail::render::Transcontext_BuildViewBlock(void*, void const*, void const*, float)` |
| `0x00497E70` | `TransContext_SetActiveTransContext` | CTR / Camera | 3,797 | 776 | `igi::retail::render::Transcontext_SetActiveTransContext(igi::retail::render::TransContext const&)` |
| `0x004659E0` | `Human_TaskViewReset` | humanplayer.qvm | 1 | 776 | `igi::retail::human::Human_TaskViewReset(void*)` |

Every row above was observed at the validated candidate entry and at the retail
entry in the paired window. The hit counts differ substantially for the per-frame
calls; reachability is not a claim of per-call parity.

### 5.2 Candidate-only (3)

| native | candidate hits | retail hits |
|--------|----------------|-------------|
| `Config_GraphicOptionsSetPerformanceFlags` | 1 | 0 |
| `Config_GraphicOptionsGetPerfFlagsFromLevel` | 1 | 0 |
| `Loop_SetStatusByte` | 1 | 0 |

These are reachable candidate paths that the retail capture did not hit in its window.
That does not make them retail behaviour; they are candidate-side execution evidence
only.

### 5.3 Neither (12)

Both sides have a valid, verified entry hook and neither called the function in its
120 s window: `Config_GetActiveGraphicOptions`,
`Config_GraphicOptionsGetPerformanceFlags`, `Config_GraphicOptionsGetPerfLevelFromFlags`,
`Config_GraphicOptionsNumPerfLevels`, `App_ContextGetDebugged`, `Camera_ResetMatrix`,
`Camera_UpdateFogPalette`, `String_VFormat`, `Mem_InvokeNewHandler`, `Error_Show`,
`Matrix_InvalidateSlot`, `Qvm_ReportError`. These are error/config accessors and
cold paths; 0 hits here is not evidence that they can never execute.

### 5.4 Unmeasured (3)

All three are retail-observed (`Config_Read` 1, `HumanPlayer_LoadParameters` 1,
`DefineHumanPlayerGeneral` 1) but have no valid candidate address, because their only
definition (`engine/src/retail/script/humanplayer_defines.cpp`) is not in the link.
They are measurement gaps, not evidence that the candidate does not need them.

## 6. What remains unmeasured, and why

| cause | count | names |
|-------|-------|-------|
| No candidate symbol in the image (definition in an unlinked translation unit) | 3 | `Config_Read`, `DefineHumanPlayerGeneral`, `HumanPlayer_LoadParameters` |
| Symbol present but entry validation failed | 0 | — |
| Symbol optimised out / removed by the compiler | 0 identified | — |
| Confirmed inlined away (no out-of-line body) | 0 identified | — |

The three unlinked functions are a build-scope gap, not an optimisation result: the
source exists and is bound to the retail VA, but adding the file to `CMakeLists.txt`
would change the candidate and is outside this measurement task. No name in the 120 is
missing because the compiler deleted a symbol that the map expected; every one of the
117 mapped names has a code symbol at a validated entry.

A separate limitation applies to every 0-hit row: this probe hooks the out-of-line
function entry. If the candidate compiler inlined a small function at all call sites,
the out-of-line copy would show 0 hits while the inlined code still ran. The build uses
`-O3 -DNDEBUG`, so this is possible in principle. It cannot be decided from entry hooks
alone, and this report does not turn a 0-hit into a claim that the behaviour is absent.
The 82 `retail_only` rows are therefore "not observed at the mapped out-of-line entry",
not "proven not to execute". This is why the matrix uses `retail_only` rather than a
behavioural label.

## 7. The most important candidate paths that still do not execute

Ranked by retail hit volume in the same 120 s window; all are `retail_only` with a
validated candidate hook and 0 candidate hits.

| retail hits | native | what the gap means |
|-------------|--------|--------------------|
| 1,444,700 | `String_ToUpperChar` | The resource/path string normalisation path is not exercised by the candidate's startup/level-1 flow. This is the single largest missing call path. |
| 243,245 | `Human_IsCrouched` | The human state query is never reached, while `Human_CameraEyePosition` is. The candidate's human/camera path is partial. |
| 169,204 | `Game_GetContext` | The flow/game context accessor is absent even though `AppContext_GetIsActive` and `QTask_OpenEvent` run. |
| 154,289 | `Script_ParseEnumReal32_5` (source name `Script_GetParseMode`) | The QVM parse path is absent; QVM execution is not reached. |
| 151,302 | `TerrainLightMap_Init_2` (source name `TerrainLightMap_PushEntry`) | Terrain loads (`Terrain_Load`, materials, textures) but lightmap entry initialisation never runs. |
| 106,814 / 101,706 | `HeapList_Insert` / `HeapList_Remove` | The candidate's allocation path does not enter the retail heap-list implementation. |
| 84,602 | `Flow_RunHandler` | `QTask_OpenEvent` is called 776 times, but the handler dispatch itself is never entered. This is the most important flow-engine gap. |
| 74,305 ×3 / 73,926 | `Mem_AllocChunk`, `Mem_AllocWithRetry`, `Mem_AllocateRawBlock`, `Mem_Alloc` | Only `Mem_InitHeapState` is reached once; the retail allocator entry points are not. The candidate is allocating through a different path. |
| 71,703 | `Ilff_GetNextChunk` | The ILFF container reader is not used. |
| 50,942 | `Resource_MatchPrefix` | Resource lookup matching is not used; only canonicalisation/hash run (once each). |
| 44,656 / 32,517 | `Qvm_ExecuteInstructions` / `Qvm_PushInteger` | No QVM instruction execution is observed at all. |
| 21,501 / 19,054 / 9,493 / 9,080 / 5,464 | `Ilff_GetChunkTag`, `Ilff_GetChunkPayload`, `Ilff_GetFirstChunk`, `Ilff_FindChunk`, `Ilff_ReadContainerHeader` | The candidate parses IFF through the alias-mapped `IffChunk_*` functions instead, so the retail ILFF API is not exercised even though some IFF traversal happens. |
| 14,272 | `sprintf` (`igi::retail::error::sprintf`) | Formatted error/string output is not reached, despite the function being linked. |
| 12,314 | `TextureModifier_Push` | Terrain texture modifiers never run, although `Terrain_LoadTexturesAndLightmaps` is called once. |
| 5,636 / 4,682 / 1,214 | `Resource_CreateNode` / `Resource_Load` / `Resource_Unload` | The resource manager's load/unload path is not entered; the candidate only canonicalises and hashes a path once. |
| 4,578 / 1,407 | `Texture_GetActiveBackendId` / `Texture_UploadMipmapPacket` | The texture backend/upload path is absent. |
| 3,798 | `Display_ComputeScreenAspect` | Screen-aspect computation is not reached. |
| 2,105 / 1,846 / 1,845 | `HeightMap_Enter` / `Terrain_TraverseContourQuadtree` / `Terrain_Draw` | Terrain draw/traversal is absent even though terrain load is observed. |
| 1,439 | `Resource_FileExists` | The script-side file-existence path is not used. |
| 1,075 / 668 / 604 | `Model_GetAttachmentCount` / `Model_GetAttachment` / `DynObj_LoadModelMesh` | No 3D model/MEF path is entered; all 8 model-category natives are retail-only. |
| 618 | `Ilff_FindFormChunk` | Retail uses its own ILFF form lookup; the candidate's IFF alias path is a different implementation. |
| 1 | `InputPort_Process` | Input processing is not reached in this window (windowed scene, no injected input). |

The three largest clusters are not isolated misses: they show three whole subsystems
whose retail entry points are not used by the candidate at level 1 — the retail heap
allocator, the ILFF container reader, and QVM execution. The candidate does reach its
own equivalents in places (it allocates, parses IFF chunks through the alias-mapped
`IffChunk_*` functions, and loads terrain), but those are different entry points, so
retail reachability cannot be inferred from the candidate's success. `Flow_RunHandler`,
`Game_GetContext`, `String_ToUpperChar` and `Human_IsCrouched` are the most important
single gaps because they are on the per-frame/flow path rather than deep in one
subsystem.

`QTask_CreateTask` is a special case. It became measurable only in this image because
`engine/src/retail/core/qtask_create_task.cpp` was added to the build during this task.
Retail calls it once during startup; the candidate has a validated entry and calls it 0
times in the window. The new implementation is linked but not yet invoked by the
candidate startup path, so it is `retail_only`, not a mapping gap.

## 8. Reachability is not parity: per-call asymmetry

For the 20 natives observed on both sides, the candidate calls most of them far less
often than retail in the same 120 s window:

| native | retail hits | candidate hits |
|--------|-------------|----------------|
| `Display_ClearSurfaces_16` | 84,606 | 778 |
| `Human_CameraEyePosition` | 243,155 | 776 |
| `AppContext_GetIsActive` | 84,602 | 776 |
| `QTask_OpenEvent` | 84,602 | 776 |
| `Iff_GetChunkPayload` | 30,042 | 18 |
| `Iff_FindChunk` | 1,638 | 18 |
| `Iff_FindFormChunk` | 406 | 12 |
| `GOInRemap` | 81 | 81 |

The candidate's per-frame calls run at roughly 776 iterations in the window versus
roughly 84,600 in retail, and its IFF traversal count is much lower. Reachability
answers "did the mapped entry execute at all"; it does not answer whether the candidate
executes the same logic the same number of times. Call counts here are observations from
one window, not a performance or parity verdict.

## 9. Limitations

- One level (`level1`), one 120 s window, windowed display, no injected input.
- Retail capture is the paired baseline from `20261023T1330` (same retail SHA); it was
  captured before the candidate rebuilds, which does not affect retail identity.
- The candidate numbers bind to frozen image `11f93920…`. Later rebuilds may change
  which functions are linked (for example the newly added `qtask_create_task.cpp`) and
  therefore the unmeasured/retail-only counts.
- Entry hooks cannot see inlined code and cannot distinguish "not called" from "called
  only through inlined copies" (section 6).
- The candidate was killed by concurrent `igi`-cleanup loops three times before a clean
  window; only the clean attempt is reported.
- These are reachability observations. They do not promote any native to
  `RUNTIME_TESTED` or `VERIFIED`, and no manifest status was changed.

## 10. Evidence files

All under `build/runtime-traces/20261023T1440-level1-candidate-corrected/` unless noted:

| file | SHA-256 |
|------|---------|
| `candidate-capture.json` | `f8ca0f0aa42a7fa2d75cdbe8d6fd986e7e788fce51519103c4687080c9ee7040` |
| `address-map.json` | `cc1e35806507455e01b0dee137219bf41d1a3dd76f86af75f2af51e24ef83779` |
| `candidate_probe.js` | `9fb7b7c9655253ed1c6609c68400ab6116759c1ca2038a82f7a463cb54dd502c` |
| `probe-derivation.json` | `c6e5a952432045a014eb9309aca40de1db118cb5e369eae7878f975d0c3fe9fe` |
| `comparison.json` | `965f2347dbbd8b9dc391d92f384794c73b37269022c451f5dd18a72911418ea2` |
| `matrix.md` | `63608d0d952c26276c9077c5cfdea2dbb715466dce10db43b7a76fa439d0b8c2` |
| `frozen-image.json` | `02bb9924cb6e564a9c80d6f6e9c2c5bfda8e6cbd9a4672ada10b1149cd51f5a3` |
| `capture-attempt-4.log` | `023526b0fd2e150a31e71f6605d56a0ca462a20915b070331177bc57045f1b9d` |
| retail baseline `retail-capture.json` (other directory) | `a2e8cc9cb47f73995a636dec53f40ef5b692904826087a57b8c3246a75bbe6e1` |

If any hash above does not match the file on disk, the file on disk wins; this report
is evidence, not a substitute for the raw artifacts.

## 11. Full matrix in retail first-hit order

| # | Retail VA | Native | Category | Retail hits | Candidate VA | Candidate symbol | Candidate hits | Label |
|---|-----------|--------|----------|-------------|--------------|------------------|----------------|-------|
| 1 | 0x004AA670 | Mem_AllocChunk | Memory & System | 74305 | 4337728 | igi::retail::mem::Mem_AllocChunk(unsigned int) | 0 | retail_only |
| 2 | 0x004AA682 | Mem_AllocWithRetry | Memory & System | 74305 | 4337552 | igi::retail::mem::Mem_AllocWithRetry(unsigned int, unsigned int) | 0 | retail_only |
| 3 | 0x004AA6AE | Mem_AllocateRawBlock | Memory & System | 74305 | 4337472 | igi::retail::mem::Mem_AllocateRawBlock(unsigned int) | 0 | retail_only |
| 4 | 0x004A7157 | Camera_ApplyBandFilter_10 | CTR / Camera | 1 | 4322736 | igi::retail::core::FP_SetDefaultPrecision() | 1 | both |
| 5 | 0x004AC655 | Camera_ApplyLineFilter_24 | CTR / Camera | 1 | 4322368 | igi::retail::core::FP_MergeControlWordNoDenormal(unsigned int, unsigned int) | 0 | retail_only |
| 6 | 0x004AC620 | Camera_SetFieldOfView_24 | CTR / Camera | 1 | 4322000 | igi::retail::core::FP_MergeControlWord(unsigned int, unsigned int) | 0 | retail_only |
| 7 | 0x004AC66B | Camera_ApplyBandFilter_24 | CTR / Camera | 1 | 4321616 | igi::retail::core::FP_ControlWordToFlags(unsigned short) | 0 | retail_only |
| 8 | 0x004AC6FD | Camera_ApplyWideScreen_24 | CTR / Camera | 1 | 4321808 | igi::retail::core::FP_FlagsToControlWord(unsigned int) | 0 | retail_only |
| 9 | 0x0048F530 | Retail_Main | Flow Engine | 1 | 4324448 | _Main | 0 | retail_only |
| 10 | 0x0048F140 | AppContext_SetIsActive | Flow Engine | 1 | 4323264 | _AppContext_SetIsActive | 1 | both |
| 11 | 0x0048F260 | AppContext_SetTerrainLightmapsUsed | Flow Engine | 2 | 4319168 | igi::retail::core::App_ContextSetTerrainLightmapsUsed(signed char) | 0 | retail_only |
| 12 | 0x00498460 | Display_ClearSurfaces_16 | Flow Engine | 84606 | 4323104 | igi::retail::core::FP_InitExtendedPrecision() | 778 | both |
| 13 | 0x00401000 | QTask_CreateTask | Flow Engine | 1 | 4324192 | _QTask_CreateTask | 0 | retail_only |
| 14 | 0x00402930 | QTask_RegisterClasses | Flow Engine | 1 | 5871464 | _QTask_RegisterClasses | 0 | retail_only |
| 15 | 0x004B0BA0 | Mem_InitHeapState | Memory & System | 1 | 4337360 | igi::retail::mem::Mem_InitHeapState() | 1 | both |
| 16 | 0x004B0D40 | QHash_ValueSet | RES | 229 | 4337344 | igi::retail::mem::QHash_ValueSet() | 0 | retail_only |
| 17 | 0x004B0C60 | Mem_Alloc | Memory & System | 73926 | 4337920 | igi::retail::mem::Mem_Alloc(int, int) | 0 | retail_only |
| 18 | 0x004AF8F0 | HeapList_Insert | Memory & System | 106814 | 4337184 | igi::retail::mem::HeapList_Insert(void*, void**) | 0 | retail_only |
| 19 | 0x004B26B0 | Texture_DisableUpload | TEX | 685 | 4615312 | igi::retail::texture::Texture_DisableUpload() | 0 | retail_only |
| 20 | 0x004BD3D0 | Qvm_SetOpcodeHandler | QVM | 472 | 4530928 | igi::retail::script::Qvm_SetOpcodeHandler(int, int, void (*)(igi::retail::script::QvmNativeStatePrefix*, unsigned int, unsigned int, unsigned int)) | 0 | retail_only |
| 21 | 0x004CEA80 | Render_GetActiveBackendId | Terrain | 1209 | 4405504 | igi::retail::render::Render_GetActiveBackendId() | 0 | retail_only |
| 22 | 0x004B59F0 | Resource_CreateNode | RES | 5636 | 4437696 | igi::retail::resource::Resource_CreateNode(char const*, void*, void*, int, int, int const*) | 0 | retail_only |
| 23 | 0x004B5AF0 | Resource_CanonicalizePath | RES | 13426 | 4531680 | igi::retail::sys::Resource_CanonicalizePath(char*, char*) | 1 | both |
| 24 | 0x004AD73D | String_ToUpperChar | RES | 1444700 | 4531648 | igi::retail::sys::String_ToUpperChar(unsigned int) | 0 | retail_only |
| 25 | 0x004B5B30 | Resource_CalculatePathHash | RES | 13426 | 4531792 | igi::retail::sys::Resource_CalculatePathHash(char const*) | 1 | both |
| 26 | 0x004AF960 | HeapList_Remove | Memory & System | 101706 | 4337216 | igi::retail::mem::HeapList_Remove(void**) | 0 | retail_only |
| 27 | 0x004A53B3 | sprintf | Memory & System | 14272 | 4334544 | igi::retail::error::sprintf(char*, char const*, ...) | 0 | retail_only |
| 28 | 0x004AAD10 | Resource_MatchPrefix | RES | 50942 | 4437520 | igi::retail::resource::Resource_MatchPrefix(void const*, void const*, int) | 0 | retail_only |
| 29 | 0x004B1E90 | String_FindExtension | QVM | 167 | 4451184 | igi::retail::script::String_FindExtension(char*, int, char*) | 0 | retail_only |
| 30 | 0x004B5F00 | Resource_Load | RES | 4682 | 4436080 | igi::retail::resource::Resource_Load(char const*, int*) | 0 | retail_only |
| 31 | 0x004B1780 | Device_GetStreamPosition | RES | 203 | 4407104 | igi::retail::resource::Device_GetStreamPosition(int*) | 0 | retail_only |
| 32 | 0x004B17F0 | Device_SeekStream | RES | 203 | 4407136 | igi::retail::resource::Device_SeekStream(int*, int) | 0 | retail_only |
| 33 | 0x004B1690 | Device_CloseStream | RES | 359 | 4407184 | igi::retail::resource::Device_CloseStream(int*) | 0 | retail_only |
| 34 | 0x004B5F80 | Resource_RegisterBuffer | RES | 203 | 4439760 | igi::retail::resource::Resource_RegisterBuffer(char const*, void*, int) | 0 | retail_only |
| 35 | 0x004B7580 | Ilff_GetChunkTag | ILFF & IFF | 21501 | 4531520 | igi::retail::sys::Ilff_GetChunkTag(int const*) | 0 | retail_only |
| 36 | 0x004B6380 | Resource_Unload | RES | 1214 | 4435968 | igi::retail::resource::Resource_Unload(char const*) | 0 | retail_only |
| 37 | 0x004B5C90 | Resource_FreeNode | RES | 1153 | 4438112 | igi::retail::resource::Resource_FreeNode(void*) | 0 | retail_only |
| 38 | 0x0048F340 | Script_GetForcePrecompiledState | QVM | 6160 | 4451088 | igi::retail::script::Script_GetForcePrecompiledState() | 0 | retail_only |
| 39 | 0x004B80B0 | Qvm_Load | QVM | 121 | 4451584 | igi::retail::script::Qvm_Load(char const*) | 0 | retail_only |
| 40 | 0x0048F1D0 | Script_GetDebugCompileState | QVM | 121 | 4451072 | igi::retail::script::Script_GetDebugCompileState() | 0 | retail_only |
| 41 | 0x004B5B60 | Resource_FileExists | QVM | 1439 | 4451104 | igi::retail::script::Resource_FileExists(char const*) | 0 | retail_only |
| 42 | 0x004B8A10 | Qvm_SetGlobalState | QVM | 121 | 4451168 | igi::retail::script::Qvm_SetGlobalState(int) | 0 | retail_only |
| 43 | 0x004BCFD0 | Qvm_ExecuteInstructions | QVM | 44656 | 4530960 | igi::retail::script::Qvm_ExecuteInstructions(igi::retail::script::QvmNativeStatePrefix*) | 0 | retail_only |
| 44 | 0x004BDA00 | Qvm_PushInteger | QVM | 32517 | 4530880 | igi::retail::script::Qvm_PushInteger(igi::retail::script::QvmNativeStatePrefix*, unsigned int) | 0 | retail_only |
| 45 | 0x004B83D0 | Qvm_Cleanup | QVM | 83 | 4451504 | igi::retail::script::Qvm_Cleanup(void*) | 0 | retail_only |
| 46 | 0x00402800 | Flow_CreateHandler | Flow Engine | 1 | 4321552 | _Flow_CreateHandler | 0 | retail_only |
| 47 | 0x00405850 | Config_Read | config.qvm | 1 | - | - | 0 | unmeasured |
| 48 | 0x004037C0 | GOInRemap | humanplayer.qvm | 81 | 4245440 | _GOInRemap | 81 | both |
| 49 | 0x004137A0 | HumanPlayer_LoadParameters | humanplayer.qvm | 1 | - | - | 0 | unmeasured |
| 50 | 0x00410070 | DefineHumanPlayerGeneral | humanplayer.qvm | 1 | - | - | 0 | unmeasured |
| 51 | 0x004ED5C0 | InputPort_Process | humanplayer.qvm | 1 | 4337104 | _InputPort_Process | 0 | retail_only |
| 52 | 0x004028A0 | Flow_SetState | Flow Engine | 1 | 4323152 | _Flow_SetState | 0 | retail_only |
| 53 | 0x0048F170 | AppContext_GetIsActive | Flow Engine | 84602 | 4323312 | _AppContext_GetIsActive | 776 | both |
| 54 | 0x00401050 | QTask_OpenEvent | Flow Engine | 84602 | 4323184 | _QTask_OpenEvent | 776 | both |
| 55 | 0x00402880 | Game_GetContext | Flow Engine | 169204 | 4323136 | _Game_GetContext | 0 | retail_only |
| 56 | 0x00402260 | Flow_RunHandler | Flow Engine | 84602 | 4320944 | _Flow_RunHandler | 0 | retail_only |
| 57 | 0x004B0F80 | Texture_GetActiveBackendId | TEX | 4578 | 4615280 | igi::retail::texture::Texture_GetActiveBackendId() | 0 | retail_only |
| 58 | 0x004B7570 | Ilff_GetFormType | ILFF & IFF | 45 | 4531488 | igi::retail::sys::Ilff_GetFormType(void const*) | 0 | retail_only |
| 59 | 0x004B60F0 | Resource_RegisterIlffContainer | RES | 29 | 4438848 | igi::retail::resource::Resource_RegisterIlffContainer(char const*, void*, void*, int const*) | 0 | retail_only |
| 60 | 0x004B7490 | Ilff_GetFirstChunk | ILFF & IFF | 9493 | 4531136 | igi::retail::sys::Ilff_GetFirstChunk(void*) | 0 | retail_only |
| 61 | 0x004B74D0 | Ilff_GetChunkPayload | ILFF & IFF | 19054 | 4531264 | igi::retail::sys::Ilff_GetChunkPayload(void*, void*) | 0 | retail_only |
| 62 | 0x004B7470 | Ilff_GetNextChunk | ILFF & IFF | 71703 | 4531088 | igi::retail::sys::Ilff_GetNextChunk(void*) | 0 | retail_only |
| 63 | 0x004B7540 | Ilff_ReadContainerHeader | ILFF & IFF | 5464 | 4531440 | igi::retail::sys::Ilff_ReadContainerHeader(int*, int*, void const*) | 0 | retail_only |
| 64 | 0x004B23B0 | Texture_UploadMipmapPacket | TEX | 1407 | 4615328 | igi::retail::texture::Texture_UploadMipmapPacket(int*, void*, int, int, int, int, int, int, void*) | 0 | retail_only |
| 65 | 0x004C5F00 | Terrain_LoadTexturesAndLightmaps | Terrain | 1 | 4589968 | igi::retail::terrain::Terrain_LoadTexturesAndLightmaps(void*, char const*) | 1 | both |
| 66 | 0x005175A0 | Terrain_LoadTexturePackage | Terrain | 1 | 4616032 | igi::retail::texture::Terrain_LoadTexturePackage(char const*) | 0 | retail_only |
| 67 | 0x004B26A0 | Texture_EnableUpload | TEX | 684 | 4615296 | igi::retail::texture::Texture_EnableUpload() | 0 | retail_only |
| 68 | 0x004B7CF0 | Texture_UploadPalettePacket | TEX | 4 | 4615520 | igi::retail::texture::Texture_UploadPalettePacket(int*, void*, int) | 0 | retail_only |
| 69 | 0x0048F250 | App_ContextGetTerrainLightmapsUsed | Flow Engine | 386 | 4319184 | igi::retail::core::App_ContextGetTerrainLightmapsUsed() | 0 | retail_only |
| 70 | 0x004C6240 | Terrain_Load | Terrain | 1 | 4584560 | igi::retail::terrain::Terrain_Load(void*, char const*) | 1 | both |
| 71 | 0x004C6670 | Terrain_CreateMaterial | Terrain | 6 | 4556416 | igi::retail::terrain::Terrain_CreateMaterial(void*, float const*) | 6 | both |
| 72 | 0x004C6760 | Terrain_CreateTileMap | Terrain | 8 | 4556688 | igi::retail::terrain::Terrain_CreateTileMap(void*, float const*) | 0 | retail_only |
| 73 | 0x004CF080 | DynObj_LoadModelTexturePackage | MTP | 2 | 4333056 | igi::retail::dynobj::DynObj_LoadModelTexturePackage(void*, char const*, int) | 0 | retail_only |
| 74 | 0x004CF910 | DynObj_LoadPackageAnimations | 3D Models & MEF | 2 | 4332032 | igi::retail::dynobj::DynObj_LoadPackageAnimations(void*) | 0 | retail_only |
| 75 | 0x004CEAA0 | Ilff_FindFormChunk | ILFF & IFF | 618 | 4331088 | igi::retail::dynobj::Ilff_FindFormChunk(void*, int, int*) | 0 | retail_only |
| 76 | 0x004D67A0 | AnimIff_LoadAnimationFile | MTP | 6 | 4343328 | igi::retail::model::IffSkeleton_Read(std::filesystem::__cxx11::path const&) | 6 | both |
| 77 | 0x00517890 | Iff_FindFormChunk | MTP | 406 | 4338336 | igi::retail::model::IffChunk_FindByTag(igi::retail::model::IffChunkNode*, unsigned int) | 12 | both |
| 78 | 0x00517940 | Iff_FindChunk | MTP | 1638 | 4339120 | igi::retail::model::IffChunk_FindChildByTag(igi::retail::model::IffChunkNode*, unsigned int) | 18 | both |
| 79 | 0x005179C0 | Iff_GetChunkPayload | MTP | 30042 | 4339840 | igi::retail::model::IffChunk_GetPayload(igi::retail::model::IffChunkNode const*) | 18 | both |
| 80 | 0x005178E0 | Iff_FindChunkBelowForm | MTP | 388 | 4339872 | igi::retail::model::IffChunk_FindBelowForm(igi::retail::model::IffChunkNode*, unsigned int) | 0 | retail_only |
| 81 | 0x004CF790 | DynObj_LoadModelTextures | TEX | 2 | 4617184 | igi::retail::texture::DynObj_LoadModelTextures(void*) | 0 | retail_only |
| 82 | 0x004B24C0 | Texture_LoadLoopTextureFile | TEX | 683 | 4615600 | igi::retail::texture::Texture_LoadLoopTextureFile(int*, void*, char const*) | 0 | retail_only |
| 83 | 0x004CF870 | DynObj_LoadPackagePalettes | TEX | 2 | 4332480 | igi::retail::dynobj::DynObj_LoadPackagePalettes(void*) | 0 | retail_only |
| 84 | 0x004CF330 | DynObj_LoadPackageMeshes | 3D Models & MEF | 2 | 4332704 | igi::retail::dynobj::DynObj_LoadPackageMeshes(void*) | 0 | retail_only |
| 85 | 0x004CF390 | DynObj_LoadModelMesh | 3D Models & MEF | 604 | 4331200 | igi::retail::dynobj::DynObj_LoadModelMesh(char const*) | 0 | retail_only |
| 86 | 0x004B7590 | Ilff_FindChunk | ILFF & IFF | 9080 | 4531552 | igi::retail::sys::Ilff_FindChunk(int*, int) | 0 | retail_only |
| 87 | 0x004CF630 | DynObj_LoadPackageLovs | 3D Models & MEF | 2 | 4332864 | igi::retail::dynobj::DynObj_LoadPackageLovs(void*) | 0 | retail_only |
| 88 | 0x004CF300 | DynObj_LookupHashItem | 3D Models & MEF | 604 | 4331008 | igi::retail::dynobj::DynObj_LookupHashItem(igi::retail::dynobj::HashItem**, int) | 0 | retail_only |
| 89 | 0x004EDB60 | LODSettings_LoadParameters | 3D Models & MEF | 462 | 4406256 | igi::retail::render::LODSettings_LoadParameters(void*, igi::retail::render::LODSettingsRecord* (*)(char const*) noexcept) | 0 | retail_only |
| 90 | 0x00505E40 | TextureModifier_Push | TEX | 12314 | 4556896 | igi::retail::terrain::TextureModifier_Push(void*) | 0 | retail_only |
| 91 | 0x00506F00 | HeightMap_Enter | Terrain | 2105 | 4557056 | igi::retail::terrain::HeightMap_Enter(void*) | 0 | retail_only |
| 92 | 0x004F82B0 | TerrainLightMap_Init_2 | Terrain | 151302 | 4557152 | igi::retail::terrain::TerrainLightMap_PushEntry(void*) | 0 | retail_only |
| 93 | 0x004F1160 | Script_ParseEnumReal32_5 | QVM | 154289 | 4531056 | igi::retail::script::Script_GetParseMode() | 0 | retail_only |
| 94 | 0x004D0D10 | Model_GetAttachmentCount | 3D Models & MEF | 1075 | 4383936 | igi::retail::model::Model_GetAttachmentCount(igi::retail::model::MefNativeModelRecord*) | 0 | retail_only |
| 95 | 0x004D0CF0 | Model_GetAttachment | 3D Models & MEF | 668 | 4383904 | igi::retail::model::Model_GetAttachment(igi::retail::model::MefNativeModelRecord*, int) | 0 | retail_only |
| 96 | 0x0048F230 | App_ContextGetLightmapsUsed | Flow Engine | 22 | 4319152 | igi::retail::core::App_ContextGetLightmapsUsed() | 0 | retail_only |
| 97 | 0x00465820 | Human_CameraEyePosition | humanplayer.qvm | 243155 | 4334896 | igi::retail::human::Human_CameraEyePosition(void*, void const*) | 776 | both |
| 98 | 0x00463180 | Human_IsCrouched | humanplayer.qvm | 243245 | 4334880 | igi::retail::human::Human_IsCrouched(void const*) | 0 | retail_only |
| 99 | 0x00497D70 | Transcontext_BuildViewBlock | CTR / Camera | 1846 | 4406816 | igi::retail::render::Transcontext_BuildViewBlock(void*, void const*, void const*, float) | 776 | both |
| 100 | 0x00491D40 | Display_ComputeScreenAspect | CTR / Camera | 3798 | 4406784 | igi::retail::render::Display_ComputeScreenAspect() | 0 | retail_only |
| 101 | 0x00497E70 | TransContext_SetActiveTransContext | CTR / Camera | 3797 | 4406416 | igi::retail::render::Transcontext_SetActiveTransContext(igi::retail::render::TransContext const&) | 776 | both |
| 102 | 0x004D14E0 | Terrain_TraverseContourQuadtree | Terrain | 1846 | 4600128 | igi::retail::terrain::Terrain_TraverseContourQuadtree(void*) | 0 | retail_only |
| 103 | 0x00510010 | Terrain_DecodeChunkCommands | Terrain | 863 | 4557232 | igi::retail::terrain::Terrain_DecodeChunkCommands() | 0 | retail_only |
| 104 | 0x00512C30 | Terrain_Draw | Terrain | 1845 | 4532736 | igi::retail::terrain::Terrain_Draw() | 0 | retail_only |
| 105 | 0x004659E0 | Human_TaskViewReset | humanplayer.qvm | 1 | 4334976 | igi::retail::human::Human_TaskViewReset(void*) | 776 | both |
| - | 0x00404590 | Config_GetActiveGraphicOptions | config.qvm | 0 | 4244880 | igi::retail::config::Config_GetActiveGraphicOptions() | 0 | neither |
| - | 0x00404F00 | Config_GraphicOptionsSetPerformanceFlags | config.qvm | 0 | 4244912 | igi::retail::config::Config_GraphicOptionsSetPerformanceFlags(void*, int) | 1 | candidate_only |
| - | 0x00404F60 | Config_GraphicOptionsGetPerformanceFlags | config.qvm | 0 | 4245008 | igi::retail::config::Config_GraphicOptionsGetPerformanceFlags(void*) | 0 | neither |
| - | 0x00405530 | Config_GraphicOptionsGetPerfLevelFromFlags | config.qvm | 0 | 4245280 | igi::retail::config::Config_GraphicOptionsGetPerfLevelFromFlags(void*, int) | 0 | neither |
| - | 0x004055C0 | Config_GraphicOptionsGetPerfFlagsFromLevel | config.qvm | 0 | 4245184 | igi::retail::config::Config_GraphicOptionsGetPerfFlagsFromLevel(void*, int) | 1 | candidate_only |
| - | 0x00405620 | Config_GraphicOptionsNumPerfLevels | config.qvm | 0 | 4245104 | igi::retail::config::Config_GraphicOptionsNumPerfLevels(void*) | 0 | neither |
| - | 0x0048F1B0 | App_ContextGetDebugged | Flow Engine | 0 | 4319104 | igi::retail::core::App_ContextGetDebugged() | 0 | neither |
| - | 0x0048F480 | Loop_SetStatusByte | Flow Engine | 0 | 4323168 | _Loop_SetStatusByte | 1 | candidate_only |
| - | 0x004A2600 | Camera_ResetMatrix | CTR / Camera | 0 | 4405840 | igi::retail::render::Camera_ResetMatrix(igi::retail::render::CameraColorContext const*) | 0 | neither |
| - | 0x004A26C0 | Camera_UpdateFogPalette | TEX | 0 | 4405712 | igi::retail::render::Camera_UpdateFogPalette(igi::retail::render::CameraColorContext const*) | 0 | neither |
| - | 0x004A5702 | String_VFormat | Memory & System | 0 | 4334496 | igi::retail::error::String_VFormat(char*, char const*, char*) | 0 | neither |
| - | 0x004AD722 | Mem_InvokeNewHandler | Memory & System | 0 | 4337424 | igi::retail::mem::Mem_InvokeNewHandler(unsigned int) | 0 | neither |
| - | 0x004AF7B0 | Error_Show | Memory & System | 0 | 4324304 | _Debug_Error | 0 | neither |
| - | 0x004B7450 | Matrix_InvalidateSlot | CTR / Camera | 0 | 4405536 | igi::retail::render::Matrix_InvalidateSlot(int*, int) | 0 | neither |
| - | 0x004B7E10 | Qvm_ReportError | QVM | 0 | 4451392 | igi::retail::script::Qvm_ReportError(char const*, ...) | 0 | neither |

### Candidate first-hit order (observed)

| # | Native | Candidate VA | Hits |
|---|--------|--------------|------|
| 1 | Camera_ApplyBandFilter_10 | 4322736 | 1 |
| 2 | Display_ClearSurfaces_16 | 4323104 | 778 |
| 3 | Config_GraphicOptionsGetPerfFlagsFromLevel | 4245184 | 1 |
| 4 | Config_GraphicOptionsSetPerformanceFlags | 4244912 | 1 |
| 5 | Loop_SetStatusByte | 4323168 | 1 |
| 6 | AppContext_SetIsActive | 4323264 | 1 |
| 7 | GOInRemap | 4245440 | 81 |
| 8 | Mem_InitHeapState | 4337360 | 1 |
| 9 | Terrain_LoadTexturesAndLightmaps | 4589968 | 1 |
| 10 | Resource_CanonicalizePath | 4531680 | 1 |
| 11 | Resource_CalculatePathHash | 4531792 | 1 |
| 12 | Terrain_Load | 4584560 | 1 |
| 13 | AnimIff_LoadAnimationFile | 4343328 | 6 |
| 14 | Iff_FindFormChunk | 4338336 | 12 |
| 15 | Iff_FindChunk | 4339120 | 18 |
| 16 | Iff_GetChunkPayload | 4339840 | 18 |
| 17 | Terrain_CreateMaterial | 4556416 | 6 |
| 18 | AppContext_GetIsActive | 4323312 | 776 |
| 19 | QTask_OpenEvent | 4323184 | 776 |
| 20 | Human_CameraEyePosition | 4334896 | 776 |
| 21 | Human_TaskViewReset | 4334976 | 776 |
| 22 | Transcontext_BuildViewBlock | 4406816 | 776 |
| 23 | TransContext_SetActiveTransContext | 4406416 | 776 |
