# Comprehensive Comparison: IGINatives.json vs scripts/GameNatives

**Canonical File**: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/IGINatives.json`

**GameNatives Directory**: `/Users/haseeb-mir/Documents/Code/project-open-igi/scripts/GameNatives`

## 1. Executive Summary

| Metric | Count | Details |
| :--- | :--- | :--- |
| **Canonical `IGINatives.json` Total** | 466 | 301 original + 165 IGIPatch additions (`0x00403C10` dual registered) |
| **`scripts/GameNatives` Total Functions** | 5734 | Total reconstructed/decompiled functions across 17,081 .cpp/.hpp files |
| **Addresses in Both** | 346 | Common address overlap between both sets |
| **Original 302 Overlap** | **301 / 301 (100%)** | Every single original canonical native exists in `scripts/GameNatives` |
| **Original 302 Name Match** | **300 / 301 (99.7%)** | Verbatim matching names between catalog and `GameNatives` files |
| **IGIPatch Additions Overlap** | 45 | Function entry points called/hooked by IGIPatch present in `GameNatives` |
| **Missing from `scripts/GameNatives`** | 120 | Mid-function hook offsets / instruction patch sites from IGIPatch |
| **Functions Only in `GameNatives`** | 5,388 | Full-engine decompiled internal routines (terrain, math, physics, etc.) |

## 2. Original 302 Canonical Natives vs scripts/GameNatives

All **301 unique addresses** from the baseline canonical catalog (`IGINatives.json`) exist in `scripts/GameNatives`.

- **Exact Name Matches**: **300**
- **Naming Divergence**: **1** (`0x00403C10`)
  - In `IGINatives.json`: `Config_SetContentControlPassword` and `GO_ContentControlPW` (both registered by retail `igi.exe` to the same address `0x00403C10`).
  - In `scripts/GameNatives`: `00403C10-GO_ContentControlPW.cpp` in `scripts/GameNatives/go/`.

## 3. The 45 Function Matches from IGIPatch Additions

The following 45 native functions added from IGIPatch match exact function entries in `scripts/GameNatives`:

| Address | `IGINatives.json` Name (from IGIPatch) | `scripts/GameNatives` Name | Subsystem Folder |
| :--- | :--- | :--- | :--- |
| `0x00401530` | `QTask_LinkQTaskEventHandlerNoContextFn` | `QTask_TriggerEvent` | `qtask` |
| `0x00401810` | `QTask_NewQTaskEventNoContextFn` | `QTask_UnableToAllocateEven_2` | `qtask` |
| `0x004018E0` | `QTask_DeleteQTaskEventNoContext` | `QTask_CreateTask_2` | `qtask` |
| `0x00401A20` | `QTask_DeleteQTaskType` | `QTask_CloseEvent_2` | `qtask` |
| `0x00402260` | `Flow_RunHandler` | `Flow_LoadParameters` | `flow` |
| `0x004027E0` | `Flow_Close` | `Flow_InitEvents` | `flow` |
| `0x00405980` | `Config_VerifyGraphicConfig` | `Config_NoD3DDriversWithHard` | `config` |
| `0x00414FD0` | `GameFunctions_IsDebugKeyPressed` | `WeaponType_GetConfig_2` | `weapontype` |
| `0x00416D20` | `Game_GetRandomState` | `Level_SetMission_5` | `level` |
| `0x004199D0` | `BackgroundFX_New` | `Menu_UpdateLanguage_6` | `menu` |
| `0x0041F2D0` | `ListBox_AddItem` | `Menu_DrawWidget_9` | `menu` |
| `0x00424CE0` | `Cursor_RunHandler` | `Cursor_LoadParameters` | `cursor` |
| `0x0045EEA0` | `Human_RunHandler` | `HumanAI_InvokeFindType` | `humanai` |
| `0x00460C80` | `Human_printf` | `AI_CheckHearing_18` | `ai` |
| `0x00464470` | `HumanView_DeleteHandler` | `HumanView_LoadParameters` | `humanview` |
| `0x00464800` | `HumanView_UpdateView` | `AI_UpdatePatrol_23` | `ai` |
| `0x00464BD0` | `HumanView_UpdateBody` | `AI_CombatTarget_23` | `ai` |
| `0x004675B0` | `ComputerObject_ProjectRotatedPos` | `AI_UpdateSensors_24` | `ai` |
| `0x00467620` | `ComputerObject_ProjectWorldPos` | `AI_CheckSector_24` | `ai` |
| `0x00471440` | `BinocularOverlay_StatusDisplayHandler` | `BinocularOverlay_Process` | `binocularoverlay` |
| `0x004782D0` | `GunCasing_RunHandler` | `WeaponGun_ProcessStep_4782D0` | `weapongun` |
| `0x0048F0A0` | `AppContext_GethWnd` | `FX_ExplosionSpawn_22` | `fx` |
| `0x0048FC20` | `Mouse_Update` | `FX_FailedGetMouseStateS` | `fx` |
| `0x0048FED0` | `Mouse_WinProcCB` | `Input_InitKeyboard` | `input` |
| `0x00490230` | `Keyboard_Update` | `Display_CloseDevice` | `display` |
| `0x00490360` | `Timer_Open` | `Display_ClearSurfaces` | `display` |
| `0x00490370` | `Timer_Read` | `Display_RestoreDevice` | `display` |
| `0x004B0D10` | `Mem_DeAllocFn` | `Sys_ExecuteBytecode_2` | `sys` |
| `0x004B0F90` | `RenderMode_PopRenderMode` | `Sys_CloseFileStream_3` | `sys` |
| `0x004B3320` | `Real64x3_NullVector` | `Sys_PopStack_11` | `sys` |
| `0x004B38E0` | `Real32x9_Create` | `Sys_ReadFileBlock_12` | `sys` |
| `0x004B6E60` | `Picture_Register` | `Sys_AllocateMemory_18` | `sys` |
| `0x004B8A20` | `Script_EvalArgInt` | `Script_GetArg` | `script` |
| `0x004C1790` | `QTaskList_Register` | `DynObj_QtasklistIsFull` | `dynobj` |
| `0x004C17C0` | `QTaskList_Remove` | `DynObj_TryingToRemoveUnrigi` | `dynobj` |
| `0x004C1800` | `QTaskList_New` | `DynObj_LookupHashItem_4` | `dynobj` |
| `0x004C1830` | `QTaskList_Delete` | `DynObj_CreateMagicObject_4` | `dynobj` |
| `0x004D3210` | `AnimController_UpdateChannels` | `Render_ClipMesh_5` | `render` |
| `0x004D9610` | `BoneDynCubeObj_GetQTaskType` | `Render_DrawRigidMesh_12` | `render` |
| `0x004E6030` | `SoundSys_SetFrequency` | `SoundSys_SetMasterVolume_4` | `soundsys` |
| `0x004E76E0` | `TextWindow_vprintf` | `SoundSys_Update3DPosition_10` | `soundsys` |
| `0x004E7B80` | `DebugText_SetCursor` | `SoundSys_InitSoundSys_11` | `soundsys` |
| `0x004E7BB0` | `DebugText_Clear` | `SoundSys_PlaySoundSample_11` | `soundsys` |
| `0x00507EC0` | `Input_Run` | `Physics_UpdateForces_6` | `physics` |
| `0x00509CF0` | `Joypad_Update` | `Physics_InterpolateWaypoint_10` | `physics` |

> **Key Observation**: In `scripts/GameNatives`, many functions carried automated or generic decompilation names (e.g., `QTask_TriggerEvent`, `AI_UpdatePatrol_23`, `SoundSys_Update3DPosition_10`, `FX_FailedGetMouseStateS`). IGIPatch's retail comments and calling conventions reveal their true human/engine semantics (`QTask_LinkQTaskEventHandlerNoContextFn`, `HumanView_UpdateView`, `TextWindow_vprintf`, `Mouse_Update`).

## 4. Why 120 Addresses Are in IGINatives.json but Not scripts/GameNatives

`scripts/GameNatives` is structured strictly by **Function Entry Points** (each `.cpp`/`.hpp` file begins at a function prologue, usually 16-byte aligned `0x...0`).

The 120 missing addresses are **Internal Instruction Hook Sites** and **Patch Targets** identified in `IGIPatch` assembly fixups (e.g. `callpatch`, `hook`), such as:

- `0x00464C28`: `HumanView_UpdateBody` + 0x58 (fix pitch speed during interpolation)
- `0x00464D72`: `HumanView_UpdateBody` + 0x1A2 (fix shake duration during interpolation)
- `0x00464E1B`: `HumanView_UpdateBody` + 0x24B (fix landing impact speed during interpolation)
- `0x0048F53F`: `WinMain` + 0x5F (allow multi-instance patch point)
- `0x0048F674`: `WinMain` + 0x194 (command-line argument parsing point)
- `0x004942D8`: `Direct3D_CreateDevice` + 0x98 (windowed cursor hide hook)
- `0x0048A452`: `LoadingScreen_New` + 0x32 (loading screen aspect scaling hook)

In `scripts/GameNatives`, these instructions are contained **inside** the decompiled body of the parent functions, rather than having individual `.cpp` files named after their offset.

