# Retail Native Naming and Provenance

C++ methods that port or directly model a retail `igi.exe` game native use a readable subsystem/action name:

- `Game_SetThing()`
- `Level_LoadParams()`
- `HumanPlayer_UpdateMotion()`
- `Weapon_Fire()`
- `SoundSystem_DefineSound()`

Use full words and stable names. Do not use address suffixes, numbered guesses, or shortened variable names as substitutes for recovered meaning.

Put a nearby source comment on each native port that identifies it as an original IGI.EXE native and records the retail virtual address. Include the available REA evidence ID and static CFG result when known. State clearly when the C++ function is only a helper or partial port; a related native address does not prove that the whole retail routine has been reproduced. Runtime parity must be described separately from static identification.

Keep QVM symbol strings exactly as shipped in the retail scripts because they are part of the script ABI. The C++ wrapper or implementation name may use the subsystem/action convention, but the QVM symbol itself must not be renamed.

## Evidence-backed names — 2026-10-18

A stripped executable does not disclose original C++ symbol spellings. The following are descriptive
names supported by observed bodies, not claims that original source symbols were recovered.
Addresses and old aliases remain in evidence records. QVM/export strings are not renamed.

| Retail address | Descriptive C++ name | Previous misleading alias | Verified role |
| --- | --- | --- | --- |
| 0x004B80B0 | Qvm_Load | unchanged | Script resource load/compiled selection path |
| 0x004B83D0 | Qvm_Cleanup | unchanged | Releases loaded QVM bytecode object |
| 0x004B8A10 | Qvm_SetGlobalState | Sys_PopStack_23 | Stores QVM status dword to 0x00A965A0 |
| 0x004B7E10 | Qvm_ReportError | Sys_FreeMemory_22 | Formats script compilation/load error buffer |
| 0x0048F1D0 | Script_GetDebugCompileState | FX_FragmentSimulate_24 | Returns script debug/compile state at 0x005C8BEC |
| 0x0048F340 | Script_GetForcePrecompiledState | FX_FireBallUpdate_27 | Returns force-precompiled QVM state at 0x005C8BF0 |
| 0x004B1E90 | String_FindExtension | Sys_OpenFileStream_7 | Reverse scans path for file extension dot |
| 0x004AD73D | String_ToUpperChar | Camera_ApplyWideScreen_28 | ASCII character uppercase helper |
| 0x004B5AF0 | Resource_CanonicalizePath | Sys_ResolveSymbol_15 | Converts path to uppercase and normalizes `\` to `/` |
| 0x004B5B30 | Resource_CalculatePathHash | Sys_CheckHeap_15 | Multiplicative hash calculation for resource paths |
| 0x004B5B60 | Resource_FileExists | Sys_AllocateMemory_16 | Probes if resource is cached or exists on device |
| 0x004B5B90 | Is_ResourceLoaded | unchanged | Probes resource hash table for loaded resource |
| 0x004B5F00 | Resource_Load | unchanged | Primary retail resource lookup and loader |
| 0x004B6380 | Resource_Unload | unchanged | Unloads resource node by virtual path |
| 0x004B59F0 | Resource_CreateNode | Sys_PopStack_15 | Allocates and links resource node in hash table |
| 0x004B5C90 | Resource_FreeNode | Sys_FreeMemory_16 | Recursively unlinks and frees resource node tree |
| 0x004B60F0 | Resource_RegisterIlffContainer | Sys_ErrorInResourceFileS | Parses ILFF/IRES chunks and registers child nodes |
| 0x004B5F80 | Resource_RegisterBuffer | Sys_Resource | Registers loaded memory buffer into resource system |
| 0x004AAD10 | Resource_MatchPrefix | Camera_ApplyLineFilter_21 | Bounded string prefix comparison helper |
| 0x004B1020 | QFile_AliasResolve | unchanged | Resolves virtual path alias via device table |
| 0x004B1780 | Device_GetStreamPosition | Sys_PushStack_4 | Dispatches stream tell/position operation |
| 0x004B17F0 | Device_SeekStream | Sys_CheckHeap_4 | Dispatches stream seek operation with offset |
| 0x004B1690 | Device_CloseStream | Sys_ReadFileBlock_4 | Dispatches stream close/flush operation |
| 0x004CF080 | DynObj_LoadModelTexturePackage | DynObj_InsertHashItem_14 | MTP package loader and chunk dispatcher |
| 0x004CF390 | DynObj_LoadModelMesh | DynObj_LocalModelsSMef | MEF model parser and mesh initialization |
| 0x004CF910 | DynObj_LoadPackageAnimations | DynObj_RemoveSymbol_14 | Loads animation sub-chunks from package |
| 0x004CF870 | DynObj_LoadPackagePalettes | DynObj_LinkDynamicCube_14 | Loads palette sub-chunks from package |
| 0x004CF330 | DynObj_LoadPackageMeshes | DynObj_CreateMagicObject_14 | Loads mesh entries from package container |
| 0x004CF630 | DynObj_LoadPackageLovs | DynObj_ParseConfigFile_14 | Loads level-of-vision metadata chunks |
| 0x004CF790 | DynObj_LoadModelTextures | DynObj_LocalTexturesSTex | Loads model texture package resources |
| 0x004CF300 | DynObj_LookupHashItem | DynObj_LookupHashItem_14 | Lookups item pointer by index in dynobj hash list |
| 0x004CEAA0 | Ilff_FindFormChunk | DynObj_CreateMagicObject_13 | Scans container for matching FORM type |
| 0x004B7470 | Ilff_GetNextChunk | Sys_PopStack_19 | Advances to the next chunk in ILFF container |
| 0x004B7490 | Ilff_GetFirstChunk | Sys_ResolveSymbol_19 | Returns pointer to the first chunk in container |
| 0x004B74D0 | Ilff_GetChunkPayload | Sys_CheckHeap_19 | Returns payload pointer for chunk |
| 0x004B7540 | Ilff_ReadContainerHeader | Sys_AllocateMemory_20 | Reads container header and total byte size |
| 0x004B7570 | Ilff_GetFormType | Sys_FreeMemory_20 | Returns container FORM FourCC type |
| 0x004B7580 | Ilff_GetChunkTag | Sys_OpenFileStream_20 | Returns 32-bit chunk tag |
| 0x004B7590 | Ilff_FindChunk | Sys_ReadFileBlock_20 | Finds chunk with specified FourCC tag |
| 0x004B0F80 | Texture_GetActiveBackendId | Sys_ReadFileBlock_3 | Returns active graphics device backend index |
| 0x004B26A0 | Texture_EnableUpload | Sys_PopStack_8 | Enables texture upload flag |
| 0x004B26B0 | Texture_DisableUpload | Sys_ResolveSymbol_8 | Disables texture upload flag |
| 0x004B23B0 | Texture_UploadMipmapPacket | Sys_FreeMemory_8 | Uploads mipmap levels to Direct3D device |
| 0x004B7CF0 | Texture_UploadPalettePacket | Sys_PushStack_21 | Uploads 256-color palette data |
| 0x004B24C0 | Texture_LoadLoopTextureFile | Sys_SIsNotAValidTextureF | Loads raw texture loop packet |
| 0x005175A0 | Terrain_LoadTexturePackage | Terrain_TextureFileNotOfRequ | Loads and initializes terrain texture package |
| 0x004CEA80 | Render_GetActiveBackendId | DynObj_InsertHashItem_13 | Returns active render device ID |
| 0x004B7450 | Matrix_InvalidateSlot | Sys_PushStack_19 | Clears 32-bit camera matrix cache slot |
| 0x004A0110 | Camera_UpdateTransform | unchanged | Invalidates camera matrix cache slots |
| 0x004A26C0 | Camera_UpdateFogPalette | Camera_UpdateTransform_2 | Sets fog/lighting palette entries from color context |
| 0x004A2600 | Camera_ResetMatrix | unchanged | Writes alternating 4-band fog palette |
| 0x004AF8F0 | HeapList_Insert | Camera_SetFieldOfView_33 | Slices node into intrusive doubly-linked list |
| 0x004AF960 | HeapList_Remove | Camera_ApplyHandiCam_33 | Unlinks node from intrusive doubly-linked list |
| 0x004B0BA0 | Mem_InitHeapState | Sys_CloseFileStream_2 | Initializes heap triplet globals and flag |
| 0x004AD722 | Mem_InvokeNewHandler | Camera_ApplyHandiCam_28 | Calls CRT new_handler callback |
| 0x004AA6AE | Mem_AllocateRawBlock | Camera_ApplyPostProcess_20 | Core mode dispatcher for heap allocation |
| 0x004AA682 | Mem_AllocWithRetry | Camera_ApplyWideScreen_20 | Size-checked allocation with retry loop |
| 0x004AA670 | Mem_AllocChunk | Camera_ApplyHandiCam_20 | Forwards allocation request to retry loop |
| 0x004B0C60 | Mem_Alloc | unchanged | Primary retail engine heap allocator |
| 0x0048F480 | Loop_SetStatusByte | FX_SmokeEmit_27 | Stores loop status byte at 0x005C8DE8 |
| 0x00401050 | QTask_OpenEvent | unchanged | Main frame dispatch boundary via setjmp |
| 0x00402880 | Game_GetContext | unchanged | Returns global game context pointer 0x00567C8C |
| 0x004028A0 | Flow_SetState | unchanged | Writes pending flow state into context +0x2C |
| 0x0049DF60 | DisplayRender_DrawModel | DisplayRender_Processmodelsnotconvert | Model render path with LOD/context/device dispatch |
| 0x004D0CF0 | Model_GetAttachment | Render_DrawBoneMesh_2 | Return attachment record through model/resource/mesh |
| 0x004D0D10 | Model_GetAttachmentCount | Render_DrawSplineMesh_2 | Return signed MESH attachment count |

The two attachment names have exhaustive original-instruction differential results. Other rows retain
their existing per-method verification limits; naming agreement is not a behavioral parity score.
Unresolved identities remain explicitly unresolved rather than receiving an invented action name.
