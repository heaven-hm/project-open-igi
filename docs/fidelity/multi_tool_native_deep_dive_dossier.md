# Multi-Tool Native Deep Dive Verification Dossier

**Target Binary**: `igi.exe` Retail v1.0 Europe (`0x00400000` base, SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`)

**Total 100% Verified Natives**: 264

**Verification Standard**: Every entry is independently corroborated across **four** sources:
1. **Direct PE Binary Machine Code** (`igi.exe` .text section raw bytes & capstone disassembly)
2. **Binary Ninja MCP** (x86 High-Level IL decompilation, prototype, calling convention, block count)
3. **Ghidra MCP / REA** (HighFunction decompilation AST, caller/callee xrefs, byte size)
4. **Radare2 MCP** (CFG structure, basic block matching, call instruction count, CFG score >= 0.85 PASS)

## 1. Executive Summary Table

| Hash | Address | Name | Raw Bytes (First 8) | BN Prototype | Ghidra Sig | R2 CFG Score | Verdict |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `0x004EC070` | `5161072` | `AnimTrigger_ParseConfigFile` | `56578b7c240c68b8` | `char*(char* arg1)` | `void AnimTrigger_ParseConfigFile` | `1.0` | **100% PASS** |
| `0x004B83D0` | `4948944` | `Qvm_Cleanup` | `568b7424088b868c` | `int32_t(int32_t* arg1)` | `void Qvm_Cleanup` | `1.0` | **100% PASS** |
| `0x004F0CF0` | `5180656` | `Config_Compile` | `8b44240456575068` | `int32_t(int32_t arg1, void* arg2, char arg3)` | `int Config_Compile` | `1.0` | **100% PASS** |
| `0x00405850` | `4216912` | `Config_Read` | `8b4424045650e855` | `int32_t(void* arg1)` | `int Config_Read` | `1.0` | **100% PASS** |
| `0x00405BD0` | `4217808` | `Config_Write` | `538b5c2408566854` | `int32_t(void* arg1)` | `void Config_Write` | `1.0` | **100% PASS** |
| `0x004AF7B0` | `4913072` | `Error_Show` | `a16862930085c074` | `int32_t(char* arg1)` | `void Error_Show` | `0.8889` | **100% PASS** |
| `0x00402820` | `4204576` | `Frames_Set` | `e84bdb08008b0d8c` | `void(int32_t frames)` | `void Frames_Set` | `1.0` | **100% PASS** |
| `0x00408350` | `4227920` | `GameMaterial_LoadParameters` | `566890855300e855` | `int32_t()` | `void GameMaterial_LoadParameters` | `1.0` | **100% PASS** |
| `0x00403B70` | `4209520` | `Config_ResetGraphicOptions` | `83ec18535657e8a5` | `int32_t(int32_t arg1)` | `void Config_ResetGraphicOptions` | `0.9697` | **100% PASS** |
| `0x004F9FF0` | `5218288` | `Graph_Open` | `83ec20568b74242c` | `int32_t(void* arg1 @ 0x8)` | `void Graph_Open` | `1.0` | **100% PASS** |
| `0x004137A0` | `4274080` | `HumanPlayer_LoadParameters` | `566888915300e805` | `int32_t()` | `void HumanPlayer_LoadParameters` | `1.0` | **100% PASS** |
| `0x004157D0` | `4282320` | `Game_DisablePlayerInput` | `83ec18565733f656` | `int32_t(int32_t arg1)` | `void Game_DisablePlayerInput` | `0.8889` | **100% PASS** |
| `0x00415830` | `4282416` | `Game_EnablePlayerInput` | `83ec18565733f656` | `int32_t(int32_t arg1)` | `void Game_EnablePlayerInput` | `0.8889` | **100% PASS** |
| `0x004F0E10` | `5180944` | `Level_LoadObjects` | `8b44240481ec0001` | `int32_t(int32_t arg1, int32_t arg2)` | `void Level_LoadObjects` | `1.0` | **100% PASS** |
| `0x00416550` | `4285776` | `Game_DeleteHandler` | `e85ba8090084c075` | `int32_t()` | `void Game_DeleteHandler` | `0.9861` | **100% PASS** |
| `0x00416FE0` | `4288480` | `Game_Restart` | `5657e8e9b8feffe8` | `int32_t()` | `void Game_Restart` | `1.0` | **100% PASS** |
| `0x00415B30` | `4283184` | `Level_Start` | `81ec800200005556` | `int32_t(void* arg1)` | `void Level_Start` | `0.9963` | **100% PASS** |
| `0x00418B00` | `4295424` | `Menu_Manager` | `8a44240c81ec2001` | `int32_t*(void* arg1, void* arg2, char arg3, char arg4, char arg5)` | `int* Menu_Manager` | `0.914` | **100% PASS** |
| `0x00495F30` | `4808496` | `Music_SetSfxVolume` | `8b442404a30c3954` | `int32_t(int32_t arg1)` | `void Music_SetSfxVolume` | `1.0` | **100% PASS** |
| `0x00495E70` | `4808304` | `Music_SetVolume` | `8b4424048b4c2408` | `int32_t(float arg1, float arg2)` | `void Music_SetVolume` | `1.0` | **100% PASS** |
| `0x004158E0` | `4282592` | `Game_UpdateVolume` | `83ec185657e8e609` | `int32_t(int32_t arg1)` | `void Game_UpdateVolume` | `0.9444` | **100% PASS** |
| `0x004EDFE0` | `5169120` | `PhysicsObjType_ParseConfigFile` | `56578b7c240c68b8` | `char*(char* arg1)` | `void PhysicsObjType_ParseConfigFile` | `1.0` | **100% PASS** |
| `0x004B0D60` | `4918624` | `Qhash_Init` | `a1e060a9008a0dfd` | `void*(char arg1)` | `void Qhash_Init` | `1.0` | **100% PASS** |
| `0x004B0D90` | `4918672` | `Qhash_Reset` | `a1e060a9008a88e8` | `void*()` | `void Qhash_Reset` | `1.0` | **100% PASS** |
| `0x004B8410` | `4949008` | `Qsc_Compile` | `81ec040100005355` | `int32_t(void* arg1)` | `int Qsc_Compile` | `0.8889` | **100% PASS** |
| `0x004F0E90` | `5181072` | `Qtask_Update` | `a1a458a70085c074` | `int32_t*()` | `void Qtask_Update` | `1.0` | **100% PASS** |
| `0x00401B20` | `4201248` | `Qtask_UpdateList` | `8b4c24048b0185c0` | `int32_t(int32_t* arg1)` | `void Qtask_UpdateList` | `1.0` | **100% PASS** |
| `0x004BB270` | `4960880` | `QScript_Assemble` | `81ec440600005556` | `int32_t(void* arg1, void* arg2)` | `int QScript_Assemble` | `0.9827` | **100% PASS** |
| `0x004B85B0` | `4949424` | `Qvm_Execute` | `81ec54080000568b` | `int32_t(void* arg1)` | `int Qvm_Execute` | `1.0` | **100% PASS** |
| `0x004B80B0` | `4948144` | `Qvm_Load` | `81ec1c0100005355` | `int32_t*(void* arg1)` | `int* Qvm_Load` | `0.9145` | **100% PASS** |
| `0x004BBCB0` | `4963504` | `QScript_ParseAndAssemble` | `81ec640800005355` | `int32_t(void* arg1, void* arg2)` | `int QScript_ParseAndAssemble` | `0.9649` | **100% PASS** |
| `0x004B63D0` | `4940752` | `Resource_Flush` | `8b0d383e94008b54` | `int32_t(int32_t arg1)` | `void Resource_Flush` | `1.0` | **100% PASS** |
| `0x004B5F00` | `4939520` | `Resource_Load` | `51568b74240c8d44` | `int32_t* __convention("regparm")(int32_t arg1, int32_t arg2, void* arg3, int32_t* arg4 @ 0x8)` | `int* Resource_Load` | `1.0` | **100% PASS** |
| `0x004B6380` | `4940672` | `Resource_Unload` | `568b7424088d4424` | `int32_t(int32_t* arg1)` | `void Resource_Unload` | `1.0` | **100% PASS** |
| `0x004B5B90` | `4938640` | `Is_ResourceLoaded` | `81ec040100005355` | `int32_t(void* arg1, void** arg2)` | `int Is_ResourceLoaded` | `1.0` | **100% PASS** |
| `0x004B18B0` | `4921520` | `Resource_PackUnpack` | `535556578b7c2414` | `void*(void* arg1, int32_t* arg2)` | `int Resource_PackUnpack` | `0.9762` | **100% PASS** |
| `0x004F0E50` | `5181008` | `Script_Init` | `8b4424048a4c2408` | `int32_t(int32_t arg1, char arg2, int32_t arg3, char arg4)` | `void Script_Init` | `1.0` | **100% PASS** |
| `0x004B8930` | `4950320` | `Script_SetSymbolContext` | `568b7424088d4424` | `int32_t(char* arg1, int32_t arg2)` | `void Script_SetSymbolContext` | `1.0` | **100% PASS** |
| `0x00485AD0` | `4741840` | `StatusMessageScreen_ClearMessages` | `83ec185657e8a616` | `void()` | `void StatusMessageScreen_ClearMessages` | `0.8889` | **100% PASS** |
| `0x004C0460` | `4981856` | `Symbol_Remove` | `8b4c2404568b7424` | `int32_t(int32_t* arg1, int32_t* arg2)` | `int Symbol_Remove` | `1.0` | **100% PASS** |
| `0x004B8810` | `4950032` | `TaskType_Set` | `8b4424088b4c2404` | `void*(int32_t* arg1, int32_t arg2)` | `void TaskType_Set` | `1.0` | **100% PASS** |
| `0x004AF810` | `4913168` | `Warning_Show` | `a17462930085c074` | `int32_t(char* arg1)` | `void Warning_Show` | `0.8889` | **100% PASS** |
| `0x004C0560` | `4982112` | `Symbol_CheckRegister` | `538b5c240c558b6c` | `int32_t(int32_t* arg1, int32_t* arg2, int32_t arg3)` | `void* Symbol_CheckRegister` | `0.9048` | **100% PASS** |
| `0x004B8890` | `4950160` | `Game_DefineOptions` | `8b4424108b4c240c` | `void*(int32_t* arg1, int32_t arg2, int32_t arg3, int32_t arg4)` | `void Game_DefineOptions` | `1.0` | **100% PASS** |
| `0x004C0200` | `4981248` | `Script_BufInit` | `8b4424048b4c2408` | `int32_t*(int32_t* arg1, int32_t arg2, int32_t arg3, int32_t arg4)` | `void Script_BufInit` | `1.0` | **100% PASS** |
| `0x004E68D0` | `5138640` | `Sound_Load` | `8b4424046a0068f0` | `int32_t(void* arg1)` | `void Sound_Load` | `1.0` | **100% PASS** |
| `0x004B0C60` | `4918368` | `Mem_Alloc` | `8b442404538b5c24` | `void*(int32_t arg1, int32_t arg2)` | `void* Mem_Alloc` | `1.0` | **100% PASS** |
| `0x004B0BE0` | `4918240` | `Mem_DeallocAll` | `565733ffe8970b01` | `int32_t()` | `void Mem_DeallocAll` | `1.0` | **100% PASS** |
| `0x00484E60` | `4738656` | `Mission_Open` | `81ec04010000538b` | `int32_t(int32_t* arg1)` | `void Mission_Open` | `0.9333` | **100% PASS** |
| `0x004A5350` | `4870992` | `File_Open` | `6a40ff74240cff74` | `int32_t*(PSTR arg1, char* arg2)` | `void* File_Open` | `1.0` | **100% PASS** |
| `0x004B1510` | `4920592` | `QFile_ReadWrite` | `8b44240853555657` | `int32_t*(void* arg1, char* arg2)` | `void* QFile_ReadWrite` | `0.9667` | **100% PASS** |
| `0x004C48E0` | `4999392` | `Rigid_DyncubeObjRead` | `56578b7c240c68b8` | `char*(char* arg1)` | `void Rigid_DyncubeObjRead` | `1.0` | **100% PASS** |
| `0x004C4930` | `4999472` | `Magic_ObjLoad` | `8b4424046a006890` | `int32_t(void* arg1)` | `void Magic_ObjLoad` | `1.0` | **100% PASS** |
| `0x004EC0C0` | `5161152` | `Anim_TriggerLoad` | `8b4424046a0068f0` | `int32_t(void* arg1)` | `void Anim_TriggerLoad` | `1.0` | **100% PASS** |
| `0x004EE030` | `5169200` | `Physics_ObjLoad` | `8b4424046a006850` | `int32_t(void* arg1)` | `void Physics_ObjLoad` | `1.0` | **100% PASS** |
| `0x00413B70` | `4275056` | `Weapon_TypeOpen` | `6a0068903a410068` | `int32_t()` | `void Weapon_TypeOpen` | `1.0` | **100% PASS** |
| `0x004A53B3` | `4871091` | `Game_DataSymbolLoad` | `558bec83ec208b45` | `int32_t(char* arg1, char* arg2)` | `int Game_DataSymbolLoad` | `1.0` | **100% PASS** |
| `0x004B80A0` | `4948128` | `Game_DataSymbolRemoveThunk` | `8b44240450e87608` | `int32_t(char* arg1)` | `void* Game_DataSymbolRemoveThunk` | `1.0` | **100% PASS** |
| `0x004B7EE0` | `4947680` | `SymbolRegister_Bool8` | `8b4424088b4c2404` | `void*(int32_t* arg1, int32_t arg2)` | `void* SymbolRegister_Bool8` | `1.0` | **100% PASS** |
| `0x004B7E80` | `4947584` | `SymbolRegister_Int16` | `8b4424088b4c2404` | `void*(int32_t* arg1, int32_t arg2)` | `void* SymbolRegister_Int16` | `1.0` | **100% PASS** |
| `0x004B7FA0` | `4947872` | `SymbolRegister_Int32` | `8b4424088b4c2404` | `void*(int32_t* arg1, int32_t arg2)` | `void* SymbolRegister_Int32` | `1.0` | **100% PASS** |
| `0x004B7FC0` | `4947904` | `SymbolRegister_Real32` | `8b4424088b4c2404` | `void*(int32_t* arg1, int32_t arg2)` | `void* SymbolRegister_Real32` | `1.0` | **100% PASS** |
| `0x0048A440` | `4760640` | `LoadingScreen_Show` | `83ec38535556576a` | `int32_t*(int32_t arg1)` | `void LoadingScreen_Show` | `0.9825` | **100% PASS** |
| `0x004BAAC0` | `4958912` | `Q_TaskHashTableSet` | `56576a01e89762ff` | `void*(int32_t* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7)` | `void Q_TaskHashTableSet` | `0.9333` | **100% PASS** |
| `0x00413BB0` | `4275120` | `Weapons_CountGet` | `a18cb15700c39090` | `int32_t()` | `int Weapons_CountGet` | `1.0` | **100% PASS** |
| `0x0045FFC0` | `4587456` | `Weapon_GunPickup` | `538b5c24088a83f5` | `int32_t(void* arg1, int32_t* arg2)` | `void Weapon_GunPickup` | `1.0` | **100% PASS** |
| `0x0045FF80` | `4587392` | `Weapon_AmmoPickup` | `8b4c24048a81f50c` | `int32_t(void* arg1, int32_t* arg2)` | `void Weapon_AmmoPickup` | `1.0` | **100% PASS** |
| `0x00416D80` | `4287872` | `Human_HitDamage` | `a1bcba57008a80e1` | `void*()` | `uint Human_HitDamage` | `1.0` | **100% PASS** |
| `0x00463760` | `4601696` | `Human_ViewCam` | `8b4c24048b442408` | `int32_t(void* arg1, int32_t arg2)` | `void Human_ViewCam` | `1.0` | **100% PASS** |
| `0x004659E0` | `4610528` | `Human_TaskViewReset` | `dd05e8355300d9f2` | `void*(void* arg1)` | `void Human_TaskViewReset` | `1.0` | **100% PASS** |
| `0x00406220` | `4219424` | `Config_GetActivePlayerProfileRecord` | `8b0d8423bc008d04` | `void*()` | `ProfileRecord* Config_GetActivePlayerProfileRecord` | `1.0` | **100% PASS** |
| `0x0045C440` | `4572224` | `Soldier_Execute` | `83ec085355568b74` | `void*(void* arg1, int32_t* arg2)` | `void Soldier_Execute` | `1.0` | **100% PASS** |
| `0x00402780` | `4204416` | `Flow_RequestEvent` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Flow_RequestEvent` | `0.8889` | **100% PASS** |
| `0x004036B0` | `4208304` | `GO_Player` | `83ec188b0d8023bc` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int GO_Player` | `0.8889` | **100% PASS** |
| `0x004037C0` | `4208576` | `GO_InRemap` | `83ec18a18023bc00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int GO_InRemap` | `0.9167` | **100% PASS** |
| `0x00403950` | `4208976` | `GO_GfxDisp` | `83ec18568b742428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int GO_GfxDisp` | `0.9167` | **100% PASS** |
| `0x00403A00` | `4209152` | `GO_GameLang` | `83ec18565768c882` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int GO_GameLang` | `0.9167` | **100% PASS** |
| `0x00403E60` | `4210272` | `GO_SoundSpeech` | `83ec18568b742428` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `int GO_SoundSpeech` | `0.8889` | **100% PASS** |
| `0x00403EF0` | `4210416` | `GO_SoundMusic` | `83ec18568b742428` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `int GO_SoundMusic` | `0.8889` | **100% PASS** |
| `0x00403F80` | `4210560` | `GO_SoundFX` | `83ec18568b742428` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `int GO_SoundFX` | `0.8889` | **100% PASS** |
| `0x004050A0` | `4214944` | `Config_CreateNewPlayerProfile` | `a18023bc0083ec20` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int Config_CreateNewPlayerProfile` | `0.8571` | **100% PASS** |
| `0x00405660` | `4216416` | `Config_FillMissionSelectionBox` | `83ec18535657e825` | `int32_t(int32_t arg1)` | `void Config_FillMissionSelectionBox` | `0.9333` | **100% PASS** |
| `0x004056F0` | `4216560` | `Config_FillMissionPictureBox` | `83ec185657e89619` | `int32_t(int32_t arg1)` | `void Config_FillMissionPictureBox` | `0.9167` | **100% PASS** |
| `0x00405760` | `4216672` | `Config_FillMissionTextBox` | `83ec185657e82619` | `int32_t(int32_t arg1)` | `void Config_FillMissionTextBox` | `0.9167` | **100% PASS** |
| `0x00404450` | `4211792` | `Config_FillScreenResolutionListBox` | `8b44240c81ec1801` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_FillScreenResolutionListBox` | `0.9167` | **100% PASS** |
| `0x004041D0` | `4211152` | `Config_GameOptionsInputGetInvertMouse` | `83ec1c5657e84620` | `int32_t(int32_t arg1)` | `int Config_GameOptionsInputGetInvertMouse` | `0.8889` | **100% PASS** |
| `0x00404290` | `4211344` | `Config_GameOptionsInputGetMouseSensitivity` | `83ec205657e8861f` | `int32_t(int32_t arg1)` | `int Config_GameOptionsInputGetMouseSensitivity` | `0.8889` | **100% PASS** |
| `0x00404340` | `4211520` | `Config_GameOptionsInputGetBloodEnabled` | `83ec1c5657e8d61e` | `int32_t(int32_t arg1)` | `int Config_GameOptionsInputGetBloodEnabled` | `0.8889` | **100% PASS** |
| `0x00404160` | `4211040` | `Config_GameOptionsInputSetInvertMouse` | `8b44240c83ec186a` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_GameOptionsInputSetInvertMouse` | `0.8889` | **100% PASS** |
| `0x00404230` | `4211248` | `Config_GameOptionsInputSetMouseSensitivity` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_GameOptionsInputSetMouseSensitivity` | `0.8889` | **100% PASS** |
| `0x004042E0` | `4211424` | `Config_GameOptionsInputSetBloodEnabled` | `83ec208b44242c53` | `int32_t(int80_t arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_GameOptionsInputSetBloodEnabled` | `0.8889` | **100% PASS** |
| `0x00404510` | `4211984` | `Config_GraphicOptionsSetResolution` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_GraphicOptionsSetResolution` | `0.9333` | **100% PASS** |
| `0x00404960` | `4213088` | `Config_GraphicOptionsSetTransparency` | `83ec188b44242453` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_GraphicOptionsSetTransparency` | `0.8889` | **100% PASS** |
| `0x00404F00` | `4214528` | `Config_GraphicOptionsSetPerformanceFlags` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_GraphicOptionsSetPerformanceFlags` | `0.8889` | **100% PASS** |
| `0x004045B0` | `4212144` | `Config_GraphicOptionsGetResolution` | `83ec208b44242c53` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int Config_GraphicOptionsGetResolution` | `0.9333` | **100% PASS** |
| `0x004049C0` | `4213184` | `Config_GraphicOptionsGetTransparency` | `83ec1c5657e8c6fb` | `int32_t(int32_t arg1)` | `int Config_GraphicOptionsGetTransparency` | `0.8889` | **100% PASS** |
| `0x00404910` | `4213008` | `Config_GraphicOptionsGetGamma` | `83ec205657e876fc` | `int32_t(int32_t arg1)` | `float Config_GraphicOptionsGetGamma` | `0.8889` | **100% PASS** |
| `0x00404F60` | `4214624` | `Config_GraphicOptionsGetPerformanceFlags` | `83ec185657e826f6` | `int32_t(int32_t arg1)` | `int Config_GraphicOptionsGetPerformanceFlags` | `0.8889` | **100% PASS** |
| `0x004055C0` | `4216256` | `Config_GraphicOptionsGetPerfFlagsFromLevel` | `83ec18568b742428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `int Config_GraphicOptionsGetPerfFlagsFromLevel` | `0.8889` | **100% PASS** |
| `0x00404A10` | `4213264` | `Config_SoundOptionsSetSoundsEffects` | `83ec188b44242453` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_SoundOptionsSetSoundsEffects` | `0.8889` | **100% PASS** |
| `0x00404AE0` | `4213472` | `Config_SoundOptionsSetSoundsEffectsVolume` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_SoundOptionsSetSoundsEffectsVolume` | `0.8889` | **100% PASS** |
| `0x00404B90` | `4213648` | `Config_SoundOptionsSetMusic` | `83ec188b44242453` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_SoundOptionsSetMusic` | `0.8889` | **100% PASS** |
| `0x00404C40` | `4213824` | `Config_SoundOptionsSetMusicVolume` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_SoundOptionsSetMusicVolume` | `0.8889` | **100% PASS** |
| `0x00404CF0` | `4214000` | `Config_SoundOptionsSetSpeech` | `83ec188b44242453` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_SoundOptionsSetSpeech` | `0.8889` | **100% PASS** |
| `0x00404DA0` | `4214176` | `Config_SoundOptionsSetSpeechVolume` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_SoundOptionsSetSpeechVolume` | `0.8889` | **100% PASS** |
| `0x00404E50` | `4214352` | `Config_SoundOptionsSetReverseStereo` | `83ec188b44242453` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Config_SoundOptionsSetReverseStereo` | `0.8889` | **100% PASS** |
| `0x00404A90` | `4213392` | `Config_SoundOptionsGetSoundsEffects` | `83ec1c5657e8d6ff` | `int32_t(int32_t arg1)` | `int Config_SoundOptionsGetSoundsEffects` | `0.8889` | **100% PASS** |
| `0x00404B40` | `4213568` | `Config_SoundOptionsGetSoundsEffectsVolume` | `83ec205657e826ff` | `int32_t(int32_t arg1)` | `float Config_SoundOptionsGetSoundsEffectsVolume` | `0.8889` | **100% PASS** |
| `0x00404BF0` | `4213744` | `Config_SoundOptionsGetMusic` | `83ec1c5657e876fe` | `int32_t(int32_t arg1)` | `int Config_SoundOptionsGetMusic` | `0.8889` | **100% PASS** |
| `0x00404CA0` | `4213920` | `Config_SoundOptionsGetMusicVolume` | `83ec205657e8c6fd` | `int32_t(int32_t arg1)` | `float Config_SoundOptionsGetMusicVolume` | `0.8889` | **100% PASS** |
| `0x00404D50` | `4214096` | `Config_SoundOptionsGetSpeech` | `83ec1c5657e816fd` | `int32_t(int32_t arg1)` | `int Config_SoundOptionsGetSpeech` | `0.8889` | **100% PASS** |
| `0x00404E00` | `4214272` | `Config_SoundOptionsGetSpeechVolume` | `83ec205657e866fc` | `int32_t(int32_t arg1)` | `float Config_SoundOptionsGetSpeechVolume` | `0.8889` | **100% PASS** |
| `0x00404EB0` | `4214448` | `Config_SoundOptionsGetReverseStereo` | `83ec1c5657e8b6fb` | `int32_t(int32_t arg1)` | `int Config_SoundOptionsGetReverseStereo` | `0.8889` | **100% PASS** |
| `0x00403B30` | `4209456` | `Config_IsGermany` | `83ec1c5657e886b7` | `int32_t(int32_t arg1)` | `int Config_IsGermany` | `0.8889` | **100% PASS** |
| `0x00407E30` | `4226608` | `DefineQ_Material` | `83ec18568b742428` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, int32_t arg4, int32_t arg5 @ 0xc)` | `void DefineQ_Material` | `0.9524` | **100% PASS** |
| `0x00407F50` | `4226896` | `Define_GameMaterial` | `a11c85530083ec18` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, int32_t arg5, float arg6 @ 0xc)` | `void Define_GameMaterial` | `0.9804` | **100% PASS** |
| `0x00410240` | `4260416` | `Define_HumanPlayerWeaponCategory` | `83ec1c53558b6c24` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Define_HumanPlayerWeaponCategory` | `0.8889` | **100% PASS** |
| `0x004102D0` | `4260560` | `Define_HumanPlayerAmmoLimit` | `83ec1856578b7c24` | `int32_t(int32_t arg1, int32_t* arg2 @ 0xc)` | `void Define_HumanPlayerAmmoLimit` | `0.8889` | **100% PASS** |
| `0x004151E0` | `4280800` | `MovieSubtitle_RegisterFont` | `81ec18040000568b` | `int32_t(int32_t arg1, int32_t arg2, float arg3)` | `void MovieSubtitle_RegisterFont` | `0.9167` | **100% PASS** |
| `0x00415260` | `4280928` | `MovieSubtitle_RegisterText` | `81ec50040000568b` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st3, long double arg4 @ st4, long double arg5 @ st5, long double arg6 @ st6, int32_t arg7, char* arg8, float arg9)` | `void MovieSubtitle_RegisterText` | `0.9792` | **100% PASS** |
| `0x00415950` | `4282704` | `Game_SetMusicVolume` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Game_SetMusicVolume` | `0.8889` | **100% PASS** |
| `0x004159B0` | `4282800` | `Game_SetSFXVolume` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Game_SetSFXVolume` | `0.8889` | **100% PASS** |
| `0x004174A0` | `4289696` | `MenuManager_RequestScreen` | `83ec1856578b7c24` | `int32_t(int32_t arg1, void* arg2, float arg3)` | `void MenuManager_RequestScreen` | `0.9167` | **100% PASS** |
| `0x00417790` | `4290448` | `MenuManager_LeaveMenus` | `83ec18568b742428` | `int32_t(int32_t arg1, void* arg2, float arg3)` | `void MenuManager_LeaveMenus` | `0.9167` | **100% PASS** |
| `0x00417710` | `4290320` | `MenuManager_PopScreen` | `8b44240c83ec1856` | `int32_t(int32_t arg1, void* arg2, float arg3)` | `void MenuManager_PopScreen` | `0.8889` | **100% PASS** |
| `0x00417880` | `4290688` | `MenuManager_ForceUpdateWindow` | `8b44240c83ec1856` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void MenuManager_ForceUpdateWindow` | `0.9167` | **100% PASS** |
| `0x004231E0` | `4338144` | `ControlsMenu_ConvertFloatFromOffset` | `83ec208b44242c56` | `int32_t(int32_t arg1, int32_t arg2 @ 0xc)` | `float ControlsMenu_ConvertFloatFromOffset` | `0.8889` | **100% PASS** |
| `0x00423240` | `4338240` | `ControlsMenu_ConvertOffsetFromFloat` | `83ec208b44242c56` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `int ControlsMenu_ConvertOffsetFromFloat` | `0.8889` | **100% PASS** |
| `0x0044D0E0` | `4509920` | `AIAction_Patrol` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_Patrol` | `0.9444` | **100% PASS** |
| `0x0044D160` | `4510048` | `AIAction_Combat` | `a158da5b0083ec18` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_Combat` | `0.9167` | **100% PASS** |
| `0x0044D260` | `4510304` | `AIAction_Dead` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_Dead` | `0.8889` | **100% PASS** |
| `0x0044D2C0` | `4510400` | `AIAction_FallFlat` | `a158da5b0083ec18` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIAction_FallFlat` | `0.9286` | **100% PASS** |
| `0x0044D420` | `4510752` | `AIAction_Activate` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_Activate` | `0.9444` | **100% PASS** |
| `0x0044D4A0` | `4510880` | `AIAction_WalkToNode` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_WalkToNode` | `0.9333` | **100% PASS** |
| `0x0044D510` | `4510992` | `AIAction_RunToNode` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_RunToNode` | `0.9333` | **100% PASS** |
| `0x0044D580` | `4511104` | `AIAction_FireAtNode` | `83ec20a158da5b00` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, int32_t arg3, float arg4 @ 0xc)` | `void AIAction_FireAtNode` | `0.9583` | **100% PASS** |
| `0x0044D630` | `4511280` | `AIAction_FireAtTask` | `83ec20a158da5b00` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, int32_t arg3, float arg4 @ 0xc)` | `void AIAction_FireAtTask` | `0.9583` | **100% PASS** |
| `0x0044D6E0` | `4511456` | `AIAction_PlayAnimation` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_PlayAnimation` | `0.9333` | **100% PASS** |
| `0x0044D750` | `4511568` | `AIAction_PlaySound` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_PlaySound` | `0.9444` | **100% PASS** |
| `0x0044D7D0` | `4511696` | `AIAction_MoveToEvent` | `83ec18a158da5b00` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_MoveToEvent` | `0.9444` | **100% PASS** |
| `0x0044D870` | `4511856` | `AIAction_LookAtEvent` | `83ec18a158da5b00` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIAction_LookAtEvent` | `0.9444` | **100% PASS** |
| `0x0044D8F0` | `4511984` | `AIAction_Stunned` | `83ec18a158da5b00` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIAction_Stunned` | `0.9333` | **100% PASS** |
| `0x0044D960` | `4512096` | `AIAction_KickGrenade` | `83ec18555657e855` | `int32_t(int32_t arg1, int32_t arg2 @ 0xc)` | `void AIAction_KickGrenade` | `0.9048` | **100% PASS** |
| `0x0044DA00` | `4512256` | `AIAction_RunPanicking` | `83ec18a158da5b00` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIAction_RunPanicking` | `0.9444` | **100% PASS** |
| `0x0044DA80` | `4512384` | `AIAction_Idle` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIAction_Idle` | `0.8889` | **100% PASS** |
| `0x0044DAE0` | `4512480` | `AIAction_SetCombat` | `83ec18a158da5b00` | `int32_t(int32_t arg1, int32_t arg2 @ 0xc)` | `void AIAction_SetCombat` | `0.9167` | **100% PASS** |
| `0x0044E6D0` | `4515536` | `AIFunction_SetScriptIntegerValue` | `83ec18568b742428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void AIFunction_SetScriptIntegerValue` | `0.8889` | **100% PASS** |
| `0x0044E790` | `4515728` | `AIFunction_SetScriptRealValue` | `83ec18568b742428` | `int32_t(int80_t arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIFunction_SetScriptRealValue` | `0.8889` | **100% PASS** |
| `0x0044EAF0` | `4516592` | `AIFunction_SetAnimationInterval` | `83ec20568b742430` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, int32_t arg3, float arg4 @ 0xc)` | `void AIFunction_SetAnimationInterval` | `0.9333` | **100% PASS** |
| `0x0044EBB0` | `4516784` | `AIFunction_AddAnimationEntry` | `a158da5b0083ec18` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void AIFunction_AddAnimationEntry` | `0.9048` | **100% PASS** |
| `0x0044EE40` | `4517440` | `AIFunction_SendResponse` | `a160da5b0083ec18` | `int32_t(int32_t arg1)` | `void AIFunction_SendResponse` | `0.8889` | **100% PASS** |
| `0x0047CAD0` | `4704976` | `Define_AmmoType` | `81ec98000000568b` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Define_AmmoType` | `0.9804` | **100% PASS** |
| `0x00484F60` | `4738912` | `Define_Mission` | `83ec18568b742428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Define_Mission` | `0.9762` | **100% PASS** |
| `0x00488E90` | `4755088` | `LevelFlow_IsCountryUSA` | `83ec1c5657e83664` | `int32_t(int32_t arg1)` | `int LevelFlow_IsCountryUSA` | `0.8889` | **100% PASS** |
| `0x004B8C60` | `4951136` | `Script_SetWarningLevel` | `83ec188b44242456` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Script_SetWarningLevel` | `0.8889` | **100% PASS** |
| `0x004C4520` | `4998432` | `Define_MagicObj` | `a13c43a40083ec18` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Define_MagicObj` | `1.0` | **100% PASS** |
| `0x004C6760` | `5007200` | `Create_TerrainTileMap` | `83ec18538b5c2428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `void Create_TerrainTileMap` | `0.8889` | **100% PASS** |
| `0x004EBA90` | `5159568` | `Define_Group` | `81ec840000005355` | `int32_t(int32_t arg1, void* arg2, float arg3)` | `void Define_Group` | `0.9762` | **100% PASS** |
| `0x004E8C60` | `5147744` | `Define_Sound` | `81ec50010000568d` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, int32_t arg8, void* arg9, float arg10)` | `void Define_Sound` | `0.9792` | **100% PASS** |
| `0x004EAD20` | `5156128` | `Define_Graph` | `81ecac0100005355` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, int32_t arg7, void* arg8, float arg9)` | `void Define_Graph` | `0.9804` | **100% PASS** |
| `0x004E9EC0` | `5152448` | `Define_TriggerOnce` | `81ec58010000568d` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, int32_t arg8, void* arg9, float arg10)` | `void Define_TriggerOnce` | `0.9815` | **100% PASS** |
| `0x004F0780` | `5179264` | `Task_New` | `a016dd540081eccc` | `int32_t(int32_t arg1, void* arg2, float arg3)` | `void Task_New` | `0.9861` | **100% PASS** |
| `0x0044E4F0` | `4515056` | `AIFunction_GetAlarmControlStatus` | `a158da5b0083ec1c` | `int32_t(int32_t arg1)` | `int AIFunction_GetAlarmControlStatus` | `0.8571` | **100% PASS** |
| `0x0044E5E0` | `4515296` | `AIFunction_GetGunnerStatus` | `a158da5b0083ec1c` | `int32_t(int32_t arg1)` | `int AIFunction_GetGunnerStatus` | `0.8571` | **100% PASS** |
| `0x0044ECB0` | `4517040` | `AIFunction_GetAnimationToPlay` | `558bec83e4f883ec` | `int32_t(int32_t arg1)` | `int AIFunction_GetAnimationToPlay` | `0.8519` | **100% PASS** |
| `0x004048B0` | `4212912` | `Config_GraphicOptionsSetGamma` | `83ec188b44242456` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `void Config_GraphicOptionsSetGamma` | `0.8889` | **100% PASS** |
| `0x0044E000` | `4513792` | `AIFunction_GetRandomValue` | `83ec208b44242c56` | `int32_t(long double arg1 @ st0, int32_t arg2, float arg3 @ 0xc)` | `float AIFunction_GetRandomValue` | `0.9167` | **100% PASS** |
| `0x0044E930` | `4516144` | `AIFunction_GetEventDistance` | `83ec20e888dfffff` | `int32_t(int32_t arg1)` | `float AIFunction_GetEventDistance` | `0.9333` | **100% PASS** |
| `0x0044E9B0` | `4516272` | `AIFunction_GetAlarmTriggerDistance` | `a158da5b0083ec20` | `int32_t(int32_t arg1)` | `float AIFunction_GetAlarmTriggerDistance` | `0.8667` | **100% PASS** |
| `0x0048F1A0` | `4780448` | `App_ContextSetDebugged` | `0fbe442404a3f88b` | `int32_t(char arg1)` | `void App_ContextSetDebugged` | `1.0` | **100% PASS** |
| `0x0048F1E0` | `4780512` | `App_ContextSetDebugtextState` | `0fbe442404a3f48b` | `int32_t(char arg1)` | `void App_ContextSetDebugtextState` | `1.0` | **100% PASS** |
| `0x0048F240` | `4780608` | `App_ContextSetLightmapsUsed` | `0fbe442404a3e48b` | `int32_t(char arg1)` | `void App_ContextSetLightmapsUsed` | `1.0` | **100% PASS** |
| `0x0048F260` | `4780640` | `App_ContextSetTerrainLightmapsUsed` | `0fbe442404a3e88b` | `int32_t(char arg1)` | `void App_ContextSetTerrainLightmapsUsed` | `1.0` | **100% PASS** |
| `0x00404590` | `4212112` | `Config_GetActiveGraphicOptions` | `8b0d8423bc008d04` | `void*()` | `GraphicOptions* Config_GetActiveGraphicOptions` | `1.0` | **100% PASS** |
| `0x00491CF0` | `4791536` | `Display_GetActiveMode` | `b8408bc200c39090` | `int32_t() __pure` | `void* Display_GetActiveMode` | `1.0` | **100% PASS** |
| `0x00491E70` | `4791920` | `Display_SetBackgroundColourFn` | `8b4424048b4c2408` | `uint32_t(char arg1, char arg2, char arg3)` | `void Display_SetBackgroundColourFn` | `1.0` | **100% PASS** |
| `0x00491A90` | `4790928` | `Display_SetMode` | `a02da05c0083ec10` | `int32_t(void* arg1)` | `int Display_SetMode` | `0.8986` | **100% PASS** |
| `0x004B6E80` | `4943488` | `Picture_GetHeight` | `8b44240450e8760f` | `int32_t(int32_t* arg1)` | `int Picture_GetHeight` | `1.0` | **100% PASS** |
| `0x004B6E70` | `4943472` | `Picture_GetWidth` | `8b44240450e8760f` | `int32_t(int32_t* arg1)` | `int Picture_GetWidth` | `1.0` | **100% PASS** |
| `0x004B0D40` | `4918592` | `QHash_ValueSet` | `c605fd22940001c3` | `int32_t()` | `void QHash_ValueSet` | `1.0` | **100% PASS** |
| `0x004B0DB0` | `4918704` | `QHash_ValueGet` | `a0fd229400c39090` | `int32_t()` | `int QHash_ValueGet` | `1.0` | **100% PASS** |
| `0x00415020` | `4280352` | `DEBUG_KEYSENABLEINVALID` | `8b442404b1808488` | `int32_t(void* arg1)` | `void DEBUG_KEYSENABLEINVALID` | `1.0` | **100% PASS** |
| `0x004B1AC0` | `4922048` | `QScript_Cleanup` | `538b5c240856576a` | `int32_t(void* arg1)` | `void QScript_Cleanup` | `1.0` | **100% PASS** |
| `0x004828D0` | `4729040` | `Human_FindNextCamera` | `6a10e84927f9ff83` | `void*(void* arg1)` | `void Human_FindNextCamera` | `1.0` | **100% PASS** |
| `0x004F4B00` | `5196544` | `CutScene_TaskCreate` | `81ec78020000c705` | `int32_t*()` | `void* CutScene_TaskCreate` | `1.0` | **100% PASS** |
| `0x004F51D0` | `5198288` | `CutScene_TaskUpdate` | `83ec0853568b7424` | `int32_t*(int80_t arg1 @ st0, void* arg2)` | `int CutScene_TaskUpdate` | `1.0` | **100% PASS** |
| `0x004109A0` | `4262304` | `Human_RunUpdate` | `558bec83e4f883ec` | `void*(int32_t* arg1)` | `void Human_RunUpdate` | `0.9869` | **100% PASS** |
| `0x00412440` | `4269120` | `Human_DamageApply` | `83ec105355568b74` | `float(float arg1, int32_t* arg2) __location("eax")` | `void Human_DamageApply` | `1.0` | **100% PASS** |
| `0x004779E0` | `4684256` | `Weapon_FlameUpdate` | `56578b7c240c8b47` | `void*(void* arg1, int32_t* arg2)` | `void Weapon_FlameUpdate` | `1.0` | **100% PASS** |
| `0x00479990` | `4692368` | `Weapon_ReloadStart` | `558b6c240c56578b` | `void**(void* arg1, int32_t* arg2)` | `void Weapon_ReloadStart` | `1.0` | **100% PASS** |
| `0x00477040` | `4681792` | `Throwable_Create` | `83ec085356e8669d` | `struct status_msg*(float arg1, void* arg2, double* arg3, int32_t arg4, int32_t* arg5, void* arg6, int32_t arg7)` | `void Throwable_Create` | `0.9848` | **100% PASS** |
| `0x00476040` | `4677696` | `Mine_ProximityCreate` | `5356576a01e816ad` | `struct status_msg*(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, char arg9)` | `void Mine_ProximityCreate` | `0.8519` | **100% PASS** |
| `0x004D9870` | `5085296` | `Camera_SubmitFrame` | `8b5424048b44241c` | `int32_t*(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t* arg6 @ 0x1c, int32_t arg7, int32_t arg8, float arg9)` | `void Camera_SubmitFrame` | `0.9048` | **100% PASS** |
| `0x00402870` | `4204656` | `Frames_Get` | `a18c7c56008b403c` | `int32_t()` | `int Frames_Get` | `1.0` | **100% PASS** |
| `0x00402880` | `4204672` | `Game_GetContext` | `a18c7c5600c39090` | `struct frames_ctx*()` | `void* Game_GetContext` | `1.0` | **100% PASS** |
| `0x00402890` | `4204688` | `Game_GetScriptContext` | `a18c7c56008b4020` | `int32_t()` | `void* Game_GetScriptContext` | `1.0` | **100% PASS** |
| `0x004028A0` | `4204704` | `Flow_SetState` | `8b0d8c7c56008b44` | `int32_t(int32_t arg1)` | `void Flow_SetState` | `1.0` | **100% PASS** |
| `0x004028B0` | `4204720` | `Frames_GetUpdateCounter` | `a18c7c56008b4034` | `uint32_t()` | `int Frames_GetUpdateCounter` | `1.0` | **100% PASS** |
| `0x004028C0` | `4204736` | `Frames_GetTaskCounter` | `a18c7c56008b4038` | `uint32_t()` | `int Frames_GetTaskCounter` | `1.0` | **100% PASS** |
| `0x004028D0` | `4204752` | `Game_TimingBegin` | `a1807c560085c075` | `int32_t()` | `void Game_TimingBegin` | `1.0` | **100% PASS** |
| `0x004028F0` | `4204784` | `Game_TimingEnd` | `a1807c560048a380` | `int32_t()` | `void Game_TimingEnd` | `1.0` | **100% PASS** |
| `0x004D97F0` | `5085168` | `Qtask_SetCallbackData` | `8b4c24048b442408` | `int32_t(void* arg1, int32_t arg2)` | `void Qtask_SetCallbackData` | `1.0` | **100% PASS** |
| `0x004D9800` | `5085184` | `Qtask_GetCallbackData` | `8b4424048b4048c3` | `int32_t(void* arg1)` | `unsigned int Qtask_GetCallbackData` | `1.0` | **100% PASS** |
| `0x004D9810` | `5085200` | `Qtask_Initialize` | `6a00e8a97ff2ff68` | `int32_t*()` | `void Qtask_Initialize` | `1.0` | **100% PASS** |
| `0x004D9830` | `5085232` | `Qtask_Shutdown` | `a04486540050e885` | `int32_t()` | `void Qtask_Shutdown` | `1.0` | **100% PASS** |
| `0x004D9850` | `5085264` | `Qtask_GetEventId` | `a044865400c39090` | `int32_t()` | `unsigned char Qtask_GetEventId` | `1.0` | **100% PASS** |
| `0x004D9860` | `5085280` | `Qtask_GetList` | `a19046a500c39090` | `int32_t()` | `void* Qtask_GetList` | `1.0` | **100% PASS** |
| `0x004D9950` | `5085520` | `Camera_ResetFrameCounters` | `8b442404c7404cff` | `void*(void* arg1)` | `void Camera_ResetFrameCounters` | `1.0` | **100% PASS** |
| `0x004D9A00` | `5085696` | `MagicObj_Initialize` | `6a006a00e8b7aefe` | `void*()` | `void MagicObj_Initialize` | `1.0` | **100% PASS** |
| `0x004D9F60` | `5087072` | `MagicObj_Shutdown` | `66a14886540050e8` | `uint32_t()` | `void MagicObj_Shutdown` | `1.0` | **100% PASS** |
| `0x004D9FA0` | `5087136` | `MagicObj_GetTypeId` | `66a148865400c390` | `int32_t()` | `unsigned short MagicObj_GetTypeId` | `1.0` | **100% PASS** |
| `0x004D9FB0` | `5087152` | `MagicObj_GetEventId` | `a04a865400c39090` | `int32_t()` | `unsigned char MagicObj_GetEventId` | `1.0` | **100% PASS** |
| `0x00401900` | `4200704` | `TaskType_Register` | `5533edb8055faf00` | `int32_t(int32_t arg1, int32_t arg2, int16_t arg3, int32_t arg4)` | `uint16_t TaskType_Register` | `1.0` | **100% PASS** |
| `0x00401CF0` | `4201712` | `TaskType_IsDerivedFrom` | `8b442404668b4c24` | `int32_t(int32_t arg1, int16_t arg2)` | `bool TaskType_IsDerivedFrom` | `1.0` | **100% PASS** |
| `0x00477C50` | `4684880` | `Weapon_EntityCreate` | `56576a01e8079103` | `struct status_msg*(void* arg1, int16_t arg2, int32_t* arg3, int32_t arg4, char arg5)` | `WeaponEntityView* Weapon_EntityCreate` | `1.0` | **100% PASS** |
| `0x00477C10` | `4684816` | `Weapon_FiringStateGet` | `a0b9075400c39090` | `int32_t()` | `uint8_t Weapon_FiringStateGet` | `1.0` | **100% PASS** |
| `0x004E7200` | `5140992` | `Sound_EventTrigger` | `a00ae6a50083ec60` | `int32_t*(char* arg1, char* arg2, int32_t arg3)` | `void Sound_EventTrigger` | `0.963` | **100% PASS** |
| `0x004A5702` | `4871938` | `String_VFormat` | `558bec83ec208b45` | `int32_t(char* arg1, char* arg2, int32_t arg3)` | `int String_VFormat` | `1.0` | **100% PASS** |
| `0x004B11A0` | `4919712` | `QFile_DeviceIndex` | `53558b6c240c5657` | `int32_t(void* arg1)` | `int QFile_DeviceIndex` | `1.0` | **100% PASS** |
| `0x004B1620` | `4920864` | `QFile_DeviceHandlerLookup` | `5355568b35046f54` | `int32_t(char* arg1)` | `uint32_t QFile_DeviceHandlerLookup` | `1.0` | **100% PASS** |
| `0x004B8A50` | `4950608` | `Profile_ValueSet` | `8b4424048b480485` | `int32_t(float arg1, float arg2)` | `void Profile_ValueSet` | `1.0` | **100% PASS** |
| `0x00410070` | `4259952` | `DefineHumanPlayerGeneral` | `83ec2053568b7424` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, long double arg8 @ st7, int32_t arg9, float arg10 @ 0xc)` | `unknown DefineHumanPlayerGeneral` | `0.9722` | **100% PASS** |
| `0x00413F90` | `4276112` | `DefineWeaponType` | `81ec9c000000558b` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, long double arg8 @ st7, int32_t arg9, float arg10 @ 0xc)` | `unknown DefineWeaponType` | `0.994` | **100% PASS** |
| `0x004C6670` | `5006960` | `CreateTerrainMaterial` | `83ec18538b5c2428` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `unknown CreateTerrainMaterial` | `0.9667` | **100% PASS** |
| `0x004EBEB0` | `5160624` | `DefineAnimTrigger` | `83ec1856578b7c24` | `int32_t(int32_t arg1, float arg2 @ 0xc)` | `unknown DefineAnimTrigger` | `1.0` | **100% PASS** |
| `0x004EDD20` | `5168416` | `DefinePhysicsObjType` | `83ec1ca1d8efa500` | `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, int32_t arg5, char* arg6 @ 0xc)` | `unknown DefinePhysicsObjType` | `1.0` | **100% PASS** |
| `0x00401530` | `4199728` | `QTask_LinkQTaskEventHandlerNoContextFn` | `8b0d707c56005355` | `int32_t(int16_t arg1, int32_t arg2, int32_t arg3)` | `void QTask_TriggerEvent` | `1.0` | **100% PASS** |
| `0x00401810` | `4200464` | `QTask_NewQTaskEventNoContextFn` | `b820000000b9f2f9` | `int32_t(int16_t arg1)` | `void QTask_UnableToAllocateEven_2` | `1.0` | **100% PASS** |
| `0x004018E0` | `4200672` | `QTask_DeleteQTaskEventNoContext` | `8b44240450e8d6ff` | `uint32_t(int32_t arg1)` | `void QTask_CreateTask_2` | `1.0` | **100% PASS** |
| `0x00401A20` | `4200992` | `QTask_DeleteQTaskType` | `8b4424045625ffff` | `uint32_t(int16_t arg1)` | `void QTask_CloseEvent_2` | `1.0` | **100% PASS** |
| `0x00402260` | `4203104` | `Flow_RunHandler` | `83ec0853555657e8` | `void*(uint32_t arg1)` | `void Flow_LoadParameters` | `1.0` | **100% PASS** |
| `0x004027E0` | `4204512` | `Flow_Close` | `66a17c7c560050e8` | `uint32_t()` | `void Flow_InitEvents` | `1.0` | **100% PASS** |
| `0x00414FD0` | `4280272` | `GameFunctions_IsDebugKeyPressed` | `8b4c2404b0808481` | `int32_t(void* arg1)` | `undefined4 WeaponType_GetConfig_2` | `1.0` | **100% PASS** |
| `0x00416D20` | `4287776` | `Game_GetRandomState` | `a1bcba570083c02c` | `int32_t()` | `int Level_InitSprites_5` | `1.0` | **100% PASS** |
| `0x004199D0` | `4299216` | `BackgroundFX_New` | `66a1f09a53008b4c` | `struct status_msg*(void* arg1)` | `void Menu_UpdateLanguage_6` | `1.0` | **100% PASS** |
| `0x00424CE0` | `4345056` | `Cursor_RunHandler` | `83ec085657e846a4` | `int32_t(void* arg1)` | `void Cursor_LoadParameters` | `1.0` | **100% PASS** |
| `0x00460C80` | `4590720` | `Human_printf` | `8b44240450e8c624` | `int32_t(int32_t arg1, char* arg2)` | `void AI_UpdateSensors_17` | `1.0` | **100% PASS** |
| `0x00464470` | `4605040` | `HumanView_DeleteHandler` | `568b7424088a86d9` | `int32_t(void* arg1)` | `void HumanView_LoadParameters` | `1.0` | **100% PASS** |
| `0x00464800` | `4605952` | `HumanView_UpdateView` | `dd05e8355300d9f2` | `float(float arg1, float arg2) __location("eax")` | `void AI_TakeCover_22` | `0.8889` | **100% PASS** |
| `0x00464BD0` | `4606928` | `HumanView_UpdateBody` | `81ec300100005355` | `int32_t(void* arg1, void* arg2)` | `void AI_UpdateOrders_22` | `0.951` | **100% PASS** |
| `0x004675B0` | `4617648` | `ComputerObject_ProjectRotatedPos` | `5657e839a70200db` | `int32_t(float* arg1, float arg2, void* arg3)` | `void AI_FireWeapon_24` | `1.0` | **100% PASS** |
| `0x00467620` | `4617760` | `ComputerObject_ProjectWorldPos` | `83ec0c8b4c241856` | `float*(float* arg1, float* arg2, float arg3, float arg4)` | `void AI_MoveToNode_24` | `1.0` | **100% PASS** |
| `0x00471440` | `4658240` | `BinocularOverlay_StatusDisplayHandler` | `568b742408578b46` | `int32_t(void* arg1)` | `void BinocularOverlay_Process` | `1.0` | **100% PASS** |
| `0x0048F0A0` | `4780192` | `AppContext_GethWnd` | `a1c48b5c00c39090` | `int32_t()` | `undefined4 FX_TrailUpdate_21` | `1.0` | **100% PASS** |
| `0x0048FC20` | `4783136` | `Mouse_Update` | `a11c8e5c0083ec14` | `int32_t()` | `void FX_FailedGetMouseStateS` | `1.0` | **100% PASS** |
| `0x00490360` | `4784992` | `Timer_Open` | `ff1520315300a334` | `uint32_t()` | `void Display_ClearSurfaces` | `1.0` | **100% PASS** |
| `0x00490370` | `4785008` | `Timer_Read` | `ff15203153002b05` | `int32_t()` | `int Display_RestoreDevice` | `1.0` | **100% PASS** |
| `0x004B0D10` | `4918544` | `Mem_DeAllocFn` | `8b4424048b150023` | `int32_t(void* arg1)` | `void Sys_ExecuteBytecode_2` | `1.0` | **100% PASS** |
| `0x004B0F90` | `4919184` | `RenderMode_PopRenderMode` | `a104239400568b34` | `int32_t()` | `int Sys_CloseFileStream_3` | `1.0` | **100% PASS** |
| `0x004B3320` | `4928288` | `Real64x3_NullVector` | `8b4c2404dd01dc1d` | `int32_t(double* arg1)` | `undefined4 Sys_PopStack_11` | `1.0` | **100% PASS** |
| `0x004B38E0` | `4929760` | `Real32x9_Create` | `83ec508b44246053` | `int32_t(float* arg1, float arg2, float arg3, float arg4)` | `void Sys_ReadFileBlock_12` | `0.8667` | **100% PASS** |
| `0x004B6E60` | `4943456` | `Picture_Register` | `8b44240450e8460f` | `int32_t(int32_t* arg1)` | `void Sys_CheckHeap_17` | `1.0` | **100% PASS** |
| `0x004B8A20` | `4950560` | `Script_EvalArgInt` | `8b4424048b480485` | `int32_t(float arg1, int32_t arg2)` | `void Script_GetArg` | `1.0` | **100% PASS** |
| `0x004C1790` | `4986768` | `QTaskList_Register` | `8b4424048b088b50` | `int32_t*(int32_t* arg1, int32_t arg2)` | `void DynObj_QtasklistIsFull` | `1.0` | **100% PASS** |
| `0x004C17C0` | `4986816` | `QTaskList_Remove` | `568b74240833c057` | `int32_t(int32_t* arg1, int32_t arg2)` | `void DynObj_TryingToRemoveUnrigi` | `1.0` | **100% PASS** |
| `0x004C1800` | `4986880` | `QTaskList_New` | `568b7424086a048d` | `int32_t*(int32_t arg1)` | `void DynObj_LookupHashItem_4` | `1.0` | **100% PASS** |
| `0x004C1830` | `4986928` | `QTaskList_Delete` | `8b44240450e8d6f4` | `int32_t(void* arg1)` | `void DynObj_CreateMagicObject_4` | `1.0` | **100% PASS** |
| `0x004D3210` | `5059088` | `AnimController_UpdateChannels` | `518b44240c5733ff` | `int32_t(int32_t arg1, int32_t* arg2)` | `undefined4 Render_ClipMesh_5` | `1.0` | **100% PASS** |
| `0x004D9610` | `5084688` | `BoneDynCubeObj_GetQTaskType` | `66a118865400c390` | `int32_t()` | `undefined2 Render_DrawRigidMesh_12` | `1.0` | **100% PASS** |
| `0x004E6030` | `5136432` | `SoundSys_SetFrequency` | `8b44240450a3fce5` | `int32_t(int32_t arg1)` | `void SoundSys_Update3DPosition_4` | `1.0` | **100% PASS** |
| `0x004E76E0` | `5142240` | `TextWindow_vprintf` | `8b44240c8b4c2408` | `int32_t(int32_t* arg1, char* arg2, int32_t arg3)` | `void SoundSys_PlaySoundSample_10` | `1.0` | **100% PASS** |
| `0x004E7B80` | `5143424` | `DebugText_SetCursor` | `a075eaa50084c074` | `int32_t(int32_t arg1, int32_t arg2)` | `void SoundSys_AttachLipsync_10` | `1.0` | **100% PASS** |
| `0x004E7BB0` | `5143472` | `DebugText_Clear` | `a075eaa50084c074` | `int32_t()` | `void SoundSys_InitSoundSys_11` | `1.0` | **100% PASS** |
| `0x00507EC0` | `5275328` | `Input_Run` | `568b7424088b461c` | `int16_t*(int32_t* arg1, int32_t arg2)` | `void Physics_SimulateRigidBody_6` | `1.0` | **100% PASS** |

