# The level-load closure: entry point, size, MVP subset, and the ordering hazard

Read-only analysis for the Lead, 2026-10-25, answering "size the level-load closure". Author: `flow-analyst`.
No `native_*.cpp`, no `CMakeLists.txt`, no `native-verification.json` was touched.

Artifact: retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Every extent below is bounded by the `ret` and the alignment nops after it. Each function's body was decoded
from its entry to its first `ret`; **0 bodies in this closure had a branch target beyond that stop**, so the
first-`ret` bound is exact for every function counted here. Call counts distinguish direct (`E8`) from
indirect calls, because that distinction already cost one round on the guard.

---

## 1. Where the level is loaded: `0x00415B30 Level_Start`

**Entry point: `0x00415B30` (`Level_Start`, transcribed at `engine/src/retail/core/game_flow.cpp`).**
Extent `0x00415B30`–`0x0041600B` = **326 instructions, 90 call instructions** — **89 direct plus one indirect**,
`call dword ptr [eax*4 + 0xA94E84]` at `0x00415D1D`.

**It has no call site anywhere in the image.** A byte scan of all of `.text` for `E8`/`E9` targeting
`0x00415B30` returns nothing. It is reached as a **registered QTask event handler**, and the registration is
in the guard:

```
0x00415556  call 0x00401900                     ; TaskType_Register   -> class id in AX
0x0041555D  mov  word [0x0057B1A2], ax          ; store the id
0x00415618  mov  ax, word [0x0057B1A2]          ; read it back
0x0041561E  push 0x00415B30                     ; Level_Start, as a raw code immediate
0x00415623  push 0
0x00415625  push eax
0x00415626  call 0x00401400                     ; QTask_LinkQTaskEventHandler
0x0041562B  mov  cx, word [0x0057B1A2]
0x00415632  push 0x00416260
0x00415637  push 1
0x00415639  push ecx
0x0041563A  call 0x00401530                     ; QTask_LinkQTaskEventHandlerNoContextFn
0x0041563F  mov  dx, word [0x0057B1A2]
0x0041564E  call 0x00401530                     ; QTask_LinkQTaskEventHandlerNoContextFn
```

All of that is inside **`0x00415530 Game_Init`** (168 instructions, extent `0x00415530`–`0x004157CD`, ret at
`0x004157CD`) — which is **guard call #6**. So the level-load entry is registered *by the guard*, against the
class id `TaskType_Register` returned one instruction earlier, and the handler address is a raw immediate.
That is why the entry has no caller: it is dispatched, not called.

`Level_Start`'s direct callees are an unmistakable load sequence, in order: `Flow` init helpers
(`0x00402820`, `0x004028D0`), `LoadingScreenShow 0x0048A440`, `Config_SetResolution 0x00406390`,
`AnimSound_LoadParameters 0x004C1780`, **`Terrain_LoadTexturesAndLightmaps 0x004C5F00`**,
**`Terrain_Load 0x004C6240`**, `SoundLoad 0x004E68D0` ×2, `Resource_Load 0x004B5F00` ×2, `0x0046B070`
(170 instructions, 38 calls, no file), `LevelLoad 0x004F0E10`, `FileOpen 0x004A5350`, `sprintf 0x004A53B3` ×5,
`Debug_Error 0x004AF7B0` ×2, and the getters of §4.

---

## 2. Size, with file coverage

Command (capstone 5.0.7, exact image, boundary decode, first-`ret` bound, manifest as the VA→file index):

```
python3 /tmp/q218/closure.py 0x415B30 <depth>        # the BFS used for this table
```

| depth | functions | instructions | call sites | distinct targets | targets with a file | targets without |
|---|---|---|---|---|---|---|
| 1 | 66 | 2469 | 253 | 146 | 61 | 85 |
| 2 | 147 | 5666 | 436 | 226 | 108 | 118 |
| 3 | 227 | 8293 | 532 | 259 | 126 | 133 |

