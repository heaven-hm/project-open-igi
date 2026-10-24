# Retail script-native registrar pairs (name string, function pointer)

- image: `/Users/haseeb-mir/Downloads/IGI/igi.exe`, base `0x00400000`
- registrars scanned: `0x4B8890`, `0x4BAAC0`, `0x4B88C0`, `0x4B88F0`
- method: `scripts/registrar_pairs.py` - for every `call` into a registrar, decode the preceding pushes backwards,
  read the name string from the image, and cross-reference the function against `docs/fidelity/native-verification.json`.

- call sites decoded: **167**, unique functions: **166**
- already implemented here: **88**, not implemented: **78**
- call sites whose pushes did not decode: **19**

## Unique functions, in address order

| retail VA | retail registered name | implemented | candidate name here |
| --- | --- | --- | --- |
| `0x402780` | `Flow_RequestEvent` | **no** | - |
| `0x403660` | `GOStart` | **no** | - |
| `0x4036B0` | `GOPlayer` | yes | GOPlayer |
| `0x403750` | `GOActiveMission` | yes | GOActiveMission |
| `0x4037C0` | `GOInRemap` | yes | GOInRemap |
| `0x403870` | `GOInMouInv` | yes | GOInMouInv |
| `0x4038E0` | `GOInMouSens` | yes | GOInMouSens |
| `0x403950` | `GOGfxDisp` | yes | GOGfxDisp |
| `0x403A00` | `GOGameLang` | yes | GOGameLang |
| `0x403AC0` | `GOGameDiff` | yes | GOGameDiff |
| `0x403B30` | `Config_IsGermany` | yes | Config_IsGermany |
| `0x403B70` | `Config_ResetGraphicOptions` | yes | Config_ResetGraphicOptions |
| `0x403C10` | `GOContentControlPW` | yes | Config_SetContentControlPassword |
| `0x403C70` | `Config_GetContentControlPassword` | yes | Config_GetContentControlPassword |
| `0x403CC0` | `Config_VerifyContentControlPassword` | yes | Config_VerifyContentControlPassword |
| `0x403D90` | `Config_IsContentControlPasswordEnabled` | yes | Config_IsContentControlPasswordEnabled |
| `0x403DF0` | `GOIsBlood` | yes | GOIsBlood |
| `0x403E60` | `GOSoundSpeech` | yes | GOSoundSpeech |
| `0x403EF0` | `GOSoundMusic` | yes | GOSoundMusic |
| `0x403F80` | `GOSoundFX` | yes | GOSoundFX |
| `0x404010` | `GOGfxDevice` | yes | GOGfxDevice |
| `0x404080` | `GOGfxGamma` | yes | GOGfxGamma |
| `0x4040F0` | `GOGfxPerformance` | yes | GOGfxPerformance |
| `0x404160` | `Config_GameOptionsInputSetInvertMouse` | yes | Config_GameOptionsInputSetInvertMouse |
| `0x4041D0` | `Config_GameOptionsInputGetInvertMouse` | yes | Config_GameOptionsInputGetInvertMouse |
| `0x404230` | `Config_GameOptionsInputSetMouseSensitivity` | yes | Config_GameOptionsInputSetMouseSensitivity |
| `0x404290` | `Config_GameOptionsInputGetMouseSensitivity` | yes | Config_GameOptionsInputGetMouseSensitivity |
| `0x4042E0` | `Config_GameOptionsInputSetBloodEnabled` | yes | Config_GameOptionsInputSetBloodEnabled |
| `0x404340` | `Config_GameOptionsInputGetBloodEnabled` | yes | Config_GameOptionsInputGetBloodEnabled |
| `0x404450` | `Config_FillScreenResolutionListBox` | yes | Config_FillScreenResolutionListBox |
| `0x404510` | `Config_GraphicOptionsSetResolution` | yes | Config_GraphicOptionsSetResolution |
| `0x4045B0` | `Config_GraphicOptionsGetResolution` | yes | Config_GraphicOptionsGetResolution |
| `0x4046A0` | `Config_FillRenderDeviceListBox` | yes | Config_FillRenderDeviceListBox |
| `0x404710` | `Config_GraphicOptionsGetDevice` | yes | Config_GraphicOptionsGetDevice |
| `0x4047F0` | `Config_GraphicOptionsSetDevice` | yes | Config_GraphicOptionsSetDevice |
| `0x4048B0` | `Config_GraphicOptionsSetGamma` | yes | Config_GraphicOptionsSetGamma |
| `0x404910` | `Config_GraphicOptionsGetGamma` | yes | Config_GraphicOptionsGetGamma |
| `0x404960` | `Config_GraphicOptionsSetTransparency` | yes | Config_GraphicOptionsSetTransparency |
| `0x4049C0` | `Config_GraphicOptionsGetTransparency` | yes | Config_GraphicOptionsGetTransparency |
| `0x404A10` | `Config_SoundOptionsSetSoundsEffects` | yes | Config_SoundOptionsSetSoundsEffects |
| `0x404A90` | `Config_SoundOptionsGetSoundsEffects` | yes | Config_SoundOptionsGetSoundsEffects |
| `0x404AE0` | `Config_SoundOptionsSetSoundsEffectsVolume` | yes | Config_SoundOptionsSetSoundsEffectsVolume |
| `0x404B40` | `Config_SoundOptionsGetSoundsEffectsVolume` | yes | Config_SoundOptionsGetSoundsEffectsVolume |
| `0x404B90` | `Config_SoundOptionsSetMusic` | yes | Config_SoundOptionsSetMusic |
| `0x404BF0` | `Config_SoundOptionsGetMusic` | yes | Config_SoundOptionsGetMusic |
| `0x404C40` | `Config_SoundOptionsSetMusicVolume` | yes | Config_SoundOptionsSetMusicVolume |
| `0x404CA0` | `Config_SoundOptionsGetMusicVolume` | yes | Config_SoundOptionsGetMusicVolume |
| `0x404CF0` | `Config_SoundOptionsSetSpeech` | yes | Config_SoundOptionsSetSpeech |
| `0x404D50` | `Config_SoundOptionsGetSpeech` | yes | Config_SoundOptionsGetSpeech |
| `0x404DA0` | `Config_SoundOptionsSetSpeechVolume` | yes | Config_SoundOptionsSetSpeechVolume |
| `0x404E00` | `Config_SoundOptionsGetSpeechVolume` | yes | Config_SoundOptionsGetSpeechVolume |
| `0x404E50` | `Config_SoundOptionsSetReverseStereo` | yes | Config_SoundOptionsSetReverseStereo |
| `0x404EB0` | `Config_SoundOptionsGetReverseStereo` | yes | Config_SoundOptionsGetReverseStereo |
| `0x404F00` | `Config_GraphicOptionsSetPerformanceFlags` | yes | Config_GraphicOptionsSetPerformanceFlags |
| `0x404F60` | `Config_GraphicOptionsGetPerformanceFlags` | yes | Config_GraphicOptionsGetPerformanceFlags |
| `0x404FA0` | `Config_FillPlayerProfileListBox` | yes | Config_FillPlayerProfileListBox |
| `0x405010` | `Config_GetActivePlayerProfileIndex` | yes | Config_GetActivePlayerProfileIndex |
| `0x405050` | `Config_SetActivePlayerProfileIndex` | yes | Config_SetActivePlayerProfileIndex |
| `0x4050A0` | `Config_CreateNewPlayerProfile` | yes | Config_CreateNewPlayerProfile |
| `0x405400` | `Config_DeletePlayerProfile` | yes | Config_DeletePlayerProfile |
| `0x4054F0` | `Config_GetNumberOfPlayerProfiles` | yes | Config_GetNumberOfPlayerProfiles |
| `0x405530` | `Config_GraphicOptionsGetPerfLevelFromFlags` | yes | Config_GraphicOptionsGetPerfLevelFromFlags |
| `0x4055C0` | `Config_GraphicOptionsGetPerfFlagsFromLevel` | yes | Config_GraphicOptionsGetPerfFlagsFromLevel |
| `0x405620` | `Config_GraphicOptionsNumPerfLevels` | yes | Config_GraphicOptionsNumPerfLevels |
| `0x405660` | `Config_FillMissionSelectionBox` | yes | Config_FillMissionSelectionBox |
| `0x4056F0` | `Config_FillMissionPictureBox` | yes | Config_FillMissionPictureBox |
| `0x405760` | `Config_FillMissionTextBox` | yes | Config_FillMissionTextBox |
| `0x4057D0` | `Config_PlayerGetActiveMission` | yes | Config_PlayerGetActiveMission |
| `0x407E30` | `DefineQMaterial` | yes | DefineQMaterial |
| `0x407F50` | `DefineGameMaterial` | yes | DefineGameMaterial |
| `0x4101C0` | `DefineHumanPlayerWeaponCycle` | yes | DefineHumanPlayerWeaponCycle |
| `0x410240` | `DefineHumanPlayerWeaponCategory` | yes | DefineHumanPlayerWeaponCategory |
| `0x4102D0` | `DefineHumanPlayerAmmoLimit` | yes | DefineHumanPlayerAmmoLimit |
| `0x4151E0` | `MovieSubtitle_RegisterFont` | **no** | - |
| `0x415260` | `MovieSubtitle_RegisterText` | **no** | - |
| `0x4157D0` | `Game_DisablePlayerInput` | yes | InputDisable |
| `0x415830` | `Game_EnablePlayerInput` | yes | InputEnable |
| `0x415890` | `Game_SetMission` | **no** | - |
| `0x4158E0` | `Game_UpdateVolume` | yes | MusicUpdateVolume |
| `0x415950` | `Game_SetMusicVolume` | **no** | - |
| `0x4159B0` | `Game_SetSFXVolume` | **no** | - |
| `0x415A10` | `Game_EnableMusic` | yes | MusicEnable |
| `0x415A60` | `Game_DisableMusic` | yes | MusicDisable |
| `0x415AB0` | `Game_CutsceneDelete` | yes | CutsceneDelete |
| `0x4174A0` | `MenuManager_RequestScreen` | **no** | - |
| `0x417690` | `MenuManager_PushScreen` | **no** | - |
| `0x417710` | `MenuManager_PopScreen` | **no** | - |
| `0x417790` | `MenuManager_LeaveMenus` | **no** | - |
| `0x417810` | `MenuManager_SetLanguage` | **no** | - |
| `0x417880` | `MenuManager_ForceUpdateWindow` | **no** | - |
| `0x417900` | `MenuManager_ActivatePopupScreen` | **no** | - |
| `0x417960` | `MenuManager_DeactivatePopuScreen` | **no** | - |
| `0x4179B0` | `MenuManager_SetEnabled` | **no** | - |
| `0x4231E0` | `ControlsMenu_ConvertFloatFromOffset` | **no** | - |
| `0x423240` | `ControlsMenu_ConvertOffsetFromFloat` | **no** | - |
| `0x44D0E0` | `AIAction_Patrol` | **no** | - |
| `0x44D160` | `AIAction_Combat` | **no** | - |
| `0x44D260` | `AIAction_Dead` | **no** | - |
| `0x44D2C0` | `AIAction_FallFlat` | **no** | - |
| `0x44D420` | `AIAction_Activate` | **no** | - |
| `0x44D4A0` | `AIAction_WalkToNode` | **no** | - |
| `0x44D510` | `AIAction_RunToNode` | **no** | - |
| `0x44D580` | `AIAction_FireAtNode` | **no** | - |
| `0x44D630` | `AIAction_FireAtTask` | **no** | - |
| `0x44D6E0` | `AIAction_PlayAnimation` | **no** | - |
| `0x44D750` | `AIAction_PlaySound` | **no** | - |
| `0x44D7D0` | `AIAction_MoveToEvent` | **no** | - |
| `0x44D870` | `AIAction_LookAtEvent` | **no** | - |
| `0x44D8F0` | `AIAction_Stunned` | **no** | - |
| `0x44D960` | `AIAction_KickGrenade` | **no** | - |
| `0x44DA00` | `AIAction_RunPanicking` | **no** | - |
| `0x44DA80` | `AIAction_Idle` | **no** | - |
| `0x44DAE0` | `AIAction_SetCombat` | **no** | - |
| `0x44DBC0` | `AIFunction_RemoveAlarmActions` | **no** | - |
| `0x44DC40` | `AIFunction_SetViewLength` | **no** | - |
| `0x44DCD0` | `AIFunction_SetAlarmViewLength` | **no** | - |
| `0x44DD30` | `AIFunction_SetViewAlpha` | **no** | - |
| `0x44DD90` | `AIFunction_SetViewGamma` | **no** | - |
| `0x44DDF0` | `AIFunction_SetSecondaryViewLength` | **no** | - |
| `0x44DE80` | `AIFunction_SetSecondaryAlarmViewLength` | **no** | - |
| `0x44DEE0` | `AIFunction_SetSecondaryViewAlpha` | **no** | - |
| `0x44DF40` | `AIFunction_SetSecondaryViewGamma` | **no** | - |
| `0x44DFA0` | `AIFunction_GetCurrentEventType` | **no** | - |
| `0x44E000` | `AIFunction_GetRandomValue` | **no** | - |
| `0x44E060` | `AIFunction_DefaultHandler` | **no** | - |
| `0x44E0C0` | `AIFunction_SetEventPriority` | **no** | - |
| `0x44E150` | `AIFunction_SetInvulnerability` | **no** | - |
| `0x44E1B0` | `AIFunction_SetInstantDeath` | **no** | - |
| `0x44E210` | `AIFunction_SetDeathAnimation` | **no** | - |
| `0x44E270` | `AIFunction_SetAlarmTriggerID` | **no** | - |
| `0x44E2D0` | `AIFunction_SetAlarmControlID` | **no** | - |
| `0x44E330` | `AIFunction_SetAlarmAccess` | **no** | - |
| `0x44E390` | `AIFunction_SetGunnerID` | **no** | - |
| `0x44E3F0` | `AIFunction_GetAlarmTriggerID` | **no** | - |
| `0x44E430` | `AIFunction_GetAlarmControlID` | **no** | - |
| `0x44E470` | `AIFunction_GetAlarmAccess` | **no** | - |
| `0x44E4B0` | `AIFunction_GetGunnerID` | **no** | - |
| `0x44E4F0` | `AIFunction_GetAlarmControlStatus` | **no** | - |
| `0x44E5E0` | `AIFunction_GetGunnerStatus` | **no** | - |
| `0x44E6D0` | `AIFunction_SetScriptIntegerValue` | **no** | - |
| `0x44E740` | `AIFunction_GetScriptIntegerValue` | **no** | - |
| `0x44E790` | `AIFunction_SetScriptRealValue` | **no** | - |
| `0x44E800` | `AIFunction_GetScriptRealValue` | **no** | - |
| `0x44E860` | `AIFunction_IsEventBehind` | **no** | - |
| `0x44E930` | `AIFunction_GetEventDistance` | **no** | - |
| `0x44E9B0` | `AIFunction_GetAlarmTriggerDistance` | **no** | - |
| `0x44EAF0` | `AIFunction_SetAnimationInterval` | **no** | - |
| `0x44EBB0` | `AIFunction_AddAnimationEntry` | **no** | - |
| `0x44ECB0` | `AIFunction_GetAnimationToPlay` | **no** | - |
| `0x44EE40` | `AIFunction_SendResponse` | **no** | - |
| `0x47CAD0` | `DefineAmmoType` | yes | DefineAmmoType |
| `0x484F60` | `DefineMission` | yes | DefineMission |
| `0x485AD0` | `StatusMessageScreen_ClearMessages` | yes | StatusMessageClear |
| `0x488E00` | `LevelFlow_GetBreakCutSceneKey` | **no** | - |
| `0x488E50` | `LevelFlow_LevelFailed` | **no** | - |
| `0x488E90` | `LevelFlow_IsCountryUSA` | **no** | - |
| `0x4B8C10` | `BreakScript` | yes | BreakScript |
| `0x4B8C60` | `_Script_SetWarningLevel` | yes | _Script_SetWarningLevel |
| `0x4C4520` | `DefineMagicObj` | yes | DefineMagicObj |
| `0x4C6760` | `CreateTerrainTileMap` | yes | Terrain_CreateTileMap |
| `0x4E8C60` | `DefineSound` | yes | DefineSound |
| `0x4E9EC0` | `DefineTriggerOnce` | yes | DefineTriggerOnce |
| `0x4EAD20` | `DefineGraph` | yes | DefineGraph |
| `0x4EBA90` | `DefineGroup` | yes | DefineGroup |
| `0x4F0740` | `Task_DeclareParameters` | **no** | - |
| `0x4F0780` | `Task_New` | **no** | - |

## Undecoded call sites

- `0x403542` -> registrar `0x4B8890`, pushes decoded: 4
- `0x407DA6` -> registrar `0x4B8890`, pushes decoded: 0
- `0x41001B` -> registrar `0x4B8890`, pushes decoded: 0
- `0x413E52` -> registrar `0x4B8890`, pushes decoded: 0
- `0x45EA44` -> registrar `0x4B8890`, pushes decoded: 0
- `0x4B7E97` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B7EF7` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B7F57` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B7FB7` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B7FD7` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B804A` -> registrar `0x4BAAC0`, pushes decoded: 4
- `0x4B8827` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B8887` -> registrar `0x4BAAC0`, pushes decoded: 5
- `0x4B88AA` -> registrar `0x4BAAC0`, pushes decoded: 0
- `0x4B88DA` -> registrar `0x4BAAC0`, pushes decoded: 0
- `0x4B890A` -> registrar `0x4BAAC0`, pushes decoded: 0
- `0x4C6580` -> registrar `0x4B8890`, pushes decoded: 1
- `0x4EBEA2` -> registrar `0x4B8890`, pushes decoded: 0
- `0x4EDD12` -> registrar `0x4B8890`, pushes decoded: 0
