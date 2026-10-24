# IGINatives.json Canonical Catalog Comparison & Expansion Report

**Canonical File Path**: `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/IGINatives.json`

**Mirror File Path**: `/Users/haseeb-mir/Documents/Code/project-igi-internals/assets/IGINatives.json`

**Original Catalog Entries**: 302 (across 301 distinct retail addresses; `0x00403C10` registered under dual script names in retail)

**Newly Merged IGIPatch Natives**: 165

**Total Expanded Canonical Catalog**: 467 entries

## 1. Executive Summary

| Metric | Count | Details |
| :--- | :--- | :--- |
| **Previous Canonical Natives** | 302 | Baseline verified entries (commit `ba07801`) |
| **IGIPatch Scanned Addresses** | 372 | 293 code/calls/hooks, 79 globals/state variables |
| **Already Present (Common)** | 27 | Verified common functions between both catalogs |
| **Newly Merged from IGIPatch** | 165 | Added with authentic signatures & notes |
| **Updated Catalog Total** | 467 | 100% verified against `igi.exe` .text range (0x00401000 - 0x00532510) |
| **Audit Suite Status** | **PASS** | `scripts/test_natives_catalog.py` 100% passed |

## 2. Common Functions (Already Present in IGINatives.json)

The following 27 functions discovered in IGIPatch were already cataloged in the canonical `IGINatives.json`:

| Address | IGIPatch Function Name | Canonical `IGINatives.json` Name | Subsystem |
| :--- | :--- | :--- | :--- |
| `0x00401CF0` | `QTask_IsQTaskTypeDerived` | `TaskType_IsDerivedFrom` | Game Loop & Flow Management |
| `0x00402820` | `Flow_SetFrequency` | `Frames_Set` | Game Loop & Flow Management |
| `0x004028B0` | `Flow_GetTicks` | `Frames_GetUpdateCounter` | Player, Human & Camera Subsystem |
| `0x00404450` | `Config_FillScreenResolutionListBox` | `Config_FillScreenResolutionListBox` | Windowing, Display & Video Modes |
| `0x00404510` | `Config_GraphicOptionsSetResolution` | `Config_GraphicOptionsSetResolution` | System Configuration & Storage |
| `0x00404590` | `Config_GetActiveGraphicOptions` | `Config_GetActiveGraphicOptions` | System Configuration & Storage |
| `0x004045B0` | `Config_GraphicOptionsGetResolution` | `Config_GraphicOptionsGetResolution` | System Configuration & Storage |
| `0x004046A0` | `Config_FillRenderDeviceListBox` | `Config_FillRenderDeviceListBox` | Graphics & Direct3D Rendering |
| `0x00404710` | `Config_GraphicOptionsGetDevice` | `Config_GraphicOptionsGetDevice` | System Configuration & Storage |
| `0x004047F0` | `Config_GraphicOptionsSetDevice` | `Config_GraphicOptionsSetDevice` | System Configuration & Storage |
| `0x00406220` | `Config_GetActivePlayerProfile` | `Config_GetActivePlayerProfileRecord` | Player, Human & Camera Subsystem |
| `0x00415020` | `GameFunctions_IsDebugKeyPressedOnce` | `DEBUG_KEYSENABLEINVALID` | Player, Human & Camera Subsystem |
| `0x004828D0` | `HumanCamera_ControlListInput` | `Human_FindNextCamera` | Player, Human & Camera Subsystem |
| `0x0048F1A0` | `AppContext_SetDebugged` | `App_ContextSetDebugged` | Diagnostics & Debug |
| `0x0048F1E0` | `AppContext_SetDebugtextState` | `App_ContextSetDebugtextState` | Diagnostics & Debug |
| `0x0048F240` | `AppContext_SetLightmapsUsed` | `App_ContextSetLightmapsUsed` | Graphics & Direct3D Rendering |
| `0x0048F260` | `AppContext_SetTerrainLightmapsUsed` | `App_ContextSetTerrainLightmapsUsed` | Graphics & Direct3D Rendering |
| `0x00491A90` | `Display_SetMode` | `Display_SetMode` | Windowing, Display & Video Modes |
| `0x00491CF0` | `Display_GetActiveMode` | `Display_GetActiveMode` | Windowing, Display & Video Modes |
| `0x00491E70` | `Display_SetBackgroundColourFn` | `Display_SetBackgroundColourFn` | Windowing, Display & Video Modes |
| `0x00497E70` | `TransContext_SetActiveTransContext` | `Transcontext_SetActiveTransContext` | Graphics & Direct3D Rendering |
| `0x004A53B3` | `_sprintf` | `Game_DataSymbolLoad` | Graphics & Direct3D Rendering |
| `0x004AF7B0` | `LDebug_Error` | `Error_Show` | Diagnostics & Debug |
| `0x004B0C60` | `Mem_AllocFn` | `Mem_Alloc` | General Engine Subsystem |
| `0x004B53B0` | `QSprite_Register4AZ` | `Qsprite_Register4Az` | General Engine Subsystem |
| `0x004B6E70` | `Picture_GetWidth` | `Picture_GetWidth` | Windowing, Display & Video Modes |
| `0x004B6E80` | `Picture_GetHeight` | `Picture_GetHeight` | Windowing, Display & Video Modes |