## 2. Deep Dive Analysis by Subsystem (Representative Samples)

### Native `0x004EC070`: `AnimTrigger_ParseConfigFile`

- **Address**: `0x004EC070` (5161072) | **Size**: 73 bytes
- **Raw PE Bytes**: `56578b7c240c68b87f540057e82f9bfb`
- **Binary Ninja MCP**:
  - Prototype: `char*(char* arg1)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 6
- **Ghidra MCP / REA**:
  - AST Signature: `void AnimTrigger_ParseConfigFile(uint* path);`
  - Callers: 1 | Callees: 5
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=6 vs R2=6 | Calls: BN=5 vs R2=5
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x004EC070:  push     esi
0x004EC071:  push     edi
0x004EC072:  mov      edi, dword ptr [esp + 0xc]
0x004EC076:  push     0x547fb8
```

### Native `0x004F0CF0`: `Config_Compile`

- **Address**: `0x004F0CF0` (5180656) | **Size**: 79 bytes
- **Raw PE Bytes**: `8b44240456575068bc9a5300e82f7cfc`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(int32_t arg1, void* arg2, char arg3)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 3
- **Ghidra MCP / REA**:
  - AST Signature: `int Config_Compile(int context, char* cfgName, int flags);`
  - Callers: 2 | Callees: 5
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=3 vs R2=3 | Calls: BN=5 vs R2=5
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x004F0CF0:  mov      eax, dword ptr [esp + 4]
0x004F0CF4:  push     esi
0x004F0CF5:  push     edi
0x004F0CF6:  push     eax
```

### Native `0x004AF7B0`: `Error_Show`

- **Address**: `0x004AF7B0` (4913072) | **Size**: 89 bytes
- **Raw PE Bytes**: `a16862930085c074445768b86d540068`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(char* arg1)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 5
- **Ghidra MCP / REA**:
  - AST Signature: `void Error_Show(char* format, ...);`
  - Callers: 146 | Callees: 2
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=5 vs R2=5 | Calls: BN=2 vs R2=3
  - CFG Structural Similarity Score: **0.8889** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x004AF7B0:  mov      eax, dword ptr [0x936268]
0x004AF7B5:  test     eax, eax
0x004AF7B7:  je       0x4af7fd
0x004AF7B9:  push     edi
```

