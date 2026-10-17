# Task 1 — early textured model loading

Status: an early Windows x86 / Direct3D 7 scene works. This is not a complete mission or complete visual parity.

## Run

Place `open-igi.exe` alongside the original game assets, then run:

```console
open-igi.exe window level1
open-igi.exe window level14
```

`level1` through `level14` select the corresponding `OBJECTS.QVM`, MTP package and paired model/texture RES archives.
`window` without a level selects Level 1. The executable resolves assets relative to its own directory.

For a bounded render test, `--capture-frame <output.bmp>` saves the Direct3D render target and exits.
This is a diagnostic capture option. The scene camera and lighting are diagnostic, not verified retail presentation.

## Research anchors used

| Address | Native/catalog or runtime-operation name | Implemented slice |
|---|---|---|
| `0x004B5F00` | `Resource_Load` | Cached resource bytes; optional output is a byte count |
| `0x004B5F80` | `Sys_Resource` | ILFF/IRES archive entries |
| `0x004B80B0` | `Qvm_Load` | Select compiled sibling for a QSC request |
| `0x004BD220` | `Qvm_LoadCompiledProgram` | Compiled program header/tables/code |
| `0x004BD430`, `0x004BD880` | QVM opcode registration / CALL handler | Limited constant-and-call object-data reader |
| `0x004CF080` | `DynObj_InsertHashItem_14` | MODS/VNAM/INST/GTT/TEXF package links |
| `0x004CF390` | `DynObj_LocalModelsSMef` | Mesh metadata and ILFF chunk discovery |
| `0x0049FB10` | `Display_ClearSurfaces_20` (provisional catalog name) | Format-specific renderer chunk association |
| `0x0049B4B0` | `Display_MeshFormatNotSupport` (provisional catalog name) | 32/40-byte static vertex uploads |
| `0x0049DF60` | `DisplayRender_Processmodelsnotconvert` | Primitive type and linked draw records |
| `0x004B24C0` | `Sys_SIsNotAValidTextureF`; port helper `Texture_LoadModelFile` | LOOP model texture versions 2 and 11 |
| `0x0050A940`, `0x0050AD90` | Runtime operations `AllocTexture`, `DownloadTexture` | Texture operation identity; base pixel/upload path |
| `0x00517C90`, `0x00517DD0`, `0x00517F00` | Catalog terrain aliases; pixel converters | ARGB1555, RGB565 and 32-bit pixel conversion |
| `0x004EFBA0`, `0x004B38E0` | Real32x9 parameter reader / rotation construction | Three-angle object orientation |
| `0x004950F0` | `Display_SetGammaRamp_7` (provisional catalog name) | Retail matrix to Direct3D matrix layout |
| `0x004D9B10` | `MagicObj_LoadParameters` (research label) | Object transform offsets for runtime comparison |
| `0x004CEC10` | `DynObj_Missing` | Empty-name/null and nonempty-name fallback branch |

Names are research labels unless explicitly identified as runtime operation names. Address existence does not
prove a complete function port. `FORMAT_NATIVE_MAPPING.md` is used as an index, with its independent audit
recorded in Section 10; CLEAN decompiler output is checked against retail instructions and runtime observations.

## Tests and runtime evidence

- 12 CTest tests passed on a clean snapshot of the committed code. New checks cover valid mesh draws,
  out-of-range indices, truncated vertex/index streams, QVM stack underflow, ARGB1555 conversion and short pixels.
- All 14 OBJECTS.QVM files passed the limited object-data reader.
- 6,069 static mesh entries passed vertex/index bounds checks; 536 skinned entries are explicitly excluded.
- 6,668 model textures passed base-level pixel decoding across the 14 level texture archives.
- All 14 scene runs created a Direct3D 7 frame and exited successfully; actual frame captures are linked below.
- Level 1: 406/406 static model identities matched the running retail LOD registry, with byte-equal positions and
  maximum matrix difference `5.96046448e-8`. An expanded 441-pose check matched 436; five destructible barrels
  were unmatched in the observed running state. Their initial/runtime-state relationship remains unverified.

The current draw slice covers initial poses of `Building`, `EditRigidObj`, `Door`, `ExplodeObject`, `Terminal`,
`SCameraControl` and `AlarmControl`. It does not execute their gameplay behavior.

| Level | Rendered model poses | Uploaded textures | Source warnings | Capture |
|---|---:|---:|---:|---|
| 1 | 441 | 190 | 0 | [Frame](screenshots/task1/level1-models.png) |
| 2 | 228 | 193 | 6 | [Frame](screenshots/task1/level2-models.png) |
| 3 | 535 | 266 | 0 | [Frame](screenshots/task1/level3-models.png) |
| 4 | 123 | 160 | 0 | [Frame](screenshots/task1/level4-models.png) |
| 5 | 372 | 200 | 3 | [Frame](screenshots/task1/level5-models.png) |
| 6 | 515 | 285 | 8 | [Frame](screenshots/task1/level6-models.png) |
| 7 | 732 | 243 | 5 | [Frame](screenshots/task1/level7-models.png) |
| 8 | 510 | 319 | 5 | [Frame](screenshots/task1/level8-models.png) |
| 9 | 457 | 226 | 2 | [Frame](screenshots/task1/level9-models.png) |
| 10 | 405 | 173 | 2 | [Frame](screenshots/task1/level10-models.png) |
| 11 | 419 | 286 | 3 | [Frame](screenshots/task1/level11-models.png) |
| 12 | 452 | 253 | 0 | [Frame](screenshots/task1/level12-models.png) |
| 13 | 914 | 282 | 0 | [Frame](screenshots/task1/level13-models.png) |
| 14 | 848 | 166 | 5 | [Frame](screenshots/task1/level14-models.png) |

Across these runs, 35 warnings concern empty source model names. Four concern unresolved nonempty names:
three `waypoint` references and one `s332_02_1`. They are logged and omitted when no retail fallback resolves;
no replacement geometry is invented. A successful scene run does not mean every source model was resolved.

## Remaining Task 1 work

Skinned actors, remaining task types, recursive attachments, complete QVM execution, lightmap/retail lighting,
full mip-chain handling and the unresolved references above still require verification. No claim is made that
every model is visible from the diagnostic camera or that the screenshots match the retail framebuffer.
Terrain is not included in these captures and has not been advanced ahead of the model work.

Raw logs, executable/asset hashes, the mapping audit, read-only retail memory checks and BMP/PNG captures remain
in the local ignored `project-igi-natives/scratch/open-igi-render-stage/` evidence directory.