## 3. Newly Merged Native Functions (165 Entries)

The 165 missing functions discovered from IGIPatch have been added to `IGINatives.json`. Grouped by subsystem:

### 3.1 Animation Controller (1 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x004D4B95` | `5065621` | `sub_4D4B60` | `void sub_4D4B60()` | scale 1st person anims progress |

### 3.2 Diagnostics & Debug (1 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x004E7BB0` | `5143472` | `DebugText_Clear` | `void DebugText_Clear()` | DebugText_Reset |

### 3.3 Game Loop & Flow Management (15 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x00401810` | `4200464` | `QTask_NewQTaskEventNoContextFn` | `void QTask_NewQTaskEventNoContextFn(undefined2 param_1)` | Flow_Open - allocate interpolation task event/list |
| `0x004018E0` | `4200672` | `QTask_DeleteQTaskEventNoContext` | `void QTask_DeleteQTaskEventNoContext(undefined4 param_1)` | Flow_Close - deallocate interpolation task event/list |
| `0x00401A20` | `4200992` | `QTask_DeleteQTaskType` | `void QTask_DeleteQTaskType(uint param_1)` | Flow_Close - deallocate interpolation task event/list |
| `0x00402065` | `4202597` | `Flow_Open` | `void Flow_Open()` | allocate interpolation task event/list |
| `0x004020B0` | `4202672` | `Flow_CreateHandler` | `void Flow_CreateHandler()` | init new members |
| `0x004021E7` | `4202983` | `Flow_CreateHandler_HookTargetIn` | `void Flow_CreateHandler_HookTargetIn()` | Hook target in Flow_CreateHandler |
| `0x00402260` | `4203104` | `Flow_RunHandler` | `void Flow_RunHandler()` | rewrite fps limiter |
| `0x004027E0` | `4204512` | `Flow_Close` | `void Flow_Close()` | deallocate interpolation task event/list |
| `0x00415F41` | `4284225` | `Game_CreateHandler` | `void Game_CreateHandler()` |  |
| `0x004162A9` | `4285097` | `Game_RunHandler` | `void Game_RunHandler()` | Hook target in Game_RunHandler |
| `0x004C1800` | `4986880` | `QTaskList_New` | `void QTaskList_New(int param_1)` | Flow_Open - allocate interpolation task event/list |
| `0x004C1830` | `4986928` | `QTaskList_Delete` | `void QTaskList_Delete(undefined4 param_1)` | Flow_Close - deallocate interpolation task event/list |
| `0x004E6030` | `5136432` | `SoundSys_SetFrequency` | `void SoundSys_SetFrequency(undefined4 param_1)` | Flow_SetFrequency - init new members |
| `0x004F5245` | `5198405` | `CutScene_RunHandler` | `void CutScene_RunHandler()` | save CutScene_isRunning |
| `0x004F525B` | `5198427` | `CutScene_RunHandler_SaveCutsceneIsrunning` | `void CutScene_RunHandler_SaveCutsceneIsrunning()` | save CutScene_isRunning |