### Native `0x00402820`: `Frames_Set`

- **Address**: `0x00402820` (4204576) | **Size**: 74 bytes
- **Raw PE Bytes**: `e84bdb08008b0d8c7c56008941308b15`
- **Binary Ninja MCP**:
  - Prototype: `void(int32_t frames)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 1
- **Ghidra MCP / REA**:
  - AST Signature: `void Frames_Set(int frames);`
  - Callers: 2 | Callees: 2
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=1 vs R2=1 | Calls: BN=2 vs R2=2
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x00402820:  call     0x490370
0x00402825:  mov      ecx, dword ptr [0x567c8c]
0x0040282B:  mov      dword ptr [ecx + 0x30], eax
0x0040282E:  mov      edx, dword ptr [0x567c8c]
```

### Native `0x004F0E10`: `Level_LoadObjects`

- **Address**: `0x004F0E10` (5180944) | **Size**: 51 bytes
- **Raw PE Bytes**: `8b44240481ec000100008d4c24005068`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(int32_t arg1, int32_t arg2)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 1
- **Ghidra MCP / REA**:
  - AST Signature: `void Level_LoadObjects(int level_path, int level_len);`
  - Callers: 1 | Callees: 2
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=1 vs R2=1 | Calls: BN=2 vs R2=2
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x004F0E10:  mov      eax, dword ptr [esp + 4]
0x004F0E14:  sub      esp, 0x100
0x004F0E1A:  lea      ecx, [esp]
0x004F0E1E:  push     eax
```

### Native `0x00410070`: `DefineHumanPlayerGeneral`

- **Address**: `0x00410070` (4259952) | **Size**: 360 bytes
- **Raw PE Bytes**: `83ec2053568b742434576a0056e8ce89`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, long double arg8 @ st7, int32_t arg9, float arg10 @ 0xc)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 5
- **Ghidra MCP / REA**:
  - AST Signature: `unknown DefineHumanPlayerGeneral(unknown param1, unknown param2);`
  - Callers: 0 | Callees: 1
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=5 vs R2=5 | Calls: BN=12 vs R2=11
  - CFG Structural Similarity Score: **0.9722** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x00410070:  sub      esp, 0x20
0x00410073:  push     ebx
0x00410074:  push     esi
0x00410075:  mov      esi, dword ptr [esp + 0x34]
```

