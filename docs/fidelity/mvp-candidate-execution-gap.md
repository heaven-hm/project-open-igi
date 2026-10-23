# Candidate execution gap for the MVP native set

Measured from `docs/fidelity/evidence/mvp-frida-pair-long.json`, the
non-inlined verification build paired against the retail level-1 run.
The window was 150 seconds, which produced 16340 candidate calls and 18
observed natives; extending the window from 45 seconds changed the call
count but not the set of natives the candidate executes, so this is an
execution gap and not an observation-time artefact.

- linked in the candidate: 100
- executed by the candidate at level 1: 30
- retail-observed but never executed by the candidate: 70
- of those, compiled in with a direct call site (so genuinely not reached, not inlined away): 29

## By subsystem

| subsystem | natives |
| --- | --- |
| `Resource` | 10 |
| `Ilff` | 8 |
| `Qvm` | 6 |
| `Texture` | 6 |
| `DynObj` | 6 |
| `Mem` | 4 |
| `Terrain` | 4 |
| `Device` | 3 |
| `String` | 2 |
| `HeapList` | 2 |
| `Human` | 2 |
| `Script` | 2 |
| `Model` | 2 |
| `App` | 2 |
| `Flow` | 2 |
| `sprintf` | 1 |
| `TextureModifier` | 1 |
| `Render` | 1 |
| `LODSettings` | 1 |
| `HeightMap` | 1 |
| `Display` | 1 |
| `Transcontext` | 1 |
| `QTask` | 1 |
| `InputPort` | 1 |

## The natives, heaviest retail use first

| native | address | retail calls | candidate call sites |
| --- | --- | --- | --- |
| String_ToUpperChar | `0x004AD73D` | 1443550 | 0 |
| HeapList_Insert | `0x004AF8F0` | 95896 | 5 |
| Mem_AllocChunk | `0x004AA670` | 73216 | 0 |
| Mem_AllocWithRetry | `0x004AA682` | 73216 | 0 |
| Mem_AllocateRawBlock | `0x004AA6AE` | 73216 | 0 |
| Mem_Alloc | `0x004B0C60` | 72995 | 14 |
| Ilff_GetNextChunk | `0x004B7470` | 71573 | 8 |
| Resource_MatchPrefix | `0x004AAD10` | 50491 | 0 |
| HeapList_Remove | `0x004AF960` | 42839 | 2 |
| Qvm_ExecuteInstructions | `0x004BCFD0` | 42766 | 0 |
| Qvm_PushInteger | `0x004BDA00` | 26297 | 0 |
| Ilff_GetChunkTag | `0x004B7580` | 21381 | 9 |
| Ilff_GetChunkPayload | `0x004B74D0` | 18984 | 12 |
| sprintf | `0x004A53B3` | 14055 | 13 |
| Resource_CanonicalizePath | `0x004B5AF0` | 13366 | 2 |
| Resource_CalculatePathHash | `0x004B5B30` | 13366 | 2 |
| Human_IsCrouched | `0x00463180` | 11664 | 0 |
| Human_CameraEyePosition | `0x00465820` | 11664 | 1 |
| Ilff_GetFirstChunk | `0x004B7490` | 9463 | 10 |
| Ilff_FindChunk | `0x004B7590` | 9050 | 10 |
| Script_GetForcePrecompiledState | `0x0048F340` | 6160 | 0 |
| Resource_CreateNode | `0x004B59F0` | 5636 | 6 |
| Ilff_ReadContainerHeader | `0x004B7540` | 5444 | 4 |
| Resource_Load | `0x004B5F00` | 4647 | 6 |
| TextureModifier_Push | `0x00505E40` | 3548 | 0 |
| Texture_GetActiveBackendId | `0x004B0F80` | 2791 | 0 |
| Resource_FileExists | `0x004B5B60` | 1439 | 0 |
| Texture_UploadMipmapPacket | `0x004B23B0` | 1387 | 0 |
| Render_GetActiveBackendId | `0x004CEA80` | 1209 | 2 |
| Resource_Unload | `0x004B6380` | 1204 | 5 |
| Resource_FreeNode | `0x004B5C90` | 1153 | 2 |
| Model_GetAttachmentCount | `0x004D0D10` | 1048 | 0 |
| Texture_DisableUpload | `0x004B26B0` | 685 | 0 |
| Texture_EnableUpload | `0x004B26A0` | 684 | 0 |
| Texture_LoadLoopTextureFile | `0x004B24C0` | 683 | 1 |
| Model_GetAttachment | `0x004D0CF0` | 638 | 0 |
| Ilff_FindFormChunk | `0x004CEAA0` | 618 | 9 |
| DynObj_LookupHashItem | `0x004CF300` | 604 | 1 |
| Qvm_SetOpcodeHandler | `0x004BD3D0` | 472 | 0 |
| LODSettings_LoadParameters | `0x004EDB60` | 462 | 0 |
| HeightMap_Enter | `0x00506F00` | 429 | 0 |
| App_ContextGetTerrainLightmapsUsed | `0x0048F250` | 386 | 0 |
| Device_CloseStream | `0x004B1690` | 345 | 0 |
| Device_GetStreamPosition | `0x004B1780` | 203 | 0 |
| Device_SeekStream | `0x004B17F0` | 203 | 0 |
| Resource_RegisterBuffer | `0x004B5F80` | 203 | 1 |
| String_FindExtension | `0x004B1E90` | 167 | 0 |
| Display_ComputeScreenAspect | `0x00491D40` | 164 | 0 |
| Script_GetDebugCompileState | `0x0048F1D0` | 121 | 0 |
| Qvm_Load | `0x004B80B0` | 121 | 0 |
| Qvm_SetGlobalState | `0x004B8A10` | 121 | 0 |
| Qvm_Cleanup | `0x004B83D0` | 83 | 0 |
| Transcontext_BuildViewBlock | `0x00497D70` | 82 | 1 |
| Terrain_TraverseContourQuadtree | `0x004D14E0` | 82 | 0 |
| Terrain_Draw | `0x00512C30` | 82 | 0 |
| Terrain_DecodeChunkCommands | `0x00510010` | 64 | 0 |
| Ilff_GetFormType | `0x004B7570` | 35 | 4 |
| Resource_RegisterIlffContainer | `0x004B60F0` | 29 | 2 |
| App_ContextGetLightmapsUsed | `0x0048F230` | 22 | 0 |
| Texture_UploadPalettePacket | `0x004B7CF0` | 4 | 0 |
| DynObj_LoadPackageMeshes | `0x004CF330` | 2 | 0 |
| DynObj_LoadPackageLovs | `0x004CF630` | 2 | 1 |
| DynObj_LoadPackageAnimations | `0x004CF910` | 2 | 1 |
| DynObj_LoadModelTextures | `0x004CF790` | 2 | 1 |
| DynObj_LoadPackagePalettes | `0x004CF870` | 2 | 1 |
| QTask_RegisterClasses | `0x00402930` | 1 | 0 |
| Flow_CreateHandler | `0x00402800` | 1 | 0 |
| Flow_SetState | `0x004028A0` | 1 | 0 |
| InputPort_Process | `0x004ED5C0` | 1 | 0 |
| Terrain_LoadTexturePackage | `0x005175A0` | 1 | 0 |