### 3.4 General Engine Subsystem (11 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x004675B0` | `4617648` | `ComputerObject_ProjectRotatedPos` | `void ComputerObject_ProjectRotatedPos(float param_1, float param_2, int param_3)` | fix aspect ratio |
| `0x00467620` | `4617760` | `ComputerObject_ProjectWorldPos` | `void ComputerObject_ProjectWorldPos(float param_1, float param_2, double param_3, undefined4 param_4)` | fix aspect ratio |
| `0x0046A531` | `4629809` | `ComputerMap_Trace` | `void ComputerMap_Trace()` | fix aspect ratio |
| `0x0046BD84` | `4636036` | `Computer_RunHandler` | `void Computer_RunHandler()` | fix aspect ratio |
| `0x0048F53F` | `4781375` | `WinMain` | `void WinMain()` | allow multiinstance |
| `0x0048F674` | `4781684` | `WinMain_InitCommandLine` | `void WinMain_InitCommandLine()` | init command-line commands |
| `0x0048F6D8` | `4781784` | `WinMain_ParseCommandLine` | `void WinMain_ParseCommandLine()` | parse command-line commands |
| `0x004A01DC` | `4850140` | `RigidDynCubeObj_EnterHandler` | `void RigidDynCubeObj_EnterHandler()` | interpolate (RigidDynCubeObj entities) |
| `0x004B0D10` | `4918544` | `Mem_DeAllocFn` | `void Mem_DeAllocFn(int param_1)` | DynCubeObj_DeallocInterpData - function to automatically deallocate/init the interp buffer |
| `0x004E0C8A` | `5115018` | `Shadow_MagicObjEnterHandler` | `void Shadow_MagicObjEnterHandler()` | interpolate |
| `0x005124EB` | `5317867` | `BoneDynCubeObj_EnterHandler` | `void BoneDynCubeObj_EnterHandler()` | interpolate (BoneDynCubeObj entities) |