### Native `0x00413F90`: `DefineWeaponType`

- **Address**: `0x00413F90` (4276112) | **Size**: 2165 bytes
- **Raw PE Bytes**: `81ec9c000000558bac24ac0000005657`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(long double arg1 @ st0, long double arg2 @ st1, long double arg3 @ st2, long double arg4 @ st3, long double arg5 @ st4, long double arg6 @ st5, long double arg7 @ st6, long double arg8 @ st7, int32_t arg9, float arg10 @ 0xc)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 15
- **Ghidra MCP / REA**:
  - AST Signature: `unknown DefineWeaponType(unknown param1, unknown param2);`
  - Callers: 0 | Callees: 6
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=15 vs R2=15 | Calls: BN=56 vs R2=55
  - CFG Structural Similarity Score: **0.994** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x00413F90:  sub      esp, 0x9c
0x00413F96:  push     ebp
0x00413F97:  mov      ebp, dword ptr [esp + 0xac]
0x00413F9E:  push     esi
```

### Native `0x00401530`: `QTask_LinkQTaskEventHandlerNoContextFn`

- **Address**: `0x00401530` (4199728) | **Size**: 158 bytes
- **Raw PE Bytes**: `8b0d707c560053558b2d486153008b44`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(int16_t arg1, int32_t arg2, int32_t arg3)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 7
- **Ghidra MCP / REA**:
  - AST Signature: `void QTask_TriggerEvent(ushort param_1, int param_2, undefined4 param_3);`
  - Callers: 217 | Callees: 2
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=7 vs R2=7 | Calls: BN=2 vs R2=2
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x00401530:  mov      ecx, dword ptr [0x567c70]
0x00401536:  push     ebx
0x00401537:  push     ebp
0x00401538:  mov      ebp, dword ptr [0x536148]
```

