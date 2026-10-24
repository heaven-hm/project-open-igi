# POSITION-1 REGISTRAR INVENTORY - QTask_RegisterClasses chain, retail `igi.exe`

Generated 2026-10-24T05:05:30Z (UTC). Read-only static inventory; no engine edits, no builds.

| item | value |
|---|---|
| retail artifact | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| retail SHA-256 | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| image | PE32 x86, base 0x400000; .text 0x401000-0x533000, .rdata 0x533000-0x536000, .data 0x536000-0xC33000, .rsrc 0xC33000-0xC35000 |
| decompiler (one per target) | Binary Ninja 6.0.10601 (free) via MCP `http://127.0.0.1:24642/mcp` |
| CFG cross-check | radare2 6.2.0 (abi 132, darwin-arm_64), `/opt/homebrew/bin/r2` |
| binary metadata | `rabin2` 6.2.0 |
| data-ref rule | absolute operand VA in `[0x530000, 0xC00000)` from r2 `agfj` opcodes, clipped to the BN function extent |

## 1. Identity and order (resolves the 0x530270 / 0x508F80 contradiction)

**QTask_RegisterClasses is `0x00402930`** (engine provenance: `engine/src/retail/core/class_registration.cpp`). Its first executed instruction is a `call`; there is no prologue:

```
00402930  call 0x508f80          bytes e8 4b 66 10 00
00402935  call 0x489560          bytes e8 26 6c 08 00
0040293a  call 0x407d60          bytes e8 21 54 00 00
0040293f  call 0x488bb0          bytes e8 6c 62 08 00
00402944  call 0x402ff0          bytes e8 a7 06 00 00
00402949  call 0x415530          bytes e8 e2 2b 01 00
0040294e  call 0x4c1780          bytes e8 2d ee 0b 00
00402953  call 0x401fc0          bytes e8 68 f6 ff ff
00402958  call 0x488800          bytes e8 a3 5e 08 00
0040295d  call 0x488620          bytes e8 be 5c 08 00
... 156 call instructions inside the BN extent 0x402930-0x402C3B (779 bytes)
```

- **First executed registrar (chain position 1) is `0x00508F80`**, first call site `0x00402930`, instruction bytes `e8 4b 66 10 00`. Retail bytes decide this; no runtime capture is needed to settle which VA runs first.
- `0x00402930` call 2 is `0x00489560` (position 2, transcribed), then `0x00407D60`, `0x00488BB0`, `0x00402FF0`, `0x00415530`, `0x004C1780`, `0x00401FC0` - the engine provenance order is confirmed by the call sequence.

### 1.1 What position 1 actually is

| VA | BN extent | bytes | blocks | edges | definition | direct callees | real role |
|---|---|---|---|---|---|---|---|
| `0x508f80` | 0x508f80-0x508f89 | 9 | 1 | 1 | `int32_t()` | 2 | position-1 thunk (call 0x5183D0, tail jmp 0x508FA0) |
| `0x5183d0` | 0x5183d0-0x518541 | 369 | 1 | 1 | `int32_t()` | 74 | position-1 first callee; flat 74-call chain; 78 registrations in its immediate callees |
| `0x508fa0` | 0x508fa0-0x509292 | 754 | 1 | 0 | `void*()` | 43 | AIGraph_Init; registers exactly 1 task type |
| `0x530270` | 0x530270-0x530292 | 34 | 2 | 1 | `int32_t()` | 7 | CamTask_Process; 1st callee inside 0x5183D0; registers nothing itself |
| `0x401900` | 0x401900-0x401a10 | 272 | 18 | 23 | `int32_t(int32_t arg1, int32_t arg2, int16_t arg3, int32_t arg4)` | 4 | task-type registration API |
| `0x4010d0` | 0x4010d0-0x4011ae | 222 | 1 | 0 | `void*()` | 16 | QTask_Init (registers id 0 in the retail capture) |
| `0x402930` | 0x402930-0x? | ? | None | None | `?` | 0 | QTask_RegisterClasses |

- `0x00508F80` body: `call 0x5183D0`, `jmp 0x508FA0` (2 instructions, single block). Both unconditional, so position 1 always executes `0x5183D0` then `0x508FA0`.
- `0x005183D0` is a flat single-block call sequence (BN end `0x00518541`, 74 direct calls). r2 linear sweep agrees through the BN end and then continues into the next function, which is why raw r2 sizes inflate for it.
- `0x00530270` is **not a registrar and not position 1**: it is `CamTask_Process` (BN extent 0x00530270-0x00530292, 34 bytes, 1 block, 7 calls, tail-jump to 0x004ED000), the first entry of the flat call list in `0x5183D0`. The "0x530270 group" in the retail trace is this first inline group *inside* position 1, not a sibling registrar.
- `0x00508FA0` `AIGraph_Init` calls `0x00401900` once, `("AIGraph", 0xC0, <Script_FormatString_21()>, 0)`; stores the returned id in `0x0054E83C`, links ~11 event handlers, writes AI parameter defaults at `0x00A7B2B8`-`0x00A7B2CC`. One class, not 44.
- `0x508F80` and `0x530270` are not the same subsystem entry: `0x508F80` is the AI-graph registrar thunk; `0x530270` is a process callback that happens to be its first grand-callee. The ids come from `0x00401900` calls distributed across the subtree, not from either function body.

### 1.2 Registration API: `0x00401900` TaskType_Register

| property | value |
|---|---|
| VA / extent | `0x00401900`-`0x00401A10` (272 B) |
| blocks / edges | 18 / 23 (BN and r2 agree exactly) |
| signature (BN) | `int32_t TaskType_Register(int32_t, int32_t, int16_t, int32_t)`; operand use `(char const* name, uint32_t object_size, uint16_t parent_type, void* task_list_arg)` |
| id arrival | scans the 0x30-stride slot flag array at `0x00AF5F05` for the first clear byte; `ebp` = slot index; stores name@slot+0x00AF5EE0, size@+0xAF5EE4, parent@+0xAF5EE8, base-handler@+0xAF5F0C; copies the parent row from the 0x180-dword handler table at `0x00A96AE0`; returns `ebp` as uint16 |
| parent sentinel | `arg3 == 0x181` (root) zeroes the handler row and routes the return through `QTask_DeleteTask` |
| call examples | `QTask_Init` 0x004010D0: `("QTask", 0x20, 0x181, 0)`; `AIGraph_Init` 0x00508FA0: `("AIGraph", 0xC0, Script_FormatString_21(), 0)` |
| engine status | `engine/src/retail/core/task_types.cpp` + `engine/include/igi/retail/core/task_types.hpp` |

Ids are first-free-slot indices and registration never frees slots, so ids follow call order (`0,1,2,...`) unless a slot was pre-occupied. Any skipped or reordered registrar shifts every following id.

## 2. Registration order inside position 1 (execution-order DFS over the captured tree)

127 direct `0x00401900` call sites lie inside the position-1 subtree (BN extents, r2 CFG). The running ids below are inferred from the sequential rule; they are **not** retail-captured here.

| inferred ids | caller VA | BN alias | registration sites in caller |
|---|---|---|---|
| 0..3 | `0x5309c0` | `Particle_TriggerMuzzleFlash_3` | 4 |
| 4..4 | `0x4010d0` | `QTask_Init` | 1 |
| 5..5 | `0x4d9730` | `QObj_Init` | 1 |
| 6..6 | `0x4e5370` | `DynCubeObj_Init` | 1 |
| 7..7 | `0x4da1d0` | `Hazinglayers_Init` | 1 |
| 8..8 | `0x531fa0` | `TransparencyList_Init` | 1 |
| 9..10 | `0x531300` | `PointlightDynCubeObj_Init` | 2 |
| 11..11 | `0x531ef0` | `Sprite3D_Init` | 1 |
| 12..12 | `0x4c3450` | `RigidDynCubeObj_Init` | 1 |
| 13..13 | `0x4d9a00` | `MagicObj_Initialize` | 1 |
| 14..14 | `0x4d7550` | `BoneDynCubeObj_Init` | 1 |
| 15..15 | `0x49a520` | `PolyListDynCubeObj_Init` | 1 |
| 16..16 | `0x4e41b0` | `SplineObjSegment_Init` | 1 |
| 17..17 | `0x4e0950` | `Shadow_Init` | 1 |
| 18..18 | `0x5317d0` | `BoneMagicObj_Init` | 1 |
| 19..21 | `0x4dcf20` | `Sector_Init` | 3 |
| 22..22 | `0x4e0840` | `MagicObjContainer_Init` | 1 |
| 23..23 | `0x498ab0` | `Rain_Init` | 1 |
| 24..26 | `0x4e6070` | `SoundSysController_Init` | 3 |
| 27..27 | `0x4ebc60` | `SoundSysMicrophone_Init` | 1 |
| 28..28 | `0x4eb1d0` | `SoundDefGroup_Init` | 1 |
| 29..29 | `0x4e8390` | `SoundDefSound_Init` | 1 |
| 30..30 | `0x4ea3d0` | `SoundDefGraph_Init` | 1 |
| 31..31 | `0x4e9520` | `SoundDefTriggerOnce_Init` | 1 |
| 32..32 | `0x4e7370` | `Screen_Init` | 1 |
| 33..33 | `0x4e7d90` | `ViewportQTask_Init` | 1 |
| 34..34 | `0x4e82c0` | `TerraDrawTask_Init` | 1 |
| 35..35 | `0x4e7c50` | `CameraControlTask_Init` | 1 |
| 36..36 | `0x5310e0` | `FreeCamera_Init` | 1 |
| 37..37 | `0x4e7840` | `DebugText_Init` | 1 |
| 38..39 | `0x4e92c0` | `FadeOutEnd_Init` | 2 |
| 40..40 | `0x4e9480` | `GlowTask_Init` | 1 |
| 41..41 | `0x530c80` | `ParticleSystemDynCubeObj_Init` | 1 |
| 42..42 | `0x531190` | `SpriteParticleSystemDynCubeObj_Init` | 1 |
| 43..43 | `0x530a80` | `FlashTask_Init` | 1 |
| 44..47 | `0x4a2a00` | `BandFilter_Init` | 4 |
| 48..48 | `0x4fcf50` | `Particle2DDynCubeObjTask_Init` | 1 |
| 49..49 | `0x530220` | `CamTask_Init` | 1 |
| 50..50 | `0x4ed510` | `InputPort_Init` | 1 |
| 51..51 | `0x508d70` | `Sync_Init` | 1 |
| 52..52 | `0x504e10` | `MenuItemTask_Init` | 1 |
| 53..53 | `0x505630` | `MenuItem_Init` | 1 |
| 54..54 | `0x52ca70` | `PushButton_Init` | 1 |
| 55..55 | `0x52ca30` | `SimBoneDynCubeObj_Init` | 1 |
| 56..60 | `0x4ee970` | `CameraBase_Init` | 5 |
| 61..61 | `0x52c820` | `LevelScript_Init` | 1 |
| 62..62 | `0x52c6b0` | `SkyTask_Init` | 1 |
| 63..63 | `0x4fb1b0` | `PhysicsObj_Init` | 1 |
| 64..64 | `0x503d60` | `PhysicsMagicObj_Init` | 1 |
| 65..65 | `0x5033f0` | `EditorRigidObj_Init` | 1 |
| 66..66 | `0x52c0c0` | `DiscardTerrain_Init` | 1 |
| 67..67 | `0x52b6c0` | `DiscardTerrainArea_Init` | 1 |
| 68..68 | `0x52b3b0` | `CubeModifier_Init` | 1 |
| 69..69 | `0x4f3440` | `AnimTask_Init` | 1 |
| 70..74 | `0x4fe7c0` | `TagItem_Init` | 5 |
| 75..76 | `0x502250` | `SplineObj_Init` | 2 |
| 77..77 | `0x4f2740` | `EditRigidObj_Init` | 1 |
| 78..78 | `0x505f60` | `TextureModifier_Init` | 1 |
| 79..79 | `0x518ef0` | `Water_Init` | 1 |
| 80..80 | `0x52b040` | `EditHazinglayer_Init` | 1 |
| 81..83 | `0x4ed6b0` | `LODSettings_Init` | 3 |
| 84..84 | `0x504c50` | `EditRigidObjAttachedToBone_Init` | 1 |
| 85..85 | `0x4f4b00` | `CutScene_TaskCreate` | 1 |
| 86..86 | `0x52ad10` | `AnimScaleTask_Init` | 1 |
| 87..87 | `0x52aaf0` | `AnimRigidDynCubeObj_Init` | 1 |
| 88..88 | `0x4faa60` | `Graph_Init` | 1 |
| 89..89 | `0x518760` | `GraphNode_Init` | 1 |
| 90..90 | `0x5284a0` | `AnimCam_Init` | 1 |
| 91..91 | `0x5043b0` | `AreaActivate_Init` | 1 |
| 92..92 | `0x4f4190` | `Vehicle_Init` | 1 |
| 93..94 | `0x527c90` | `GlobalLight_Init` | 2 |
| 95..96 | `0x526ee0` | `Dirlight_Init` | 2 |
| 97..97 | `0x526330` | `LightmapInfo_Init` | 1 |
| 98..99 | `0x520bb0` | `Lensflare_Init` | 2 |
| 100..100 | `0x505c90` | `Container_Init` | 1 |
| 101..101 | `0x5207a0` | `Origo_Init` | 1 |
| 102..102 | `0x51fef0` | `NoiseQTask_Init` | 1 |
| 103..103 | `0x51f580` | `MoveRigidObj_Init` | 1 |
| 104..104 | `0x51eee0` | `SmoothQTask_Init` | 1 |
| 105..105 | `0x51ecb0` | `MipMapControl_Init` | 1 |
| 106..106 | `0x4ee410` | `ScreenGrab_Init` | 1 |
| 107..108 | `0x51ddf0` | `FlatSky_Init` | 2 |
| 109..110 | `0x51c2b0` | `FlatWater_Init` | 2 |
| 111..111 | `0x51bfb0` | `SoundManager_Init` | 1 |
| 112..112 | `0x507f10` | `EditCamera_Init` | 1 |
| 113..113 | `0x51bac0` | `LevelTimer_Init` | 1 |
| 114..114 | `0x51b960` | `EditorMagicObj_Init` | 1 |
| 115..115 | `0x51b400` | `AnimSound_Init` | 1 |
| 116..116 | `0x4f7360` | `RainDynCubeObj_Init` | 1 |
| 117..117 | `0x4f8340` | `TerrainLightMap_Init` | 1 |
| 118..119 | `0x5034e0` | `SimpointObj_Init` | 2 |
| 120..120 | `0x519850` | `SplinePathNodeQTask_Init` | 1 |
| 121..121 | `0x51a1d0` | `SplinePathDynCubeObj_Init` | 1 |
| 122..122 | `0x51b140` | `SplinePathGuideQTask_Init` | 1 |
| 123..123 | `0x506fa0` | `HeightMap_Init` | 1 |
| 124..124 | `0x519760` | `ShadowTask_Init` | 1 |
| 125..125 | `0x519710` | `GFXCapsViewer_Init` | 1 |
| 126..126 | `0x508fa0` | `AIGraph_Init` | 1 |

**Against "position 1 registers ids 0..43":** under the sequential rule the subtree reaches id 126, not 43. `QTask` id 0 is registered by `QTask_Init` 0x004010D0, reached inside this same subtree; ids 1..126 follow from the 126 further sites above before position 2 runs. A 44-id claim holds only if most of those calls do not execute during registration (branch or later execution) - static evidence cannot decide that; every listed call is unconditional. Required capture is section 6.

## 3. Transitive call tree (position 1 = `0x00508F80`)

Captured by BFS over BN direct callees from `0x00508F80` (no depth cap, 537 distinct code VAs reached; calls to imports/IAT and to data are excluded). Per function: BN extent size, r2 CFG blocks/edges clipped to the BN extent, direct callees, engine implementation state. `edges` = sum of taken `jump`/`fail` edges over the clipped blocks; single-block functions therefore have 0 edges.

**Totals:** 537 functions, 2106 blocks, 2423 edges, 127 registration call sites, 54 functions already implemented in `engine/`, 483 absent. Depth distribution: 0:1, 1:2, 2:89, 3:114, 4:36, 5:50, 6:35, 7:40, 8:43, 9:53, 10:41, 11:11, 12:7, 13:3, 14:6, 15:5, 16:1.