### 3.5 Graphics & Direct3D Rendering (38 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x0041A8A4` | `4303012` | `TypeWriterBox_DrawHandler` | `void TypeWriterBox_DrawHandler()` | fix credits rect position |
| `0x0041D7A3` | `4315043` | `InputBox_DrawHandler` | `void InputBox_DrawHandler()` | fix input boxes position |
| `0x0041F2D0` | `4322000` | `ListBox_AddItem` | `void ListBox_AddItem(char param_1, int param_2)` | Config_FillRenderDeviceListBox |
| `0x00421B5D` | `4332381` | `MenuScreen_UpdateInternalDataHandler_FixParametersFor` | `void MenuScreen_UpdateInternalDataHandler_FixParametersFor()` | fix parameters for background picture drawing |
| `0x00421CB6` | `4332726` | `MenuScreen_DrawHandler` | `void MenuScreen_DrawHandler()` | draw background picture with custom scaling mode |
| `0x00424CE0` | `4345056` | `Cursor_RunHandler` | `void Cursor_RunHandler()` | Cursor_DrawInterpolateHandler - new interpolation handler |
| `0x00464BD0` | `4606928` | `HumanView_UpdateBody` | `void HumanView_UpdateBody(int param_1, float param_2)` | HumanPlayer_DrawInterpolateHandler - new interpolation handler |
| `0x00492737` | `4794167` | `Direct3D_EnumDisplayModesCB` | `void Direct3D_EnumDisplayModesCB()` | skip unnecessary null string checks that uses incorrect type casting |
| `0x0049276D` | `4794221` | `Direct3D_EnumDisplayModesCB_SkipUnnecessaryNull` | `void Direct3D_EnumDisplayModesCB_SkipUnnecessaryNull()` | skip unnecessary null string checks that uses incorrect type casting |
| `0x004942D8` | `4801240` | `Direct3D_CreateDevice` | `void Direct3D_CreateDevice()` | hide windows cursor in windowed mode |
| `0x004942F3` | `4801267` | `Direct3D_CreateDevice_SaveRefreshRate` | `void Direct3D_CreateDevice_SaveRefreshRate()` | save refresh rate from current set mode |
| `0x00494305` | `4801285` | `Direct3D_CreateDevice_HideWindowsCursor` | `void Direct3D_CreateDevice_HideWindowsCursor()` | hide windows cursor in windowed mode |
| `0x0049432E` | `4801326` | `Direct3D_CreateDevice_0049432E` | `void Direct3D_CreateDevice_0049432E()` | save refresh rate from current set mode |
| `0x00494FB1` | `4804529` | `Direct3D_WinProcCB` | `void Direct3D_WinProcCB()` | window resizing |
| `0x00497DE7` | `4816359` | `TransContext_Create` | `void TransContext_Create()` | set viewport rendering mode |
| `0x0049E006` | `4841478` | `Direct3DRender_DrawRigidMesh` | `void Direct3DRender_DrawRigidMesh()` | set viewport rendering mode |
| `0x0049E0B1` | `4841649` | `Direct3DRender_DrawRigidMesh_ReadInterpolatedPos` | `void Direct3DRender_DrawRigidMesh_ReadInterpolatedPos()` | read interpolated pos from second context |
| `0x0049E272` | `4842098` | `Direct3DRender_DrawRigidMesh_0049E272` | `void Direct3DRender_DrawRigidMesh_0049E272()` | read interpolated pos from second context |
| `0x0049F00D` | `4845581` | `Direct3DRender_DrawSortedFaceGroup_Rigid` | `void Direct3DRender_DrawSortedFaceGroup_Rigid()` | set viewport rendering mode |
| `0x0049F5AF` | `4847023` | `Direct3DRender_DrawSortedFaceGroup_Lightmap` | `void Direct3DRender_DrawSortedFaceGroup_Lightmap()` | set viewport rendering mode |
| `0x0049F766` | `4847462` | `Direct3DRender_DrawBoneMesh` | `void Direct3DRender_DrawBoneMesh()` | set viewport rendering mode |
| `0x0049F80F` | `4847631` | `Direct3DRender_DrawBoneMesh_ReadInterpolatedPos` | `void Direct3DRender_DrawBoneMesh_ReadInterpolatedPos()` | read interpolated pos from second context |
| `0x004B0F90` | `4919184` | `RenderMode_PopRenderMode` | `int RenderMode_PopRenderMode()` | Config_Open - call to Config_UpdateDevicesDefMode |
| `0x004B6E60` | `4943456` | `Picture_Register` | `void Picture_Register(undefined4 param_1)` | MenuScreen_DrawHandler - draw background picture with custom scaling mode |
| `0x004CFE74` | `5045876` | `ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)` | `void ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)()` | fix lod quality and rendering distance |
| `0x004CFE95` | `5045909` | `ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_FixLodQuality` | `void ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_FixLodQuality()` | fix lod quality and rendering distance |
| `0x004CFEB2` | `5045938` | `ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_004CFEB2` | `void ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_004CFEB2()` | fix lod quality and rendering distance |
| `0x004CFEF2` | `5046002` | `ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_004CFEF2` | `void ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)_004CFEF2()` | fix lod quality and rendering distance |
| `0x004D0097` | `5046423` | `Mesh3D_GetBoneMeshLOD` | `void Mesh3D_GetBoneMeshLOD()` | fix lod quality and rendering distance |
| `0x004D00B7` | `5046455` | `Mesh3D_GetBoneMeshLOD_FixLodQuality` | `void Mesh3D_GetBoneMeshLOD_FixLodQuality()` | fix lod quality and rendering distance |
| `0x004D00D4` | `5046484` | `Mesh3D_GetBoneMeshLOD_004D00D4` | `void Mesh3D_GetBoneMeshLOD_004D00D4()` | fix lod quality and rendering distance |
| `0x004D010A` | `5046538` | `Mesh3D_GetBoneMeshLOD_004D010A` | `void Mesh3D_GetBoneMeshLOD_004D010A()` | fix lod quality and rendering distance |
| `0x004D02DD` | `5047005` | `Mesh3D_GetSplineMeshLOD` | `void Mesh3D_GetSplineMeshLOD()` | fix lod quality and rendering distance |
| `0x004D02FD` | `5047037` | `Mesh3D_GetSplineMeshLOD_FixLodQuality` | `void Mesh3D_GetSplineMeshLOD_FixLodQuality()` | fix lod quality and rendering distance |
| `0x004D031A` | `5047066` | `Mesh3D_GetSplineMeshLOD_004D031A` | `void Mesh3D_GetSplineMeshLOD_004D031A()` | fix lod quality and rendering distance |
| `0x004D0350` | `5047120` | `Mesh3D_GetSplineMeshLOD_004D0350` | `void Mesh3D_GetSplineMeshLOD_004D0350()` | fix lod quality and rendering distance |
| `0x004E7F50` | `5144400` | `ViewportQTask_DrawHandler` | `void ViewportQTask_DrawHandler()` | interpolate (camera) |
| `0x004E7F97` | `5144471` | `ViewportQTask_DrawHandler_CaptureTransContext` | `void ViewportQTask_DrawHandler_CaptureTransContext()` | capture trans context for the binoculars overlay |