**The closure is not saturated at depth 3**: depth 2→3 adds 80 functions and 33 distinct targets, and the
growth is not flattening. So the table is a **lower bound**, not the closure's size. What is definite:

- **[M]** At depth 3 the closure touches **227 functions, 8293 instructions, 532 call sites, 259 distinct
  targets**, of which **126 have a transcribed file and 133 do not**.
- **[M]** Depth 1 alone is **66 functions / 2469 instructions / 253 call sites / 146 distinct targets /
  61 with a file**, and it is the part that contains the whole scene path named in §3.
- Of the depth-1 146 targets, 85 have no file. Those 85 are the immediate distance, and unlike the guard's
  308 they are **on the visibility path** — `Terrain_Load` and `Terrain_LoadTexturesAndLightmaps` are among
  the depth-1 targets that already have files, and they call the missing ones.

---

## 3. The MVP-first subset: identifiable, and here it is

**Unlike the window question, this subset is identifiable** — the level-load closure has a single obvious
entry (`Level_Start`) and its scene branches are explicit in the call sequence. The minimum that puts terrain
on screen is the terrain branch plus the resource/QVM plumbing it stands on:

| what | VA | size | file |
|---|---|---|---|
| `Terrain_Load` | `0x004C6240` | 363 ins, 18 calls | `terrain_loaders.cpp` |
| `Terrain_LoadTexturesAndLightmaps` | `0x004C5F00` | 213 ins, 16 calls | `terrain_loaders.cpp` |
| `Terrain_LoadTexturePackage` | `0x005175A0` | depth 2 | `texture_loaders.cpp` |
| `Texture_UploadMipmapPacket` / `Texture_LoadLoopTextureFile` / `Texture_UploadPalettePacket` | `0x004B23B0` / `0x004B24C0` / `0x004B7CF0` | depth 2 | `texture_loaders.cpp` |
| `Sys_EnterRenderMode` / `Sys_LeaveRenderMode` | `0x004B0F90` / `0x004B0FD0` | depth 2 | `native_4b0f90.cpp` / `native_4b0fd0.cpp` |
| `Resource_Load` / `Resource_FileExists` | `0x004B5F00` / `0x004B5B60` | depth 1/2 | `resource_lifecycle.cpp`, `qvm_native_loader.cpp` |
| `Qvm_Load` / `Qvm_Cleanup` / `Qvm_Execute` | `0x004B80B0` / `0x004B83D0` / `0x004B85B0` | depth 1 | `qvm_native_loader.cpp`, `qvm_execute.cpp` |
| `FileOpen` | `0x004A5350` | depth 1 | `004A5350-File_Open.cpp` |
| `Mem_Alloc` | `0x004B0C60` | depth 1 | `mem_alloc.cpp` |

**The terrain branch, sized on its own — `Terrain_Load 0x004C6240` at depth 1: 11 functions, 607
instructions, 38 call sites, 26 distinct targets, 20 with a transcribed file, 6 without.** Its missing six are
`0x004CE3D0`, `0x004D11C0` and four reached through them. **Those six are the concrete first increment for the
terrain path**, and the branch is 77% covered — the best-covered part of the closure measured so far.

For **one object on screen**, the model branch sits at depth 3 and is already transcribed:
`DynObj_LoadPackageMeshes 0x004CF330`, `DynObj_LoadPackageLovs 0x004CF630`,
`DynObj_LoadModelTextures 0x004CF790`, `DynObj_LoadPackagePalettes 0x004CF870`,
`DynObj_LoadPackageAnimations 0x004CF910` (`mtp_package_loader.cpp`, `texture_loaders.cpp`),
`DynObj_LoadModelTexturePackage 0x004CF080` and `DynObj_ParseConfigFile_12 0x004CE910` at depth 2, with the
ILFF chunk readers (`Ilff_GetFirstChunk`/`GetChunkPayload`/`ReadContainerHeader`/`GetFormType`/`GetChunkTag`/
`FindChunk`, `ilff_navigation.cpp`), `Ilff_FindFormChunk`/`DynObj_LookupHashItem` (`hash_lookup.cpp`) and
`Camera_ResetMatrix_26 0x004AD140` / `Camera_UpdateTransform_27 0x004AD1B0` at depth 3.

