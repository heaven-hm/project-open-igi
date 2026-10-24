# File-Format → Retail Native Mapping

**Comparison of `open-igi` (C#) and `project-igi-editor` (C++17) against the verified `igi.exe` natives in**
**`assets/BinaryNinja/natives/CLEAN/`.**

> **Purpose.** Neither `open-igi` nor `project-igi-editor` contains the retail engine. This document does *pattern
> matching* between their implementations and the decompiled `igi.exe` natives, so that each file-format reader can
> be tied back to the **exact retail method + address** and then implemented 1:1 in `project-open-igi`.

---

## 1. Method

1. **Extract** every retail address cited in `open-igi`'s C# sources (`0x…` literals in the `0x400000–0xC40000` range).
2. **Verify** each address with Binary Ninja MCP (`bn_function_info`) against the retail `igi.exe`.
3. **Pattern-match** an implementation to a native when (a) it cites the address, or (b) its structure matches the
   native's decompiled behaviour (used for `project-igi-editor`, which cites almost nothing).
4. **Classify** each match: `VERIFIED` (address cited **and** confirmed in the binary), `INFERRED` (semantic match
   only), `DATA` (cited address is a global in the data section, not a function).

## 2. Corpus results

| Metric | Value |
|---|---|
| `open-igi` C# files citing retail code addresses | **361** |
| distinct (file → address) citations | **1,481** |
| distinct addresses cited | **977** |
| …that are real function starts in `igi.exe` | **782** |
| …that are **data globals** in the data section | **20** |
| …that are **neither** (unmatched code addresses) | **175** |
| `project-igi-editor` files citing retail code addresses | **0** (its code names no retail native at all) |
| retail natives available as ground truth | **5,732** (`natives/CLEAN/`) |

A fourth implementation, `project-igi-converter` (the source of the bundled `igi1conv.exe`), is included in the
format comparison (§9–§10).

So `open-igi`'s citations are usable as a **verified index**, but ~20 % of what it cites is not a function start —
treat every address as a lead to confirm, never as a given.

## 3. Four-way comparison

| | **`natives/CLEAN/`** | **`open-igi`** | **`project-igi-editor`** | **`project-igi-converter`** |
|---|---|---|---|---|
| Is it the retail code? | **Yes** — decompilation of `igi.exe` | No | No | No |
| Language | n/a (it *is* the code) | C# | C++17 (editor) | C++ / Qt (converter) |
| Loaders | n/a | own C#, one per format | few in-process; rest via `igi1conv.exe` | own C++, `source/parsers/*` |
| Links code→retail address | address **is** the identity | **782** verified function citations | **0** | **0** |
| Format-level tag fidelity | the tags *are* in the code | high (see §9) | low | **highest** (see §9) |
| Mission bytecode | n/a | own QVM interpreter | own QVM interpreter | own QVM parser/compiler |
| Licence | derived data | open | Heaven-HM source-available | Heaven-HM source-available |
| Role for the port | **ground truth** | **address index** | format reference | **best format reference** |

**CLEAN exports are decompiler evidence, not original source or proof of a compilable 1:1 port.**

Function existence, inferred names, recovered types and behavioral parity are separate checks. Renaming registers or compiling a translation does not establish semantic parity.

---

## 4. Per-format mapping

### ILFF / IRES container

- **Retail natives (verified):** Resource_Load `0x4B5F00`, Sys_Resource `0x4B5F80`, Resource_Unpack `0x4B16D0`
- **`open-igi`:** `Formats/Ilff/IresArchive.cs`, `IlffContainer.cs`
  - cited addresses: `0x4B60F0` `Sys_ErrorInResourceFileS`<br>`0x4B5F80` `Sys_Resource`
- **`project-igi-editor`:** `renderer/res_writer.cpp`, `renderer/res_compiler.cpp`, `level/res_model_set.cpp` — *in-process (write)*

### MEF model

- **Retail natives (verified):** DynObj_LocalModelsSMef `0x4CF390`
- **`open-igi`:** `Formats/World/MefModel.cs`, `MefMeshFormat.cs`, `MefVertex.cs`, `MefDrawRecord.cs`, `MefCollisionMesh.cs`, `MefAttachment.cs`, `MefSkinInfluence.cs`
  - cited addresses: `0x49B4B0` `Display_MeshFormatNotSupport`<br>`0x49B700` `Bone_PaletteSkinner`<br>`0x49F700` `Mesh_SkinBones`<br>`0x49FB10` `Display_ClearSurfaces_20`<br>`0x4C1840` `RigidDynCubeObj_Setup`<br>`0x4CF390` `DynObj_LocalModelsSMef`<br>`0x4DFA50` `Render_MagicObjectTypeSNotD`<br>`0x523D40` `Light_RenderLensflare_3`
- **`project-igi-editor`:** `renderer/mef_native.cpp` — *in-process (read)*

### MTP package

- **Retail natives (verified):** DynObj_InsertHashItem_14 `0x4CF080`, DynObj_CreateMagicObject_13 `0x4CEAA0`, DynObj_RegisterSymbol_14 `0x4CEFA0`
- **`open-igi`:** `Formats/World/MtpPack.cs`, `MtpFile.cs`, `MtpChunk.cs`, `MtpTags.cs`, `PackId.cs`
  - cited addresses: `0x4CEAA0` `DynObj_CreateMagicObject_13`<br>`0x4CEFA0` `DynObj_RegisterSymbol_14`<br>`0x4CF080` `DynObj_InsertHashItem_14`<br>`0x4CF690` `DynObj_UpdateDynTree_14`
- **`project-igi-editor`:** `level/mtp_writer.cpp` — *delegated to igi1conv*

### TEX texture

- **Retail natives (verified):** DynObj_LocalTexturesSTex `0x4CF790`
- **`open-igi`:** `Formats/Textures/TexFile.cs`, `TextureDecoder.cs`, `TextureDescriptor.cs`, `TexAtlasFile.cs`, `MipChainBuilder.cs`
  - cited addresses: `0x4B2070` `Sys_PopStack_7`<br>`0x4B23B0` `Sys_FreeMemory_8`<br>`0x4B24C0` `Sys_SIsNotAValidTextureF`<br>`0x50AD90` `Physics_InterpolateWaypoint_12`<br>`0x517C90` `Terrain_RenderQuadtree_7`<br>`0x517DD0` `Terrain_SelectTerrainLOD_7`<br>`0x517F00` `Terrain_UpdateWaterWaves_7`<br>`0x517FD0` `Terrain_RenderWaterSurface_7`
- **`project-igi-editor`:** `renderer/olm_texture.cpp`, `renderer/tex_writer.cpp`, `renderer/renderer_objects_texture.cpp` — *delegated (convert)*

### Terrain

- **Retail natives (verified):** DynObj_Createterrainmateria `0x4C6240`, DynObj_ParseConfigFile_7 `0x4C5F00`, CreateTerrainTileMap `0x4C6760`, Terrain_GenerateOctree `0x4D14E0`
- **`open-igi`:** `Formats/World/Terrain*.cs` (11 files)
  - cited addresses: `0x4C1840` `RigidDynCubeObj_Setup`<br>`0x4C5F00` `DynObj_ParseConfigFile_7`<br>`0x4C6240` `DynObj_Createterrainmateria`<br>`0x4C9020` `DynObj_LinkDynamicCube_9`<br>`0x4C9A30` `DynObj_RegisterSymbol_10`<br>`0x4CA010` `DynObj_InsertHashItem_10`<br>`0x4CABF0` `DynObj_LookupHashItem_10`<br>`0x4CB050` `DynObj_CreateMagicObject_10`<br>`0x4CB810` `DynObj_ParseConfigFile_10`<br>`0x4CBE40` `DynObj_RemoveSymbol_10`<br>`0x4CC2A0` `DynObj_FindSymbol_11`<br>`0x4CDB20` `World_TraceBatch`<br>`0x517940` `Terrain_SelectTerrainLOD_6`<br>`0x517980` `Terrain_UpdateWaterWaves_6`<br>`0x517A00` `Terrain_SampleHeightmap_6`<br>`0x548060` `NO_FUNCTION` *(not a function start)*<br>`0x54844C` `NO_FUNCTION` *(not a function start)*<br>`0x54EBA0` `NO_FUNCTION` *(not a function start)*<br>`0xA7E06C` `NO_FUNCTION` *(data/global)*<br>`0xB12BB8` `NO_FUNCTION` *(data/global)*<br>`0xB12BE0` `NO_FUNCTION` *(data/global)*<br>`0xB16CE0` `NO_FUNCTION` *(data/global)*<br>`0xBA2030` `NO_FUNCTION` *(data/global)*<br>`0xBA5050` `NO_FUNCTION` *(data/global)*<br>`0xBA5060` `NO_FUNCTION` *(data/global)*<br>`0xBA80B0` `NO_FUNCTION` *(data/global)*<br>`0xBCAAE0` `NO_FUNCTION` *(data/global)*<br>`0xC28FE0` `NO_FUNCTION` *(data/global)*<br>`0xC30080` `NO_FUNCTION` *(data/global)*
- **`project-igi-editor`:** `level/terrain_files.cpp`, `level/terrain_io.cpp`, `level/terrain_mesh.cpp`, `level/terrain_lod.cpp`, `renderer/renderer_terrain.cpp` — *in-process (read)*

### QVM / QSC script

- **Retail natives (verified):** Qvm_Load `0x4B80B0`, Qsc_Compile `0x4B8410`, Qvm_Cleanup `0x4B83D0`
- **`open-igi`:** `Scripting/Qvm/QvmInterpreter.cs`, `QvmProgram.cs`, `QvmDisassembler.cs`
  - cited addresses: —
- **`project-igi-editor`:** `level/qvm_interpreter.cpp`, `level/qvm_parser.cpp`, `level/qvm_compiler.cpp`, `level/qvm_decompiler.cpp`, `level/qsc_lexer.cpp`, `level/qsc_parser.cpp` — *in-process (VM)*

### Animation / IFF

- **Retail natives (verified):** Bones_PlayAnimation `0x4D61D0`, Bone_PaletteSkinner `0x49B700`, Mesh_SkinBones `0x49F700`
- **`open-igi`:** `Formats/Animation/*` (11 files)
  - cited addresses: `0x464BD0` `AI_CombatTarget_23`<br>`0x49B700` `Bone_PaletteSkinner`<br>`0x4D2D60` `Render_FailedToCreateBoneob`<br>`0x4D2FB0` `Render_TransformVertices_5`<br>`0x4D38C0` `Render_DrawBoneMesh_7`<br>`0x4D3A90` `Render_DrawSplineMesh_7`<br>`0x4D41C0` `Render_TransformVertices_7`<br>`0x4D4500` `Render_ComputeNormals_7`<br>`0x4D47D0` `Render_DrawSplineMesh_8`<br>`0x4D4840` `Render_DrawLightmapMesh_8`<br>`0x4D4B60` `Render_SkinBones_8`<br>`0x4D5080` `PhysicsRigid_Invoke8_4D5080`<br>`0x4D5180` `PhysicsRigid_ExecutePipeline_4D5180`<br>`0x4D5295` `NO_FUNCTION` *(not a function start)*<br>`0x4D5EC0` `Render_DrawLightmapMesh_9`<br>`0x4D5FF0` `Render_ComputeNormals_9`<br>`0x4D61D0` `Bones_PlayAnimation`<br>`0x4D67A0` `Render_FileSIsNotIffFile`
- **`project-igi-editor`:** `animation.cpp`, `player_animation_driver.cpp`, `animation_motion.h` — *in-process*

### AI graph

- **Retail natives (verified):** Graph_Open `0x4F9FF0`, Graph_Init `0x4FAA60`, AIGraph_Action `0x509480`, Graph_Cleanup `0x4FAFE0`
- **`open-igi`:** `Formats/Ai/GraphFile.cs`, `GraphNode.cs`, `GraphLink.cs`, `GraphRouteTable.cs`, `GraphStreamReader.cs`, `GraphNearestNode.cs`
  - cited addresses: `0x4F9510` `Script_GraphnodeIdAlreadyUs`<br>`0x4F9620` `Script_ParseEnumReal32_23`<br>`0x4F9720` `Script_ExecuteLevelScript_23`<br>`0x4F9770` `Script_CallNativeHandler_23`<br>`0x4F9B80` `Script_SpawnNewTask_24`<br>`0x4F9CD0` `Script_ParseEnumString_24`<br>`0x4F9DF0` `Script_GraphstreamCollision`<br>`0x4F9E70` `Script_ExecuteLevelScript_24`<br>`0x4F9FF0` `Graph_Open`<br>`0x4FA160` `Script_SpawnNewTask_25`<br>`0x4FA3B0` `Script_ParseEnumInt32_25`<br>`0x4FA9D0` `Script_ExecuteLevelScript_27`<br>`0x4FAA10` `Script_CallNativeHandler_27`<br>`0x4FAA60` `Graph_Init`<br>`0x4FAE50` `Graph_OnEvent`<br>`0x4FAF50` `WeatherLightmap_Invoke24`<br>`0x4FAFE0` `Graph_Cleanup`<br>`0x4FB030` `Graph_Cleanup_2`<br>`0x4FB080` `PhysicsObj_Create`<br>`0x508F80` `Physics_ApplyImpulse_7`<br>`0x5092E0` `Physics_SolveConstraints_7`<br>`0x509340` `Physics_CalculateSplineLength_8`<br>`0x509390` `Physics_InterpolateWaypoint_8`<br>`0x509450` `AnimSpline_Invoke27`<br>`0x509480` `AIGraph_Action`<br>`0x5096C0` `Physics_CheckHullCollision_8`<br>`0x5096F0` `Physics_ApplyImpulse_8`<br>`0x509870` `Physics_SolveConstraints_8`<br>`0x5186C0` `Terrain_RenderQuadtree_9`
- **`project-igi-editor`:** `level/qvm_ai_bindings.cpp`, `ai_script_host.cpp` — *in-process*

### Sound (ILSF)

- **Retail natives (verified):** *(out of MVP scope)*
- **`open-igi`:** `Formats/Sound/Ilsf*.cs` (7 files)
  - cited addresses: `0x4952F0` `Display_ClearSurfaces_8`<br>`0x495F70` `Display_SoundFileSNotValidFo`<br>`0x496280` `Display_SwapBuffers_10`<br>`0x496420` `Display_ClearSurfaces_10`<br>`0x543920` `NO_FUNCTION` *(not a function start)*<br>`0x543930` `NO_FUNCTION` *(not a function start)*
- **`project-igi-editor`:** — — *editor: n/a*

### Sprites

- **Retail natives (verified):** Qsprite_Register4Az `0x4B53B0`
- **`open-igi`:** `Formats/Sprites/Sprite*.cs`
  - cited addresses: `0x4B2070` `Sys_PopStack_7`<br>`0x4B6540` `Sys_FreeMemory_17`<br>`0x4B6720` `Sys_IlfFileCorruptS`<br>`0x4B75C0` `Sys_CloseFileStream_20`<br>`0x517FD0` `Terrain_RenderWaterSurface_7`
- **`project-igi-editor`:** — — *n/a*

### DAT

- **Retail natives (verified):** *(no native in catalog)*
- **`open-igi`:** `Formats/World/DatIndex.cs`, `DatEntry.cs`
  - cited addresses: `0x4CEFA0` `DynObj_RegisterSymbol_14`
- **`project-igi-editor`:** `renderer/dat_writer.cpp` — *delegated to igi1conv*

---

## 5. Data-section globals cited by `open-igi`

These are **not** functions — they are globals in the data section, and they are correct as data references. Keep them
as runtime state/table addresses in the port.

| Address | Cited in | Note |
|---|---|---|
| `0xA43E88` | OpenIGI.Engine/Configuration/ConfigVariableRegistry.cs | global |
| `0xA71890` | OpenIGI.Engine/Tasks/TaskTree.cs | global |
| `0xA7E06C` | OpenIGI.Game/World/TerrainChunkMesh.cs | global |
| `0xA96AE0` | OpenIGI.Engine/Tasks/TaskMessage.cs, OpenIGI.Game/World/CutsceneRuntime.cs | global |
| `0xAF5EE0` | OpenIGI.Engine/Tasks/TaskType.cs, OpenIGI.Engine/Tasks/TaskTypeRegistry.cs | global |
| `0xAFA6E0` | OpenIGI.Engine/Tasks/TaskTreeWalker.cs | global |
| `0xAFA7E0` | OpenIGI.Engine/Tasks/TaskTreeWalker.cs | global |
| `0xB12BB8` | OpenIGI.Game/World/TerrainDiscardRegion.cs | global |
| `0xB12BE0` | OpenIGI.Formats/World/TerrainOrientation.cs | global |
| `0xB16CE0` | OpenIGI.Game/World/TerrainChunk.cs | global |
| `0xBA2030` | OpenIGI.Formats/World/TerrainHeightmap.cs, OpenIGI.Game/World/TerrainHeightBinding.cs, OpenIGI.Game/World/TerrainHeightBindings.cs | global |
| `0xBA5050` | OpenIGI.Formats/World/TerrainVisibility.cs, OpenIGI.Game/World/TerrainTileLayer.cs | global |
| `0xBA5060` | OpenIGI.Game/World/TerrainSurface.cs | global |
| `0xBA80B0` | OpenIGI.Formats/World/TerrainLightmaps.cs | global |
| `0xBC2380` | OpenIGI.Game/Config/GameConfig.cs | global |
| `0xBC2384` | OpenIGI.Game/Config/GameConfig.cs | global |
| `0xBC2388` | OpenIGI.Game/Config/GameConfig.cs | global |
| `0xBCAAE0` | OpenIGI.Game/World/TerrainView.cs | global |
| `0xC28FE0` | OpenIGI.Formats/World/TerrainMaterial.cs | global |
| `0xC30080` | OpenIGI.Formats/World/TerrainTileMap.cs | global |

## 6. Unmatched addresses (175)

Cited by `open-igi` but not a function start in the current `igi.exe` analysis. This list includes data and
read-only-data addresses as well as code labels and stale citations. They must not be used as entry points
without checking the PE section and instruction boundary.

```
0x400000, 0x408447, 0x40BBF7, 0x40EF60, 0x40FB6D, 0x410424, 0x4104C6, 0x4104E1, 0x4109E7, 0x410B7E, 0x412622, 0x4126BB, 0x412884, 0x415C92, 0x438458, 0x4384C9, 0x43856B, 0x4385AD, 0x438624, 0x4386B8, 0x4386E4, 0x43C440, 0x43C8A0, 0x43D0E0, 0x444500, 0x446C62, 0x446CCF, 0x446DF0, 0x446E38, 0x447302, 0x447323, 0x4473F5, 0x447422, 0x44EFF3, 0x44FAD2, 0x44FAEB, 0x4551A4, 0x45551C, 0x45574F, 0x4573DB, 0x45A376, 0x45D51D, 0x45F94A, 0x461D71, 0x4626E1, 0x463AD9, 0x463B9C, 0x463C6C, 0x463C98, 0x463D1B, 0x463D20, 0x463D95, 0x463E2F, 0x463E3E, 0x463F10, 0x464044, 0x464099, 0x464178, 0x4641A5, 0x46D3A7, 0x47BAAD, 0x47BB96, 0x47BD88, 0x47C262, 0x47F0D5, 0x47F146, 0x47F4A5, 0x47F515, 0x47FF1B, 0x480312, 0x4812AC, 0x48D5C1, 0x4936E3, 0x4936F7, 0x49370B, 0x493715, 0x4A0B30, 0x4B8BC0, 0x4BB770, 0x4D36A0, 0x4D51D0, 0x4D5220, 0x4D5295, 0x4DD750, 0x4F1574, 0x4F1725, 0x4F5E20, 0x4FD756, 0x4FDA74, 0x4FDBF8, 0x4FE17A, 0x4FE2CB, 0x4FE3F4, 0x4FE40E, 0x5035FA, 0x512C3A, 0x5131A9, 0x513462, 0x514529, 0x5145BB, 0x520D20, 0x520DC0, 0x520EF0, 0x527B41, 0x53331C, 0x5333B8, 0x533430, 0x53367C, 0x534230, 0x536410, 0x536558, 0x537B74, 0x537B90, 0x5385B4, 0x538664, 0x538678, 0x5386AB, 0x5386B0, 0x5386E0, 0x538710, 0x538870, 0x5389F8, 0x538B08, 0x5391B4, 0x539558, 0x539560, 0x53B778, 0x53C94C, 0x53C9C0, 0x53CA34, 0x53ED10, 0x53F1D0, 0x53F318, 0x5412A4, 0x54138C, 0x5413B4, 0x5413DC, 0x541404, 0x54142C, 0x541454, 0x54147C, 0x5414A4, 0x5414CC, 0x5414F4, 0x541590, 0x5415A4, 0x5415B8, 0x5415CC, 0x5415E0, 0x5415F4, 0x541608, 0x54161C, 0x541630, 0x541644, 0x543904, 0x543908, 0x54390C, 0x543914, 0x543920, 0x543930, 0x548060, 0x54844C, 0x54EBA0, 0x54F09C, 0x5655A0, 0x56E0A0, 0x56E258, 0x57D768, 0x57D838, 0x5BDA70, 0x5BE3E0, 0x5C1590, 0x5C8BCC, 0x67A4A0, 0x800000
```

## 7. `project-igi-editor` — no native linkage at all

- Its C++ code names **zero** retail natives (`DynObj_`, `Qvm_`, `Terrain_`, `AI_`, `Resource_`, `Model_Load` → 0 hits).
- Format handling is split: a few **in-process** readers (`renderer/mef_native.cpp`, `level/terrain_files.cpp`,
  `level/qvm_*.cpp`, `qsc_*.cpp`) and everything else **delegated to the bundled `igi1conv.exe`** from
  `heaven-hm/project-igi-conv` (a Qt app, bundled as a prebuilt binary).
- Its `RE.md` contains its own native-discovery log (e.g. candidate `0x0049F700 = Mesh_SkinBones`), but those
  candidates are **not used by the code**. `Mesh_SkinBones @ 0x49F700` **does** verify in the binary; its other
  candidates (`0x4A18F0`, `0x4A19B0`, `0x4A20F0`) do **not** resolve to functions.

**Consequence for the port:** the editor is a good *format* reference (it reads the same files), but it can take you
to the wrong method because it never records which native it mirrors — and it depends on a restrictive-licence
external binary.

## 8. What this tells us (the "accurate guess")

For every format, the retail method to implement 1:1 is the one `open-igi` cites in that reader, once the address is
confirmed in the binary. The strongest per-format anchors:

| Format | Implement 1:1 under this name | Address | Cited by |
|---|---|---|---|
| MEF model | `DynObj_LocalModelsSMef` | `0x4CF390` | `Formats/World/MefModel.cs` |
| MEF geometry hook | `Display_ClearSurfaces_20` *(name unverified; the D3D geometry hook `dword_A94E84[id]`)* | `0x49FB10` | `MefModel.cs`, `MefMeshFormat.cs` |
| Bone skinning | `Mesh_SkinBones` / `Bone_PaletteSkinner` | `0x49F700` / `0x49B700` | `Formats/Animation/*`, `MefSkinInfluence.cs` |
| MTP package | `DynObj_InsertHashItem_14` | `0x4CF080` | `Formats/World/MtpPack.cs` |
| Texture stream | `DynObj_LocalTexturesSTex` | `0x4CF790` | *(catalog)* |
| Resource container | `Sys_Resource`, `Resource_Load` | `0x4B5F80`, `0x4B5F00` | `Formats/Legacy/FlatResourceArchive.cs` |
| Terrain config | `DynObj_ParseConfigFile_7` / `_10` | `0x4C5F00` / `0x4CB810` | `Formats/World/TerrainMaterial.cs` |
| Terrain tile map | `CreateTerrainTileMap` | `0x4C6760` | *(catalog)* |
| AI graph open | `Graph_Open`, `Graph_Init`, `AIGraph_Action` | `0x4F9FF0`, `0x4FAA60`, `0x509480` | `Formats/Ai/GraphFile.cs` |
| Animation play | `Bones_PlayAnimation` | `0x4D61D0` | `Formats/Animation/BoneObject.cs` |

## 9. Assembly-level confirmation (FourCC / constant matching)

Because neither project is compiled from the retail code, a literal *assembly diff* is not possible. The strongest
achievable comparison is **constant-level**: the retail native's x86 disassembly contains immediates, and the
format-level ones are **FourCC chunk tags** (a dword that spells 4 ASCII bytes). Matching those against the tags the
implementations name proves they parse **the same file format** — and, combined with an address citation, identifies
**the same method**.

Method: read the retail native's disassembly (`bn_function_disassembly`), decode every immediate ≥ `0x1000` as a
4-byte ASCII tag (both byte orders), then intersect with the quoted 4-char tags found in each project's sources.

| Retail native | tags in the retail code | `open-igi` | `editor` | `converter` |
|---|---|---|---|---|
| MEF model `0x4CF390` | 13 (`MESH`, `ATTA`, `MVTX`, `PORT`, `PTFC`, `PTVX`, `TXAN`, …) | **8** | 6 | 6 |
| TEX stream `0x4CF790` | 2 (`TEXF`) | **1** | 1 | 1 |
| RES container `0x4B5F80` | 4 (`ILFF`, `IRES`) | **2** | 2 | 2 |
| MTP hash `0x4CF080` | 6 (`INST`, `VNAM`, `GTT`) | **2** | 2 | 2 |
| IFF / bone `0x4D67A0` | 30 (`BOHD`, `BALH`, `PLST`, `TLST`, …) | **15** | 0 | **15** |
| sprite / QVM / terrain / DAT | 0 tag immediates | — | — | — |

**How to read this**

- **Tag matches are positive proof of format identity.** e.g. `TEXF` appears both as an immediate in `0x4CF790`
  and in all three implementations → they read the IGI1 `.tex` container the same way.
- **`15/30` on the bone/IFF native** is the standout: `open-igi` and the converter independently name the same 15
  chunk tags the retail function compares against, the editor effectively none.
- **Zero tags ≠ mismatch.** Functions that do arithmetic/pointer work (QVM load, terrain config, DAT) carry no tag
  immediates, so this test simply cannot confirm them — they need control-flow/offset analysis instead.
- **Memory offsets do NOT match, by design.** e.g. `0x4CF390` uses struct offsets `+0x88/+0x8C/+0x90/+0x94/+0x98`,
  while `open-igi` uses its own record layout (`0x78`, `0xA0`). Reimplementations do not preserve retail memory
  layout, so offset matching is not a valid test for them. That is expected, not a defect.

## 10. `project-igi-converter` profile

`project-igi-converter` (503 source files, C++/Qt) is the origin of the `igi1conv.exe` the editor bundles.

- **Parsers:** `source/parsers/` — `mef_parser`, `tex_parser`, `mtp_parser`, `res_parser`, `iff_parser`,
  `qvm_parser`, `qsc_parser`, `graph_parser`, `fnt_parser`, `dat_parser`, `terrain_files`.
- **Retail addresses cited in code:** **0** — same as the editor. Its knowledge is in the format code, not in
  native references. It *does* carry `source/parsers/mef_native.{h,cpp}` and `hardcoded_bones.h`, i.e. the same
  structural knowledge as the editor, from the same upstream (`hjcminus/Project-IGI-Terrain`).
- **Format-level fidelity:** **the highest of the three** — it matches 15/30 bone/IFF tags (tied with `open-igi`)
  and covers the widest format set (IFF/BEF, FNT, DAT, graph, terrain LMP/CTR), because it is a purpose-built
  converter with round-trip tests.
- **Licence:** Heaven-HM source-available (noncommercial) — `LICENSE_SCOPE.md`.
- **Caveat:** it targets IGI1 **and** IGI2 from shared parsers, so its IGI1 path must be checked against IGI1
  behaviour specifically; and like the editor it gives no retail method names.

**Practical use:** treat the converter as the **best available format specification** (what the bytes mean) and
`open-igi` as the **best available method index** (which retail address does the work). Neither replaces `igi.exe`.

## 11. Independent renderer/placement checks — 2026-10-17

The listed-address audit examined the 296 distinct addresses actually written in this document, not the
977-address source-citation census. Live Binary Ninja reported 5,732 analyzed function starts. Of these 296
references, 97 match those starts; 86 non-function references are in `.data`, five in `.rdata`, 106 in `.text`,
and two are outside the section ranges. This verifies address classification only. The source-citation counts
and inferred names above have not been independently re-established by this audit.

The model demo uses these independently checked slices:

- Runtime reading of the retail operation table: `AllocTexture` points to `0x0050A940` and
  `DownloadTexture` to `0x0050AD90`. The existing physics labels at these addresses are misleading.
- `0x0049B4B0` copies 32-byte format-0 and 40-byte format-3 vertices. The executable's FVF constants at
  `0x00543CB8` are `0x112`, `0x212`, `0x112`; the earlier fixed 36-byte claim is incorrect.
- Draw dispatcher `0x0049DF60` uses 28-byte format-0 and 32-byte format-3 batch headers. Lightmapped
  batches read first vertex at `+0x14` and vertex count at `+0x16`; format-0 uses `+0x12` and `+0x14`.
- Texture loader `0x004B24C0`, uploader `0x0050AD90`, and converters `0x00517C90`, `0x00517DD0`,
  `0x00517F00` establish the base-level pixel conversion used by this demo. Palette and mip upload are
  not completed by this slice.
- `DynObj_Missing` at `0x004CEC10` returns null for an empty unresolved name and tries the literal
  `missing` for a nonempty unresolved name. The demo logs unresolved sources without inventing geometry.
- All 14 level object scripts pass the limited constant/call reader. This reader is deliberately not
  labeled a complete `Qvm_Execute` implementation.
- Asset checks passed for 6,069 static meshes and 6,668 model textures across 14 level archives.
  Another 536 skinned mesh entries are explicitly excluded from the static decoder.
- The first 406 Level 1 static model poses matched the retail LOD-model registry names and position
  bytes in running-process memory. Maximum orientation difference was `5.96046448e-8`; this is a
  numerical-tolerance check, not bitwise floating-point equivalence or a callback execution trace.

Raw audit, assembly, memory results and screenshots are retained locally under
`/Users/haseeb-mir/Documents/Code/project-igi-natives/scratch/open-igi-render-stage/`.
The demo is still incomplete: remaining task models, skinning, recursive attachments and retail
lighting/lightmap presentation must be verified separately before full visual parity can be claimed.

## 12. Engine loader inventory (`project-open-igi/engine/`)

Read-only audit of the C++ port's own loaders (17 `.cpp`, ~3,365 lines). This is where the *format* work for the
port actually lives — it is not a copy of either reference project.

| Loader | File | Retail native | Loads correctly? |
|---|---|---|---|
| RES / IRES archive | `resource/resource_archive.cpp` | `Sys_Resource 0x4B5F80`, `Resource_Load 0x4B5F00` | **Yes** — ILFF→IRES form check, NAME/BODY sibling walk, plus legacy `"RESOURCE"` branch; bounds-checked |
| Resource cache / VFS | `resource/resource_cache.cpp` | — | Present (host-side handle cache) |
| MEF container + chunks | `model/mef_model_loader.cpp` | `DynObj_LocalModelsSMef 0x4CF390` | **Yes** — 20-byte container header, chunk alignment, sibling offsets, nested ILFF, `OCEM` form, MESH version `0x3DCCCCCD` (0.1) |
| MEF → draw data | `model/mef_render_data.cpp` | `DisplayRender_Processmodelsnotconvert 0x49DF60`, upload `0x49B4B0` | **Yes** — 32/40-byte vertices, 28/32-byte batch headers, linked index lists |
| MTP metadata | `model/model_texture_package.cpp` | `DynObj_InsertHashItem_14 0x4CF080` | **Yes** — MODS / VNAM / INST / GTT + TEXF |
| Model `.tex` pixels | `texture/model_texture_pixels.cpp` | `Texture_LoadModelFile 0x4B24C0` | Present; RGBA into D3D surfaces |
| Terrain LOOP `.tex` | `texture/loop_terrain_texture.cpp` | `Terrain_TextureFileNotOfRequ 0x5175A0` | **Yes** — LOOP signature, versions 7 and 9 only, palette + record strides |
| QVM | `script/qvm_loader.cpp`, `qvm_compiled_program.cpp`, `qvm_object_calls.cpp` | `Qvm_Load 0x4B80B0` | **Partial** — validates the compiled program and reads `Task_New` object calls; does **not** execute the VM |
| Audio (5 files) | `audio/*` | ILSF / ADPCM / attenuation | Present, but **out of MVP scope** |

**Wired into the app:** `platform/windows/retail_model_scene.cpp` loads
`location0/common/level1.{mtp,models/models.res,textures/textures.res}` + `objects.qsc`, resolves each `Task_New`
(`EditRigidObj`, `Building`, `ExplodeObject`, `Terminal`, `SCameraControl`, `AlarmControl`, `Door`), applies
position + Euler orientation (world scale `1/4096`), and draws through the Direct3D 7 HAL path.
**Models, textures and object placement do load and draw.**

### 12.1 Not present at all

| Missing | Consequence for the build order |
|---|---|
| **Terrain geometry** — `.HMP` / `.CTR` / `.BIT` / `.LMP` | Only the LOOP `.tex` decoder exists; there is **no terrain loader, mesh or draw** → step 3 (terrain) has nothing to build on yet |
| AI graph (`.dat`) | step 9 blocked |
| DAT index | — |
| FNT fonts | — |
| IFF / BEF animation clips | only a bone-record struct; skinned models are explicitly "not yet rendered" → step 12 blocked |
| SPR sprites | — |
| Physics / player / AI / weapons / health / interaction | steps 7–12 blocked |

### 12.2 Test-coverage caveat

All 13 test targets are **synthetic** — 0 reference real retail assets (`IGI_GAME_PATH` / `missions/location` → 0
hits; they construct bytes in memory). "Tests pass" therefore proves the *parser logic*, **not** that a retail
Level 1 file loads. Every loader header also carries **"runtime parity unverified"**; none has been diffed against
a live `igi.exe` frame.

## 13. Vertex-stride correction (closes the §17 open item)

The engine's `mef_render_data.cpp` uses **32-byte format-0** and **40-byte format-3** vertex records
(position +0, normal +0x0C, UV +0x18, lightmap UV +0x20). Checked against the binary:

- `0x0049FB10` contains `add edi, 0x20` in the format-0 loop → **32-byte stride supported**.
- The other branch stores `0x28` (40) into a descriptor field → **40-byte stride supported**.

So the earlier note in `NATIVES_DATABASE.md` §17 ("the only observed stride is `0x14`/20; stride unverified") was
**too pessimistic for the vertex stream**: the `0x14` (20) record belongs to the `TXAN` record array inside
`0x004CF390`, which is a different structure. Status is upgraded to **"supported: 32 / 40, loop not yet fully
traced"**. `NATIVES_DATABASE.md` §17 has been updated to match.

## 14. Caveats

- **Names in `igi.exe` are not symbols** (`igi.pdb` does not exist). Every name here is an inferred label; the
  **address** is the only hard fact. Where the label is uncertain it is marked.
- Some cited names are BN/decompiler auto-names with numeric suffixes (e.g. `Sys_FreeMemory_8`) whose bodies do
  something else entirely — confirm the body before implementing.
- This matching is a **map**, not proof. A format reader is only "ported 1:1" when the ported method's body is
  reproduced from `igi.exe`, not when this table is followed.