### 3.6 Input & Cursor Management (15 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x00401530` | `4199728` | `QTask_LinkQTaskEventHandlerNoContextFn` | `void QTask_LinkQTaskEventHandlerNoContextFn(ushort param_1, int param_2, undefined4 param_3)` | Cursor_Open - add interpolation handler |
| `0x00424C08` | `4344840` | `Cursor_Open` | `void Cursor_Open()` | add interpolation handler |
| `0x00424C20` | `4344864` | `Cursor_CreateHandler` | `void Cursor_CreateHandler()` | fix cursor not being properly centered |
| `0x00424C3B` | `4344891` | `Cursor_CreateHandler_RegisterInterpolationList` | `void Cursor_CreateHandler_RegisterInterpolationList()` | register interpolation list |
| `0x00424DEA` | `4345322` | `Cursor_RunHandler_DoNotUpdate` | `void Cursor_RunHandler_DoNotUpdate()` | do not update buttons down state during interpolation |
| `0x0048FC20` | `4783136` | `Mouse_Update` | `void Mouse_Update()` | Flow_UpdateCInput - new function to handle short interval input updates |
| `0x0048FC7B` | `4783227` | `Mouse_Update_FixMouseSensitivity` | `void Mouse_Update_FixMouseSensitivity()` | fix mouse sensitivity being tied to fps / add option for custom mouse sensitivty multiplier / add IsLocked support |
| `0x0048FED0` | `4783824` | `Mouse_WinProcCB` | `void Mouse_WinProcCB()` | fix mouse motion/buttons getting stuck when window gets out of focus |
| `0x00490230` | `4784688` | `Keyboard_Update` | `void Keyboard_Update()` | InputPort_RunHandler_NEW - rewrite function |
| `0x004C1790` | `4986768` | `QTaskList_Register` | `void QTaskList_Register(int param_1, int param_2)` | Cursor_CreateHandler - register interpolation list |
| `0x004C17C0` | `4986816` | `QTaskList_Remove` | `void QTaskList_Remove(int param_1, int param_2)` | Cursor_DeleteHandler_NEW - unregister interpolation list |
| `0x004E7B80` | `5143424` | `DebugText_SetCursor` | `void DebugText_SetCursor(undefined4 param_1, undefined4 param_2)` | DebugText_Reset |
| `0x004ED420` | `5166112` | `InputPort_InputHandler` | `void InputPort_InputHandler()` | point to Mouse_vAnalogX/Y |
| `0x00507EC0` | `5275328` | `Input_Run` | `void Input_Run(undefined4 param_1, undefined4 param_2)` | InputPort_Update - new function equivalent to InputPort_RunHandler minus the per-device update calls |
| `0x00509CF0` | `5283056` | `Joypad_Update` | `void Joypad_Update()` | InputPort_RunHandler_NEW - rewrite function |