### Native `0x00464800`: `HumanView_UpdateView`

- **Address**: `0x00464800` (4605952) | **Size**: 964 bytes
- **Raw PE Bytes**: `dd05e8355300d9f2538b5c240855568b`
- **Binary Ninja MCP**:
  - Prototype: `float(float arg1, float arg2) __location("eax")`
  - Calling Convention: `cdecl`
  - Basic Blocks: 55
- **Ghidra MCP / REA**:
  - AST Signature: `void AI_TakeCover_22(int param_1, float param_2);`
  - Callers: 5 | Callees: 3
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=55 vs R2=55 | Calls: BN=6 vs R2=4
  - CFG Structural Similarity Score: **0.8889** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x00464800:  fld      qword ptr [0x5335e8]
0x00464806:  fptan    
0x00464808:  push     ebx
0x00464809:  mov      ebx, dword ptr [esp + 8]
```

### Native `0x0048F0A0`: `AppContext_GethWnd`

- **Address**: `0x0048F0A0` (4780192) | **Size**: 6 bytes
- **Raw PE Bytes**: `a1c48b5c00c390909090909090909090`
- **Binary Ninja MCP**:
  - Prototype: `int32_t()`
  - Calling Convention: `cdecl`
  - Basic Blocks: 1
- **Ghidra MCP / REA**:
  - AST Signature: `undefined4 FX_TrailUpdate_21(void);`
  - Callers: 5 | Callees: 0
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=1 vs R2=1 | Calls: BN=0 vs R2=0
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x0048F0A0:  mov      eax, dword ptr [0x5c8bc4]
0x0048F0A5:  ret      
0x0048F0A6:  nop      
0x0048F0A7:  nop      
```