| # | VA | depth | BN alias | size | blocks | edges | callees | engine |
|---|---|---|---|---|---|---|---|---|
| 1 | `0x4010d0` | 6 | `QTask_Init` | 222 | 1 | 0 | 16 | **landed** `task_types.cpp` |
| 2 | `0x4011b0` | 7 | `QTask_Update` | 22 | 2 | 3 | 0 | **landed** `task_types.cpp` |
| 3 | `0x4011d0` | 7 | `QTask_OnEvent` | 18 | 2 | 3 | 0 | **landed** `task_types.cpp` |
| 4 | `0x401400` | 2 | `QTask_RemoveEventCallback` | 163 | 7 | 10 | 2 | **landed** `task_types.cpp` |
| 5 | `0x4014b0` | 3 | `QTask_DeleteTask` | 42 | 5 | 6 | 1 | absent |
| 6 | `0x4014e0` | 4 | `QTask_ChangeParent` | 74 | 5 | 6 | 0 | absent |
| 7 | `0x401530` | 2 | `QTask_LinkQTaskEventHandlerNoContextFn` | 157 | 7 | 10 | 2 | **landed** `task_types.cpp` |
| 8 | `0x4017c0` | 3 | `QTask_UnableToAllocateEven` | 75 | 6 | 7 | 1 | **landed** `task_types.cpp` |
| 9 | `0x401810` | 3 | `QTask_NewQTaskEventNoContextFn` | 73 | 6 | 7 | 1 | **landed** `native_401810.cpp` |
| 10 | `0x401860` | 7 | `QTask_CleanupEvents` | 40 | 1 | 0 | 0 | absent |
| 11 | `0x401890` | 7 | `QTask_ProcessQueue` | 35 | 1 | 0 | 0 | absent |
| 12 | `0x4018f0` | 3 | `QTask_OpenEvent_2` | 6 | 1 | 0 | 0 | **landed** `task_types.cpp` |
| 13 | `0x401900` | 2 | `TaskType_Register` | 272 | 18 | 23 | 4 | **landed** `task_types.cpp` |
| 14 | `0x401d30` | 8 | `QTask_CleanupTypes_2` | 29 | 6 | 7 | 1 | absent |
| 15 | `0x401f60` | 3 | `QTask_DispatchEvent_3` | 18 | 1 | 0 | 0 | absent |
| 16 | `0x401fa0` | 3 | `QTask_RegisterEventCallback_3` | 31 | 1 | 0 | 0 | absent |
| 17 | `0x48f020` | 8 | `FX_Germany` | 79 | 4 | 5 | 4 | absent |
| 18 | `0x48f090` | 9 | `FX_TrailUpdate_21` | 5 | 1 | 0 | 0 | **landed** `native_48f090.cpp` |
| 19 | `0x48f0a0` | 9 | `AppContext_GethWnd` | 5 | 1 | 0 | 0 | **landed** `native_48f0a0.cpp` |
| 20 | `0x48f180` | 9 | `FX_TrailUpdate_23` | 19 | 1 | 0 | 0 | absent |
| 21 | `0x48f2b0` | 9 | `FX_FireBallUpdate_26` | 9 | 1 | 0 | 0 | **landed** `app_startup_globals.cpp` |
| 22 | `0x48f490` | 9 | `FX_FailedToRegisterWinp` | 90 | 6 | 7 | 2 | absent |
| 23 | `0x48fd90` | 9 | `FX_CouldNotGetMouseCaps` | 65 | 4 | 4 | 1 | **landed** `native_48fd90.cpp` |
| 24 | `0x48fde0` | 8 | `FX_CouldNotCreateDirect` | 225 | 13 | 16 | 9 | absent |
| 25 | `0x48fff0` | 8 | `FX_CouldNotCreateDirect_2` | 293 | 15 | 19 | 11 | absent |
| 26 | `0x490340` | 9 | `Display_SyncVSync` | 7 | 1 | 0 | 0 | **landed** `native_490340.cpp` |
| 27 | `0x490350` | 9 | `Display_SwapBuffers` | 9 | 1 | 0 | 0 | **landed** `native_490350.cpp` |
| 28 | `0x490360` | 8 | `Timer_Open` | 11 | 1 | 0 | 1 | absent |
| 29 | `0x490370` | 3 | `Timer_Read` | 12 | 1 | 0 | 1 | absent |
| 30 | `0x490380` | 8 | `Display_CheckCapabilities` | 21 | 1 | 0 | 2 | absent |
| 31 | `0x491630` | 5 | `Display_SetDisplayMode_3` | 18 | 1 | 0 | 1 | absent |
| 32 | `0x491650` | 6 | `Display_NoMoreDisplayCallbac` | 68 | 6 | 7 | 1 | absent |
| 33 | `0x4916c0` | 5 | `Display_SwapBuffers_3` | 18 | 1 | 0 | 1 | absent |
| 34 | `0x491700` | 8 | `Display_DisplayCanOnlyBeOpen` | 338 | 4 | 4 | 23 | absent |
| 35 | `0x491860` | 9 | `Display_RestoreDevice_3` | 37 | 2 | 3 | 0 | absent |
| 36 | `0x491db0` | 4 | `Display_SetGammaRamp_4` | 5 | 1 | 0 | 0 | absent |
| 37 | `0x491dc0` | 10 | `Display_CreateSurface_4` | 5 | 1 | 0 | 0 | absent |
| 38 | `0x491dd0` | 10 | `Display_OpenDevice_5` | 5 | 1 | 0 | 0 | absent |
| 39 | `0x491de0` | 10 | `Display_CloseDevice_5` | 5 | 1 | 0 | 0 | absent |
| 40 | `0x491df0` | 9 | `Display_SetDisplayMode_5` | 5 | 1 | 0 | 0 | absent |
| 41 | `0x491e00` | 10 | `Display_SyncVSync_5` | 5 | 1 | 0 | 0 | absent |
| 42 | `0x491e10` | 10 | `Display_SwapBuffers_5` | 5 | 1 | 0 | 0 | absent |
| 43 | `0x491e20` | 10 | `Display_ClearSurfaces_5` | 5 | 1 | 0 | 0 | absent |
| 44 | `0x491e30` | 10 | `Display_RestoreDevice_5` | 5 | 1 | 0 | 0 | absent |
| 45 | `0x491e40` | 10 | `Display_CheckCapabilities_5` | 5 | 1 | 0 | 0 | absent |
| 46 | `0x491e50` | 10 | `Display_SetGammaRamp_5` | 5 | 1 | 0 | 0 | absent |
| 47 | `0x491e60` | 10 | `Display_CreateSurface_5` | 5 | 1 | 0 | 0 | absent |
| 48 | `0x493580` | 9 | `Display_Direct3D` | 662 | 1 | 0 | 36 | absent |
| 49 | `0x4950e0` | 5 | `Display_CheckCapabilities_7` | 5 | 1 | 0 | 0 | absent |
| 50 | `0x495170` | 9 | `Display_CreateSurface_7` | 18 | 1 | 0 | 1 | absent |
| 51 | `0x495190` | 8 | `Display_NoMoreDirect3DCallba` | 68 | 6 | 7 | 1 | absent |
| 52 | `0x4951e0` | 9 | `Display_OpenDevice_8` | 18 | 1 | 0 | 1 | absent |
| 53 | `0x495200` | 7 | `Display_CloseDevice_8` | 18 | 1 | 0 | 1 | absent |
| 54 | `0x495280` | 8 | `Display_SwapBuffers_8` | 19 | 1 | 1 | 4 | absent |
| 55 | `0x4952a0` | 9 | `Display_Cdaudio` | 68 | 3 | 3 | 1 | absent |
| 56 | `0x4952f0` | 9 | `Display_ClearSurfaces_8` | 767 | 23 | 30 | 9 | absent |
| 57 | `0x4955f0` | 10 | `Display_CheckCapabilities_8` | 32 | 2 | 3 | 0 | absent |
| 58 | `0x495620` | 10 | `Display_SetGammaRamp_8` | 30 | 3 | 3 | 0 | absent |
| 59 | `0x495640` | 9 | `Display_CdAudio` | 1041 | 56 | 90 | 17 | absent |
| 60 | `0x495a60` | 9 | `Display_CloseDevice_9` | 34 | 2 | 3 | 0 | absent |
| 61 | `0x495cb0` | 5 | `Display_CheckCapabilities_9` | 5 | 1 | 0 | 0 | absent |
| 62 | `0x497510` | 5 | `Display_OpenDevice_13` | 5 | 1 | 0 | 0 | absent |
| 63 | `0x497520` | 4 | `Display_CloseDevice_13` | 45 | 1 | 0 | 0 | absent |
| 64 | `0x497550` | 8 | `Display_SetDisplayMode_13` | 30 | 1 | 0 | 0 | absent |
| 65 | `0x497570` | 3 | `Display_SyncVSync_13` | 124 | 1 | 0 | 5 | absent |
| 66 | `0x497680` | 3 | `Display_CheckCapabilities_13` | 124 | 1 | 0 | 2 | absent |
| 67 | `0x497880` | 4 | `Display_SwapBuffers_14` | 39 | 3 | 6 | 0 | absent |
| 68 | `0x4978d0` | 3 | `Display_RestoreDevice_14` | 13 | 1 | 0 | 1 | absent |
| 69 | `0x4978f0` | 3 | `Display_SetGammaRamp_14` | 37 | 1 | 0 | 1 | absent |
| 70 | `0x497940` | 9 | `Display_CreateSurface_14` | 9 | 1 | 0 | 0 | absent |
| 71 | `0x497980` | 8 | `Display_Transcontextprimitiv` | 123 | 1 | 0 | 10 | absent |
| 72 | `0x497ed0` | 9 | `Display_SwapBuffers_15` | 10 | 1 | 0 | 0 | absent |
| 73 | `0x498080` | 9 | `Display_Offscreenbuffer` | 179 | 1 | 0 | 13 | absent |
| 74 | `0x498370` | 11 | `Display_CloseDevice_16` | 5 | 1 | 0 | 0 | absent |
| 75 | `0x498900` | 9 | `Display_CheckCapabilities_16` | 43 | 1 | 0 | 4 | absent |
| 76 | `0x498950` | 8 | `Display_SetGammaRamp_16` | 9 | 2 | 1 | 2 | absent |
| 77 | `0x498ab0` | 6 | `Rain_Init` | 68 | 1 | 0 | 4 | absent |
| 78 | `0x49a340` | 7 | `Display_SyncVSync_18` | 10 | 1 | 0 | 0 | absent |
| 79 | `0x49a520` | 6 | `PolyListDynCubeObj_Init` | 190 | 1 | 0 | 10 | absent |
| 80 | `0x49bb40` | 6 | `Display_CloseDevice_19` | 495 | 3 | 3 | 40 | absent |
| 81 | `0x4a1470` | 6 | `Camera_ApplyLineFilter` | 55 | 1 | 0 | 7 | absent |
| 82 | `0x4a2a00` | 4 | `BandFilter_Init` | 715 | 1 | 0 | 47 | absent |
| 83 | `0x4a53b3` | 3 | `sprintf` | 81 | 4 | 4 | 2 | **landed** `error_show.cpp` |
| 84 | `0x4a54a0` | 3 | `Camera_UpdateTransform_5` | 46 | 3 | 4 | 0 | absent |
| 85 | `0x4a5702` | 4 | `String_VFormat` | 80 | 4 | 4 | 2 | **landed** `error_show.cpp` |
| 86 | `0x4a5980` | 9 | `Camera_UpdateTransform_6` | 139 | 13 | 19 | 2 | absent |
| 87 | `0x4a6a50` | 11 | `Camera_UpdateTransform_9` | 104 | 10 | 14 | 5 | absent |
| 88 | `0x4a6d7d` | 5 | `Camera_ResetMatrix_9` | 153 | 13 | 17 | 4 | absent |
| 89 | `0x4a760b` | 4 | `_flsbuf` | 276 | 26 | 38 | 5 | absent |
| 90 | `0x4a7720` | 4 | `_output` | 1824 | 184 | 278 | 25 | absent |
| 91 | `0x4a7e61` | 5 | `Camera_ApplyPostProcess_11` | 52 | 6 | 6 | 1 | absent |
| 92 | `0x4a7e96` | 5 | `Camera_UpdateViewCone_11` | 48 | 5 | 7 | 1 | absent |
| 93 | `0x4a7ec7` | 5 | `Camera_ResetMatrix_11` | 55 | 5 | 7 | 1 | absent |
| 94 | `0x4a7eff` | 5 | `Camera_UpdateTransform_12` | 12 | 1 | 0 | 0 | absent |
| 95 | `0x4a7f0c` | 5 | `Camera_SetFieldOfView_12` | 15 | 1 | 0 | 0 | absent |
| 96 | `0x4a7f1c` | 5 | `Camera_ApplyLineFilter_12` | 13 | 1 | 0 | 0 | absent |
| 97 | `0x4a7f2a` | 9 | `Camera_ApplyBandFilter_12` | 116 | 9 | 10 | 1 | absent |
| 98 | `0x4a80bf` | 14 | `Camera_ApplyPostProcess_12` | 538 | 37 | 53 | 3 | absent |
| 99 | `0x4a8372` | 14 | `Camera_UpdateViewCone_12` | 534 | 57 | 85 | 6 | absent |
| 100 | `0x4a8611` | 14 | `Camera_UpdateTransform_13` | 37 | 5 | 7 | 0 | absent |
| 101 | `0x4a869b` | 16 | `Camera_ApplyHandiCam_13` | 40 | 1 | 0 | 0 | absent |
| 102 | `0x4a871e` | 15 | `Camera_ApplyPostProcess_13` | 192 | 17 | 24 | 2 | absent |
| 103 | `0x4a87df` | 15 | `Camera_UpdateViewCone_13` | 13 | 1 | 0 | 0 | absent |
| 104 | `0x4a87ed` | 15 | `Camera_ResetMatrix_13` | 14 | 1 | 0 | 0 | absent |
| 105 | `0x4a87fc` | 14 | `Camera_UpdateTransform_14` | 34 | 1 | 0 | 0 | absent |
| 106 | `0x4a881f` | 15 | `Camera_SetFieldOfView_14` | 85 | 11 | 15 | 0 | absent |
| 107 | `0x4a9c57` | 12 | `Camera_UpdateViewCone_17` | 59 | 3 | 3 | 1 | absent |
| 108 | `0x4a9cb5` | 12 | `Camera_UpdateTransform_18` | 22 | 1 | 0 | 0 | absent |
| 109 | `0x4a9ccc` | 12 | `Camera_SetFieldOfView_18` | 24 | 3 | 2 | 0 | absent |
| 110 | `0x4a9d28` | 11 | `Camera_ApplyBandFilter_18` | 21 | 3 | 2 | 0 | absent |
| 111 | `0x4a9fc1` | 10 | `Camera_SetFieldOfView_19` | 202 | 18 | 26 | 2 | absent |
| 112 | `0x4aa670` | 4 | `Mem_AllocChunk` | 17 | 1 | 0 | 1 | **landed** `mem_alloc.cpp` |
| 113 | `0x4aa682` | 5 | `Mem_AllocWithRetry` | 43 | 5 | 9 | 2 | **landed** `mem_alloc.cpp` |
| 114 | `0x4aa6ae` | 6 | `Mem_AllocateRawBlock` | 115 | 16 | 22 | 3 | **landed** `mem_alloc.cpp` |
| 115 | `0x4aa730` | 5 | `Camera_UpdateViewCone_20` | 122 | 14 | 18 | 0 | absent |
| 116 | `0x4aad90` | 12 | `Camera_ApplyHandiCam_21` | 42 | 5 | 7 | 0 | absent |
| 117 | `0x4aadbb` | 12 | `Mem_AllocateCustomPool_1` | 808 | 50 | 76 | 4 | absent |
| 118 | `0x4ab0e4` | 7 | `Mem_GrowCustomPool_1` | 776 | 65 | 101 | 2 | absent |
| 119 | `0x4ab3ed` | 8 | `Camera_UpdateViewCone_21` | 176 | 9 | 12 | 4 | absent |
| 120 | `0x4ab49e` | 8 | `Camera_ResetMatrix_21` | 250 | 14 | 19 | 1 | absent |
| 121 | `0x4ab599` | 8 | `Camera_UpdateTransform_22` | 323 | 22 | 31 | 6 | absent |
| 122 | `0x4ab6dd` | 14 | `Camera_SetFieldOfView_22` | 85 | 5 | 5 | 2 | absent |
| 123 | `0x4ab733` | 13 | `Camera_ApplyLineFilter_22` | 193 | 20 | 33 | 2 | absent |
| 124 | `0x4ab7f5` | 12 | `Camera_ApplyBandFilter_22` | 86 | 9 | 12 | 0 | absent |
| 125 | `0x4ab84c` | 12 | `Camera_ApplyHandiCam_22` | 68 | 3 | 5 | 1 | absent |
| 126 | `0x4ab891` | 7 | `Mem_AllocateCustomPool` | 519 | 43 | 66 | 5 | absent |
| 127 | `0x4aba99` | 8 | `Mem_GrowCustomPool_2` | 291 | 41 | 58 | 0 | absent |
| 128 | `0x4abc46` | 6 | `Camera_UpdateViewCone_22` | 102 | 10 | 14 | 0 | absent |
| 129 | `0x4abe33` | 6 | `Camera_ApplyLineFilter_23` | 60 | 4 | 4 | 0 | absent |
| 130 | `0x4abec7` | 5 | `Camera_ApplyHandiCam_23` | 428 | 33 | 50 | 6 | absent |
| 131 | `0x4ac323` | 5 | `Camera_ResetMatrix_23` | 67 | 4 | 4 | 1 | absent |
| 132 | `0x4acd10` | 8 | `Camera_ApplyBandFilter_26` | 87 | 10 | 15 | 0 | absent |
| 133 | `0x4acd70` | 13 | `Camera_ApplyHandiCam_26` | 820 | 33 | 23 | 8 | absent |
| 134 | `0x4ad0ae` | 5 | `Camera_ApplyPostProcess_26` | 37 | 3 | 2 | 0 | absent |
| 135 | `0x4ad0d4` | 5 | `Camera_UpdateViewCone_26` | 103 | 9 | 11 | 1 | absent |
| 136 | `0x4ad140` | 5 | `Camera_ResetMatrix_26` | 103 | 11 | 16 | 0 | absent |
| 137 | `0x4ad1b0` | 5 | `Camera_UpdateTransform_27` | 116 | 11 | 16 | 0 | absent |
| 138 | `0x4ad225` | 10 | `Camera_SetFieldOfView_27` | 328 | 20 | 29 | 8 | absent |
| 139 | `0x4ad36e` | 14 | `Camera_ApplyHandiCam_27` | 2 | 1 | 0 | 0 | absent |
| 140 | `0x4ad371` | 13 | `Camera_ApplyWideScreen_27` | 207 | 25 | 38 | 5 | absent |
| 141 | `0x4ad4d3` | 9 | `Camera_UpdateViewCone_27` | 547 | 31 | 49 | 12 | absent |
| 142 | `0x4ad6f7` | 10 | `Camera_ApplyBandFilter_28` | 42 | 6 | 8 | 0 | absent |
| 143 | `0x4ad722` | 6 | `Mem_InvokeNewHandler` | 26 | 4 | 4 | 0 | **landed** `mem_alloc.cpp` |
| 144 | `0x4ad73d` | 8 | `String_ToUpperChar` | 203 | 18 | 26 | 2 | **landed** `resource_lookup.cpp` |
| 145 | `0x4ae730` | 9 | `Camera_SetFieldOfView_30` | 5 | 1 | 0 | 1 | **landed** `native_4ae730.cpp` |
| 146 | `0x4af740` | 9 | `Camera_ApplyBandFilter_30` | 5 | 1 | 0 | 1 | **landed** `native_4af740.cpp` |
| 147 | `0x4af746` | 10 | `Camera_ApplyHandiCam_30` | 5 | 1 | 0 | 1 | absent |
| 148 | `0x4af758` | 9 | `Camera_UpdateViewCone_30` | 5 | 1 | 0 | 1 | absent |
| 149 | `0x4af7b0` | 3 | `Debug_Error` | 88 | 4 | 5 | 2 | **landed** `retail_error.cpp` |
| 150 | `0x4af810` | 3 | `Warning_Show` | 77 | 2 | 3 | 2 | absent |
| 151 | `0x4af860` | 9 | `Camera_ApplyBandFilter_32` | 9 | 1 | 0 | 0 | absent |
| 152 | `0x4af870` | 10 | `Camera_ApplyHandiCam_32` | 9 | 1 | 0 | 0 | absent |
| 153 | `0x4af880` | 9 | `Camera_ApplyWideScreen_32` | 9 | 1 | 0 | 0 | absent |
| 154 | `0x4af890` | 9 | `Camera_ApplyPostProcess_32` | 9 | 1 | 0 | 0 | absent |
| 155 | `0x4af8a0` | 10 | `Camera_UpdateViewCone_32` | 9 | 1 | 0 | 0 | absent |
| 156 | `0x4af8b0` | 8 | `Camera_ResetMatrix_32` | 9 | 1 | 0 | 0 | absent |
| 157 | `0x4af8c0` | 9 | `Camera_UpdateTransform_33` | 45 | 1 | 0 | 6 | absent |
| 158 | `0x4af8f0` | 3 | `HeapList_Insert` | 27 | 1 | 0 | 0 | **landed** `heap_list.cpp` |
| 159 | `0x4af910` | 6 | `Camera_ApplyLineFilter_33` | 22 | 1 | 0 | 0 | absent |
| 160 | `0x4af960` | 3 | `HeapList_Remove` | 32 | 1 | 0 | 0 | **landed** `heap_list.cpp` |
| 161 | `0x4af9d0` | 4 | `Camera_UpdateViewCone_33` | 1189 | 68 | 97 | 6 | absent |
| 162 | `0x4aff90` | 5 | `Camera_ResetMatrix_33` | 28 | 1 | 0 | 1 | absent |
| 163 | `0x4affb0` | 5 | `Camera_UpdateTransform_34` | 575 | 41 | 58 | 0 | absent |
| 164 | `0x4b0b00` | 3 | `Sys_FreeMemory_2` | 100 | 5 | 7 | 1 | absent (mentioned: terrain_storage.cpp) |
| 165 | `0x4b0ba0` | 4 | `Mem_InitHeapState` | 52 | 1 | 0 | 1 | **landed** `mem_alloc.cpp` |
| 166 | `0x4b0c60` | 3 | `Mem_AllocFn` | 171 | 8 | 11 | 4 | **landed** `mem_alloc.cpp` |
| 167 | `0x4b0d10` | 10 | `Mem_DeAllocFn` | 41 | 1 | 0 | 2 | absent |
| 168 | `0x4b0d40` | 5 | `QHash_ValueSet` | 7 | 1 | 0 | 0 | **landed** `mem_alloc.cpp` |
| 169 | `0x4b0d60` | 5 | `Qhash_Init` | 33 | 1 | 0 | 0 | absent |
| 170 | `0x4b0d90` | 5 | `Qhash_Reset` | 23 | 1 | 0 | 0 | absent |
| 171 | `0x4b0dc0` | 8 | `Sys_PushStack_2` | 60 | 1 | 0 | 3 | absent |
| 172 | `0x4b0e00` | 10 | `Sys_RendermodeFailedToCr` | 74 | 6 | 7 | 1 | absent |
| 173 | `0x4b0e70` | 4 | `Sys_RendermodeFailedToCr_2` | 63 | 6 | 7 | 1 | absent |
| 174 | `0x4b0ec0` | 5 | `Sys_CheckHeap_2` | 29 | 1 | 0 | 0 | absent |
| 175 | `0x4b0ee0` | 4 | `Sys_AllocateMemory_3` | 75 | 5 | 6 | 0 | absent |
| 176 | `0x4b1200` | 8 | `Sys_PopStack_3` | 81 | 1 | 0 | 3 | absent |
| 177 | `0x4b12d0` | 9 | `Sys_AllocateMemory_4` | 23 | 2 | 3 | 0 | absent |
| 178 | `0x4b1b90` | 10 | `Sys_ReadFileBlock_5` | 39 | 2 | 3 | 0 | absent |
| 179 | `0x4b2180` | 8 | `Sys_Alloctexture` | 202 | 1 | 1 | 15 | absent |
| 180 | `0x4b26b0` | 9 | `Texture_DisableUpload` | 7 | 1 | 0 | 0 | **landed** `texture_loaders.cpp` |
| 181 | `0x4b26e0` | 11 | `Sys_AllocateMemory_9` | 5 | 1 | 0 | 0 | absent |
| 182 | `0x4b26f0` | 11 | `Sys_FreeMemory_9` | 5 | 1 | 0 | 0 | absent |
| 183 | `0x4b2700` | 11 | `Sys_OpenFileStream_9` | 5 | 1 | 0 | 0 | absent |
| 184 | `0x4b2710` | 11 | `Sys_ReadFileBlock_9` | 5 | 1 | 0 | 0 | absent |
| 185 | `0x4b2740` | 9 | `Sys_ExecuteBytecode_9` | 31 | 1 | 0 | 0 | absent |
| 186 | `0x4b27d0` | 9 | `Sys_PushStack_9` | 96 | 5 | 5 | 2 | absent |
| 187 | `0x4b33a0` | 3 | `Sys_CheckHeap_11` | 330 | 1 | 0 | 2 | absent |
| 188 | `0x4b38e0` | 4 | `Real32x9_Create` | 639 | 1 | 0 | 5 | absent |
| 189 | `0x4b3b60` | 3 | `Mat33_RotateX` | 63 | 1 | 0 | 0 | absent |
| 190 | `0x4b3ba0` | 5 | `Mat33_RotateY` | 59 | 1 | 0 | 0 | absent |
| 191 | `0x4b3be0` | 5 | `Mat33_RotateZ` | 63 | 1 | 0 | 0 | absent |
| 192 | `0x4b46a0` | 8 | `Sys_PopStack_13` | 115 | 5 | 6 | 1 | absent |
| 193 | `0x4b4720` | 3 | `Sys_ResolveSymbol_13` | 24 | 1 | 0 | 0 | absent |
| 194 | `0x4b4770` | 5 | `Sys_AllocateMemory_14` | 70 | 1 | 0 | 0 | absent |
| 195 | `0x4b47e0` | 3 | `Sys_OpenFileStream_14` | 49 | 1 | 0 | 1 | absent |
| 196 | `0x4b59a0` | 8 | `Sys_QvertexRegisterstrip` | 34 | 1 | 1 | 2 | absent |
| 197 | `0x4b59f0` | 6 | `Resource_CreateNode` | 248 | 10 | 13 | 3 | **landed** `sys_resource.cpp` |
| 198 | `0x4b5af0` | 7 | `Resource_CanonicalizePath` | 52 | 5 | 5 | 1 | **landed** `resource_lookup.cpp` |
| 199 | `0x4b5b30` | 7 | `Resource_CalculatePathHash` | 41 | 3 | 4 | 0 | **landed** `resource_lookup.cpp` |
| 200 | `0x4b5e70` | 8 | `Sys_ReadFileBlock_16` | 65 | 1 | 0 | 1 | absent |
| 201 | `0x4b6420` | 5 | `Sys_DispatchOpcode_16` | 29 | 1 | 0 | 1 | absent |
| 202 | `0x4b6d40` | 7 | `Sys_PushStack_17` | 26 | 1 | 0 | 0 | absent |
| 203 | `0x4b73c0` | 8 | `Sys_Sortrender` | 29 | 1 | 0 | 2 | absent |
| 204 | `0x4b73f0` | 10 | `Sys_ExecuteBytecode_19` | 54 | 1 | 0 | 1 | absent |
| 205 | `0x4b7430` | 7 | `Sys_DispatchOpcode_19` | 21 | 1 | 0 | 0 | absent |
| 206 | `0x4b7630` | 6 | `Sys_DispatchOpcode_20` | 150 | 5 | 7 | 7 | absent |
| 207 | `0x4b7c80` | 8 | `Sys_Allocpalette` | 48 | 1 | 0 | 3 | absent |
| 208 | `0x4b8810` | 3 | `TaskType_Set` | 31 | 1 | 0 | 1 | absent |
| 209 | `0x4b8890` | 3 | `Game_DefineOptions` | 34 | 1 | 0 | 1 | absent |
| 210 | `0x4b8970` | 8 | `Sys_ExecuteBytecode_23` | 9 | 1 | 0 | 0 | absent |
| 211 | `0x4b89f0` | 8 | `Sys_DispatchOpcode_23` | 9 | 1 | 0 | 0 | absent |
| 212 | `0x4b8a00` | 8 | `Sys_PushStack_23` | 9 | 1 | 0 | 0 | absent |
| 213 | `0x4b8b20` | 7 | `Sys_False` | 224 | 1 | 0 | 18 | absent |
| 214 | `0x4baac0` | 4 | `Q_TaskHashTableSet` | 103 | 1 | 0 | 5 | absent |
| 215 | `0x4bacd0` | 8 | `Sys_PopStack_25` | 105 | 1 | 0 | 1 | absent |
| 216 | `0x4bbc80` | 8 | `Sys_ResolveSymbol_26` | 36 | 1 | 1 | 1 | absent |
| 217 | `0x4bc920` | 8 | `Sys_CheckHeap_27` | 32 | 1 | 0 | 1 | absent |
| 218 | `0x4bd3d0` | 9 | `Qvm_SetOpcodeHandler` | 25 | 1 | 0 | 0 | **landed** `qvm_stack_natives.cpp` |
| 219 | `0x4bd430` | 8 | `Sys_OpenFileStream_29` | 758 | 7 | 9 | 51 | absent |
| 220 | `0x4c0250` | 8 | `DynObj_ParseConfigFile` | 4 | 1 | 1 | 1 | absent |
| 221 | `0x4c0360` | 9 | `DynObj_RemoveSymbol` | 86 | 5 | 5 | 2 | absent |
| 222 | `0x4c04b0` | 6 | `DynObj_InsertHashItem_2` | 127 | 12 | 17 | 1 | absent |
| 223 | `0x4c0530` | 6 | `DynObj_LookupHashItem_2` | 38 | 3 | 4 | 0 | absent |
| 224 | `0x4c0560` | 5 | `Symbol_CheckRegister` | 191 | 4 | 4 | 7 | absent |
| 225 | `0x4c0900` | 9 | `DynObj_InsertHashItem_3` | 259 | 3 | 3 | 17 | absent |
| 226 | `0x4c10e0` | 9 | `DynObj_UpdateDynTree_3` | 259 | 3 | 3 | 17 | absent |
| 227 | `0x4c1780` | 2 | `AnimSound_LoadParameters` | 0 | None | None | 0 | absent |
| 228 | `0x4c1800` | 3 | `QTaskList_New` | 33 | 1 | 0 | 1 | **landed** `qtask_list.cpp` |
| 229 | `0x4c3450` | 7 | `RigidDynCubeObj_Init` | 375 | 1 | 0 | 22 | absent |
| 230 | `0x4c48c0` | 3 | `DynObj_LookupHashItem_5` | 6 | 1 | 0 | 0 | absent |
| 231 | `0x4c56d0` | 3 | `DynObj_RemoveSymbol_5` | 5 | 1 | 0 | 0 | absent |
| 232 | `0x4c56e0` | 3 | `DynObj_RegisterSymbol_6` | 5 | 1 | 0 | 0 | absent |
| 233 | `0x4c56f0` | 3 | `DynObj_FindSymbol_6` | 5 | 1 | 0 | 0 | absent |
| 234 | `0x4c5700` | 7 | `DynObj_InsertHashItem_6` | 5 | 1 | 0 | 0 | absent |
| 235 | `0x4c5800` | 3 | `DynObj_CreateMagicObject_6` | 5 | 1 | 0 | 0 | absent |
| 236 | `0x4c5900` | 8 | `DynObj_RemoveSymbol_6` | 270 | 1 | 0 | 17 | absent |
| 237 | `0x4cea10` | 3 | `DynObj_UpdateDynTree_12` | 5 | 1 | 0 | 0 | absent |
| 238 | `0x4cea20` | 3 | `DynObj_LinkDynamicCube_12` | 5 | 1 | 0 | 0 | absent |
| 239 | `0x4cea60` | 7 | `DynObj_RegisterSymbol_13` | 5 | 1 | 0 | 0 | absent |
| 240 | `0x4cea70` | 7 | `DynObj_FindSymbol_13` | 5 | 1 | 0 | 0 | absent |
| 241 | `0x4cea80` | 7 | `Render_GetActiveBackendId` | 5 | 1 | 0 | 0 | **landed** `backend.cpp` |
| 242 | `0x4cea90` | 7 | `DynObj_LookupHashItem_13` | 5 | 1 | 0 | 0 | absent |
| 243 | `0x4ceba0` | 3 | `DynObj_MtpNlodvirmodelD` | 105 | 6 | 6 | 3 | absent |
| 244 | `0x4ceeb0` | 6 | `DynObj_Initmodels` | 119 | 1 | 0 | 5 | absent |
| 245 | `0x4cfc30` | 7 | `DynObj_ParseConfigFile_15` | 5 | 1 | 0 | 0 | absent |
| 246 | `0x4cfc40` | 7 | `DynObj_UpdateDynTree_15` | 5 | 1 | 0 | 0 | absent |
| 247 | `0x4cfc50` | 7 | `DynObj_LinkDynamicCube_15` | 5 | 1 | 0 | 0 | absent |
| 248 | `0x4cfc60` | 7 | `DynObj_RemoveSymbol_15` | 5 | 1 | 0 | 0 | absent |
| 249 | `0x4cfc70` | 7 | `DynObj_RegisterSymbol_16` | 5 | 1 | 0 | 0 | absent |
| 250 | `0x4cfc80` | 7 | `DynObj_FindSymbol_16` | 5 | 1 | 0 | 0 | absent |
| 251 | `0x4cfc90` | 7 | `DynObj_InsertHashItem_16` | 5 | 1 | 0 | 0 | absent |
| 252 | `0x4cfca0` | 3 | `DynObj_LookupHashItem_16` | 5 | 1 | 0 | 0 | absent |
| 253 | `0x4d0b30` | 6 | `Render_Genericuvfromrenderm` | 285 | 3 | 3 | 14 | absent |
| 254 | `0x4d0ef0` | 7 | `Render_TransformVertices_3` | 713 | 65 | 98 | 11 | absent |
| 255 | `0x4d14c0` | 7 | `Render_DrawBoneMesh_4` | 5 | 1 | 0 | 0 | absent |
| 256 | `0x4d14d0` | 3 | `Render_DrawSplineMesh_4` | 5 | 1 | 0 | 0 | absent |
| 257 | `0x4d2b80` | 8 | `Render_NoMoreQcubeCallbacks` | 69 | 6 | 7 | 1 | absent |
| 258 | `0x4d2bd0` | 8 | `Render_CallbackNotFound` | 74 | 9 | 11 | 1 | absent |
| 259 | `0x4d2c30` | 9 | `Render_BindTexture_4` | 186 | 3 | 3 | 6 | absent |
| 260 | `0x4d38a0` | 10 | `Render_DrawRigidMesh_7` | 19 | 1 | 0 | 0 | absent |
| 261 | `0x4d7550` | 7 | `BoneDynCubeObj_Init` | 199 | 1 | 0 | 10 | absent |
| 262 | `0x4d9610` | 3 | `BoneDynCubeObj_GetQTaskType` | 6 | 1 | 0 | 0 | absent |
| 263 | `0x4d9730` | 9 | `QObj_Init` | 82 | 1 | 0 | 7 | absent |
| 264 | `0x4d97e0` | 7 | `QObj_OnEvent` | 5 | 1 | 0 | 0 | absent |
| 265 | `0x4d9810` | 9 | `Qtask_Initialize` | 30 | 1 | 0 | 2 | absent |
| 266 | `0x4d9a00` | 6 | `MagicObj_Initialize` | 256 | 1 | 0 | 16 | absent |
| 267 | `0x4d9fa0` | 3 | `MagicObj_GetTypeId` | 6 | 1 | 0 | 0 | absent |
| 268 | `0x4d9fb0` | 3 | `MagicObj_GetEventId` | 5 | 1 | 0 | 0 | absent |
| 269 | `0x4da1d0` | 7 | `Hazinglayers_Init` | 107 | 1 | 0 | 5 | absent |
| 270 | `0x4da880` | 9 | `Render_BindTexture_12` | 19 | 1 | 0 | 1 | absent |
| 271 | `0x4dcf20` | 6 | `Sector_Init` | 661 | 1 | 0 | 42 | absent |
| 272 | `0x4e0830` | 3 | `SoundSys_Update3DPosition` | 6 | 1 | 0 | 0 | absent |
| 273 | `0x4e0840` | 6 | `MagicObjContainer_Init` | 70 | 1 | 0 | 5 | absent |
| 274 | `0x4e0950` | 6 | `Shadow_Init` | 100 | 1 | 0 | 6 | absent |
| 275 | `0x4e41b0` | 6 | `SplineObjSegment_Init` | 202 | 1 | 0 | 12 | absent |
| 276 | `0x4e5350` | 7 | `SoundSys_EvaluateTrigger_2` | 6 | 1 | 0 | 0 | absent |
| 277 | `0x4e5360` | 3 | `SoundSys_AttachLipsync_2` | 5 | 1 | 0 | 0 | absent |
| 278 | `0x4e5370` | 9 | `DynCubeObj_Init` | 47 | 1 | 0 | 2 | absent |
| 279 | `0x4e55b0` | 3 | `SoundSys_PlaySoundSample_3` | 5 | 1 | 0 | 0 | absent |
| 280 | `0x4e55c0` | 8 | `SoundSys_Update3DPosition_3` | 38 | 1 | 0 | 3 | absent |
| 281 | `0x4e5d90` | 7 | `SoundSys_PlaySoundSample_4` | 261 | 19 | 27 | 2 | absent |
| 282 | `0x4e6070` | 4 | `SoundSysController_Init` | 536 | 4 | 5 | 27 | absent |
| 283 | `0x4e6290` | 5 | `SoundSys_LoadSoundDef_4` | 29 | 3 | 3 | 0 | absent |
| 284 | `0x4e65f0` | 5 | `SoundSys_SetMasterVolume_5` | 24 | 2 | 3 | 1 | absent |
| 285 | `0x4e6610` | 3 | `SoundSys_SetFrequency_5` | 24 | 2 | 3 | 1 | absent |
| 286 | `0x4e6630` | 5 | `SoundSys_StopSoundSample_5` | 24 | 2 | 3 | 1 | absent |
| 287 | `0x4e6650` | 5 | `SoundSys_LoadSoundDef_5` | 24 | 2 | 3 | 1 | absent |
| 288 | `0x4e6670` | 5 | `SoundSys_EvaluateTrigger_5` | 24 | 2 | 3 | 1 | absent |
| 289 | `0x4e6690` | 5 | `SoundSys_AttachLipsync_5` | 24 | 2 | 3 | 1 | absent |
| 290 | `0x4e66b0` | 5 | `SoundSys_InitSoundSys_6` | 24 | 2 | 3 | 1 | absent |
| 291 | `0x4e66d0` | 5 | `SoundSys_PlaySoundSample_6` | 24 | 2 | 3 | 1 | absent |
| 292 | `0x4e66f0` | 5 | `SoundSys_Update3DPosition_6` | 24 | 2 | 3 | 1 | absent |
| 293 | `0x4e6710` | 5 | `SoundSys_SetMasterVolume_6` | 24 | 2 | 3 | 1 | absent |
| 294 | `0x4e6730` | 5 | `SoundSys_SetFrequency_6` | 27 | 2 | 3 | 1 | absent |
| 295 | `0x4e7350` | 5 | `SoundSys_Update3DPosition_9` | 16 | 1 | 0 | 0 | absent |
| 296 | `0x4e7370` | 4 | `Screen_Init` | 133 | 1 | 0 | 8 | absent |
| 297 | `0x4e7530` | 3 | `StatusScreen_GetEvent` | 5 | 1 | 0 | 0 | absent |
| 298 | `0x4e7600` | 5 | `SoundSys_AttachLipsync_9` | 80 | 4 | 4 | 2 | absent |
| 299 | `0x4e7830` | 5 | `SoundSys_EvaluateTrigger_10` | 12 | 1 | 0 | 0 | absent |
| 300 | `0x4e7840` | 4 | `DebugText_Init` | 178 | 1 | 0 | 10 | absent |
| 301 | `0x4e7c50` | 4 | `CameraControlTask_Init` | 63 | 1 | 0 | 4 | absent |
| 302 | `0x4e7d90` | 4 | `ViewportQTask_Init` | 131 | 1 | 0 | 9 | absent |
| 303 | `0x4e7e20` | 5 | `ViewportQTask_Process` | 10 | 1 | 0 | 0 | absent |
| 304 | `0x4e8290` | 3 | `SoundSys_InitSoundSys_13` | 35 | 5 | 5 | 0 | absent |
| 305 | `0x4e82c0` | 4 | `TerraDrawTask_Init` | 77 | 1 | 0 | 5 | absent |
| 306 | `0x4e8390` | 4 | `SoundDefSound_Init` | 388 | 1 | 0 | 27 | absent |
| 307 | `0x4e92c0` | 4 | `FadeOutEnd_Init` | 145 | 1 | 0 | 9 | absent |
| 308 | `0x4e9480` | 4 | `GlowTask_Init` | 77 | 1 | 0 | 5 | absent |
| 309 | `0x4e9520` | 4 | `SoundDefTriggerOnce_Init` | 562 | 1 | 0 | 39 | absent |
| 310 | `0x4ea3d0` | 4 | `SoundDefGraph_Init` | 388 | 1 | 0 | 27 | absent |
| 311 | `0x4eb1d0` | 4 | `SoundDefGroup_Init` | 385 | 1 | 0 | 27 | absent |
| 312 | `0x4ebc60` | 4 | `SoundSysMicrophone_Init` | 102 | 1 | 0 | 6 | absent |
| 313 | `0x4ebdd0` | 5 | `SoundSysMicrophone_Process` | 5 | 1 | 0 | 0 | absent |
| 314 | `0x4ebe70` | 3 | `SoundSys_Defineanimtrigger` | 58 | 1 | 0 | 4 | absent |
| 315 | `0x4ec040` | 3 | `SoundSys_SetFrequency_15` | 5 | 1 | 0 | 0 | absent |
| 316 | `0x4ec050` | 3 | `SoundSys_StopSoundSample_15` | 5 | 1 | 0 | 0 | absent |
| 317 | `0x4ec060` | 3 | `SoundSys_LoadSoundDef_15` | 5 | 1 | 0 | 0 | absent |
| 318 | `0x4ec160` | 3 | `SoundSys_AttachLipsync_15` | 25 | 1 | 0 | 1 | absent |
| 319 | `0x4ed000` | 3 | `SoundSys_SetFrequency_17` | 10 | 1 | 0 | 0 | absent |
| 320 | `0x4ed510` | 2 | `InputPort_Init` | 70 | 1 | 0 | 3 | absent |
| 321 | `0x4ed6b0` | 2 | `LODSettings_Init` | 949 | 1 | 0 | 49 | absent |
| 322 | `0x4edce0` | 2 | `SoundSys_Definephysicsobjtype` | 58 | 1 | 0 | 4 | absent |
| 323 | `0x4ee0e0` | 3 | `SoundSys_Update3DPosition_18` | 5 | 1 | 0 | 0 | absent |
| 324 | `0x4ee0f0` | 3 | `SoundSys_SetMasterVolume_18` | 5 | 1 | 0 | 0 | absent |
| 325 | `0x4ee100` | 3 | `SoundSys_SetFrequency_18` | 5 | 1 | 0 | 0 | absent |
| 326 | `0x4ee410` | 2 | `ScreenGrab_Init` | 272 | 1 | 0 | 16 | absent |
| 327 | `0x4ee970` | 2 | `CameraBase_Init` | 3708 | 3 | 3 | 263 | absent |
| 328 | `0x4f0ef0` | 3 | `Script_SpawnNewTask_2` | 5 | 1 | 0 | 0 | absent |
| 329 | `0x4f0f20` | 3 | `Script_ParseEnumString_2` | 5 | 1 | 0 | 0 | absent |
| 330 | `0x4f0f30` | 3 | `Script_ExecuteLevelScript_2` | 5 | 1 | 0 | 0 | absent |
| 331 | `0x4f0f40` | 3 | `Script_CallNativeHandler_2` | 5 | 1 | 0 | 0 | absent |
| 332 | `0x4f0f50` | 3 | `Script_EvaluateCondition_2` | 5 | 1 | 0 | 0 | absent |
| 333 | `0x4f0f60` | 3 | `Script_FormatString_2` | 5 | 1 | 0 | 0 | absent |
| 334 | `0x4f0f70` | 3 | `Script_SpawnNewTask_3` | 5 | 1 | 0 | 0 | absent |
| 335 | `0x4f0f80` | 2 | `Script_ParseEnumInt32_3` | 5 | 1 | 0 | 0 | **landed** `native_4f0f80.cpp` |
| 336 | `0x4f0fa0` | 3 | `Script_ParseEnumString_3` | 5 | 1 | 0 | 0 | absent |
| 337 | `0x4f0fc0` | 3 | `Script_CallNativeHandler_3` | 5 | 1 | 0 | 0 | absent |
| 338 | `0x4f0fd0` | 3 | `Script_EvaluateCondition_3` | 5 | 1 | 0 | 0 | absent |
| 339 | `0x4f0fe0` | 3 | `Script_FormatString_3` | 5 | 1 | 0 | 0 | absent |
| 340 | `0x4f0ff0` | 3 | `Script_SpawnNewTask_4` | 5 | 1 | 0 | 0 | absent |
| 341 | `0x4f1000` | 2 | `Script_ParseEnumInt32_4` | 5 | 1 | 0 | 0 | **landed** `native_4f1000.cpp` |
| 342 | `0x4f1010` | 3 | `Script_ParseEnumReal32_4` | 5 | 1 | 0 | 0 | absent |
| 343 | `0x4f1020` | 3 | `Script_ParseEnumString_4` | 5 | 1 | 0 | 0 | absent |
| 344 | `0x4f11f0` | 3 | `Script_CallNativeHandler_5` | 5 | 1 | 0 | 0 | absent |
| 345 | `0x4f1210` | 3 | `Script_FormatString_5` | 5 | 1 | 0 | 0 | absent |
| 346 | `0x4f1220` | 3 | `Script_SpawnNewTask_6` | 5 | 1 | 0 | 0 | absent |
| 347 | `0x4f1840` | 2 | `Script_ExecuteLevelScript_7` | 122 | 1 | 0 | 9 | absent |
| 348 | `0x4f1950` | 3 | `Script_EvaluateCondition_7` | 40 | 1 | 0 | 1 | absent |
| 349 | `0x4f1980` | 2 | `Script_FormatString_7` | 56 | 1 | 0 | 3 | absent |
| 350 | `0x4f19c0` | 3 | `Script_UnableToAllocatePara` | 120 | 8 | 10 | 2 | absent |
| 351 | `0x4f1a60` | 2 | `Script_ParseEnumInt32_8` | 5 | 1 | 0 | 0 | **landed** `native_4f1a60.cpp` |
| 352 | `0x4f1a70` | 2 | `Script_ParseEnumReal32_8` | 5 | 1 | 0 | 0 | **landed** `native_4f1a70.cpp` |
| 353 | `0x4f1a80` | 2 | `Script_UndefinedParameterTy` | 104 | 4 | 4 | 1 | absent |
| 354 | `0x4f1b20` | 3 | `Script_UnableToAllocatePara_2` | 40 | 6 | 7 | 1 | absent |
| 355 | `0x4f1b60` | 3 | `Script_CallNativeHandler_8` | 5 | 1 | 0 | 0 | absent |
| 356 | `0x4f1b70` | 3 | `Script_EvaluateCondition_8` | 5 | 1 | 0 | 0 | absent |
| 357 | `0x4f1b80` | 3 | `Script_FormatString_8` | 5 | 1 | 0 | 0 | absent |
| 358 | `0x4f1b90` | 3 | `Script_SpawnNewTask_9` | 5 | 1 | 0 | 0 | absent |
| 359 | `0x4f1ba0` | 3 | `Script_ParseEnumInt32_9` | 5 | 1 | 0 | 0 | absent |
| 360 | `0x4f1bb0` | 3 | `Script_ParseEnumReal32_9` | 5 | 1 | 0 | 0 | absent |
| 361 | `0x4f1bc0` | 3 | `Script_ParseEnumString_9` | 85 | 6 | 8 | 1 | absent |
| 362 | `0x4f1c90` | 3 | `Script_CallNativeHandler_9` | 25 | 1 | 0 | 0 | absent |
| 363 | `0x4f1cb0` | 3 | `Script_EvaluateCondition_9` | 25 | 1 | 0 | 0 | absent |
| 364 | `0x4f1cd0` | 3 | `Script_FormatString_9` | 18 | 1 | 0 | 1 | absent |
| 365 | `0x4f1d60` | 4 | `Script_SpawnNewTask_10` | 25 | 1 | 0 | 0 | absent |
| 366 | `0x4f1db0` | 3 | `Script_ParseEnumReal32_10` | 70 | 1 | 0 | 3 | absent |
| 367 | `0x4f1e20` | 3 | `Script_CallNativeHandler_10` | 5 | 1 | 0 | 0 | absent |
| 368 | `0x4f1e30` | 2 | `Script_Enumint32` | 108 | 1 | 0 | 10 | absent |
| 369 | `0x4f1ee0` | 2 | `Script_FormatString_10` | 111 | 1 | 0 | 9 | **landed** `native_4f1ee0.cpp` |
| 370 | `0x4f2740` | 2 | `EditRigidObj_Init` | 481 | 1 | 0 | 30 | absent |
| 371 | `0x4f3440` | 2 | `AnimTask_Init` | 289 | 1 | 0 | 22 | absent |
| 372 | `0x4f36c0` | 2 | `Script_FormatString_13` | 635 | 1 | 0 | 36 | absent |
| 373 | `0x4f40d0` | 3 | `Script_ParseEnumReal32_15` | 5 | 1 | 0 | 0 | absent |
| 374 | `0x4f40e0` | 3 | `Script_ParseEnumString_15` | 5 | 1 | 0 | 0 | absent |
| 375 | `0x4f40f0` | 3 | `Script_ExecuteLevelScript_15` | 5 | 1 | 0 | 0 | absent |
| 376 | `0x4f4100` | 3 | `Script_CallNativeHandler_15` | 6 | 1 | 0 | 0 | absent |
| 377 | `0x4f4110` | 3 | `Script_EvaluateCondition_15` | 5 | 1 | 0 | 0 | absent |
| 378 | `0x4f4120` | 3 | `Script_FormatString_15` | 5 | 1 | 0 | 0 | absent |
| 379 | `0x4f4130` | 3 | `Script_SpawnNewTask_16` | 5 | 1 | 0 | 0 | absent |
| 380 | `0x4f4140` | 3 | `Script_ParseEnumInt32_16` | 5 | 1 | 0 | 0 | absent |
| 381 | `0x4f4190` | 2 | `Vehicle_Init` | 306 | 1 | 0 | 20 | absent |
| 382 | `0x4f4b00` | 2 | `CutScene_TaskCreate` | 977 | 1 | 0 | 59 | absent |
| 383 | `0x4f7360` | 2 | `RainDynCubeObj_Init` | 693 | 1 | 0 | 40 | absent |
| 384 | `0x4f8340` | 2 | `TerrainLightMap_Init` | 927 | 1 | 0 | 51 | absent |
| 385 | `0x4f9200` | 2 | `Script_EvaluateCondition_21` | 5 | 1 | 0 | 0 | **landed** `native_4f9200.cpp` |
| 386 | `0x4f9210` | 2 | `Script_FormatString_21` | 6 | 1 | 0 | 0 | **landed** `native_4f9210.cpp` |
| 387 | `0x4f9220` | 2 | `Script_SpawnNewTask_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9220.cpp` |
| 388 | `0x4f9230` | 2 | `Script_ParseEnumInt32_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9230.cpp` |
| 389 | `0x4f9240` | 2 | `Script_ParseEnumReal32_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9240.cpp` |
| 390 | `0x4f9250` | 2 | `Script_ParseEnumString_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9250.cpp` |
| 391 | `0x4f9260` | 2 | `Script_ExecuteLevelScript_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9260.cpp` |
| 392 | `0x4f9270` | 3 | `Script_CallNativeHandler_22` | 5 | 1 | 0 | 0 | absent |
| 393 | `0x4f9280` | 2 | `Script_EvaluateCondition_22` | 5 | 1 | 0 | 0 | **landed** `native_4f9280.cpp` |
| 394 | `0x4f9460` | 2 | `Script_NoFreeNodesizeElemen` | 99 | 11 | 14 | 1 | absent |
| 395 | `0x4f9df0` | 2 | `Script_GraphstreamCollision` | 119 | 7 | 9 | 4 | absent |
| 396 | `0x4f9e70` | 3 | `Script_ExecuteLevelScript_24` | 59 | 6 | 9 | 0 | absent |
| 397 | `0x4faa60` | 2 | `Graph_Init` | 836 | 5 | 6 | 46 | absent |
| 398 | `0x4fb1b0` | 2 | `PhysicsObj_Init` | 215 | 1 | 0 | 13 | absent |
| 399 | `0x4fb5e0` | 3 | `Script_SpawnNewTask_28` | 6 | 1 | 0 | 0 | absent |
| 400 | `0x4fc850` | 3 | `Script_ExecuteLevelScript_29` | 5 | 1 | 0 | 0 | absent |
| 401 | `0x4fcf50` | 2 | `Particle2DDynCubeObjTask_Init` | 198 | 1 | 0 | 10 | absent |
| 402 | `0x4fe7c0` | 2 | `TagItem_Init` | 659 | 1 | 0 | 39 | absent |
| 403 | `0x502250` | 2 | `SplineObj_Init` | 1161 | 1 | 0 | 66 | absent |
| 404 | `0x5033f0` | 2 | `EditorRigidObj_Init` | 195 | 1 | 0 | 14 | absent |
| 405 | `0x5034d0` | 3 | `EditorRigidObj_OnEvent` | 6 | 1 | 0 | 0 | absent |
| 406 | `0x5034e0` | 2 | `SimpointObj_Init` | 200 | 1 | 0 | 12 | absent |
| 407 | `0x503d60` | 2 | `PhysicsMagicObj_Init` | 134 | 1 | 0 | 8 | absent |
| 408 | `0x5043b0` | 2 | `AreaActivate_Init` | 374 | 1 | 0 | 23 | absent |
| 409 | `0x504a30` | 3 | `Physics_UnableToRegisterArea` | 92 | 6 | 7 | 2 | absent |
| 410 | `0x504c50` | 2 | `EditRigidObjAttachedToBone_Init` | 308 | 1 | 0 | 18 | absent |
| 411 | `0x504df0` | 3 | `EditRigidObjAttachedToBone_Action` | 5 | 1 | 0 | 0 | absent |
| 412 | `0x504e00` | 3 | `EditRigidObjAttachedToBone_OnTick` | 5 | 1 | 0 | 0 | absent |
| 413 | `0x504e10` | 2 | `MenuItemTask_Init` | 208 | 1 | 0 | 15 | absent |
| 414 | `0x505630` | 2 | `MenuItem_Init` | 216 | 1 | 0 | 16 | absent |
| 415 | `0x505c50` | 3 | `Physics_CalculateSplineLength_5` | 5 | 1 | 0 | 0 | absent |
| 416 | `0x505c60` | 3 | `Physics_InterpolateWaypoint_5` | 5 | 1 | 0 | 0 | absent |
| 417 | `0x505c70` | 3 | `Physics_EvaluateAreaCriteria_5` | 5 | 1 | 0 | 0 | absent |
| 418 | `0x505c80` | 3 | `Physics_SimulateRigidBody_5` | 6 | 1 | 0 | 0 | absent |
| 419 | `0x505c90` | 2 | `Container_Init` | 150 | 1 | 0 | 11 | absent |
| 420 | `0x505f60` | 2 | `TextureModifier_Init` | 729 | 1 | 0 | 41 | absent |
| 421 | `0x506fa0` | 2 | `HeightMap_Init` | 921 | 1 | 0 | 52 | absent |
| 422 | `0x507e70` | 2 | `Physics_SimulateRigidBody_6` | 15 | 1 | 0 | 1 | **landed** `native_507e70.cpp` |
| 423 | `0x507f10` | 2 | `EditCamera_Init` | 886 | 1 | 0 | 48 | absent |
| 424 | `0x508d60` | 3 | `Physics_SimulateRigidBody_7` | 5 | 1 | 0 | 0 | absent |
| 425 | `0x508d70` | 2 | `Sync_Init` | 143 | 1 | 0 | 9 | absent |
| 426 | `0x508f60` | 3 | `Sync_Process` | 5 | 1 | 0 | 0 | absent |
| 427 | `0x508f70` | 3 | `Physics_CheckHullCollision_7` | 5 | 1 | 0 | 0 | absent |
| 428 | `0x508f80` | 0 | `Physics_ApplyImpulse_7` | 9 | 1 | 1 | 2 | absent |
| 429 | `0x508fa0` | 1 | `AIGraph_Init` | 754 | 1 | 0 | 43 | absent |
| 430 | `0x5098b0` | 9 | `Physics_Local` | 18 | 1 | 0 | 1 | absent |
| 431 | `0x509910` | 9 | `Physics_CheckHullCollision_9` | 9 | 1 | 1 | 2 | absent |
| 432 | `0x509930` | 9 | `Physics_QvertexDrawstrip` | 32 | 1 | 1 | 4 | absent |
| 433 | `0x509980` | 8 | `Physics_CouldNotCreateDirect` | 184 | 14 | 19 | 7 | absent |
| 434 | `0x509cc0` | 9 | `Physics_CalculateSplineLength_10` | 40 | 3 | 2 | 0 | **landed** `native_509cc0.cpp` |
| 435 | `0x509f00` | 9 | `Physics_SimulateRigidBody_10` | 67 | 1 | 0 | 0 | **landed** `native_509f00.cpp` |
| 436 | `0x50a6c0` | 10 | `Physics_ApplyImpulse_11` | 360 | 1 | 0 | 32 | absent |
| 437 | `0x50bd80` | 10 | `Physics_ApplyImpulse_12` | 130 | 1 | 0 | 7 | absent |
| 438 | `0x50dc30` | 10 | `Physics_SimulateRigidBody_13` | 136 | 1 | 0 | 7 | absent |
| 439 | `0x50ec00` | 10 | `Physics_CalculateSplineLength_14` | 51 | 1 | 0 | 4 | absent |
| 440 | `0x50fac0` | 7 | `Physics_CheckHullCollision_14` | 22 | 1 | 1 | 2 | absent |
| 441 | `0x5125a0` | 6 | `Terrain_BuildMipMaps` | 30 | 1 | 0 | 4 | absent |
| 442 | `0x512820` | 7 | `Terrain_Glowreadbufferprimit` | 251 | 10 | 13 | 4 | absent |
| 443 | `0x516600` | 8 | `Terrain_RenderQuadtree_3` | 62 | 2 | 3 | 3 | absent (mentioned: terrain_storage.cpp) |
| 444 | `0x5168e0` | 10 | `Terrain_CalculateNormals_3` | 19 | 1 | 0 | 2 | absent |
| 445 | `0x5168fd` | 11 | `Terrain_BuildMipMaps_3` | 173 | 26 | 36 | 4 | absent |
| 446 | `0x518260` | 6 | `Terrain_RenderWaterSurface_8` | 42 | 11 | 14 | 3 | absent |
| 447 | `0x5183d0` | 1 | `Terrain_CalculateNormals_8` | 369 | 1 | 1 | 74 | absent |
| 448 | `0x5186c0` | 2 | `Terrain_RenderQuadtree_9` | 83 | 6 | 7 | 2 | absent |
| 449 | `0x518760` | 2 | `GraphNode_Init` | 265 | 3 | 3 | 15 | absent |
| 450 | `0x518ef0` | 2 | `Water_Init` | 743 | 3 | 3 | 42 | absent |
| 451 | `0x519710` | 2 | `GFXCapsViewer_Init` | 56 | 1 | 0 | 3 | absent |
| 452 | `0x519760` | 2 | `ShadowTask_Init` | 169 | 1 | 0 | 11 | absent |
| 453 | `0x519850` | 2 | `SplinePathNodeQTask_Init` | 376 | 1 | 0 | 22 | absent |
| 454 | `0x51a1d0` | 2 | `SplinePathDynCubeObj_Init` | 95 | 1 | 0 | 5 | absent |
| 455 | `0x51b140` | 2 | `SplinePathGuideQTask_Init` | 369 | 1 | 0 | 23 | absent |
| 456 | `0x51b400` | 2 | `AnimSound_Init` | 166 | 1 | 0 | 11 | absent |
| 457 | `0x51b960` | 2 | `EditorMagicObj_Init` | 243 | 1 | 0 | 16 | absent |
| 458 | `0x51bac0` | 2 | `LevelTimer_Init` | 413 | 1 | 0 | 25 | absent |
| 459 | `0x51bfb0` | 2 | `SoundManager_Init` | 213 | 1 | 0 | 14 | absent |
| 460 | `0x51c2b0` | 2 | `FlatWater_Init` | 752 | 1 | 0 | 41 | absent |
| 461 | `0x51ddf0` | 2 | `FlatSky_Init` | 1018 | 1 | 0 | 55 | absent |
| 462 | `0x51ecb0` | 2 | `MipMapControl_Init` | 337 | 1 | 0 | 19 | absent |
| 463 | `0x51eee0` | 2 | `SmoothQTask_Init` | 606 | 1 | 0 | 35 | absent |
| 464 | `0x51f580` | 2 | `MoveRigidObj_Init` | 642 | 3 | 3 | 35 | absent |
| 465 | `0x51fef0` | 2 | `NoiseQTask_Init` | 771 | 1 | 0 | 44 | absent |
| 466 | `0x5207a0` | 2 | `Origo_Init` | 352 | 1 | 0 | 22 | absent |
| 467 | `0x520bb0` | 2 | `Lensflare_Init` | 642 | 3 | 3 | 37 | absent |
| 468 | `0x526330` | 2 | `LightmapInfo_Init` | 1322 | 5 | 6 | 71 | absent |
| 469 | `0x526ee0` | 2 | `Dirlight_Init` | 629 | 1 | 0 | 39 | absent |
| 470 | `0x5274c0` | 3 | `Light_SampleLightmap_5` | 6 | 1 | 0 | 0 | absent |
| 471 | `0x527c90` | 2 | `GlobalLight_Init` | 1103 | 6 | 7 | 58 | absent |
| 472 | `0x5284a0` | 2 | `AnimCam_Init` | 690 | 1 | 0 | 42 | absent |
| 473 | `0x52aaf0` | 2 | `AnimRigidDynCubeObj_Init` | 219 | 1 | 0 | 15 | absent |
| 474 | `0x52ad10` | 2 | `AnimScaleTask_Init` | 304 | 1 | 0 | 18 | absent |
| 475 | `0x52b040` | 2 | `EditHazinglayer_Init` | 368 | 1 | 0 | 22 | absent |
| 476 | `0x52b3b0` | 2 | `CubeModifier_Init` | 282 | 1 | 0 | 17 | absent |
| 477 | `0x52b6c0` | 2 | `DiscardTerrainArea_Init` | 290 | 1 | 0 | 18 | absent |
| 478 | `0x52c0c0` | 2 | `DiscardTerrain_Init` | 452 | 1 | 0 | 28 | absent |
| 479 | `0x52c6b0` | 2 | `SkyTask_Init` | 222 | 1 | 0 | 14 | absent |
| 480 | `0x52c820` | 2 | `LevelScript_Init` | 188 | 1 | 0 | 13 | absent |
| 481 | `0x52ca30` | 2 | `SimBoneDynCubeObj_Init` | 34 | 1 | 0 | 2 | absent |
| 482 | `0x52ca70` | 2 | `PushButton_Init` | 84 | 1 | 0 | 6 | absent |
| 483 | `0x52d360` | 6 | `Light_RenderLensflare_10` | 11 | 1 | 0 | 1 | absent |
| 484 | `0x52d420` | 6 | `Light_UpdateGlobalLight_10` | 18 | 1 | 0 | 1 | absent |
| 485 | `0x52dd70` | 7 | `Light_Watermesh` | 29 | 1 | 0 | 2 | absent |
| 486 | `0x530220` | 2 | `CamTask_Init` | 34 | 1 | 0 | 2 | **landed** `cam_task.cpp` |
| 487 | `0x530260` | 3 | `CamTask_OnEvent` | 6 | 1 | 0 | 0 | absent |
| 488 | `0x530270` | 2 | `CamTask_Process` | 34 | 2 | 1 | 7 | absent |
| 489 | `0x530800` | 3 | `Particle_Render3DSprites_2` | 15 | 1 | 0 | 1 | absent |
| 490 | `0x530950` | 4 | `Particle_EmitParticles_3` | 15 | 1 | 0 | 0 | absent |
| 491 | `0x530960` | 3 | `Particle_UpdateParticlePhysics_3` | 27 | 1 | 0 | 2 | absent |
| 492 | `0x5309c0` | 3 | `Particle_TriggerMuzzleFlash_3` | 94 | 2 | 1 | 19 | absent |
| 493 | `0x530a80` | 4 | `FlashTask_Init` | 73 | 1 | 0 | 4 | absent |
| 494 | `0x530c80` | 4 | `ParticleSystemDynCubeObj_Init` | 78 | 1 | 0 | 4 | absent |
| 495 | `0x5310d0` | 5 | `Particle_EmitParticles_4` | 6 | 1 | 0 | 0 | absent |
| 496 | `0x5310e0` | 4 | `FreeCamera_Init` | 66 | 1 | 0 | 4 | absent |
| 497 | `0x531190` | 4 | `SpriteParticleSystemDynCubeObj_Init` | 44 | 1 | 0 | 2 | absent |
| 498 | `0x5311e0` | 4 | `SpriteParticleSystemDynCubeObj_OnEvent` | 9 | 4 | 4 | 2 | absent |
| 499 | `0x531300` | 6 | `PointlightDynCubeObj_Init` | 224 | 1 | 0 | 10 | absent |
| 500 | `0x531540` | 5 | `Particle_UpdateParticlePhysics_4` | 99 | 4 | 4 | 20 | absent |
| 501 | `0x5317d0` | 6 | `BoneMagicObj_Init` | 252 | 1 | 0 | 16 | absent |
| 502 | `0x531d30` | 6 | `Particle_FadeParticles_4` | 49 | 1 | 1 | 10 | absent |
| 503 | `0x531db0` | 7 | `Particle_EmitParticles_5` | 14 | 2 | 1 | 3 | absent |
| 504 | `0x531dd0` | 8 | `Particle_Render3DSprites_5` | 24 | 2 | 1 | 5 | absent |
| 505 | `0x531ef0` | 6 | `Sprite3D_Init` | 67 | 1 | 0 | 3 | absent |
| 506 | `0x531fa0` | 7 | `TransparencyList_Init` | 135 | 1 | 0 | 8 | absent |
| 507 | `0x5323c0` | 5 | `Particle_UpdateParticlePhysics_6` | 14 | 1 | 1 | 3 | absent |
| 508 | `0x5323e0` | 6 | `Particle_TriggerMuzzleFlash_6` | 9 | 2 | 1 | 2 | absent |
| 509 | `0x532400` | 7 | `Particle_FadeParticles_6` | 129 | 2 | 1 | 25 | absent |
| 510 | `0x53301c` | 10 | `?` | ? | None | None | 0 | absent |
| 511 | `0x533038` | 10 | `?` | ? | None | None | 0 | absent |
| 512 | `0x53303c` | 10 | `?` | ? | None | None | 0 | absent |
| 513 | `0x533044` | 11 | `?` | ? | None | None | 0 | absent |
| 514 | `0x533060` | 6 | `?` | ? | None | None | 0 | absent |
| 515 | `0x533090` | 6 | `?` | ? | None | None | 0 | absent |
| 516 | `0x533098` | 6 | `?` | ? | None | None | 0 | absent |
| 517 | `0x53309c` | 15 | `?` | ? | None | None | 0 | absent |
| 518 | `0x5330a4` | 9 | `?` | ? | None | None | 0 | absent |
| 519 | `0x5330ac` | 7 | `?` | ? | None | None | 0 | absent |
| 520 | `0x5330b0` | 8 | `?` | ? | None | None | 0 | absent |
| 521 | `0x5330b4` | 9 | `?` | ? | None | None | 0 | absent |
| 522 | `0x5330bc` | 9 | `?` | ? | None | None | 0 | absent |
| 523 | `0x5330c8` | 10 | `?` | ? | None | None | 0 | absent |
| 524 | `0x5330cc` | 11 | `?` | ? | None | None | 0 | absent |
| 525 | `0x5330d0` | 11 | `?` | ? | None | None | 0 | absent |
| 526 | `0x5330d4` | 10 | `?` | ? | None | None | 0 | absent |
| 527 | `0x5330d8` | 10 | `?` | ? | None | None | 0 | absent |
| 528 | `0x5330ec` | 9 | `?` | ? | None | None | 0 | absent |
| 529 | `0x533120` | 4 | `?` | ? | None | None | 0 | absent |
| 530 | `0x533128` | 6 | `?` | ? | None | None | 0 | absent |
| 531 | `0x5331f0` | 10 | `?` | ? | None | None | 0 | absent |
| 532 | `0x5331fc` | 10 | `?` | ? | None | None | 0 | absent |
| 533 | `0x533200` | 10 | `?` | ? | None | None | 0 | absent |
| 534 | `0x533204` | 10 | `?` | ? | None | None | 0 | absent |
| 535 | `0x533208` | 10 | `?` | ? | None | None | 0 | absent |
| 536 | `0x53320c` | 10 | `?` | ? | None | None | 0 | absent |
| 537 | `0x533210` | 10 | `?` | ? | None | None | 0 | absent |