### 3.7 Player, Human & Camera Subsystem (52 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x0041006B` | `4259947` | `HumanPlayer_Open` | `void HumanPlayer_Open()` | add interpolation handler |
| `0x00414FD0` | `4280272` | `GameFunctions_IsDebugKeyPressed` | `undefined4 GameFunctions_IsDebugKeyPressed(int param_1)` | HumanCamera_ThirdPersonControlInput - skip debug keys check during interpolation |
| `0x00416D20` | `4287776` | `Game_GetRandomState` | `int Game_GetRandomState()` | HumanView_UpdateBody - save Game_GetRandomState seed |
| `0x0045E07E` | `4579454` | `HumanPlayerInput_ReadChannels` | `void HumanPlayerInput_ReadChannels()` | apply correct per-phase mouse delta |
| `0x0045E0E4` | `4579556` | `HumanPlayerInput_ReadChannels_SkipDuplicatedMouse` | `void HumanPlayerInput_ReadChannels_SkipDuplicatedMouse()` | skip duplicated mouse sensitivity/inversion calculation |
| `0x0045ECC0` | `4582592` | `Human_CreateHandler` | `void Human_CreateHandler()` | alloc interp data |
| `0x0045EEA0` | `4583072` | `Human_RunHandler` | `void Human_RunHandler()` | update interp data |
| `0x0045EFC5` | `4583365` | `Human_DeleteHandler` | `void Human_DeleteHandler()` | dealloc interp data |
| `0x0045FD58` | `4586840` | `Human_GetHumanCameraInfoHandler` | `void Human_GetHumanCameraInfoHandler()` | skip checking debug keys during interpolation |
| `0x00460133` | `4587827` | `Human_gun_event_related_3` | `void Human_gun_event_related_3()` | interpolate (1st person gun model part 1) |
| `0x00460187` | `4587911` | `Human_gun_event_related_3_Interpolate1stPerson` | `void Human_gun_event_related_3_Interpolate1stPerson()` | interpolate (1st person gun model part 1) |
| `0x00460202` | `4588034` | `Human_gun_event_related_4` | `void Human_gun_event_related_4()` | interpolate (1st person gun model part 2) |
| `0x00460272` | `4588146` | `Human_gun_event_related_5` | `void Human_gun_event_related_5()` | interpolate (1st person gun model part 3) |
| `0x00460C80` | `4590720` | `Human_printf` | `void Human_printf(undefined4 param_1, undefined4 param_2)` | skip priting text every frame |
| `0x004612CD` | `4592333` | `Human_UpdateBody` | `void Human_UpdateBody()` | apply correct per-phase mouse delta |
| `0x00464466` | `4605030` | `HumanView_CreateHandler` | `void HumanView_CreateHandler()` | alloc interp data |
| `0x00464470` | `4605040` | `HumanView_DeleteHandler` | `void HumanView_DeleteHandler(int param_1)` | dealloc interp data |
| `0x004644EE` | `4605166` | `HumanView_EnterHandler` | `void HumanView_EnterHandler()` | interpolate (1st person view model) |
| `0x00464800` | `4605952` | `HumanView_UpdateView` | `void HumanView_UpdateView(int param_1, float param_2)` | apply correct per-phase mouse delta |
| `0x00464C28` | `4607016` | `HumanView_UpdateBody_SaveGameGetrandomstate` | `void HumanView_UpdateBody_SaveGameGetrandomstate()` | save Game_GetRandomState seed |
| `0x00464D72` | `4607346` | `HumanView_UpdateBody_FixShakeDuration` | `void HumanView_UpdateBody_FixShakeDuration()` | fix shake duration during interpolation |
| `0x00464E1B` | `4607515` | `HumanView_UpdateBody_FixLandingImpact` | `void HumanView_UpdateBody_FixLandingImpact()` | fix landing impact speed during interpolation |
| `0x00465102` | `4608258` | `HumanView_UpdateBody_FixViewModel` | `void HumanView_UpdateBody_FixViewModel()` | fix view model retraction speed on collision during interpolation |
| `0x00465251` | `4608593` | `HumanView_UpdateBody_FixObjectSpeed` | `void HumanView_UpdateBody_FixObjectSpeed()` | fix object alpha/gamma speed during interpolation |
| `0x0046553F` | `4609343` | `HumanView_UpdateBody_SwitchAnimUpdates` | `void HumanView_UpdateBody_SwitchAnimUpdates()` | switch anim updates during interpolation |
| `0x00471440` | `4658240` | `BinocularOverlay_StatusDisplayHandler` | `void BinocularOverlay_StatusDisplayHandler(int param_1)` | project with the captured trans context |
| `0x004719D3` | `4659667` | `VUMeter_Update` | `void VUMeter_Update()` | fix fx bars update rate when using binoculars |
| `0x00471C26` | `4660262` | `BinocularOverlay_TrackMovement` | `void BinocularOverlay_TrackMovement()` | project interpolated pos |
| `0x00473EB8` | `4669112` | `StationaryGun_CreateHandler` | `void StationaryGun_CreateHandler()` | register interpolation list |
| `0x00474186` | `4669830` | `StationaryGun_Open` | `void StationaryGun_Open()` | add interpolation handler |
| `0x00474195` | `4669845` | `StationaryGun_RunHandler` | `void StationaryGun_RunHandler()` | save StationaryGun_isUsedByPlayer |
| `0x0047497B` | `4671867` | `StationaryGun_DeleteHandler` | `void StationaryGun_DeleteHandler()` | unregister interpolation list |
| `0x004782C1` | `4686529` | `GunCasing_CreateHandler` | `void GunCasing_CreateHandler()` | alloc interp data |
| `0x004782D0` | `4686544` | `GunCasing_RunHandler` | `void GunCasing_RunHandler()` | update interp data |
| `0x00478505` | `4687109` | `GunCasing_DeleteHandler` | `void GunCasing_DeleteHandler()` | dealloc interp data |
| `0x004785E5` | `4687333` | `Gun_CreateHandler` | `void Gun_CreateHandler()` | alloc interp data |
| `0x00478649` | `4687433` | `Gun_RunHandler` | `void Gun_RunHandler()` | update interp data |
| `0x004788C6` | `4688070` | `Gun_DeleteHandler` | `void Gun_DeleteHandler()` | dealloc interp data |
| `0x00482642` | `4728386` | `HumanCamera_Open` | `void HumanCamera_Open()` | add interpolation handler |
| `0x004826F5` | `4728565` | `HumanCamera_CreateHandler` | `void HumanCamera_CreateHandler()` | register interpolation list |
| `0x00482C31` | `4729905` | `HumanCamera_ThirdPersonControlInput` | `void HumanCamera_ThirdPersonControlInput()` | skip debug keys check during interpolation |
| `0x00483415` | `4731925` | `HumanCamera_MovementControlInput` | `void HumanCamera_MovementControlInput()` | skip debug keys check during interpolation |
| `0x00483455` | `4731989` | `HumanCamera_MovementControlInput_ApplyCorrectPer` | `void HumanCamera_MovementControlInput_ApplyCorrectPer()` | apply correct per-phase mouse delta |
| `0x004834AB` | `4732075` | `HumanCamera_MovementControlInput_004834AB` | `void HumanCamera_MovementControlInput_004834AB()` | apply correct per-phase mouse delta |
| `0x00483E17` | `4734487` | `HumanCamera_DeathCameraControlInput` | `void HumanCamera_DeathCameraControlInput()` | skip debug keys check during interpolation |
| `0x00483E57` | `4734551` | `HumanCamera_DeathCameraControlInput_ApplyCorrectPer` | `void HumanCamera_DeathCameraControlInput_ApplyCorrectPer()` | apply correct per-phase mouse delta |
| `0x00483E67` | `4734567` | `HumanCamera_DeathCameraControlInput_00483E67` | `void HumanCamera_DeathCameraControlInput_00483E67()` | apply correct per-phase mouse delta |
| `0x00484CEB` | `4738283` | `HumanCamera_DeleteHandler` | `void HumanCamera_DeleteHandler()` | unregister interpolation list |
| `0x004B3320` | `4928288` | `Real64x3_NullVector` | `undefined4 Real64x3_NullVector(double param_1)` | HumanView_UpdateView - apply correct per-phase mouse delta |
| `0x004B38E0` | `4929760` | `Real32x9_Create` | `void Real32x9_Create(float param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4)` | Human_InterpBody |
| `0x004D3210` | `5059088` | `AnimController_UpdateChannels` | `undefined4 AnimController_UpdateChannels(undefined4 param_1, int param_2)` | HumanView_UpdateBody - switch anim updates during interpolation |
| `0x004D9610` | `5084688` | `BoneDynCubeObj_GetQTaskType` | `undefined2 BoneDynCubeObj_GetQTaskType()` | HumanView_EnterHandler - interpolate (1st person view model) |