## What this means for verification

A native cannot be verified end-to-end until the candidate executes it, so
this list is the runtime backlog behind the static one. The largest groups
are the resource and IFF loaders, the QVM script interpreter, the dynamic
object and texture loaders, and the human and flow updates. Each group
corresponds to a subsystem of the retail flow that the port has not wired
into its frame loop yet; the individual natives in that subsystem are
already implemented and compiled, which is why they have call sites and
symbols, but nothing calls them.


## Measured 2026-10-23: candidate scene modes do not extend reachability

The candidate supports command-line scene modes beyond `window levelN`, including
`devmode` (free camera and developer keys) and `inputdemo` (posts the retail key
sequence so the input port, human controller and camera can run without a
keyboard). Both were plausible ways to reach more of the linked natives.

The paired capture driver now accepts `--candidate-args` for exactly this test,
and the experiment was run with `devmode inputdemo` at level 1, reusing the
earlier retail observations:

| run | linked natives observed in the candidate |
| --- | --- |
| window level1 | 18 |
| window level1 devmode inputdemo | 18, identical set |

Every observed native was the same, by name: the model, terrain, config, flow
and frame natives. So the shortfall is not attach timing, observation window or
scene mode - the diagnostics baseline simply does not execute the Human, Script,
Qvm, Resource, Animation or Weapon code paths, because those units are not part
of the built scene. Closing that gap is porting work, not probing work.

## Reusing retail observations from an archived capture

Captures are archived to `docs/fidelity/evidence/` with the raw per-call
snapshots dropped, which silently broke `--reuse-retail`: the driver looked for
`retailObservations`, found none, and reported zero retail observations, so every
native came out inconclusive. It now falls back to the per-native aggregate the
archive does keep (`retail_observed_calls`, `retail_ret_classes`,
`retail_argument_class_patterns`) and rebuilds one final snapshot per native.
Verified against the corrected capture: retail 83 observed, candidate 18,
matched 8, mismatched 5 - the same verdicts as the full paired run, without
relaunching retail.