### 3.1 Edge list (direct callees per function, in BN order)

```
0x4010d0 -> 0xc35044, 0x4011d0, 0x4011b0, 0xc35044, 0x401900, 0x4018f0, 0x401860, 0x4018f0, 0x401890, 0x4018f0, 0x401890, 0x4018f0, 0x401890, 0xc35044, 0x4b0b00, 0x4b8810
0x401400 -> 0x401400, 0x4014b0
0x4014b0 -> 0x4014e0
0x401530 -> 0x401530, 0x4014b0
0x4017c0 -> 0x4af7b0
0x401810 -> 0x4af7b0
0x401900 -> 0x4af7b0, 0x4b0b00, 0x4014b0, 0x4014b0
0x401d30 -> 0x4014b0
0x48f020 -> 0x5330ec, 0x4a5980, 0x48f2b0, 0x48f180
0x48f490 -> 0x4af7b0, 0x4af8f0
0x48fd90 -> 0x4af7b0
0x48fde0 -> 0x48f090, 0x4ae730, 0x4af7b0, 0x4af7b0, 0x4af7b0, 0x48f0a0, 0x4af7b0, 0x48fd90, 0x48f490
0x48fff0 -> 0x48f090, 0x4af740, 0x4af7b0, 0x4af7b0, 0x4af7b0, 0x48f0a0, 0x4af7b0, 0x490340, 0x490350, 0xc35044, 0x48f490
0x490360 -> 0x533120
0x490370 -> 0x533120
0x490380 -> 0x4af758, 0xc35044
0x491630 -> 0x491650
0x491650 -> 0x4af7b0
0x4916c0 -> 0x491650
0x491700 -> 0x4af7b0, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x491860, 0x4c1780, 0x4978d0, 0x4978d0, 0x4978d0, 0x493580, 0x498080, 0x491df0, 0x4b0ee0, 0x4af860, 0x4af880, 0x4af890
0x493580 -> 0x4b0e00, 0x491df0, 0x4b0ec0, 0x491e00, 0x4b0ec0, 0x491e10, 0x4b0ec0, 0x491e20, 0x4b0ec0, 0x491e30, 0x4b0ec0, 0x491e40, 0x4b0ec0, 0x491e50, 0x4b0ec0, 0x491e60, 0x4b0ec0, 0x491dc0, 0x4b0ec0, 0x491db0, 0x4b0ec0, 0x491dd0, 0x4b0ec0, 0x491de0, 0x4b0ec0, 0xc35044, 0xc35040, 0xc35040, 0x4b73f0, 0x4b73f0, 0x4b73f0, 0x4b73f0, 0x4b7430, 0x4b7430, 0x48f490, 0x495200
0x495170 -> 0x495190
0x495190 -> 0x4af7b0
0x4951e0 -> 0x495190
0x495200 -> 0x495190
0x495280 -> 0x4952a0, 0x495640, 0x4952f0, 0x495a60
0x4952a0 -> 0x533210
0x4952f0 -> 0x4955f0, 0x4af746, 0x48f0a0, 0xc35044, 0xc35044, 0x4b0c60, 0xc35044, 0xc35044, 0x495620
0x495640 -> 0x5331fc, 0xc35044, 0x533200, 0x533204, 0xc35044, 0x533208, 0xc35040, 0xc35044, 0x533208, 0x4b0c60, 0x5331f0, 0xc35040, 0xc35040, 0x4b0c60, 0x53320c, 0x4b0d10, 0x4b0d10
0x497570 -> 0x497680, 0x4b0c60, 0x4b0c60, 0x497880, 0x497880
0x497680 -> 0x491db0, 0x497520
0x4978d0 -> 0x4b0e70
0x4978f0 -> 0x4b0ee0
0x497980 -> 0x497ed0, 0x4978d0, 0x4b0ee0, 0x4b0e70, 0x4b0e70, 0x497940, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0
0x498080 -> 0x4b0e00, 0x491df0, 0x4b0ec0, 0x491e00, 0x4b0ec0, 0x491e50, 0x4b0ec0, 0x491e40, 0x4b0ec0, 0x491dc0, 0x4b0ec0, 0x491db0, 0x4b0ec0
0x498900 -> 0x4af8a0, 0x4af860, 0x4af880, 0x4af890
0x498950 -> 0x4af8c0, 0x498900
0x498ab0 -> 0x4b4720, 0x401900, 0x401400, 0x495200
0x49a520 -> 0x401900, 0x401400, 0x401530, 0x4978d0, 0x4950e0, 0x4b0ec0, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430
0x49bb40 -> 0x4cfc30, 0x4950e0, 0x4b0ec0, 0x4cfc40, 0x4950e0, 0x4b0ec0, 0x4cfc50, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4cfc60, 0x4950e0, 0x4b0ec0, 0x4cea80, 0x4950e0, 0x4b0ec0, 0x4cea90, 0x4950e0, 0x4b0ec0, 0x4cea60, 0x4950e0, 0x4b0ec0, 0x4cea70, 0x4950e0, 0x4b0ec0, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430, 0x4b7430, 0x495200, 0x4d14d0, 0x4e5350, 0x401400
0x4a1470 -> 0x4c3450, 0x4d14d0, 0x4c48c0, 0x401400, 0x4d97e0, 0x4c48c0, 0x401400
0x4a2a00 -> 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4e7530, 0x401530, 0x401530, 0x401530, 0x4e7530, 0x401530, 0x401530, 0x4e7530, 0x401530, 0x401530, 0x401530, 0x4e7530, 0x401530, 0x401530, 0x401530, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4978d0, 0x4978d0, 0x4978d0, 0x4978d0, 0x4978f0, 0x4978f0, 0x4978f0, 0x4978f0, 0x4b4720, 0x4b4770
0x4a53b3 -> 0x4a7720, 0x4a760b
0x4a5702 -> 0x4a7720, 0x4a760b
0x4a5980 -> 0x4a9fc1, 0x4a9fc1
0x4a6a50 -> 0x4aad90, 0x4aadbb, 0x4ab7f5, 0x4ab84c, 0x5330a4
0x4a6d7d -> 0x4abe33, 0x533098, 0x533128, 0x4abc46
0x4a760b -> 0x4ad0ae, 0x4ac323, 0x4abec7, 0x4a6d7d, 0x4abec7
0x4a7720 -> 0x4a7eff, 0x4a7eff, 0x4a7e61, 0x4a7e61, 0x4a7eff, 0x4a7f1c, 0x4ad0d4, 0x4a7eff, 0x4a7eff, 0x4aa730, 0x4a7eff, 0x4a7f0c, 0x4a7eff, 0x4a7eff, 0x4a7eff, 0x4a7eff, 0x4ad1b0, 0x4ad140, 0x4a7e96, 0x4a7ec7, 0x4a7e96, 0x4ad0d4, 0x4a7ec7, 0x4a7ec7, 0x4a7e96
0x4a7e61 -> 0x4a760b
0x4a7e96 -> 0x4a7e61
0x4a7ec7 -> 0x4a7e61
0x4a7f2a -> 0x4ad225
0x4a80bf -> 0x4a87df, 0x4a87ed, 0x53309c
0x4a8372 -> 0x4a881f, 0x4a881f, 0x4a881f, 0x4a871e, 0x4a881f, 0x4a881f
0x4a871e -> 0x4a869b, 0x4a869b
0x4a9c57 -> 0x4ad371
0x4a9fc1 -> 0x4a7f2a, 0x4ad4d3
0x4aa670 -> 0x4aa682
0x4aa682 -> 0x4aa6ae, 0x4ad722
0x4aa6ae -> 0x4ab0e4, 0x4ab891, 0x5330ac
0x4aadbb -> 0x5330bc, 0x5330bc, 0x5330a4, 0x4acd70
0x4ab0e4 -> 0x4ab3ed, 0x4ab49e
0x4ab3ed -> 0x5330b4, 0x5330ac, 0x5330b0, 0x5330a4
0x4ab49e -> 0x5330b0
0x4ab599 -> 0x5330ac, 0x5330b0, 0x5330b0, 0x4acd10, 0x5330bc, 0x5330a4
0x4ab6dd -> 0x5330bc, 0x5330a4
0x4ab733 -> 0x5330bc, 0x4ab6dd
0x4ab84c -> 0x4ab733
0x4ab891 -> 0x4aba99, 0x4aba99, 0x5330b0, 0x4acd10, 0x4ab599
0x4abec7 -> 0x4a6d7d, 0x533090, 0x533128, 0x533090, 0x533128, 0x4abc46
0x4ac323 -> 0x4aa670
0x4acd70 -> 0xc35040, 0xc35040, 0xc35040, 0xc35040, 0xc35040, 0xc35040, 0xc35040, 0xc35040
0x4ad0d4 -> 0x533060
0x4ad225 -> 0x5330d0, 0x5330cc, 0x5330cc, 0x5330c8, 0x4a54a0, 0x4acd10, 0x5330c8, 0x5330d0
0x4ad371 -> 0x4a8372, 0x4a80bf, 0x4a87fc, 0x4ad36e, 0x4a8611
0x4ad4d3 -> 0x5330d8, 0x5330d4, 0x4ad6f7, 0x5330d4, 0x5330c8, 0x4a54a0, 0x5330c8, 0x5330d8, 0x5330d8, 0x4a54a0, 0x5330d8, 0x533060
0x4ad73d -> 0x4a7f2a, 0x4ad4d3
0x4ae730 -> 0x53303c
0x4af740 -> 0x533038
0x4af746 -> 0x533044
0x4af758 -> 0x53301c
0x4af7b0 -> 0x4a53b3, 0x4a5702
0x4af810 -> 0x4a53b3, 0x4a5702
0x4af8c0 -> 0x4af860, 0x4af8a0, 0x4af870, 0x4af880, 0x4af890, 0x4af8b0
0x4af9d0 -> 0x4affb0, 0x4aff90, 0xc35044, 0xc35044, 0xc35044, 0xc35040
0x4aff90 -> 0xc35044
0x4b0b00 -> 0x4b0c60
0x4b0ba0 -> 0x4b0d40
0x4b0c60 -> 0x4aa670, 0x4af7b0, 0x4b0ba0, 0x4af8f0
0x4b0d10 -> 0x4af960, 0x4a6a50
0x4b0dc0 -> 0xc35044, 0xc35044, 0xc35044
0x4b0e00 -> 0x4af7b0
0x4b0e70 -> 0x4af7b0
0x4b1200 -> 0x4b12d0, 0xc35044, 0x5098b0
0x4b2180 -> 0x4b2740, 0x4b26b0, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x509910
0x4b27d0 -> 0x5168e0, 0x5168e0
0x4b33a0 -> 0x4b38e0, 0xc35040
0x4b38e0 -> 0x4b3be0, 0x4b3ba0, 0xc35040, 0x4b3b60, 0xc35040
0x4b46a0 -> 0x4b27d0
0x4b47e0 -> 0x4b4720
0x4b59a0 -> 0x4b0e70, 0x509930
0x4b59f0 -> 0x4b5af0, 0x4b5b30, 0x4af8f0
0x4b5af0 -> 0x4ad73d
0x4b5e70 -> 0x4b0b00
0x4b6420 -> 0x4b59f0
0x4b73c0 -> 0x4978d0, 0x4978f0
0x4b73f0 -> 0x4b0c60
0x4b7630 -> 0x4b0c60, 0xc35044, 0xc35044, 0x4b6d40, 0x4b0c60, 0xc35044, 0xc35044
0x4b7c80 -> 0x4b0e70, 0x4b0e70, 0x4b0e70
0x4b8810 -> 0x4baac0
0x4b8890 -> 0x4baac0
0x4b8b20 -> 0xc35044, 0x4bacd0, 0x4b8970, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8810, 0x4b8890, 0x4b8890, 0x4b8a00, 0x4b89f0, 0x4bd430, 0x4bc920, 0x4bbc80, 0x4c0250
0x4baac0 -> 0x4b0d60, 0x4b0c60, 0xc35044, 0x4c0560, 0x4b0d90
0x4bacd0 -> 0x4c0360
0x4bbc80 -> 0x4c0900
0x4bc920 -> 0xc35044
0x4bd430 -> 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0
0x4c0250 -> 0x4c10e0
0x4c0360 -> 0x4b0c60, 0x4b0c60
0x4c04b0 -> 0x4c0530
0x4c0560 -> 0x4c04b0, 0x4af7b0, 0x4b0c60, 0xc35040, 0xc35040, 0x4c0530, 0x4af910
0x4c0900 -> 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0
0x4c10e0 -> 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0, 0x4bd3d0
0x4c1800 -> 0x4b0c60
0x4c3450 -> 0x401900, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4e55b0, 0x401400, 0x4cea10, 0x401400, 0x401400, 0x4b8890, 0x401f60, 0x401fa0
0x4c5900 -> 0x401860, 0x401860, 0x401860, 0x401860, 0x401860, 0x401860, 0x401d30, 0x401d30, 0x401d30, 0x401d30, 0x401d30, 0x401d30, 0x401d30, 0x4017c0, 0x4017c0, 0x401400, 0xc35044
0x4ceba0 -> 0x4af7b0, 0xc35040, 0xc35040
0x4ceeb0 -> 0xc35044, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70
0x4d0b30 -> 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x401810, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x4b0e70, 0x49a340
0x4d0ef0 -> 0x4b0c60, 0x401860, 0x401860, 0x401860, 0x401860, 0x401d30, 0x401d30, 0x4017c0, 0x4017c0, 0x4d2b80, 0x4d2bd0
0x4d2b80 -> 0x4af7b0
0x4d2bd0 -> 0x4af7b0
0x4d2c30 -> 0x4d38a0, 0x4b0b00, 0x4b0b00, 0x4b0b00, 0x4b0b00, 0x4b0b00
0x4d7550 -> 0x401900, 0x4017c0, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4cea10, 0x401400, 0x401400
0x4d9730 -> 0x401860, 0x401860, 0x401860, 0x401860, 0x4017c0, 0x4018f0, 0x401900
0x4d9810 -> 0x4017c0, 0x4c1800
0x4d9a00 -> 0x4c48c0, 0x401900, 0x4017c0, 0x4017c0, 0x401400, 0x4c56f0, 0x401400, 0x401530, 0x401400, 0x4d9fb0, 0x401400, 0x401400, 0x4c5700, 0x401400, 0x401530, 0x4b8810
0x4da1d0 -> 0x401900, 0x401400, 0x401400, 0x401400, 0x401400
0x4da880 -> 0x4b0b00
0x4dcf20 -> 0x4c48c0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4cea10, 0x401400, 0x4cea20, 0x401400, 0x401900, 0x401400, 0x4e55b0, 0x401400, 0x401400, 0x401530, 0x401400, 0x401400, 0x401400, 0x401400, 0x4d14c0, 0x401400, 0x401530, 0x4cfc70, 0x401400, 0x4cfc80, 0x401400, 0x4cfc90, 0x401400, 0x4d14d0, 0x401400, 0x4cea10, 0x401400, 0x4cea20, 0x401400, 0x4b0b00, 0x4b0b00, 0x4c48c0, 0x401900, 0x4cea10, 0x401400, 0x4cea20, 0x401400
0x4e0840 -> 0x4018f0, 0x401900, 0x401530, 0x4c56f0, 0x401400
0x4e0950 -> 0x4018f0, 0x401900, 0x4c56f0, 0x401400, 0x401530, 0x401530
0x4e41b0 -> 0x4c48c0, 0x401900, 0x401530, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4e55b0, 0x401400, 0x401f60, 0x401fa0
0x4e5370 -> 0x401900, 0x4017c0
0x4e55c0 -> 0x4018f0, 0x4017c0, 0x4b0b00
0x4e5d90 -> 0x4017c0, 0xc35044
0x4e6070 -> 0x4e7350, 0x4c1800, 0x4c1800, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x401400, 0x401530, 0x4e6630, 0x401400, 0x401400, 0x497510, 0x4b0c60, 0xc35044, 0xc35044, 0x4e6290, 0x4b8890, 0x4b8890, 0x4b8890, 0x4b8890, 0x495cb0
0x4e65f0 -> 0x4017c0
0x4e6610 -> 0x4017c0
0x4e6630 -> 0x4017c0
0x4e6650 -> 0x4017c0
0x4e6670 -> 0x4017c0
0x4e6690 -> 0x4017c0
0x4e66b0 -> 0x4017c0
0x4e66d0 -> 0x4017c0
0x4e66f0 -> 0x4017c0
0x4e6710 -> 0x4017c0
0x4e6730 -> 0x401810
0x4e7370 -> 0x401810, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4e7530, 0x401530, 0xc35044
0x4e7600 -> 0x4b0c60, 0x4b7630
0x4e7840 -> 0x4e7600, 0x4e7830, 0x491630, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4e7530, 0x401530, 0x4b6420
0x4e7c50 -> 0x4018f0, 0x401900, 0x401530, 0x401530
0x4e7d90 -> 0x4018f0, 0x401900, 0x401530, 0x401530, 0x4e7530, 0x401530, 0x4916c0, 0x4e7e20, 0xc35044
0x4e82c0 -> 0x4018f0, 0x401900, 0x4e7530, 0x401530, 0x401530
0x4e8390 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4ebdd0, 0x401400, 0x4e65f0, 0x401400, 0x4e6630, 0x401400, 0x4e6650, 0x401400, 0x4e6670, 0x401400, 0x4e6690, 0x401400, 0x4e66b0, 0x401400, 0x4e66d0, 0x401400, 0x4e66f0, 0x401400, 0x4e6710, 0x401400, 0x4e6730, 0x401530
0x4e92c0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401530, 0x4e7530, 0x401530, 0x4e7530, 0x401530
0x4e9480 -> 0x4018f0, 0x401900, 0x4e7530, 0x401530, 0x401530
0x4e9520 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4ebdd0, 0x401400, 0x4e65f0, 0x401400, 0x4e6630, 0x401400, 0x4e6650, 0x401400, 0x4e6670, 0x401400, 0x4e6690, 0x401400, 0x4e66b0, 0x401400, 0x4e66d0, 0x401400, 0x4e66f0, 0x401400, 0x4e6710, 0x401400, 0x4e6730, 0x401530, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4cea20, 0x401400, 0x4cea10, 0x401400
0x4ea3d0 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4ebdd0, 0x401400, 0x4e65f0, 0x401400, 0x4e6630, 0x401400, 0x4e6650, 0x401400, 0x4e6670, 0x401400, 0x4e6690, 0x401400, 0x4e66b0, 0x401400, 0x4e66d0, 0x401400, 0x4e66f0, 0x401400, 0x4e6710, 0x401400, 0x4e6730, 0x401530
0x4eb1d0 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4e65f0, 0x401400, 0x4e6610, 0x401400, 0x4e6630, 0x401400, 0x4e6650, 0x401400, 0x4e6670, 0x401400, 0x4e6690, 0x401400, 0x4e66b0, 0x401400, 0x4e66d0, 0x401400, 0x4e66f0, 0x401400, 0x4e6710, 0x401400, 0x4e6730, 0x401530
0x4ebc60 -> 0x4018f0, 0x401900, 0x4017c0, 0x401400, 0x401530, 0x401530
0x4ebe70 -> 0x4017c0, 0x4017c0, 0x4017c0, 0x4b8890
0x4ec160 -> 0x4017c0
0x4ed510 -> 0x4018f0, 0x401900, 0x401530
0x4ed6b0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x401530, 0x4f1950, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80
0x4edce0 -> 0x4017c0, 0x4017c0, 0x4017c0, 0x4b8890
0x4ee410 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4e7530, 0x401530, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0fc0, 0x4f1a80, 0x4f1000, 0x4f1a80
0x4ee970 -> 0xc35044, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x4017c0, 0x4017c0, 0x401810, 0x401810, 0x401810, 0x401530, 0x401400, 0x401530, 0x401530, 0x401400, 0x401530, 0x4cea20, 0x401400, 0x4cea10, 0x401400, 0x401530, 0x401400, 0x401530, 0x401530, 0x401400, 0x401530, 0x401530, 0x401400, 0x401530, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4f19c0, 0x4b8890, 0x4b8890
0x4f1840 -> 0xc35044, 0x4017c0, 0x4017c0, 0x4f1b20, 0x4f1b20, 0x4f1b20, 0x4f1b20, 0x4f1b20, 0x4f1b20
0x4f1950 -> 0x4b0c60
0x4f1980 -> 0x4f1950, 0xc35040, 0xc35040
0x4f19c0 -> 0x4af7b0, 0xc35040
0x4f1a80 -> 0x4af7b0
0x4f1b20 -> 0x4af7b0
0x4f1bc0 -> 0x4f1bc0
0x4f1cd0 -> 0x4f1d60
0x4f1db0 -> 0x4b0c60, 0x4af9d0, 0x4af8f0
0x4f1e30 -> 0x4f0fa0, 0x4f19c0, 0x4f0f50, 0x4f19c0, 0x4f0fd0, 0x4f19c0, 0x4f0fe0, 0x4f19c0, 0x4f0ff0, 0x4f19c0
0x4f1ee0 -> 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x401810, 0x401810, 0x4017c0
0x4f2740 -> 0x4e0830, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4c5800, 0x401400, 0x401400, 0x4cea20, 0x401400, 0x4cea10, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f1c90, 0x4f0f40, 0x4f1a80, 0x4f1c90, 0x4f0f40, 0x4f1a80, 0x4b8810
0x4f3440 -> 0x5034d0, 0x401900, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4f19c0, 0x4f1b60, 0x4f1bc0, 0x4f1b80, 0x4f1bc0, 0x4f1ba0, 0x4f1bc0, 0x4f1bb0, 0x4f1bc0, 0x4f1b90, 0x4f1bc0, 0x4f1b70, 0x4f1bc0
0x4f36c0 -> 0x4a54a0, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f4110, 0x401400, 0x4f4120, 0x401400, 0x4f4130, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1a80, 0x4c48c0, 0x401400
0x4f4190 -> 0x4fb5e0, 0x401900, 0x401400, 0x401530, 0x401530, 0x401400, 0x401400, 0x4f4120, 0x401400, 0x4ee0e0, 0x401400, 0x4ee0f0, 0x401400, 0x4cea10, 0x401400, 0x4cea20, 0x401400, 0x504a30, 0x4017c0, 0x4017c0
0x4f4b00 -> 0x4ceba0, 0x5034d0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f40d0, 0x401400, 0x4f40e0, 0x401400, 0x4f4110, 0x401400, 0x4f4140, 0x401400, 0x4f4130, 0x401400, 0x4f4120, 0x401400, 0x4cea20, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4b8810, 0x4b4720
0x4f7360 -> 0x4a54a0, 0x401900, 0x401400, 0x401530, 0x401530, 0x401400, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4978d0, 0x4978f0, 0x4b4720, 0x490370, 0x4b47e0, 0xc35044
0x4f8340 -> 0x4b4720, 0x4b47e0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80
0x4f9460 -> 0x4af7b0
0x4f9df0 -> 0x4f9e70, 0x4af960, 0x4af8f0, 0x4af810
0x4faa60 -> 0x5034d0, 0x401900, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x4017c0, 0x401400, 0x401530, 0x401530, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1b60, 0x4f1020, 0x4f1bc0, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1020, 0x4f1a80, 0x4f9460, 0x4af8f0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0, 0x4f9df0
0x4fb1b0 -> 0x4c48c0, 0x401900, 0x4017c0, 0x4017c0, 0x4017c0, 0x401400, 0x401530, 0x401530, 0x401400, 0x4fc850, 0x401400, 0x4ee100, 0x401400
0x4fcf50 -> 0x401900, 0x4017c0, 0x401400, 0x401530, 0x401530, 0x401400, 0x4cea10, 0x401400, 0x4b4720, 0x4b47e0
0x4fe7c0 -> 0x4017c0, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a70, 0x401400, 0x4f1950, 0x4f0ff0, 0x4f1a80, 0x401900, 0x401400, 0x4f1a60, 0x401400, 0x4f1980, 0x4f0fa0, 0x4f1a80, 0x401900, 0x401400, 0x4f1a60, 0x401400, 0x4f1980, 0x4f0f50, 0x4f1a80, 0x401900, 0x401400, 0x4f1a60, 0x401400, 0x4f1980, 0x4f0fd0, 0x4f1a80, 0x401900, 0x401400, 0x4f1a60, 0x401400, 0x4f1980, 0x4f0ff0, 0x4f1a80
0x502250 -> 0x4018f0, 0x401900, 0x4c48c0, 0x401900, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x4c5800, 0x401400, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x4f11f0, 0x401400, 0x4c5800, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4f1950, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f60, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f1950, 0x4f0f70, 0x4f1a80, 0x4f1010, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80
0x5033f0 -> 0x4018f0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x4cea20, 0x401400, 0x4cea10, 0x401400, 0x401400
0x5034e0 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4c48c0, 0x401900, 0x401400, 0x401530, 0x401530, 0x401400, 0x4b4720
0x503d60 -> 0x4fb5e0, 0x401900, 0x401530, 0x401400, 0x4d9fb0, 0x401400, 0x401400, 0x4b8810
0x5043b0 -> 0x5034d0, 0x401900, 0x4017c0, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401530, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0f60, 0x4f1a80, 0x4f0fc0, 0x4f1a80
0x504a30 -> 0x4af7b0, 0x4b8810
0x504c50 -> 0x4c48c0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0f60, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fd0, 0x4f1a80
0x504e10 -> 0x4018f0, 0x401900, 0x4017c0, 0x4017c0, 0x401530, 0x4e7530, 0x401530, 0x508f70, 0x401400, 0x508f60, 0x401400, 0x504df0, 0x401400, 0x504e00, 0x401400
0x505630 -> 0x4017c0, 0x4017c0, 0x4017c0, 0x4018f0, 0x401900, 0x401530, 0x4e7530, 0x401530, 0x505c50, 0x401400, 0x505c60, 0x401400, 0x504e00, 0x401400, 0x505c70, 0x401400
0x505c90 -> 0x4018f0, 0x401900, 0x401530, 0x4c56f0, 0x401400, 0x4c5800, 0x401400, 0x4cea20, 0x401400, 0x4cea10, 0x401400
0x505f60 -> 0x4b4720, 0x4b47e0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x4e5360, 0x401400, 0x401400, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80
0x506fa0 -> 0x4b4720, 0x4b47e0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x4e5360, 0x401400, 0x401400, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1000, 0x4f1a80
0x507e70 -> 0x4017c0
0x507f10 -> 0x4f1cd0, 0x4f1db0, 0x4ceba0, 0x5034d0, 0x401900, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401530, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x508d60, 0x4f1e20, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4b8810
0x508d70 -> 0x4018f0, 0x401900, 0x4017c0, 0x4017c0, 0x401400, 0x401530, 0x401530, 0x4e7530, 0x401530
0x508f80 -> 0x5183d0, 0x508fa0
0x508fa0 -> 0x4f9210, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f9280, 0x401400, 0x4f9230, 0x401400, 0x4f9250, 0x401400, 0x4f9260, 0x401400, 0x4f9220, 0x401400, 0x4f9240, 0x401400, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0xc3504c, 0x4f9200, 0x4f1980, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f9460, 0x5186c0, 0x5186c0, 0x5186c0, 0x4f9df0
0x5098b0 -> 0x4b1b90
0x509910 -> 0x50bd80, 0x50a6c0
0x509930 -> 0x4b0e70, 0x4c1780, 0x50ec00, 0x50dc30
0x509980 -> 0x48f090, 0x4af740, 0x4af7b0, 0x509cc0, 0x509f00, 0x4af7b0, 0x4af7b0
0x50a6c0 -> 0x4b2700, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4b26e0, 0x4950e0, 0x4b0ec0, 0x4b26f0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4b2710, 0x4950e0, 0x4b0ec0, 0x4b0b00, 0x495170
0x50bd80 -> 0x498370, 0x4b0ec0, 0x4b0ec0, 0x4b0ec0, 0x4b0ec0, 0x4b0ec0, 0x4b0ec0
0x50dc30 -> 0x4950e0, 0x4b0ec0, 0x4950e0, 0x4b0ec0, 0x4b7430, 0x4b7430, 0x4b7430
0x50ec00 -> 0x498370, 0x4b0ec0, 0x498370, 0x4b0ec0
0x50fac0 -> 0x4b0e70, 0x516600
0x5125a0 -> 0x4d7550, 0x4d14d0, 0x4d9610, 0x401400
0x512820 -> 0x4978d0, 0x4978f0, 0x4978d0, 0x4978f0
0x516600 -> 0x495170, 0x4951e0, 0x495200
0x5168e0 -> 0x4a9d28, 0x5168fd
0x5168fd -> 0x4a9c57, 0x4a9cb5, 0x4a9ccc, 0x4a9c57
0x518260 -> 0x4b0b00, 0x4b0b00, 0x512820
0x5183d0 -> 0x530270, 0x4fcf50, 0x530220, 0x507e70, 0x4ed510, 0x508d70, 0x504e10, 0x505630, 0x52ca70, 0x4f1ee0, 0x52ca30, 0x4f1840, 0x4ee970, 0x4f1e30, 0x52c820, 0x52c6b0, 0x4edce0, 0x4fb1b0, 0x503d60, 0x5033f0, 0x52c0c0, 0x52b6c0, 0x52b3b0, 0x4f3440, 0x4fe7c0, 0x502250, 0x4f36c0, 0x4f2740, 0x505f60, 0x518ef0, 0x52b040, 0x4ed6b0, 0x504c50, 0x4f4b00, 0x52ad10, 0x52aaf0, 0x4faa60, 0x518760, 0x5284a0, 0x5043b0, 0x4c1780, 0x4f4190, 0x527c90, 0x526ee0, 0x526330, 0x520bb0, 0x505c90, 0x5207a0, 0x51fef0, 0x51f580, 0x51eee0, 0x51ecb0, 0x4ee410, 0x51ddf0, 0x51c2b0, 0x4c1780, 0x4c1780, 0x4c1780, 0x4c1780, 0x4c1780, 0x51bfb0, 0x507f10, 0x51bac0, 0x51b960, 0x51b400, 0x4f7360, 0x4f8340, 0x5034e0, 0x519850, 0x51a1d0, 0x51b140, 0x506fa0, 0x519760, 0x519710
0x5186c0 -> 0x4af7b0, 0x4b8810
0x518760 -> 0x5034d0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f9270, 0x401400, 0x4ceba0, 0x4f1950, 0x4f0fa0, 0x4f1a80
0x518ef0 -> 0x4a54a0, 0xc35044, 0x4c48c0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401530, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x4e55b0, 0x401400, 0x401f60, 0x401fa0, 0x4f1950, 0x4f0f50, 0x4f19c0, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80
0x519710 -> 0x401900, 0x4f11f0, 0x401400
0x519760 -> 0x4018f0, 0x401900, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f0f60, 0x4f1a80, 0x4f0f50, 0x4f1a80
0x519850 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f1010, 0x4f1a80, 0x4f1000, 0x4f1a80
0x51a1d0 -> 0x401900, 0x401400, 0x401530, 0x4f1a60, 0x401400
0x51b140 -> 0x4018f0, 0x401900, 0x4017c0, 0x4017c0, 0x401530, 0x401400, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401530, 0x4f1950, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4c1800
0x51b400 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4ec060, 0x401400, 0x4ec050, 0x401400, 0x4ec040, 0x401400, 0x4b8810
0x51b960 -> 0x4d9fa0, 0x401900, 0x401400, 0x401530, 0x4c56d0, 0x401400, 0x4c56e0, 0x401400, 0x4c56f0, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x4b8810
0x51bac0 -> 0x5034d0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4b8810
0x51bfb0 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4e6610, 0x401400, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0fe0, 0x4f1a80
0x51c2b0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401530, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f1950, 0x4f0ff0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80
0x51ddf0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401530, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f0f30, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f1950, 0x4f0ff0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80
0x51ecb0 -> 0x4018f0, 0x401900, 0x401400, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f1000, 0x4f1a80, 0x4b8810, 0x4b8810, 0x4b8810, 0x4f0fa0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80
0x51eee0 -> 0x5034d0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4b8810
0x51f580 -> 0x4c48c0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4f1010, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4b8810
0x51fef0 -> 0x5034d0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1210, 0x401530, 0x4f1220, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fc0, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4b8810, 0x4b4720
0x5207a0 -> 0x4c48c0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x4e5360, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80
0x520bb0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f1950, 0x4f0f50, 0x4f19c0, 0x4f0f60, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4b4720, 0xc35044, 0xc35044
0x526330 -> 0x4018f0, 0x401900, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0fd0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4b0b00, 0x4b0b00, 0x4b4720, 0x4b3b60, 0x4b33a0, 0x4017c0, 0x4017c0, 0x4017c0, 0x5274c0, 0x401400, 0x5274c0, 0x401400, 0x5274c0, 0x401400, 0x4cfca0, 0x4c48c0, 0x401400, 0x497570, 0x497680, 0x4b0c60
0x526ee0 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f1950, 0x4f1000, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f1cb0, 0x4f0f50, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f20, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4b0b00, 0x4b4720, 0x4e8290
0x527c90 -> 0x4018f0, 0x401900, 0x4018f0, 0x401900, 0x401400, 0x401530, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f0ef0, 0x4f1a80, 0x4f1950, 0x4f0ef0, 0x4f1a80, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4a53b3, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f40, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4e8290
0x5284a0 -> 0x530260, 0x401900, 0x401530, 0x401530, 0x401400, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f40d0, 0x401400, 0x4f40e0, 0x401400, 0x4f4110, 0x401400, 0x4f4120, 0x401400, 0x4f4130, 0x401400, 0x4f4140, 0x401400, 0x4f1950, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80
0x52aaf0 -> 0x4c48c0, 0x401900, 0x401530, 0x401400, 0x401530, 0x4f40d0, 0x401400, 0x4f40e0, 0x401400, 0x4f4110, 0x401400, 0x4f4120, 0x401400, 0x4f4130, 0x401400
0x52ad10 -> 0x4a54a0, 0x4f4100, 0x401900, 0x401400, 0x401530, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0f70, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0ff0, 0x4f1a80, 0x4f1000, 0x4f1a80, 0x4f40f0, 0x4f1a80
0x52b040 -> 0x5034d0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4e5360, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0f50, 0x4f1a80, 0x4f0f80, 0x4f1a80, 0x4f0f80, 0x4f1a80
0x52b3b0 -> 0xc35044, 0x5034d0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0ff0, 0x4f1a80
0x52b6c0 -> 0x4c48c0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f0fa0, 0x4f1a80, 0x4f1000, 0x4f1a80
0x52c0c0 -> 0x5034d0, 0x401900, 0x401400, 0x401400, 0x401400, 0x401400, 0x401530, 0x4f1a60, 0x401400, 0x4f1a70, 0x401400, 0x4e5360, 0x401400, 0x401400, 0x4cea20, 0x401400, 0x4cea10, 0x401400, 0x4d14d0, 0x401400, 0x401400, 0x401400, 0x401400, 0x4f1950, 0x4f1010, 0x4f1a80, 0x4f0fa0, 0x4f1a80
0x52c6b0 -> 0x4018f0, 0x401900, 0x401400, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0f50, 0x4f1a80, 0x4f0f20, 0x4f1a80
0x52c820 -> 0x4018f0, 0x401900, 0x4018f0, 0x4017c0, 0x401400, 0x401530, 0x4f1a70, 0x401400, 0x4f1a60, 0x401400, 0x4f1950, 0x4f0fa0, 0x4f1a80
0x52ca30 -> 0x4d9610, 0x401900
0x52ca70 -> 0x505c80, 0x401900, 0x4e7530, 0x401530, 0x504df0, 0x401400
0x52d360 -> 0x495200
0x52d420 -> 0x4ceba0
0x52dd70 -> 0x4978d0, 0x4978f0
0x530220 -> 0x4018f0, 0x401900
0x530270 -> 0x5309c0, 0x530960, 0x4ec160, 0x4ebe70, 0x4c1780, 0x530800, 0x4ed000
0x530800 -> 0x530950
0x530960 -> 0x4017c0, 0x4017c0
0x5309c0 -> 0x5311e0, 0x4e6070, 0x4ebc60, 0x4eb1d0, 0x4e8390, 0x4ea3d0, 0x4e9520, 0x4e7370, 0x4e7d90, 0x4e82c0, 0x4e7c50, 0x5310e0, 0x4e7840, 0x4e92c0, 0x4e9480, 0x530c80, 0x531190, 0x530a80, 0x4a2a00
0x530a80 -> 0x4018f0, 0x401900, 0x401530, 0x401530
0x530c80 -> 0x401900, 0x401400, 0x401530, 0x4b4720
0x5310e0 -> 0x4018f0, 0x401900, 0x401530, 0x401530
0x531190 -> 0x5310d0, 0x401900
0x5311e0 -> 0x5323c0, 0x531540
0x531300 -> 0x401900, 0x401900, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x401400, 0x4b0b00, 0x4b0b00
0x531540 -> 0x531d30, 0x4ceeb0, 0x531300, 0x52d420, 0x518260, 0x531ef0, 0x4a1470, 0x4d9a00, 0x5125a0, 0x49a520, 0x4c1780, 0x4e41b0, 0x4e0950, 0x4d0b30, 0x5317d0, 0x4dcf20, 0x4e0840, 0x498ab0, 0x52d360, 0x49bb40
0x5317d0 -> 0x4d9610, 0x401900, 0x401400, 0x4c56f0, 0x401400, 0x401530, 0x401400, 0x4d9fb0, 0x401400, 0x401400, 0x4c56d0, 0x401400, 0x4c56e0, 0x401400, 0x401530, 0x4b8810
0x531d30 -> 0x531db0, 0x4e5d90, 0x4c1780, 0x4d0ef0, 0x4c1780, 0x4c1780, 0x50fac0, 0x52dd70, 0x4da1d0, 0x531fa0
0x531db0 -> 0x531dd0, 0x4c5900, 0x4e55c0
0x531dd0 -> 0x4d9730, 0x4d9810, 0x4e5370, 0x4d2c30, 0x4da880
0x531ef0 -> 0x401900, 0x401400, 0x4b0b00
0x531fa0 -> 0x401900, 0x401400, 0x401530, 0x4978d0, 0x4978d0, 0x4978f0, 0x4950e0, 0x4b0ec0
0x5323c0 -> 0x5323e0, 0x4010d0, 0x4c1780
0x5323e0 -> 0x532400, 0x4b8b20
0x532400 -> 0x4c1780, 0x498950, 0x4af8b0, 0x48f020, 0x4c1780, 0x4b46a0, 0x4b0ba0, 0x4c1780, 0x4b1200, 0x4b0dc0, 0x491700, 0x4b5e70, 0x4b7c80, 0x4b2180, 0x495280, 0x4b59a0, 0x48fff0, 0x48fde0, 0x509980, 0x497550, 0x497980, 0x490360, 0x4c1780, 0x4b73c0, 0x490380
```