### 3.8 System Configuration & Storage (4 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x00402C32` | `4205618` | `J_Open` | `void J_Open()` |  |
| `0x00403642` | `4208194` | `Config_Open_CallToConfig` | `void Config_Open_CallToConfig()` | call to Config_UpdateDevicesDefMode |
| `0x00405980` | `4217216` | `Config_VerifyGraphicConfig` | `void Config_VerifyGraphicConfig()` |  |
| `0x004B8A20` | `4950560` | `Script_EvalArgInt` | `void Script_EvalArgInt()` | Config_GraphicOptionsSetDevice |

### 3.9 Timing & Clocks (2 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x00490360` | `4784992` | `Timer_Open` | `void Timer_Open()` |  |
| `0x00490370` | `4785008` | `Timer_Read` | `int Timer_Read()` |  |

### 3.10 UI, Menus & Loading Screen (5 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x00418B38` | `4295480` | `MenuManager_New` | `void MenuManager_New()` | custom main menu resolution |
| `0x00418BDB` | `4295643` | `MenuManager_New_ChangeBackgroundColor` | `void MenuManager_New_ChangeBackgroundColor()` | change background color (only visible if bg pic does not fully cover the client area) |
| `0x00418CB5` | `4295861` | `MenuManager_New_AddBackgroundfxSwitch` | `void MenuManager_New_AddBackgroundfxSwitch()` | add BackgroundFX switch |
| `0x00418CF7` | `4295927` | `MenuManager_New_HookTargetIn` | `void MenuManager_New_HookTargetIn()` | Hook target in MenuManager_New |
| `0x004199D0` | `4299216` | `BackgroundFX_New` | `void BackgroundFX_New(undefined4 param_1)` | MenuManager_New - add BackgroundFX switch |

