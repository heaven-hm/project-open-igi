# Native delta classification (post-merge structural natives)

This classifies the natives left at `STRUCTURALLY_VERIFIED` after the terrain merge, using the
live differential (`scripts/native_leaves_runner.py`): retail x86 bytes under Radare2 ESIL vs the
compiled C++ candidate on identical inputs (scalar args by seed, pointer args against mapped
zeroed memory with argument-buffer memory-effect comparison).

Categories: **GENUINE_LOGIC_DELTA** (fix the port), **ABI_ADAPTATION** (host pointer width / host
callback dispatch - cannot match retail byte-for-byte), **INCONCLUSIVE** (harness cannot establish
equivalence).

| address | native | outcome | category | basis |
|---|---|---|---|---|
| 0x00401050 | QTask_OpenEvent | ERROR candidate failed for seed 0x0: dyl | INCONCLUSIVE | ERROR candidate failed for seed 0x0: dyld[81713]: symbol not |
| 0x004028A0 | Flow_SetState | PASS (8/8) | RUNTIME_TESTED | Fresh Radare2 ESIL and candidate comparison; release threshold still requires VERIFIED |
| 0x004952F0 | FUN_004952F0 | ERROR 4 errors generated. | INCONCLUSIVE | ERROR 4 errors generated. |
| 0x00497E70 | Transcontext_SetActiveTransContext | ERROR 5 errors generated. | INCONCLUSIVE | ERROR 5 errors generated. |
| 0x004A53B3 | Game_DataSymbolLoad | ERROR 4 errors generated. | INCONCLUSIVE | ERROR 4 errors generated. |
| 0x004AA670 | Mem_AllocChunk | angr x86 differential PASS (13/13) | VERIFIED_WITH_DOCUMENTED_SCOPE | returned pointer normalized to null/non-null; allocation request sizes match in documented scope; see `scopes/allocator-default-heap.md` |
| 0x004AA682 | Mem_AllocWithRetry | angr x86 differential PASS (16/16) | VERIFIED_WITH_DOCUMENTED_SCOPE | size guard, sentinel/null exits, allocation request sizes match with null new-handler; see scope document |
| 0x004AA6AE | Mem_AllocateRawBlock | angr x86 differential PASS (24/24) | VERIFIED_WITH_DOCUMENTED_SCOPE | default heap and fallback cases match; custom-pool allocations are excluded and remain a real discrepancy |
| 0x004AF7B0 | Error_Show | ERROR 4 errors generated. | INCONCLUSIVE | ERROR 4 errors generated. |
| 0x004AF8F0 | HeapList_Insert | angr x86 differential PASS (12/12) | VERIFIED | return and exact list/node memory match for empty and 1-11 node lists |
| 0x004AF960 | HeapList_Remove | angr x86 differential PASS (15/15) | VERIFIED | return and list/node memory match for first, middle, and last removal |
| 0x004B0C60 | Mem_Alloc | angr x86 differential PASS (23/23) | INCONCLUSIVE | successful heap-mode-0 allocations match; OOM, nonempty initial heaps, custom heap modes, new-handler retries, and real CRT behavior remain untested |
| 0x004B1690 | Device_CloseStream | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B16D0 | Resource_Unpack | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B1780 | Device_GetStreamPosition | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B17F0 | Device_SeekStream | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B1E90 | String_FindExtension | ERROR Command '['/opt/homebrew/bin/r2',  | INCONCLUSIVE | ERROR Command '['/opt/homebrew/bin/r2', '-q', '-e', 'bin.rel |
| 0x004B23B0 | Texture_UploadMipmapPacket | MISMATCH(8/8) seed-0x0:ref=-1,cand=0; se | ABI_ADAPTATION | pointer return is a 64-bit host address vs retail 32-bit; host callback table returns 0 vs retail renderer return |
| 0x004B24C0 | Texture_LoadLoopTextureFile | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B59F0 | Resource_CreateNode | ERROR Command '['/opt/homebrew/bin/r2',  | INCONCLUSIVE | ERROR Command '['/opt/homebrew/bin/r2', '-q', '-e', 'bin.rel |
| 0x004B5C90 | Resource_FreeNode | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004B5F80 | Resource_RegisterBuffer | MISMATCH(8/8) seed-0x0:ref=0,cand=437380 | ABI_ADAPTATION | pointer return is a 64-bit host address vs retail 32-bit; host callback table returns 0 vs retail renderer return |
| 0x004B7CF0 | Texture_UploadPalettePacket | MISMATCH(8/8) seed-0x0:ref=1879080948,ca | ABI_ADAPTATION | pointer return is a 64-bit host address vs retail 32-bit; host callback table returns 0 vs retail renderer return |
| 0x004B7E10 | Qvm_ReportError | ERROR 4 errors generated. | INCONCLUSIVE | ERROR 4 errors generated. |
| 0x004BCFD0 | Qvm_ExecuteInstructions | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004BD220 | Qvm_LoadCompiledProgram | ERROR 6 errors generated. | INCONCLUSIVE | ERROR 6 errors generated. |
| 0x004BDA00 | Qvm_PushInteger | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF080 | DynObj_LoadModelTexturePackage | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF300 | DynObj_LookupHashItem | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF330 | DynObj_LoadPackageMeshes | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF390 | DynObj_LoadModelMesh | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF630 | DynObj_LoadPackageLovs | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF790 | DynObj_LoadModelTextures | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF870 | DynObj_LoadPackagePalettes | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004CF910 | DynObj_LoadPackageAnimations | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004D0CF0 | Model_GetAttachment | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |
| 0x004D0D10 | Model_GetAttachmentCount | ERROR candidate failed for seed 0x0: | INCONCLUSIVE | ERROR candidate failed for seed 0x0: |

## Summary

- ABI_ADAPTATION: 7
- INCONCLUSIVE: 29
- GENUINE_LOGIC_DELTA: 0

`ABI_ADAPTATION` is a diagnostic category, not a verification result. Three allocator entries now have hash-bound scoped comparisons. Their scopes normalize allocation addresses and explicitly exclude custom-pool behavior where the production source still has stub callees. The remaining ABI-boundary entries stay inconclusive until their own fresh runners compare the named observable fields.

### Notes
- The four `App_ContextSet*` setters were a genuine regression (the merge made them return `void`);
  retail executes `movsx eax,[esp+4]; mov [g],eax; retn`. They were fixed and now pass the differential.
- `Ilff_ReadContainerHeader`, `Ilff_GetFormType`, `Ilff_GetChunkTag`, `Resource_CanonicalizePath`,
  `Resource_CalculatePathHash`, `Qvm_SetOpcodeHandler`, `Terrain_LoadTexturePackage`, `Game_GetContext`,
  `Loop_SetStatusByte`, `Resource_Load` moved from MISMATCH/ERROR to PASS once pointer arguments were
  exercised against mapped memory and `void` natives were compared by memory effect.
- `Mem_AllocChunk`, `Mem_AllocWithRetry`, default/fallback `Mem_AllocateRawBlock`, and successful
  `Mem_Alloc` paths execute as retail x86 and candidate x86 under angr VEX. `Mem_Alloc` matches
  all 23 recorded cases; the retail Binary Ninja/Radare2 CFG agreement is 1.00 (8 blocks, 11 edges).
  That graph score compares the two retail analyses, not candidate CFG parity. The custom-pool
  callees remain local stubs and are excluded from the raw allocator's scope. OOM, nonempty initial
  heaps, custom heap modes, new-handler retries, and real CRT behavior remain untested.
- `Texture_Upload*Packet` have no meaningful packet-memory comparison yet: the current scalar ESIL
  seeds write no packet bytes, and renderer callback dispatch is not modeled by that harness.
- Variadic natives (`Error_Show`, `Qvm_ReportError`, `Game_DataSymbolLoad`) and context-graph natives
  (`DynObj_*`, `Device_*`) cannot be driven by the generic harness and stay INCONCLUSIVE.

## Current gate state

The latest `./build.sh` release gate checked 80 production natives in production-tree mode and blocked on 46 evidence and status failures. It did not reach CMake or CTest. `Mem_Alloc` meets the 0.85 retail BN/Radare2 CFG agreement threshold and has fresh 23-case success-path evidence, but remains `INCONCLUSIVE` overall because important paths listed above are untested.

Of the eight ABI-boundary functions, three allocator entries pass with documented limits, `Mem_Alloc` passes 23 modeled successful-allocation cases, and `HeapList_Insert` passes a direct x86 comparison. Three remain inconclusive (`Texture_UploadMipmapPacket`, `Texture_UploadPalettePacket`, and `Resource_RegisterBuffer`). `HeapList_Remove` also passes a direct x86 comparison. Other inconclusive records need valid per-native behavioral runners or a compatible retail-x86 runtime. angr recovered 79 of 80 exact entries; `0x004AF7B0` retains unresolved jumps. Raw CFG comparisons between angr and Binary Ninja/Radare2 show substantial disagreements on several functions and need investigation; they are not source-equivalence scores.

`build.sh` stopped at its required production gate, before CMake compilation, CTest, or executable deployment. I separately rebuilt the host test targets and passed 36/36 CTest cases, and linked the Windows x86 executable. Keep release blocked until the production gate passes with current binary-bound evidence and behavioral comparisons.