### 3.2 Data tables / global arrays referenced (r2 absolute operands inside each BN extent)

| VA | BN alias | referenced data VAs (section) |
|---|---|---|
| `0x4010d0` | `QTask_Init` | `0x0053614c` (.data), `0x00536164` (.data), `0x0054f928` (.data), `0x0054f92c` (.data), `0x0054ff24` (.data), `0x0054ff28` (.data), `0x0054ff2c` (.data), `0x0054ff30` (.data), `0x00567c38` (.data), `0x00a969e0` (.data), `0x00a96ae0` (.data), `0x00afa6e0` (.data), `0x00afa7e0` (.data) |
| `0x4011b0` | `QTask_Update` | `0x00af5ee0` (.data), `0x00afa6e0` (.data) |
| `0x4011d0` | `QTask_OnEvent` | `0x0054f932` (.data), `0x0054ff26` (.data) |
| `0x401400` | `QTask_RemoveEventCallback` | `0x00536148` (.data), `0x0054ff34` (.data), `0x00567c6c` (.data), `0x00a96ae0` (.data), `0x00af5ee8` (.data), `0x00afa6e8` (.data) |
| `0x4014b0` | `QTask_DeleteTask` | `0x00af5f08` (.data), `0x00afa708` (.data) |
| `0x4014e0` | `QTask_ChangeParent` | `0x0054ff24` (.data), `0x00a96ae0` (.data) |
| `0x401530` | `QTask_LinkQTaskEventHandlerNoContextFn` | `0x00536148` (.data), `0x0054ff34` (.data), `0x00567c70` (.data), `0x00a96ae0` (.data), `0x00af5ee8` (.data), `0x00afa6e8` (.data) |
| `0x4017c0` | `QTask_UnableToAllocateEven` | `0x005361a8` (.data), `0x0054f930` (.data), `0x0054f932` (.data), `0x0054f933` (.data), `0x0054f9f2` (.data), `0x0054ff26` (.data) |
| `0x401810` | `QTask_NewQTaskEventNoContextFn` | `0x005361a8` (.data), `0x0054f930` (.data), `0x0054f932` (.data), `0x0054f933` (.data), `0x0054f9f2` (.data), `0x0054ff26` (.data) |
| `0x401860` | `QTask_CleanupEvents` | `0x0054f930` (.data), `0x0054f932` (.data), `0x0054f933` (.data) |
| `0x401890` | `QTask_ProcessQueue` | `0x0054f930` (.data), `0x0054f932` (.data), `0x0054f933` (.data) |
| `0x4018f0` | `QTask_OpenEvent_2` | `0x0054f928` (.data) |
| `0x401900` | `TaskType_Register` | `0x005361c4` (.data), `0x00a96ae0` (.data), `0x00af5ee0` (.data), `0x00af5ee4` (.data), `0x00af5ee8` (.data), `0x00af5eec` (.data), `0x00af5f04` (.data), `0x00af5f05` (.data), `0x00af5f0c` (.data), `0x00afa705` (.data) |
| `0x401d30` | `QTask_CleanupTypes_2` | `0x0054ff24` (.data), `0x00a969e0` (.data), `0x00af5f08` (.data), `0x00afa708` (.data) |
| `0x401f60` | `QTask_DispatchEvent_3` | `0x00567c5c` (.data) |
| `0x401fa0` | `QTask_RegisterEventCallback_3` | `0x00af5f0c` (.data) |
| `0x48f020` | `FX_Germany` | `0x005330ec` (.rdata), `0x00541d60` (.data), `0x005c8c20` (.data) |
| `0x48f090` | `FX_TrailUpdate_21` | `0x005c8bc0` (.data) |
| `0x48f0a0` | `AppContext_GethWnd` | `0x005c8bc4` (.data) |
| `0x48f180` | `FX_TrailUpdate_23` | `0x005c8c18` (.data), `0x005c8c1c` (.data) |
| `0x48f2b0` | `FX_FireBallUpdate_26` | `0x005c8bdc` (.data) |
| `0x48f490` | `FX_FailedToRegisterWinp` | `0x00541d70` (.data), `0x005c8d28` (.data), `0x005c8d2c` (.data), `0x005c8d30` (.data), `0x005c8df0` (.data) |
| `0x48fd90` | `FX_CouldNotGetMouseCaps` | `0x00542150` (.data), `0x005c8e1c` (.data) |
| `0x48fde0` | `FX_CouldNotCreateDirect` | `0x005348b0` (.rdata), `0x00534910` (.rdata), `0x005349b0` (.rdata), `0x005349d0` (.rdata), `0x0054216c` (.data), `0x0054219c` (.data), `0x005421c4` (.data), `0x005421f0` (.data), `0x005c8e08` (.data), `0x005c8e0c` (.data), `0x005c8e10` (.data), `0x005c8e14` (.data), `0x005c8e1c` (.data) |
| `0x48fff0` | `FX_CouldNotCreateDirect_2` | `0x005348c8` (.rdata), `0x00534900` (.rdata), `0x00534904` (.rdata), `0x00534908` (.rdata), `0x0053490c` (.rdata), `0x0054216c` (.data), `0x0054221c` (.data), `0x00542244` (.data), `0x00542274` (.data), `0x005c8e24` (.data), `0x005c8e28` (.data), `0x005c8e30` (.data) |
| `0x490360` | `Timer_Open` | `0x00533120` (.rdata), `0x005c8e34` (.data) |
| `0x490370` | `Timer_Read` | `0x00533120` (.rdata), `0x005c8e34` (.data) |
| `0x490380` | `Display_CheckCapabilities` | `0x005c8e48` (.data) |
| `0x491630` | `Display_SetDisplayMode_3` | `0x00bcadec` (.data) |
| `0x491650` | `Display_NoMoreDisplayCallbac` | `0x00542344` (.data) |
| `0x4916c0` | `Display_SwapBuffers_3` | `0x00bcae00` (.data) |
| `0x491700` | `Display_DisplayCanOnlyBeOpen` | `0x00542310` (.data), `0x00542314` (.data), `0x00542318` (.data), `0x0054231c` (.data), `0x00542320` (.data), `0x00542324` (.data), `0x00542328` (.data), `0x0054232c` (.data), `0x00542330` (.data), `0x00542334` (.data), `0x00542338` (.data), `0x0054233c` (.data), `0x00542340` (.data), `0x0054236c` (.data), `0x00542380` (.data), `0x00542390` (.data), `0x005423a4` (.data), `0x005423b0` (.data), `0x005423b8` (.data), `0x005423c8` (.data), `0x005423dc` (.data), `0x005423ec` (.data), `0x005423fc` (.data), `0x00542410` (.data), `0x00542420` (.data), `0x00542430` (.data), `0x0054243c` (.data), `0x005c9fa8` (.data), `0x005ca02c` (.data), `0x005ca02d` (.data) |
| `0x491860` | `Display_RestoreDevice_3` | `0x00bcadec` (.data), `0x00bcadfc` (.data), `0x00bcae00` (.data) |
| `0x491db0` | `Display_SetGammaRamp_4` | `0x00542340` (.data) |
| `0x491dc0` | `Display_CreateSurface_4` | `0x00542338` (.data) |
| `0x491dd0` | `Display_OpenDevice_5` | `0x0054233c` (.data) |
| `0x491de0` | `Display_CloseDevice_5` | `0x00542310` (.data) |
| `0x491df0` | `Display_SetDisplayMode_5` | `0x00542314` (.data) |
| `0x491e00` | `Display_SyncVSync_5` | `0x00542318` (.data) |
| `0x491e10` | `Display_SwapBuffers_5` | `0x0054231c` (.data) |
| `0x491e20` | `Display_ClearSurfaces_5` | `0x00542320` (.data) |
| `0x491e30` | `Display_RestoreDevice_5` | `0x00542328` (.data) |
| `0x491e40` | `Display_CheckCapabilities_5` | `0x0054232c` (.data) |
| `0x491e50` | `Display_SetGammaRamp_5` | `0x00542330` (.data) |
| `0x491e60` | `Display_CreateSurface_5` | `0x00542334` (.data) |
| `0x493580` | `Display_Direct3D` | `0x005436a4` (.data), `0x005ca0e0` (.data), `0x005ca0f8` (.data), `0x00bcabec` (.data), `0x00bcabf0` (.data), `0x00bcacec` (.data), `0x00bcacf0` (.data), `0x00bcad20` (.data), `0x00bcad70` (.data), `0x00bcad78` (.data), `0x00bcad7c` (.data), `0x00bcad8c` (.data), `0x00bcad9c` (.data), `0x00bcada0` (.data), `0x00bcada8` (.data), `0x00bcadac` (.data), `0x00bcadb0` (.data), `0x00bcadb4` (.data), `0x00bcadb8` (.data), `0x00bcadbc` (.data), `0x00bcadc0` (.data), `0x00bcadc4` (.data), `0x00bcadc8` (.data), `0x00bcadcc` (.data), `0x00bcadd0` (.data), `0x00bcadd4` (.data), `0x00bcadd8` (.data), `0x00bcaddc` (.data), `0x00bcade0` (.data), `0x00bcade4` (.data), `0x00bcade8` (.data) |
| `0x4950e0` | `Display_CheckCapabilities_7` | `0x005ca0e0` (.data) |
| `0x495170` | `Display_CreateSurface_7` | `0x005ca118` (.data) |
| `0x495190` | `Display_NoMoreDirect3DCallba` | `0x005438d8` (.data), `0x00bcad20` (.data), `0x00bcad24` (.data), `0x00bcad74` (.data) |
| `0x4951e0` | `Display_OpenDevice_8` | `0x005ca11c` (.data) |
| `0x495200` | `Display_CloseDevice_8` | `0x005ca120` (.data) |
| `0x4952a0` | `Display_Cdaudio` | `0x00533210` (.rdata), `0x005439e4` (.data), `0x005ca130` (.data), `0x005ca188` (.data), `0x00676ab4` (.data), `0x006840e8` (.data) |
| `0x4952f0` | `Display_ClearSurfaces_8` | `0x00534260` (.rdata), `0x00534268` (.rdata), `0x00543900` (.data), `0x005ca134` (.data), `0x005ca154` (.data), `0x005ca1b8` (.data), `0x00676e50` (.data), `0x0067a498` (.data), `0x0067a4a0` (.data), `0x006840e0` (.data), `0x006840e9` (.data), `0x006840ea` (.data), `0x006840f0` (.data), `0x006840f4` (.data), `0x006840f8` (.data) |
| `0x495640` | `Display_CdAudio` | `0x005331f0` (.rdata), `0x005331fc` (.rdata), `0x00533200` (.rdata), `0x00533204` (.rdata), `0x00533208` (.rdata), `0x0053320c` (.rdata), `0x005439ec` (.data), `0x005439f8` (.data), `0x005ca150` (.data), `0x00676e58` (.data), `0x006840e8` (.data) |
| `0x495a60` | `Display_CloseDevice_9` | `0x005ca190` (.data), `0x005ca194` (.data), `0x005ca1b4` (.data), `0x006840e9` (.data) |
| `0x495cb0` | `Display_CheckCapabilities_9` | `0x006840e9` (.data) |
| `0x497510` | `Display_OpenDevice_13` | `0x005ca134` (.data) |
| `0x497520` | `Display_CloseDevice_13` | `0x00684110` (.data) |
| `0x497550` | `Display_SetDisplayMode_13` | `0x00684100` (.data), `0x00684104` (.data), `0x00684108` (.data) |
| `0x497680` | `Display_CheckCapabilities_13` | `0x00533504` (.rdata) |
| `0x497940` | `Display_CreateSurface_14` | `0x00543a64` (.data) |
| `0x497980` | `Display_Transcontextprimitiv` | `0x00543a68` (.data), `0x00543a6c` (.data), `0x00543a84` (.data), `0x00543a9c` (.data), `0x006e5ba4` (.data), `0x006e5ba8` (.data) |
| `0x497ed0` | `Display_SwapBuffers_15` | `0x006e5bb0` (.data) |
| `0x498080` | `Display_Offscreenbuffer` | `0x00543b00` (.data), `0x006e5bb8` (.data), `0x006e5bbc` (.data) |
| `0x498370` | `Display_CloseDevice_16` | `0x006e5bb8` (.data) |
| `0x498ab0` | `Rain_Init` | `0x00543c5c` (.data), `0x0054875c` (.data) |
| `0x49a340` | `Display_SyncVSync_18` | `0x00720558` (.data) |
| `0x49a520` | `PolyListDynCubeObj_Init` | `0x00543c64` (.data), `0x00543c68` (.data), `0x00543c84` (.data), `0x0054875c` (.data), `0x0072057c` (.data), `0x007205a0` (.data), `0x007205a4` (.data), `0x007205a8` (.data), `0x00bcad7c` (.data), `0x00bcadac` (.data), `0x00bcadc4` (.data), `0x00bcade8` (.data) |
| `0x49bb40` | `Display_CloseDevice_19` | `0x007205b0` (.data), `0x007205b8` (.data), `0x007205bc` (.data), `0x00721560` (.data), `0x007234b8` (.data), `0x00723504` (.data), `0x00723508` (.data), `0x0072350c` (.data), `0x00724458` (.data), `0x007ac09c` (.data), `0x007addd4` (.data), `0x00b8170c` (.data), `0x00b817a0` (.data), `0x00bcad7c` (.data), `0x00bcadac` (.data), `0x00bcade8` (.data) |
| `0x4a2a00` | `BandFilter_Init` | `0x005342ec` (.rdata), `0x00543d4c` (.data), `0x00543d4e` (.data), `0x00543d50` (.data), `0x00543d58` (.data), `0x00543d5c` (.data), `0x00543d70` (.data), `0x00543d80` (.data), `0x00543d90` (.data), `0x00543da0` (.data), `0x00543dac` (.data), `0x00543dc0` (.data), `0x00543dcc` (.data), `0x00543de8` (.data), `0x00543e08` (.data), `0x00543e20` (.data), `0x00543e38` (.data), `0x00935bf0` (.data), `0x00935bfc` (.data), `0x00935c00` (.data), `0x00935c08` (.data), `0x00935c0c` (.data), `0x00936010` (.data), `0x00936014` (.data), `0x00936018` (.data), `0x00936038` (.data) |
| `0x4a5980` | `Camera_UpdateTransform_6` | `0x009361dc` (.data) |
| `0x4a6a50` | `Camera_UpdateTransform_9` | `0x005330a4` (.rdata) |
| `0x4a6d7d` | `Camera_ResetMatrix_9` | `0x00533098` (.rdata), `0x00533128` (.rdata), `0x00936064` (.data), `0x00936068` (.data) |
| `0x4a760b` | `_flsbuf` | `0x00543fb0` (.data), `0x00546860` (.data), `0x00546880` (.data) |
| `0x4a7720` | `_output` | `0x00534660` (.rdata), `0x00534680` (.rdata), `0x00544050` (.data), `0x00544054` (.data), `0x0054405c` (.data), `0x00544068` (.data), `0x0054406c` (.data), `0x00544070` (.data) |
| `0x4a7f2a` | `Camera_ApplyBandFilter_12` | `0x00544070` (.data) |
| `0x4a80bf` | `Camera_ApplyPostProcess_12` | `0x0053309c` (.rdata) |
| `0x4a8372` | `Camera_UpdateViewCone_12` | `0x005333b0` (.rdata), `0x00544368` (.data), `0x00544378` (.data) |
| `0x4a8611` | `Camera_UpdateTransform_13` | `0x00936064` (.data) |
| `0x4a871e` | `Camera_ApplyPostProcess_13` | `0x005333b0` (.rdata) |
| `0x4a881f` | `Camera_SetFieldOfView_14` | `0x00544390` (.data), `0x0054439c` (.data) |
| `0x4a9ccc` | `Camera_SetFieldOfView_18` | `0x005347e0` (.rdata) |
| `0x4a9fc1` | `Camera_SetFieldOfView_19` | `0x00544070` (.data), `0x0054427c` (.data), `0x009361e4` (.data) |
| `0x4aa670` | `Mem_AllocChunk` | `0x00936250` (.data) |
| `0x4aa6ae` | `Mem_AllocateRawBlock` | `0x005330ac` (.rdata), `0x005466d4` (.data) |
| `0x4aadbb` | `Mem_AllocateCustomPool_1` | `0x005330a4` (.rdata), `0x005330bc` (.rdata) |
| `0x4ab3ed` | `Camera_UpdateViewCone_21` | `0x005330a4` (.rdata), `0x005330ac` (.rdata), `0x005330b0` (.rdata), `0x005330b4` (.rdata) |
| `0x4ab49e` | `Camera_ResetMatrix_21` | `0x005330b0` (.rdata) |
| `0x4ab599` | `Camera_UpdateTransform_22` | `0x005330a4` (.rdata), `0x005330ac` (.rdata), `0x005330b0` (.rdata), `0x005330bc` (.rdata), `0x005446b0` (.data), `0x005446b4` (.data), `0x005446c0` (.data) |
| `0x4ab6dd` | `Camera_SetFieldOfView_22` | `0x005330a4` (.rdata), `0x005330bc` (.rdata), `0x005446b0` (.data), `0x005446c0` (.data), `0x005466d0` (.data) |
| `0x4ab733` | `Camera_ApplyLineFilter_22` | `0x005330bc` (.rdata), `0x005446b4` (.data), `0x00936200` (.data) |
| `0x4ab7f5` | `Camera_ApplyBandFilter_22` | `0x005446b0` (.data) |
| `0x4ab84c` | `Camera_ApplyHandiCam_22` | `0x00936200` (.data) |
| `0x4ab891` | `Mem_AllocateCustomPool` | `0x005330b0` (.rdata), `0x005446b0` (.data), `0x005466d0` (.data) |
| `0x4abc46` | `Camera_UpdateViewCone_22` | `0x005466d8` (.data), `0x005466dc` (.data), `0x00546840` (.data), `0x00936064` (.data), `0x00936068` (.data) |
| `0x4abe33` | `Camera_ApplyLineFilter_23` | `0x00936064` (.data), `0x00936068` (.data) |
| `0x4abec7` | `Camera_ApplyHandiCam_23` | `0x00533090` (.rdata), `0x00533128` (.rdata), `0x00936064` (.data), `0x00936068` (.data) |
| `0x4ac323` | `Camera_ResetMatrix_23` | `0x00936210` (.data) |
| `0x4ad0d4` | `Camera_UpdateViewCone_26` | `0x00533060` (.rdata), `0x0054427c` (.data), `0x00936064` (.data), `0x009361e4` (.data), `0x009361f4` (.data) |
| `0x4ad225` | `Camera_SetFieldOfView_27` | `0x005330c8` (.rdata), `0x005330cc` (.rdata), `0x005330d0` (.rdata), `0x00534860` (.rdata), `0x00534864` (.rdata), `0x00534868` (.rdata), `0x009361e4` (.data), `0x009361f4` (.data), `0x00936248` (.data) |
| `0x4ad371` | `Camera_ApplyWideScreen_27` | `0x00546af0` (.data) |
| `0x4ad4d3` | `Camera_UpdateViewCone_27` | `0x00533060` (.rdata), `0x005330c8` (.rdata), `0x005330d4` (.rdata), `0x005330d8` (.rdata), `0x00534860` (.rdata), `0x00534864` (.rdata), `0x00534878` (.rdata), `0x009361f4` (.data), `0x0093624c` (.data) |
| `0x4ad722` | `Mem_InvokeNewHandler` | `0x00936254` (.data) |
| `0x4ad73d` | `String_ToUpperChar` | `0x00544070` (.data), `0x0054427c` (.data), `0x009361e4` (.data) |
| `0x4ae730` | `Camera_SetFieldOfView_30` | `0x0053303c` (.rdata) |
| `0x4af740` | `Camera_ApplyBandFilter_30` | `0x00533038` (.rdata) |
| `0x4af746` | `Camera_ApplyHandiCam_30` | `0x00533044` (.rdata) |
| `0x4af758` | `Camera_UpdateViewCone_30` | `0x0053301c` (.rdata) |
| `0x4af7b0` | `Debug_Error` | `0x00546db8` (.data), `0x00936268` (.data), `0x0093626c` (.data), `0x00a84640` (.data) |
| `0x4af810` | `Warning_Show` | `0x00546dc8` (.data), `0x00936274` (.data), `0x00a84640` (.data) |
| `0x4af860` | `Camera_ApplyBandFilter_32` | `0x00936268` (.data) |
| `0x4af870` | `Camera_ApplyHandiCam_32` | `0x00936270` (.data) |
| `0x4af880` | `Camera_ApplyWideScreen_32` | `0x00936274` (.data) |
| `0x4af890` | `Camera_ApplyPostProcess_32` | `0x0093625c` (.data) |
| `0x4af8a0` | `Camera_UpdateViewCone_32` | `0x0093626c` (.data) |
| `0x4af8b0` | `Camera_ResetMatrix_32` | `0x0093627c` (.data) |
| `0x4b0ba0` | `Mem_InitHeapState` | `0x009422fc` (.data), `0x00a960e0` (.data), `0x00a960f0` (.data), `0x00a960f4` (.data), `0x00a960f8` (.data) |
| `0x4b0c60` | `Mem_AllocFn` | `0x00546e3c` (.data), `0x009422fc` (.data), `0x00942300` (.data), `0x00a960f0` (.data) |
| `0x4b0d10` | `Mem_DeAllocFn` | `0x00942300` (.data) |
| `0x4b0d40` | `QHash_ValueSet` | `0x009422fd` (.data) |
| `0x4b0d60` | `Qhash_Init` | `0x009422e8` (.data), `0x009422fd` (.data), `0x00a960e0` (.data) |
| `0x4b0d90` | `Qhash_Reset` | `0x009422e8` (.data), `0x009422fd` (.data), `0x00a960e0` (.data) |
| `0x4b0dc0` | `Sys_PushStack_2` | `0x00942304` (.data), `0x00a94a64` (.data), `0x00a94a80` (.data), `0x00a94e80` (.data), `0x00a950a0` (.data) |
| `0x4b0e00` | `Sys_RendermodeFailedToCr` | `0x00546ea0` (.data), `0x00a950a0` (.data), `0x00a952a4` (.data), `0x00a962e4` (.data) |
| `0x4b0e70` | `Sys_RendermodeFailedToCr_2` | `0x00546ed4` (.data), `0x00a94a80` (.data), `0x00a94a84` (.data), `0x00a94e84` (.data) |
| `0x4b0ec0` | `Sys_CheckHeap_2` | `0x00a950a4` (.data) |
| `0x4b0ee0` | `Sys_AllocateMemory_3` | `0x00a950a0` (.data), `0x00a960e0` (.data) |
| `0x4b1200` | `Sys_PopStack_3` | `0x009436b0` (.data), `0x0094396c` (.data), `0x00943970` (.data), `0x00943974` (.data), `0x00943978` (.data), `0x00943988` (.data), `0x00943a18` (.data) |
| `0x4b12d0` | `Sys_AllocateMemory_4` | `0x009423c0` (.data), `0x00943640` (.data) |
| `0x4b1b90` | `Sys_ReadFileBlock_5` | `0x00943708` (.data) |
| `0x4b2180` | `Sys_Alloctexture` | `0x00546f90` (.data), `0x00546f94` (.data), `0x00546f98` (.data), `0x00546f9c` (.data), `0x00546fa0` (.data), `0x00546fa4` (.data), `0x00546fa8` (.data), `0x00546fac` (.data), `0x00546fb0` (.data), `0x00546fb4` (.data), `0x00546fb8` (.data), `0x00546fbc` (.data), `0x00546fc0` (.data), `0x00546fc8` (.data), `0x00546fd8` (.data), `0x00546fe8` (.data), `0x00546ff8` (.data), `0x00547008` (.data), `0x00547018` (.data), `0x00547020` (.data), `0x00547030` (.data), `0x00547044` (.data), `0x00547054` (.data), `0x00547064` (.data), `0x00943b20` (.data) |
| `0x4b26b0` | `Texture_DisableUpload` | `0x00943b3c` (.data) |
| `0x4b26e0` | `Sys_AllocateMemory_9` | `0x00546f9c` (.data) |
| `0x4b26f0` | `Sys_FreeMemory_9` | `0x00546f98` (.data) |
| `0x4b2700` | `Sys_OpenFileStream_9` | `0x00546f94` (.data) |
| `0x4b2710` | `Sys_ReadFileBlock_9` | `0x00546f90` (.data) |
| `0x4b27d0` | `Sys_PushStack_9` | `0x005333f8` (.rdata), `0x005336b8` (.rdata) |
| `0x4b46a0` | `Sys_PopStack_13` | `0x005333e0` (.rdata), `0x005333e8` (.rdata), `0x00534aa4` (.rdata), `0x00a84a5c` (.data), `0x00a88a5c` (.data), `0x00a8ca60` (.data), `0x00a94a64` (.data) |
| `0x4b4770` | `Sys_AllocateMemory_14` | `0x00534aa8` (.rdata) |
| `0x4b59a0` | `Sys_QvertexRegisterstrip` | `0x005470b8` (.data), `0x00a84a50` (.data), `0x00a84a54` (.data), `0x00a84a58` (.data) |
| `0x4b59f0` | `Resource_CreateNode` | `0x00943e20` (.data), `0x00943e24` (.data), `0x00943e28` (.data), `0x00943e30` (.data), `0x00943e34` (.data), `0x00943e38` (.data) |
| `0x4b5e70` | `Sys_ReadFileBlock_16` | `0x00943b90` (.data), `0x00943e20` (.data), `0x00943e38` (.data), `0x00943e3c` (.data), `0x00943e40` (.data) |
| `0x4b73c0` | `Sys_Sortrender` | `0x005471e0` (.data), `0x00943e48` (.data) |
| `0x4b7c80` | `Sys_Allocpalette` | `0x005471ec` (.data), `0x005471fc` (.data), `0x0054720c` (.data), `0x00a84a40` (.data), `0x00a84a44` (.data), `0x00a84a48` (.data) |
| `0x4b8970` | `Sys_ExecuteBytecode_23` | `0x00a96598` (.data) |
| `0x4b89f0` | `Sys_DispatchOpcode_23` | `0x00a969c0` (.data) |
| `0x4b8a00` | `Sys_PushStack_23` | `0x00a969c4` (.data) |
| `0x4b8b20` | `Sys_False` | `0x00538370` (.data), `0x00538378` (.data), `0x00547340` (.data), `0x0054734c` (.data), `0x00547364` (.data), `0x00547380` (.data), `0x0054739c` (.data), `0x005473bc` (.data), `0x005473c0` (.data), `0x00a43e60` (.data), `0x00a43e64` (.data), `0x00a43e68` (.data), `0x00a43e6c` (.data), `0x00a43e70` (.data), `0x00a43e74` (.data), `0x00a9659c` (.data) |
| `0x4baac0` | `Q_TaskHashTableSet` | `0x00a43e88` (.data) |
| `0x4bacd0` | `Sys_PopStack_25` | `0x00a43e88` (.data), `0x00a43e8c` (.data), `0x00a43e90` (.data), `0x00a43e94` (.data), `0x00a43e98` (.data), `0x00a43ea0` (.data), `0x00a43ea4` (.data), `0x00a43ea8` (.data), `0x00a43eac` (.data), `0x00a43eb0` (.data), `0x00a43eb4` (.data), `0x00a43eb8` (.data) |
| `0x4bbc80` | `Sys_ResolveSymbol_26` | `0x00a43ec0` (.data), `0x00a43ec4` (.data), `0x00a43ec8` (.data), `0x00a43ecc` (.data), `0x00a43ed4` (.data), `0x00a43ed8` (.data) |
| `0x4bc920` | `Sys_CheckHeap_27` | `0x00a442b4` (.data), `0x00a442b8` (.data), `0x00a442bc` (.data), `0x00a442c0` (.data), `0x00a442c4` (.data), `0x00a442cc` (.data) |
| `0x4bd3d0` | `Qvm_SetOpcodeHandler` | `0x00a96108` (.data) |
| `0x4bd430` | `Sys_OpenFileStream_29` | `0x00a442ec` (.data), `0x00a442f4` (.data), `0x00a442f8` (.data), `0x00a442fc` (.data), `0x00a44300` (.data), `0x00a44308` (.data) |
| `0x4c0560` | `Symbol_CheckRegister` | `0x00547e78` (.data) |
| `0x4c3450` | `RigidDynCubeObj_Init` | `0x00547f3c` (.data), `0x00547f3d` (.data), `0x00547f3e` (.data), `0x00547f3f` (.data), `0x00547f40` (.data), `0x00547f42` (.data), `0x00547f5c` (.data), `0x00547f6c` (.data), `0x0054875c` (.data) |
| `0x4c48c0` | `DynObj_LookupHashItem_5` | `0x00547f42` (.data) |
| `0x4c56d0` | `DynObj_RemoveSymbol_5` | `0x00547f3c` (.data) |
| `0x4c56e0` | `DynObj_RegisterSymbol_6` | `0x00547f3d` (.data) |
| `0x4c56f0` | `DynObj_FindSymbol_6` | `0x00547f3e` (.data) |
| `0x4c5700` | `DynObj_InsertHashItem_6` | `0x00547f3f` (.data) |
| `0x4c5800` | `DynObj_CreateMagicObject_6` | `0x00547f40` (.data) |
| `0x4c5900` | `DynObj_RemoveSymbol_6` | `0x00548176` (.data), `0x00548177` (.data), `0x0054875c` (.data), `0x00a44348` (.data), `0x00a4434c` (.data), `0x00a44350` (.data), `0x00a44354` (.data), `0x00a44358` (.data), `0x00a4435c` (.data), `0x00a44360` (.data), `0x00a44364` (.data), `0x00a44368` (.data), `0x00a4436c` (.data), `0x00a44370` (.data), `0x00a44374` (.data), `0x00a44378` (.data), `0x00a4437c` (.data), `0x00a44380` (.data), `0x00a44384` (.data), `0x00a44388` (.data) |
| `0x4cea10` | `DynObj_UpdateDynTree_12` | `0x00548176` (.data) |
| `0x4cea20` | `DynObj_LinkDynamicCube_12` | `0x00548177` (.data) |
| `0x4cea60` | `DynObj_RegisterSymbol_13` | `0x00548200` (.data) |
| `0x4cea70` | `DynObj_FindSymbol_13` | `0x00548204` (.data) |
| `0x4cea80` | `Render_GetActiveBackendId` | `0x00548208` (.data) |
| `0x4cea90` | `DynObj_LookupHashItem_13` | `0x0054820c` (.data) |
| `0x4ceba0` | `DynObj_MtpNlodvirmodelD` | `0x00548210` (.data), `0x00a443a0` (.data), `0x00a4edec` (.data) |
| `0x4ceeb0` | `DynObj_Initmodels` | `0x00548200` (.data), `0x00548204` (.data), `0x00548208` (.data), `0x0054820c` (.data), `0x00548230` (.data), `0x0054823c` (.data), `0x00548248` (.data), `0x00548258` (.data), `0x00a443a0` (.data), `0x00a4ec60` (.data), `0x00a4ec64` (.data), `0x00a4ec68` (.data), `0x00a4ed6c` (.data), `0x00a4edec` (.data), `0x00a4edf0` (.data) |
| `0x4cfc30` | `DynObj_ParseConfigFile_15` | `0x00b46d14` (.data) |
| `0x4cfc40` | `DynObj_UpdateDynTree_15` | `0x00b81880` (.data) |
| `0x4cfc50` | `DynObj_LinkDynamicCube_15` | `0x00b46ce0` (.data) |
| `0x4cfc60` | `DynObj_RemoveSymbol_15` | `0x00a4ee78` (.data) |
| `0x4cfc70` | `DynObj_RegisterSymbol_16` | `0x00548367` (.data) |
| `0x4cfc80` | `DynObj_FindSymbol_16` | `0x00548366` (.data) |
| `0x4cfc90` | `DynObj_InsertHashItem_16` | `0x00548365` (.data) |
| `0x4cfca0` | `DynObj_LookupHashItem_16` | `0x00548368` (.data) |
| `0x4d0b30` | `Render_Genericuvfromrenderm` | `0x005333e0` (.rdata), `0x005336c4` (.rdata), `0x005338c0` (.rdata), `0x0053421c` (.rdata), `0x00548364` (.data), `0x00548365` (.data), `0x00548366` (.data), `0x00548367` (.data), `0x00548368` (.data), `0x0054836c` (.data), `0x005483b0` (.data), `0x005483c0` (.data), `0x005483d4` (.data), `0x005483e4` (.data), `0x005483f4` (.data), `0x00548404` (.data), `0x00548420` (.data), `0x00a4ee78` (.data), `0x00b46ce0` (.data), `0x00b46d14` (.data), `0x00b81700` (.data), `0x00b81704` (.data), `0x00b81708` (.data), `0x00b8170c` (.data), `0x00b817a0` (.data), `0x00b817c0` (.data), `0x00b81880` (.data), `0x00b818dc` (.data) |
| `0x4d0ef0` | `Render_TransformVertices_3` | `0x00548060` (.data), `0x00548440` (.data), `0x00548441` (.data), `0x00a4ef14` (.data), `0x00a50ffc` (.data), `0x00b12340` (.data), `0x00b12b40` (.data), `0x00b12be0` (.data), `0x00b12c00` (.data), `0x00b12c20` (.data), `0x00b16c98` (.data) |
| `0x4d14c0` | `Render_DrawBoneMesh_4` | `0x00548441` (.data) |
| `0x4d14d0` | `Render_DrawSplineMesh_4` | `0x00548440` (.data) |
| `0x4d2b80` | `Render_NoMoreQcubeCallbacks` | `0x005484dc` (.data), `0x00a4eea8` (.data), `0x00a4eeac` (.data), `0x00a4eefc` (.data), `0x00a538e4` (.data) |
| `0x4d2bd0` | `Render_CallbackNotFound` | `0x00548500` (.data), `0x00a538e4` (.data) |
| `0x4d2c30` | `Render_BindTexture_4` | `0x00a538f0` (.data), `0x00a542f4` (.data), `0x00a545f0` (.data), `0x00a545f4` (.data), `0x00a54608` (.data), `0x00a54620` (.data), `0x00a54640` (.data), `0x00a54658` (.data), `0x00a54660` (.data), `0x00a54678` (.data), `0x00a54684` (.data), `0x00a54688` (.data) |
| `0x4d38a0` | `Render_DrawRigidMesh_7` | `0x00a538e8` (.data), `0x00a538ec` (.data) |
| `0x4d7550` | `BoneDynCubeObj_Init` | `0x00548618` (.data), `0x0054861a` (.data), `0x0054861c` (.data), `0x0054875c` (.data) |
| `0x4d9610` | `BoneDynCubeObj_GetQTaskType` | `0x00548618` (.data) |
| `0x4d9730` | `QObj_Init` | `0x00548638` (.data), `0x0054863a` (.data), `0x0054863c` (.data) |
| `0x4d97e0` | `QObj_OnEvent` | `0x0054863a` (.data) |
| `0x4d9810` | `Qtask_Initialize` | `0x00548644` (.data), `0x00a54690` (.data) |
| `0x4d9a00` | `MagicObj_Initialize` | `0x00548648` (.data), `0x0054864a` (.data), `0x0054864b` (.data), `0x0054864c` (.data), `0x00548660` (.data) |
| `0x4d9fa0` | `MagicObj_GetTypeId` | `0x00548648` (.data) |
| `0x4d9fb0` | `MagicObj_GetEventId` | `0x0054864a` (.data) |
| `0x4da1d0` | `Hazinglayers_Init` | `0x00548638` (.data), `0x0054866c` (.data), `0x00a5e304` (.data) |
| `0x4da880` | `Render_BindTexture_12` | `0x00b041e0` (.data) |
| `0x4dcf20` | `Sector_Init` | `0x00548694` (.data), `0x00548696` (.data), `0x00548698` (.data), `0x0054869c` (.data), `0x005486b4` (.data), `0x005486c4` (.data), `0x00a5e318` (.data), `0x00a5e338` (.data) |
| `0x4e0830` | `SoundSys_Update3DPosition` | `0x00548698` (.data) |
| `0x4e0840` | `MagicObjContainer_Init` | `0x005486f0` (.data), `0x00a5e364` (.data) |
| `0x4e0950` | `Shadow_Init` | `0x00548740` (.data), `0x00a5e36c` (.data) |
| `0x4e41b0` | `SplineObjSegment_Init` | `0x00548748` (.data), `0x00a5e3f4` (.data) |
| `0x4e5350` | `SoundSys_EvaluateTrigger_2` | `0x00a5e3f4` (.data) |
| `0x4e5360` | `SoundSys_AttachLipsync_2` | `0x0054875e` (.data) |
| `0x4e5370` | `DynCubeObj_Init` | `0x00548638` (.data), `0x0054875c` (.data), `0x0054875e` (.data), `0x00548760` (.data) |
| `0x4e55b0` | `SoundSys_PlaySoundSample_3` | `0x00a5e408` (.data) |
| `0x4e55c0` | `SoundSys_Update3DPosition_3` | `0x00a5e408` (.data), `0x00b019e0` (.data) |
| `0x4e5d90` | `SoundSys_PlaySoundSample_4` | `0x00548791` (.data), `0x005487a8` (.data), `0x005487a9` (.data), `0x005487d8` (.data), `0x00a5e424` (.data), `0x00a5e464` (.data), `0x00b015c8` (.data), `0x00b015e0` (.data), `0x00b017e0` (.data) |
| `0x4e6070` | `SoundSysController_Init` | `0x00548808` (.data), `0x0054880a` (.data), `0x0054880c` (.data), `0x0054880e` (.data), `0x00548810` (.data), `0x0054882c` (.data), `0x00548840` (.data), `0x0054884c` (.data), `0x00548858` (.data), `0x00548864` (.data), `0x00548878` (.data), `0x00548884` (.data), `0x00a5e5ec` (.data), `0x00a5e5f0` (.data), `0x00a5e5f8` (.data), `0x00a5e5fc` (.data), `0x00a5e600` (.data), `0x00a5e60a` (.data), `0x00a5e60c` (.data), `0x00a5e610` (.data) |
| `0x4e65f0` | `SoundSys_SetMasterVolume_5` | `0x00548812` (.data) |
| `0x4e6610` | `SoundSys_SetFrequency_5` | `0x00548813` (.data) |
| `0x4e6630` | `SoundSys_StopSoundSample_5` | `0x00548814` (.data) |
| `0x4e6650` | `SoundSys_LoadSoundDef_5` | `0x00548815` (.data) |
| `0x4e6670` | `SoundSys_EvaluateTrigger_5` | `0x00548816` (.data) |
| `0x4e6690` | `SoundSys_AttachLipsync_5` | `0x00548817` (.data) |
| `0x4e66b0` | `SoundSys_InitSoundSys_6` | `0x00548818` (.data) |
| `0x4e66d0` | `SoundSys_PlaySoundSample_6` | `0x0054881b` (.data) |
| `0x4e66f0` | `SoundSys_Update3DPosition_6` | `0x00548819` (.data) |
| `0x4e6710` | `SoundSys_SetMasterVolume_6` | `0x0054881a` (.data) |
| `0x4e6730` | `SoundSys_SetFrequency_6` | `0x0054881c` (.data) |
| `0x4e7350` | `SoundSys_Update3DPosition_9` | `0x00a5e5e8` (.data) |
| `0x4e7370` | `Screen_Init` | `0x005488c8` (.data), `0x005488d0` (.data), `0x00a5e618` (.data), `0x00a5e940` (.data), `0x00a5ea65` (.data), `0x00a5ea66` (.data) |
| `0x4e7530` | `StatusScreen_GetEvent` | `0x005488c8` (.data) |
| `0x4e7840` | `DebugText_Init` | `0x005488dc` (.data), `0x0054d958` (.data), `0x0054d968` (.data), `0x00a5ea70` (.data), `0x00a5ea74` (.data), `0x00a5ea75` (.data), `0x00a5ea76` (.data), `0x00a5ebcc` (.data), `0x00a5ebd0` (.data), `0x00a5ebd4` (.data) |
| `0x4e7c50` | `CameraControlTask_Init` | `0x0054d974` (.data), `0x0054d978` (.data) |
| `0x4e7d90` | `ViewportQTask_Init` | `0x0054d98c` (.data), `0x0054d990` (.data), `0x00a5ebe0` (.data), `0x00a5ec64` (.data) |
| `0x4e7e20` | `ViewportQTask_Process` | `0x00a5ec6c` (.data) |
| `0x4e8290` | `SoundSys_InitSoundSys_13` | `0x00a5ebe0` (.data), `0x00a5ec1c` (.data) |
| `0x4e82c0` | `TerraDrawTask_Init` | `0x0054d9a0` (.data), `0x0054d9a4` (.data) |
| `0x4e8390` | `SoundDefSound_Init` | `0x0054d9b4` (.data), `0x0054d9b6` (.data), `0x0054d9b8` (.data) |
| `0x4e92c0` | `FadeOutEnd_Init` | `0x0054da34` (.data), `0x0054da40` (.data), `0x00a5ec70` (.data), `0x00a5ec74` (.data), `0x00a5ec78` (.data) |
| `0x4e9480` | `GlowTask_Init` | `0x0054da50` (.data), `0x0054da54` (.data) |
| `0x4e9520` | `SoundDefTriggerOnce_Init` | `0x0054da60` (.data), `0x0054da62` (.data), `0x0054da64` (.data) |
| `0x4ea3d0` | `SoundDefGraph_Init` | `0x0054dad8` (.data), `0x0054dada` (.data), `0x0054dadc` (.data) |
| `0x4eb1d0` | `SoundDefGroup_Init` | `0x0054db48` (.data), `0x0054db4a` (.data), `0x0054db4c` (.data) |
| `0x4ebc60` | `SoundSysMicrophone_Init` | `0x0054db68` (.data), `0x0054db6a` (.data), `0x0054db6c` (.data) |
| `0x4ebdd0` | `SoundSysMicrophone_Process` | `0x0054db6a` (.data) |
| `0x4ebe70` | `SoundSys_Defineanimtrigger` | `0x0054db80` (.data), `0x0054db81` (.data), `0x0054db82` (.data), `0x0054db84` (.data) |
| `0x4ec040` | `SoundSys_SetFrequency_15` | `0x0054db80` (.data) |
| `0x4ec050` | `SoundSys_StopSoundSample_15` | `0x0054db81` (.data) |
| `0x4ec060` | `SoundSys_LoadSoundDef_15` | `0x0054db82` (.data) |
| `0x4ec160` | `SoundSys_AttachLipsync_15` | `0x00a5ef88` (.data), `0x00a5ef90` (.data) |
| `0x4ed000` | `SoundSys_SetFrequency_17` | `0x00a5ef98` (.data) |
| `0x4ed510` | `InputPort_Init` | `0x0054dbd0` (.data), `0x00a5efa0` (.data), `0x00bc20a0` (.data), `0x00bc2150` (.data), `0x00bc2200` (.data), `0x00bc22b0` (.data) |
| `0x4ed6b0` | `LODSettings_Init` | `0x0054dbdc` (.data), `0x0054dbde` (.data), `0x0054dbe0` (.data), `0x0054dbe4` (.data), `0x0054dc00` (.data), `0x0054dc14` (.data), `0x00a5efa8` (.data), `0x00a5efac` (.data), `0x00a5efb0` (.data), `0x00a5efb4` (.data), `0x00a5efb8` (.data), `0x00a5efbc` (.data), `0x00a5efc0` (.data), `0x00a5efc4` (.data), `0x00a5efc8` (.data), `0x00a5efcc` (.data) |
| `0x4edce0` | `SoundSys_Definephysicsobjtype` | `0x0054dc30` (.data), `0x0054dc31` (.data), `0x0054dc32` (.data), `0x0054dc34` (.data) |
| `0x4ee0e0` | `SoundSys_Update3DPosition_18` | `0x0054dc30` (.data) |
| `0x4ee0f0` | `SoundSys_SetMasterVolume_18` | `0x0054dc31` (.data) |
| `0x4ee100` | `SoundSys_SetFrequency_18` | `0x0054dc32` (.data) |
| `0x4ee410` | `ScreenGrab_Init` | `0x0054dcb8` (.data), `0x0054dcc0` (.data), `0x00a70c5a` (.data), `0x00a70c5b` (.data), `0x00a70c60` (.data), `0x00a70c64` (.data) |
| `0x4ee970` | `CameraBase_Init` | `0x00539688` (.data), `0x00539abc` (.data), `0x00541ba4` (.data), `0x0054dd0c` (.data), `0x0054dd0e` (.data), `0x0054dd10` (.data), `0x0054dd12` (.data), `0x0054dd14` (.data), `0x0054dda4` (.data), `0x0054dda5` (.data), `0x0054dda8` (.data), `0x0054ddac` (.data), `0x0054ddb0` (.data), `0x0054dec0` (.data), `0x0054ded8` (.data), `0x0054dee8` (.data), `0x0054def4` (.data), `0x0054df00` (.data), `0x0054df08` (.data), `0x0054df10` (.data), `0x0054df14` (.data), `0x0054df20` (.data), `0x0054df2c` (.data), `0x0054df38` (.data), `0x0054df48` (.data), `0x0054df50` (.data), `0x0054df58` (.data), `0x0054df64` (.data), `0x0054df74` (.data), `0x0054df80` (.data), `0x0054df8c` (.data), `0x0054df98` (.data), `0x0054dfa4` (.data), `0x0054dfac` (.data), `0x0054dfb4` (.data), `0x0054dfc0` (.data), `0x0054dfc8` (.data), `0x0054dfd4` (.data), `0x0054dfe0` (.data), `0x0054dfe8` (.data), `0x0054dff0` (.data), `0x0054dffc` (.data), `0x0054e004` (.data), `0x00a70c68` (.data), `0x00a70c6c` (.data), `0x00a70c70` (.data), `0x00a70c78` (.data), `0x00a70c7c` (.data), `0x00a70c80` (.data), `0x00a70c84` (.data), `0x00a70c88` (.data), `0x00a70c8c` (.data), `0x00a70c90` (.data), `0x00a70c9c` (.data), `0x00a70ca0` (.data), `0x00a70ca4` (.data), `0x00a70ca8` (.data), `0x00a7186c` (.data), `0x00a71870` (.data), `0x00a71874` (.data), `0x00a71878` (.data), `0x00a7187c` (.data), `0x00a71880` (.data), `0x00a71884` (.data), `0x00a71888` (.data), `0x00a7188c` (.data), `0x00a71890` (.data), `0x00a75890` (.data), `0x00a75894` (.data), `0x00a75898` (.data), `0x00a758a0` (.data), `0x00a758bd` (.data), `0x00a758be` (.data), `0x00a758c0` (.data), `0x00a758c4` (.data) |
| `0x4f0ef0` | `Script_SpawnNewTask_2` | `0x00a75898` (.data) |
| `0x4f0f20` | `Script_ParseEnumString_2` | `0x00a70c84` (.data) |
| `0x4f0f30` | `Script_ExecuteLevelScript_2` | `0x00a70ca4` (.data) |
| `0x4f0f40` | `Script_CallNativeHandler_2` | `0x00a71880` (.data) |
| `0x4f0f50` | `Script_EvaluateCondition_2` | `0x00a71870` (.data) |
| `0x4f0f60` | `Script_FormatString_2` | `0x00a70c68` (.data) |
| `0x4f0f70` | `Script_SpawnNewTask_3` | `0x00a70c6c` (.data) |
| `0x4f0f80` | `Script_ParseEnumInt32_3` | `0x00a71874` (.data) |
| `0x4f0fa0` | `Script_ParseEnumString_3` | `0x00a70c7c` (.data) |
| `0x4f0fc0` | `Script_CallNativeHandler_3` | `0x00a70c9c` (.data) |
| `0x4f0fd0` | `Script_EvaluateCondition_3` | `0x00a71884` (.data) |
| `0x4f0fe0` | `Script_FormatString_3` | `0x00a71888` (.data) |
| `0x4f0ff0` | `Script_SpawnNewTask_4` | `0x00a7186c` (.data) |
| `0x4f1000` | `Script_ParseEnumInt32_4` | `0x00a70c90` (.data) |
| `0x4f1010` | `Script_ParseEnumReal32_4` | `0x00a70c88` (.data) |
| `0x4f1020` | `Script_ParseEnumString_4` | `0x00a70ca0` (.data) |
| `0x4f11f0` | `Script_CallNativeHandler_5` | `0x0054dda4` (.data) |
| `0x4f1210` | `Script_FormatString_5` | `0x0054dda8` (.data) |
| `0x4f1220` | `Script_SpawnNewTask_6` | `0x0054ddac` (.data) |
| `0x4f1840` | `Script_ExecuteLevelScript_7` | `0x0054e0e4` (.data), `0x0054e0e5` (.data), `0x0054e0e6` (.data), `0x0054e0e7` (.data), `0x0054e0e8` (.data), `0x0054e0e9` (.data), `0x0054e0ea` (.data), `0x0054e0eb` (.data), `0x00a758c8` (.data), `0x00a758cc` (.data), `0x00a758d0` (.data), `0x00a758d4` (.data), `0x00a758e0` (.data) |
| `0x4f19c0` | `Script_UnableToAllocatePara` | `0x0054e0ec` (.data), `0x00a758e0` (.data), `0x00a758e4` (.data), `0x00a75928` (.data), `0x00a76c28` (.data) |
| `0x4f1a60` | `Script_ParseEnumInt32_8` | `0x0054e0e4` (.data) |
| `0x4f1a70` | `Script_ParseEnumReal32_8` | `0x0054e0e5` (.data) |
| `0x4f1a80` | `Script_UndefinedParameterTy` | `0x0054e110` (.data) |
| `0x4f1b20` | `Script_UnableToAllocatePara_2` | `0x0054e12c` (.data), `0x00a758c8` (.data) |
| `0x4f1b60` | `Script_CallNativeHandler_8` | `0x0054e0ea` (.data) |
| `0x4f1b70` | `Script_EvaluateCondition_8` | `0x0054e0e6` (.data) |
| `0x4f1b80` | `Script_FormatString_8` | `0x0054e0eb` (.data) |
| `0x4f1b90` | `Script_SpawnNewTask_9` | `0x0054e0e9` (.data) |
| `0x4f1ba0` | `Script_ParseEnumInt32_9` | `0x0054e0e7` (.data) |
| `0x4f1bb0` | `Script_ParseEnumReal32_9` | `0x0054e0e8` (.data) |
| `0x4f1bc0` | `Script_ParseEnumString_9` | `0x00a758e4` (.data), `0x00a758e8` (.data), `0x00a76be4` (.data) |
| `0x4f1c90` | `Script_CallNativeHandler_9` | `0x00a76bf0` (.data), `0x00a76bf4` (.data) |
| `0x4f1cb0` | `Script_EvaluateCondition_9` | `0x00a76be8` (.data), `0x00a76bec` (.data) |
| `0x4f1e20` | `Script_CallNativeHandler_10` | `0x0054e158` (.data) |
| `0x4f1e30` | `Script_Enumint32` | `0x0054e150` (.data), `0x0054e154` (.data), `0x0054e158` (.data), `0x0054e15c` (.data), `0x0054e160` (.data), `0x0054e164` (.data), `0x0054e174` (.data), `0x0054e184` (.data), `0x0054e194` (.data), `0x0054e1a0` (.data) |
| `0x4f1ee0` | `Script_FormatString_10` | `0x00a76bfc` (.data), `0x00a76c00` (.data), `0x00a76c01` (.data), `0x00a76c02` (.data), `0x00a76c03` (.data), `0x00a76c04` (.data), `0x00a76c08` (.data), `0x00a76c09` (.data), `0x00a76c10` (.data) |
| `0x4f2740` | `EditRigidObj_Init` | `0x0054e1ac` (.data), `0x0054e1b0` (.data), `0x0054e1c8` (.data), `0x00a76c14` (.data) |
| `0x4f3440` | `AnimTask_Init` | `0x0054e1d8` (.data), `0x0054e1da` (.data), `0x0054e1db` (.data), `0x0054e1dc` (.data), `0x0054e1dd` (.data), `0x0054e1de` (.data), `0x0054e1df` (.data), `0x0054e1e0` (.data), `0x0054e1e4` (.data), `0x0054e1e8` (.data), `0x0054e1f4` (.data) |
| `0x4f36c0` | `Script_FormatString_13` | `0x0054e1d8` (.data), `0x0054e1db` (.data), `0x0054e1e4` (.data), `0x00a76c20` (.data), `0x00a76c28` (.data), `0x00a76c2c` (.data), `0x00a76c30` (.data), `0x00a76c34` (.data), `0x00a76c3c` (.data) |
| `0x4f40d0` | `Script_ParseEnumReal32_15` | `0x0054e1da` (.data) |
| `0x4f40e0` | `Script_ParseEnumString_15` | `0x0054e1dc` (.data) |
| `0x4f40f0` | `Script_ExecuteLevelScript_15` | `0x0054e1e4` (.data) |
| `0x4f4100` | `Script_CallNativeHandler_15` | `0x0054e1d8` (.data) |
| `0x4f4110` | `Script_EvaluateCondition_15` | `0x0054e1dd` (.data) |
| `0x4f4120` | `Script_FormatString_15` | `0x0054e1de` (.data) |
| `0x4f4130` | `Script_SpawnNewTask_16` | `0x0054e1df` (.data) |
| `0x4f4140` | `Script_ParseEnumInt32_16` | `0x0054e1e0` (.data) |
| `0x4f4190` | `Vehicle_Init` | `0x0054e200` (.data), `0x0054e202` (.data), `0x0054e203` (.data), `0x0054e284` (.data), `0x0054e298` (.data), `0x00a76c48` (.data) |
| `0x4f4b00` | `CutScene_TaskCreate` | `0x0054e2a0` (.data), `0x0054e2b4` (.data), `0x0054e2c0` (.data), `0x00a76c50` (.data), `0x00a76c58` (.data), `0x00a76c60` (.data), `0x00a76c64` (.data), `0x00a76c68` (.data), `0x00a76c70` (.data) |
| `0x4f7360` | `RainDynCubeObj_Init` | `0x0054875c` (.data), `0x0054e2cc` (.data), `0x0054e2d8` (.data), `0x0054e2e8` (.data), `0x00a76c8c` (.data), `0x00a76c90` (.data), `0x00a76c9c` (.data), `0x00a76ca4` (.data), `0x00a76ccc` (.data) |
| `0x4f8340` | `TerrainLightMap_Init` | `0x0054875c` (.data), `0x0054e304` (.data), `0x00a76cd0` (.data), `0x00a76cdc` (.data), `0x00a76d04` (.data), `0x00b97d24` (.data) |
| `0x4f9200` | `Script_EvaluateCondition_21` | `0x00a76d10` (.data) |
| `0x4f9210` | `Script_FormatString_21` | `0x0054e364` (.data) |
| `0x4f9220` | `Script_SpawnNewTask_22` | `0x0054e36b` (.data) |
| `0x4f9230` | `Script_ParseEnumInt32_22` | `0x0054e36c` (.data) |
| `0x4f9240` | `Script_ParseEnumReal32_22` | `0x0054e36d` (.data) |
| `0x4f9250` | `Script_ParseEnumString_22` | `0x0054e36e` (.data) |
| `0x4f9260` | `Script_ExecuteLevelScript_22` | `0x0054e36f` (.data) |
| `0x4f9270` | `Script_CallNativeHandler_22` | `0x0054e371` (.data) |
| `0x4f9280` | `Script_EvaluateCondition_22` | `0x0054e369` (.data) |
| `0x4f9460` | `Script_NoFreeNodesizeElemen` | `0x0054e374` (.data), `0x00a77228` (.data), `0x00a7722c` (.data), `0x00a77248` (.data) |
| `0x4f9df0` | `Script_GraphstreamCollision` | `0x0054e3e8` (.data), `0x00a76d18` (.data), `0x00a774a0` (.data) |
| `0x4faa60` | `Graph_Init` | `0x0054df48` (.data), `0x0054e364` (.data), `0x0054e368` (.data), `0x0054e369` (.data), `0x0054e36a` (.data), `0x0054e36b` (.data), `0x0054e36c` (.data), `0x0054e36d` (.data), `0x0054e36e` (.data), `0x0054e36f` (.data), `0x0054e370` (.data), `0x0054e371` (.data), `0x0054e430` (.data), `0x0054e43c` (.data), `0x0054e448` (.data), `0x0054e454` (.data), `0x0054e464` (.data), `0x0054e474` (.data), `0x0054e480` (.data), `0x0054e48c` (.data), `0x0054e49c` (.data), `0x0054e4a4` (.data), `0x00a76d10` (.data), `0x00a76d18` (.data), `0x00a76d1c` (.data), `0x00a76d20` (.data), `0x00a76d28` (.data), `0x00a77228` (.data), `0x00a7722c` (.data), `0x00a77248` (.data), `0x00a7724c` (.data), `0x00a77250` (.data), `0x00a774a0` (.data), `0x00a774a4` (.data), `0x00a774a8` (.data) |
| `0x4fb1b0` | `PhysicsObj_Init` | `0x0054e4cc` (.data), `0x0054e4cd` (.data), `0x0054e4ce` (.data), `0x0054e4d0` (.data), `0x00a774b0` (.data), `0x00a774b8` (.data) |
| `0x4fb5e0` | `Script_SpawnNewTask_28` | `0x00a774b0` (.data) |
| `0x4fc850` | `Script_ExecuteLevelScript_29` | `0x0054e4cd` (.data) |
| `0x4fcf50` | `Particle2DDynCubeObjTask_Init` | `0x0054875c` (.data), `0x0054e568` (.data), `0x0054e56a` (.data), `0x0054e56c` (.data), `0x00a7a4c0` (.data), `0x00a7a4d0` (.data), `0x00a7a4d4` (.data) |
| `0x4fe7c0` | `TagItem_Init` | `0x0054e588` (.data), `0x0054e59c` (.data), `0x0054e5ac` (.data), `0x0054e5bc` (.data), `0x0054e5cc` (.data), `0x00a7a4e0` (.data), `0x00a7a4e4` (.data), `0x00a7a4e8` (.data), `0x00a7a4ec` (.data), `0x00a7a4f0` (.data), `0x00a7a4f4` (.data), `0x00a7a4f6` (.data), `0x00a7a4f8` (.data), `0x00a7a4fc` (.data), `0x00a7a4fe` (.data), `0x00a7a500` (.data) |
| `0x502250` | `SplineObj_Init` | `0x0054e5dc` (.data), `0x0054e5f0` (.data), `0x00a7a508` (.data), `0x00a7a610` (.data), `0x00a7a614` (.data), `0x00a7a618` (.data), `0x00a7a61c` (.data), `0x00a7a620` (.data) |
| `0x5033f0` | `EditorRigidObj_Init` | `0x0054e634` (.data), `0x0054e638` (.data) |
| `0x5034d0` | `EditorRigidObj_OnEvent` | `0x0054e634` (.data) |
| `0x5034e0` | `SimpointObj_Init` | `0x0054e648` (.data), `0x0054e654` (.data), `0x00a7a628` (.data), `0x00a7a62a` (.data), `0x00a7a630` (.data) |
| `0x503d60` | `PhysicsMagicObj_Init` | `0x0054e66c` (.data), `0x0054e670` (.data), `0x0054e68c` (.data) |
| `0x5043b0` | `AreaActivate_Init` | `0x0054e69c` (.data), `0x0054e69e` (.data), `0x0054e6a0` (.data), `0x00a7a7b4` (.data) |
| `0x504a30` | `Physics_UnableToRegisterArea` | `0x0054e6bc` (.data), `0x00a7a640` (.data), `0x00a7a644` (.data), `0x00a7a648` (.data), `0x00a7a7b4` (.data) |
| `0x504c50` | `EditRigidObjAttachedToBone_Init` | `0x0054e6ec` (.data), `0x00ba8080` (.data), `0x00ba8084` (.data) |
| `0x504df0` | `EditRigidObjAttachedToBone_Action` | `0x0054e70b` (.data) |
| `0x504e00` | `EditRigidObjAttachedToBone_OnTick` | `0x0054e70a` (.data) |
| `0x504e10` | `MenuItemTask_Init` | `0x0054e708` (.data), `0x0054e70a` (.data), `0x0054e70b` (.data), `0x0054e70c` (.data) |
| `0x505630` | `MenuItem_Init` | `0x0054e71c` (.data), `0x0054e71d` (.data), `0x0054e71e` (.data), `0x0054e720` (.data), `0x0054e724` (.data) |
| `0x505c50` | `Physics_CalculateSplineLength_5` | `0x0054e71e` (.data) |
| `0x505c60` | `Physics_InterpolateWaypoint_5` | `0x0054e71d` (.data) |
| `0x505c70` | `Physics_EvaluateAreaCriteria_5` | `0x0054e71c` (.data) |
| `0x505c80` | `Physics_SimulateRigidBody_5` | `0x0054e720` (.data) |
| `0x505c90` | `Container_Init` | `0x0054e76c` (.data), `0x00a7a7bc` (.data) |
| `0x505f60` | `TextureModifier_Init` | `0x0054875c` (.data), `0x0054e7a4` (.data), `0x00a7a8ec` (.data), `0x00a7a8f0` (.data), `0x00a7acf8` (.data), `0x00b97df8` (.data) |
| `0x506fa0` | `HeightMap_Init` | `0x0054875c` (.data), `0x0054e7e0` (.data), `0x00a7b138` (.data), `0x00a7b244` (.data), `0x00a7b248` (.data), `0x00b97df4` (.data) |
| `0x507e70` | `Physics_SimulateRigidBody_6` | `0x00a7b254` (.data) |
| `0x507f10` | `EditCamera_Init` | `0x0054e7ec` (.data), `0x0054e800` (.data), `0x0054e80c` (.data), `0x0054e818` (.data), `0x00a7b260` (.data), `0x00a7b268` (.data), `0x00a7b278` (.data), `0x00a7b280` (.data), `0x00a7b284` (.data), `0x00a7b288` (.data), `0x00a7b28c` (.data), `0x00a7b290` (.data) |
| `0x508d60` | `Physics_SimulateRigidBody_7` | `0x00a7b268` (.data) |
| `0x508d70` | `Sync_Init` | `0x0054e830` (.data), `0x0054e831` (.data), `0x0054e834` (.data), `0x00a7b298` (.data) |
| `0x508f60` | `Sync_Process` | `0x0054e830` (.data) |
| `0x508f70` | `Physics_CheckHullCollision_7` | `0x0054e831` (.data) |
| `0x508fa0` | `AIGraph_Init` | `0x0054e83c` (.data), `0x0054e840` (.data), `0x0054e84c` (.data), `0x0054e860` (.data), `0x0054e874` (.data), `0x0054e888` (.data), `0x00a7b2a8` (.data), `0x00a7b2ac` (.data), `0x00a7b2b0` (.data), `0x00a7b2b4` (.data), `0x00a7b2b8` (.data), `0x00a7b2bc` (.data), `0x00a7b2c0` (.data), `0x00a7b2c4` (.data), `0x00a7b2cc` (.data), `0x00a7b2d0` (.data), `0x00a7b2d4` (.data), `0x00a7b2d8` (.data) |
| `0x5098b0` | `Physics_Local` | `0x0053f30c` (.data) |
| `0x509930` | `Physics_QvertexDrawstrip` | `0x0054e890` (.data), `0x0054e894` (.data) |
| `0x509980` | `Physics_CouldNotCreateDirect` | `0x0054e8e8` (.data), `0x0054e920` (.data), `0x0054e950` (.data), `0x00a7b430` (.data), `0x00a7b4e0` (.data), `0x00a7b4f0` (.data), `0x00a7b638` (.data) |
| `0x509cc0` | `Physics_CalculateSplineLength_10` | `0x00a7b2e0` (.data), `0x00a7b4e0` (.data) |
| `0x509f00` | `Physics_SimulateRigidBody_10` | `0x00a7b440` (.data), `0x00a7b4e0` (.data), `0x00bcaa00` (.data), `0x00bcaa01` (.data), `0x00bcaa03` (.data), `0x00bcaa04` (.data), `0x00bcaa07` (.data) |
| `0x50a6c0` | `Physics_ApplyImpulse_11` | `0x00546fa8` (.data), `0x00546fac` (.data), `0x00546fb0` (.data), `0x00546fb4` (.data), `0x00546fb8` (.data), `0x00546fbc` (.data), `0x00a7c128` (.data), `0x00a84a40` (.data), `0x00a84a44` (.data), `0x00a84a48` (.data) |
| `0x50bd80` | `Physics_ApplyImpulse_12` | `0x00546fa8` (.data), `0x00546fac` (.data), `0x00546fb0` (.data), `0x00a7cec8` (.data), `0x00a7cecc` (.data), `0x00a84a40` (.data), `0x00a84a44` (.data), `0x00a84a48` (.data) |
| `0x50dc30` | `Physics_SimulateRigidBody_13` | `0x0054e890` (.data), `0x00a7ced8` (.data), `0x00a7d9f4` (.data), `0x00a7d9f8` (.data), `0x00a7da00` (.data), `0x00a7da04` (.data), `0x00a84a54` (.data), `0x00bcad7c` (.data), `0x00bcadac` (.data), `0x00bcade8` (.data) |
| `0x50ec00` | `Physics_CalculateSplineLength_14` | `0x0054e890` (.data), `0x00a84a54` (.data) |
| `0x50fac0` | `Physics_CheckHullCollision_14` | `0x0054eba8` (.data), `0x00a7da10` (.data) |
| `0x512820` | `Terrain_Glowreadbufferprimit` | `0x005333e0` (.rdata), `0x005333ec` (.rdata), `0x00534c10` (.rdata), `0x00534c14` (.rdata), `0x00534c18` (.rdata), `0x00534c20` (.rdata), `0x0054ebc0` (.data), `0x0054ebd8` (.data), `0x00a7dbd8` (.data), `0x00a7dbe0` (.data), `0x00a7dc60` (.data), `0x00a7dc64` (.data) |
| `0x516600` | `Terrain_RenderQuadtree_3` | `0x00a7e080` (.data), `0x00a7f85c` (.data) |
| `0x5168fd` | `Terrain_BuildMipMaps_3` | `0x005443b0` (.data), `0x005443ba` (.data), `0x0054ec30` (.data), `0x00936058` (.data) |
| `0x518260` | `Terrain_RenderWaterSurface_8` | `0x005333e0` (.rdata), `0x005333ec` (.rdata), `0x00534c10` (.rdata), `0x00534c14` (.rdata), `0x00534c18` (.rdata), `0x00534c20` (.rdata), `0x0054ebc0` (.data), `0x0054ebd8` (.data), `0x00a7dbd8` (.data), `0x00a7dbe0` (.data), `0x00a7dc60` (.data), `0x00a7dc64` (.data), `0x00a800d0` (.data), `0x00b015a0` (.data) |
| `0x5183d0` | `Terrain_CalculateNormals_8` | `0x00530220` (.text), `0x00530270` (.text) |
| `0x5186c0` | `Terrain_RenderQuadtree_9` | `0x0054ecdc` (.data), `0x00a800f8` (.data), `0x00a800fc` (.data), `0x00a801f8` (.data) |
| `0x518760` | `GraphNode_Init` | `0x0054ec9c` (.data), `0x0054ed00` (.data), `0x0054ed0c` (.data), `0x00a800f0` (.data), `0x00a800fc` (.data), `0x00a801fc` (.data) |
| `0x518ef0` | `Water_Init` | `0x0054ed18` (.data), `0x0054ed24` (.data), `0x00a80300` (.data), `0x00a80304` (.data), `0x00a80430` (.data), `0x00a81014` (.data), `0x00a81020` (.data), `0x00a81030` (.data) |
| `0x519710` | `GFXCapsViewer_Init` | `0x0054ed2c` (.data), `0x00ba2028` (.data) |
| `0x519760` | `ShadowTask_Init` | `0x0054ed3c` (.data), `0x0054ed40` (.data), `0x00a83744` (.data) |
| `0x519850` | `SplinePathNodeQTask_Init` | `0x0054ed4c` (.data), `0x0054ed54` (.data), `0x00a8374c` (.data) |
| `0x51a1d0` | `SplinePathDynCubeObj_Init` | `0x0054875c` (.data), `0x0054ed4e` (.data), `0x0054eda4` (.data) |
| `0x51b140` | `SplinePathGuideQTask_Init` | `0x0054ed50` (.data), `0x0054ed52` (.data), `0x0054ed53` (.data), `0x0054edbc` (.data), `0x00a83754` (.data), `0x00a83760` (.data) |
| `0x51b400` | `AnimSound_Init` | `0x0054edd4` (.data), `0x0054ede8` (.data), `0x00a83764` (.data) |
| `0x51b960` | `EditorMagicObj_Init` | `0x0054edf4` (.data), `0x0054ee0c` (.data), `0x00a8376c` (.data) |
| `0x51bac0` | `LevelTimer_Init` | `0x0054ee1c` (.data), `0x0054ee30` (.data), `0x00a83774` (.data), `0x00a83778` (.data) |
| `0x51bfb0` | `SoundManager_Init` | `0x0054ee3c` (.data), `0x0054ee40` (.data), `0x00a83780` (.data) |
| `0x51c2b0` | `FlatWater_Init` | `0x0054ee68` (.data), `0x0054ee70` (.data), `0x0054ee78` (.data), `0x0054ee88` (.data), `0x00ba2018` (.data), `0x00ba201c` (.data), `0x00ba2020` (.data), `0x00ba2024` (.data) |
| `0x51ddf0` | `FlatSky_Init` | `0x0054eeb0` (.data), `0x0054eeb8` (.data), `0x0054eec0` (.data), `0x0054eec8` (.data), `0x0054eed8` (.data), `0x00ba1fdc` (.data), `0x00ba1fde` (.data), `0x00ba1fe0` (.data), `0x00ba1fe4` (.data) |
| `0x51ecb0` | `MipMapControl_Init` | `0x0054eee0` (.data), `0x0054eee4` (.data), `0x0054ef00` (.data), `0x0054ef14` (.data), `0x0054ef28` (.data), `0x00a83788` (.data), `0x00a8378c` (.data), `0x00a83790` (.data), `0x00a83798` (.data), `0x00a8379c` (.data) |
| `0x51eee0` | `SmoothQTask_Init` | `0x0054ef58` (.data), `0x0054ef70` (.data), `0x00a837a0` (.data), `0x00a837a4` (.data) |
| `0x51f580` | `MoveRigidObj_Init` | `0x0054ef9c` (.data), `0x0054efb4` (.data), `0x0054efc0` (.data), `0x0054efc8` (.data), `0x0054efd4` (.data), `0x0054efe0` (.data), `0x00a837ac` (.data), `0x00a837cc` (.data), `0x00a837d0` (.data), `0x00a837d4` (.data), `0x00a837f4` (.data), `0x00a83814` (.data) |
| `0x51fef0` | `NoiseQTask_Init` | `0x0054eff0` (.data), `0x0054f004` (.data), `0x00a83838` (.data), `0x00a8383c` (.data), `0x00a83840` (.data) |
| `0x5207a0` | `Origo_Init` | `0x0054f010` (.data), `0x00a83850` (.data), `0x00a83854` (.data) |
| `0x520bb0` | `Lensflare_Init` | `0x0054f018` (.data), `0x0054f024` (.data), `0x0054f034` (.data), `0x00a83860` (.data), `0x00a83888` (.data), `0x00a83894` (.data), `0x00a83898` (.data), `0x00a8389c` (.data), `0x00a838b0` (.data), `0x00a83968` (.data), `0x00a83970` (.data), `0x00a83978` (.data), `0x00ba1e20` (.data) |
| `0x526330` | `LightmapInfo_Init` | `0x00533468` (.rdata), `0x0054f040` (.data), `0x0054f050` (.data), `0x0054f0ac` (.data), `0x00a8397c` (.data), `0x00a83980` (.data), `0x00a83a00` (.data), `0x00a83a18` (.data), `0x00a83b28` (.data), `0x00a83b30` (.data), `0x00a83b3c` (.data), `0x00a83b44` (.data), `0x00a83c58` (.data), `0x00a83cf8` (.data) |
| `0x526ee0` | `Dirlight_Init` | `0x0054f0d4` (.data), `0x0054f0e8` (.data), `0x00a83d30` (.data), `0x00a83d34` (.data), `0x00a83d38` (.data), `0x00a83d48` (.data), `0x00a83d4c` (.data), `0x00ba1e00` (.data) |
| `0x5274c0` | `Light_SampleLightmap_5` | `0x00a83d34` (.data) |
| `0x527c90` | `GlobalLight_Init` | `0x00534ca0` (.rdata), `0x0054f0f4` (.data), `0x0054f104` (.data), `0x0054f110` (.data), `0x0054f11c` (.data), `0x0054f128` (.data), `0x0054f138` (.data), `0x0054f148` (.data), `0x0054f15c` (.data), `0x0054f170` (.data), `0x0054f174` (.data), `0x0054f178` (.data), `0x0054f18c` (.data), `0x00a83d60` (.data), `0x00a83e00` (.data), `0x00a83ea4` (.data), `0x00a83f44` (.data), `0x00a83f48` (.data), `0x00a83fe8` (.data), `0x00a84088` (.data), `0x00a8408c` (.data), `0x00a84090` (.data), `0x00a84130` (.data), `0x00a84150` (.data), `0x00a8415c` (.data), `0x00a84168` (.data), `0x00a84174` (.data), `0x00a8417c` (.data), `0x00a84188` (.data), `0x00a84194` (.data), `0x00a841bc` (.data), `0x00a842d0` (.data), `0x00a842d4` (.data), `0x00a84374` (.data) |
| `0x5284a0` | `AnimCam_Init` | `0x00530260` (.text), `0x0054f198` (.data), `0x0054f410` (.data), `0x00a84388` (.data) |
| `0x52aaf0` | `AnimRigidDynCubeObj_Init` | `0x0054f498` (.data), `0x0054f678` (.data) |
| `0x52ad10` | `AnimScaleTask_Init` | `0x0054f68c` (.data), `0x0054f690` (.data), `0x00a84394` (.data) |
| `0x52b040` | `EditHazinglayer_Init` | `0x0054f6a0` (.data), `0x00a84398` (.data), `0x00a8439c` (.data) |
| `0x52b3b0` | `CubeModifier_Init` | `0x0054f6b0` (.data), `0x00a843a4` (.data), `0x00a843a8` (.data), `0x00a843ac` (.data) |
| `0x52b6c0` | `DiscardTerrainArea_Init` | `0x0054f6c0` (.data), `0x00a84540` (.data), `0x00a84544` (.data) |
| `0x52c0c0` | `DiscardTerrain_Init` | `0x0054f6d4` (.data), `0x00a8454c` (.data), `0x00a84550` (.data) |
| `0x52c6b0` | `SkyTask_Init` | `0x0054f6e4` (.data), `0x0054f6e8` (.data), `0x00a84558` (.data) |
| `0x52c820` | `LevelScript_Init` | `0x0054f6f0` (.data), `0x0054f6f2` (.data), `0x0054f6f4` (.data), `0x00a84564` (.data) |
| `0x52ca30` | `SimBoneDynCubeObj_Init` | `0x0054f700` (.data), `0x0054f704` (.data) |
| `0x52ca70` | `PushButton_Init` | `0x0054dee8` (.data), `0x0054f718` (.data) |
| `0x52d420` | `Light_UpdateGlobalLight_10` | `0x0054f72c` (.data), `0x00a84570` (.data) |
| `0x52dd70` | `Light_Watermesh` | `0x0054f75c` (.data), `0x00a84578` (.data) |
| `0x530220` | `CamTask_Init` | `0x0054f768` (.data), `0x0054f76c` (.data) |
| `0x530260` | `CamTask_OnEvent` | `0x0054f768` (.data) |
| `0x530270` | `CamTask_Process` | `0x00530800` (.text), `0x00530960` (.text), `0x005309c0` (.text), `0x00a5ef98` (.data) |
| `0x530800` | `Particle_Render3DSprites_2` | `0x00530950` (.text), `0x0054f778` (.data) |
| `0x530950` | `Particle_EmitParticles_3` | `0x00a845d8` (.data) |
| `0x530960` | `Particle_UpdateParticlePhysics_3` | `0x00a845e0` (.data), `0x00a845e1` (.data) |
| `0x5309c0` | `Particle_TriggerMuzzleFlash_3` | `0x00530a80` (.text), `0x00530c80` (.text), `0x005310e0` (.text), `0x00531190` (.text), `0x005311e0` (.text), `0x005342ec` (.rdata), `0x00543d4c` (.data), `0x00543d4e` (.data), `0x00543d50` (.data), `0x00543d58` (.data), `0x00543d5c` (.data), `0x00543d70` (.data), `0x00543d80` (.data), `0x00543d90` (.data), `0x00543da0` (.data), `0x00543dac` (.data), `0x00543dc0` (.data), `0x00543dcc` (.data), `0x00543de8` (.data), `0x00543e08` (.data), `0x00543e20` (.data), `0x00543e38` (.data), `0x00935bf0` (.data), `0x00935bfc` (.data), `0x00935c00` (.data), `0x00935c08` (.data), `0x00935c0c` (.data), `0x00936010` (.data), `0x00936014` (.data), `0x00936018` (.data), `0x00936038` (.data) |
| `0x530a80` | `FlashTask_Init` | `0x00530ad0` (.text), `0x00530c40` (.text), `0x0054f7a0` (.data), `0x0054f7a4` (.data), `0x00a845ec` (.data) |
| `0x530c80` | `ParticleSystemDynCubeObj_Init` | `0x00530cd0` (.text), `0x00531090` (.text), `0x0054875c` (.data), `0x0054f7b0` (.data), `0x0054f7b4` (.data), `0x00a845f8` (.data) |
| `0x5310d0` | `Particle_EmitParticles_4` | `0x0054f7b0` (.data) |
| `0x5310e0` | `FreeCamera_Init` | `0x00531130` (.text), `0x0054f7d0` (.data), `0x0054f7d4` (.data) |
| `0x531190` | `SpriteParticleSystemDynCubeObj_Init` | `0x005310d0` (.text), `0x0054f7e0` (.data), `0x00a84608` (.data), `0x00a84610` (.data) |
| `0x5311e0` | `SpriteParticleSystemDynCubeObj_OnEvent` | `0x00531540` (.text), `0x005323c0` (.text), `0x007205b0` (.data), `0x007205b8` (.data), `0x007205bc` (.data), `0x00721560` (.data), `0x007234b8` (.data), `0x00723504` (.data), `0x00723508` (.data), `0x0072350c` (.data), `0x00724458` (.data), `0x007ac09c` (.data), `0x007addd4` (.data), `0x00b8170c` (.data), `0x00b817a0` (.data), `0x00bcad7c` (.data), `0x00bcadac` (.data), `0x00bcade8` (.data) |
| `0x531300` | `PointlightDynCubeObj_Init` | `0x00531200` (.text), `0x00531220` (.text), `0x005313f0` (.text), `0x00531470` (.text), `0x00531480` (.text), `0x0054875c` (.data), `0x0054f800` (.data), `0x0054f802` (.data), `0x0054f804` (.data), `0x0054f818` (.data), `0x00afa800` (.data), `0x00afa820` (.data) |
| `0x531540` | `Particle_UpdateParticlePhysics_4` | `0x00531300` (.text), `0x005317d0` (.text), `0x00531d30` (.text), `0x00531ef0` (.text), `0x007205b0` (.data), `0x007205b8` (.data), `0x007205bc` (.data), `0x00721560` (.data), `0x007234b8` (.data), `0x00723504` (.data), `0x00723508` (.data), `0x0072350c` (.data), `0x00724458` (.data), `0x007ac09c` (.data), `0x007addd4` (.data), `0x00b8170c` (.data), `0x00b817a0` (.data), `0x00bcad7c` (.data), `0x00bcadac` (.data), `0x00bcade8` (.data) |
| `0x5317d0` | `BoneMagicObj_Init` | `0x00531620` (.text), `0x005316f0` (.text), `0x005317b0` (.text), `0x005318d0` (.text), `0x005319b0` (.text), `0x00531ca0` (.text), `0x00531cd0` (.text), `0x0054f848` (.data), `0x0054f84c` (.data), `0x0054f864` (.data) |
| `0x531d30` | `Particle_FadeParticles_4` | `0x00531db0` (.text), `0x00531fa0` (.text) |
| `0x531db0` | `Particle_EmitParticles_5` | `0x00531dd0` (.text), `0x00a5e408` (.data), `0x00b019e0` (.data) |
| `0x531dd0` | `Particle_Render3DSprites_5` | `0x00b041e0` (.data) |
| `0x531ef0` | `Sprite3D_Init` | `0x00531e10` (.text), `0x0054875c` (.data), `0x0054f874` (.data) |
| `0x531fa0` | `TransparencyList_Init` | `0x00531f60` (.text), `0x00532030` (.text), `0x00532100` (.text), `0x00532140` (.text), `0x0054875c` (.data), `0x0054f884` (.data), `0x0054f888` (.data), `0x0054f8a8` (.data), `0x0054f8c4` (.data) |
| `0x5323c0` | `Particle_UpdateParticlePhysics_6` | `0x005323e0` (.text) |
| `0x5323e0` | `Particle_TriggerMuzzleFlash_6` | `0x00532400` (.text), `0x00538370` (.data), `0x00538378` (.data), `0x00547340` (.data), `0x0054734c` (.data), `0x00547364` (.data), `0x00547380` (.data), `0x0054739c` (.data), `0x005473bc` (.data), `0x005473c0` (.data), `0x00a43e60` (.data), `0x00a43e64` (.data), `0x00a43e68` (.data), `0x00a43e6c` (.data), `0x00a43e70` (.data), `0x00a43e74` (.data), `0x00a9659c` (.data) |
| `0x532400` | `Particle_FadeParticles_6` | `0x005c8e48` (.data) |

