> **SQLite:** `build/native-auto/natives.db` is the automatically refreshed evidence ledger.
> Read-only table: `python3 scripts/native_database_read.py --verified`.
> Full `.db` → Markdown export: `python3 scripts/native_database_read.py > build/native-auto/export.md`.
> Table exports never approve natives; exact tested hashes and strict verification remain required.

> **Live verification database:** `docs/fidelity/native-verification.json` is the machine catalog.
> Run `python3 scripts/native_database.py` to regenerate the complete Markdown ledger at
> `build/native-auto/database.md` and JSON at `build/native-auto/database.json`.
> Commit verification and automatic scans regenerate it too. Historical claims below are
> reference notes, not acceptance. CFG >= 0.85 AND source-semantic/angr evidence AND
> native-specific paired Frida comparisons are required. Evidence for an older executable
> never approves the currently built `igi1.exe`. See [automatic workflow](fidelity/native-feature-verification.md).

> **2026-10-17 terrain correction:** The old 512x512/int16 HMP description below is superseded.
> Retail 0x004C6240 walks 256 12-byte records and optional `(dimension + 1)^2` byte HMP blocks.
> 0x00510010 reads signed-byte heights and applies bias 64 / scale 256. CTR relocation uses 32-byte
> records and +0x1C CMD offsets; that does not validate the previously guessed quadtree struct.
> 0x00512040 is a vertex/interpolant clipping path, not an established HMP sampler.
> See [current evidence and all-level audit](docs/RETAIL_TERRAIN_ATTACHMENT_STATUS.md).

# Project I.G.I. (2000) Game Natives Reference & File Formats Architecture Guide