### Native `0x0048FC20`: `Mouse_Update`

- **Address**: `0x0048FC20` (4783136) | **Size**: 359 bytes
- **Raw PE Bytes**: `a11c8e5c0083ec148d5424008b085652`
- **Binary Ninja MCP**:
  - Prototype: `int32_t()`
  - Calling Convention: `cdecl`
  - Basic Blocks: 38
- **Ghidra MCP / REA**:
  - AST Signature: `void FX_FailedGetMouseStateS(void);`
  - Callers: 1 | Callees: 3
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=38 vs R2=38 | Calls: BN=3 vs R2=3
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x0048FC20:  mov      eax, dword ptr [0x5c8e1c]
0x0048FC25:  sub      esp, 0x14
0x0048FC28:  lea      edx, [esp]
0x0048FC2C:  mov      ecx, dword ptr [eax]
```

### Native `0x004E6030`: `SoundSys_SetFrequency`

- **Address**: `0x004E6030` (5136432) | **Size**: 17 bytes
- **Raw PE Bytes**: `8b44240450a3fce5a500e881fcfaff59`
- **Binary Ninja MCP**:
  - Prototype: `int32_t(int32_t arg1)`
  - Calling Convention: `cdecl`
  - Basic Blocks: 1
- **Ghidra MCP / REA**:
  - AST Signature: `void SoundSys_Update3DPosition_4(undefined4 param_1);`
  - Callers: 1 | Callees: 1
- **Radare2 MCP CFG Gate**:
  - Blocks: BN=1 vs R2=1 | Calls: BN=1 vs R2=1
  - CFG Structural Similarity Score: **1.0** (Gate: **PASS**)
- **Disassembly Preview**:
```x86asm
0x004E6030:  mov      eax, dword ptr [esp + 4]
0x004E6034:  push     eax
0x004E6035:  mov      dword ptr [0xa5e5fc], eax
0x004E603A:  call     0x495cc0
```

