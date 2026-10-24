# Game native database

Generated from `docs/fidelity/native-verification.json` by `scripts/build_native_database.py`.
The manifest is the source of truth; this table is a rendering of it and must be regenerated, never edited by hand.

Retail artifact: `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.

Status meanings follow `AGENTS.md` section 10.5. **Only rows under VERIFIED and VERIFIED_WITH_DOCUMENTED_SCOPE
carry a verification claim**; everything below those two sections is static work in progress, and saying otherwise
would misrepresent the tree.

Rows below carry decoder evidence, a semantic review and a behavioral/runtime contract, and were re-checked
against this branch's records. Legacy rows whose VERIFIED status has no such record are listed separately at
the end of this document instead of being counted here.

## VERIFIED (24)

24 of these rows have no implementation file; their comment column says so and they are not
coverage, whatever the status word.

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `Config_ResetGraphicOptions` | `int Config_ResetGraphicOptions(int context)` | `0x00403B70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GetActiveGraphicOptions` | `void* __cdecl Config_GetActiveGraphicOptions(void);` | `0x00404590` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Read` | `extern "C" std::int32_t IGI_RETAIL_CDECL Config_Read(const char* path)` | `0x00405850` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Write` | `int Config_Write(void * context)` | `0x00405BD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GetActivePlayerProfile` | `void * Config_GetActivePlayerProfileRecord(void)` | `0x00406220` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters` | `extern "C" std::int32_t IGI_RETAIL_CDECL HumanPlayer_LoadParameters()` | `0x004137A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Level_Start` | `unestablished` | `0x00415B30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_HitDamage` | `void * Human_HitDamage(void)` | `0x00416D80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_ViewCam` | `int Human_ViewCam(void * context, int arg2)` | `0x00463760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_TaskViewReset` | `void __cdecl Human_TaskViewReset(void* human);` | `0x004659E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Transcontext_SetActiveTransContext` | `int __cdecl TransContext_SetActiveTransContext(const void* source);` | `0x00497E70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_Alloc` | `unestablished` | `0x004B0C60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_PackUnpack` | `void * Resource_PackUnpack(void * context, int* arg2)` | `0x004B18B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_Load` | `void* Resource_Load(const char*, int32_t*) (candidate cdecl; callback fallback differences remain unverified)` | `0x004B5F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_Unload` | `unestablished` | `0x004B6380` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_Flush` | `int Resource_Flush(int context)` | `0x004B63D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_Load` | `int32_t* Qvm_Load(void* qvm_path)` | `0x004B80B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_Cleanup` | `unestablished` | `0x004B83D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_ExecuteInstructions` | `unestablished` | `0x004BCFD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_SetOpcodeHandler` | `int __cdecl Qvm_SetOpcodeHandler(int group, int index, int handler);` | `0x004BD3D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_PushInteger` | `int __cdecl Qvm_PushInteger(void* qvm, int value);` | `0x004BDA00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AnimTrigger_ParseConfigFile` | `char * AnimTrigger_ParseConfigFile(char * context)` | `0x004EC070` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsObjType_ParseConfigFile` | `char * PhysicsObjType_ParseConfigFile(char * context)` | `0x004EDFE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Compile` | `int Config_Compile(int context, void * arg2, char arg3)` | `0x004F0CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## VERIFIED_WITH_DOCUMENTED_SCOPE (6)

6 of these rows have no implementation file; their comment column says so and they are not
coverage, whatever the status word.

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `StoreCommandLine` | `char const* __cdecl(char const*)` | `0x0048F0C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `StoreInstanceHandle` | `void* __cdecl StoreInstanceHandle(void*)` | `0x0048F0D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `SetConfigFileName` | `int __cdecl FX_TrailUpdate_22(int context);` | `0x0048F0F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppContext_SetIsActive` | `int __cdecl FX_SmokeEmit_23(int context);` | `0x0048F140` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppContext_SetDebugged` | `x86 cdecl int32_t(int8_t)` | `0x0048F1A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppContext_SetDebugtextState` | `int __cdecl AppContext_SetDebugtextState(int context);` | `0x0048F1E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## RUNTIME_TESTED (12)

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `SetAppTitle` | `int32_t(const char* title)` | `0x0048F100` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_GetScreenMode` | `int32_t(void)` | `0x0048F130` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppContext_GetIsActive` | `int __cdecl AppContext_GetIsActive(void);` | `0x0048F170` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Game_SetStartupMode` | `int32_t(int32_t value)` | `0x0048F210` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `App_ContextSetLightmapsUsed` | `int32_t(int8_t)` | `0x0048F240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppContext_SetTerrainLightmapsUsed` | `std::int32_t App_ContextSetTerrainLightmapsUsed(std::int8_t used)` | `0x0048F260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppMain_ParseCmdLineArgs` | `unestablished` | `0x0048f360` | - | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppMain_HardcoreGermanCommand` | `unestablished` | `0x0048F960` | - | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AppMain_TokenizeDelimited` | `unestablished` | `0x004A5A0C` | - | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTaskList_Register` | `void* __cdecl(void* task_list, void* task)` | `0x004C1790` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTaskList_Remove` | `int32_t __cdecl(void* task_list, void* task)` | `0x004C17C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTaskList_New` | `void* __cdecl(int32_t capacity)` | `0x004C1800` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## BUILD_VERIFIED (5)

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `QTask_CreateTask` | `int __cdecl int32_t(void)` | `0x00401000` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Native_00498480` | `uint32_t __cdecl Native_00498480(uint32_t* capability, uint32_t* platform);` | `0x00498480` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Native_004A5000` | `int __cdecl Native_004A5000(void* buffer, unsigned count, ...);` | `0x004A5000` | - | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Native_004A7EFF` | `uint32_t __cdecl Native_004A7EFF(uint32_t** cursor);` | `0x004A7EFF` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_ReportError` | `std::int32_t Qvm_ReportError(const char* format, ...)` | `0x004B7E10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## STRUCTURALLY_VERIFIED (1046)

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `QTask_OpenEvent` | `void __cdecl QTask_OpenEvent(void);` | `0x00401050` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_Init` | `unestablished` | `0x004010D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_ClearTaskTypes` | `unestablished` | `0x004011B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_ClearEventTable` | `unestablished` | `0x004011D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_LinkQTaskEventHandler` | `unestablished` | `0x00401400` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TaskType_RefreshHandlerMask` | `unestablished` | `0x004014B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `TaskType_HandlerPhaseMask` | `unestablished` | `0x004014E0` | 1.000 | CFG 1.000; bn+r2 agree |
| `QTask_LinkQTaskEventHandlerNoContextFn` | `unestablished` | `0x00401530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_UnlinkTask_4015D0` | `unestablished` | `0x004015D0` | 1.000 | CFG 1.000; bn+r2 agree |
| `QTask_DestroyTask_4015F0` | `unestablished` | `0x004015F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004015F0-semantic-review.json` |
| `QTask_AllocateEvent` | `unestablished` | `0x004017C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_NewQTaskEventNoContextFn` | `unestablished` | `0x00401810` | 1.000 | CFG 1.000; bn+r2 agree |
| `TaskEventTable_MarkSlot` | `unestablished` | `0x00401860` | 1.000 | CFG 1.000; bn+r2 agree |
| `TaskEventTable_MarkSlotNoMask` | `unestablished` | `0x00401890` | 1.000 | CFG 1.000; bn+r2 agree |
| `TaskEventTable_ClearSlot` | `unestablished` | `0x004018C0` | 1.000 | CFG 1.000; bn+r2 agree |
| `QTask_GetCurrentType` | `unestablished` | `0x004018F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QHashSlotTable_Release` | `unestablished` | `0x00401B20` | 1.000 | CFG 1.000; bn+r2 agree |
| `TaskType_SetPhaseHandlerByte` | `unestablished` | `0x00401D30` | 0.058 | CFG 0.058; bn+r2 agree |
| `Flow_Init` | `unestablished` | `0x00401FC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Flow_Start` | `unestablished` | `0x00402070` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Flow_RunHandler` | `unestablished` | `0x00402260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Flow_Delete` | `unestablished` | `0x00402740` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Flow_CreateHandler` | `unestablished` | `0x00402800` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Game_GetContext` | `void* __cdecl Game_GetContext(void);` | `0x00402880` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Flow_SetState` | `void __cdecl Flow_SetState(int state);` | `0x004028A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_YmbeAfp` | `int __fastcall Config_YmbeAfp(int context)` | `0x00402F50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillPlayerProfileListBox_2` | `int Config_FillPlayerProfileListBox_2(void)` | `0x00402FF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOPlayer` | `int32_t GOPlayer(int32_t param_1, float param_2);` | `0x004036B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOActiveMission` | `int32_t GOActiveMission(int32_t param_1, float param_2);` | `0x00403750` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOInRemap` | `int32_t GOInRemap(int32_t param_1, float param_2);` | `0x004037C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOInMouInv` | `int32_t GOInMouInv(int32_t param_1, float param_2);` | `0x00403870` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOInMouSens` | `int32_t GOInMouSens(long param_1, int32_t param_2, float param_3);` | `0x004038E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGfxDisp` | `int32_t GOGfxDisp(int32_t param_1, float param_2);` | `0x00403950` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGameLang` | `int32_t GOGameLang(int32_t param_1, float param_2);` | `0x00403A00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGameDiff` | `int32_t GOGameDiff(int32_t param_1, float param_2);` | `0x00403AC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_IsGermany` | `int Config_IsGermany(int context)` | `0x00403B30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GetContentControlPassword` | `int Config_GetContentControlPassword(int context)` | `0x00403C70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_VerifyContentControlPassword` | `int Config_VerifyContentControlPassword(int context, float arg2)` | `0x00403CC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_IsContentControlPasswordEnabled` | `int Config_IsContentControlPasswordEnabled(int context)` | `0x00403D90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOIsBlood` | `int32_t GOIsBlood(int32_t param_1, float param_2);` | `0x00403DF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOSoundSpeech` | `int32_t GOSoundSpeech(long param_1, int32_t param_2, float param_3);` | `0x00403E60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOSoundMusic` | `int32_t GOSoundMusic(long param_1, int32_t param_2, float param_3);` | `0x00403EF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOSoundFX` | `int32_t GOSoundFX(long param_1, int32_t param_2, float param_3);` | `0x00403F80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGfxDevice` | `int32_t GOGfxDevice(int32_t param_1, float param_2);` | `0x00404010` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGfxGamma` | `int32_t GOGfxGamma(long param_1, int32_t param_2, float param_3);` | `0x00404080` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GOGfxPerformance` | `int32_t GOGfxPerformance(int32_t param_1, float param_2);` | `0x004040F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputSetInvertMouse` | `int Config_GameOptionsInputSetInvertMouse(long double context, int arg2, float arg3)` | `0x00404160` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputGetInvertMouse` | `int Config_GameOptionsInputGetInvertMouse(int context)` | `0x004041D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputSetMouseSensitivity` | `int Config_GameOptionsInputSetMouseSensitivity(long double context, int arg2, float arg3)` | `0x00404230` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputGetMouseSensitivity` | `int Config_GameOptionsInputGetMouseSensitivity(int context)` | `0x00404290` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputSetBloodEnabled` | `int Config_GameOptionsInputSetBloodEnabled(long double context, int arg2, float arg3)` | `0x004042E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GameOptionsInputGetBloodEnabled` | `int Config_GameOptionsInputGetBloodEnabled(int context)` | `0x00404340` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_RegisterOptions` | `void * Config_RegisterOptions(void * context)` | `0x00404390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillScreenResolutionListBox` | `int Config_FillScreenResolutionListBox(int context, float arg2)` | `0x00404450` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsSetResolution` | `int Config_GraphicOptionsSetResolution(int context, float arg2)` | `0x00404510` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetResolution` | `int Config_GraphicOptionsGetResolution(int context, float arg2)` | `0x004045B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillRenderDeviceListBox` | `int Config_FillRenderDeviceListBox(int context)` | `0x004046A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetDevice` | `int Config_GraphicOptionsGetDevice(int context)` | `0x00404710` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsSetDevice` | `int Config_GraphicOptionsSetDevice(int context, float arg2)` | `0x004047F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsSetGamma` | `int Config_GraphicOptionsSetGamma(long double context, int arg2, float arg3)` | `0x004048B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetGamma` | `int Config_GraphicOptionsGetGamma(int context)` | `0x00404910` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsSetTransparency` | `int Config_GraphicOptionsSetTransparency(int context, float arg2)` | `0x00404960` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetTransparency` | `int Config_GraphicOptionsGetTransparency(int context)` | `0x004049C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetSoundsEffects` | `int Config_SoundOptionsSetSoundsEffects(int context, float arg2)` | `0x00404A10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveGameOptions` | `void * Config_SaveGameOptions(void)` | `0x00404A70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetSoundsEffects` | `int Config_SoundOptionsGetSoundsEffects(int context)` | `0x00404A90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetSoundsEffectsVolume` | `int Config_SoundOptionsSetSoundsEffectsVolume(long double context, int arg2, float arg3)` | `0x00404AE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetSoundsEffectsVolume` | `int Config_SoundOptionsGetSoundsEffectsVolume(int context)` | `0x00404B40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetMusic` | `int Config_SoundOptionsSetMusic(int context, float arg2)` | `0x00404B90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetMusic` | `int Config_SoundOptionsGetMusic(int context)` | `0x00404BF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetMusicVolume` | `int Config_SoundOptionsSetMusicVolume(long double context, int arg2, float arg3)` | `0x00404C40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetMusicVolume` | `int Config_SoundOptionsGetMusicVolume(int context)` | `0x00404CA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetSpeech` | `int Config_SoundOptionsSetSpeech(int context, float arg2)` | `0x00404CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetSpeech` | `int Config_SoundOptionsGetSpeech(int context)` | `0x00404D50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetSpeechVolume` | `int Config_SoundOptionsSetSpeechVolume(long double context, int arg2, float arg3)` | `0x00404DA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetSpeechVolume` | `int Config_SoundOptionsGetSpeechVolume(int context)` | `0x00404E00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsSetReverseStereo` | `int Config_SoundOptionsSetReverseStereo(int context, float arg2)` | `0x00404E50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SoundOptionsGetReverseStereo` | `int Config_SoundOptionsGetReverseStereo(int context)` | `0x00404EB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsSetPerformanceFlags` | `void __cdecl Config_GraphicOptionsSetPerformanceFlags(std::uint32_t* result, int unused, void* vm);` | `0x00404F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetPerformanceFlags` | `void __cdecl Config_GraphicOptionsGetPerformanceFlags(std::uint32_t* result, int unused, void* vm);` | `0x00404F60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillPlayerProfileListBox` | `int Config_FillPlayerProfileListBox(int context)` | `0x00404FA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GetActivePlayerProfileIndex` | `int Config_GetActivePlayerProfileIndex(int context)` | `0x00405010` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetActivePlayerProfileIndex` | `int Config_SetActivePlayerProfileIndex(int context, float arg2)` | `0x00405050` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_CreateNewPlayerProfile` | `int Config_CreateNewPlayerProfile(int context, float arg2)` | `0x004050A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveGraphicOptions` | `int Config_SaveGraphicOptions(void)` | `0x00405300` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_DeletePlayerProfile` | `int Config_DeletePlayerProfile(int context, float arg2)` | `0x00405400` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GetNumberOfPlayerProfiles` | `int Config_GetNumberOfPlayerProfiles(int context)` | `0x004054F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetPerfLevelFromFlags` | `void __cdecl Config_GraphicOptionsGetPerfLevelFromFlags(std::uint32_t* result, int unused, void* vm);` | `0x00405530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsGetPerfFlagsFromLevel` | `void __cdecl Config_GraphicOptionsGetPerfFlagsFromLevel(std::uint32_t* result, int unused, void* vm);` | `0x004055C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_GraphicOptionsNumPerfLevels` | `void __cdecl Config_GraphicOptionsNumPerfLevels(std::uint32_t* result, int unused, void* vm);` | `0x00405620` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillMissionSelectionBox` | `int Config_FillMissionSelectionBox(int context)` | `0x00405660` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillMissionPictureBox` | `int Config_FillMissionPictureBox(int context)` | `0x004056F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_FillMissionTextBox` | `int Config_FillMissionTextBox(int context)` | `0x00405760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_PlayerGetActiveMission` | `int Config_PlayerGetActiveMission(int context, int arg2)` | `0x004057D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Language` | `int Config_Language(void)` | `0x00405830` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveSoundOptions` | `int Config_SaveSoundOptions(void)` | `0x004058A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_NoD3DDriversWithHard` | `int Config_NoD3DDriversWithHard(void)` | `0x00405980` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Language_2` | `int Config_Language_2(void)` | `0x00405B30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveInputOptions` | `int Config_SaveInputOptions(int* context)` | `0x00405C50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Goplayer` | `int Config_Goplayer(int* context, int arg2)` | `0x00405C90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_English` | `int Config_English(int* context, int arg2)` | `0x00405D40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Gosoundspeech` | `int Config_Gosoundspeech(int* context, int arg2)` | `0x00405E00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Gogfxdisp` | `int Config_Gogfxdisp(int* context, int arg2)` | `0x00405EC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Gocontentcontrolpw` | `int Config_Gocontentcontrolpw(int* context)` | `0x00406170` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SavePlayerProfile` | `void Config_SavePlayerProfile(void)` | `0x004061B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetGamma` | `void * Config_SetGamma(void)` | `0x004061C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetResolution` | `int Config_SetResolution(void)` | `0x004061E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetVolume` | `int Config_SetVolume(int context)` | `0x00406200` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetDifficulty` | `void Config_SetDifficulty(void)` | `0x00406270` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_Moveleft` | `int Config_Moveleft(int context, int* arg2, void ** arg3)` | `0x00406280` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_ValidatePassword` | `void Config_ValidatePassword(void)` | `0x004062B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_RegisterOptions_2` | `long double Config_RegisterOptions_2(void)` | `0x004062C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveGameOptions_2` | `long double Config_SaveGameOptions_2(void)` | `0x004062D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveGraphicOptions_2` | `int Config_SaveGraphicOptions_2(void)` | `0x004062E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveSoundOptions_2` | `int Config_SaveSoundOptions_2(int context)` | `0x004062F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SaveInputOptions_2` | `void Config_SaveInputOptions_2(void)` | `0x00406310` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_English_2` | `void Config_English_2(void)` | `0x00406320` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SavePlayerProfile_2` | `void Config_SavePlayerProfile_2(void)` | `0x00406330` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetGamma_2` | `void * Config_SetGamma_2(int context)` | `0x00406340` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetResolution_2` | `int __fastcall Config_SetResolution_2(int context)` | `0x00406390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAIConfig_Init` | `int* HumanAIConfig_Init(void)` | `0x00406450` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAIConfig_OnEvent` | `int HumanAIConfig_OnEvent(void * context)` | `0x00406D10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Setactivematerial` | `void * HumanPlayer_Setactivematerial(void)` | `0x00407D60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineQMaterial` | `int32_t DefineQMaterial(long param_1, long param_2, long param_3, int32_t param_4, int32_t param_5);` | `0x00407E30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineGameMaterial` | `int32_t DefineGameMaterial(long param_1, long param_2, long param_3, long param_4, int32_t param_5, float param_6);` | `0x00407F50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_2` | `unresolved` | `0x004082B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion` | `int HumanPlayer_UpdateMotion(void)` | `0x00408320` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround` | `int HumanPlayer_CheckGround(void)` | `0x00408330` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina` | `int HumanPlayer_UpdateStamina(void)` | `0x00408340` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth` | `int HumanPlayer_UpdateHealth(int context)` | `0x00408390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput` | `int HumanPlayer_HandleInput(short context, int arg2, int arg3, int arg4, int* arg5)` | `0x004083C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics` | `void * HumanPlayer_ApplyPhysics(void * context)` | `0x00408420` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep` | `int HumanPlayer_PlayFootstep(void * context)` | `0x00408720` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound` | `int HumanPlayer_PlaySound(void * context, void * arg2)` | `0x00408770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_4087B0` | `int HumanPlayer_ExecutePipeline_4087B0(void* arg1, void* arg2)` | `0x004087B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage` | `int HumanPlayer_TakeDamage(void * context, void * arg2)` | `0x00408A40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation` | `int HumanPlayer_UpdateAnimation(void * context, void * arg2)` | `0x00408AA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision` | `int HumanPlayer_CheckCollision(void * context, void * arg2)` | `0x00408C30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil` | `int HumanPlayer_ApplyRecoil(void * context, void * arg2)` | `0x00408C80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_3` | `unresolved` | `0x00408DF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_2` | `int HumanPlayer_UpdateMotion_2(void * context, void * arg2)` | `0x00408E70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_2` | `int HumanPlayer_CheckGround_2(void * context, float arg2)` | `0x00408FE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_2` | `int HumanPlayer_UpdateStamina_2(void * context, void * arg2)` | `0x00409090` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_2` | `int HumanPlayer_UpdateHealth_2(void * context, float arg2)` | `0x00409220` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_2` | `int HumanPlayer_HandleInput_2(void * context, void * arg2)` | `0x004092A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_2` | `int HumanPlayer_ApplyPhysics_2(void * context, float arg2)` | `0x00409410` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_2` | `int HumanPlayer_PlayFootstep_2(void * context, void * arg2)` | `0x00409490` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_2` | `int HumanPlayer_PlaySound_2(void * context, void * arg2)` | `0x00409600` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_2` | `int HumanPlayer_TakeDamage_2(void * context, void * arg2)` | `0x00409660` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_2` | `int HumanPlayer_UpdateAnimation_2(void * context, void * arg2)` | `0x004097F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_2` | `int HumanPlayer_CheckCollision_2(void * context, void * arg2)` | `0x00409850` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_2` | `int HumanPlayer_ApplyRecoil_2(void * context, float arg2)` | `0x004099E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_409A80` | `int HumanPlayer_ExecutePipeline_409A80(void* arg1, void* arg2)` | `0x00409A80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_4` | `unresolved` | `0x00409C10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_409C90` | `int HumanPlayer_ExecutePipeline_409C90(void* arg1, void* arg2)` | `0x00409C90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_3` | `int HumanPlayer_UpdateMotion_3(void * context, float arg2)` | `0x00409E00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_409E80` | `int HumanPlayer_ExecutePipeline_409E80(void* arg1, void* arg2)` | `0x00409E80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_3` | `int HumanPlayer_CheckGround_3(void * context, float arg2)` | `0x00409FF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40A090` | `int HumanPlayer_ExecutePipeline_40A090(void* arg1, void* arg2)` | `0x0040A090` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_3` | `int HumanPlayer_UpdateStamina_3(void * context, float arg2)` | `0x0040A220` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40A2A0` | `int HumanPlayer_ExecutePipeline_40A2A0(void* arg1, void* arg2)` | `0x0040A2A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_3` | `int HumanPlayer_UpdateHealth_3(void * context, float arg2)` | `0x0040A410` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40A490` | `int HumanPlayer_ExecutePipeline_40A490(void* arg1, void* arg2)` | `0x0040A490` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_3` | `int HumanPlayer_HandleInput_3(void * context, float arg2)` | `0x0040A600` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40A670` | `int HumanPlayer_ExecutePipeline_40A670(void* arg1, void* arg2)` | `0x0040A670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_3` | `int HumanPlayer_ApplyPhysics_3(void * context, float arg2)` | `0x0040A800` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40A870` | `int HumanPlayer_ExecutePipeline_40A870(void* arg1, void* arg2)` | `0x0040A870` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_3` | `int HumanPlayer_PlayFootstep_3(void * context, void * arg2)` | `0x0040AA00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_InvokeStateAnimation` | `void* HumanPlayer_InvokeStateAnimation(void* arg1, int arg2)` | `0x0040AA30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_3` | `int HumanPlayer_PlaySound_3(void * context, void * arg2)` | `0x0040AA90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_InvokeStateAnimation_40AAC0` | `void* HumanPlayer_InvokeStateAnimation_40AAC0(void* arg1, int arg2)` | `0x0040AAC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_3` | `int HumanPlayer_TakeDamage_3(void * context, void * arg2)` | `0x0040AB20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40AB60` | `void* HumanPlayer_ExecutePipeline_40AB60(void* arg1, int arg2)` | `0x0040AB60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_3` | `int HumanPlayer_UpdateAnimation_3(void * context, float arg2)` | `0x0040ACB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40AD20` | `void* HumanPlayer_ExecutePipeline_40AD20(void* arg1, int arg2)` | `0x0040AD20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_3` | `int HumanPlayer_CheckCollision_3(void * context, float arg2)` | `0x0040AE70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40AEF0` | `void* HumanPlayer_ExecutePipeline_40AEF0(void* arg1, int arg2)` | `0x0040AEF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_3` | `int HumanPlayer_ApplyRecoil_3(void * context, float arg2)` | `0x0040B020` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B0A0` | `void* HumanPlayer_ExecutePipeline_40B0A0(void* arg1, int arg2)` | `0x0040B0A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_5` | `unresolved` | `0x0040B1D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B240` | `void* HumanPlayer_ExecutePipeline_40B240(void* arg1, int arg2)` | `0x0040B240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_4` | `int HumanPlayer_UpdateMotion_4(void * context, float arg2)` | `0x0040B390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B410` | `void* HumanPlayer_ExecutePipeline_40B410(void* arg1, int arg2)` | `0x0040B410` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_4` | `int HumanPlayer_CheckGround_4(void * context, float arg2)` | `0x0040B540` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B5C0` | `void* HumanPlayer_ExecutePipeline_40B5C0(void* arg1, int arg2)` | `0x0040B5C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_4` | `int HumanPlayer_UpdateStamina_4(void * context, float arg2)` | `0x0040B6F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B760` | `void* HumanPlayer_ExecutePipeline_40B760(void* arg1, int arg2)` | `0x0040B760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_4` | `int HumanPlayer_UpdateHealth_4(void * context, float arg2)` | `0x0040B8B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40B920` | `void* HumanPlayer_ExecutePipeline_40B920(void* arg1, int arg2)` | `0x0040B920` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_4` | `int* HumanPlayer_HandleInput_4(void * context, void * arg2)` | `0x0040BA70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_4` | `int HumanPlayer_ApplyPhysics_4(void * context, float arg2)` | `0x0040BBD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_4` | `float HumanPlayer_PlayFootstep_4(void * context, void * arg2)` | `0x0040BDB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_4` | `int HumanPlayer_PlaySound_4(void * context, float arg2)` | `0x0040BE40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_4` | `void * HumanPlayer_TakeDamage_4(void * context, float arg2)` | `0x0040BED0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_4` | `void * HumanPlayer_UpdateAnimation_4(void * context)` | `0x0040C140` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_4` | `void * HumanPlayer_CheckCollision_4(void * context)` | `0x0040C150` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Processpickloopend` | `int HumanPlayer_Processpickloopend(void* arg1, void* arg2)` | `0x0040C160` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_4` | `int* HumanPlayer_ApplyRecoil_4(int context, void * arg2)` | `0x0040C270` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_6` | `unresolved` | `0x0040C2D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_5` | `void * HumanPlayer_UpdateMotion_5(void * context)` | `0x0040C2E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_5` | `void * HumanPlayer_CheckGround_5(void * context, float arg2)` | `0x0040C2F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_5` | `int HumanPlayer_UpdateStamina_5(void * context, int arg2)` | `0x0040C510` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_5` | `struct status_msg* HumanPlayer_UpdateHealth_5(void * context, float arg2)` | `0x0040C670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Processtypecompend` | `int HumanPlayer_Processtypecompend(void* arg1, void* arg2)` | `0x0040C870` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_5` | `void * HumanPlayer_HandleInput_5(void * context, float arg2)` | `0x0040C970` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke21` | `void* HumanPlayer_Invoke21(void* arg1, void* arg2)` | `0x0040CAE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_5` | `void * HumanPlayer_ApplyPhysics_5(void * context, float arg2)` | `0x0040CB90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_5` | `int* HumanPlayer_PlayFootstep_5(int* context, void * arg2)` | `0x0040CCC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_5` | `int* HumanPlayer_PlaySound_5(void * context, int arg2)` | `0x0040CD30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_5` | `void * HumanPlayer_TakeDamage_5(void * context, float arg2)` | `0x0040CF70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_5` | `void * HumanPlayer_UpdateAnimation_5(void * context, void * arg2)` | `0x0040D180` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke21_40D580` | `int HumanPlayer_Invoke21_40D580(void* arg1, void* arg2)` | `0x0040D580` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_5` | `void ** HumanPlayer_CheckCollision_5(void * context, void * arg2)` | `0x0040D660` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_5` | `void ** HumanPlayer_ApplyRecoil_5(void * context, void * arg2)` | `0x0040D8E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_7` | `unresolved` | `0x0040DA40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_6` | `int* HumanPlayer_UpdateMotion_6(void * context, void * arg2)` | `0x0040DA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_WireSlide1` | `struct status_msg* HumanPlayer_WireSlide1(void * context, void * arg2)` | `0x0040DBA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_6` | `void HumanPlayer_CheckGround_6(void * context, void * arg2)` | `0x0040DBF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_6` | `void ** HumanPlayer_UpdateStamina_6(void * context, void * arg2)` | `0x0040DCA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_InvokeStateAnimation_40DD00` | `void* HumanPlayer_InvokeStateAnimation_40DD00(void* arg1, void* arg2)` | `0x0040DD00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_6` | `void ** HumanPlayer_UpdateHealth_6(void * context, void * arg2)` | `0x0040DD60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Processwiredragd` | `struct status_msg* HumanPlayer_Processwiredragd(void* arg1, void* arg2)` | `0x0040DDA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_6` | `void ** HumanPlayer_HandleInput_6(void * context, void * arg2)` | `0x0040DE90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_6` | `int* HumanPlayer_ApplyPhysics_6(void * context)` | `0x0040DEF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_6` | `void * HumanPlayer_PlayFootstep_6(void * context, void * arg2)` | `0x0040DF70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_40DFC0` | `float HumanPlayer_ExecutePipeline_40DFC0(float arg1) __location("result_val")` | `0x0040DFC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_6` | `struct status_msg* HumanPlayer_TakeDamage_6(void * context, void * arg2)` | `0x0040E370` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_6` | `int HumanPlayer_UpdateAnimation_6(void * context, void * arg2)` | `0x0040E3C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_6` | `void * HumanPlayer_CheckCollision_6(void * context, void * arg2)` | `0x0040E470` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_6` | `int HumanPlayer_ApplyRecoil_6(void * context, void * arg2)` | `0x0040E4D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayerDeath` | `int HumanPlayer_PlayerDeath(void * context, void * arg2)` | `0x0040E560` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_7` | `int* HumanPlayer_UpdateMotion_7(void * context, void * arg2)` | `0x0040E6E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_7` | `short HumanPlayer_CheckGround_7(void * context, float arg2)` | `0x0040E740` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_7` | `void ** HumanPlayer_UpdateStamina_7(void * context, void * arg2)` | `0x0040E960` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke9` | `int HumanPlayer_Invoke9(void* arg1, void* arg2)` | `0x0040E980` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_7` | `void * HumanPlayer_UpdateHealth_7(void * context)` | `0x0040E9B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_7` | `int* HumanPlayer_HandleInput_7(void * context, int* arg2)` | `0x0040E9C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_7` | `void ** HumanPlayer_PlayFootstep_7(void * context, void * arg2)` | `0x0040EF70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_GunsDry1` | `struct status_msg* HumanPlayer_GunsDry1(void * context, void * arg2)` | `0x0040F0D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke12` | `int HumanPlayer_Invoke12(void* arg1, void* arg2)` | `0x0040F100` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_7` | `void * HumanPlayer_PlaySound_7(void * context)` | `0x0040F170` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_7` | `int HumanPlayer_TakeDamage_7(void * context, void * arg2)` | `0x0040F180` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_7` | `void * HumanPlayer_UpdateAnimation_7(void * context, void * arg2)` | `0x0040F230` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_7` | `int HumanPlayer_CheckCollision_7(void * context, int* arg2)` | `0x0040F280` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_7` | `int HumanPlayer_ApplyRecoil_7(void * context, void * arg2)` | `0x0040F300` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke20` | `int HumanPlayer_Invoke20(void* arg1, void* arg2)` | `0x0040F3F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_8` | `int HumanPlayer_UpdateMotion_8(void * context, void * arg2)` | `0x0040F4A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_8` | `int HumanPlayer_CheckGround_8(void * context, void * arg2)` | `0x0040F4F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_GrenadeTick1` | `struct status_msg* HumanPlayer_GrenadeTick1(void * context, void * arg2)` | `0x0040F590` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_8` | `void * HumanPlayer_UpdateStamina_8(void * context, int* arg2)` | `0x0040F640` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_8` | `int HumanPlayer_UpdateHealth_8(int* context, int arg2)` | `0x0040F760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_8` | `void * HumanPlayer_HandleInput_8(void * context, void * arg2)` | `0x0040F930` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_8` | `void * HumanPlayer_ApplyPhysics_8(void * context, int* arg2)` | `0x0040F950` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_8` | `int HumanPlayer_PlayFootstep_8(void * context, void * arg2)` | `0x0040FA20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_8` | `void * HumanPlayer_PlaySound_8(void * context, void * arg2)` | `0x0040FA60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_8` | `int HumanPlayer_UpdateAnimation_8(void * context, void * arg2)` | `0x0040FA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_8` | `int HumanPlayer_CheckCollision_8(void * context, void * arg2)` | `0x0040FB30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke12_40FB90` | `void* HumanPlayer_Invoke12_40FB90(void* arg1, void* arg2)` | `0x0040FB90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_8` | `int* HumanPlayer_ApplyRecoil_8(void * context, void * arg2)` | `0x0040FBE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_GetTeam` | `void* HumanPlayer_GetTeam(void* context, int arg2)` | `0x0040FD70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_SetTeam` | `void HumanPlayer_SetTeam(void* context, void* arg2)` | `0x0040FDF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Init` | `void * HumanPlayer_Init(void)` | `0x0040FE30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineHumanPlayerGeneral` | `extern "C" void* IGI_RETAIL_CDECL DefineHumanPlayerGeneral(void* context, std::int32_t argument_slot, void* frame)` | `0x00410070` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineHumanPlayerWeaponCycle` | `int32_t DefineHumanPlayerWeaponCycle(int32_t param_1, int32_t* param_2);` | `0x004101C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineHumanPlayerWeaponCategory` | `int32_t DefineHumanPlayerWeaponCategory(int32_t param_1, float param_2);` | `0x00410240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineHumanPlayerAmmoLimit` | `int32_t DefineHumanPlayerAmmoLimit(int32_t param_1, int32_t* param_2);` | `0x004102D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_DefaultHandler_410360` | `int HumanPlayer_DefaultHandler_410360()` | `0x00410360` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_4105C0` | `int HumanPlayer_ExecutePipeline_4105C0(void* arg1, void* arg2)` | `0x004105C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ExecutePipeline_410700` | `int HumanPlayer_ExecutePipeline_410700(void* arg1, void* arg2)` | `0x00410700` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Setup` | `int HumanPlayer_Setup(void * context)` | `0x00410840` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke8` | `int* HumanPlayer_Invoke8(int* arg1)` | `0x004108B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke8_4108C0` | `int32_t HumanPlayer_Invoke8_4108C0(int32_t param_1, int32_t param_2);` | `0x004108C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke29` | `int HumanPlayer_Invoke29(void* arg1, int arg2)` | `0x00410900` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_InvokeRunUpdate` | `void* HumanPlayer_InvokeRunUpdate(int arg1, void* arg2)` | `0x00410960` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_RunUpdate` | `void * Human_RunUpdate(int* context)` | `0x004109A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_9` | `int* HumanPlayer_UpdateStamina_9(void * context)` | `0x00410F70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_9` | `int* HumanPlayer_UpdateHealth_9(int* context)` | `0x00411000` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Earcandy` | `void * HumanPlayer_Earcandy(void * context)` | `0x00411240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_9` | `void * HumanPlayer_HandleInput_9(void * context)` | `0x00411300` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_9` | `int HumanPlayer_ApplyPhysics_9(void * context)` | `0x00411350` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_9` | `int HumanPlayer_PlayFootstep_9(void * context)` | `0x004113C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_9` | `int* HumanPlayer_PlaySound_9(float context)` | `0x00411770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_9` | `int HumanPlayer_TakeDamage_9(void * context)` | `0x00411AE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ProcessNoactiveplayerpr` | `void* HumanPlayer_ProcessNoactiveplayerpr(int* arg1)` | `0x004123F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Invoke16` | `int HumanPlayer_Invoke16(int* arg1)` | `0x00412420` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_DamageApply` | `float Human_DamageApply(float context, int* arg2)` | `0x00412440` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Update` | `int HumanPlayer_Update(float context, float arg2)` | `0x00412990` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_OnEvent` | `int HumanPlayer_OnEvent(float context, float* arg2)` | `0x00412CE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Action` | `struct status_msg* HumanPlayer_Action(void * context)` | `0x00412F80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Cleanup` | `int HumanPlayer_Cleanup(void * context, int* arg2)` | `0x00413090` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_Cleanup_2` | `void * HumanPlayer_Cleanup_2(void * context, int arg2)` | `0x00413120` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_14` | `unresolved` | `0x00413730` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_9` | `unsigned int HumanPlayer_UpdateAnimation_9(void)` | `0x00413770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_9` | `void ** HumanPlayer_CheckCollision_9(void * context, float arg2)` | `0x004137E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_9` | `int HumanPlayer_ApplyRecoil_9(void * context, int arg2)` | `0x00413A60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_12` | `unresolved` | `0x00413A70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_10` | `long double HumanPlayer_UpdateMotion_10(void)` | `0x00413A80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_10` | `int HumanPlayer_CheckGround_10(void)` | `0x00413B00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_10` | `int HumanPlayer_UpdateStamina_10(void)` | `0x00413B10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Core_GetFlagByte_413B20` | `unestablished` | `0x00413B20` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00413B20-semantic-review.json` |
| `HumanPlayer_HandleInput_10` | `int HumanPlayer_HandleInput_10(void)` | `0x00413B30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_10` | `int HumanPlayer_ApplyPhysics_10(void)` | `0x00413B40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlayFootstep_10` | `int HumanPlayer_PlayFootstep_10(void)` | `0x00413B50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_PlaySound_10` | `int HumanPlayer_PlaySound_10(void)` | `0x00413B60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_TakeDamage_10` | `void HumanPlayer_TakeDamage_10(void)` | `0x00413B90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateAnimation_10` | `int HumanPlayer_UpdateAnimation_10(void)` | `0x00413BC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckCollision_10` | `int* HumanPlayer_CheckCollision_10(int context)` | `0x00413BD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyRecoil_10` | `void * HumanPlayer_ApplyRecoil_10(char * context)` | `0x00413C00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_13` | `unresolved` | `0x00413C80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateMotion_11` | `int* HumanPlayer_UpdateMotion_11(int* context, int arg2)` | `0x00413CC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_CheckGround_11` | `void * HumanPlayer_CheckGround_11(int context, int arg2, int arg3, int arg4, int arg5, int arg6, void * arg7, int arg8, int arg9)` | `0x00413D10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateStamina_11` | `void HumanPlayer_UpdateStamina_11(void)` | `0x00413DB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_UpdateHealth_11` | `void HumanPlayer_UpdateHealth_11(void)` | `0x00413DC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_HandleInput_11` | `void HumanPlayer_HandleInput_11(void)` | `0x00413DD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_ApplyPhysics_11` | `void HumanPlayer_ApplyPhysics_11(void)` | `0x00413DE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Game_Init` | `unestablished` | `0x00415530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_Init` | `int* ModelViewBox_Init(void)` | `0x00422B40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_Process` | `int ModelViewBox_Process(void * context, int arg2)` | `0x00422C90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_Setup` | `int ModelViewBox_Setup(void * context)` | `0x00422DF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_Update` | `void * ModelViewBox_Update(void * context, int arg2)` | `0x00422E40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_LoadParameters` | `int ModelViewBox_LoadParameters(int context)` | `0x00422F30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `ModelViewBox_OnEvent` | `int ModelViewBox_OnEvent(void * context)` | `0x00423000` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal32_424870` | `unestablished` | `0x00424870` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_424880` | `unestablished` | `0x00424880` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_424890` | `unestablished` | `0x00424890` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_424B80` | `unestablished` | `0x00424B80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_SetConstant_427D70` | `unestablished` | `0x00427D70` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_SetConstant_427F80` | `unestablished` | `0x00427F80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal16_42FDC0` | `unestablished` | `0x0042FDC0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal16_430950` | `unestablished` | `0x00430950` | 1.000 | CFG 1.000; bn+r2 agree |
| `VehiclePhysics_ProcessVirModelsnotavai` | `int VehiclePhysics_ProcessVirModelsnotavai(int* arg1, float arg2)` | `0x00434E10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `VehiclePhysics_ProcessVirModelsnotavai_435460` | `int VehiclePhysics_ProcessVirModelsnotavai_435460(int* arg1, float arg2)` | `0x00435460` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `VehiclePhysics_ProcessVirModelsnotavai_435C50` | `int VehiclePhysics_ProcessVirModelsnotavai_435C50(int* arg1, float arg2)` | `0x00435C50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal16_443E80` | `unestablished` | `0x00443E80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_443E90` | `unestablished` | `0x00443E90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal16_448550` | `unestablished` | `0x00448550` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_448560` | `unestablished` | `0x00448560` | 1.000 | CFG 1.000; bn+r2 agree |
| `HumanAI_Init` | `void * HumanAI_Init(void)` | `0x0044C8D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_LoadParameters` | `int HumanAI_LoadParameters(void * context)` | `0x0044F060` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Setup` | `void * HumanAI_Setup(int context)` | `0x0044F410` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_EventDispatch` | `int HumanAI_EventDispatch(void * context, int arg2, int arg3)` | `0x0044FC90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_RunActions` | `int* HumanAI_RunActions(void * context)` | `0x0044FEC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Process` | `int* HumanAI_Process(void * context)` | `0x0044FFC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_EventOffer` | `double* HumanAI_EventOffer(void * context, double** arg2)` | `0x00450110` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_EventQueued` | `int* HumanAI_EventQueued(void * context, int* arg2)` | `0x00450520` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_LoadScript` | `int* HumanAI_LoadScript(int* context)` | `0x004505C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_OnEvent` | `int HumanAI_OnEvent(int* context)` | `0x004505D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Action` | `int HumanAI_Action(void * context)` | `0x00450770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Cleanup` | `int HumanAI_Cleanup(void * context)` | `0x004507D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_453630` | `void* HumanAI_GetContext_453630(void* arg1)` | `0x00453630` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke33` | `int* HumanAI_Invoke33(int* arg1)` | `0x00453650` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke4` | `int* HumanAI_Invoke4(int* arg1)` | `0x00453670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke4_453680` | `void* HumanAI_Invoke4_453680(void* arg1)` | `0x00453680` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_453AE0` | `void* HumanAI_ExecutePipeline_453AE0(void* arg1, int arg2)` | `0x00453AE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_453CB0` | `void* HumanAI_ExecutePipeline_453CB0(void* arg1, int arg2)` | `0x00453CB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_453E50` | `void* HumanAI_ExecutePipeline_453E50(void* arg1, int arg2)` | `0x00453E50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation` | `void* HumanAI_InvokeStateAnimation(void* arg1, int arg2)` | `0x00454220` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_454330` | `void* HumanAI_InvokeStateAnimation_454330(void* arg1, int arg2)` | `0x00454330` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_454410` | `void* HumanAI_InvokeStateAnimation_454410(void* arg1, int arg2)` | `0x00454410` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_FindAnimation` | `void* HumanAI_FindAnimation(void* context, void* arg2)` | `0x004547B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierFemale_Init` | `int* HumanSoldierFemale_Init(void)` | `0x00454970` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke17` | `int* HumanAI_Invoke17(void* arg1, int arg2)` | `0x00454B10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_VerifyAnimation` | `int HumanAI_VerifyAnimation(int context)` | `0x00454EE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_PlayAnimation` | `int* HumanAI_PlayAnimation(int* context)` | `0x00454F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierFemale_Setup` | `int HumanSoldierFemale_Setup(int* context, int arg2)` | `0x00454F10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeSpeedZMS2F` | `int HumanAI_InvokeSpeedZMS2F(int arg1, void* arg2)` | `0x00455070` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierFemale_Process` | `int HumanSoldierFemale_Process(float context, int* arg2)` | `0x004553E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierFemale_Update` | `unsigned int HumanSoldierFemale_Update(void * context, float arg2)` | `0x00455570` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierFemale_OnEvent` | `int HumanSoldierFemale_OnEvent(void * context, float* arg2)` | `0x00455780` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_455A40` | `void* HumanAI_ExecutePipeline_455A40(void* arg1, int arg2)` | `0x00455A40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_455BE0` | `void* HumanAI_ExecutePipeline_455BE0(void* arg1, int arg2)` | `0x00455BE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_455D80` | `void* HumanAI_ExecutePipeline_455D80(void* arg1, int arg2)` | `0x00455D80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_456010` | `void* HumanAI_InvokeStateAnimation_456010(void* arg1, int arg2)` | `0x00456010` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_4560F0` | `void* HumanAI_InvokeStateAnimation_4560F0(void* arg1, int arg2)` | `0x004560F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_4561A0` | `void* HumanAI_InvokeStateAnimation_4561A0(void* arg1, int arg2)` | `0x004561A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_456250` | `void* HumanAI_InvokeStateAnimation_456250(void* arg1, int arg2)` | `0x00456250` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_4565A0` | `void* HumanAI_InvokeStateAnimation_4565A0(void* arg1, int arg2)` | `0x004565A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ValidateAnimEntry` | `void* HumanAI_ValidateAnimEntry(void* context, void* arg2)` | `0x00456920` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierRPG_Init_2` | `void* HumanSoldierRPG_Init_2(void* context, void* arg2)` | `0x00456A10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierRPG_Init` | `int* HumanSoldierRPG_Init(void)` | `0x00456AD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke17_456C70` | `int HumanAI_Invoke17_456C70(void* arg1, int arg2)` | `0x00456C70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_456CF0` | `int HumanAI_ExecutePipeline_456CF0(void* arg1, void* arg2)` | `0x00456CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ResetAnimation` | `int* HumanAI_ResetAnimation(int* context)` | `0x00456E40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Process` | `int HumanSoldier_Process(int* context, int arg2)` | `0x00456E50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke2` | `int HumanAI_Invoke2(int arg1, void* arg2)` | `0x00456FB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierRPG_Process` | `int HumanSoldierRPG_Process(void * context, int* arg2)` | `0x004572D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierRPG_Update` | `unsigned int HumanSoldierRPG_Update(void * context, float arg2)` | `0x00457430` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierRPG_OnEvent` | `int HumanSoldierRPG_OnEvent(void * context, float* arg2)` | `0x00457620` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_457A00` | `void* HumanAI_ExecutePipeline_457A00(void* arg1, int arg2)` | `0x00457A00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_457D30` | `void* HumanAI_ExecutePipeline_457D30(void* arg1, int arg2)` | `0x00457D30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_457FC0` | `void* HumanAI_ExecutePipeline_457FC0(void* arg1, int arg2)` | `0x00457FC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_4582C0` | `void* HumanAI_ExecutePipeline_4582C0(void* arg1, int arg2)` | `0x004582C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458810` | `void* HumanAI_InvokeStateAnimation_458810(void* arg1, int arg2)` | `0x00458810` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458950` | `void* HumanAI_InvokeStateAnimation_458950(void* arg1, int arg2)` | `0x00458950` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458A30` | `void* HumanAI_InvokeStateAnimation_458A30(void* arg1, int arg2)` | `0x00458A30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458B10` | `void* HumanAI_InvokeStateAnimation_458B10(void* arg1, int arg2)` | `0x00458B10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_458BF0` | `void* HumanAI_ExecutePipeline_458BF0(void* arg1, int arg2)` | `0x00458BF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458DD0` | `void* HumanAI_InvokeStateAnimation_458DD0(void* arg1, int arg2)` | `0x00458DD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_458EB0` | `int HumanAI_InvokeStateAnimation_458EB0(void* arg1, void* arg2)` | `0x00458EB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_459040` | `void* HumanAI_InvokeStateAnimation_459040(void* arg1, int arg2)` | `0x00459040` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_CheckAnimProgress` | `void* HumanAI_CheckAnimProgress(void* context, void* arg2)` | `0x00459680` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierPistol_Init` | `int* HumanSoldierPistol_Init(void)` | `0x00459880` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke17_459A20` | `int* HumanAI_Invoke17_459A20(void* arg1, int arg2)` | `0x00459A20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Setup` | `int HumanSoldier_Setup(void * context)` | `0x00459EC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_459F00` | `int* HumanAI_GetContext_459F00(int* arg1)` | `0x00459F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke3` | `int HumanAI_Invoke3(int arg1, void* arg2)` | `0x00459F10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldierPistol_Process` | `int HumanSoldierPistol_Process(float context, int* arg2)` | `0x0045A230` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_OnEvent` | `unsigned int HumanSoldier_OnEvent(void * context, float arg2)` | `0x0045A3D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Action` | `int HumanSoldier_Action(void * context, float* arg2)` | `0x0045A5E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ExecutePipeline_45AD10` | `void* HumanAI_ExecutePipeline_45AD10(void* arg1, int arg2)` | `0x0045AD10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45B6E0` | `void* HumanAI_InvokeStateAnimation_45B6E0(void* arg1, int arg2)` | `0x0045B6E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45B820` | `void* HumanAI_InvokeStateAnimation_45B820(void* arg1, int arg2)` | `0x0045B820` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45B900` | `void* HumanAI_InvokeStateAnimation_45B900(void* arg1, int arg2)` | `0x0045B900` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45B9C0` | `void* HumanAI_InvokeStateAnimation_45B9C0(void* arg1, int arg2)` | `0x0045B9C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45BA80` | `void* HumanAI_InvokeStateAnimation_45BA80(void* arg1, int arg2)` | `0x0045BA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45BB40` | `void* HumanAI_InvokeStateAnimation_45BB40(void* arg1, int arg2)` | `0x0045BB40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45BDC0` | `void* HumanAI_InvokeStateAnimation_45BDC0(void* arg1, int arg2)` | `0x0045BDC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45BED0` | `void* HumanAI_InvokeStateAnimation_45BED0(void* arg1, int arg2)` | `0x0045BED0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45BF90` | `void* HumanAI_InvokeStateAnimation_45BF90(void* arg1, int arg2)` | `0x0045BF90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeStateAnimation_45C0C0` | `int HumanAI_InvokeStateAnimation_45C0C0(void* arg1, void* arg2)` | `0x0045C0C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ProcessAnimFrame` | `void* HumanAI_ProcessAnimFrame(void* context, void* arg2)` | `0x0045C700` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_CompleteAnimation` | `void* HumanAI_CompleteAnimation(void* context)` | `0x0045C740` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Init_2` | `void* HumanSoldier_Init_2(void* context, void* arg2)` | `0x0045C9B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Update` | `void* HumanSoldier_Update(void* context, int arg2)` | `0x0045CA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_Init` | `int* HumanSoldier_Init(void)` | `0x0045CAB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke17_45CC50` | `int* HumanAI_Invoke17_45CC50(void* arg1, int arg2)` | `0x0045CC50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanSoldier_LoadParameters` | `int HumanSoldier_LoadParameters(void * context)` | `0x0045CF90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_45D030` | `int* HumanAI_GetContext_45D030(int* arg1)` | `0x0045D030` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke4_45D040` | `int HumanAI_Invoke4_45D040(int arg1, void* arg2)` | `0x0045D040` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_DamageHandler` | `int Human_DamageHandler(void * context, float arg2)` | `0x0045D3C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayerInput_Init` | `int* HumanPlayerInput_Init(void)` | `0x0045DB70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_45DCB0` | `int* HumanAI_GetContext_45DCB0(void* arg1)` | `0x0045DCB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayerInput_LoadParameters` | `int HumanPlayerInput_LoadParameters(long double context, void * arg2)` | `0x0045DCF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayerInput_Setup` | `char * HumanPlayerInput_Setup(void * context)` | `0x0045DE20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_45DF40` | `int* HumanAI_GetContext_45DF40(int* arg1)` | `0x0045DF40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayerInput_Process` | `int HumanPlayerInput_Process(void * context)` | `0x0045DF50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Create` | `char* Human_Create(char* context)` | `0x0045E3D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Action` | `void Human_Action(void * context, char * arg2)` | `0x0045E3E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Init` | `int Human_Init(void)` | `0x0045E450` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_GetCutSceneDeltaTime` | `int Human_GetCutSceneDeltaTime(int context)` | `0x0045EAE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeQtasklistIsFull` | `int* HumanAI_InvokeQtasklistIsFull(void* arg1)` | `0x0045EB30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeFindType` | `char* HumanAI_InvokeFindType(int* arg1)` | `0x0045EEA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke4_45EF40` | `int HumanAI_Invoke4_45EF40(void* arg1)` | `0x0045EF40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Setup` | `int* Human_Setup(float context, int arg2)` | `0x0045F050` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_LoadParameters` | `unsigned int Human_LoadParameters(float context, int arg2)` | `0x0045F280` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Process` | `char * Human_Process(float context, int* arg2)` | `0x0045F500` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Update` | `char * Human_Update(float context, int* arg2)` | `0x0045F800` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeMissing` | `int* HumanAI_InvokeMissing(int* arg1)` | `0x0045F880` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_OnEvent` | `float* Human_OnEvent(float context)` | `0x0045F890` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_45F900` | `int HumanAI_GetContext_45F900(int arg1, int* arg2)` | `0x0045F900` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_45F910` | `int HumanAI_GetContext_45F910(int arg1, int* arg2)` | `0x0045F910` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetParam_45F920` | `char* HumanAI_GetParam_45F920(char* arg1)` | `0x0045F920` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_BaseDamage` | `int Human_BaseDamage(void * context, int* arg2)` | `0x0045F930` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup` | `int Human_Cleanup(void * context, float arg2)` | `0x0045F9D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_2` | `void * Human_Cleanup_2(void * context, int* arg2)` | `0x0045FA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_3` | `int Human_Cleanup_3(void * context, void * arg2)` | `0x0045FCD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_InvokeCheckHearing` | `void* HumanAI_InvokeCheckHearing(void* arg1, void* arg2)` | `0x0045FDA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke22` | `int HumanAI_Invoke22(void* arg1, int* arg2)` | `0x0045FE30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_4` | `void Human_Cleanup_4(void * context, int* arg2)` | `0x0045FE70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke12` | `int HumanAI_Invoke12(int arg1)` | `0x00460070` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ProcessStep_460090` | `int* HumanAI_ProcessStep_460090(void* arg1, int* arg2)` | `0x00460090` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ProcessStep_4600C0` | `int* HumanAI_ProcessStep_4600C0(void* arg1, int* arg2)` | `0x004600C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ProcessStep_4601A0` | `void* HumanAI_ProcessStep_4601A0(void* arg1, double* arg2)` | `0x004601A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_ProcessStep_460210` | `void* HumanAI_ProcessStep_460210(void* arg1, double* arg2)` | `0x00460210` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_5` | `double* Human_Cleanup_5(float context)` | `0x00460280` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_4606D0` | `int* HumanAI_GetContext_4606D0(int arg1, int* arg2)` | `0x004606D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_6` | `int* Human_Cleanup_6(void * context, int* arg2)` | `0x004606F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_7` | `int Human_Cleanup_7(void * context)` | `0x00460850` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_8` | `int Human_Cleanup_8(void * context)` | `0x004608D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_9` | `int Human_Cleanup_9(void * context, int arg2)` | `0x00460950` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_10` | `int Human_Cleanup_10(void * context, int arg2)` | `0x004609A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke18` | `int HumanAI_Invoke18(int arg1, bool* arg2)` | `0x004609F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke18_460A10` | `double* HumanAI_Invoke18_460A10(int* arg1)` | `0x00460A10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke18_460A70` | `double* HumanAI_Invoke18_460A70(int* arg1)` | `0x00460A70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Cleanup_11` | `int Human_Cleanup_11(void * context)` | `0x00460AC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_MovementIntegrator` | `int Human_MovementIntegrator(void * context)` | `0x004614B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_WallSweep` | `int Human_WallSweep(void * context, double* arg2, int* arg3, float* arg4, int* arg5)` | `0x00461A70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_GroundQuery` | `int* Human_GroundQuery(void * context, double* arg2, int* arg3, int* arg4, float arg5)` | `0x00462390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_Separation` | `float* Human_Separation(double* context, double* arg2, float* arg3)` | `0x00462BA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_AttachedMovement` | `int Human_AttachedMovement(void * context)` | `0x00462CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_IsCrouched` | `std::uint8_t __cdecl Human_IsCrouched(void* human);` | `0x00463180` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_SetParent` | `void * Human_SetParent(void * context, void * arg2)` | `0x00463310` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanView_Init` | `int HumanView_Init(void)` | `0x00464260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke22_4643A0` | `void* HumanAI_Invoke22_4643A0(void* arg1)` | `0x004643A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanView_DeleteHandler` | `int HumanView_LoadParameters(void * context)` | `0x00464470` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanView_Setup` | `int* HumanView_Setup(void * context, int arg2)` | `0x004644A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanView_Process` | `int HumanView_Process(void * context, int* arg2)` | `0x00464580` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_Invoke005011` | `int* HumanAI_Invoke005011(int* arg1)` | `0x004645B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAI_GetContext_4645D0` | `int* HumanAI_GetContext_4645D0(int arg1, int* arg2)` | `0x004645D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_CameraEyePosition` | `void __cdecl Human_CameraEyePosition(void* out, void* human);` | `0x00465820` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAttach_Init` | `int HumanAttach_Init(void)` | `0x00465A10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAttach_LoadParameters` | `int HumanAttach_LoadParameters(void * context, int arg2)` | `0x00465B00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanAttach_OnEvent` | `unsigned int HumanAttach_OnEvent(void)` | `0x00465BB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Task_ClearPendingWord_468BB0` | `unestablished` | `0x00468BB0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00468BB0-semantic-review.json` |
| `Sys_GetGlobal8_46AB20` | `unestablished` | `0x0046AB20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetPointer5BE2B0_46B040` | `unestablished` | `0x0046B040` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetBlock5BDDB0_46B4D0` | `unestablished` | `0x0046B4D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x0046B4D0-semantic-review.json` |
| `Sys_GetGlobal16_477BF0` | `unestablished` | `0x00477BF0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_47B100` | `unestablished` | `0x0047B100` | 1.000 | CFG 1.000; bn+r2 agree |
| `DefineAmmoType` | `int32_t DefineAmmoType(long param_1, int32_t param_2, float param_3);` | `0x0047CAD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal8_47E6F0` | `unestablished` | `0x0047E6F0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal16_481340` | `unestablished` | `0x00481340` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_481350` | `unestablished` | `0x00481350` | 1.000 | CFG 1.000; bn+r2 agree |
| `HumanCamera_Init` | `char * HumanCamera_Init(void)` | `0x004825C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Human_FindNextCamera` | `void * Human_FindNextCamera(void * context)` | `0x004828D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanCamera_CleanupList` | `void* HumanCamera_CleanupList(void* context)` | `0x00484CE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineMission` | `int32_t DefineMission(int32_t param_1, float param_2);` | `0x00484F60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal8_487190` | `unestablished` | `0x00487190` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetBlock5C8A4C_4871B0` | `unestablished` | `0x004871B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Task_WalkAndDispatch_488700` | `unestablished` | `0x00488700` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00488700-semantic-review.json` |
| `Sys_GetGlobal8_489610` | `unestablished` | `0x00489610` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_489620` | `unestablished` | `0x00489620` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_489630` | `unestablished` | `0x00489630` | 1.000 | CFG 1.000; bn+r2 agree |
| `Human_StateAnimation` | `int Human_StateAnimation(void * context, int arg2, float arg3)` | `0x00489E70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GetCommandLine` | `char const* __cdecl(void)` | `0x0048F080` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `GetInstanceHandle` | `unestablished` | `0x0048F090` | 1.000 | CFG 1.000; bn+r2 agree |
| `GetWindowHandle` | `unestablished` | `0x0048F0A0` | 1.000 | CFG 1.000; bn+r2 agree |
| `GetAppTitle` | `char const* __cdecl(void)` | `0x0048F0B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `SetWindowHandle` | `x86 cdecl void*(void*)` | `0x0048F0E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `App_ContextGetDebugged` | `int __cdecl App_ContextGetDebugged(void);` | `0x0048F1B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_GetDebugCompileState` | `int __cdecl Script_GetDebugCompileState(void);` | `0x0048F1D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `App_ContextGetLightmapsUsed` | `int __cdecl App_ContextGetLightmapsUsed(void);` | `0x0048F230` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `App_ContextGetTerrainLightmapsUsed` | `int __cdecl App_ContextGetTerrainLightmapsUsed(void);` | `0x0048F250` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Input_GetInstance` | `unestablished` | `0x0048F270` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_GetForcePrecompiledState` | `int __cdecl Script_GetForcePrecompiledState(void);` | `0x0048F340` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Loop_SetStatusByte` | `std::uint32_t __cdecl Loop_SetStatusByte(std::uint8_t value); returns the stored byte; retail defines only AL, candidate zero-extends` | `0x0048F480` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `WinProc_RegisterHandler` | `unestablished` | `0x0048F490` | 1.000 | CFG 1.000; bn+r2 agree |
| `WinProc_RemoveHandler` | `unestablished` | `0x0048F4F0` | 1.000 | CFG 1.000; bn+r2 agree |
| `WinMain` | `int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)` | `0x0048F530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `WindowProcedure` | `unestablished` | `0x0048F970` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DirectInput_ErrorText` | `unestablished` | `0x0048FA80` | 0.939 | CFG 0.939; bn+r2 agree |
| `Input_MouseGetCaps` | `unestablished` | `0x0048FD90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mouse_WinProc` | `unestablished` | `0x0048FED0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Keyboard_WinProc` | `unestablished` | `0x00490120` | 1.000 | CFG 1.000; bn+r2 agree |
| `Keyboard_VkToStateIndex` | `unestablished` | `0x00490190` | 1.000 | CFG 1.000; bn+r2 agree |
| `Keyboard_Update` | `unestablished` | `0x00490230` | 1.000 | CFG 1.000; bn+r2 agree |
| `Display_SetVSyncFlag` | `unestablished` | `0x00490340` | 1.000 | CFG 1.000; bn+r2 agree |
| `Display_SetSwapFlag` | `unestablished` | `0x00490350` | 1.000 | CFG 1.000; bn+r2 agree |
| `Display_SetGammaRamp` | `void __stdcall Display_SetGammaRamp(void);` | `0x004903A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_ComputeScreenAspect` | `x86 cdecl double()` | `0x00491D40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_SetGammaRamp_4` | `unestablished` | `0x00491DB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_CreateSurface_4` | `unestablished` | `0x00491DC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_OpenDevice_5` | `unestablished` | `0x00491DD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_CloseDevice_5` | `unestablished` | `0x00491DE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_SetDisplayMode_5` | `unestablished` | `0x00491DF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_SyncVSync_5` | `unestablished` | `0x00491E00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_SwapBuffers_5` | `unestablished` | `0x00491E10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_ClearSurfaces_5` | `unestablished` | `0x00491E20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_RestoreDevice_5` | `unestablished` | `0x00491E30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_CheckCapabilities_5` | `unestablished` | `0x00491E40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_SetGammaRamp_5` | `unestablished` | `0x00491E50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Display_CreateSurface_5` | `unestablished` | `0x00491E60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal32_4950E0` | `unestablished` | `0x004950E0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Transcontext_BuildViewBlock` | `x86 cdecl int32_t(void*, const void*, const void*, float)` | `0x00497D70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FP_InitExtendedPrecision` | `void __cdecl Display_ClearSurfaces_16(void);` | `0x00498460` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DisplayRender_Processmodelsnotconvert` | `int* DisplayRender_Processmodelsnotconvert(int* arg1)` | `0x0049DF60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DisplayRender_Processmodelsnotconvert_4A14B0` | `void* DisplayRender_Processmodelsnotconvert_4A14B0(void* arg1, double* arg2)` | `0x004A14B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyHandiCam` | `uint32_t Camera_ApplyHandiCam(void);` | `0x004A23D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix` | `unestablished` | `0x004A2600` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateFogPalette` | `unestablished` | `0x004A26C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyHandiCam_3` | `int32_t Camera_ApplyHandiCam_3(void);` | `0x004A50F5` | 0.907 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateViewCone_8` | `void* __stdcall Camera_UpdateViewCone_8(int32_t* param_1);` | `0x004A68A9` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_FreeBlock` | `unestablished` | `0x004A6A50` | 1.000 | CFG 1.000; bn+r2 agree |
| `FP_SetDefaultPrecision` | `unestablished` | `0x004A7157` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `String_FindCharacter_4AA0A0` | `unestablished` | `0x004AA0A0` | 0.014 | CFG 0.014; bn+r2 agree |
| `Mem_AllocChunk` | `void* __cdecl Mem_AllocChunk(std::uint32_t size);` | `0x004AA670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_AllocWithRetry` | `void* __cdecl Mem_AllocWithRetry(std::uint32_t size, std::int32_t retry);` | `0x004AA682` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_AllocateRawBlock` | `unestablished` | `0x004AA6AE` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_MatchPrefix` | `int __cdecl Resource_MatchPrefix(const char* a, const char* b, std::uint32_t n);` | `0x004AAD10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Pool_Init` | `unestablished` | `0x004AAD48` | 1.000 | CFG 1.000; bn+r2 agree |
| `Pool_FindBlock` | `unestablished` | `0x004AAD90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_FreeSmallBlock` | `unestablished` | `0x004AADBB` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_FreeBucketNode` | `unestablished` | `0x004AB6DD` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_PurgeBucketPages` | `unestablished` | `0x004AB733` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_FindBucketBlock` | `unestablished` | `0x004AB7F5` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_MarkBucketPage` | `unestablished` | `0x004AB84C` | 1.000 | CFG 1.000; bn+r2 agree |
| `FP_MergeControlWord` | `unestablished` | `0x004AC620` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FP_MergeControlWordNoDenormal` | `unestablished` | `0x004AC655` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FP_ControlWordToFlags` | `unestablished` | `0x004AC66B` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FP_FlagsToControlWord` | `unestablished` | `0x004AC6FD` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_ReturnZero_4AD36E` | `unestablished` | `0x004AD36E` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AD36E-semantic-review.json` |
| `Mem_InvokeNewHandler` | `int __cdecl Mem_InvokeNewHandler(std::uint32_t size);` | `0x004AD722` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `String_ToUpperChar` | `int __cdecl String_ToUpperChar(int ch);` | `0x004AD73D` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_29` | `void __stdcall Camera_ResetMatrix_29(void* param_1, void* param_2, EXCEPTION_RECORD* param_3, void* param_4);` | `0x004AE670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DirectInputCreateEx` | `unestablished` | `0x004AE730` | 1.000 | CFG 1.000; bn+r2 agree |
| `DirectInputCreateA` | `unestablished` | `0x004AF740` | 1.000 | CFG 1.000; bn+r2 agree |
| `Camera_ApplyHandiCam_30` | `int32_t Camera_ApplyHandiCam_30(void);` | `0x004AF746` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyWideScreen_30` | `HRESULT __stdcall Camera_ApplyWideScreen_30(LPDDENUMCALLBACKEXA param_1, void* param_2, uint32_t param_3);` | `0x004AF74C` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyPostProcess_30` | `HRESULT __stdcall Camera_ApplyPostProcess_30(GUID* param_1, void** param_2, GUID* param_3, struct param_4);` | `0x004AF752` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateViewCone_30` | `void __stdcall Camera_UpdateViewCone_30(void);` | `0x004AF758` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_30` | `void __stdcall Camera_ResetMatrix_30(void);` | `0x004AF75E` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateTransform_31` | `uint32_t __stdcall Camera_UpdateTransform_31(struct param_1);` | `0x004AF764` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_SetFieldOfView_31` | `uint32_t __stdcall Camera_SetFieldOfView_31(struct param_1);` | `0x004AF76A` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyLineFilter_31` | `LRESULT __stdcall Camera_ApplyLineFilter_31(HIC param_1);` | `0x004AF770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyBandFilter_31` | `LRESULT __stdcall Camera_ApplyBandFilter_31(HIC param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4);` | `0x004AF776` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyHandiCam_31` | `HRESULT __stdcall Camera_ApplyHandiCam_31(struct param_1, int32_t param_2, void* param_3, int32_t* param_4);` | `0x004AF77C` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyWideScreen_31` | `HRESULT __stdcall Camera_ApplyWideScreen_31(struct param_1, struct param_2, int32_t param_3);` | `0x004AF782` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyPostProcess_31` | `HRESULT __stdcall Camera_ApplyPostProcess_31(struct param_1, struct param_2, uint32_t param_3, int32_t param_4);` | `0x004AF788` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateViewCone_31` | `HRESULT __stdcall Camera_UpdateViewCone_31(struct param_1, struct param_2, int32_t param_3);` | `0x004AF78E` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_31` | `HRESULT __stdcall Camera_ResetMatrix_31(struct param_1, PSTR param_2, uint32_t param_3, GUID* param_4);` | `0x004AF794` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateTransform_32` | `HIC __stdcall Camera_UpdateTransform_32(uint32_t param_1, uint32_t param_2, BITMAPINFOHEADER* param_3, BITMAPINFOHEADER* param_4, uint16_t param_5);` | `0x004AF79A` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_SetFieldOfView_32` | `uint32_t __stdcall Camera_SetFieldOfView_32(HIC param_1, uint32_t param_2, BITMAPINFOHEADER* param_3, void* param_4, BITMAPINFOHEADER* param_5, void* param_6);` | `0x004AF7A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyLineFilter_32` | `HRESULT __stdcall Camera_ApplyLineFilter_32(struct param_1, int32_t param_2, int32_t param_3, void* param_4, int32_t param_5, int32_t* param_6, int32_t* param_7);` | `0x004AF7A6` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_WarningLog` | `unestablished` | `0x004AF810` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_StoreWord_4AF860` | `unestablished` | `0x004AF860` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AF860-semantic-review.json` |
| `Error_CallbackSet_4AF870` | `unestablished` | `0x004AF870` | 1.000 | CFG 1.000; bn+r2 agree |
| `Error_CallbackSet_4AF880` | `unestablished` | `0x004AF880` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_StoreWord_4AF890` | `unestablished` | `0x004AF890` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AF890-semantic-review.json` |
| `Sys_StoreWord_4AF8A0` | `unestablished` | `0x004AF8A0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AF8A0-semantic-review.json` |
| `Sys_StoreWord_4AF8B0` | `unestablished` | `0x004AF8B0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AF8B0-semantic-review.json` |
| `Script_QHashListInsert` | `unestablished` | `0x004AF910` | 1.000 | CFG 1.000; bn+r2 agree |
| `HeapList_Remove` | `void** __cdecl HeapList_Remove(void** node);` | `0x004AF960` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_AllocIndexedBlock` | `unestablished` | `0x004B0B00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QFile_ReleaseBlock_4B0B70` | `unestablished` | `0x004B0B70` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0B70-semantic-review.json` |
| `Sys_ReadFileBlock_2` | `int32_t Sys_ReadFileBlock_2(void);` | `0x004B0B90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_InitHeapState` | `unestablished` | `0x004B0BA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_DeAllocFn` | `unestablished` | `0x004B0D10` | 1.000 | CFG 1.000; bn+r2 agree |
| `QHash_ValueSet` | `unestablished` | `0x004B0D40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Mem_QHashFlagClear` | `unestablished` | `0x004B0D50` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_FlagStackPop` | `unestablished` | `0x004B0D60` | 1.000 | CFG 1.000; bn+r2 agree |
| `Mem_FlagStackPush` | `unestablished` | `0x004B0D90` | 1.000 | CFG 1.000; bn+r2 agree |
| `QFile_GetByte_4B0DB0` | `unestablished` | `0x004B0DB0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0DB0-semantic-review.json` |
| `Sys_ResetRenderTables_4B0DC0` | `unestablished` | `0x004B0DC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0DC0-semantic-review.json` |
| `Sys_AddRenderMode_4B0E00` | `unestablished` | `0x004B0E00` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0E00-semantic-review.json` |
| `Sys_ClearRenderModeSlot_4B0E50` | `unestablished` | `0x004B0E50` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0E50-semantic-review.json` |
| `Sys_AddSlot_4B0E70` | `unestablished` | `0x004B0E70` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0E70-semantic-review.json` |
| `Sys_ClearSlotByte_4B0EB0` | `unestablished` | `0x004B0EB0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0EB0-semantic-review.json` |
| `Sys_SetSlotWord_4B0EC0` | `unestablished` | `0x004B0EC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0EC0-semantic-review.json` |
| `Sys_EnumRenderModes_4B0EE0` | `unestablished` | `0x004B0EE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0EE0-semantic-review.json` |
| `Sys_SelectRenderMode_4B0F30` | `unestablished` | `0x004B0F30` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0F30-semantic-review.json` |
| `Sys_ResetSlotWord_4B0F70` | `unestablished` | `0x004B0F70` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0F70-semantic-review.json` |
| `Texture_GetActiveBackendId` | `std::int32_t __cdecl Texture_GetActiveBackendId(void);` | `0x004B0F80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_EnterRenderMode_4B0F90` | `unestablished` | `0x004B0F90` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0F90-semantic-review.json` |
| `Sys_LeaveRenderMode_4B0FD0` | `unestablished` | `0x004B0FD0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B0FD0-semantic-review.json` |
| `Sys_InvokeSlotWord_4B1000` | `unestablished` | `0x004B1000` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1000-semantic-review.json` |
| `QFile_AliasResolve_4B1020` | `unestablished` | `0x004B1020` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1020-semantic-review.json` |
| `QFile_AllocateDevice_4B1180` | `unestablished` | `0x004B1180` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1180-semantic-review.json` |
| `QFile_DeviceIndex_4B11A0` | `unestablished` | `0x004B11A0` | 1.000 | CFG 1.000; bn+r2 agree |
| `QFile_SetDeviceName_4B1290` | `unestablished` | `0x004B1290` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1290-semantic-review.json` |
| `QFile_ClearTable_4B12D0` | `unestablished` | `0x004B12D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B12D0-semantic-review.json` |
| `QFile_RegisterAlias_4B12F0` | `unestablished` | `0x004B12F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B12F0-semantic-review.json` |
| `QFile_ClearEntryByName_4B13A0` | `unestablished` | `0x004B13A0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B13A0-semantic-review.json` |
| `QFile_DispatchDevicePath_4B1420` | `unestablished` | `0x004B1420` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1420-semantic-review.json` |
| `QFile_LookupDeviceByName_4B1620` | `unestablished` | `0x004B1620` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1620-semantic-review.json` |
| `Device_CloseStream` | `int __cdecl Device_CloseStream(void* stream);` | `0x004B1690` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QFile_DeviceSlot10Call_4B16D0` | `unestablished` | `0x004B16D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B16D0-semantic-review.json` |
| `QFile_InvokeDriverSlot_4B1700` | `unestablished` | `0x004B1700` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1700-semantic-review.json` |
| `QFile_InvokeDriverSlot_4B1730` | `unestablished` | `0x004B1730` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1730-semantic-review.json` |
| `QFile_InvokeDriverSlot_4B1760` | `unestablished` | `0x004B1760` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1760-semantic-review.json` |
| `Device_GetStreamPosition` | `int __cdecl Device_GetStreamPosition(void* stream);` | `0x004B1780` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QFile_InvokeDriverSlot_4B17A0` | `unestablished` | `0x004B17A0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B17A0-semantic-review.json` |
| `QFile_InvokeDriverSlot_4B17C0` | `unestablished` | `0x004B17C0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B17C0-semantic-review.json` |
| `Device_SeekStream` | `int __cdecl Device_SeekStream(void* stream, std::int32_t offset);` | `0x004B17F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QFile_CreateLoadedFileRecord_4B19C0` | `unestablished` | `0x004B19C0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B19C0-semantic-review.json` |
| `QFile_FindRecordAtAddress_4B1A90` | `unestablished` | `0x004B1A90` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1A90-semantic-review.json` |
| `QFile_RegisterDevice_4B1B90` | `unestablished` | `0x004B1B90` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1B90-semantic-review.json` |
| `QFile_InvokeDriverSlot_4B1BC0` | `unestablished` | `0x004B1BC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1BC0-semantic-review.json` |
| `QFile_SetDriverField_4B1BE0` | `unestablished` | `0x004B1BE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1BE0-semantic-review.json` |
| `QFile_SetDriverField_4B1C00` | `unestablished` | `0x004B1C00` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1C00-semantic-review.json` |
| `QFile_SetDriverField_4B1C20` | `unestablished` | `0x004B1C20` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1C20-semantic-review.json` |
| `QFile_SetDriverField_4B1C40` | `unestablished` | `0x004B1C40` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1C40-semantic-review.json` |
| `QFile_SetDriverField_4B1C60` | `unestablished` | `0x004B1C60` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1C60-semantic-review.json` |
| `QFile_SetDriverField_4B1C80` | `unestablished` | `0x004B1C80` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1C80-semantic-review.json` |
| `QFile_SetDriverField_4B1CA0` | `unestablished` | `0x004B1CA0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1CA0-semantic-review.json` |
| `QFile_SetDriverField_4B1CC0` | `unestablished` | `0x004B1CC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1CC0-semantic-review.json` |
| `QFile_SetDriverField_4B1CE0` | `unestablished` | `0x004B1CE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1CE0-semantic-review.json` |
| `QFile_SetDriverField_4B1D00` | `unestablished` | `0x004B1D00` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1D00-semantic-review.json` |
| `QFile_SetDriverField_4B1D20` | `unestablished` | `0x004B1D20` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1D20-semantic-review.json` |
| `QFile_SetDriverField_4B1D40` | `unestablished` | `0x004B1D40` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1D40-semantic-review.json` |
| `QFile_SetDriverField_4B1D60` | `unestablished` | `0x004B1D60` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1D60-semantic-review.json` |
| `QFile_SetDriverField_4B1D80` | `unestablished` | `0x004B1D80` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1D80-semantic-review.json` |
| `QFile_SetDriverHandler_4B1DA0` | `unestablished` | `0x004B1DA0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1DA0-semantic-review.json` |
| `QFile_SetDriverField_4B1DC0` | `unestablished` | `0x004B1DC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1DC0-semantic-review.json` |
| `QFile_GetDeviceName_4B1DE0` | `unestablished` | `0x004B1DE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1DE0-semantic-review.json` |
| `String_FindExtension` | `char* __cdecl String_FindExtension(char* out, int size, const char* path);` | `0x004B1E90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QFile_GetResolvedAlias_4B1FB0` | `unestablished` | `0x004B1FB0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B1FB0-semantic-review.json` |
| `Texture_UploadMipmapPacket` | `unestablished` | `0x004B23B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Texture_LoadLoopTextureFile` | `unestablished` | `0x004B24C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Texture_EnableUpload` | `void __cdecl Texture_EnableUpload(void);` | `0x004B26A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Texture_DisableUpload` | `void __cdecl Texture_DisableUpload(void);` | `0x004B26B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_StoreThreeWords_4B4720` | `unestablished` | `0x004B4720` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B4720-semantic-review.json` |
| `Resource_CreateNode` | `unestablished` | `0x004B59F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_CanonicalizePath` | `void __cdecl Resource_CanonicalizePath(char* dst, const char* src);` | `0x004B5AF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_CalculatePathHash` | `std::uint32_t __cdecl Resource_CalculatePathHash(const char* path);` | `0x004B5B30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_FileExists` | `int __cdecl Resource_FileExists(const char* path);` | `0x004B5B60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_FreeNode` | `unestablished` | `0x004B5C90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_RegisterBuffer` | `unestablished` | `0x004B5F80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_RegisterIlffContainer` | `unestablished` | `0x004B60F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Resource_CreateKind3Node_4B6420` | `unestablished` | `0x004B6420` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B6420-semantic-review.json` |
| `Resource_UploadMipmapPacket_4B64F0` | `unestablished` | `0x004B64F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B64F0-semantic-review.json` |
| `Matrix_InvalidateSlot` | `void __cdecl Matrix_InvalidateSlot(void* context, std::int32_t slot);` | `0x004B7450` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_GetNextChunk` | `void* __cdecl Ilff_GetNextChunk(void* chunk);` | `0x004B7470` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_GetFirstChunk` | `void* __cdecl Ilff_GetFirstChunk(void* container);` | `0x004B7490` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_GetChunkPayload` | `void* __cdecl Ilff_GetChunkPayload(void* chunk, void* container);` | `0x004B74D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_ReadContainerHeader` | `void __cdecl Ilff_ReadContainerHeader(std::uint32_t* outSize, std::uint32_t* outAlign, void* container);` | `0x004B7540` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_GetFormType` | `std::uint32_t __cdecl Ilff_GetFormType(void* container);` | `0x004B7570` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_GetChunkTag` | `std::uint32_t __cdecl Ilff_GetChunkTag(void* chunk);` | `0x004B7580` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_FindChunk` | `void* __cdecl Ilff_FindChunk(void* chunk, std::uint32_t tag);` | `0x004B7590` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Texture_UploadPalettePacket` | `unestablished` | `0x004B7CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_QHashNodeRegister` | `unestablished` | `0x004B8810` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_QHashNodeRegisterEx` | `unestablished` | `0x004B8890` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_SetItemWord_4B8970` | `unestablished` | `0x004B8970` | 1.000 | CFG 1.000; bn+r2 agree |
| `Qvm_SetGlobalState` | `int __cdecl Qvm_SetGlobalState(int state);` | `0x004B8A10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_GetFloatArg` | `extern "C" long double IGI_RETAIL_CDECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index)` | `0x004B8A50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Qvm_IndexBelowCount` | `unestablished` | `0x004B8B00` | 1.000 | CFG 1.000; bn+r2 agree |
| `BreakScript` | `int32_t BreakScript(int32_t param_1);` | `0x004B8C10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `_Script_SetWarningLevel` | `int32_t _Script_SetWarningLevel(int32_t param_1, float param_2);` | `0x004B8C60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_QHashNodeCreate` | `unestablished` | `0x004BAAC0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_QHashSymbolTableInit` | `unestablished` | `0x004BACD0` | 1.000 | CFG 1.000; bn+r2 agree |
| `DynObj_InvokeField18_4BF1E0` | `unestablished` | `0x004BF1E0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004BF1E0-semantic-review.json` |
| `DynObj_GetField0C_4C0230` | `unestablished` | `0x004C0230` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C0230-semantic-review.json` |
| `DynObj_GetField04_4C0240` | `unestablished` | `0x004C0240` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C0240-semantic-review.json` |
| `DynObj_ParseConfigFile` | `int32_t DynObj_ParseConfigFile(void);` | `0x004C0250` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_QHashTableCreate` | `unestablished` | `0x004C0360` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_QHashNodeLookup` | `unestablished` | `0x004C04B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Hash32` | `unestablished` | `0x004C0530` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_QHashSymbolRegister` | `unestablished` | `0x004C0560` | 1.000 | CFG 1.000; bn+r2 agree |
| `DynObj_ParseConfigFile_2` | `int* DynObj_ParseConfigFile_2(int* context)` | `0x004C0660` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_3` | `void ** DynObj_ParseConfigFile_3(void ** context, int* arg2)` | `0x004C0F80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AnimSound_LoadParameters` | `unestablished` | `0x004C1780` | 1.000 | CFG 1.000; bn+r2 agree |
| `DynObj_ParseConfigFile_4` | `int DynObj_ParseConfigFile_4(int* context)` | `0x004C2F90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineMagicObj` | `int32_t DefineMagicObj(int32_t param_1, float param_2);` | `0x004C4520` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_GetByte_4C56D0` | `unestablished` | `0x004C56D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C56D0-semantic-review.json` |
| `DynObj_GetByte_4C56E0` | `unestablished` | `0x004C56E0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C56E0-semantic-review.json` |
| `DynObj_GetByte_4C56F0` | `unestablished` | `0x004C56F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C56F0-semantic-review.json` |
| `DynObj_GetByte_4C5700` | `unestablished` | `0x004C5700` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C5700-semantic-review.json` |
| `DynObj_GetByte_4C5800` | `unestablished` | `0x004C5800` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C5800-semantic-review.json` |
| `QTask_InitPhaseHandlers` | `unestablished` | `0x004C5900` | 1.000 | CFG 1.000; bn+r2 agree |
| `Terrain_LoadTexturesAndLightmaps` | `int32_t __cdecl Terrain_LoadTexturesAndLightmaps(void* context, const char* base_path);` | `0x004C5F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_Load` | `void* __cdecl Terrain_Load(void* context, const char* base_path);` | `0x004C6240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CreateMaterial` | `void* __cdecl Terrain_CreateMaterial(void* context, const float* args);` | `0x004C6670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CreateTileMap` | `void* __cdecl Terrain_CreateTileMap(void* context, const float* args);` | `0x004C6760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LinkDynamicCube` | `int DynObj_LinkDynamicCube_7(int context, int arg2, int arg3, int arg4, int arg5, int arg6);` | `0x004C67E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_8` | `void * DynObj_ParseConfigFile_8(void * context, void * arg2)` | `0x004C6E30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_9` | `unsigned int DynObj_ParseConfigFile_9(float context, double* arg2, void * const arg3, float arg4, double* arg5, double* arg6, char arg7)` | `0x004C8130` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_10` | `int DynObj_ParseConfigFile_10(float context, double* arg2)` | `0x004CB810` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_11` | `unsigned int DynObj_ParseConfigFile_11(int context, char arg2, int arg3, double* arg4, int arg5)` | `0x004CE390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_12` | `int* DynObj_ParseConfigFile_12(int context, int* arg2, int arg3, int arg4)` | `0x004CE910` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_PhaseIdGet548176_4CEA10` | `unestablished` | `0x004CEA10` | 1.000 | CFG 1.000; bn+r2 agree |
| `QTask_PhaseIdGet548177_4CEA20` | `unestablished` | `0x004CEA20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Render_GetActiveBackendId` | `std::int32_t __cdecl Render_GetActiveBackendId(void);` | `0x004CEA80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Ilff_FindFormChunk` | `unestablished` | `0x004CEAA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_MtpNlodvirmodelD` | `void * DynObj_MtpNlodvirmodelD(int context)` | `0x004CEBA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_13` | `int* DynObj_ParseConfigFile_13(char * context)` | `0x004CEE30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_Initmodels` | `int DynObj_Initmodels(void)` | `0x004CEEB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Core_CopyState_4CEF30` | `unestablished` | `0x004CEF30` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004CEF30-semantic-review.json` |
| `DynObj_LoadModelTexturePackage` | `unestablished` | `0x004CF080` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LookupHashItem` | `void* __cdecl DynObj_LookupHashItem(void* table, std::int32_t index);` | `0x004CF300` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadPackageMeshes` | `unestablished` | `0x004CF330` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadModelMesh` | `void* __cdecl DynObj_LoadModelMesh(const char* model_name)` | `0x004CF390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadPackageLovs` | `unestablished` | `0x004CF630` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadModelTextures` | `unestablished` | `0x004CF790` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadPackagePalettes` | `unestablished` | `0x004CF870` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_LoadPackageAnimations` | `unestablished` | `0x004CF910` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_15` | `int DynObj_ParseConfigFile_15(void)` | `0x004CFC30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynObj_ParseConfigFile_16` | `int DynObj_ParseConfigFile_16(float context)` | `0x004CFF70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture` | `void ** Render_BindTexture(void ** context, void * arg2, int arg3, char * arg4)` | `0x004D09E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Model_GetAttachment` | `void* __cdecl Model_GetAttachment(void* model, std::int32_t index);` | `0x004D0CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Model_GetAttachmentCount` | `std::int32_t __cdecl Model_GetAttachmentCount(void* model);` | `0x004D0D10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal32_4D0D80` | `unestablished` | `0x004D0D80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Render_BindTexture_2` | `int Render_BindTexture_2(void * context, int arg2, int arg3)` | `0x004D0DC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_3` | `int Render_BindTexture_3(void)` | `0x004D1260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_GetByte_4D14C0` | `unestablished` | `0x004D14C0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004D14C0-semantic-review.json` |
| `Render_GetByte_4D14D0` | `unestablished` | `0x004D14D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004D14D0-semantic-review.json` |
| `Terrain_TraverseContourQuadtree` | `std::int32_t Terrain_TraverseContourQuadtree(void* context)` | `0x004D14E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_4` | `int Render_BindTexture_4(void)` | `0x004D2C30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_5` | `int Render_BindTexture_5(float* context, float* arg2, int* arg3)` | `0x004D31C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_NoOpHandler` | `unestablished` | `0x004D3710` | 1.000 | CFG 1.000; bn+r2 agree |
| `Render_BindTexture_6` | `int* Render_BindTexture_6(void * context)` | `0x004D3730` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_7` | `int* Render_BindTexture_7(int context)` | `0x004D46E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_8` | `int* Render_BindTexture_8(int* context, int* arg2, int arg3, int arg4, int arg5, int arg6)` | `0x004D4F60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_9` | `int Render_BindTexture_9(float context, int arg2, int arg3, int arg4, int arg5, int arg6, float arg7)` | `0x004D6130` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_10` | `void * Render_BindTexture_10(void * context, int arg2, int arg3)` | `0x004D65B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AnimIff_LoadAnimationFile` | `std::optional<IffSkeleton> IffSkeleton_Read(const std::filesystem::path& path)` | `0x004D67A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_11` | `int Render_BindTexture_11(int* context, int arg2, int arg3)` | `0x004D74F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_VirmodelIsNull` | `int Render_VirmodelIsNull(void * context, int* arg2)` | `0x004D9700` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QObj_Init` | `unestablished` | `0x004D9730` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_4D97E0` | `unestablished` | `0x004D97E0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal8_4D9850` | `unestablished` | `0x004D9850` | 1.000 | CFG 1.000; bn+r2 agree |
| `MagicObj_ValidateVirModel` | `int MagicObj_ValidateVirModel(int context)` | `0x004D99E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsRigid_ProcessVirModelsnotavai` | `int* PhysicsRigid_ProcessVirModelsnotavai(void* arg1, float arg2)` | `0x004D9B10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Hazinglayers_Init` | `unestablished` | `0x004DA1D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_12` | `int Render_BindTexture_12(void)` | `0x004DA880` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_13` | `void Render_BindTexture_13(int* context, void ** arg2)` | `0x004DBF30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_14` | `int Render_BindTexture_14(int context, void * arg2, double* arg3)` | `0x004DCF00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Render_BindTexture_15` | `float Render_BindTexture_15(float context)` | `0x004DEEC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DynCubeObj_Init` | `unestablished` | `0x004E5370` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Core_GetEventWord_4E7530` | `unestablished` | `0x004E7530` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004E7530-semantic-review.json` |
| `CameraControlTask_Init` | `unestablished` | `0x004E7C50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineSound` | `int32_t DefineSound(long param_1, long param_2, long param_3, long param_4, long param_5, long param_6, long param_7, int32_t param_8, void* param_9, float param_10);` | `0x004E8C60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineTriggerOnce` | `int32_t DefineTriggerOnce(long param_1, long param_2, long param_3, long param_4, long param_5, long param_6, long param_7, int32_t param_8, void* param_9, float param_10);` | `0x004E9EC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineGraph` | `int32_t DefineGraph(long param_1, long param_2, long param_3, long param_4, long param_5, long param_6, int32_t param_7, void* param_8, float param_9);` | `0x004EAD20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DefineGroup` | `int32_t DefineGroup(int32_t param_1, void* param_2, float param_3);` | `0x004EBA90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `SoundSysMicrophone_Init` | `unestablished` | `0x004EBC60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `InputPort_Init` | `unestablished` | `0x004ED510` | 1.000 | CFG 1.000; bn+r2 agree |
| `InputPort_Process` | `struct status_msg* InputPort_Process(void * context)` | `0x004ED5C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EDA70` | `void* PhysicsTerrain_GetContext_4EDA70(void* arg1)` | `0x004EDA70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `LODSettings_LoadParameters` | `x86 cdecl void*(void* context, LODSettingsRecord* (*lookup)(const char*))` | `0x004EDB60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EE530` | `void* PhysicsTerrain_GetContext_4EE530(void* arg1)` | `0x004EE530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_ProcessLOCALgrab` | `int PhysicsTerrain_ProcessLOCALgrab()` | `0x004EE540` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_ProcessLOCALgrab_4EE5E0` | `int PhysicsTerrain_ProcessLOCALgrab_4EE5E0(void* arg1)` | `0x004EE5E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke4` | `int* PhysicsTerrain_Invoke4(int* arg1)` | `0x004EE680` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_HashBucket` | `unestablished` | `0x004EE960` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_Invoke5` | `int* PhysicsTerrain_Invoke5(int arg1, int* arg2)` | `0x004EF7F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Handler_4EF830` | `unestablished` | `0x004EF830` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EF850` | `unestablished` | `0x004EF850` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EF8B0` | `unestablished` | `0x004EF8B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EF990` | `unestablished` | `0x004EF990` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EF9B0` | `unestablished` | `0x004EF9B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EF9D0` | `unestablished` | `0x004EF9D0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_Handler_4EFA10` | `unestablished` | `0x004EFA10` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_InvokeDataSymbolLoad` | `int PhysicsTerrain_InvokeDataSymbolLoad(float* arg1, void* arg2)` | `0x004EFA20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Handler_4EFA90` | `unestablished` | `0x004EFA90` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_InvokeTooFewParametersInLi_4EFAA0` | `int* PhysicsTerrain_InvokeTooFewParametersInLi_4EFAA0(int* arg1, int* arg2)` | `0x004EFAA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeTooFewParametersInLi_4EFAB0` | `int PhysicsTerrain_InvokeTooFewParametersInLi_4EFAB0(long double arg1, long double arg2, long double arg3, float* arg4, int* arg5)` | `0x004EFAB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4EFB00` | `unestablished` | `0x004EFB00` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_InvokeDataSymbolLoad_4EFB10` | `int PhysicsTerrain_InvokeDataSymbolLoad_4EFB10(float* arg1, void* arg2)` | `0x004EFB10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFB60` | `int PhysicsTerrain_GetContext_4EFB60(int* arg1, int* arg2)` | `0x004EFB60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFB80` | `int PhysicsTerrain_GetContext_4EFB80(int* arg1, int* arg2)` | `0x004EFB80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Processfffffffff` | `int PhysicsTerrain_Processfffffffff(float* arg1, void* arg2)` | `0x004EFC10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFC90` | `int* PhysicsTerrain_GetContext_4EFC90(int arg1, int* arg2)` | `0x004EFC90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFCB0` | `int* PhysicsTerrain_GetContext_4EFCB0(int arg1, int* arg2)` | `0x004EFCB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4EFCD0` | `unestablished` | `0x004EFCD0` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_ExecutePipeline_4EFD50` | `int PhysicsTerrain_ExecutePipeline_4EFD50(float arg1, float arg2)` | `0x004EFD50` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFEE0` | `int* PhysicsTerrain_GetContext_4EFEE0(int arg1, int* arg2)` | `0x004EFEE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFF00` | `int* PhysicsTerrain_GetContext_4EFF00(int arg1, int* arg2)` | `0x004EFF00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4EFF20` | `unestablished` | `0x004EFF20` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_InvokeDataSymbolLoad_4EFF60` | `int PhysicsTerrain_InvokeDataSymbolLoad_4EFF60(int* arg1, void* arg2)` | `0x004EFF60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFF90` | `int PhysicsTerrain_GetContext_4EFF90(int* arg1, int* arg2)` | `0x004EFF90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4EFFB0` | `int* PhysicsTerrain_GetContext_4EFFB0(int* arg1, int* arg2)` | `0x004EFFB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeTooFewParametersInLi_4EFFD0` | `int PhysicsTerrain_InvokeTooFewParametersInLi_4EFFD0(long double arg1, long double arg2, long double arg3, double* arg4, int* arg5)` | `0x004EFFD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F0020` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F0020(int* arg1, void* arg2)` | `0x004F0020` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0060` | `int* PhysicsTerrain_GetContext_4F0060(int arg1, int* arg2)` | `0x004F0060` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0080` | `int* PhysicsTerrain_GetContext_4F0080(int arg1, int* arg2)` | `0x004F0080` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4F00A0` | `unestablished` | `0x004F00A0` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F00E0` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F00E0(int* arg1, void* arg2)` | `0x004F00E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F0140` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F0140(short* arg1, void* arg2)` | `0x004F0140` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0170` | `int* PhysicsTerrain_GetContext_4F0170(short* arg1, int* arg2)` | `0x004F0170` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0190` | `short* PhysicsTerrain_GetContext_4F0190(short* arg1, int* arg2)` | `0x004F0190` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4F01B0` | `unestablished` | `0x004F01B0` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_Invoke2` | `int* PhysicsTerrain_Invoke2(int* arg1, int* arg2)` | `0x004F0270` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke2_4F02B0` | `int PhysicsTerrain_Invoke2_4F02B0(int* arg1, int* arg2)` | `0x004F02B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeTooFewParametersInLi_4F0320` | `int* PhysicsTerrain_InvokeTooFewParametersInLi_4F0320(int* arg1, int* arg2)` | `0x004F0320` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F03D0` | `int PhysicsTerrain_GetContext_4F03D0(int arg1, int* arg2)` | `0x004F03D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F0430` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F0430(int arg1, void* arg2)` | `0x004F0430` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F04A0` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F04A0(int* arg1, void* arg2)` | `0x004F04A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeDataSymbolLoad_4F0530` | `int PhysicsTerrain_InvokeDataSymbolLoad_4F0530(char* arg1, void* arg2)` | `0x004F0530` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0560` | `int* PhysicsTerrain_GetContext_4F0560(char* arg1, int* arg2)` | `0x004F0560` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F0570` | `char* PhysicsTerrain_GetContext_4F0570(char* arg1, int* arg2)` | `0x004F0570` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_ExecutePipeline_4F0580` | `void* PhysicsTerrain_ExecutePipeline_4F0580(void* arg1)` | `0x004F0580` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetParam_4F0600` | `int PhysicsTerrain_GetParam_4F0600(int arg1)` | `0x004F0600` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Clear_LevelState` | `unestablished` | `0x004F0610` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_ProcessStep_4F0620` | `int PhysicsTerrain_ProcessStep_4F0620(int arg1)` | `0x004F0620` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_ClearStaticState` | `unestablished` | `0x004F0640` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_ExecutePipeline_4F0660` | `void* PhysicsTerrain_ExecutePipeline_4F0660(void* arg1)` | `0x004F0660` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetParam_4F06E0` | `int PhysicsTerrain_GetParam_4F06E0(int arg1)` | `0x004F06E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Clear_DynamicState` | `unestablished` | `0x004F06F0` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_GetParam_4F0700` | `int PhysicsTerrain_GetParam_4F0700(int arg1)` | `0x004F0700` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Clear_IdleState` | `unestablished` | `0x004F0710` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_GetParam_4F0720` | `int PhysicsTerrain_GetParam_4F0720(int arg1)` | `0x004F0720` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_Clear_CameraBaseState` | `unestablished` | `0x004F0730` | 1.000 | CFG 1.000; bn+r2 agree |
| `QHashNode_Release` | `unestablished` | `0x004F0C90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0EF0` | `unestablished` | `0x004F0EF0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F00` | `unestablished` | `0x004F0F00` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F20` | `unestablished` | `0x004F0F20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F30` | `unestablished` | `0x004F0F30` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F40` | `unestablished` | `0x004F0F40` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F60` | `unestablished` | `0x004F0F60` | 1.000 | CFG 1.000; bn+r2 agree |
| `Camera_Get_4F0F80` | `unestablished` | `0x004F0F80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F0F90` | `unestablished` | `0x004F0F90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Core_GetRenderWordA_4F0FA0` | `unestablished` | `0x004F0FA0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0FA0-semantic-review.json` |
| `Sys_GetGlobal32_4F0FB0` | `unestablished` | `0x004F0FB0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Core_GetRenderWordB_4F0FD0` | `unestablished` | `0x004F0FD0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0FD0-semantic-review.json` |
| `Sys_GetGlobal32_4F0FF0` | `unestablished` | `0x004F0FF0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Camera_Get_4F1000` | `unestablished` | `0x004F1000` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F1010` | `unestablished` | `0x004F1010` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_GetParseMode` | `int Script_ParseEnumReal32_5(void)` | `0x004F1160` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QHash_FindValue` | `unestablished` | `0x004F1180` | 1.000 | CFG 1.000; bn+r2 agree |
| `QHashNode_Find` | `unestablished` | `0x004F11A0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F1210` | `unestablished` | `0x004F1210` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F1220` | `unestablished` | `0x004F1220` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventInit` | `unestablished` | `0x004F1840` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventReset` | `unestablished` | `0x004F18C0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_TaskContainer_Alloc` | `unestablished` | `0x004F1950` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_TaskContainer_Grow` | `unestablished` | `0x004F1980` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterTypeRegister` | `unestablished` | `0x004F19C0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterTypeFree` | `unestablished` | `0x004F1A40` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_PhaseIdGetE4_4F1A60` | `unestablished` | `0x004F1A60` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_PhaseIdGetE5_4F1A70` | `unestablished` | `0x004F1A70` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_TaskContainer_Append` | `unestablished` | `0x004F1A80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotAlloc` | `unestablished` | `0x004F1B20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotClear` | `unestablished` | `0x004F1B50` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetEA` | `unestablished` | `0x004F1B60` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetE6` | `unestablished` | `0x004F1B70` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetEB` | `unestablished` | `0x004F1B80` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetE9` | `unestablished` | `0x004F1B90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetE7` | `unestablished` | `0x004F1BA0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterEventSlotGetE8` | `unestablished` | `0x004F1BB0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_ParameterPropertySet` | `unestablished` | `0x004F1BC0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Qvm_CheckParameterCount` | `unestablished` | `0x004F1C20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_InitFloatPair_4F1C90` | `unestablished` | `0x004F1C90` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F1C90-semantic-review.json` |
| `PhysicsTerrain_Invoke25` | `int PhysicsTerrain_Invoke25(int arg1)` | `0x004F1CF0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke3` | `void PhysicsTerrain_Invoke3(int arg1, int* arg2, int arg3)` | `0x004F1D10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal32_4F1E00` | `unestablished` | `0x004F1E00` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F1E10` | `unestablished` | `0x004F1E10` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F1E20` | `unestablished` | `0x004F1E20` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F2020` | `unestablished` | `0x004F2020` | 1.000 | CFG 1.000; bn+r2 agree |
| `Sys_GetGlobal32_4F2030` | `unestablished` | `0x004F2030` | 1.000 | CFG 1.000; bn+r2 agree |
| `PhysicsTerrain_GetContext_4F2B80` | `int* PhysicsTerrain_GetContext_4F2B80(int* arg1)` | `0x004F2B80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F3630` | `void* PhysicsTerrain_GetContext_4F3630(void* arg1, int* arg2)` | `0x004F3630` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F3670` | `void* PhysicsTerrain_GetContext_4F3670(void* arg1, int* arg2)` | `0x004F3670` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke5_4F36B0` | `int* PhysicsTerrain_Invoke5_4F36B0(int* arg1)` | `0x004F36B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke8` | `int PhysicsTerrain_Invoke8(int arg1, char* arg2)` | `0x004F3940` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetContext_4F39F0` | `int* PhysicsTerrain_GetContext_4F39F0(int* arg1)` | `0x004F39F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_DefaultHandler_4F3A00` | `void* PhysicsTerrain_DefaultHandler_4F3A00(void* arg1)` | `0x004F3A00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke4_4F3CA0` | `int* PhysicsTerrain_Invoke4_4F3CA0(void* arg1, int* arg2)` | `0x004F3CA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_InvokeComputeNormals` | `int PhysicsTerrain_InvokeComputeNormals(float arg1, void* arg2)` | `0x004F42D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_GetParam_4F4390` | `char* PhysicsTerrain_GetParam_4F4390(int arg1, char* arg2)` | `0x004F4390` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke2_4F43B0` | `int PhysicsTerrain_Invoke2_4F43B0(int* arg1)` | `0x004F43B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke12` | `int* PhysicsTerrain_Invoke12(int* arg1)` | `0x004F43D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke12_4F44A0` | `int PhysicsTerrain_Invoke12_4F44A0(void* arg1, int arg2)` | `0x004F44A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `PhysicsTerrain_Invoke3_4F4560` | `int* PhysicsTerrain_Invoke3_4F4560(int* arg1)` | `0x004F4560` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_4F64E0` | `unestablished` | `0x004F64E0` | 1.000 | CFG 1.000; bn+r2 agree |
| `TerrainLightMap_PushEntry` | `void* __cdecl TerrainLightMap_PushEntry(void* self);` | `0x004F82B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TerrainLightMap_Render` | `void* TerrainLightMap_Render(void* context)` | `0x004F8320` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TerrainLightMap_Init` | `int* TerrainLightMap_Init(void)` | `0x004F8340` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TerrainLightMap_LoadParameters` | `int TerrainLightMap_LoadParameters(void * context)` | `0x004F86E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TerrainLightMap_Setup` | `int TerrainLightMap_Setup(int* context)` | `0x004F8780` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_NodeSizeTable_Set` | `unestablished` | `0x004F9460` | 1.000 | CFG 1.000; bn+r2 agree |
| `Script_StringHash` | `unestablished` | `0x004F9E70` | 1.000 | CFG 1.000; bn+r2 agree |
| `Physics_VirmodelIsNull` | `struct status_msg* Physics_VirmodelIsNull(void * context, int arg2, int arg3, int arg4, int* arg5)` | `0x00503CC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TextureModifier_Push` | `void* __cdecl TextureModifier_Push(void* self);` | `0x00505E40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TextureModifier_Create` | `void* TextureModifier_Create(void* context)` | `0x00505ED0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TextureModifier_Init_2` | `void* TextureModifier_Init_2(void* context)` | `0x00505F20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TextureModifier_Init` | `int* TextureModifier_Init(void)` | `0x00505F60` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TextureModifier_LoadParameters` | `int TextureModifier_LoadParameters(int* context)` | `0x005062A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HeightMap_Enter` | `void* __cdecl HeightMap_Enter(void* context);` | `0x00506F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetWord_509860` | `unestablished` | `0x00509860` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00509860-semantic-review.json` |
| `Sys_GetWord_509870` | `unestablished` | `0x00509870` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00509870-semantic-review.json` |
| `Sys_GetWord_509880` | `unestablished` | `0x00509880` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00509880-semantic-review.json` |
| `Sys_TestWordMask_509890` | `unestablished` | `0x00509890` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00509890-semantic-review.json` |
| `Sys_GetWord16_5098A0` | `unestablished` | `0x005098A0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x005098A0-semantic-review.json` |
| `Joypad_IsAcquired` | `unestablished` | `0x00509CC0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Joypad_ReleaseAll` | `unestablished` | `0x00509E90` | 1.000 | CFG 1.000; bn+r2 agree |
| `Joypad_InitActionFlags` | `unestablished` | `0x00509F00` | 1.000 | CFG 1.000; bn+r2 agree |
| `QFile_DeviceCallback_A0_50A5A0` | `unestablished` | `0x0050A5A0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x0050A5A0-semantic-review.json` |
| `QFile_DeviceCallback_9C_50A5B0` | `unestablished` | `0x0050A5B0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x0050A5B0-semantic-review.json` |
| `Terrain_DecodeChunkCommands` | `std::int32_t __cdecl Terrain_DecodeChunkCommands();` | `0x00510010` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves` | `int Terrain_UpdateWaterWaves(void)` | `0x00511500` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface` | `void * Terrain_RenderWaterSurface(void * context)` | `0x005116B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode` | `int Terrain_ConnectGraphNode(double* context)` | `0x005116D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap` | `int Terrain_SampleHeightmap(int context, int arg2, int arg3, int arg4, int arg5, int arg6, int* arg7, int* arg8, int* arg9, char arg10, char arg11, char arg12)` | `0x00512040` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals` | `int Terrain_CalculateNormals(int context, int arg2)` | `0x005122B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps` | `int Terrain_BuildMipMaps(void)` | `0x005125A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_2` | `float Terrain_RenderQuadtree_2(float context, void * arg2)` | `0x005125C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_2` | `uint32_t Terrain_SelectTerrainLOD_2(void);` | `0x005127D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_2` | `int Terrain_UpdateWaterWaves_2(void * context)` | `0x005127E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_Glowreadbufferprimit` | `int Terrain_Glowreadbufferprimit(void)` | `0x00512820` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_2` | `int Terrain_RenderWaterSurface_2(void)` | `0x00512920` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_2` | `int Terrain_ConnectGraphNode_2(int context)` | `0x00512940` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_2` | `int Terrain_SampleHeightmap_2(void)` | `0x00512BA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_Draw` | `std::int32_t Terrain_Draw()` | `0x00512C30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UnknownBlendmode` | `int Terrain_UnknownBlendmode(int context)` | `0x005144E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_OutOfTerrainRenderch` | `int* Terrain_OutOfTerrainRenderch(int context, int arg2)` | `0x00514680` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_2` | `std::uint32_t Terrain_BuildMipMaps_2(TerrainMipMapsVertex r1, TerrainMipMapsVertex r2, TerrainMipMapsVertex r3, TerrainMipMapsCall* call)` | `0x00514760` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_3` | `int* Terrain_RenderQuadtree_3(void)` | `0x00516600` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_3` | `std::int32_t Terrain_SelectTerrainLOD_3()` | `0x00516640` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_3` | `int Terrain_UpdateWaterWaves_3(void)` | `0x005166F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_3` | `char * Terrain_RenderWaterSurface_3(char * context)` | `0x00516770` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_3` | `long double Terrain_ConnectGraphNode_3(long double context)` | `0x00516810` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_3` | `long double __convention Terrain_SampleHeightmap_3("regparm")(int context, short arg2, long double arg3, int arg4, int arg5)` | `0x0051682D` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_3` | `long double Terrain_CalculateNormals_3(long double context)` | `0x005168E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_3` | `long double __convention Terrain_BuildMipMaps_3("regparm")(int context, short arg2, long double arg3, int arg4, int arg5)` | `0x005168FD` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_4` | `char * Terrain_RenderQuadtree_4(char * context, char * arg2, int arg3)` | `0x005169B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_4` | `long double Terrain_SelectTerrainLOD_4(char * context)` | `0x00516AD3` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_4` | `char * Terrain_UpdateWaterWaves_4(char * context, char * arg2, char * arg3, char * arg4, char * arg5)` | `0x00516B20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_4` | `int Terrain_RenderWaterSurface_4(PSTR context)` | `0x00516C67` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_4` | `unsigned int Terrain_ConnectGraphNode_4(char * context, int arg2, unsigned int arg3, int* arg4)` | `0x00516C91` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_4` | `unsigned int Terrain_SampleHeightmap_4(char * context, int arg2, unsigned int arg3, int* arg4)` | `0x00516D79` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_4` | `int Terrain_CalculateNormals_4(int context)` | `0x00516E83` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_4` | `char * Terrain_BuildMipMaps_4(char * context, char * arg2, int arg3)` | `0x00516F14` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_5` | `int Terrain_SelectTerrainLOD_5(int context, void ** arg2, char * arg3, int arg4)` | `0x00517260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_5` | `int Terrain_UpdateWaterWaves_5(char * context)` | `0x00517510` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_5` | `long double Terrain_RenderWaterSurface_5(char * context)` | `0x00517570` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_5` | `int Terrain_ConnectGraphNode_5(char * context)` | `0x00517580` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_LoadTexturePackage` | `x86 cdecl void(void*, char const*)` | `0x005175A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_5` | `int Terrain_SampleHeightmap_5(void * context)` | `0x005177F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_5` | `void Terrain_CalculateNormals_5(int* context)` | `0x00517840` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Iff_FindFormChunk` | `IffChunkNode* IffChunk_FindByTag(IffChunkNode* list, std::uint32_t tag)` | `0x00517890` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Iff_FindChunkBelowForm` | `IffChunkNode* IffChunk_FindBelowForm(IffChunkNode* node, std::uint32_t tag)` | `0x005178E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Iff_FindChunk` | `IffChunkNode* IffChunk_FindChildByTag(IffChunkNode* form, std::uint32_t tag)` | `0x00517940` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_6` | `int* Terrain_UpdateWaterWaves_6(int* context, int arg2)` | `0x00517980` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Iff_GetChunkPayload` | `const std::byte* IffChunk_GetPayload(const IffChunkNode* chunk)` | `0x005179C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_6` | `int Terrain_ConnectGraphNode_6(char * context)` | `0x005179D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_6` | `int* Terrain_SampleHeightmap_6(char * context)` | `0x00517A00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_6` | `int* Terrain_CalculateNormals_6(int* context, int arg2, int arg3)` | `0x00517A30` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_6` | `int Terrain_BuildMipMaps_6(int context, int* arg2, int* arg3, int arg4)` | `0x00517B00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_7` | `short* Terrain_RenderQuadtree_7(char * context, int arg2, short* arg3, short* arg4, int arg5, int arg6, int* arg7)` | `0x00517C90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_7` | `short* Terrain_SelectTerrainLOD_7(char * context, int arg2, short* arg3, short* arg4, int arg5, int arg6, int* arg7)` | `0x00517DD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_7` | `int* Terrain_UpdateWaterWaves_7(int* context, int arg2, int* arg3, int arg4, int arg5, int arg6, int* arg7)` | `0x00517F00` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_7` | `void * Terrain_RenderWaterSurface_7(void * context, int arg2, int arg3, int arg4, char * arg5, int arg6, int* arg7, int arg8, int* arg9)` | `0x00517FD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_7` | `double* Terrain_ConnectGraphNode_7(double* context, int arg2, int arg3, double arg4, double arg5, double arg6)` | `0x00518130` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_7` | `long double Terrain_SampleHeightmap_7(double* context, double arg2)` | `0x00518190` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_7` | `long double Terrain_CalculateNormals_7(double* context, double arg2)` | `0x005181C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_7` | `void * Terrain_BuildMipMaps_7(void ** context)` | `0x005181F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_8` | `int Terrain_RenderQuadtree_8(int context)` | `0x00518230` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_8` | `int Terrain_SelectTerrainLOD_8(void)` | `0x00518240` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_8` | `int Terrain_UpdateWaterWaves_8(int context)` | `0x00518250` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_8` | `int Terrain_RenderWaterSurface_8(void)` | `0x00518260` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_8` | `int Terrain_ConnectGraphNode_8(void)` | `0x00518290` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_8` | `int Terrain_SampleHeightmap_8(int* context, float* arg2, float* arg3)` | `0x005182B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_8` | `int Terrain_BuildMipMaps_8(void)` | `0x00518550` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_NodeCriteriaRegister` | `unestablished` | `0x005186C0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Terrain_SelectTerrainLOD_9` | `int Terrain_SelectTerrainLOD_9(int context)` | `0x00518720` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_9` | `unsigned int Terrain_UpdateWaterWaves_9(void)` | `0x00519650` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_9` | `void * Terrain_RenderWaterSurface_9(int context)` | `0x00519690` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_9` | `int Terrain_ConnectGraphNode_9(int* context)` | `0x005196D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_9` | `unsigned int Terrain_SampleHeightmap_9(void)` | `0x00519BA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_9` | `float Terrain_CalculateNormals_9(float context)` | `0x00519BD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_9` | `long double Terrain_BuildMipMaps_9(int context, float arg2)` | `0x0051A0C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_10` | `void * Terrain_RenderQuadtree_10(void * context)` | `0x0051A110` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SelectTerrainLOD_10` | `int Terrain_SelectTerrainLOD_10(float context, int arg2)` | `0x0051A160` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `SplinePathDynCubeObj_Init` | `unestablished` | `0x0051A1D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_UpdateWaterWaves_10` | `float* Terrain_UpdateWaterWaves_10(float context, float* arg2, float arg3, float arg4)` | `0x0051A4A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_10` | `void Terrain_RenderWaterSurface_10(float context)` | `0x0051A8C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_10` | `int Terrain_ConnectGraphNode_10(void * context)` | `0x0051ACB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_10` | `void * Terrain_SampleHeightmap_10(void * context)` | `0x0051B0B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_10` | `unsigned int Terrain_CalculateNormals_10(void)` | `0x0051B3A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Script_SetConstant_51BAA0` | `unestablished` | `0x0051BAA0` | 1.000 | CFG 1.000; bn+r2 agree |
| `Terrain_BuildMipMaps_10` | `unsigned int Terrain_BuildMipMaps_10(void)` | `0x0051BF90` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_11` | `int Terrain_RenderQuadtree_11(void)` | `0x0051C290` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderWaterSurface_11` | `unsigned int Terrain_RenderWaterSurface_11(void)` | `0x0051DDB0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_ConnectGraphNode_11` | `unsigned int Terrain_ConnectGraphNode_11(void)` | `0x0051EC70` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_SampleHeightmap_11` | `unsigned int Terrain_SampleHeightmap_11(void)` | `0x0051EEC0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_11` | `unsigned int Terrain_CalculateNormals_11(void)` | `0x0051F560` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_BuildMipMaps_11` | `int Terrain_BuildMipMaps_11(float context, void * arg2, int arg3)` | `0x0051FBA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_RenderQuadtree_12` | `unsigned int Terrain_RenderQuadtree_12(void)` | `0x0051FED0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `WeatherWaterSky_Invoke7_5236C0` | `int32_t __convention WeatherWaterSky_Invoke7_5236C0("regparm")(int32_t param_1, int32_t param_2, int32_t* param_3, void* param_4);` | `0x005236C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_Prepare` | `int* DiscardTerrain_Prepare(int* context)` | `0x0052BFE0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_LoadParameters` | `int DiscardTerrain_LoadParameters(int* context)` | `0x0052C000` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_Init_2` | `int DiscardTerrain_Init_2()` | `0x0052C0B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_Init` | `int* DiscardTerrain_Init(void)` | `0x0052C0C0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_Setup` | `int DiscardTerrain_Setup(int context, char * arg2)` | `0x0052C290` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `DiscardTerrain_Process` | `int DiscardTerrain_Process(int* context, void * arg2)` | `0x0052C5A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FlashTask_Init` | `unestablished` | `0x00530A80` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `FreeCamera_Init` | `unestablished` | `0x005310E0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## TRANSCRIBED (59)

Records written in the Level 1 MVP session (2026-10-25). They carry static structure matching retail at CFG 1.000 and retail-side runtime observations, but no candidate runtime comparison, because the candidate exits at 0x00402930 before executing game natives. They are therefore not claimed as verified.

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `QTask_RegisterInputPortHandler` | `x86 cdecl uint32_t(uint32_t, uint32_t, uint32_t)` | `0x004012A0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `QTask_AllocateInputPort` | `x86 cdecl void*(uint32_t, uint32_t)` | `0x004012F0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `TaskType_Register` | `unestablished` | `0x00401900` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Config_SetContentControlPassword` | `int GO_ContentControlPW(int context, float arg2)` | `0x00403C10` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetSettingByte_4071D0` | `unsigned char __cdecl(void)` | `0x004071D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004071D0-semantic-review.json` |
| `FallLanding` | `int32_t FallLanding(float param_1);` | `0x00408460` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_8` | `unresolved` | `0x0040E620` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_9` | `unresolved` | `0x0040F370` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_10` | `unresolved` | `0x0040FDA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_LoadParameters_11` | `unresolved` | `0x00410370` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HumanPlayer_WireSlide` | `int HumanPlayer_WireSlide1_2(float* context)` | `0x00411F20` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetSettingByte_4167D0` | `unsigned char __cdecl(void)` | `0x004167D0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004167D0-semantic-review.json` |
| `Sys_GetSettingByte_4167E0` | `unsigned char __cdecl(void)` | `0x004167E0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004167E0-semantic-review.json` |
| `Sys_GetSettingByte_4167F0` | `unsigned char __cdecl(void)` | `0x004167F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004167F0-semantic-review.json` |
| `Sys_GetSettingByte_416800` | `unsigned char __cdecl(void)` | `0x00416800` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416800-semantic-review.json` |
| `Sys_GetSettingByte_416810` | `unsigned char __cdecl(void)` | `0x00416810` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416810-semantic-review.json` |
| `Sys_GetSettingByte_416820` | `unsigned char __cdecl(void)` | `0x00416820` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416820-semantic-review.json` |
| `Sys_GetSettingByte_416830` | `unsigned char __cdecl(void)` | `0x00416830` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416830-semantic-review.json` |
| `Sys_GetSettingByte_416840` | `unsigned char __cdecl(void)` | `0x00416840` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416840-semantic-review.json` |
| `Sys_GetSettingByte_416850` | `unsigned char __cdecl(void)` | `0x00416850` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416850-semantic-review.json` |
| `Sys_GetSettingByte_416860` | `unestablished` | `0x00416860` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416860-semantic-review.json` |
| `Sys_GetSettingByte_416880` | `unsigned char __cdecl(void)` | `0x00416880` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416880-semantic-review.json` |
| `Sys_GetSettingByte_416890` | `unsigned char __cdecl(void)` | `0x00416890` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00416890-semantic-review.json` |
| `Sys_GetGlobal16_424850` | `unestablished` | `0x00424850` | 1.000 | CFG 1.000; bn+r2 agree; review `0x00424850-semantic-review.json` |
| `WeaponGun_ProcessWNES` | `int32_t __convention WeaponGun_ProcessWNES("regparm")(int32_t param_1, int32_t param_2, int32_t param_3, void* param_4);` | `0x00471120` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `AI_Detection_Init` | `unestablished` | `0x00489560` | 1.000 | CFG 1.000; bn+r2 agree; static only |
| `Camera_ResetMatrix_5` | `int32_t Camera_ResetMatrix_5(long param_1);` | `0x004A5955` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `MathSys_Invoke7_4A67E4` | `int32_t __convention MathSys_Invoke7_4A67E4("regparm")(char* param_1, int16_t param_2, void* param_3, void* param_4, int32_t param_5);` | `0x004A67E4` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_14` | `long double(double arg1, int16_t arg2 @ 0xa) __pure Camera_ResetMatrix_14(void);` | `0x004A8A91` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateTransform_15` | `int32_t __fastcall Camera_UpdateTransform_15(int32_t param_1, void* param_2, void* param_3, long param_4);` | `0x004A8AD0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_SetFieldOfView_15` | `int32_t __fastcall Camera_SetFieldOfView_15(int32_t param_1, void* param_2, void* param_3, long param_4, long param_5);` | `0x004A8B37` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyWideScreen_17` | `int32_t Camera_ApplyWideScreen_17(long param_1);` | `0x004A9BD1` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyPostProcess_17` | `long double __convention Camera_ApplyPostProcess_17("regparm")(int32_t param_1, int32_t param_2, long param_3, int32_t param_4, int32_t param_5, int32_t param_6, int32_t param_7);` | `0x004A9C40` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateViewCone_17` | `long double __convention Camera_UpdateViewCone_17("regparm")(int32_t param_1, int32_t param_2, long param_3, int32_t param_4, int32_t param_5);` | `0x004A9C57` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_17` | `long double Camera_ResetMatrix_17(long param_1);` | `0x004A9CA0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateTransform_18` | `int32_t Camera_UpdateTransform_18(int32_t param_1);` | `0x004A9CB5` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_SetFieldOfView_18` | `void __convention("regparm")(int32_t arg1, long double arg2 @ st0) __pure Camera_SetFieldOfView_18(void);` | `0x004A9CCC` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyLineFilter_18` | `long double __fastcall Camera_ApplyLineFilter_18(int32_t param_1, double* param_2);` | `0x004A9CE5` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ApplyBandFilter_18` | `int32_t(int32_t arg1 @ 0x8) __pure Camera_ApplyBandFilter_18(void);` | `0x004A9D28` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `MathSys_Invoke19` | `void* __convention MathSys_Invoke19("regparm")(int32_t param_1, char* param_2);` | `0x004AA0A6` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `MathSys_Invoke21` | `int32_t __convention MathSys_Invoke21("regparm")(int32_t param_1, int32_t param_2, int32_t param_3, int32_t* param_4, void* param_5, void* param_6, int32_t param_7);` | `0x004AAA44` | 0.720 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_MapCode_4ABC46` | `unestablished` | `0x004ABC46` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004ABC46-semantic-review.json` |
| `MathSys_Invoke3` | `int32_t __convention MathSys_Invoke3("regparm")(int32_t param_1, int32_t param_2, int32_t param_3, int32_t* param_4, void* param_5, void* param_6, int32_t param_7);` | `0x004AD004` | 0.720 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_ResetMatrix_26` | `uint32_t __stdcall(int32_t arg1, uint32_t arg2, int32_t arg3, uint32_t arg4) __pure Camera_ResetMatrix_26(void);` | `0x004AD140` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Camera_UpdateTransform_27` | `uint32_t __stdcall(int32_t arg1, uint32_t arg2, int32_t arg3, uint32_t arg4) __pure Camera_UpdateTransform_27(void);` | `0x004AD1B0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `HeapList_Insert` | `unestablished` | `0x004AF8F0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004AF8F0-semantic-review.json`; CLASSIFICATION CORRECTION (round 244): called from 0x004012BF inside the CRT startup region and placed among CRT pattern/list helpers (0x004AFF90, 0x004AFFB0, 0x004AF9D0), so this is runtime list support, not a game native. Kept as a transcribed record with its evidence; not boundary progress. |
| `Model_GetBoneWord0C_4B6CE0` | `unestablished` | `0x004B6CE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B6CE0-semantic-review.json` |
| `Model_GetBoneWord10_4B6D20` | `unestablished` | `0x004B6D20` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004B6D20-semantic-review.json` |
| `DynObj_IsNumericToken_4C0170` | `unestablished` | `0x004C0170` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C0170-semantic-review.json` |
| `Sys_GetGlobal16_4C48C0` | `unestablished` | `0x004C48C0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004C48C0-semantic-review.json` |
| `unnamed in retail` | `unestablished` | `0x004C5810` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Sys_GetGlobal16_4D9FA0` | `unestablished` | `0x004D9FA0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004D9FA0-semantic-review.json` |
| `Sys_GetGlobal8_4D9FB0` | `unestablished` | `0x004D9FB0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004D9FB0-semantic-review.json` |
| `Sys_GetGlobal32_4F0F50` | `unestablished` | `0x004F0F50` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0F50-semantic-review.json` |
| `Sys_GetGlobal32_4F0F70` | `unestablished` | `0x004F0F70` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0F70-semantic-review.json` |
| `Sys_GetGlobal32_4F0FC0` | `unestablished` | `0x004F0FC0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0FC0-semantic-review.json` |
| `Sys_GetGlobal32_4F0FE0` | `unestablished` | `0x004F0FE0` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F0FE0-semantic-review.json` |
| `Sys_InitListHead_4F1D60` | `unestablished` | `0x004F1D60` | 1.000 | CFG 1.000; bn+r2 agree; review `0x004F1D60-semantic-review.json` |
| `CamTask_Init` | `unestablished` | `0x00530220` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## ANALYZED (4)

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `QTask_RegisterClasses` | `unknown (not transcribed; fail-closed stub throws)` | `0x00402930` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `sprintf` | `unestablished` | `0x004A53B3` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `String_VFormat` | `unestablished` | `0x004A5702` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |
| `Terrain_CalculateNormals_8` | `int Terrain_CalculateNormals_8(void)` | `0x005183D0` | 1.000 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## INCONCLUSIVE (1)

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `HumanPlayer_GetContext_40EF5C` | `void* HumanPlayer_GetContext_40EF5C(void* arg1)` | `0x0040EF5C` | 0.650 | NO IMPLEMENTATION FILE: record exists, no code backs it |

## STATUS CLAIM WITHOUT RECORDED EVIDENCE (96)

These rows claim VERIFIED or VERIFIED_WITH_DOCUMENTED_SCOPE in the manifest but their record carries no decoder
evidence, no semantic review, or no behavioral/runtime contract. They are historical statuses from earlier branch
work and were **not** re-validated in this session, so they are not counted as verified here. Reproducing their
evidence, or downgrading the status, is required before they can be relied on.

| native name | signature | retail address | CFG | verification comment |
| --- | --- | --- | --- | --- |
| `QTask_DeleteQTaskEventNoContext` | `unestablished` | `0x004018e0` | - | claim without recorded evidence |
| `QTask_DeleteQTaskType` | `unestablished` | `0x00401a20` | - | claim without recorded evidence |
| `QTask_IsQTaskTypeDerived` | `unestablished` | `0x00401cf0` | - | claim without recorded evidence |
| `FramesSet` | `unestablished` | `0x00402820` | - | claim without recorded evidence |
| `Flow_GetTicks` | `unestablished` | `0x004028b0` | - | claim without recorded evidence |
| `WeaponConfigRead` | `unestablished` | `0x004071e0` | - | claim without recorded evidence |
| `GameMaterialLoad` | `unestablished` | `0x00408350` | - | claim without recorded evidence |
| `WeaponTypeOpen` | `unestablished` | `0x00413b70` | - | claim without recorded evidence |
| `WeaponsCountGet` | `unestablished` | `0x00413bb0` | - | claim without recorded evidence |
| `GameFunctions_IsDebugKeyPressed` | `unestablished` | `0x00414fd0` | - | claim without recorded evidence |
| `GameFunctions_IsDebugKeyPressedOnce` | `unestablished` | `0x00415020` | - | claim without recorded evidence |
| `InputDisable` | `unestablished` | `0x004157d0` | - | claim without recorded evidence |
| `InputEnable` | `unestablished` | `0x00415830` | - | claim without recorded evidence |
| `MusicUpdateVolume` | `unestablished` | `0x004158e0` | - | claim without recorded evidence |
| `MusicEnable` | `unestablished` | `0x00415a10` | - | claim without recorded evidence |
| `MusicDisable` | `unestablished` | `0x00415a60` | - | claim without recorded evidence |
| `CutsceneDelete` | `unestablished` | `0x00415ab0` | - | claim without recorded evidence |
| `LevelQuit` | `unestablished` | `0x00416550` | - | claim without recorded evidence |
| `Game_GetRandomState` | `unestablished` | `0x00416d20` | - | claim without recorded evidence |
| `LevelRestart` | `unestablished` | `0x00416fe0` | - | claim without recorded evidence |
| `MenuManager` | `unestablished` | `0x00418b00` | - | claim without recorded evidence |
| `BackgroundFX_New` | `unestablished` | `0x004199d0` | - | claim without recorded evidence |
| `ListBox_AddItem` | `unestablished` | `0x0041f2d0` | - | claim without recorded evidence |
| `Cursor_RunHandler` | `unestablished` | `0x00424ce0` | - | claim without recorded evidence |
| `-` | `unestablished` | `0x004444e0` | - | claim without recorded evidence |
| `SoldierExecute` | `unestablished` | `0x0045c440` | - | claim without recorded evidence |
| `WeaponAmmoPickup` | `unestablished` | `0x0045ff80` | - | claim without recorded evidence |
| `WeaponGunPickup` | `unestablished` | `0x0045ffc0` | - | claim without recorded evidence |
| `AI_UpdateOrders_18` | `unestablished` | `0x004610c0` | - | claim without recorded evidence |
| `HumanView_UpdateView` | `unestablished` | `0x00464800` | - | claim without recorded evidence |
| `HumanView_UpdateBody` | `unestablished` | `0x00464bd0` | - | claim without recorded evidence |
| `AmmoTypeOpen` | `unestablished` | `0x0047cab0` | - | claim without recorded evidence |
| `MissionOpen` | `unestablished` | `0x00484e60` | - | claim without recorded evidence |
| `StatusMessageShow` | `unestablished` | `0x00485970` | - | claim without recorded evidence |
| `StatusMessageClear` | `unestablished` | `0x00485ad0` | - | claim without recorded evidence |
| `LoadingScreenShow` | `unestablished` | `0x0048a440` | - | claim without recorded evidence |
| `Display_SetScreenMode` | `int32_t(int32_t mode)` | `0x0048F120` | 1.000 | claim without recorded evidence |
| `SetDebugCompileState` | `int32_t(int32_t value)` | `0x0048F1C0` | 1.000 | claim without recorded evidence |
| `AppMain_Set5C8C04` | `int32_t(int32_t value)` | `0x0048F200` | 1.000 | claim without recorded evidence |
| `Input_SetUseWin32Input` | `int32_t(int32_t value)` | `0x0048F280` | 1.000 | claim without recorded evidence |
| `AppMain_Set5C8C0C` | `int32_t(int32_t value)` | `0x0048F290` | 1.000 | claim without recorded evidence |
| `AppMain_Set5C8C10` | `int32_t(int32_t value)` | `0x0048F2A0` | 1.000 | claim without recorded evidence |
| `AppContext_SetGermanVersion` | `int32_t(int32_t value)` | `0x0048F2B0` | 1.000 | claim without recorded evidence |
| `AppContext_SetLanguage` | `int32_t(const char* language)` | `0x0048F300` | 1.000 | claim without recorded evidence |
| `AppMain_WindowCommand` | `unestablished` | `0x0048F350` | - | claim without recorded evidence |
| `AppMain_WinInputCommand` | `unestablished` | `0x0048F920` | - | claim without recorded evidence |
| `AppMain_GermanyCommand` | `unestablished` | `0x0048F930` | - | claim without recorded evidence |
| `AppMain_USACommand` | `unestablished` | `0x0048F950` | - | claim without recorded evidence |
| `Mouse_Update` | `unestablished` | `0x0048fc20` | - | claim without recorded evidence |
| `Display_SetMode` | `unestablished` | `0x00491a90` | - | claim without recorded evidence |
| `Display_GetActiveMode` | `unestablished` | `0x00491cf0` | - | claim without recorded evidence |
| `Display_SetBackgroundColourFn` | `unestablished` | `0x00491e70` | - | claim without recorded evidence |
| `MusicSetVolume` | `unestablished` | `0x00495e70` | - | claim without recorded evidence |
| `MusicSetSfxVolume` | `unestablished` | `0x00495f30` | - | claim without recorded evidence |
| `FileOpen` | `unestablished` | `0x004a5350` | - | claim without recorded evidence |
| `Debug_Error` | `int Debug_Error(const char* format, ...)` | `0x004AF7B0` | 1.000 | claim without recorded evidence |
| `MemoryDealloc` | `unestablished` | `0x004b0be0` | - | claim without recorded evidence |
| `Real64x3_NullVector` | `unestablished` | `0x004b3320` | - | claim without recorded evidence |
| `Real32x9_Create` | `unestablished` | `0x004b38e0` | - | claim without recorded evidence |
| `Qsprite_Register4Az` | `unestablished` | `0x004b53b0` | - | claim without recorded evidence |
| `IsResourceLoaded` | `unestablished` | `0x004b5b90` | - | claim without recorded evidence |
| `Picture_Register` | `unestablished` | `0x004b6e60` | - | claim without recorded evidence |
| `Picture_GetWidth` | `unestablished` | `0x004b6e70` | - | claim without recorded evidence |
| `Picture_GetHeight` | `unestablished` | `0x004b6e80` | - | claim without recorded evidence |
| `SymbolRegisterInt16` | `unestablished` | `0x004b7e80` | - | claim without recorded evidence |
| `SymbolRegisterBool8` | `unestablished` | `0x004b7ee0` | - | claim without recorded evidence |
| `SymbolRegisterInt32` | `unestablished` | `0x004b7fa0` | - | claim without recorded evidence |
| `SymbolRegisterReal32` | `unestablished` | `0x004b7fc0` | - | claim without recorded evidence |
| `GameDataSymbolRemove` | `unestablished` | `0x004b80a0` | - | claim without recorded evidence |
| `QscCompile` | `unestablished` | `0x004b8410` | - | claim without recorded evidence |
| `Qvm_Execute` | `extern "C" std::int32_t IGI_RETAIL_CDECL Qvm_Execute(void* qvm)` | `0x004B85B0` | 1.000 | claim without recorded evidence |
| `ScriptSetsymbolCxt` | `unestablished` | `0x004b8930` | - | claim without recorded evidence |
| `Script_EvalArgInt` | `unestablished` | `0x004b8a20` | - | claim without recorded evidence |
| `QvmAssemble` | `unestablished` | `0x004bb270` | - | claim without recorded evidence |
| `LogAdd` | `unestablished` | `0x004bbc30` | - | claim without recorded evidence |
| `ScriptBufInit` | `unestablished` | `0x004c0200` | - | claim without recorded evidence |
| `SymbolRemove` | `unestablished` | `0x004c0460` | - | claim without recorded evidence |
| `QTaskList_Delete` | `unestablished` | `0x004c1830` | - | claim without recorded evidence |
| `RigidDyncubeObjRead` | `unestablished` | `0x004c48e0` | - | claim without recorded evidence |
| `MagicObjLoad` | `unestablished` | `0x004c4930` | - | claim without recorded evidence |
| `AnimController_UpdateChannels` | `unestablished` | `0x004d3210` | - | claim without recorded evidence |
| `BoneDynCubeObj_GetQTaskType` | `unestablished` | `0x004d9610` | - | claim without recorded evidence |
| `SoundSys_SetFrequency` | `unestablished` | `0x004e6030` | - | claim without recorded evidence |
| `SoundLoad` | `unestablished` | `0x004e68d0` | - | claim without recorded evidence |
| `TextWindow_vprintf` | `unestablished` | `0x004e76e0` | - | claim without recorded evidence |
| `DebugText_SetCursor` | `unestablished` | `0x004e7b80` | - | claim without recorded evidence |
| `DebugText_Clear` | `unestablished` | `0x004e7bb0` | - | claim without recorded evidence |
| `SoundSys_EvaluateTrigger_11` | `unestablished` | `0x004e8100` | - | claim without recorded evidence |
| `AnimTriggerLoad` | `unestablished` | `0x004ec0c0` | - | claim without recorded evidence |
| `PhysicsObjLoad` | `unestablished` | `0x004ee030` | - | claim without recorded evidence |
| `LevelLoad` | `unestablished` | `0x004f0e10` | - | claim without recorded evidence |
| `ScriptInit` | `unestablished` | `0x004f0e50` | - | claim without recorded evidence |
| `QtaskUpdate` | `unestablished` | `0x004f0e90` | - | claim without recorded evidence |
| `GraphOpen` | `unestablished` | `0x004f9ff0` | - | claim without recorded evidence |
| `Input_Run` | `unestablished` | `0x00507ec0` | - | claim without recorded evidence |
| `Joypad_Update` | `unestablished` | `0x00509cf0` | - | claim without recorded evidence |