> **Primary Code Location**: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/`  
> **Reference Binary**: Retail `IGI.EXE` (PE32 Intel 80386, Base `0x00400000`, 1,384,448 bytes)  
> **Purpose**: Exhaustive technical directory of all game file formats, engine subsystems, exact clean source paths, function signatures, argument definitions, and execution sequences for engine integration.  
> **Mandatory Rule**: No sample code blocks. This document provides precise file locations, technical format specifications, binary layouts, and calling interfaces.

---

## 0. Verification Status (audited against `igi.exe`)

Every address below was re-checked with Binary Ninja MCP against the retail binary. Results of the last audit:

| Check | Count | Result |
|---|---|---|
| Cited clean-file paths existing on disk | 170 | **148 exist**, 22 were broken |
| Cited `function @ address` name matches in `igi.exe` | 57 | **49 match**, 1 wrong address, 7 are not functions |

**Corrections applied (previously wrong):**

| Claimed | Was | Correct retail native |
|---|---|---|
| Player ground check | `HumanPlayer_CheckGround @ 0x0045E120` | `HumanPlayer_CheckGround @ 0x00408330` |
| Player motion | `HumanPlayer_UpdateMotion @ 0x0045DCB0` | `HumanPlayer_UpdateMotion @ 0x00408320` |
| Player attributes | `HumanPlayer_LoadParameters_2 @ 0x0045BF00` | `HumanPlayer_LoadParameters_2 @ 0x004082B0` |
| Fall damage | `Human_FallLanding_0045c710 @ 0x0045C710` | `FallLanding @ 0x00408460` (subsystem `misc`) |
| Human damage | `Human_Damage_0045d650 @ 0x0045D650` | `Human_DamageApply @ 0x00412440` + `Human_DamageHandler @ 0x0045D3C0` |

**Removed — these names exist nowhere in the verified catalog (5,732 natives) and their addresses are mid-function x87 code, not function starts:**
`HumanAI_SensoryCones`, `HumanAI_CheckAlertState`, `HumanAI_UpdatePatrol`, `CombatState_TargetPrioritization`, `Human_FallLanding_0045c710`.
Section 5 has been rewritten with the real `HumanAI_*` / `AI_*` / `CombatState_*` natives.

> **Rule going forward:** a function may only be listed here if `bn_function_info <address>` returns that exact name from `igi.exe`. Anything else is marked `UNVERIFIED` and excluded.

### 0.1 Downgraded after second review (claims that do not hold)

| Claim in this document | Evidence against | New status |
|---|---|---|
| `XTRV` = fixed **32-byte** FVF stride | The listed fields sum to 36 bytes; the only stride observed in the decompiled `DynObj_LocalModelsSMef` is **`0x14` (20 bytes)**, and it indexes a *record array*, not a vertex stream | **UNVERIFIED** |
| `Terrain_RenderQuadtree` = the quadtree renderer | Catalog entry `0x00510010` is **gate FAIL, score 0.667, `L1_cfg` false, confidence 4/6**; REA has no symbol for it. The name is not established by the address | **NAME UNVERIFIED** |
| `.CTR` = 32-byte quadtree node descriptor | Not produced by any decompiled function; `DynObj_Createterrainmateria` only maps the file into the terrain context | **UNVERIFIED** |
| `Display_InitDirectDraw` @ `0x004924D0` | Gate PASS 1.0 (`discovered-bn-r2`), but **REA/Ghidra has no procedure** at that address and the binary has no symbols, so the *name* is inferred, not proven | **NAME INFERRED** |
| `Display_OpenDevice` @ `0x00490190` | Gate PASS 1.0 (`confirmed-rea`); address is a real function, but again the name is inferred (no symbols in `igi.exe`) | **NAME INFERRED** |

**Naming caveat for the whole document:** `igi.pdb` does not exist, so **no name in this file is a symbol from the retail binary** — every name is either BN/r2 CFG-confirmed *behaviour* plus an inferred label, or an open-igi cross-citation. Addresses and bodies are verifiable; names are working labels.

---

## Master Subsystem Index

1. [3D Models & Hierarchical Geometry (.mef, .res, .mtp)](#1-3d-models--hierarchical-geometry-mef-res-mtp)
2. [Level Terrain & TerraForm Quadtree (.hmp, .ctr, .lmp, .bit, .tex, .qsc)](#2-level-terrain--terraform-quadtree-hmp-ctr-lmp-bit-tex-qsc)
3. [Player Locomotion, Stance & Movement Physics](#3-player-locomotion-stance--movement-physics)
4. [Rigid Body Dynamics & Mesh Collision Physics](#4-rigid-body-dynamics--mesh-collision-physics)
5. [AI Perception, Vision Cones, Hearing & Combat State Machine](#5-ai-perception-vision-cones-hearing--combat-state-machine)
6. [Skeletal Animation, Bone Hierarchy, Quaternions & Splines (.iff, .bef)](#6-skeletal-animation-bone-hierarchy-quaternions--splines-iff-bef)
7. [Visual Effects, Smoke, Particles, Tracers & Weather Dynamics](#7-visual-effects-smoke-particles-tracers--weather-dynamics)
8. [Vehicles, Flight Dynamics & Transports (Planes, Helicopters, APCs, Tanks, Trains, Wheels)](#8-vehicles-flight-dynamics--transports-planes-helicopters-apcs-tanks-trains-wheels)
9. [Audio Engine, DirectSound, 3D Spatializer & Positional Audio (.wav)](#9-audio-engine-directsound-3d-spatializer--positional-audio-wav)
10. [Cutscenes, Cinematics & FMV Movie Streaming (.bik, .avi)](#10-cutscenes-cinematics--fmv-movie-streaming-bik-avi)
11. [Subtitles, Typography, Glyphs & On-Screen Display (.fnt)](#11-subtitles-typography-glyphs--on-screen-display-fnt)
12. [Interactive World Entities & Triggers (Doors, Cabinets, Keypads, Switches, Terminals, Alarms)](#12-interactive-world-entities--triggers-doors-cabinets-keypads-switches-terminals-alarms)
13. [Weapons, Ballistics, Recoil Systems & Fire Timers](#13-weapons-ballistics-recoil-systems--fire-timers)
14. [Mission Scripts, Virtual Machine & Cooperative Task Engine (.qsc, .qvm, QTask)](#14-mission-scripts-virtual-machine--cooperative-task-engine-qsc-qvm-qtask)
15. [Level Streaming, Sublevels & State Persistence (.res, sublevel tasks)](#15-level-streaming-sublevels--state-persistence-res-sublevel-tasks)
16. [Master Function-to-Clean-Path Cross Reference Table](#16-master-function-to-clean-path-cross-reference-table)
17. [MEF Mesh Geometry, Vertex Stream & Index Buffer Decoding (XTRV, DNER)](#17-mef-mesh-geometry-vertex-stream--index-buffer-decoding-xtrv-dner)
18. [Retail .TEX Pixel & Mipmap Decoder & Direct3D 7 Texture Binding](#18-retail-tex-pixel--mipmap-decoder--direct3d-7-texture-binding)
19. [Level 1 Terrain Elevation, Quadtree & Lightmap Pipeline (.HMP, .CTR, .BIT, .LMP)](#19-level-1-terrain-elevation-quadtree--lightmap-pipeline-hmp-ctr-bit-lmp)
20. [2D/3D Billboard Sprite Pipeline & Draw Path (.SPR)](#20-2d3d-billboard-sprite-pipeline--draw-path-spr)
21. [Direct3D 7 Device Setup, SwapChain & 3D Render Loop](#21-direct3d-7-device-setup-swapchain--3d-render-loop)
22. [Missing Pipeline Stages: Asset Decode, Texture Bind, Sprite Draw, Container IO](#22-missing-pipeline-stages-asset-decode-texture-bind-sprite-draw-container-io)
23. [Verified Native Inventory — MVP Subsystems](#23-verified-native-inventory--mvp-subsystems)
24. [MVP Build Order — Step-to-Native Mapping and Verification](#24-mvp-build-order--step-to-native-mapping-and-verification)

---

## 1. 3D Models & Hierarchical Geometry (.mef, .res, .mtp)

### Formats & Binary Specifications
- **`.mef` (Model Engine File)**: Packed inside an ILFF container with format signature `HSEM` (`0x4D455348` backward). Contains polygonal meshes, vertex attributes, bone linkage matrices, attachment locators, and portal visibility hulls.
- **`.res` (Virtual Resource Container)**: High-speed packed asset archive with FourCC header `ILFF` / `IRES`. Used to package level models (`models/*.res`), textures (`textures/*.res`), and sounds (`sound/*.res`) into single contiguous files.
- **`.mtp` (Model-Texture Package Table)**: Relational mapping table linking object instances to material packages. Links `models/%s.res` with `textures/%s.res` and parses:
  - `VNAM` (`0x564E414D`): Vertex and joint name string table.
  - `INST` (`0x494E5354`): Object placement instance transformation matrices.
  - `GTT ` (`0x47545420`): Global Texture Table binding material indices to texture stages.
- **Key MEF FourCC Sub-Chunks**:
  - `MESH` (`0x4D455348`): Main mesh header. Verifies engine compatibility version float `0x3DCCCCCD` (`0.1f`).
  - `ATTA` (`0x41545441`): Attachment locators. Each entry is 68 bytes (3x4 transformation matrix + 32-bit child model identifier).
  - `XTRV` (`0x58545256`): Interleaved Direct3D vertex streams (Position `float3`, Normal `float3`, Diffuse `uint32`, UV Coordinates `float2`).
  - `DNER` (`0x444E4552`): Primitive render list (Triangle strips and indexed triangle lists).
  - `MVTX` (`0x4D565458`): Marker vertices for weapon muzzle flashes, shell ejection ports, and engine thrusters.
  - `PORT` (`0x504F5254`), `PTFC` (`0x50544643`), `PTVX` (`0x50545658`): Indoor portal convex cells, portal polygon faces, and boundary vertices for occlusion culling.

### Clean Native Locations
- **MEF Model Geometry Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.h`
- **MTP Package Binder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF080-DynObj_InsertHashItem_14.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF080-DynObj_InsertHashItem_14.h`
- **Model Texture Stream Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.h`
- **Resource Stream Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B5F00-Resource_Load.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B5F00-Resource_Load.h`
- **ILFF Container Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sys/004B5F80-Sys_Resource.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sys/004B5F80-Sys_Resource.h`

### Function Signatures & Calling Interface
- **`DynObj_InsertHashItem_14` (`0x004CF080`)**:
  - Signature: `int32_t* DynObj_InsertHashItem_14(void* dynObjContext, int32_t packagePathString, int32_t flags);`
  - Parameters:
    - `dynObjContext`: Pointer to a 0x60-byte dynamic entity container.
    - `packagePathString`: Pointer to C-string representing package path (e.g., `"LOCAL:models/weapons/ak47"`).
    - `flags`: Mode bitmask (`1` = concurrently load paired texture archive).
  - Return: Pointer to initialized dynamic object hash node.
- **`DynObj_LocalModelsSMef` (`0x004CF390`)**:
  - Signature: `int32_t* DynObj_LocalModelsSMef(int32_t dynObjHandle);`
  - Parameters:
    - `dynObjHandle`: Integer handle / pointer to the entity container initialized above.
  - Return: Pointer to parsed model descriptor containing vertex buffers and bone nodes.

### Execution Sequence & How to Use
1. **Allocate Entity Context**: Allocate a 0x60-byte structure aligned to 4 bytes for the dynamic entity.
2. **Mount Model-Texture Package**: Call `DynObj_InsertHashItem_14` passing the entity buffer and virtual package path. The engine reads `models/%s.res` and `textures/%s.res`, resolves `INST` matrices, and maps texture IDs via `GTT `.
3. **Parse Mesh Geometry**: Call `DynObj_LocalModelsSMef` passing the entity handle. Decompiled behaviour of `0x004CF390`: it loads `LOCAL:models/%s.mef` via `Resource_Load`, reads the `HSEM` chunk (`0x4D455348`) and checks the version dword `0x3DCCCCCD` (`0.1f`); then it reads the `ATTA` (`0x41545441`), `MVTX` (`0x4D565458`), `PORT` (`0x504F5254`), `PTFC` (`0x50544643`) and `PTVX` (`0x50545658`) chunks into the model struct at `+0x88/+0x8C/+0x90/+0x94/+0x98`, walks a `TXAN` (`0x4E415854`) list, allocates `count * 0x14` bytes and fills 20-byte records (`+0x00`, `+0x04 = 0`, `+0x08`, `+0x10`), then dispatches through the table at `g_a94E84`. On a version mismatch it calls `Error_Show("Mesh … incompatible …")`. **It does not decode the vertex/index stream** — the earlier text claiming it "extracts the `XTRV` vertex array" and "builds `DNER` draw blocks" was wrong and has been corrected.
4. **Bind Texture Surfaces**: Call `DynObj_LocalTexturesSTex` to upload decoded texture mipmaps into Direct3D texture stage registers.

---

## 2. Level Terrain & TerraForm Quadtree (.hmp, .ctr, .lmp, .bit, .tex, .qsc)

### Formats & Binary Specifications
- **`.hmp` (Heightmap Grid)**: Optional table of byte-sample grids; loader record/size behavior is verified at 0x004C6240, and signed sample consumption at 0x00510010. Bindings determine origin/scale.
- **`.ctr` (Contour Mesh Index)**: Hierarchical quadtree contour representation defining terrain LOD transitions and patch edges.
- **`.lmp` (Lightmap Surface)**: Pre-baked static sunlight angles, ambient occlusion, and building ground contact shadows.
- **`.bit` (Occlusion Bitmask)**: Horizon and ridge visibility bitmask used for early CPU/GPU triangle occlusion culling.
- **`.tex` (Multi-Tile Loop Texture Archive)**: LoopEngine texture archive holding tiled material mipmaps (grass, rock, snow, mud, sand). Magic: `"LOOP"` (versions 7 and 9).
- **`.qsc` / `.qvm` (TerraForm Script)**: Level bytecode script initiating `CreateTerrainMaterial` and `CreateTerrainTileMap`.

### Clean Native Locations
- **Heightmap, Contour & Material Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C6240-DynObj_Createterrainmateria.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C6240-DynObj_Createterrainmateria.h`
- **Terrain Texture & Lightmap Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C5F00-DynObj_ParseConfigFile_7.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C5F00-DynObj_ParseConfigFile_7.h`
- **LoopEngine .TEX Tile Decoder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.h`
- **Quadtree Terrain Renderer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00510010-Terrain_RenderQuadtree.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00510010-Terrain_RenderQuadtree.h`
- **Heightmap Elevation Sampler**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512040-Terrain_SampleHeightmap.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512040-Terrain_SampleHeightmap.h`
- **Surface Normal Calculator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005122B0-Terrain_CalculateNormals.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005122B0-Terrain_CalculateNormals.h`
- **Water Surface Simulator & Wave Generator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005116B0-Terrain_RenderWaterSurface.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00511500-Terrain_UpdateWaterWaves.cpp`

### Function Signatures & Calling Interface
- **`DynObj_ParseConfigFile_7` (`0x004C5F00`)**:
  - Signature: `int32_t DynObj_ParseConfigFile_7(void* terrainContext, int32_t basePathString);`
  - Loads `<basePath>.tex` and `<basePath>.lmp`, extracts texture blocks into `terrainContext + 0xDBC`, and initializes the 256-color palette ramp.
- **`DynObj_Createterrainmateria` (`0x004C6240`)**:
  - Signature: `int32_t DynObj_Createterrainmateria(void* terrainContext, int32_t basePathString);`
  - Appends `.bit` (offset `+0xDCC`), `.hmp` (offset `+0xDD0`), `.cmd` (offset `+0x08`), `.ctr` (offset `+0x0C`), and calls `Qvm_Load` / `Qvm_Execute` on the terrain script.
- **`0x00510010`** (working name `Terrain_RenderQuadtree`, **unverified** — see §19):
  - Signature: `int32_t (void* terrainContext);`
  - *(Unverified)* claimed to evaluate camera-distance LOD and dispatch terrain draws. See §19 — the name is a working label and the algorithm is not established.

### Execution Sequence & How to Use
1. **Initialize Terrain Task**: Invoke `DiscardTerrain_Init` (`0x0052C0C0`) to register task descriptor `0x158` with the QTask engine.
2. **Mount Textures & Lightmaps**: Call `DynObj_ParseConfigFile_7` passing the terrain base path (e.g., `"LOCAL:levels/level01/terrain"`).
3. **Parse Elevation & Bitmasks**: Call `DynObj_Createterrainmateria` with the same base path. The engine maps the 16-bit `.hmp` elevation grid and quadtree contour indices.
4. **Bind World Streaming**: Call `DiscardTerrain_LoadParameters` to register spatial chunks into 4,096.0-unit world boundary buckets.
5. **Per-Frame Render**: Execute `Terrain_RenderQuadtree` followed by `Terrain_RenderWaterSurface`.

---

## 3. Player Locomotion, Stance & Movement Physics

### Formats & Discrete States
- **Stance Enumeration**:
  - `0`: Standing (Full sprint allowed, full height collision cylinder).
  - `1`: Crouching (Reduced speed factor `0.45f`, half collision cylinder).
  - `2`: Prone (Reduced speed factor `0.20f`, low-profile collision slab).
  - `3`: Ladder Climbing (Velocity constrained along vertical ladder spline).
  - `4`: Swimming / Water Traversal (Buoyancy physics applied).
- **Physical Quantities**: Horizontal velocity vector, vertical fall acceleration (`-9.81 m/s²`), stamina depletion (`0.0f` to `100.0f`), fall landing threshold (`-18.0 m/s`).

### Clean Native Locations
- **Ground Contact Raycaster**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408330-HumanPlayer_CheckGround.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408330-HumanPlayer_CheckGround.h`
- **Motion Physics & Input Integrator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408320-HumanPlayer_UpdateMotion.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408320-HumanPlayer_UpdateMotion.h`
- **Fall Landing Impact Evaluator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/misc/00408460-FallLanding.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/misc/00408460-FallLanding.h`
- **Health & Damage Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/human/00412440-Human_DamageApply.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/human/0045D3C0-Human_DamageHandler.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/human/0045D3C0-Human_DamageHandler.h`
- **Stamina & Player Attributes**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/004082B0-HumanPlayer_LoadParameters_2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/004082B0-HumanPlayer_LoadParameters_2.h`
- **Player Per-Frame Clocks (stamina, health, animation)**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408340-HumanPlayer_UpdateStamina.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408390-HumanPlayer_UpdateHealth.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408AA0-HumanPlayer_UpdateAnimation.cpp`

### Function Signatures & Calling Interface
- **`HumanPlayer_CheckGround` (`0x00408330`)**:
  - Signature: `int32_t HumanPlayer_CheckGround(void);` (retail proto: `int32_t()`)
  - Performs ray-cylinder cast against terrain elevation and floor collision hulls. Returns `1` if grounded, `0` if airborne.
- **`HumanPlayer_UpdateMotion` (`0x00408320`)**:
  - Signature: `int32_t HumanPlayer_UpdateMotion(void);` (retail proto: `int32_t()`)
  - Integrates user keyboard/mouse movement, modifies acceleration based on stance speed multipliers, and decrements stamina if sprinting.
- **`FallLanding` (`0x00408460`)**:
  - Signature: `int32_t FallLanding(float impactVelocity);`
  - Evaluates impact velocity on ground touchdown. If `impactVelocity < -18.0f`, applies the landing damage through `Human_DamageApply`.
- **`Human_DamageApply` (`0x00412440`)**:
  - Signature: `float Human_DamageApply(float damageAmount, int32_t* victimEntity); __location("eax")`
  - Applies a validated damage amount to the target body and returns the remaining health.
- **`Human_DamageHandler` (`0x0045D3C0`)**:
  - Signature: `int32_t Human_DamageHandler(void* victimEntity, float damageAmount);`
  - Hit-location aware damage router (head / neck / limb multipliers) used by both player and AI.

### Execution Sequence & How to Use
1. **Sample Inputs**: Poll keyboard movement vector and mouse look delta.
2. **Execute Ground Verification**: Call `HumanPlayer_CheckGround` to determine support status.
3. **Integrate Movement**: Call `HumanPlayer_UpdateMotion` passing delta time in seconds.
4. **Process Landing Events**: If transitioning from airborne to grounded, pass downward velocity into `FallLanding`.

---

## 4. Rigid Body Dynamics & Mesh Collision Physics

### Formats & Structural Roles
- **MEF Collision Sub-Chunks**:
  - `XTVC` (`0x58545643`): Collision vertices (Bounding boxes, convex hull points).
  - `ECFC` (`0x45434643`): Collision face planes and normal equations (`Ax + By + Cz + D = 0`).
- **Dynamic Physics Objects**: Movable crates, barrels, swinging lights, and explosive canisters subjected to gravity, terrain friction, and impact impulses.

### Clean Native Locations
- **Rigid Object Setup**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physicsobj/0042DA70-PhysicsObj_Setup.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physicsobj/0042DA70-PhysicsObj_Setup.h`
- **Rigid Body Simulation Step**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/005098F0-Physics_SimulateRigidBody_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/005098F0-Physics_SimulateRigidBody_9.h`
- **Hull Collision Detection**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509910-Physics_CheckHullCollision_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509910-Physics_CheckHullCollision_9.h`
- **External Impulse Application**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509920-Physics_ApplyImpulse_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509920-Physics_ApplyImpulse_9.h`
- **Force Accumulator & Gravity**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509960-Physics_UpdateForces_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509960-Physics_UpdateForces_9.h`
- **Constraint Solver**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509A40-Physics_SolveConstraints_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509A40-Physics_SolveConstraints_9.h`
- **Terrain Normal & Slope Interaction**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physicsterrain/004F42D0-PhysicsTerrain_InvokeComputeNormals.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physicsterrain/004F42D0-PhysicsTerrain_InvokeComputeNormals.h`

### Function Signatures & Calling Interface
- **`PhysicsObj_Setup` (`0x0042DA70`)**:
  - Signature: `int32_t PhysicsObj_Setup(void* bodyContext, void* collisionHullData, float mass);`
  - Initializes mass, center of mass, and inverse inertia tensor matrix.
- **`Physics_ApplyImpulse_9` (`0x00509920`)**:
  - Signature: `int32_t Physics_ApplyImpulse_9(void* bodyContext, float* impulseVector, float* hitPoint);`
  - Imparts linear impulse (`dp = J`) and angular impulse (`dL = r x J`).
- **`Physics_SimulateRigidBody_9` (`0x005098F0`)**:
  - Signature: `int32_t Physics_SimulateRigidBody_9(void* bodyContext, float deltaSeconds);`
  - Performs Verlet integration on linear and angular momentum.

### Execution Sequence & How to Use
1. **Initialize Rigid Entities**: Pass `XTVC` hull pointers to `PhysicsObj_Setup`.
2. **Apply Dynamic Impulses**: When bullet impacts or explosions hit the object, invoke `Physics_ApplyImpulse_9`.
3. **Step Physics Clock**: Call `Physics_UpdateForces_9` followed by `Physics_SimulateRigidBody_9` and `Physics_SolveConstraints_9`.

---

## 5. AI Perception, Vision Cones, Hearing & Combat State Machine

### Formats & Sensory Thresholds
- **Vision Cones**: 75-degree horizontal FOV, 60m daylight vision range, reduced to 15m in heavy fog or night missions.
- **Hearing Sensitivities**: Unsuppressed gunfire alerts within 120m radius; crouched footstep sounds alert within 3m; sprinting footstep sounds alert within 15m.
- **Alert States**:
  - `0` (Green): Relaxed / Patrol. Operates on waypoint spline graph.
  - `1` (Yellow): Suspicious / Investigation. Searches last heard sound location.
  - `2` (Red): Combat / Engaging. Takes cover, fires bursts, communicates to squad.

### Clean Native Locations
- **Line-of-Sight Visibility Ray**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/ai/00489B20-AI_LineOfSight.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/ai/00489B20-AI_LineOfSight.h`
- **AI Actor Task Lifecycle (Init / Load / Setup / Cleanup)**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044C8D0-HumanAI_Init.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044F060-HumanAI_LoadParameters.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044F410-HumanAI_Setup.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/004507D0-HumanAI_Cleanup.cpp`
- **AI Event / Action Dispatch**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044FC90-HumanAI_EventDispatch.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044FEC0-HumanAI_RunActions.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044FFC0-HumanAI_Process.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/004505D0-HumanAI_OnEvent.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/00450770-HumanAI_Action.cpp`
- **Waypoint Patrol Navigation**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/aiaction/0044D0E0-AIAction_Patrol.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/ai/00450F10-AI_UpdatePatrol_2.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/patrolpath/00453470-PatrolPath_Create.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/patrolpath/00453490-PatrolPath_Update_2.cpp`
- **Combat State & Aim**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/combatstate/00481E60-CombatState_ProcessStep_481E60.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/combat/004511F0-Combat_SetAimMode.cpp`

### Function Signatures & Calling Interface
- **`AI_LineOfSight` (`0x00489B20`)**:
  - Signature: `int32_t AI_LineOfSight(float maxDistance, double* fromPosition, int32_t toEntity);`
  - Casts the visibility ray through the occlusion/portal structure and terrain. Returns `1` if the target is visible.
- **`HumanAI_Process` (`0x0044FFC0`)**:
  - Signature: `int32_t* HumanAI_Process(void* aiActor);`
  - Per-frame AI tick: evaluates senses, runs the current state, and dispatches actions.
- **`HumanAI_EventDispatch` (`0x0044FC90`)**:
  - Signature: `int32_t HumanAI_EventDispatch(void* aiActor, int32_t eventId, int32_t eventArg);`
  - Routes sound events, body discoveries and alarm notifications to the alert state.
- **`AI_UpdatePatrol_2` (`0x00450F10`)**:
  - Signature: `int32_t AI_UpdatePatrol_2(void* aiActor, void* patrolPath);`
  - Advances AI actor toward the current patrol waypoint; triggers idle pauses or look-around animations.
- **`CombatState_ProcessStep_481E60` (`0x00481E60`)**:
  - Signature: `void* CombatState_ProcessStep_481E60(void* aiActor, void* targetEntity);`
  - Runs the combat state machine step: target selection, cover, burst timing.

### Execution Sequence & How to Use
1. **Sense Evaluation**: Invoke `AI_LineOfSight` against the player and squad members.
2. **State Transition**: Call `HumanAI_EventDispatch` to update internal state from Green to Yellow or Red.
3. **Dispatch Behavior**:
   - If Green: Call `AI_UpdatePatrol_2`.
   - If Red: Call `CombatState_ProcessStep_481E60` to orient the weapon toward the target and calculate firing burst timings.

---

## 6. Skeletal Animation, Bone Hierarchy, Quaternions & Splines (.iff, .bef)

### Formats & Binary Specifications
- **`.iff` (Interchange File Format / Animation Clip)**: Binary keyframe tracks containing timestamps, joint indices, 3D translation vectors, and 4D quaternion rotations (`qx, qy, qz, qw`).
- **`.bef` (Bone Element File)**: Text table defining skeletal bone linkages (Root, Pelvis, Spine, Neck, Head, Clavicle, UpperArm, Forearm, Hand, Thigh, Calf, Foot).
- **`REIH` & `MANB` Chunks**: Embedded inside character MEF models specifying rest-pose transformation matrices and 16-character bone identifier strings.

### Clean Native Locations
- **Spline Curve Evaluator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509570-AnimSpline_InvokeLoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509570-AnimSpline_InvokeLoadParameters.h`
- **Quaternion Slerp Interpolation**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509450-AnimSpline_Invoke27.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509450-AnimSpline_Invoke27.h`
- **Weapon Animation Updater**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414E20-WeaponType_UpdateAnim.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414E20-WeaponType_UpdateAnim.h`

### Function Signatures & Calling Interface
- **`AnimSpline_InvokeLoadParameters` (`0x00509570`)**:
  - Signature: `int32_t* AnimSpline_InvokeLoadParameters(void* splineContext, void* fileStream);`
  - Parses keyframe knot array, timestamps, and rotational splines from the `.iff` file stream.
- **`AnimSpline_Invoke27` (`0x00509450`)**:
  - Signature: `int32_t AnimSpline_Invoke27(float normalizedTime, float* outQuaternion, float* outTranslation);`
  - Performs spherical linear interpolation (Slerp) between adjacent quaternion keyframes.

### Execution Sequence & How to Use
1. **Load Animation Clip**: Mount `.iff` animation stream using `AnimSpline_InvokeLoadParameters`.
2. **Calculate Playback Phase**: Advance local playback time using clip frame rate.
3. **Interpolate Transforms**: Call `AnimSpline_Invoke27` to obtain bone rotations and translation offsets.
4. **Skinning Pass**: Traverse bone tree and concatenate parent-child matrices into final skinning arrays.

---

## 7. Visual Effects, Smoke, Particles, Tracers & Weather Dynamics

### Formats & Particle Types
- **Particle System Structure**: Emitter position, velocity vector, initial size, expansion rate, lifetime, alpha fade curve, and texture atlas index.
- **Particle Types**:
  - Smoke puffs (Grenades, vehicle exhaust, chimney plumes).
  - Weapon muzzle flash flares (3D billboards anchored to MEF `MVTX` marker vertices).
  - Bullet tracer ribbons (Cylindrical elongated triangle strips along bullet trajectories).
  - Blood spray & impact debris.
  - Rain & snow precipitation volumes.

### Clean Native Locations
- **Particle Physics Integrator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530210-Particle_UpdateParticlePhysics.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530210-Particle_UpdateParticlePhysics.h`
- **Billboard 3D Sprite Renderer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530470-Particle_Render3DSprites.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530470-Particle_Render3DSprites.h`
- **Muzzle Flash Flare Trigger**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/005304B0-Particle_TriggerMuzzleFlash.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/005304B0-Particle_TriggerMuzzleFlash.h`
- **Tracer Trail Ribbon Renderer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530520-Particle_RenderTrails.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530520-Particle_RenderTrails.h`
- **Smoke Alpha Fade & Dissipation Engine**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530580-Particle_FadeParticles.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530580-Particle_FadeParticles.h`
- **Emitter Reset Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/005305F0-Particle_ResetEmitters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/005305F0-Particle_ResetEmitters.h`
- **Particle Emitter Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530630-Particle_EmitParticles_2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530630-Particle_EmitParticles_2.h`
- **Weather & Environmental Atmosphere**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weatherwatersky/00523450-WeatherWaterSky_InvokeLightmapsRes.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weatherwatersky/00523450-WeatherWaterSky_InvokeLightmapsRes.h`

### Function Signatures & Calling Interface
- **`Particle_EmitParticles_2` (`0x00530630`)**:
  - Signature: `int32_t Particle_EmitParticles_2(int32_t emitterContext, int32_t count, int16_t particleType, int16_t* parameters);`
  - Spawns active particle batch into memory pools.
- **`Particle_UpdateParticlePhysics` (`0x00530210`)**:
  - Signature: `int32_t Particle_UpdateParticlePhysics(void* particlePool, float deltaSeconds);`
  - Updates particle coordinates with velocity, drag resistance, and gravity.
- **`Particle_Render3DSprites` (`0x00530470`)**:
  - Signature: `int32_t Particle_Render3DSprites(void* cameraMatrix);`
  - Generates camera-facing billboard quad polygons with additive or alpha blend modes.

### Execution Sequence & How to Use
1. **Trigger FX**: Call `Particle_EmitParticles_2` or `Particle_TriggerMuzzleFlash` when firing or explosion occurs.
2. **Step Frame Physics**: Call `Particle_UpdateParticlePhysics` every tick.
3. **Fade Dead Sprites**: Call `Particle_FadeParticles` to ramp alpha to zero.
4. **Draw Billboards**: Call `Particle_Render3DSprites` and `Particle_RenderTrails` in the transparent render pass.

---

## 8. Vehicles, Flight Dynamics & Transports (Planes, Helicopters, APCs, Tanks, Trains, Wheels)

### Formats & Physics Properties
- **Aircraft Mechanics**: Rotor lift calculations, torque balance, aerodynamic rudder/elevator surfaces, cockpit instruments, retractable landing gear hydraulics.
- **Ground Transports**: Multi-wheel suspension compression, track friction (APCs/Tanks), turret traverse angles, railway track followers (Train levels).

### Clean Native Locations
- **Aircraft Setup & Aerodynamics**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/004304B0-Vehicle_InitPlane.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/004304B0-Vehicle_InitPlane.h`
- **Helicopter Flight Dynamics**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00431560-Vehicle_InitHeli_2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00431560-Vehicle_InitHeli_2.h`
- **Rudder & Aerodynamic Controls**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00430530-Vehicle_UpdateRudder.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00430530-Vehicle_UpdateRudder.h`
- **Cockpit View Matrix**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00430940-Vehicle_UpdateCockpit.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00430940-Vehicle_UpdateCockpit.h`
- **Landing Gear Hydraulics**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00431E30-Vehicle_DeployGear_2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00431E30-Vehicle_DeployGear_2.h`
- **Wheel Suspension & Ground Contact**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehiclephysics/0042FEF0-VehiclePhysics_ProcessWheel.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehiclephysics/0042FEF0-VehiclePhysics_ProcessWheel.h`
- **Wheel Parameters Binder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/wheel/00430000-Wheel_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/wheel/00430000-Wheel_LoadParameters.h`
- **Train Track Traversal Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/train/0042A100-Train_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/train/0042A100-Train_LoadParameters.h`

### Function Signatures & Calling Interface
- **`Vehicle_InitPlane` (`0x004304B0`)**:
  - Signature: `long double Vehicle_InitPlane(float speed, int32_t vehicleHandle);`
  - Calculates aerodynamic drag and lift coefficients based on current airspeed.
- **`Vehicle_InitHeli_2` (`0x00431560`)**:
  - Signature: `int32_t* Vehicle_InitHeli_2(float rotorRpm);`
  - Initializes helicopter rotor lift curves and anti-torque tail rotor matrices.
- **`VehiclePhysics_ProcessWheel` (`0x0042FEF0`)**:
  - Signature: `int32_t* VehiclePhysics_ProcessWheel(int32_t* wheelContext);`
  - Casts suspension spring ray against ground terrain elevation to resolve wheel normal force.
- **`Train_LoadParameters` (`0x0042A100`)**:
  - Signature: `int32_t Train_LoadParameters(void* trainContext);`
  - Binds train carriage entity to the railway spline path.

### Execution Sequence & How to Use
1. **Initialize Vehicle**: Call `Vehicle_InitPlane`, `Vehicle_InitHeli_2`, or `Train_LoadParameters` during mission entity spawn.
2. **Poll Flight / Drive Controls**: Update throttle, pitch, roll, and yaw inputs.
3. **Execute Physics Simulation**: Call `VehiclePhysics_ProcessWheel` for each wheel, then update chassis transform.

---

## 9. Audio Engine, DirectSound, 3D Spatializer & Positional Audio (.wav)

### Formats & Audio Specs
- **`.wav` (RIFF Wave PCM)**: Standard uncompressed PCM audio (11.025 kHz, 22.05 kHz, 44.1 kHz, 16-bit mono for 3D positional effects, stereo for ambient music and cutscenes).
- **Positional Spatializer**: DirectSound3D hardware buffer spatializer calculating logarithmic distance attenuation, Doppler frequency shift, and stereo panning (`-1.0f` to `+1.0f`).

### Clean Native Locations
- **Play Sound Sample**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0810-SoundSys_PlaySoundSample.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0810-SoundSys_PlaySoundSample.h`
- **3D Positional Spatializer Update**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0830-SoundSys_Update3DPosition.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0830-SoundSys_Update3DPosition.h`
- **Master Volume Controls**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0CB0-SoundSys_SetMasterVolume.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0CB0-SoundSys_SetMasterVolume.h`
- **Frequency / Pitch Shift (Doppler)**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0CC0-SoundSys_SetFrequency.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0CC0-SoundSys_SetFrequency.h`
- **Stop Channel Voice**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0E20-SoundSys_StopSoundSample.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0E20-SoundSys_StopSoundSample.h`
- **Sound Definition Script Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E1130-SoundSys_LoadSoundDef.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E1130-SoundSys_LoadSoundDef.h`

### Function Signatures & Calling Interface
- **`SoundSys_PlaySoundSample` (`0x004E0810`)**:
  - Signature: `int32_t SoundSys_PlaySoundSample(int32_t sampleId, float volumeGain, float stereoPan);`
  - Allocates hardware channel voice. Returns allocated channel ID integer.
- **`SoundSys_Update3DPosition` (`0x004E0830`)**:
  - Signature: `int32_t SoundSys_Update3DPosition(int32_t channelId, float posX, float posY, float posZ);`
  - Updates DirectSound3D buffer coordinates in world space.
- **`SoundSys_StopSoundSample` (`0x004E0E20`)**:
  - Signature: `int32_t SoundSys_StopSoundSample(int32_t channelId);`
  - Immediately releases voice channel back to the free mixer pool.

### Execution Sequence & How to Use
1. **Load Sound Catalog**: Call `SoundSys_LoadSoundDef` to map sound IDs to `.wav` files.
2. **Trigger Audio**: Invoke `SoundSys_PlaySoundSample` with sample ID and initial volume.
3. **Update 3D Coordinates**: For moving sources (bullets, vehicles, human voices), call `SoundSys_Update3DPosition` each frame.
4. **Halt Channel**: Call `SoundSys_StopSoundSample` upon sound completion.

---

## 10. Cutscenes, Cinematics & FMV Movie Streaming (.bik, .avi)

### Formats & Cinematic Modes
- **FMV Video Playback**: Full-motion video using Bink Video (`.bik`) and Video for Windows (`.avi`). Stored under virtual resource archive `"LANGUAGE:movies.res"`.
- **In-Engine Scripted Cinematics**: Real-time camera splines, actor choreography, and dialog synchronizers. Can be interrupted by pressing the Escape key.

### Clean Native Locations
- **Cutscene Task Creator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F4B00-CutScene_TaskCreate.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F4B00-CutScene_TaskCreate.h`
- **Cutscene Parameters & Script Binder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F4EE0-CutScene_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F4EE0-CutScene_LoadParameters.h`
- **Cutscene Frame Sequencer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F55D0-CutScene_ApplyFrame.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F55D0-CutScene_ApplyFrame.h`
- **Cutscene Tick Updater**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F51D0-CutScene_TaskUpdate.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F51D0-CutScene_TaskUpdate.h`
- **Cutscene Cleanup Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F6520-CutScene_Cleanup.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F6520-CutScene_Cleanup.h`
- **Cutscene Interruption Key Handler (ESC Key)**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488E00-LevelFlow_GetBreakCutSceneKey.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488E00-LevelFlow_GetBreakCutSceneKey.h`
- **Movie Media Stream Registrar**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004150E0-MovieSubtitle_RegisterMedia.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004150E0-MovieSubtitle_RegisterMedia.h`

### Function Signatures & Calling Interface
- **`MovieSubtitle_RegisterMedia` (`0x004150E0`)**:
  - Signature: `int32_t MovieSubtitle_RegisterMedia(void* context, int32_t* movieParameters);`
  - Mounts `"LANGUAGE:movies.res"`, allocates DirectDraw streaming surface, and connects to the video presentation loop.
- **`CutScene_TaskCreate` (`0x004F4B00`)**:
  - Signature: `int32_t* CutScene_TaskCreate(void);`
  - Allocates cutscene task descriptor and registers it with the QTask cooperative scheduler.
- **`CutScene_ApplyFrame` (`0x004F55D0`)**:
  - Signature: `int32_t CutScene_ApplyFrame(void* cutsceneContext, float frameTime);`
  - Computes camera spline position, field-of-view, and dispatches actor triggers.
- **`LevelFlow_GetBreakCutSceneKey` (`0x00488E00`)**:
  - Signature: `int32_t LevelFlow_GetBreakCutSceneKey(int32_t keyContext);`
  - Returns `1` if the skip cutscene key was pressed.

### Execution Sequence & How to Use
1. **Mount Movie Container**: Call `MovieSubtitle_RegisterMedia` to open `"LANGUAGE:movies.res"`.
2. **Create Task**: Invoke `CutScene_TaskCreate` to register the cinematic sequencer.
3. **Execute Frame Evaluation**: Call `CutScene_ApplyFrame` each frame tick.
4. **Monitor Skip Input**: Check `LevelFlow_GetBreakCutSceneKey` to allow player skipping.
5. **Halt & Restore Controls**: Call `CutScene_Cleanup` to release cutscene camera and return player controls.

---

## 11. Subtitles, Typography, Glyphs & On-Screen Display (.fnt)

### Formats & Glyph Atlas Structure
- **`.fnt` (Bitmap Font Descriptor)**: Contains font header, character height, baseline offset, and per-glyph coordinates `[X, Y, Width, Height]`.
  - `font1.fnt`: Small HUD text (Objectives, ammo counters, subtitles).
  - `font4.fnt`: Large menu and title header font.
- **Subtitle Records**: Text string, display timestamp, screen X/Y anchor, RGBA color, and persistence duration float.

### Clean Native Locations
- **Font File Loader (`font1.fnt`, `font4.fnt`)**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/fx/00485FB0-FX_Font4Fnt.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/fx/00485FB0-FX_Font4Fnt.h`
- **Subtitle Font Registrar**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004151E0-MovieSubtitle_RegisterFont.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004151E0-MovieSubtitle_RegisterFont.h`
- **Subtitle Text & Timing Registrar**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/00415260-MovieSubtitle_RegisterText.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/00415260-MovieSubtitle_RegisterText.h`
- **On-Screen Display String Formatter**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/textbox/0041AE20-TextBox_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/textbox/0041AE20-TextBox_LoadParameters.h`
- **Typewriter Text Effect Box**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/typewriterbox/0041A6D0-TypeWriterBox_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/typewriterbox/0041A6D0-TypeWriterBox_LoadParameters.h`

### Function Signatures & Calling Interface
- **`FX_Font4Fnt` (`0x00485FB0`)**:
  - Signature: `void* FX_Font4Fnt(int32_t context, int32_t flag);`
  - Loads `font1.fnt` or `font4.fnt` into memory based on flag parameter.
- **`MovieSubtitle_RegisterFont` (`0x004151E0`)**:
  - Signature: `int32_t MovieSubtitle_RegisterFont(int32_t context, int32_t fontId, float scale);`
  - Registers the font face for subtitle rendering.
- **`MovieSubtitle_RegisterText` (`0x00415260`)**:
  - Signature: `int32_t MovieSubtitle_RegisterText(long double timeStart, long double timeEnd, long double posX, long double posY, long double scaleX, long double scaleY, int32_t fontId, char* textString, float alpha);`
  - Queues timed subtitle text entry for display.

### Execution Sequence & How to Use
1. **Load Font Asset**: Call `FX_Font4Fnt` to parse glyph metrics from the `.fnt` file.
2. **Register Subtitle Font**: Invoke `MovieSubtitle_RegisterFont`.
3. **Queue Dialogue Line**: Call `MovieSubtitle_RegisterText` passing start/end timestamps and text string.
4. **Render Subtitles**: Text is composited onto screen coordinates during HUD draw phase.

---

## 12. Interactive World Entities & Triggers (Doors, Cabinets, Keypads, Switches, Terminals, Alarms)

### Formats & Entity State Machine
- **Interactive Objects**: Sliding doors, swinging cabinet doors, breakable glass panels, security card readers, computer terminals, alarm bells.
- **States**: `0` = Closed/Locked, `1` = Opening, `2` = Open, `3` = Closing, `4` = Jammed/Alarm Triggered.

### Clean Native Locations
- **Door & Cabinet Interaction Pipeline**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/004481F0-DoorCabinet_Invoke8_4481F0.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/004481F0-DoorCabinet_Invoke8_4481F0.h`
- **Glass Shatter Trigger**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/00449CB0-DoorCabinet_Invoke333333333Glassshatte.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/00449CB0-DoorCabinet_Invoke333333333Glassshatte.h`
- **Door Movement Step Integrator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/0044C2C0-DoorCabinet_ProcessStep_44C2C0.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/0044C2C0-DoorCabinet_ProcessStep_44C2C0.h`
- **Helicopter Door Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/helidoor/00427240-HeliDoor_Init_2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/helidoor/00427240-HeliDoor_Init_2.h`
- **Wall Switch Initialization & Setup**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/00444BF0-Switch_Init.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/00444BF0-Switch_Init.h`
  - Setup: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/004450F0-Switch_Setup.cpp`
- **Switch Interaction Updater**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/004452F0-Switch_Update.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/004452F0-Switch_Update.h`
- **Computer Terminal System**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terminal/00443680-Terminal_Init.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terminal/00443680-Terminal_Init.h`
  - Update: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terminal/00443B80-Terminal_Update.cpp`
- **Alarm Bell Trigger Subsystem**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/env/00447D50-Env_TriggerAlarm_9.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/env/00447D50-Env_TriggerAlarm_9.h`

### Function Signatures & Calling Interface
- **`DoorCabinet_Invoke333333333Glassshatte` (`0x00449CB0`)**:
  - Signature: `struct status_msg* DoorCabinet_Invoke333333333Glassshatte(struct status_msg* statusMsg, int32_t* doorContext);`
  - Spawns glass shards and shatters the window collision face when penetrated by bullet raycast.
- **`Switch_Update` (`0x004452F0`)**:
  - Signature: `int32_t Switch_Update(void* switchContext, int32_t playerInteractFlag);`
  - Toggles switch state and emits QVM event to open linked security gates.
- **`Env_TriggerAlarm_9` (`0x00447D50`)**:
  - Signature: `int32_t Env_TriggerAlarm_9(void* envContext, int32_t alarmZoneId);`
  - Starts rotating red beacon lights, sound sirens, and alerts all AI guards in the zone to Red alert.

### Execution Sequence & How to Use
1. **Initialize Objects**: Mount level entities via `Switch_Init`, `Terminal_Init`, or `DoorCabinet_Invoke8_4481F0`.
2. **Detect Player Use Key**: When player presses Action key within interaction radius, call `Switch_Update`.
3. **Handle Bullet Impacts**: If bullet strikes glass, call `DoorCabinet_Invoke333333333Glassshatte`.
4. **Trigger Alarm**: Call `Env_TriggerAlarm_9` when camera spots player or guard reaches alarm button.

---

## 13. Weapons, Ballistics, Recoil Systems & Fire Timers

### Formats & Weapon Descriptors
- **Weapon Descriptor Array**: Base address `0x00AFA6E0`, count at `0x00AFA7E0`. Each entry contains weapon ID, fire-rate cooldown, recoil kick pitch impulse, horizontal dispersion cone, magazine capacity, and muzzle velocity float.
- **Ballistics Raycast**: Straight-line raycast tested against terrain heightmap and entity collision hulls. Bullet penetration calculated through wood, glass, and thin sheet metal.

### Clean Native Locations
- **Weapon Count Getter**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapons/00413BB0-Weapons_CountGet.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapons/00413BB0-Weapons_CountGet.h`
- **Weapon Type Registrar**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414810-WeaponType_RegisterType.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414810-WeaponType_RegisterType.h`
- **Weapon Configuration Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414950-WeaponType_GetConfig.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414950-WeaponType_GetConfig.h`
- **Weapon Cycle & Slot Switcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/004149B0-WeaponType_CycleWeapon.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/004149B0-WeaponType_CycleWeapon.h`
- **Weapon Gun Event Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapongun/0047D060-WeaponGun_InvokeOnEvent.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapongun/0047D060-WeaponGun_InvokeOnEvent.h`
- **Gun Firing & Recoil Tick Processor**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapongun/0047D320-WeaponGun_ProcessStep_47D320.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapongun/0047D320-WeaponGun_ProcessStep_47D320.h`

### Function Signatures & Calling Interface
- **`Weapons_CountGet` (`0x00413BB0`)**:
  - Signature: `int32_t Weapons_CountGet(void);`
  - Returns total registered weapons in the global weapon table.
- **`WeaponType_GetConfig` (`0x00414950`)**:
  - Signature: `int32_t WeaponType_GetConfig(int32_t weaponId);`
  - Returns pointer to weapon configuration struct.
- **`WeaponGun_ProcessStep_47D320` (`0x0047D320`)**:
  - Signature: `int32_t WeaponGun_ProcessStep_47D320(void* gunContext, float deltaSeconds);`
  - Updates recoil recovery springs and cools down the fire timer.

### Execution Sequence & How to Use
1. **Fetch Parameters**: Call `WeaponType_GetConfig` for active weapon ID.
2. **Trigger Fire**: When user presses Fire, check cooldown timer. If zero, spawn projectile raycast, decrement ammo, trigger muzzle flash, and apply pitch recoil impulse.
3. **Simulate Gun Recovery**: Call `WeaponGun_ProcessStep_47D320` every frame tick to damp pitch kick back to center.

---

## 14. Mission Scripts, Virtual Machine & Cooperative Task Engine (.qsc, .qvm, QTask)

### Formats & Virtual Machine Architecture
- **`.qsc` (Quake/Loop Script Source)**: Text source script declaring functions, triggers, patrols, and objectives.
- **`.qvm` (Quake/Loop Virtual Machine Bytecode)**: Compiled 32-bit stack bytecode.
- **`QTask`**: Cooperative multi-subsystem engine task scheduler. Tasks register initialization, per-frame tick, and cleanup function pointers.

### Clean Native Locations
- **QVM Bytecode Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qvm/004B80B0-Qvm_Load.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qvm/004B80B0-Qvm_Load.h`
- **QSC Script Compiler**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qsc/004B8410-Qsc_Compile.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qsc/004B8410-Qsc_Compile.h`
- **QTask Task Scheduler & Creator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qtask/00401000-QTask_CreateTask.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qtask/00401000-QTask_CreateTask.h`

### Function Signatures & Calling Interface
- **`Qvm_Load` (`0x004B80B0`)**:
  - Signature: `int32_t* Qvm_Load(void* scriptPath);`
  - Loads `.qvm` file into execution memory and verifies bytecode header.
- **`QTask_CreateTask` (`0x00401000`)**:
  - Signature: `int32_t* QTask_CreateTask(void* taskInitFunc, void* taskUpdateFunc, void* taskCleanupFunc, int32_t priority);`
  - Registers task in the engine cooperative scheduler queue.

### Execution Sequence & How to Use
1. **Compile or Load**: Call `Qvm_Load` passing virtual script path (e.g., `"LOCAL:levels/level01/mission.qvm"`).
2. **Register Tasks**: Pass update callbacks to `QTask_CreateTask`.
3. **Execute Main Loop**: Scheduler iterates active tasks each frame in priority order.

---

## 15. Level Streaming, Sublevels & State Persistence (.res, sublevel tasks)

### Formats & Persistence Model
- **Sublevels**: Modular mission blocks (`sublevel/*.res`) loaded on demand as the player moves across the large open terrain.
- **State Persistence**: Serializes player health, inventory, ammo counts, completed objectives, and destroyed structures.

### Clean Native Locations
- **SubLevel Parameter & Script Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sublevel/00488780-SubLevel_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sublevel/00488780-SubLevel_LoadParameters.h`
- **Level Flow Task (mission start / state)** — `LevelFlow_StartMission_2` **does not exist**; the real `LevelFlow` natives are:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488BB0-LevelFlow_Init.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00489290-LevelFlow_LoadParameters.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/004890F0-LevelFlow_Update.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488F90-LevelFlow_Process.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488ED0-LevelFlow_CheckState.cpp`

### Function Signatures & Calling Interface
- **`SubLevel_LoadParameters` (`0x00488780`)**:
  - Signature: `int32_t* SubLevel_LoadParameters(void* context);`
  - Reads sublevel definition from file, loads embedded `.qvm`, registers `"Task_New"` script symbol context, executes script bytecode, and cleans up.

### Execution Sequence & How to Use
1. **Trigger Sublevel Stream**: When player crosses spatial boundary trigger, call `SubLevel_LoadParameters`.
2. **Execute Initialization**: Embedded script executes setup routines and instantiates sublevel entities.

---

## 16. Master Function-to-Clean-Path Cross Reference Table

| Subsystem | Function Name | Virtual Address | Exact Clean Implementation File |
|---|---|---|---|
| **3D Models** | `DynObj_InsertHashItem_14` | `0x004CF080` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF080-DynObj_InsertHashItem_14.cpp` |
| **3D Models** | `DynObj_LocalModelsSMef` | `0x004CF390` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.cpp` |
| **3D Models** | `DynObj_LocalTexturesSTex` | `0x004CF790` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.cpp` |
| **Resources** | `Resource_Load` | `0x004B5F00` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B5F00-Resource_Load.cpp` |
| **Terrain** | `DynObj_ParseConfigFile_7` | `0x004C5F00` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C5F00-DynObj_ParseConfigFile_7.cpp` |
| **Terrain** | `DynObj_Createterrainmateria` | `0x004C6240` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C6240-DynObj_Createterrainmateria.cpp` |
| **Terrain** | `Terrain_TextureFileNotOfRequ`| `0x005175A0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.cpp` |
| **Terrain** | `Terrain_RenderQuadtree` *(name unverified, gate FAIL)* | `0x00510010` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00510010-Terrain_RenderQuadtree.cpp` |
| **Terrain** | `Terrain_SampleHeightmap` | `0x00512040` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512040-Terrain_SampleHeightmap.cpp` |
| **Terrain** | `Terrain_CalculateNormals` | `0x005122B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005122B0-Terrain_CalculateNormals.cpp` |
| **Terrain** | `Terrain_RenderWaterSurface` | `0x005116B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005116B0-Terrain_RenderWaterSurface.cpp` |
| **Player** | `HumanPlayer_CheckGround` | `0x00408330` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408330-HumanPlayer_CheckGround.cpp` |
| **Player** | `HumanPlayer_UpdateMotion` | `0x00408320` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanplayer/00408320-HumanPlayer_UpdateMotion.cpp` |
| **Player** | `FallLanding` | `0x00408460` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/misc/00408460-FallLanding.cpp` |
| **Player** | `Human_DamageApply` | `0x00412440` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/human/00412440-Human_DamageApply.cpp` |
| **Player** | `Human_DamageHandler` | `0x0045D3C0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/human/0045D3C0-Human_DamageHandler.cpp` |
| **Physics** | `PhysicsObj_Setup` | `0x0042DA70` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physicsobj/0042DA70-PhysicsObj_Setup.cpp` |
| **Physics** | `Physics_SimulateRigidBody_9`| `0x005098F0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/005098F0-Physics_SimulateRigidBody_9.cpp` |
| **Physics** | `Physics_CheckHullCollision_9`| `0x00509910` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509910-Physics_CheckHullCollision_9.cpp` |
| **Physics** | `Physics_ApplyImpulse_9` | `0x00509920` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509920-Physics_ApplyImpulse_9.cpp` |
| **AI** | `AI_LineOfSight` | `0x00489B20` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/ai/00489B20-AI_LineOfSight.cpp` |
| **AI** | `HumanAI_Process` | `0x0044FFC0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044FFC0-HumanAI_Process.cpp` |
| **AI** | `HumanAI_EventDispatch` | `0x0044FC90` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/humanai/0044FC90-HumanAI_EventDispatch.cpp` |
| **AI** | `AI_UpdatePatrol_2` | `0x00450F10` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/ai/00450F10-AI_UpdatePatrol_2.cpp` |
| **AI** | `CombatState_ProcessStep_481E60`| `0x00481E60` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/combatstate/00481E60-CombatState_ProcessStep_481E60.cpp` |
| **Animation** | `AnimSpline_InvokeLoadParameters`| `0x00509570` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509570-AnimSpline_InvokeLoadParameters.cpp` |
| **Animation** | `AnimSpline_Invoke27` | `0x00509450` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/00509450-AnimSpline_Invoke27.cpp` |
| **Particles** | `Particle_UpdateParticlePhysics`| `0x00530210` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530210-Particle_UpdateParticlePhysics.cpp` |
| **Particles** | `Particle_Render3DSprites` | `0x00530470` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530470-Particle_Render3DSprites.cpp` |
| **Particles** | `Particle_TriggerMuzzleFlash`| `0x005304B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/005304B0-Particle_TriggerMuzzleFlash.cpp` |
| **Particles** | `Particle_RenderTrails` | `0x00530520` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530520-Particle_RenderTrails.cpp` |
| **Particles** | `Particle_FadeParticles` | `0x00530580` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530580-Particle_FadeParticles.cpp` |
| **Particles** | `Particle_EmitParticles_2` | `0x00530630` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/particle/00530630-Particle_EmitParticles_2.cpp` |
| **Vehicles** | `Vehicle_InitPlane` | `0x004304B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/004304B0-Vehicle_InitPlane.cpp` |
| **Vehicles** | `Vehicle_InitHeli_2` | `0x00431560` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehicle/00431560-Vehicle_InitHeli_2.cpp` |
| **Vehicles** | `VehiclePhysics_ProcessWheel`| `0x0042FEF0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/vehiclephysics/0042FEF0-VehiclePhysics_ProcessWheel.cpp` |
| **Vehicles** | `Train_LoadParameters` | `0x0042A100` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/train/0042A100-Train_LoadParameters.cpp` |
| **Sound** | `SoundSys_PlaySoundSample` | `0x004E0810` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0810-SoundSys_PlaySoundSample.cpp` |
| **Sound** | `SoundSys_Update3DPosition` | `0x004E0830` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0830-SoundSys_Update3DPosition.cpp` |
| **Sound** | `SoundSys_StopSoundSample` | `0x004E0E20` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/soundsys/004E0E20-SoundSys_StopSoundSample.cpp` |
| **Cutscenes** | `CutScene_TaskCreate` | `0x004F4B00` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F4B00-CutScene_TaskCreate.cpp` |
| **Cutscenes** | `CutScene_ApplyFrame` | `0x004F55D0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F55D0-CutScene_ApplyFrame.cpp` |
| **Cutscenes** | `CutScene_Cleanup` | `0x004F6520` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/cutscene/004F6520-CutScene_Cleanup.cpp` |
| **Cutscenes** | `LevelFlow_GetBreakCutSceneKey`| `0x00488E00` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/levelflow/00488E00-LevelFlow_GetBreakCutSceneKey.cpp` |
| **Movies** | `MovieSubtitle_RegisterMedia`| `0x004150E0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004150E0-MovieSubtitle_RegisterMedia.cpp` |
| **Movies** | `MovieSubtitle_RegisterFont` | `0x004151E0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/004151E0-MovieSubtitle_RegisterFont.cpp` |
| **Subtitles** | `MovieSubtitle_RegisterText` | `0x00415260` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/moviesubtitle/00415260-MovieSubtitle_RegisterText.cpp` |
| **Fonts** | `FX_Font4Fnt` | `0x00485FB0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/fx/00485FB0-FX_Font4Fnt.cpp` |
| **Entities** | `DoorCabinet_Invoke333333333Glassshatte`| `0x00449CB0`| `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/doorcabinet/00449CB0-DoorCabinet_Invoke333333333Glassshatte.cpp` |
| **Entities** | `Switch_Update` | `0x004452F0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/switch/004452F0-Switch_Update.cpp` |
| **Entities** | `Terminal_Update` | `0x00443B80` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terminal/00443B80-Terminal_Update.cpp` |
| **Entities** | `Env_TriggerAlarm_9` | `0x00447D50` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/env/00447D50-Env_TriggerAlarm_9.cpp` |
| **Weapons** | `Weapons_CountGet` | `0x00413BB0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapons/00413BB0-Weapons_CountGet.cpp` |
| **Weapons** | `WeaponType_GetConfig` | `0x00414950` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapontype/00414950-WeaponType_GetConfig.cpp` |
| **Weapons** | `WeaponGun_ProcessStep_47D320`| `0x0047D320`| `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/weapongun/0047D320-WeaponGun_ProcessStep_47D320.cpp` |
| **QVM** | `Qvm_Load` | `0x004B80B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qvm/004B80B0-Qvm_Load.cpp` |
| **QTask** | `QTask_CreateTask` | `0x00401000` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qtask/00401000-QTask_CreateTask.cpp` |
| **Sublevels** | `SubLevel_LoadParameters` | `0x00488780` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sublevel/00488780-SubLevel_LoadParameters.cpp` |
| **Asset Decode** | `DynObj_LocalModelsSMef` | `0x004CF390` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.cpp` |
| **Asset Decode** | `DynObj_LocalTexturesSTex` | `0x004CF790` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.cpp` |
| **Texture Bind** | `Terrain_BuildMipMaps` | `0x005125A0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005125A0-Terrain_BuildMipMaps.cpp` |
| **Texture Bind** | `Render_BindTexture` | `0x004D09E0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/render/004D09E0-Render_BindTexture.cpp` |
| **Texture Bind** | `Render_DrawLightmapMesh` | `0x004D06C0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/render/004D06C0-Render_DrawLightmapMesh.cpp` |
| **Terrain** | `CreateTerrainMaterial` | `0x004C6670` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/misc/004C6670-CreateTerrainMaterial.cpp` |
| **Terrain** | `CreateTerrainTileMap` | `0x004C6760` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/create/004C6760-Create_TerrainTileMap.cpp` |
| **Terrain** | `Terrain_GenerateOctree` | `0x004D14E0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/render/004D14E0-Render_ShouldHaveAllocatedD.cpp` |
| **Sprite** | `Qsprite_Register4Az` | `0x004B53B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qsprite/004B53B0-Qsprite_Register4Az.cpp` |
| **Sprite** | `Sprite3D_LoadParameters` | `0x00531E10` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sprite3d/00531E10-Sprite3D_LoadParameters.cpp` |
| **Sprite** | `Sprite3D_Init` | `0x00531EF0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sprite3d/00531EF0-Sprite3D_Init.cpp` |
| **Sprite** | `Sprite3D_Update` | `0x00531F40` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sprite3d/00531F40-Sprite3D_Update.cpp` |
| **Container IO** | `Resource_Unpack` | `0x004B16D0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B16D0-Resource_Unpack.cpp` |
| **Container IO** | `Resource_PackUnpack` | `0x004B18B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B18B0-Resource_PackUnpack.cpp` |
| **Container IO** | `Resource_Unload` | `0x004B6380` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/resource/004B6380-Resource_Unload.cpp` |
| **Container IO** | `Sys_Resource` | `0x004B5F80` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/sys/004B5F80-Sys_Resource.cpp` |
| **Container IO** | `QFile_AliasResolve` | `0x004B1020` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qfile/004B1020-QFile_AliasResolve.cpp` |
| **Container IO** | `QFile_DeviceIndex` | `0x004B11A0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qfile/004B11A0-QFile_DeviceIndex.cpp` |
| **Container IO** | `QFile_ReadWrite` | `0x004B1510` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qfile/004B1510-QFile_ReadWrite.cpp` |
| **Container IO** | `QFile_DeviceHandlerLookup` | `0x004B1620` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/qfile/004B1620-QFile_DeviceHandlerLookup.cpp` |
| **Container IO** | `File_Open` | `0x004A5350` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/file/004A5350-File_Open.cpp` |

---

## 17. MEF Mesh Geometry, Vertex Stream & Index Buffer Decoding (XTRV, DNER)

### Formats & Binary Specifications
- **ILFF Container Hierarchy**: MEF files are Interchange File Format (IFF / ILFF) packages where each sub-chunk begins with a 4-byte FourCC identifier followed by a 32-bit big-endian chunk length. The root chunk is `HSEM` (`0x4D455348`, little-endian `"MESH"`), which validates engine version float `0x3DCCCCCD` (`0.1f`).
- **`XTRV` (Vertex Stream Buffer)** — FourCC `0x58545256`. **Stride: supported as 32 (format 0) / 40 (format 3); loop not yet fully traced.** Evidence: `0x0049FB10` (format selector / vertex upload path) contains `add edi, 0x20` in the format-0 loop, and the other branch stores `0x28` (40) into a descriptor field; the port's `mef_render_data.cpp` decodes exactly those records (position +0, normal +0x0C, UV +0x18, lightmap UV +0x20).
  - **Correction of the earlier revision:** it claimed a fixed 32-byte FVF stride while listing fields summing to **36 bytes** (0x24) — inconsistent. The only stride seen inside the decompiled `DynObj_LocalModelsSMef` is **`0x14` (20 bytes)** (`Mem_Alloc(i_2 * 0x14, 4)`, `add ebx, 0x14`), which indexes a **4-dword `TXAN` record array** — a *different structure*, not the vertex stream. Do not use 20 as the vertex stride.
  - **Still to confirm:** full trace of the format-3 loop increment (the `0x28` value is stored into a descriptor, so confirm it is then used as the per-vertex advance).

- **`DNER` (Primitive Index & Draw Group List)**: FourCC `0x444E4552` (`"REND"` reversed). Organizes vertices into renderable primitive batches:
  - Header: 32-bit integer count of primitive render batches.
  - Each batch entry:
    - Primitive Topology (`uint32`): `3` = Indexed Triangle List (`D3DPT_TRIANGLELIST`), `4` = Triangle Strip (`D3DPT_TRIANGLESTRIP`), `5` = Triangle Fan (`D3DPT_TRIANGLEFAN`).
    - Material/Texture Group Index (`uint32`): Maps directly to the GTT (Global Texture Table) index in the paired `.MTP` container.
    - Index Count (`uint32`): Number of 16-bit indices in the batch.
    - Index Stream: Sequential 16-bit unsigned integers (`uint16_t`) indexing vertices in the `XTRV` stream.
- **`ATTA` (Attachment Matrices)**: FourCC `0x41545441`. Holds 68-byte locator nodes (3x4 affine rotation/translation matrix + child model identifier) for weapon attachment points, vehicle passenger sockets, and camera pivot locators.
- **`PORT` / `PTFC` / `PTVX` (Indoor Occlusion Portals)**: Defines convex cells, portal polygon boundaries, and occlusion planes for room-to-room visibility determination.

### Clean Native Locations
- **MEF Model Geometry Parser & Chunk Extractor**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.h`
- **Dynamic Object Magic Sub-Chunk Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF330-DynObj_CreateMagicObject_14.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF330-DynObj_CreateMagicObject_14.h`
- **Mesh Format Validation & Stream Verification**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/0049B4B0-Display_MeshFormatNotSupport.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/0049B4B0-Display_MeshFormatNotSupport.h`
- **Model Conversion & D3D Render Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/0049DF60-DisplayRender_Processmodelsnotconvert.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/0049DF60-DisplayRender_Processmodelsnotconvert.h`
- **Vertex Buffer Stripped Drawing Pipeline**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509930-Physics_QvertexDrawstrip.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509930-Physics_QvertexDrawstrip.h`

### Function Signatures & Calling Interface
- **`DynObj_LocalModelsSMef` (`0x004CF390`)**:
  - Signature: `int32_t* DynObj_LocalModelsSMef(int32_t context);`
  - Decodes ILFF chunks, verifies `MESH` version `0x3DCCCCCD`, unpacks `XTRV` vertex array and `DNER` primitive groups into a 0x18-byte mesh header.
- **`Display_MeshFormatNotSupport` (`0x0049B4B0`)**:
  - Signature: `void* Display_MeshFormatNotSupport(void* context);`
  - Validates the mesh's FVF against the Direct3D 7 device and configures the vertex stream. *(The specific FVF/stride is not established — see the `XTRV` note above.)*
- **`DisplayRender_Processmodelsnotconvert` (`0x0049DF60`)**:
  - Signature: `int32_t* DisplayRender_Processmodelsnotconvert(int32_t* arg1);`
  - Traverses the model hierarchy, applies bone transformation matrices via `Render_DrawSplineMesh`, and dispatches indexed primitive drawing calls to the active Direct3D device.

### Execution Sequence & How to Use
1. **Unpack Model ILFF Container**: Load the `.mef` data stream into memory and invoke `DynObj_LocalModelsSMef` to extract `MESH`, `XTRV`, `DNER`, and `ATTA` chunks.
2. **Setup Direct3D Vertex Buffer**: Allocate a Direct3D 7 vertex buffer, lock it, and copy the interleaved `XTRV` vertices. **The FVF and stride must be read from the binary first** (see the `XTRV` note) — the previously stated 32-byte `XYZ|NORMAL|DIFFUSE|TEX1` layout is unverified.
3. **Parse Primitive Index Groups**: Read the `DNER` block. For each primitive group, extract the triangle topology (`D3DPT_TRIANGLELIST` or `D3DPT_TRIANGLESTRIP`), the 16-bit index array, and the GTT material index.
4. **Link Material Stages**: Query the `.MTP` container via `DynObj_InsertHashItem_14` to map the material index to the active texture stage ID.
5. **Dispatch Geometry Render**: Set the world transform matrix for the instance, bind the material texture surface, and call `IDirect3DDevice7::DrawIndexedPrimitiveStrided` with the decoded vertex stream and index buffer.

---

## 18. Retail .TEX Pixel & Mipmap Decoder & Direct3D 7 Texture Binding

### Formats & Binary Specifications
- **Container FourCC**: `"LOOP"` (`0x504F4F4C`). Retail versions are `7` (uncompressed 16-bit / 32-bit / paletted) and `9` (compressed / run-length encoded).
- **Header Layout (32 bytes)**:
  - Offset `+0x00`: FourCC identifier `"LOOP"` (`uint32`).
  - Offset `+0x04`: Format Version (`uint32`, value `7` or `9`).
  - Offset `+0x08`: Texture Width in pixels (`uint32`, e.g., 256, 128, 64).
  - Offset `+0x0C`: Texture Height in pixels (`uint32`, e.g., 256, 128, 64).
  - Offset `+0x10`: Mipmap Chain Count (`uint32`, e.g., 8 levels down to 1x1).
  - Offset `+0x14`: Pixel Color Format Enumeration (`uint32`):
    - `0x01` = 16-bit RGB 5:6:5 (Opaque diffuse textures).
    - `0x02` = 16-bit ARGB 1:5:5:5 (1-bit punch-through alpha for foliage/fences).
    - `0x03` = 16-bit ARGB 4:4:4:4 (4-bit gradient alpha for glass/smoke).
    - `0x04` = 24-bit RGB 8:8:8 (Uncompressed 24-bit truecolor).
    - `0x05` = 32-bit ARGB 8:8:8:8 (Full 8-bit alpha channel).
    - `0x06` = 8-bit Paletted / Indexed (Accompanied by 256-entry RGB palette table).
  - Offset `+0x18`: Surface Flags (`uint32`, transparency flags, clamp/repeat bits).
  - Offset `+0x1C`: Data Block Size (`uint32`, total byte length of all mip levels).
- **Mipmap Chain Storage**: Mip levels are stored contiguously from highest resolution (Level 0) down to 1x1. Stride for each level equals `Width_L * BytesPerPixel`.

### Clean Native Locations
- **Dynamic Object Texture Archive Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.h`
- **LoopEngine Terrain .TEX Decoder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.h`
- **Direct3D Texture Eviction & Memory Management**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00494700-DisplayRender_ProcessCouldntevicttext.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00494700-DisplayRender_ProcessCouldntevicttext.h`
- **Direct3D Texture Stage Register Binder**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497920-DisplayRender_Invoke2.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497920-DisplayRender_Invoke2.h`
- **DirectDraw Surface Creator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004903B0-Display_CreateSurface.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004903B0-Display_CreateSurface.h`

### Function Signatures & Calling Interface
- **`DynObj_LocalTexturesSTex` (`0x004CF790`)**:
  - Signature: `int32_t* DynObj_LocalTexturesSTex(void* context);`
  - Unpacks the `"TEXF"` block from the container, parses texture count, allocates 0x24-byte texture descriptors, and prepares mipmap chains.
- **`Terrain_TextureFileNotOfRequ` (`0x005175A0`)**:
  - Signature: `int32_t Terrain_TextureFileNotOfRequ(void* textureBuffer, int32_t expectedVersion);`
  - Validates `"LOOP"` FourCC and matches version `7` or `9`. Unpacks the palette table if format is 8-bit indexed.
- **`DisplayRender_Invoke2` (`0x00497920`)**:
  - Signature: `int32_t DisplayRender_Invoke2(int32_t stageIndex, void* lpD3DTextureSurface);`
  - Binds the uploaded DirectDraw texture surface to Direct3D texture stage `stageIndex` via `IDirect3DDevice7::SetTexture`.

### Execution Sequence & How to Use
1. **Load Raw .TEX Stream**: Read the binary file into memory and verify the `"LOOP"` FourCC header via `Terrain_TextureFileNotOfRequ`.
2. **Create DirectDraw Texture Surface**: Populate `DDSURFACEDESC2` with dimensions `Width` and `Height`, mipmap count, and flags `DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT`. Set caps to `DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP`. Call `IDirectDraw7::CreateSurface`.
3. **Upload Mipmap Levels**: For each mip level:
   - Call `IDirectDrawSurface7::Lock` to obtain the GPU surface pointer and pitch.
   - Convert or copy scanlines from the `.TEX` mip data into the surface buffer.
   - Call `IDirectDrawSurface7::Unlock`.
   - Obtain the next attached mipmap surface via `GetAttachedSurface(DDSCAPS_MIPMAP)`.
4. **Set Texture Stage Filtering**: Set Direct3D render states for texture stages:
   - `D3DTSS_MINFILTER = D3DTFN_LINEAR`
   - `D3DTSS_MAGFILTER = D3DTFG_LINEAR`
   - `D3DTSS_MIPFILTER = D3DTFP_LINEAR`
5. **Bind to Pipeline**: Call `DisplayRender_Invoke2(stage, lpD3DTextureSurface)` immediately before issuing primitive drawing calls.

---

## 19. Level 1 Terrain Elevation, Quadtree & Lightmap Pipeline (.HMP, .CTR, .BIT, .LMP)

### Formats & Binary Specifications
- **`.hmp` (Heightmap Elevation Grid)**:
  - Optional file containing 256 12-byte records, followed by byte grids.
  - Each occupied record stores `dimension` at +0x08; its block contains `(dimension + 1)^2` bytes.
  - Consumer 0x00510010 sign-extends samples, interpolates, subtracts 64 and scales by 256.
  - Grid-to-world bindings must be recovered from task/runtime data; no universal 512x512 or 8-unit assumption.
- **`.ctr` (Contour Mesh Index)** — **`UNVERIFIED`**: the file is referenced by `DynObj_Createterrainmateria` (mapped into the terrain context at `+0x0C`), but **no decompiled function decodes a node structure for it**.
  - An earlier revision described a 32-byte quadtree node (`MinBound`/`MaxBound` float3, `uint16_t[4]` child offsets, LOD metric). **That layout is not supported by any function in the binary and has been removed as fact.**
  - **To settle it:** decompile the consumer of the `+0x0C` handle and record the strides/offsets actually used, or diff a real `terrain.ctr` against its parsed runtime state.
- **`.bit` (Occlusion & Horizon Bitmask Grid)**:
  - Horizon elevation angles packed into 1-bit / 2-bit masks.
  - Used by the CPU frustum culler to rapidly discard entire mountain valleys hidden behind foreground ridgelines.
- **`.lmp` (Static Terrain Lightmap)**:
  - Baked static illumination and building cast shadows.
  - 2D grid storing 8-bit luminance or 16-bit RGB values mapped across terrain quads.
- **`.tex` (Multi-Tile Terrain Atlas)**:
  - Contains LoopEngine material texture tiles (grass, rock, gravel, road, dirt) indexed by the terrain tile map.

### Clean Native Locations
- **Terrain Tile Map Allocator & Initializer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/create/004C6760-Create_TerrainTileMap.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/create/004C6760-Create_TerrainTileMap.h`
- **Terrain Elevation, Contour & Bitmask Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C6240-DynObj_Createterrainmateria.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C6240-DynObj_Createterrainmateria.h`
- **Terrain Texture & Lightmap Parser**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C5F00-DynObj_ParseConfigFile_7.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/dynobj/004C5F00-DynObj_ParseConfigFile_7.h`
- **Terrain Quadtree Visibility & Frustum Culler**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00510010-Terrain_RenderQuadtree.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00510010-Terrain_RenderQuadtree.h`
- **Terrain Strip Drawing Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512C30-Terrain_Draw.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512C30-Terrain_Draw.h`
- **TerraDrawTask Engine Task Initializer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terradrawtask/004E82C0-TerraDrawTask_Init.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terradrawtask/004E82C0-TerraDrawTask_Init.h`
- **Terrain Spatial Chunk Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/discardterrain/0052C000-DiscardTerrain_LoadParameters.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/discardterrain/0052C000-DiscardTerrain_LoadParameters.h`

### Function Signatures & Calling Interface
- **`CreateTerrainTileMap` (`0x004C6760`)**:
  - Signature: `int32_t CreateTerrainTileMap(int32_t context, float scale);`
  - Allocates and links the 64-byte terrain tile descriptors at `0x00C30080`, binding tile index integers to material blend stages.
- **`DynObj_Createterrainmateria` (`0x004C6240`)**:
  - Signature: `int32_t DynObj_Createterrainmateria(void* terrainContext, int32_t basePathString);`
  - Maps `.bit` (offset `+0xDCC`), `.hmp` (offset `+0xDD0`), `.cmd` (offset `+0x08`), and `.ctr` (offset `+0x0C`) into the terrain subsystem context.
- **`0x00510010`** — **NAME UNVERIFIED** (`Terrain_RenderQuadtree` is a working label; catalog gate **FAIL**, score 0.667, `L1_cfg` false, confidence 4/6; REA has no symbol):
  - Signature: `int32_t (void* terrainContext);`
  - It *is* a real, observed function (143 basic blocks, `0x510010`–`0x5114F3`). The claim that it is the quadtree frustum culler is **inferred from the name only**, not from a decoded node structure — treat the internal algorithm as unknown until its callees are read.
- **`Terrain_Draw` (`0x00512C30`)**:
  - Signature: `int32_t __stdcall Terrain_Draw(float context, float p2, float p3, float p4, float p5, float p6, float p7, float p8, double p9, double p10, double p11, double p12);`
  - Issues Direct3D triangle strip draw calls for all visible terrain patches, blending multi-tile textures with the baked `.lmp` lightmap.

### Execution Sequence & How to Use
1. **Initialize TerraDraw Task**: Call `TerraDrawTask_Init` to register the terrain rendering lifecycle with the QTask manager.
2. **Mount Level 1 Terrain Base**: Invoke `DynObj_ParseConfigFile_7` with the base path `"LOCAL:levels/level01/terrain"` to unpack `terrain.tex` and `terrain.lmp`.
3. **Parse Elevation & Contour Hierarchy**: Call `DynObj_Createterrainmateria` passing the base path. This reads `terrain.hmp` into the elevation matrix and `terrain.ctr` into the quadtree node graph.
4. **Build Tile Material Map**: Execute `CreateTerrainTileMap` to establish multi-tile material assignments across grid coordinates.
5. **Setup World Partitioning**: Call `DiscardTerrain_LoadParameters` to register spatial chunks for dynamic streaming around the player.
6. **Per-Frame Render**: In the main render loop, execute `Terrain_RenderQuadtree` to cull invisible sectors and call `Terrain_Draw` to render textured terrain strips.

---

## 20. 2D/3D Billboard Sprite Pipeline & Draw Path (.SPR)

### Formats & Binary Specifications
- **`.spr` (Sprite Animation Container)**:
  - Stored inside virtual containers (e.g., `LOCAL:common/sprites/%s.spr`).
  - Used for foliage (trees, bushes), weapon muzzle flashes, bullet hit sparks, smoke puffs, and lens flares.
- **Header Layout**:
  - Offset `+0x00`: Frame Count (`uint16_t`).
  - Offset `+0x02`: Sprite Type Flags (`uint16_t`, 2D HUD icon vs 3D camera-facing billboard).
  - Offset `+0x04`: Bounding Sphere Radius (`float32`).
  - Offset `+0x08`: Hotspot / Anchor Offset `X`, `Y` (`float32[2]`, defines rotation/pivot center).
- **Frame Descriptors**: Each frame defines:
  - Width and Height in texture texels.
  - Sub-rectangle UV coordinates in the parent texture surface (`U0, V0, U1, V1`).
  - Blend Mode:
    - Normal Alpha Blending (`D3DBLEND_SRCALPHA`, `D3DBLEND_INVSRCALPHA`).
    - Additive Blending (`D3DBLEND_SRCALPHA`, `D3DBLEND_ONE`) for explosions, muzzle flashes, and fire.
- **Billboarding Algorithm**:
  - Quad vertices are dynamically generated on the CPU using the Camera Right vector (`CameraMatrix[0..2]`) and Camera Up vector (`CameraMatrix[4..6]`):
    - `V_TopLeft = Position - Right * (Width * 0.5) + Up * Height`
    - `V_TopRight = Position + Right * (Width * 0.5) + Up * Height`
    - `V_BottomLeft = Position - Right * (Width * 0.5)`
    - `V_BottomRight = Position + Right * (Width * 0.5)`

### Clean Native Locations
- **Level Sprite Archive Loader**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/004161D0-Level_LocalCommonSprites.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/004161D0-Level_LocalCommonSprites.h`
- **Sprite Subsystem Initializers**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/00416800-Level_InitSprites_2.cpp`
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/00416870-Level_InitSprites_3.cpp`
- **Offscreen Buffer Sprite Drawer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/0050EDA0-AnimSpline_InvokeOffscreenbuffersprit.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/0050EDA0-AnimSpline_InvokeOffscreenbuffersprit.h`
- **Translucent Context Primitive Renderer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497970-DisplayRender_ProcessTransContextPrim.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497970-DisplayRender_ProcessTransContextPrim.h`

### Function Signatures & Calling Interface
- **`Level_LocalCommonSprites` (`0x004161D0`)**:
  - Signature: `int32_t Level_LocalCommonSprites();`
  - Scans `LOCAL:common/sprites/` and loads all shared billboard sprites into global sprite cache `0x0057BAB8`.
- **`AnimSpline_InvokeOffscreenbuffersprit` (`0x0050EDA0`)**:
  - Signature: `int32_t AnimSpline_InvokeOffscreenbuffersprit(void* spriteCtx, int32_t frameIndex, void* positionVec);`
  - Computes the screen-space projection or camera-aligned billboard quad, applies transparency, and inserts the quad into the transparent draw list.
- **`DisplayRender_ProcessTransContextPrim` (`0x00497970`)**:
  - Signature: `int32_t DisplayRender_ProcessTransContextPrim(void* renderContext);`
  - Sorts translucent billboard quads back-to-front and flushes them to Direct3D with Z-write disabled (`D3DRENDERSTATE_ZWRITEENABLE = FALSE`).

### Execution Sequence & How to Use
1. **Initialize Sprite Subsystem**: Call `Level_InitSprites_2` and `Level_InitSprites_3` during game startup.
2. **Pre-Cache Level Sprites**: Execute `Level_LocalCommonSprites` to load shared billboard assets from `LOCAL:common/sprites/`.
3. **Queue Billboards During Scene Traversal**: When rendering foliage, particles, or weapon flashes, call `AnimSpline_InvokeOffscreenbuffersprit` with world position and frame ID.
4. **Back-to-Front Translucent Flush**: After all opaque models and terrain have been drawn, call `DisplayRender_ProcessTransContextPrim` to enable alpha blending and render billboard quads without corrupting the Z-buffer.

---

## 21. Direct3D 7 Device Setup, SwapChain & 3D Render Loop

### Formats & Architecture Specifications
- **DirectDraw 7 / Direct3D 7 Architecture**: Retail `IGI.EXE` renders through `IDirectDraw7` and `IDirect3DDevice7` (IID `IID_IDirect3DHALDevice`).
- **Surface Chain**:
  - Primary Surface (`DDSURFACEDESC2`, `DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX`).
  - Back Buffer Surface (`DDSCAPS_BACKBUFFER`), retrieved via `GetAttachedSurface(DDSCAPS_BACKBUFFER)`.
  - 16-bit / 24-bit Z-Buffer (`DDSCAPS_ZBUFFER`), created and attached to the back buffer via `AddAttachedSurface`.
- **Hardware Transform & Lighting (TnL)**:
  - World Transform: `lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld)`
  - View Transform: `lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView)`
  - Projection Transform: `lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj)`
- **Mandatory Direct3D 7 Render States**:
  - `D3DRENDERSTATE_ZENABLE = D3DZB_TRUE` (Enable depth buffering)
  - `D3DRENDERSTATE_ZWRITEENABLE = TRUE` (Write depth for opaque geometry)
  - `D3DRENDERSTATE_CULLMODE = D3DCULL_CCW` (Counter-clockwise backface culling)
  - `D3DRENDERSTATE_DITHERENABLE = TRUE` (16-bit color dithering)
  - `D3DRENDERSTATE_LIGHTING = TRUE` (Direct3D fixed-function lighting)

### Clean Native Locations
- **DirectDraw 7 Master Initializer**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004924D0-Display_InitDirectDraw.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004924D0-Display_InitDirectDraw.h`
- **Direct3D 7 Device Opener & Mode Selector**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490190-Display_OpenDevice.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490190-Display_OpenDevice.h`
- **Render Device Invoker & Context Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490120-DisplayRender_InvokeOpenDevice.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490120-DisplayRender_InvokeOpenDevice.h`
- **Render State Manager**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00493A60-DisplayRender_ProcessCouldntsetrender.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00493A60-DisplayRender_ProcessCouldntsetrender.h`
- **Hardware Device Enumerator**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/004927B0-DisplayRender_ProcessCouldntgetdevice.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/004927B0-DisplayRender_ProcessCouldntgetdevice.h`
- **Vertex Buffer Synchronizer & Waiter**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00492400-DisplayRender_ProcessCouldntwaitforvb.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00492400-DisplayRender_ProcessCouldntwaitforvb.h`
- **Surface Clear Dispatcher**:
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490360-Display_ClearSurfaces.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490360-Display_ClearSurfaces.h`
- **Device Loss & Surface Restorer** (corrected — `0x00490370` is `Timer_Now`, **not** `Display_RestoreDevice`):
  - Implementation: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490530-DisplayRender_InvokeRestoreDevice.cpp`
  - Header: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490530-DisplayRender_InvokeRestoreDevice.h`

### Function Signatures & Calling Interface
- **`0x004924D0`** — **NAME INFERRED** (`Display_InitDirectDraw` is a working label):
  - Real function: catalog gate PASS 1.0, `src = discovered-bn-r2`, 13 blocks, `0x4924D0`–`0x49266C`. **REA/Ghidra has no procedure at this address** and the binary carries no symbols, so the name is a label, not a proven symbol. Confirm the body before relying on the DirectDraw description below.
  - Claimed signature: `void* const __stdcall (int32_t* pThis);`
  - *Claimed (unverified):* creates the DirectDraw 7 COM instance, sets cooperative level, enumerates display modes.
- **`0x00490190`** — **NAME INFERRED** (`Display_OpenDevice` is a working label):
  - Real function: catalog gate PASS 1.0, `src = confirmed-rea`, 10 blocks, `0x490190`–`0x4901F2`. This is a **very small** function (98 bytes) — it is not a full device initialiser, so treat the description below as unconfirmed.
  - Claimed signature: `int32_t (int32_t context, int32_t param_2);`
- **`DisplayRender_ProcessCouldntsetrender` (`0x00493A60`)**:
  - Signature: `int32_t DisplayRender_ProcessCouldntsetrender(int32_t stateType, int32_t stateValue);`
  - Safe wrapper around `IDirect3DDevice7::SetRenderState` with error trapping.
- **`Display_ClearSurfaces` (`0x00490360`)**:
  - Signature: `int32_t Display_ClearSurfaces(int32_t flags, uint32_t colorRGB, float depthZ, uint32_t stencilVal);`
  - Clears the viewport color buffer and Z-buffer (`D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER`).

### Complete 3D Frame Render Loop Sequence
1. **Frame Synchronization**: Call `DisplayRender_ProcessCouldntwaitforvb` to guarantee GPU has completed the prior frame.
2. **Clear Viewport & Z-Buffer**: Call `Display_ClearSurfaces` with clear color and `depthZ = 1.0f`.
3. **Begin 3D Scene**: Invoke `IDirect3DDevice7::BeginScene()`.
4. **Set Camera Transforms**:
   - Compute Camera View Matrix from player eye position and forward/up vectors.
   - Compute Projection Matrix with FOV from `Camera_SetFieldOfView_8`.
   - Set transforms via `SetTransform(D3DTRANSFORMSTATE_VIEW)` and `SetTransform(D3DTRANSFORMSTATE_PROJECTION)`.
5. **Stage 1 - Render Terrain**: Traversal and draw via `Terrain_RenderQuadtree` and `Terrain_Draw`.
6. **Stage 2 - Render Opaque 3D Models**: Traversal of world entities, binding textures via `DisplayRender_Invoke2`, and rendering indexed primitives via `DisplayRender_Processmodelsnotconvert`.
7. **Stage 3 - Render Translucent Assets & Sprites**: Disable Z-write (`SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE)`), sort billboards back-to-front, and flush quads via `DisplayRender_ProcessTransContextPrim`. Re-enable Z-write.
8. **End 3D Scene**: Invoke `IDirect3DDevice7::EndScene()`.
9. **Present / Flip Surface**: In fullscreen mode, call `IDirectDrawSurface7::Flip`. In windowed mode, call `IDirectDrawSurface7::Blt` copying the back buffer to the primary surface.


---

## 22. Missing Pipeline Stages: Asset Decode, Texture Bind, Sprite Draw, Container IO

These are the stages the engine needs **before anything can be drawn**: the container reader, the MEF/texture decoders, the terrain tile pipeline, the sprite path, and the mipmap upload. Every address below was verified with `bn_function_info` against `igi.exe`.

### 17.1 Container / File IO (open the archives)

- **`Sys_Resource` (`0x004B5F80`)** — `int32_t* Sys_Resource(char* virtualPath, int32_t* mode, void* outHandle);` — resolves a virtual path (`LOCAL:`, `LANGUAGE:`, …) to a device and opens the stream.
- **`QFile_AliasResolve` (`0x004B1020`)** — `int32_t QFile_AliasResolve(char* alias, void* out, int32_t flags);` — expands the alias/device prefix (`LOCAL:models/...`).
- **`QFile_DeviceIndex` (`0x004B11A0`)** — `int32_t QFile_DeviceIndex(void* device);` / **`QFile_DeviceHandlerLookup` (`0x004B1620`)** — device-table lookup used by the resource layer.
- **`QFile_ReadWrite` (`0x004B1510`)** — `int32_t* QFile_ReadWrite(void* handle, char* mode);` — the read/write primitive every loader funnels through.
- **`File_Open` (`0x004A5350`)** — `int32_t* File_Open(PSTR path, char* mode);` — Win32 layer entry.
- **`Resource_Unpack` (`0x004B16D0`)** — `int32_t Resource_Unpack(int32_t* archive, int32_t index, int32_t flags);` — extracts one entry from the `ILFF`/`IRES` container.
- **`Resource_PackUnpack` (`0x004B18B0`)** — `void* Resource_PackUnpack(void* entry, int32_t* outSize);` — resolves an entry to its byte range.
- **`Resource_Unload` (`0x004B6380`)** — `int32_t Resource_Unload(int32_t* archive);`
- **`Resource_Load` (`0x004B5F00`)** — see §1; the high-level mount.

**Order:** `File_Open`/`QFile_AliasResolve` → `Sys_Resource` → `Resource_Unpack` → per-format decoder.

### 17.2 MEF Decode (vertex / index / material)

- **`DynObj_LocalModelsSMef` (`0x004CF390`)** — `int32_t* DynObj_LocalModelsSMef(int32_t dynObjHandle);` — loads `LOCAL:models/%s.mef`, reads the `HSEM` chunk (`0x4D455348`) and checks version `0x3DCCCCCD`; reads `ATTA`/`MVTX`/`PORT`/`PTFC`/`PTVX` into `+0x88/+0x8C/+0x90/+0x94/+0x98`; walks the `TXAN` (`0x4E415854`) list into a `count * 0x14` record array. **No vertex/index (XTRV/DNER) decoding happens here** — do not implement the vertex layout from this function (§17 note).
- **`DynObj_InsertHashItem_14` (`0x004CF080`)** — mounts `models/%s.res` + `textures/%s.res` and builds the hash node the decoder fills.
- **`DynObj_LocalTexturesSTex` (`0x004CF790`)** — `int32_t* DynObj_LocalTexturesSTex(void* dynObjHandle);` — decodes the paired `.TEX` stream.

### 17.3 Texture Decode, Mipmaps & Binding

- **`Terrain_BuildMipMaps` (`0x005125A0`)** — `int32_t Terrain_BuildMipMaps(void);` — the mip-chain builder (companions `_2` … `_11` cover the per-format variants).
- **`Render_BindTexture` (`0x004D09E0`)** — `void** Render_BindTexture(void** surface, void* palette, int32_t stage, char* name);` — uploads the decoded surface to the texture stage. This is the Direct3D 7 binding point; in the SDL3 + OpenGL port it becomes the GL texture upload.
- **`Render_DrawLightmapMesh` (`0x004D06C0`)** — `int32_t Render_DrawLightmapMesh(float scale, int32_t mesh);` — lightmap-mapped draw (the `.LMP` consumer).

### 17.4 Terrain Data Pipeline (`.HMP` / `.CTR` / `.BIT` / `.LMP`)

- **`DynObj_ParseConfigFile_7` (`0x004C5F00`)** — reads `<base>.tex` + `<base>.lmp`.
- **`DynObj_Createterrainmateria` (`0x004C6240`)** — `void* DynObj_Createterrainmateria(void* terrainContext, int32_t basePath);` — appends `.bit` / `.hmp` / `.cmd` / `.ctr`, then loads and runs the terrain `.qvm`.
- **`CreateTerrainMaterial` (`0x004C6670`)** — `int32_t CreateTerrainMaterial(int32_t materialId, float scale);` — the QVM-callable that materialises a terrain material.
- **`CreateTerrainTileMap` (`0x004C6760`)** — `int32_t CreateTerrainTileMap(int32_t tileMapId, float scale);` — builds the terrain tile map.
- **`Terrain_GenerateOctree` (`0x004D14E0`)** — `int32_t Terrain_GenerateOctree(void* terrainContext);` — builds the collision/LOD octree.
- **`Terrain_SampleHeightmap` (`0x00512040`)** / **`Terrain_CalculateNormals` (`0x005122B0`)** — `.HMP` sampler and normal generator.
- **`Terrain_RenderQuadtree` (`0x00510010`)** / **`Terrain_RenderWaterSurface` (`0x005116B0`)** — the draw pass.

**Order:** `DynObj_ParseConfigFile_7` → `DynObj_Createterrainmateria` → `CreateTerrainMaterial` + `CreateTerrainTileMap` → `Terrain_GenerateOctree` → `Terrain_RenderQuadtree`.

### 17.5 Sprite Decode & Draw (`.SPR` / `QSprite` / `Sprite3D`)

- **`Qsprite_Register4Az` (`0x004B53B0`)** — `int32_t Qsprite_Register4Az(int32_t* sprite, int32_t frameCount, int32_t format, ...);` — registers a sprite sheet from the resource archive.
- **`Sprite3D_LoadParameters` (`0x00531E10`)** — `int32_t Sprite3D_LoadParameters(float scale);`
- **`Sprite3D_Init` (`0x00531EF0`)** — `int32_t Sprite3D_Init(void);`
- **`Sprite3D_Update` (`0x00531F40`)** — `uint32_t Sprite3D_Update(void);` — billboard quad billboard/screen placement.
- **`Particle_Render3DSprites` (`0x00530470`)** — the transparent-pass draw (see §7).

### 17.6 What is still **not** covered here

The Windows host currently paints diagnostics via GDI; there is **no** render device loop yet. A native-only build cannot render until: (a) the container reader above is ported, (b) `DynObj_LocalModelsSMef` + `Terrain_BuildMipMaps`/`Render_BindTexture` produce client-side vertex/pixel data, (c) a SDL3 + OpenGL 3.3 device replaces `Render_BindTexture`'s D3D7 binding, and (d) `Render_DrawLightmapMesh` / `Terrain_RenderQuadtree` / `Sprite3D_Update` issue the draw calls. Audio is intentionally out of scope for this document.
| **Render** | `Display_InitDirectDraw` | `0x004924D0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004924D0-Display_InitDirectDraw.cpp` |
| **Render** | `Display_OpenDevice` | `0x00490190` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490190-Display_OpenDevice.cpp` |
| **Render** | `DisplayRender_InvokeOpenDevice` | `0x00490120` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490120-DisplayRender_InvokeOpenDevice.cpp` |
| **Render** | `DisplayRender_ProcessCouldntsetrender` | `0x00493A60` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00493A60-DisplayRender_ProcessCouldntsetrender.cpp` |
| **Render** | `DisplayRender_ProcessCouldntgetdevice` | `0x004927B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/004927B0-DisplayRender_ProcessCouldntgetdevice.cpp` |
| **Render** | `DisplayRender_ProcessCouldntwaitforvb` | `0x00492400` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00492400-DisplayRender_ProcessCouldntwaitforvb.cpp` |
| **Render** | `Display_ClearSurfaces` | `0x00490360` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490360-Display_ClearSurfaces.cpp` |
| **Render** | `DisplayRender_InvokeRestoreDevice` | `0x00490530` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00490530-DisplayRender_InvokeRestoreDevice.cpp` |
| **Timer** | `Timer_Now` | `0x00490370` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/00490370-Display_RestoreDevice.cpp` *(file name is stale; contents are `Timer_Now`)* |
| **Models** | `Display_MeshFormatNotSupport` | `0x0049B4B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/0049B4B0-Display_MeshFormatNotSupport.cpp` |
| **Models** | `DisplayRender_Processmodelsnotconvert` | `0x0049DF60` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/0049DF60-DisplayRender_Processmodelsnotconvert.cpp` |
| **Models** | `Physics_QvertexDrawstrip` | `0x00509930` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/physics/00509930-Physics_QvertexDrawstrip.cpp` |
| **Textures** | `Terrain_TextureFileNotOfRequ` | `0x005175A0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.cpp` |
| **Textures** | `DisplayRender_ProcessCouldntevicttext` | `0x00494700` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00494700-DisplayRender_ProcessCouldntevicttext.cpp` |
| **Textures** | `DisplayRender_Invoke2` | `0x00497920` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497920-DisplayRender_Invoke2.cpp` |
| **Textures** | `Display_CreateSurface` | `0x004903B0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/display/004903B0-Display_CreateSurface.cpp` |
| **Terrain** | `CreateTerrainTileMap` | `0x004C6760` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/create/004C6760-Create_TerrainTileMap.cpp` |
| **Terrain** | `Terrain_Draw` | `0x00512C30` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terrain/00512C30-Terrain_Draw.cpp` |
| **Terrain** | `TerraDrawTask_Init` | `0x004E82C0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terradrawtask/004E82C0-TerraDrawTask_Init.cpp` |
| **Terrain** | `TerraDrawTask_Update` | `0x004E8350` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/terradrawtask/004E8350-TerraDrawTask_Update.cpp` |
| **Sprites** | `Level_LocalCommonSprites` | `0x004161D0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/004161D0-Level_LocalCommonSprites.cpp` |
| **Sprites** | `Level_InitSprites_2` | `0x00416800` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/00416800-Level_InitSprites_2.cpp` |
| **Sprites** | `Level_InitSprites_3` | `0x00416870` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/level/00416870-Level_InitSprites_3.cpp` |
| **Sprites** | `AnimSpline_InvokeOffscreenbuffersprit` | `0x0050EDA0` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/animspline/0050EDA0-AnimSpline_InvokeOffscreenbuffersprit.cpp` |
| **Sprites** | `DisplayRender_ProcessTransContextPrim` | `0x00497970` | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/natives/CLEAN/displayrender/00497970-DisplayRender_ProcessTransContextPrim.cpp` |



---

---

## 23. Verified Native Inventory — MVP Subsystems

Requested areas: **human AI, weapons & shooting, health, movement/jump, AI graphs & navigation, terrain, physics, script engine, file formats**.

- **Source of truth:** `IGINatives_VERIFIED.json` (5,732 natives) joined with `DISCOVERED_NATIVES_RECORD.json` subsystem tags; `gate`/`score` are the catalog's own CFG-gate fields.
- **Sampling:** a random sample of 36 entries across these tables was re-checked with `bn_function_info` against `igi.exe` — **36/36 matched**.
- **CLEAN coverage:** all 5,732 catalog addresses have a `CLEAN/**/*.c` + `*.h` (verified: 0 missing). Folder per row below.
- **Rows marked not PASS are listed separately** and must be re-verified before use.
- These tables are the *inventory* (what exists, where). For *how to call them*, see the per-subsystem sections above.

### 23.1 Human AI
- **Verified natives:** 613 total — **599 gate PASS**, 14 not PASS
- **CLEAN folders:** `ai`, `humanai`, `aifunction`, `combatstate`, `aiaction`, `aiconfig`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x00406450` | `HumanAIConfig_Init` | PASS | `int32_t*()` |
| `0x00406B70` | `AIConfig_ProcessStep_406B70` | PASS | `int32_t(void* arg1, int32_t* arg2)` |
| `0x00406D00` | `AIConfig_CreateHumanTask` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x00406D10` | `HumanAIConfig_OnEvent` | PASS | `int32_t(void* arg1)` |
| `0x00406D90` | `AIConfig_InitItem` | PASS | `uint32_t()` |
| `0x00406DF0` | `AIConfig_UnableToCreateHumana` | PASS | `void*(void* arg1)` |
| `0x00407D00` | `AIConfig_InitItem_2` | PASS | `uint32_t()` |
| `0x0044C8D0` | `HumanAI_Init` | PASS | `void*()` |
| `0x0044CCA0` | `AIAction_Patrol_2` | PASS | `void*()` |
| `0x0044D0E0` | `AIAction_Patrol` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D160` | `AIAction_Combat` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D260` | `AIAction_Dead` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D2C0` | `AIAction_FallFlat` | PASS | `int32_t(long double arg1 @ st0, int32_t arg2, ` |
| `0x0044D420` | `AIAction_Activate` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D4A0` | `AIAction_WalkToNode` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D510` | `AIAction_RunToNode` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D580` | `AIAction_FireAtNode` | PASS | `int32_t(long double arg1 @ st0, long double ar` |
| `0x0044D630` | `AIAction_FireAtTask` | PASS | `int32_t(long double arg1 @ st0, long double ar` |
| `0x0044D6E0` | `AIAction_PlayAnimation` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D750` | `AIAction_PlaySound` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D7D0` | `AIAction_MoveToEvent` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x0044D870` | `AIAction_LookAtEvent` | PASS | `int32_t(long double arg1 @ st0, int32_t arg2, ` |
| `0x0044D8F0` | `AIAction_Stunned` | PASS | `int32_t(long double arg1 @ st0, int32_t arg2, ` |
| `0x0044D960` | `AIAction_KickGrenade` | PASS | `int32_t(int32_t arg1, int32_t arg2 @ 0xc)` |
| `0x0044DA00` | `AIAction_RunPanicking` | PASS | `int32_t(long double arg1 @ st0, int32_t arg2, ` |
| `0x0044DA80` | `AIAction_Idle` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |

- **Not gate-PASS (verify before use):** `AIConfig_ApplyProfile`, `HumanAI_LoadParameters`, `AI_FireWeapon_3`, `AI_CombatTarget_17`, `AI_FireWeapon_20`, `AI_UpdateOrders_22`, `AI_FireWeapon_23`, `AI_MoveToNode_23`, `AI_TakeCover_23`, `AI_AlertAllies_23`, `AI_UpdateSensors_23`, `AI_UpdateOrders_26`

### 23.2 Weapons & Shooting
- **Verified natives:** 277 total — **263 gate PASS**, 14 not PASS
- **CLEAN folders:** `weapon`, `weapongun`, `gun`, `weapontype`, `addammo`, `mine`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x004148F0` | `WeaponType_InitType` | PASS | `int32_t(int32_t arg1 @ 0xc, int32_t arg2, doub` |
| `0x00414E20` | `WeaponType_UpdateAnim` | PASS | `double*(double* arg1, double* arg2, int32_t ar` |
| `0x00414F40` | `WeaponType_InitType_2` | PASS | `int32_t(int32_t arg1, void* arg2, int32_t arg3` |
| `0x00427830` | `RocketPod_Init` | PASS | `void*()` |
| `0x00427940` | `RocketPod_LoadParameters` | PASS | `uint32_t(int32_t* arg1, float arg2)` |
| `0x00427A70` | `RocketPod_Setup` | PASS | `int32_t(void* arg1)` |
| `0x00427FF0` | `Missile_Init` | PASS | `int32_t*()` |
| `0x00428CA0` | `Missile_Setup` | PASS | `void*(void* arg1)` |
| `0x00429540` | `Missile_Process` | PASS | `int32_t*(void* arg1)` |
| `0x00429730` | `Missile_Update` | PASS | `char*(long double arg1 @ st0, long double arg2` |
| `0x004298E0` | `Missile_OnEvent` | PASS | `int32_t(int32_t* arg1 @ 0x8)` |
| `0x00429920` | `Missile_Action` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x0045E3D0` | `Human_Create` | PASS | `char*(char* arg1 @ 0x8)` |
| `0x0046D420` | `AmmoPickup_Init` | PASS | `int32_t*()` |
| `0x0046D5F0` | `AmmoPickup_LoadParameters` | PASS | `int32_t(int32_t* arg1)` |
| `0x0046D7C0` | `PickupTrigger_InvokeInit_46D7C0` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x0046D7D0` | `PickupTrigger_InvokeInit_46D7D0` | PASS | `int32_t*(float arg1)` |
| `0x0046D8D0` | `AmmoPickup_Process` | PASS | `int32_t(void* arg1)` |
| `0x0046D930` | `AmmoPickup_Update` | PASS | `int32_t(void* arg1)` |
| `0x0046D990` | `AmmoPickup_OnEvent` | PASS | `int32_t(void* arg1, int32_t* arg2)` |
| `0x0046DB40` | `AddAmmo_Init` | PASS | `int32_t*()` |
| `0x0046DC40` | `AddAmmo_LoadParameters` | PASS | `int32_t(int32_t* arg1)` |
| `0x0046DCA0` | `GunX2_Init` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x0046DCB0` | `GunX2_Setup` | PASS | `void*(void* arg1)` |
| `0x0046DCE0` | `AddAmmo_OnEvent` | PASS | `uint32_t()` |
| `0x0046DD00` | `Gunx2_Init` | PASS | `void*()` |

- **Not gate-PASS (verify before use):** `Weapon_ConfigRead`, `WeaponType_Open`, `Weapon_EjectMagazine`, `Weapon_UpdateZoom`, `Weapon_UpdateMuzzleFlash_2`, `Weapon_UpdateRecoil_3`, `Weapon_FireBullet_4`, `GunClip_Action`, `Weapon_UpdateZoom_5`, `Weapon_FireUpdate`, `Weapon_CycleFireMode_6`, `Weapon_ComputeAccuracy_7`

### 23.3 Health & Damage
- **Verified natives:** 46 total — **42 gate PASS**, 4 not PASS
- **CLEAN folders:** `human`, `soldier`, `medipack`, `hitzone`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x00412440` | `Human_DamageApply` | PASS | `float(float arg1, int32_t* arg2) __location("e` |
| `0x00416D80` | `Human_HitDamage` | PASS | `void*()` |
| `0x00427D90` | `HitZone_Init` | PASS | `void*()` |
| `0x0045C440` | `Soldier_Execute` | PASS | `void*(void* arg1, int32_t* arg2)` |
| `0x0045D3C0` | `Human_DamageHandler` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x0045E3E0` | `Human_Action` | PASS | `void(void* arg1, char* arg2)` |
| `0x0045E450` | `Human_Init` | PASS | `int32_t()` |
| `0x0045F050` | `Human_Setup` | PASS | `int32_t*(float arg1, int32_t arg2)` |
| `0x0045F280` | `Human_LoadParameters` | PASS | `uint32_t(float arg1, int32_t arg2)` |
| `0x0045F500` | `Human_Process` | PASS | `char*(float arg1, int32_t* arg2)` |
| `0x0045F800` | `Human_Update` | PASS | `char*(float arg1, int32_t* arg2)` |
| `0x0045F890` | `Human_OnEvent` | PASS | `float*(float arg1)` |
| `0x0045F930` | `Human_BaseDamage` | PASS | `int32_t(void* arg1, int32_t* arg2)` |
| `0x0045F9D0` | `Human_Cleanup` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x0045FA80` | `Human_Cleanup_2` | PASS | `void*(void* arg1, int32_t* arg2)` |
| `0x00460280` | `Human_Cleanup_5` | PASS | `double*(float arg1 @ 0x8)` |
| `0x004606F0` | `Human_Cleanup_6` | PASS | `int32_t*(void* arg1, int32_t* arg2)` |
| `0x00460850` | `Human_Cleanup_7` | PASS | `int32_t(void* arg1)` |
| `0x004608D0` | `Human_Cleanup_8` | PASS | `int32_t(void* arg1)` |
| `0x00460950` | `Human_Cleanup_9` | PASS | `int32_t(void* arg1, int32_t arg2)` |
| `0x004609A0` | `Human_Cleanup_10` | PASS | `int32_t(void* arg1, int32_t arg2)` |
| `0x00460AC0` | `Human_Cleanup_11` | PASS | `int32_t(void* arg1)` |
| `0x00462390` | `Human_GroundQuery` | PASS | `int32_t*(void* arg1, double* arg2, int32_t* ar` |
| `0x00470410` | `Medipack_Init` | PASS | `void*()` |
| `0x00470570` | `Medipack_Update` | PASS | `void*(int32_t* arg1 @ 0x8)` |
| `0x00470600` | `Medipack_OnEvent` | PASS | `uint32_t()` |

- **Not gate-PASS (verify before use):** `Human_GetCutSceneDeltaTime`, `Human_Cleanup_3`, `Human_Cleanup_4`, `Human_SetParent`

### 23.4 Movement & Jump
- **Verified natives:** 258 total — **251 gate PASS**, 7 not PASS
- **CLEAN folders:** `humanplayer`, `human`, `ladder`, `humanplayerinput`, `humanview`, `humancamera`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x004082B0` | `HumanPlayer_LoadParameters_2` | PASS | `uint32_t()` |
| `0x00408320` | `HumanPlayer_UpdateMotion` | PASS | `int32_t()` |
| `0x00408330` | `HumanPlayer_CheckGround` | PASS | `int32_t()` |
| `0x00408340` | `HumanPlayer_UpdateStamina` | PASS | `int32_t()` |
| `0x00408390` | `HumanPlayer_UpdateHealth` | PASS | `int32_t(int32_t arg1)` |
| `0x004087B0` | `HumanPlayer_ExecutePipeline_4087B0` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00408A40` | `HumanPlayer_TakeDamage` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00408AA0` | `HumanPlayer_UpdateAnimation` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00408C30` | `HumanPlayer_CheckCollision` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00408DF0` | `HumanPlayer_LoadParameters_3` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x00408E70` | `HumanPlayer_UpdateMotion_2` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00408FE0` | `HumanPlayer_CheckGround_2` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x00409090` | `HumanPlayer_UpdateStamina_2` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409220` | `HumanPlayer_UpdateHealth_2` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x00409660` | `HumanPlayer_TakeDamage_2` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x004097F0` | `HumanPlayer_UpdateAnimation_2` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409850` | `HumanPlayer_CheckCollision_2` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409A80` | `HumanPlayer_ExecutePipeline_409A80` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409C10` | `HumanPlayer_LoadParameters_4` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x00409C90` | `HumanPlayer_ExecutePipeline_409C90` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409E00` | `HumanPlayer_UpdateMotion_3` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x00409E80` | `HumanPlayer_ExecutePipeline_409E80` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x00409FF0` | `HumanPlayer_CheckGround_3` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x0040A090` | `HumanPlayer_ExecutePipeline_40A090` | PASS | `int32_t(void* arg1, void* arg2)` |
| `0x0040A220` | `HumanPlayer_UpdateStamina_3` | PASS | `int32_t(void* arg1, float arg2)` |
| `0x0040A2A0` | `HumanPlayer_ExecutePipeline_40A2A0` | PASS | `int32_t(void* arg1, void* arg2)` |

- **Not gate-PASS (verify before use):** `HumanPlayer_HandleInput`, `HumanPlayer_CheckGround_6`, `HumanPlayer_CheckGround_11`, `Human_GetCutSceneDeltaTime`, `Human_Cleanup_3`, `Human_Cleanup_4`, `Human_SetParent`

### 23.5 AI Graphs & Navigation
- **Verified natives:** 49 total — **46 gate PASS**, 3 not PASS
- **CLEAN folders:** `patrolpath`, `graph`, `aigraph`, `patrolpathcommand`, `splinepathnodeqtask`, `splinepathdyncubeobj`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x004533B0` | `PatrolPathCommand_Init` | PASS | `int32_t*()` |
| `0x00453470` | `PatrolPath_Create` | PASS | `void*(void* arg1)` |
| `0x00453480` | `PatrolPath_Init_2` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x00453490` | `PatrolPath_Update_2` | PASS | `int32_t(void* arg1)` |
| `0x004534B0` | `PatrolPathCommand_Update` | PASS | `void(void* arg1)` |
| `0x00453520` | `PatrolPathCommand_OnEvent` | PASS | `int32_t()` |
| `0x00453540` | `PatrolPathCommand_Process` | PASS | `int32_t(void* arg1)` |
| `0x00453600` | `PatrolPath_Update` | PASS | `void*()` |
| `0x004536E0` | `PatrolPath_OnEvent` | PASS | `int32_t(void* arg1)` |
| `0x00453710` | `PatrolPath_Process` | PASS | `uint32_t()` |
| `0x00453720` | `PatrolPath_Action` | PASS | `int32_t(void* arg1)` |
| `0x00453730` | `PatrolPath_OnTick` | PASS | `int32_t*(int32_t* arg1)` |
| `0x00453840` | `PatrolPath_Find` | PASS | `void* const(int32_t arg1)` |
| `0x0048E920` | `Patrol_Tick` | PASS | `int32_t*(int32_t* arg1, int32_t* arg2)` |
| `0x004F90B0` | `Graph_LoadParameters` | PASS | `int32_t*(void* arg1)` |
| `0x004F9160` | `Graph_Process` | PASS | `void*(void* arg1)` |
| `0x004F9180` | `Graph_Update` | PASS | `int32_t(void* arg1)` |
| `0x004FAA60` | `Graph_Init` | PASS | `void*()` |
| `0x004FAE50` | `Graph_OnEvent` | PASS | `int32_t(void* arg1, int32_t* arg2)` |
| `0x004FAEE0` | `Graph_Action` | PASS | `int32_t*(int32_t* arg1)` |
| `0x004FAFE0` | `Graph_Cleanup` | PASS | `int32_t __convention("regparm")(int32_t arg1, ` |
| `0x004FB030` | `Graph_Cleanup_2` | PASS | `int32_t*(int32_t* arg1)` |
| `0x004FB0D0` | `Graph_Cleanup_3` | PASS | `int32_t*(int32_t* arg1)` |
| `0x00508E30` | `AIGraph_Init_2` | PASS | `int32_t()` |
| `0x00508FA0` | `AIGraph_Init` | PASS | `void*()` |
| `0x005092A0` | `AIGraph_Cleanup` | PASS | `char*(int32_t* arg1)` |

- **Not gate-PASS (verify before use):** `PatrolPath_Init`, `SplinePathNodeQTask_LoadParameters`, `SplinePathDynCubeObj_OnEvent`

### 23.6 Terrain
- **Verified natives:** 183 total — **168 gate PASS**, 15 not PASS
- **CLEAN folders:** `terrain`, `physicsterrain`, `discardterrain`, `heightmap`, `discardterrainarea`, `terradrawtask`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x004C6760` | `CreateTerrainTileMap` | PASS | `int32_t(int32_t arg1, float arg2 @ 0xc)` |
| `0x004E82C0` | `TerraDrawTask_Init` | PASS | `int32_t()` |
| `0x004E8350` | `TerraDrawTask_Update` | PASS | `uint32_t()` |
| `0x004E8370` | `TerraDrawTask_OnEvent` | PASS | `struct status_msg*(void* arg1, uint32_t arg2)` |
| `0x004EE540` | `PhysicsTerrain_ProcessLOCALgrab` | PASS | `int32_t()` |
| `0x004EE5E0` | `PhysicsTerrain_ProcessLOCALgrab_4EE5E0` | PASS | `int32_t(void* arg1)` |
| `0x004EF830` | `PhysicsTerrain_InvokeLoadParameters` | PASS | `int32_t*(int32_t arg1, int32_t* arg2)` |
| `0x004EFC10` | `PhysicsTerrain_Processfffffffff` | PASS | `int32_t(float* arg1, void* arg2)` |
| `0x004EFD50` | `PhysicsTerrain_ExecutePipeline_4EFD50` | PASS | `int32_t(float arg1, float arg2)` |
| `0x004F0580` | `PhysicsTerrain_ExecutePipeline_4F0580` | PASS | `void*(void* arg1)` |
| `0x004F0620` | `PhysicsTerrain_ProcessStep_4F0620` | PASS | `int32_t(int32_t arg1)` |
| `0x004F0640` | `PhysicsTerrain_ProcessStep_4F0640` | PASS | `int32_t()` |
| `0x004F0660` | `PhysicsTerrain_ExecutePipeline_4F0660` | PASS | `void*(void* arg1)` |
| `0x00506FA0` | `HeightMap_Init` | PASS | `int32_t*()` |
| `0x00507340` | `HeightMap_LoadParameters` | PASS | `void*(void* arg1)` |
| `0x00507410` | `HeightMap_Setup` | PASS | `int32_t(int32_t* arg1)` |
| `0x00511500` | `Terrain_UpdateWaterWaves` | PASS | `int32_t()` |
| `0x005127E0` | `Terrain_UpdateWaterWaves_2` | PASS | `int32_t(void* arg1)` |
| `0x00516B20` | `Terrain_UpdateWaterWaves_4` | PASS | `char*(char* arg1, char* arg2, char* arg3, char` |
| `0x00517510` | `Terrain_UpdateWaterWaves_5` | PASS | `int32_t(char* arg1)` |
| `0x00517980` | `Terrain_UpdateWaterWaves_6` | PASS | `int32_t*(int32_t* arg1, int32_t arg2)` |
| `0x00517F00` | `Terrain_UpdateWaterWaves_7` | PASS | `int32_t*(int32_t* arg1, int32_t arg2, int32_t*` |
| `0x00518250` | `Terrain_UpdateWaterWaves_8` | PASS | `int32_t(int32_t arg1)` |
| `0x00519650` | `Terrain_UpdateWaterWaves_9` | PASS | `uint32_t()` |
| `0x0052B620` | `DiscardTerrainArea_Create` | PASS | `int32_t*(int32_t* arg1 @ 0x8)` |
| `0x0052B630` | `DiscardTerrainArea_Init_2` | PASS | `void*(int32_t arg1, void* arg2)` |

- **Not gate-PASS (verify before use):** `Terrain_RenderQuadtree`, `Terrain_SampleHeightmap`, `Terrain_CalculateNormals`, `Terrain_SelectTerrainLOD_2`, `Terrain_Draw`, `Terrain_BuildMipMaps_2`, `Terrain_UpdateWaterWaves_3`, `Terrain_BuildMipMaps_4`, `Terrain_RenderWaterSurface_8`, `Terrain_CalculateNormals_8`, `Terrain_BuildMipMaps_8`, `Terrain_UpdateWaterWaves_10`

### 23.7 Physics Engine
- **Verified natives:** 190 total — **164 gate PASS**, 26 not PASS
- **CLEAN folders:** `physics`, `physicsrigid`, `physicsobj`, `rigiddyncubeobj`, `genericphysicsmagicobj`, `moverigidobj`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x0042DA70` | `PhysicsObj_Setup` | PASS | `void*(int32_t* arg1 @ 0x8)` |
| `0x00438DA0` | `RotatingObject_Setup` | PASS | `char*(void* arg1)` |
| `0x00438DF0` | `RotatingObject_Init` | PASS | `int32_t*()` |
| `0x00439090` | `RotatingObject_LoadParameters` | PASS | `void*(int80_t arg1 @ st0, float arg2)` |
| `0x004395D0` | `RotatingObject_Process` | PASS | `int32_t(float arg1)` |
| `0x0043E4F0` | `GenericPhysicsMagicObj_Init` | PASS | `void*()` |
| `0x0043E580` | `GenericPhysicsMagicObj_LoadParameters` | PASS | `float*(float arg1)` |
| `0x0043E600` | `GenericPhysicsMagicObj_OnEvent` | PASS | `float*(float arg1)` |
| `0x0043E650` | `GenericPhysicsMagicObj_Setup` | PASS | `void*(void* arg1)` |
| `0x0043E670` | `GenericPhysicsMagicObj_Process` | PASS | `void*(void* arg1, int32_t* arg2)` |
| `0x0043E7A0` | `GenericPhysicsMagicObj_Update` | PASS | `void*(void* arg1, void* arg2)` |
| `0x0043E8D0` | `GenericPhysicsObj_Init` | PASS | `void*()` |
| `0x0043EAB0` | `GenericPhysicsObj_Update` | PASS | `void*(float arg1)` |
| `0x0043EBA0` | `GenericPhysicsObj_LoadParameters` | PASS | `float*(float arg1)` |
| `0x0043EC80` | `GenericPhysicsObj_Process` | PASS | `int32_t(void* arg1)` |
| `0x0043EDF0` | `GenericPhysicsObj_Update_2` | PASS | `void*(void* arg1, int32_t* arg2)` |
| `0x004A01B0` | `RigidDynCubeObj_LoadParameters` | PASS | `int32_t(void* arg1)` |
| `0x004C1840` | `RigidDynCubeObj_Setup` | PASS | `int32_t*(void* arg1, int32_t* arg2)` |
| `0x004C3010` | `RigidDynCubeObj_Action` | PASS | `void(int32_t* arg1, float arg2)` |
| `0x004C31F0` | `RigidDynCubeObj_Cleanup` | PASS | `void*()` |
| `0x004C3450` | `RigidDynCubeObj_Init` | PASS | `uint32_t()` |
| `0x004C35F0` | `RigidDynCubeObj_OnEvent` | PASS | `void*(void* arg1, int32_t* arg2)` |
| `0x004C4660` | `RigidDynCubeObj_Cleanup_2` | PASS | `void(int32_t* arg1, float arg2)` |
| `0x004C4780` | `RigidDynCubeObj_Cleanup_3` | PASS | `int32_t(float arg1, double* arg2)` |
| `0x004D5180` | `PhysicsRigid_ExecutePipeline_4D5180` | PASS | `int32_t(int32_t* arg1, int32_t* arg2, int32_t*` |
| `0x004D57C0` | `PhysicsRigid_ProcessStep_4D57C0` | PASS | `int32_t(int32_t arg1)` |

- **Not gate-PASS (verify before use):** `ForceAccumulator_Add`, `PhysicsObj_LoadParameters`, `Physics_EvaluateAreaCriteria`, `Physics_CheckHullCollision_2`, `Physics_ApplyImpulse_6`, `Physics_InterpolateWaypoint_7`, `Physics_EvaluateAreaCriteria_7`, `Physics_ApplyImpulse_7`, `Physics_UpdateForces_7`, `Physics_CheckHullCollision_9`, `Physics_ApplyImpulse_9`, `Physics_QvertexDrawstrip`

### 23.8 Script Engine (QVM/QTask)
- **Verified natives:** 385 total — **352 gate PASS**, 33 not PASS
- **CLEAN folders:** `script`, `qtask`, `qvm`, `smoothqtask`, `noiseqtask`, `qhash`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x00401000` | `QTask_CreateTask` | PASS | `int32_t()` |
| `0x004010D0` | `QTask_Init` | PASS | `void*()` |
| `0x004011B0` | `QTask_Update` | PASS | `int32_t*()` |
| `0x004011D0` | `QTask_OnEvent` | PASS | `char*()` |
| `0x00401230` | `QTask_Action` | PASS | `void()` |
| `0x00401270` | `QTask_OnTick` | PASS | `void()` |
| `0x00401780` | `QTask_CleanupTypes` | PASS | `int32_t*(void* arg1)` |
| `0x00401860` | `QTask_CleanupEvents` | PASS | `uint32_t(int16_t arg1, char arg2)` |
| `0x00401890` | `QTask_ProcessQueue` | PASS | `int32_t(int16_t arg1, int32_t arg2)` |
| `0x004018C0` | `QTask_InitContext` | PASS | `uint32_t(char arg1)` |
| `0x004018E0` | `QTask_CreateTask_2` | PASS | `uint32_t(int32_t arg1)` |
| `0x00401B20` | `Qtask_UpdateList` | PASS | `int32_t(int32_t* arg1)` |
| `0x00401D80` | `QTask_ProcessQueue_2` | PASS | `int32_t(void* arg1, int32_t arg2, int32_t arg3` |
| `0x00401E30` | `QTask_InitContext_2` | PASS | `void*(void* arg1, int32_t arg2, int32_t arg3)` |
| `0x00401E70` | `QTask_CreateTask_3` | PASS | `int32_t(void* arg1, int32_t arg2, int32_t arg3` |
| `0x00402070` | `QTask_ProcessLOCALconfigqsc` | PASS | `int32_t(struct frames_ctx* arg1)` |
| `0x00402740` | `QTask_ProcessLOCALconfigqsc_402740` | PASS | `int32_t()` |
| `0x004B0D60` | `Qhash_Init` | PASS | `void*(char arg1)` |
| `0x004B1AC0` | `QScript_Cleanup` | PASS | `int32_t(void* arg1)` |
| `0x004B83D0` | `Qvm_Cleanup` | PASS | `int32_t(int32_t* arg1)` |
| `0x004B85B0` | `Qvm_Execute` | PASS | `int32_t(void* arg1)` |
| `0x004BC950` | `Qvm_ProcessInstructionnotim` | PASS | `void() __noreturn` |
| `0x004BECB0` | `Qvm_ProcessPUSH` | PASS | `int32_t(int32_t arg1, int32_t arg2 @ 0xc)` |
| `0x004BEFE0` | `Qvm_ExecutePipeline_4BEFE0` | PASS | `int32_t(int32_t* arg1)` |
| `0x004C0200` | `Script_BufInit` | PASS | `int32_t*(int32_t* arg1, int32_t arg2, int32_t ` |
| `0x004C0560` | `Symbol_CheckRegister` | PASS | `int32_t(int32_t* arg1, int32_t* arg2, int32_t ` |

- **Not gate-PASS (verify before use):** `QTask_CloseEvent`, `QTask_DispatchEvent`, `QTask_Process`, `QTask_CleanupTypes_2`, `QTask_CleanupEvents_2`, `QTask_InvokeRestoreDevice`, `QFile_AliasResolve`, `Qsprite_Register4Az`, `Script_FormatString_7`, `Script_UnableToAllocatePara`, `Script_ParseEnumString_8`, `Script_FormatString_12`

### 23.9 File Formats / Loaders
- **Verified natives:** 178 total — **159 gate PASS**, 19 not PASS
- **CLEAN folders:** `dynobj`, `magicobj`, `bonedyncubeobj`, `polylistdyncubeobj`, `resource`, `dyncubeobj`

| Address | Native | Gate | Proto |
|---|---|---|---|
| `0x0049A350` | `PolyListDynCubeObj_Setup` | PASS | `void*(void* arg1)` |
| `0x0049A3C0` | `PolyListDynCubeObj_LoadParameters` | PASS | `void**(void* arg1)` |
| `0x0049A520` | `PolyListDynCubeObj_Init` | PASS | `int32_t()` |
| `0x0049A9B0` | `PolyListDynCubeObj_Process` | PASS | `int32_t(int32_t arg1 @ ebp, void* arg2)` |
| `0x004C0240` | `DynObj_CreateMagicObject` | PASS | `int32_t(int32_t* arg1)` |
| `0x004C0260` | `DynObj_UpdateDynTree` | PASS | `int32_t(int32_t* arg1, int32_t arg2, int32_t a` |
| `0x004C0620` | `DynObj_CreateMagicObject_2` | PASS | `int32_t(int32_t* arg1, int32_t* arg2, char* ar` |
| `0x004C0680` | `DynObj_UpdateDynTree_2` | PASS | `void*(void* arg1, char* arg2, int32_t arg3, in` |
| `0x004C0E60` | `DynObj_CreateMagicObject_3` | PASS | `int32_t(int32_t* arg1, int32_t* arg2, char arg` |
| `0x004C10E0` | `DynObj_UpdateDynTree_3` | PASS | `int32_t()` |
| `0x004C1830` | `DynObj_CreateMagicObject_4` | PASS | `int32_t(void* arg1)` |
| `0x004C3440` | `DynObj_UpdateDynTree_4` | PASS | `int32_t(int32_t arg1) __pure` |
| `0x004C48D0` | `DynObj_CreateMagicObject_5` | PASS | `int32_t(void* arg1, int32_t arg2)` |
| `0x004C5800` | `DynObj_CreateMagicObject_6` | PASS | `int32_t()` |
| `0x004C5830` | `DynObj_UpdateDynTree_6` | PASS | `int32_t()` |
| `0x004C5CE0` | `DynObj_CreateMagicObject_7` | PASS | `int32_t(int32_t* arg1)` |
| `0x004C6160` | `DynObj_UpdateDynTree_7` | PASS | `int32_t(void* arg1)` |
| `0x004C6E70` | `DynObj_UpdateDynTree_8` | PASS | `int32_t(void* arg1, void* arg2, double* arg3, ` |
| `0x004CB050` | `DynObj_CreateMagicObject_10` | PASS | `int32_t(void* arg1, int32_t* arg2, double* arg` |
| `0x004CB9F0` | `DynObj_UpdateDynTree_10` | PASS | `int32_t*(int32_t arg1, int32_t* arg2, double* ` |
| `0x004CE090` | `DynObj_CreateMagicObject_11` | PASS | `uint32_t(int32_t arg1, char arg2, int32_t arg3` |
| `0x004CE3D0` | `DynObj_UpdateDynTree_11` | PASS | `void*(void* arg1)` |
| `0x004CE830` | `DynObj_CreateMagicObject_12` | PASS | `int32_t*(int32_t* arg1, int32_t arg2)` |
| `0x004CEA10` | `DynObj_UpdateDynTree_12` | PASS | `int32_t()` |
| `0x004CEAA0` | `DynObj_CreateMagicObject_13` | PASS | `void*(void* arg1, int32_t arg2, int32_t* arg3)` |
| `0x004CEEA0` | `DynObj_UpdateDynTree_13` | PASS | `int32_t()` |

- **Not gate-PASS (verify before use):** `PolyListDynCubeObj_OnEvent`, `Resource_Unpack`, `DynObj_ParseConfigFile`, `DynObj_LinkDynamicCube`, `DynObj_LookupHashItem_6`, `DynObj_LookupHashItem_7`, `DynObj_Createterrainmateria`, `DynObj_CreateMagicObject_9`, `DynObj_RemoveSymbol_9`, `DynObj_RegisterSymbol_10`, `DynObj_LookupHashItem_11`, `DynObj_LookupHashItem_12`


---

## 24. MVP Build Order — Step-to-Native Mapping and Verification

This maps the ordered Level 1 build steps (see [plan.md](plan.md) / [MVP.md](MVP.md)) to the verified retail natives.
**Method:** every native was checked with `bn_function_info` (Binary Ninja MCP) against the retail `igi.exe`, then joined to `IGINatives_VERIFIED.json` for gate status.

**Result: 3,050 unique natives — 3,035 name-verified, 15 name conflicts, 0 lookup errors.**
Full per-native table: `assets/BinaryNinja/natives/CLEAN/VERIFICATION.md`.

| Step | Native areas | Unique | Name-verified |
|---|---|---|---|
| 2 Models + exact positions | `dynobj`, `resource`, `magicobj`, `rigid`, `physicsobj`, `origo`, `vec3`, `mat33` | 187 | 187 |
| 3 Terrain + textures | `terrain`, `physicsterrain`, `discardterrain`, `terradrawtask`, `heightmap`, `create` | 187 | 187 |
| 5 Placement verification | `origo`, `vec3`, `mat33`, `splineobj`, `worldobject` | 33 | 33 |
| 6 Texture verification | `dynobj`, `mesh`, `material`, `mipmapcontrol`, `render`, `display` | 514 | 514 |
| 7 Player / simulation | `humanplayer`, `locomotion`, `body`, `bone`, `humancamera`, `ladder`, `collision` | 221 | 221 |
| 8 Physics + interaction | `physics*`, `rigid`, `forceaccumulator` | 190 | 190 |
| 9 AI navigation | `aigraph`, `graph*`, `patrol*`, `splinepath*`, `ai`, `humanai`, `combatstate` | 637 | 637 |
| 10 Health / damage | `hitzone`, `medipack`, `human`, `soldier` | 7 | 7 |
| 11 Weapons | `weapon*`, `gun*`, `ammo*`, `m203`, `grenade`, `mine` | 248 | 248 |
| 12 Animation | `anim*`, `frames`, `*dyncubeobj` | 71 | 71 |
| 12 Interaction | `door`, `doorcabinet`, `switch`, `terminal`, `elevator`, `glass` | 192 | 192 |
| 13 Script engine | `script`, `qsc`, `qvm`, `qtask`, `qobj`, `symbol` | 568 | 568 |

### 24.1 Name conflicts — use by address only

These catalog labels do **not** match the binary at that address. Do not rely on the label; the address is the key.

| Address | Catalog label | Binary name | Gate |
|---|---|---|---|
| `0x00413A90` | `WeaponType_Open` | `WeaponType_Open_2` | CANDIDATE |
| `0x0043DCF0` | `Elevator_Move` | `ExplosionObj_Invoke7` | PASS |
| `0x00443840` | `Terminal_Create` | `DoorCabinet_Invoke3_443840` | PASS |
| `0x00443A00` | `Terminal_Process` | `DoorCabinet_Invoke8_443A00` | PASS |
| `0x00443A10` | `Terminal_Update` | `DoorCabinet_Invoke8_443A10` | PASS |
| `0x004468F0` | `Door_Create` | `DoorCabinet_Invoke3_4468F0` | PASS |
| `0x00446CE0` | `Door_PlayOpenCloseSound` | `DoorCabinet_Invoke9_446CE0` | PASS |
| `0x00447A80` | `Door_GetPropertyTable` | `DoorCabinet_GetContext_447A80` | PASS |
| `0x0045DCB0` | `HumanPlayerInput_Create` | `HumanAI_GetContext_45DCB0` | PASS |
| `0x00475CC0` | `Mine_Setup` | `WeaponGun_InvokeTrailUpdate` | PASS |
| `0x00475D80` | `Mine_Process` | `WeaponGun_Invoke8_475D80` | PASS |
| `0x00476EA0` | `Grenade_Throw` | `WeaponGun_GetContext_476EA0` | PASS |
| `0x00476F70` | `Grenade_Detonate` | `WeaponGun_InvokeValueSet_476F70` | PASS |
| `0x00482650` | `HumanCamera_Create` | `CombatState_GetContext_482650` | PASS |
| `0x00482700` | `HumanCamera_Update` | `CombatState_InvokeErrorInHumancameraHu` | PASS |

Both sides are auto-generated labels (the binary has no symbols), so treat the *behaviour* as unknown until the body is read — but never cite these by the catalog name.

### 24.2 Object placement path (MVP step 2 — exact positions)

Verified entry points for placing an object at its retail transform:

- **`WorldObject_CommitTransform` (`0x004C6940`)** — `int32_t WorldObject_CommitTransform(void* world, void* object, float* transform, float radius);`
  Writes the object transform at `object+0x20 … +0x34` (6 dwords, position/rotation pair) and `object+0x44 = radius` (a float arg), then links the object into the world's dynamic-cube structure via `DynObj_LinkDynamicCube_7(…)` using the world offsets at `world+0xDE0/+0xDE4/+0xDE8`. This is the commit point where a script/AI-produced transform becomes a placed world object — the field offsets above are from the decompilation; confirm before relying on them.
- **`DynObj_CreateMagicObject_2` (`0x004C0620`)** — `int32_t DynObj_CreateMagicObject_2(int32_t* hash, int32_t* outIndex, char* name);` — returns the object pointer (`[ecx+8]`) and writes its type index (`[ecx+0xC]`).
- **`DynObj_CreateMagicObject`** (`0x004C0240`) and `_3 … _14` — the create family; `RigidDynCubeObj_*` supplies the rigid-body variant.

**Still open:** how the raw position/orientation values are read from the Level 1 `OBJECTS.QVM` stream into `transform` before this commit is **not yet established**. Trace the caller of `WorldObject_CommitTransform` on the Level 1 path before implementing placement.

### 24.3 Model chunk reading (MVP step 2)

`DynObj_LocalModelsSMef` (`0x004CF390`) reads the `HSEM`, `ATTA`, `MVTX`, `PORT`, `PTFC`, `PTVX` and `TXAN` chunks; it does **not** decode the vertex/index stream (see §17 and §1). The vertex stride/FVF remains **UNVERIFIED** and is explicitly out of scope for this pass.


## 25. Runtime load-path corrections (2026-10-17)

The captured retail Level 1 chain is `Level_LoadObjects (0x004F0E10)` → `Level_Load (0x004F0D40; catalog alias Task_New_2)` → `Qvm_Load (0x004B80B0)` → compiled QVM loader `(0x004BD220)` → `Resource_Load (0x004B5F00)` → `Qvm_Execute`. The QSC path selects the compiled objects.qvm resource. **Resource_Load returns bytes; its optional second argument receives a byte count**, not a resource-entry pointer: observed `0x2CDC5` matches the file length 183749. `0x004D9B10` executes as the MagicObj task parameter callback; its catalog label and float parameter are not a valid ABI description. Generic transform field decoding remains partly invalid, so exact placement is still unverified.

Evidence, exact samples, capped entry counts, REA/Ghidra IDs, CLEAN aliases, secondary-source navigation and capture limitations: [retail runtime/static comparison](/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.md). This section supersedes conflicting signature and completed-placement claims above. It does not establish full decoder, renderer, gameplay or per-object equality.