**So the MVP-first subset is: `Level_Start`'s terrain branch (6 missing files) + the DynObj/ILFF model branch
(transcribed) + the texture-upload and render-mode pair (transcribed).** It is a lower bound, not a proven
minimum: nothing here proves that a smaller set suffices, and the closure is unsaturated, so the "minimum that
puts terrain and one object on screen" cannot be *closed* from static structure alone — the terrain branch's
6 missing files are the first thing that has to exist to test the claim.

---

## 4. The ordering hazard: this closure has the same class of dependency, and it is worse

**4.1 — `0x0057B1A2`, the class id that makes the level-load entry exist.** Written at **one** site,
`0x0041555D` (`mov word [0x57B1A2], ax`, immediately after `TaskType_Register` at `0x00415556`), and read
three times in the same routine — `0x00415618`, `0x0041562B`, `0x0041563F` — **each read feeding one of the
three handler registrations**. Nothing else in the image writes it. Consequence: **the level-load closure
cannot be entered unless the guard's `Game_Init` ran**, because without that write the registration uses a
stale id and `Level_Start` is never bound to an event. This is the guard→scene dependency stated as a
written-then-read pair, in the same 168-instruction routine.

**4.2 — `0x0057BABC`, the current-level pointer.** Written at exactly one site, `0x00415B9B`
(`mov dword [0x57BABC], ebp`, inside `Level_Start`), and referenced 23 times inside `Level_Start`'s own
extent (60 raw references image-wide). Two getters read it from elsewhere:

```
0x00416D40  mov eax, [0x0057BABC] ; ret                     <- the level getter
0x00416920  mov eax, [0x0057BABC] ; mov eax, [eax+0x20] ; ret  <- level -> field +0x20
```

`Level_Start` calls them **7 times** — `0x00416D40` at `0x00415DBE`, `0x00415E0A`, `0x00415E30`,
`0x00415EFF`; `0x00416920` at `0x00415DFA`, `0x00415E29`, `0x00415F2D` — **every call after the write at
`0x00415B9B`**. So the pointer must be set before any of that work, and any subset that runs a reader without
the writer dereferences a stale level pointer, `[eax+0x20]` included.

**4.3 — What this means for "run only what we need".** The same conclusion as the guard, one level deeper:
the closure's entry is *created by* the guard, the closure's own state is written once and read many times
after, and both facts are invisible to a build or a link check. A subset is possible in principle — the
terrain branch is self-contained enough to size — but it cannot be *entered* without the guard's `Game_Init`
and `TaskType_Register` having run first.

---

## 5. Unresolved, and what I did not measure

- **The closure is unsaturated.** Depth 3 is the deepest I measured; 80 new functions appeared at that depth
  and the count had not flattened. Any "total size" claim needs a depth-4/5 run or a saturation test.
- **133 of 259 depth-3 targets have no file**, including 85 of the depth-1 146.
- **The dispatch site is not located.** `Level_Start` is registered as a handler; which routine dispatches
  that event, and what state it reads before dispatching, is **not measured here**.
- **The terrain branch's 6 missing targets** (`0x004CE3D0`, `0x004D11C0`, +4) are named but not decoded.
- **`0x0046B070`** (170 instructions, 38 calls, depth 1, no file) is the largest single untranscribed routine
  on the level-load path and is not classified here.
- **The `0x004B8890` chain remains not started.** It is, however, *on this path*:
  `Script_QHashNodeRegisterEx 0x004B8890` and `Script_QHashSymbolRemove_4B8920` are both direct callees of
  `Terrain_Load`. The Lead's chain and the level-load closure meet at `Terrain_Load`.