422 functions carry image/BSS data references; the rest are pure control-flow/call shells. Registration-relevant tables: handler-id globals `0x0054E83C` (AIGraph id), slot flag/name table `0x00AF5EE0`-`0x00AF5F0C`, handler table `0x00A96AE0`, AI defaults `0x00A7B2A8`-`0x00A7B2CC`.

## 4. Engine implementation state per function

54 of 537 tree functions already have an implementation file in `engine/` (provenance line naming the exact retail VA). All others are absent, and their callees are overwhelmingly generic `TaskType_Register`/`QTask_*`/`Script_*` natives plus per-class `*_Init` registration shells.

| VA | BN alias | engine file | provenance status |
|---|---|---|---|
| `0x4010d0` | `QTask_Init` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x4011b0` | `QTask_Update` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x4011d0` | `QTask_OnEvent` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x401400` | `QTask_RemoveEventCallback` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x401530` | `QTask_LinkQTaskEventHandlerNoContextFn` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x4017c0` | `QTask_UnableToAllocateEven` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x401810` | `QTask_NewQTaskEventNoContextFn` | `engine/src/retail/core/native_401810.cpp` | TRANSCRIBED; QTask_AllocateEvent (0x004017C0) has the same table scan and is the reference for the scan bounds; runtime UNOBSERVED. |
| `0x4018f0` | `QTask_OpenEvent_2` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x401900` | `TaskType_Register` | `engine/src/retail/core/task_types.cpp` | TRANSCRIBED (body); the VA extent and tail behavior are the retail's. 32-bit |
| `0x48f090` | `FX_TrailUpdate_21` | `engine/src/retail/input/native_48f090.cpp` | TRANSCRIBED; reads the existing port global InstanceHandle (app_startup_natives.hpp), which transcribed StoreInstanceHandle 0x48F0D0 writes. Runtime UNOBSERVED. |
| `0x48f0a0` | `AppContext_GethWnd` | `engine/src/retail/input/native_48f0a0.cpp` | TRANSCRIBED; reads the existing port global g_window_handle (native_0048f0e0.hpp), which transcribed SetWindowHandle 0x48F0E0 writes. Runtime UNOBSERVED. |
| `0x48f2b0` | `FX_FireBallUpdate_26` | `engine/src/retail/core/app_startup_globals.cpp` | STRUCTURALLY_VERIFIED; retail full-game runtime observed (2 calls, values 1 then 0); candidate runtime unobserved until the retail main/handler order is wired |
| `0x48fd90` | `FX_CouldNotGetMouseCaps` | `engine/src/retail/input/native_48fd90.cpp` | TRANSCRIBED; only callee Debug_Error is an engine native; the device object is runtime data (0x005C8E1C). Return leaves the caps byte in eax (callers ignore it) |
| `0x490340` | `Display_SyncVSync` | `engine/src/retail/render/native_490340.cpp` | TRANSCRIBED; the byte is .data zero-fill, runtime data (no callees). Runtime UNOBSERVED. |
| `0x490350` | `Display_SwapBuffers` | `engine/src/retail/render/native_490350.cpp` | TRANSCRIBED; byte is .data zero-fill runtime data. Runtime UNOBSERVED. |
| `0x4a53b3` | `sprintf` | `engine/src/retail/error/error_show.cpp` | STRUCTURALLY_VERIFIED; high for exact unbounded CRT vsprintf semantics and NUL termination. |
| `0x4a5702` | `String_VFormat` | `engine/src/retail/error/error_show.cpp` | STRUCTURALLY_VERIFIED; high for exact unbounded CRT vsprintf semantics and NUL termination. |
| `0x4aa670` | `Mem_AllocChunk` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4aa682` | `Mem_AllocWithRetry` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4aa6ae` | `Mem_AllocateRawBlock` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4ad722` | `Mem_InvokeNewHandler` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4ad73d` | `String_ToUpperChar` | `engine/src/retail/sys/resource_lookup.cpp` | VERIFIED for the C-locale branch only (locale id 0; all |
| `0x4ae730` | `Camera_SetFieldOfView_30` | `engine/src/retail/input/native_4ae730.cpp` | TRANSCRIBED; the IAT call is part of the retail native's flow (no stub). Runtime UNOBSERVED. |
| `0x4af740` | `Camera_ApplyBandFilter_30` | `engine/src/retail/input/native_4af740.cpp` | TRANSCRIBED; the IAT call is part of the retail native's flow (no stub, no reimplementation). Runtime UNOBSERVED. |
| `0x4af7b0` | `Debug_Error` | `engine/src/retail/core/retail_error.cpp` | STRUCTURALLY_VERIFIED; compile-checked with MinGW x86, no runtime evidence. The candidate does not install g_936268/g_93626C, so its current runtime path is sil |
| `0x4af8f0` | `HeapList_Insert` | `engine/src/retail/mem/heap_list.cpp` | VERIFIED; high for exact pointer splice sequence. |
| `0x4af960` | `HeapList_Remove` | `engine/src/retail/mem/heap_list.cpp` | VERIFIED; high for exact pointer splice sequence. |
| `0x4b0ba0` | `Mem_InitHeapState` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4b0c60` | `Mem_AllocFn` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4b0d40` | `QHash_ValueSet` | `engine/src/retail/mem/mem_alloc.cpp` | VERIFIED; high for single byte store. |
| `0x4b26b0` | `Texture_DisableUpload` | `engine/src/retail/texture/texture_loaders.cpp` | STRUCTURALLY_VERIFIED; high for exact global getter semantics. |
| `0x4b59f0` | `Resource_CreateNode` | `engine/src/retail/resource/sys_resource.cpp` | VERIFIED; high for exact bounded prefix comparison (`strncmp`-equivalent). |
| `0x4b5af0` | `Resource_CanonicalizePath` | `engine/src/retail/sys/resource_lookup.cpp` | VERIFIED for the C-locale branch only (locale id 0; all |
| `0x4b5b30` | `Resource_CalculatePathHash` | `engine/src/retail/sys/resource_lookup.cpp` | VERIFIED for the C-locale branch only (locale id 0; all |
| `0x4bd3d0` | `Qvm_SetOpcodeHandler` | `engine/src/retail/script/qvm_stack_natives.cpp` | STRUCTURALLY_VERIFIED; high for this leaf's writes; caller state allocation and complete VM integration are not ported. |
| `0x4c1800` | `QTaskList_New` | `engine/src/retail/core/qtask_list.cpp` | STRUCTURALLY_VERIFIED; CFG 1.000 (BN 1 block = r2 1 block) and angr exact entry; behavioural differential and paired runtime are pending |
| `0x4cea80` | `Render_GetActiveBackendId` | `engine/src/retail/render/backend.cpp` | STRUCTURALLY_VERIFIED; see verification record for scope and port limits. |
| `0x4f0f80` | `Script_ParseEnumInt32_3` | `engine/src/retail/render/native_4f0f80.cpp` | TRANSCRIBED; the address is runtime-written and in the .data zero-fill tail (no file bytes), so the port global starts at 0 and only the untranscribed writer 0x |
| `0x4f1000` | `Script_ParseEnumInt32_4` | `engine/src/retail/render/native_4f1000.cpp` | TRANSCRIBED; runtime-written address in the .data zero-fill tail (no file bytes), port global starts at 0; only 0x4EE970 can change it. Runtime UNOBSERVED. |
| `0x4f1a60` | `Script_ParseEnumInt32_8` | `engine/src/retail/script/native_4f1a60.cpp` | TRANSCRIBED; image byte 0xFF (file offset 0x14E0E4); runtime-written, only the untranscribed writers change it. ABI note: the port returns uint8_t (zero-extends |
| `0x4f1a70` | `Script_ParseEnumReal32_8` | `engine/src/retail/script/native_4f1a70.cpp` | TRANSCRIBED; image byte 0xFF (file offset 0x14E0E5); runtime-written, only the untranscribed writers change it. ABI note as 0x4F1A60. Runtime UNOBSERVED. |
| `0x4f1ee0` | `Script_FormatString_10` | `engine/src/retail/core/native_4f1ee0.cpp` | TRANSCRIBED; callees 0x4017C0/0x401810 are transcribed. Global block is .data zero-fill (init 0). Runtime UNOBSERVED. |
| `0x4f9200` | `Script_EvaluateCondition_21` | `engine/src/retail/core/native_4f9200.cpp` | TRANSCRIBED; image init 0U; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX bits |
| `0x4f9210` | `Script_FormatString_21` | `engine/src/retail/core/native_4f9210.cpp` | TRANSCRIBED; image init 0x0181U; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX |
| `0x4f9220` | `Script_SpawnNewTask_22` | `engine/src/retail/core/native_4f9220.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x4f9230` | `Script_ParseEnumInt32_22` | `engine/src/retail/core/native_4f9230.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x4f9240` | `Script_ParseEnumReal32_22` | `engine/src/retail/core/native_4f9240.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x4f9250` | `Script_ParseEnumString_22` | `engine/src/retail/core/native_4f9250.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x4f9260` | `Script_ExecuteLevelScript_22` | `engine/src/retail/core/native_4f9260.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x4f9280` | `Script_EvaluateCondition_22` | `engine/src/retail/core/native_4f9280.cpp` | TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX b |
| `0x507e70` | `Physics_SimulateRigidBody_6` | `engine/src/retail/core/native_507e70.cpp` | TRANSCRIBED; only callee 0x4017C0 is transcribed (task_types.cpp). Runtime UNOBSERVED (position 4 runs inside registration, after the boundary). |
| `0x509cc0` | `Physics_CalculateSplineLength_10` | `engine/src/retail/input/native_509cc0.cpp` | TRANSCRIBED; only an indirect COM call on runtime device data - no native dependencies. Runtime UNOBSERVED. |
| `0x509f00` | `Physics_SimulateRigidBody_10` | `engine/src/retail/input/native_509f00.cpp` | TRANSCRIBED; no callees, no native dependencies (runtime data block). Runtime UNOBSERVED. |
| `0x530220` | `CamTask_Init` | `engine/src/retail/core/cam_task.cpp` | TRANSCRIBED; no runtime evidence; the handler set is empty in retail, this body is only the registration. |

Absent (sample of registration-carrying callers; full absence is visible in the section 3 table as `absent`): `0x00508F80`, `0x005183D0`, `0x00530270`, `0x005309C0`, `0x00530220`(CamTask_Init), `0x004FCF50`, `0x004ED510`, `0x00508D70`, `0x00504E10`, `0x00505630`, `0x0052CA70`, `0x0052CA30`, `0x004EE970`, `0x004F1EE0`, `0x00507E70`, `0x00530330`+... (see table).

## 5. Ordered transcription plan (minimum to let position 1 run without throwing)

The runtime throw is a port-side `ReportNotTranscribed("AI graph")` stub, not retail behavior: retail position 1 is `0x00508F80`, so the first real work is that thunk plus what it calls. Ordered by first execution:

- `0x508F80` - position-1 thunk (9 B, 2 instructions); no arguments, no returns used.
- `0x5183D0` - first callee; flat 74-call chain. Transcribe the call sequence verbatim, in order - it is the spine that makes ids 1..78.
- `0x530270` - first entry of that chain (CamTask_Process): 7 calls + tail-jump; no data refs.
- `0x5309C0` - next chain entry; 18 init calls + tail-jump (registers 4 ids through them).
- `0x4010D0` - QTask_Init: registers id 0 ("QTask",0x20,0x181,0) and initialises the handler table. Needed before any id comparison.
- `0x401900` - TaskType_Register: landed already; must be executed by the real dispatch instead of the port table for ids to be retail-derived.
- `0x401400` - QTask_RemoveEventCallback (handler link/clear): called by AIGraph_Init; landed.
- `0x401530` - QTask_LinkQTaskEventHandlerNoContextFn: called by AIGraph_Init; landed.
- `0x508FA0` - AIGraph_Init: 1 registration + 11 link calls + 0xA7B2xx defaults; last step of position 1.

Blocked before transcription can be claimed complete: the per-class registration callers in the 74-call chain (all `*_Init` shells) have no engine implementation and their only known contract is `TaskType_Register(name,size,parent,arg)`; each is a separate native transcription. The registry order itself is the 156-call sequence at `0x402930` (section 1), so position 1 needs only the 9 functions above to *execute*; the remaining 528 functions in the tree are its callees and are needed only for **complete** coverage, not for the registrar to stop throwing.

## 6. Contradictions, uncertainty and exact captures needed

| # | finding | status | capture needed to settle |
|---|---|---|---|
| 1 | The request said position 1 is `0x530270`; retail says position 1 is `0x00508F80` at call site `0x00402930`. `0x530270` is a grand-callee (`0x508F80 -> 0x5183D0 -> 0x530270`). | **Settled from retail bytes** (`e8 4b 66 10 00` at 0x402930). | none |
| 2 | `0x508F80` is a 9-byte thunk and `0x508FA0` is AIGraph_Init (1 registration). Neither registers 44 classes. The 44-class claim can only describe a prefix of the subtree. | static: contradicted by 127 unconditional registration sites | Frida hook on `0x00401900`: log (return id, caller return address, name, size, parent) for the first 300 calls after `0x402930` entry; compare with `docs/fidelity/evidence/0x00401900-retail-registration-order.json` (which has QTask=0, Game=124, Flow=125) |
| 3 | `QTask_RegisterClasses` extent is inconsistent between sources: engine comment `0x00402930..0x00402A40` (272 B), BN highest address `0x00402C3B` (779 B, 156 call sites), r2 linear sweep continues past `0x0040317B` with no `ret`. The number of registrars depends on the real extent. | **open** | r2/BN boundary check: `af` at `0x402C3B..0x40317B`, verify a `ret`/`push ebp` boundary; alternatively count retail registrations (279 types per engine capture) against the number of `0x401900` sites reachable from `0x402930` under each boundary hypothesis |
| 4 | `0x5183D0` boundary: BN end `0x00518541` (74 calls); r2 linear sweep shows further calls at `0x518541..0x518578` before the next function. | **open** | r2 `af`/`afij` at `0x00518541` and `0x00518578`; verify which address the next `push ebp` starts |
| 5 | BN function aliases are unreliable across this subtree (`Physics_ApplyImpulse_7` for `0x508F80`, `Terrain_CalculateNormals_8` for `0x5183D0`, `CamTask_Process` for `0x530270`, `Particle_TriggerMuzzleFlash_3` for `0x5309C0`). Inventory therefore reports VAs and BN names as labels only, never as semantics. | known tool limitation | none; use r2 bytes/decompiled operands for semantics |
| 6 | `0x401900` third argument is not a constant for `AIGraph_Init` (it is `Script_FormatString_21()`), so the recorded engine signature `(name,size,parent,arg)` fits `QTask_Init` but not all call sites; the parameter may be a per-class string table rather than the parent type. | **open** | decompile 3 more call sites with distinct argument shapes (e.g. `0x530220` CamTask_Init, `0x504E10` MenuItemTask_Init) and/or runtime-print arg3 |

## 7. Artifacts

All under this directory; `artifact-manifest.json` has per-file SHA-256. BN decompiles: `bn-dec-*.c`; r2 linear disassembly: `r2-dis-*.txt`; full graph data: `position1-tree.json` (BN info, depth, filtered r2 CFG, engine mapping), `callsum.json` (root call lists). Retail SHA-256 and tool versions are repeated in every file header.