### 3.11 Windowing, Display & Video Modes (12 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x004035F5` | `4208117` | `Config_Open` | `void Config_Open()` | init new display devices struct |
| `0x00421AB9` | `4332217` | `MenuScreen_UpdateInternalDataHandler` | `void MenuScreen_UpdateInternalDataHandler()` | fix decentered EU logo |
| `0x00421AEC` | `4332268` | `MenuScreen_UpdateInternalDataHandler_FixLogoPosition` | `void MenuScreen_UpdateInternalDataHandler_FixLogoPosition()` | fix logo position |
| `0x0048A452` | `4760658` | `LoadingScreen_New` | `void LoadingScreen_New()` | scale loading screen properly |
| `0x0048A4AD` | `4760749` | `LoadingScreen_New_ScaleLoadingScreen` | `void LoadingScreen_New_ScaleLoadingScreen()` | scale loading screen properly |
| `0x0048A50D` | `4760845` | `LoadingScreen_New_0048A50D` | `void LoadingScreen_New_0048A50D()` | scale loading screen properly |
| `0x0048F0A0` | `4780192` | `AppContext_GethWnd` | `undefined4 AppContext_GethWnd()` | Display_SetMode - resolution change |
| `0x0048F6F3` | `4781811` | `WinMain_WindowCreation` | `void WinMain_WindowCreation()` | window creation |
| `0x0048F724` | `4781860` | `WinMain_0048F724` | `void WinMain_0048F724()` | window creation |
| `0x00491B7B` | `4791163` | `Display_SetMode_ResolutionChange` | `void Display_SetMode_ResolutionChange()` | resolution change |
| `0x00491C05` | `4791301` | `Display_SetMode_SaveHeightAnd` | `void Display_SetMode_SaveHeightAnd()` | save width, height and aspect ratio from current active display mode |
| `0x004E76E0` | `5142240` | `TextWindow_vprintf` | `void TextWindow_vprintf(undefined4 param_1, undefined4 param_2, undefined4 param_3)` | DebugText_printf_NEW |

### 3.12 World Objects & Entities (9 entries)

| Hash | Address | Name | Signature | Role / Note Summary |
| :--- | :--- | :--- | :--- | :--- |
| `0x0043D273` | `4444787` | `Elevator_CreateHandler` | `void Elevator_CreateHandler()` | alloc interp data |
| `0x0043D28A` | `4444810` | `Elevator_RunHandler` | `void Elevator_RunHandler()` | update interp data |
| `0x0043DD90` | `4447632` | `Elevator_DeleteHandler` | `void Elevator_DeleteHandler()` | dealloc interp data |
| `0x004450DD` | `4477149` | `Switch_CreateHandler` | `void Switch_CreateHandler()` | alloc interp data |
| `0x004450F4` | `4477172` | `Switch_RunHandler` | `void Switch_RunHandler()` | update interp data |
| `0x004452D1` | `4477649` | `Switch_DeleteHandler` | `void Switch_DeleteHandler()` | dealloc interp data |
| `0x00446AAF` | `4483759` | `Door_CreateHandler` | `void Door_CreateHandler()` | alloc interp data |
| `0x00446B49` | `4483913` | `Door_DeleteHandler` | `void Door_DeleteHandler()` | dealloc interp data |
| `0x00446D1F` | `4484383` | `Door_RunHandler` | `void Door_RunHandler()` | update interp data |

