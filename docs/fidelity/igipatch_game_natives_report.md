# Project IGI Retail Game Natives & Symbols Catalog
## Complete Extraction & Audit from [BlanknameES/IGIPatch](https://github.com/BlanknameES/IGIPatch)

> **Audit Target:** `https://github.com/BlanknameES/IGIPatch`  
> **Target Executable:** `igi.exe` v1.0 (European Retail Release, SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`)  
> **Total Extracted Symbols / Addresses:** **372**  

### Subsystem Summary

| Subsystem | Symbol Count | Description |
|:---|:---:|:---|
| **Player, Human & Camera Subsystem** | 101 | Functions, hooks, and variables related to player, human & camera subsystem |
| **Render & Direct3D Pipeline** | 100 | Functions, hooks, and variables related to render & direct3d pipeline |
| **Input & Cursor Management** | 39 | Functions, hooks, and variables related to input & cursor management |
| **Game Loop & Flow Management** | 39 | Functions, hooks, and variables related to game loop & flow management |
| **Windowing, Display & Video Modes** | 37 | Functions, hooks, and variables related to windowing, display & video modes |
| **World Objects & Interactive Entities** | 12 | Functions, hooks, and variables related to world objects & interactive entities |
| **Core Engine Subsystem** | 11 | Functions, hooks, and variables related to core engine subsystem |
| **Configuration, Storage & Profiles** | 9 | Functions, hooks, and variables related to configuration, storage & profiles |
| **UI, Menus & Loading Screen** | 7 | Functions, hooks, and variables related to ui, menus & loading screen |
| **Diagnostics, Logging & Debug** | 5 | Functions, hooks, and variables related to diagnostics, logging & debug |
| **Core Data & State Tables** | 4 | Functions, hooks, and variables related to core data & state tables |
| **Animation Controller** | 3 | Functions, hooks, and variables related to animation controller |
| **Engine BSS & Runtime Globals** | 3 | Functions, hooks, and variables related to engine bss & runtime globals |
| **Timing & Performance Counters** | 2 | Functions, hooks, and variables related to timing & performance counters |

---

### 1. Player, Human & Camera Subsystem (101 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x004028B0` | **Flow_GetTicks** | Native Function Call Target | StationaryGun_RunHandler - save StationaryGun_isUsedByPlayer | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1065`<br>`_code_mainhooks.asm:5433` |
| `0x00406220` | **Config_GetActivePlayerProfile** | Native Function Call Target | InputOptions_UpdateMouseSensitivity - updates mouse sensitivity multiplier variable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1501`<br>`_code_mainhooks.asm:7304` |
| `0x0041006B` | **HumanPlayer_Open** | Engine Native Hook Entry | add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1003`<br>`_code_mainhooks.asm:5233` |
| `0x00414FD0` | **GameFunctions_IsDebugKeyPressed** | Native Function Call Target | HumanCamera_ThirdPersonControlInput - skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1185`<br>`_code_applypatches_id0.asm:1195` |
| `0x00415020` | **GameFunctions_IsDebugKeyPressedOnce** | Native Function Call Target | HumanCamera_ControlListInput - skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1213`<br>`_code_mainhooks.asm:5931` |
| `0x00416D20` | **Game_GetRandomState** | Native Function Call Target | HumanView_UpdateBody - save Game_GetRandomState seed | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1089`<br>`_code_mainhooks.asm:5486` |
| `0x0045E07E` | **HumanPlayerInput_ReadChannels** | Engine Native Hook Entry | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1597`<br>`_code_mainhooks.asm:8094` |
| `0x0045E09D` | **HumanPlayerInput_ReadChannels** | Native Function Call Target | HumanPlayerInput_ReadChannels - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1601`<br>`_code_mainhooks.asm:8122` |
| `0x0045E0E4` | **HumanPlayerInput_ReadChannels** | Engine Native Hook Entry | skip duplicated mouse sensitivity/inversion calculation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1605`<br>`_code_mainhooks.asm:8125` |
| `0x0045E159` | **HumanPlayerInput_ReadChannels** | Native Function Call Target | HumanPlayerInput_ReadChannels - skip duplicated mouse sensitivity/inversion calculation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1607`<br>`_code_mainhooks.asm:8142` |
| `0x0045ECC0` | **Human_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1251`<br>`_code_mainhooks.asm:6108` |
| `0x0045EEA0` | **Human_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1259`<br>`_code_mainhooks.asm:6126` |
| `0x0045EEA9` | **Human_RunHandler** | Native Function Call Target | Human_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1261`<br>`_code_mainhooks.asm:6136` |
| `0x0045EFC5` | **Human_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1255`<br>`_code_mainhooks.asm:6117` |
| `0x0045FD58` | **Human_GetHumanCameraInfoHandler** | Engine Native Hook Entry | skip checking debug keys during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1167`<br>`_code_mainhooks.asm:5836` |
| `0x0045FD5E` | **Human_GetHumanCameraInfoHandler** | Native Function Call Target | Human_GetHumanCameraInfoHandler - skip checking debug keys during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1171`<br>`_code_mainhooks.asm:5845` |
| `0x0045FD85` | **Human_GetHumanCameraInfoHandler** | Native Function Call Target | Human_GetHumanCameraInfoHandler - skip checking debug keys during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1169`<br>`_code_mainhooks.asm:5840` |
| `0x00460133` | **Human_gun_event_related_3** | Engine Native Hook Entry | interpolate (1st person gun model part 1) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1405`<br>`_code_mainhooks.asm:6750` |
| `0x00460187` | **Human_gun_event_related_3** | Engine Native Hook Entry | interpolate (1st person gun model part 1) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1409`<br>`_code_mainhooks.asm:6796` |
| `0x00460202` | **Human_gun_event_related_4** | Engine Native Hook Entry | interpolate (1st person gun model part 2) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1413`<br>`_code_mainhooks.asm:6844` |
| `0x00460272` | **Human_gun_event_related_5** | Engine Native Hook Entry | interpolate (1st person gun model part 3) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1417`<br>`_code_mainhooks.asm:6894` |
| `0x00460C80` | **Human_printf** | Engine Native Hook Entry | skip priting text every frame | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1175`<br>`_code_mainhooks.asm:5848` |
| `0x00460C85` | **Human_printf** | Native Function Call Target | Human_printf - skip priting text every frame | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1179`<br>`_code_mainhooks.asm:5865` |
| `0x004612CD` | **Human_UpdateBody** | Engine Native Hook Entry | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1611`<br>`_code_mainhooks.asm:8145` |
| `0x0046139B` | **Human_UpdateBody** | Native Function Call Target | Human_UpdateBody - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1619`<br>`_code_mainhooks.asm:8184` |
| `0x00464466` | **HumanView_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1269`<br>`_code_mainhooks.asm:6139` |
| `0x00464470` | **HumanView_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1273`<br>`_code_mainhooks.asm:6150` |
| `0x00464475` | **HumanView_DeleteHandler** | Native Function Call Target | HumanView_DeleteHandler - dealloc interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1275`<br>`_code_mainhooks.asm:6161` |
| `0x004644EE` | **HumanView_EnterHandler** | Engine Native Hook Entry | interpolate (1st person view model) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1397`<br>`_code_mainhooks.asm:6706` |
| `0x004644F3` | **HumanView_EnterHandler** | Native Function Call Target | HumanView_EnterHandler - interpolate (1st person view model) | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1401`<br>`_code_mainhooks.asm:6747` |
| `0x00464800` | **HumanView_UpdateView** | Engine Native Hook Entry | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1631`<br>`_code_mainhooks.asm:8265` |
| `0x004648D1` | **HumanView_UpdateView** | Native Function Call Target | HumanView_UpdateView - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1641`<br>`_code_mainhooks.asm:8313` |
| `0x00464C28` | **HumanView_UpdateBody** | Engine Native Hook Entry | save Game_GetRandomState seed | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1087`<br>`_code_mainhooks.asm:5483` |
| `0x00464C2E` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - save Game_GetRandomState seed | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1091`<br>`_code_mainhooks.asm:5494` |
| `0x00464D72` | **HumanView_UpdateBody** | Engine Native Hook Entry | fix shake duration during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1107`<br>`_code_mainhooks.asm:5497` |
| `0x00464D78` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix shake duration during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1113`<br>`_code_mainhooks.asm:5554` |
| `0x00464DE2` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix shake duration during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1111`<br>`_code_mainhooks.asm:5549` |
| `0x00464E1B` | **HumanView_UpdateBody** | Engine Native Hook Entry | fix landing impact speed during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1117`<br>`_code_mainhooks.asm:5557` |
| `0x00464E21` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix landing impact speed during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1125`<br>`_code_mainhooks.asm:5589` |
| `0x00464E2D` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix landing impact speed during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1121`<br>`_code_applypatches_id0.asm:1123` |
| `0x00465102` | **HumanView_UpdateBody** | Engine Native Hook Entry | fix view model retraction speed on collision during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1129`<br>`_code_mainhooks.asm:5592` |
| `0x00465108` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix view model retraction speed on collision during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1139`<br>`_code_mainhooks.asm:5671` |
| `0x004651F8` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix view model retraction speed on collision during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1137`<br>`_code_mainhooks.asm:5666` |
| `0x00465251` | **HumanView_UpdateBody** | Engine Native Hook Entry | fix object alpha/gamma speed during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1143`<br>`_code_mainhooks.asm:5674` |
| `0x00465257` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix object alpha/gamma speed during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1149`<br>`_code_mainhooks.asm:5781` |
| `0x0046534D` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - fix object alpha/gamma speed during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1147`<br>`_code_mainhooks.asm:5776` |
| `0x0046553F` | **HumanView_UpdateBody** | Engine Native Hook Entry | switch anim updates during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1153`<br>`_code_mainhooks.asm:5784` |
| `0x0046555F` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - switch anim updates during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1161`<br>`_code_mainhooks.asm:5829` |
| `0x0046560D` | **HumanView_UpdateBody** | Native Function Call Target | HumanView_UpdateBody - switch anim updates during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1163`<br>`_code_mainhooks.asm:5833` |
| `0x00471440` | **BinocularOverlay_StatusDisplayHandler** | Engine Native Hook Entry | project with the captured trans context | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1227`<br>`_code_mainhooks.asm:5957` |
| `0x00471446` | **BinocularOverlay_StatusDisplayHandler** | Native Function Call Target | BinocularOverlay_StatusDisplayHandler - project with the captured trans context | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1231`<br>`_code_mainhooks.asm:5999` |
| `0x0047145E` | **BinocularOverlay_StatusDisplayHandler** | Native Function Call Target | BinocularOverlay_StatusDisplayHandler - project with the captured trans context | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1229`<br>`_code_mainhooks.asm:5993` |
| `0x004719D3` | **VUMeter_Update** | Engine Native Hook Entry | fix fx bars update rate when using binoculars | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1071`<br>`_code_mainhooks.asm:5438` |
| `0x004719D8` | **VUMeter_Update** | Native Function Call Target | VUMeter_Update - fix fx bars update rate when using binoculars | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1075`<br>`_code_mainhooks.asm:5455` |
| `0x00471C26` | **BinocularOverlay_TrackMovement** | Engine Native Hook Entry | project interpolated pos | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1237`<br>`_code_mainhooks.asm:6002` |
| `0x00471C36` | **BinocularOverlay_TrackMovement** | Native Function Call Target | BinocularOverlay_TrackMovement - project interpolated pos | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1239`<br>`_code_mainhooks.asm:6020` |
| `0x00473EB8` | **StationaryGun_CreateHandler** | Engine Native Hook Entry | register interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1049`<br>`_code_mainhooks.asm:5386` |
| `0x00474186` | **StationaryGun_Open** | Engine Native Hook Entry | add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1041`<br>`_code_mainhooks.asm:5362` |
| `0x00474195` | **StationaryGun_RunHandler** | Engine Native Hook Entry | save StationaryGun_isUsedByPlayer | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1061`<br>`_code_mainhooks.asm:5416` |
| `0x0047419A` | **StationaryGun_RunHandler** | Native Function Call Target | StationaryGun_RunHandler - save StationaryGun_isUsedByPlayer | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1067`<br>`_code_mainhooks.asm:5435` |
| `0x0047497B` | **StationaryGun_DeleteHandler** | Engine Native Hook Entry | unregister interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1055`<br>`_code_mainhooks.asm:5401` |
| `0x004782C1` | **GunCasing_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1339`<br>`_code_mainhooks.asm:6300` |
| `0x004782D0` | **GunCasing_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1347`<br>`_code_mainhooks.asm:6319` |
| `0x004782D7` | **GunCasing_RunHandler** | Native Function Call Target | GunCasing_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1349`<br>`_code_mainhooks.asm:6330` |
| `0x00478505` | **GunCasing_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1343`<br>`_code_mainhooks.asm:6309` |
| `0x004785E5` | **Gun_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1283`<br>`_code_mainhooks.asm:6177` |
| `0x00478649` | **Gun_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1291`<br>`_code_mainhooks.asm:6195` |
| `0x0047864F` | **Gun_RunHandler** | Native Function Call Target | Gun_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1293`<br>`_code_mainhooks.asm:6202` |
| `0x004788C6` | **Gun_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1287`<br>`_code_mainhooks.asm:6186` |
| `0x00482642` | **HumanCamera_Open** | Engine Native Hook Entry | add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1015`<br>`_code_mainhooks.asm:5276` |
| `0x004826F5` | **HumanCamera_CreateHandler** | Engine Native Hook Entry | register interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1029`<br>`_code_mainhooks.asm:5338` |
| `0x004828D0` | **HumanCamera_ControlListInput** | Engine Native Hook Entry | skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1211`<br>`_code_mainhooks.asm:5923` |
| `0x004828DA` | **HumanCamera_ControlListInput** | Native Function Call Target | HumanCamera_ControlListInput - skip debug keys check during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1215`<br>`_code_mainhooks.asm:5934` |
| `0x00482C31` | **HumanCamera_ThirdPersonControlInput** | Engine Native Hook Entry | skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1183`<br>`_code_mainhooks.asm:5871` |
| `0x00482C3F` | **HumanCamera_ThirdPersonControlInput** | Native Function Call Target | HumanCamera_ThirdPersonControlInput - skip debug keys check during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1187`<br>`_code_mainhooks.asm:5886` |
| `0x00483415` | **HumanCamera_MovementControlInput** | Engine Native Hook Entry | skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1191`<br>`_code_mainhooks.asm:5893` |
| `0x0048341F` | **HumanCamera_MovementControlInput** | Native Function Call Target | HumanCamera_MovementControlInput - skip debug keys check during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1197`<br>`_code_mainhooks.asm:5905` |
| `0x00483455` | **HumanCamera_MovementControlInput** | Native Function Call Target | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1193`<br>`_code_applypatches_id0.asm:1653` |
| `0x0048346D` | **HumanCamera_MovementControlInput** | Native Function Call Target | HumanCamera_MovementControlInput - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1655`<br>`_code_mainhooks.asm:8495` |
| `0x004834AB` | **HumanCamera_MovementControlInput** | Engine Native Hook Entry | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1657`<br>`_code_mainhooks.asm:8498` |
| `0x004834B7` | **HumanCamera_MovementControlInput** | Native Function Call Target | HumanCamera_MovementControlInput - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1659`<br>`_code_mainhooks.asm:8532` |
| `0x00483E17` | **HumanCamera_DeathCameraControlInput** | Engine Native Hook Entry | skip debug keys check during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1201`<br>`_code_mainhooks.asm:5908` |
| `0x00483E21` | **HumanCamera_DeathCameraControlInput** | Native Function Call Target | HumanCamera_DeathCameraControlInput - skip debug keys check during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1207`<br>`_code_mainhooks.asm:5920` |
| `0x00483E57` | **HumanCamera_DeathCameraControlInput** | Native Function Call Target | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1203`<br>`_code_applypatches_id0.asm:1663` |
| `0x00483E63` | **HumanCamera_DeathCameraControlInput** | Native Function Call Target | HumanCamera_DeathCameraControlInput - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1665`<br>`_code_mainhooks.asm:8569` |
| `0x00483E67` | **HumanCamera_DeathCameraControlInput** | Engine Native Hook Entry | apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1667`<br>`_code_mainhooks.asm:8572` |
| `0x00483E73` | **HumanCamera_DeathCameraControlInput** | Native Function Call Target | HumanCamera_DeathCameraControlInput - apply correct per-phase mouse delta | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1669`<br>`_code_mainhooks.asm:8605` |
| `0x00484CEB` | **HumanCamera_DeleteHandler** | Engine Native Hook Entry | unregister interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1035`<br>`_code_mainhooks.asm:5350` |
| `0x00488707` | **HumanView_RunHandler_NEW** | Native Function Call Target | HumanView_RunHandler_NEW - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1279`<br>`_code_mainhooks.asm:6174` |
| `0x004B3320` | **Real64x3_NullVector** | Native Function Call Target | HumanView_UpdateView - apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1633`<br>`_code_applypatches_id0.asm:1645` |
| `0x004B38E0` | **Real32x9_Create** | Native Function Call Target | Human_InterpBody | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1627`<br>`_code_mainhooks.asm:8250` |
| `0x004D3210` | **AnimController_UpdateChannels** | Native Function Call Target | HumanView_UpdateBody - switch anim updates during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1159`<br>`_code_mainhooks.asm:5819` |
| `0x00533400` | **stdcall Patcher_WriteAddressReloc,HumanView_InterpView.fixup2,edi,0x00533400,esi,FALSE ;flt_533400** | Global Variable / State Field | Human_UpdateBody - apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1615`<br>`_code_applypatches_id0.asm:1623` |
| `0x00533468` | **flt_533468** | Global Variable / State Field | Human_UpdateBody - apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1617`<br>`_code_applypatches_id0.asm:1625` |
| `0x005334A8` | **flt_5334A8** | Global Variable / State Field | HumanView_UpdateBody - fix landing impact speed during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1119`<br>`_code_mainhooks.asm:5573` |
| `0x005339D4` | **flt_5339D4** | Global Variable / State Field | HumanView_UpdateBody - fix view model retraction speed on collision during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1133`<br>`_code_mainhooks.asm:5619` |
| `0x005339D8` | **flt_5339D8** | Global Variable / State Field | HumanView_UpdateBody - fix view model retraction speed on collision during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1131`<br>`_code_mainhooks.asm:5614` |
| `0x005385B0` | **HumanPlayer_eQTaskType** | Global Variable / State Field | HumanPlayer_Open - add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1005`<br>`_code_applypatches_id0.asm:1063` |
| `0x00540990` | **HumanCamera_eQTaskType** | Global Variable / State Field | HumanCamera_Open - add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1017`<br>`_code_applypatches_id0.asm:1391` |
| `0x005BE388` | **StationaryGun_eQTaskType** | Global Variable / State Field | StationaryGun_Open - add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1043`<br>`_code_mainhooks.asm:5366` |
| `0x00BC210C` | **dword_BC210C** | Global Variable / State Field | HumanPlayerInput_ReadChannels - apply correct per-phase mouse delta | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1599`<br>`_code_mainhooks.asm:8117` |


### 1. Render & Direct3D Pipeline (100 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00401CF0` | **QTask_IsQTaskTypeDerived** | Native Function Call Target | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1357`<br>`_code_applypatches_id0.asm:1371` |
| `0x004046A0` | **Config_FillRenderDeviceListBox** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:360` |
| `0x0041A8A4` | **TypeWriterBox_DrawHandler** | Engine Native Hook Entry | fix credits rect position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:802`<br>`_code_mainhooks.asm:3317` |
| `0x0041A8AA` | **TypeWriterBox_DrawHandler** | Native Function Call Target | TypeWriterBox_DrawHandler - fix credits rect position | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:808`<br>`_code_mainhooks.asm:3343` |
| `0x0041D7A3` | **InputBox_DrawHandler** | Engine Native Hook Entry | fix input boxes position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:812`<br>`_code_mainhooks.asm:3346` |
| `0x0041D7BA` | **InputBox_DrawHandler** | Native Function Call Target | InputBox_DrawHandler - fix input boxes position | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:818`<br>`_code_mainhooks.asm:3371` |
| `0x0041F2D0` | **ListBox_AddItem** | Native Function Call Target | Config_FillRenderDeviceListBox | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:368`<br>`_code_applypatches_id0.asm:398` |
| `0x00421B5D` | **MenuScreen_UpdateInternalDataHandler** | Engine Native Hook Entry | fix parameters for background picture drawing | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:756`<br>`_code_mainhooks.asm:2854` |
| `0x00421BC2` | **MenuScreen_UpdateInternalDataHandler** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix parameters for background picture drawing | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:770`<br>`_code_mainhooks.asm:2955` |
| `0x00421CB6` | **MenuScreen_DrawHandler** | Engine Native Hook Entry | draw background picture with custom scaling mode | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:778`<br>`_code_mainhooks.asm:3104` |
| `0x00424CE0` | **Cursor_RunHandler** | Native Function Call Target | Cursor_DrawInterpolateHandler - new interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:979`<br>`_code_mainhooks.asm:5180` |
| `0x00464BD0` | **HumanView_UpdateBody** | Native Function Call Target | HumanPlayer_DrawInterpolateHandler - new interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1011`<br>`_code_mainhooks.asm:5267` |
| `0x0048F240` | **AppContext_SetLightmapsUsed** | Native Function Call Target | Main_ParseNoLightmapsCB | DebugFeaturesPatch, Main Hooks | Manifest (): `App_ContextSetLightmapsUsed` | `_code_applypatches_id0.asm:644`<br>`_code_mainhooks.asm:2599` |
| `0x0048F260` | **AppContext_SetTerrainLightmapsUsed** | Native Function Call Target | Main_ParseNoTerrainLightmapCB | DebugFeaturesPatch, Main Hooks | Manifest (): `App_ContextSetTerrainLightmapsUsed` | `_code_applypatches_id0.asm:648`<br>`_code_mainhooks.asm:2608` |
| `0x00492737` | **Direct3D_EnumDisplayModesCB** | Engine Patch Point | skip unnecessary null string checks that uses incorrect type casting | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:330` |
| `0x0049276D` | **Direct3D_EnumDisplayModesCB** | Engine Patch Point | skip unnecessary null string checks that uses incorrect type casting | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:334` |
| `0x004942D8` | **Direct3D_CreateDevice** | Engine Native Hook Entry | hide windows cursor in windowed mode | MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:221` |
| `0x004942F3` | **Direct3D_CreateDevice** | Engine Native Hook Entry | save refresh rate from current set mode | Main Hooks, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:62`<br>`_code_mainhooks.asm:190` |
| `0x004942FC` | **Direct3D_CreateDevice** | Native Function Call Target | Direct3D_CreateDevice - save refresh rate from current set mode | [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:64`<br>`_code_mainhooks.asm:197` |
| `0x00494305` | **Direct3D_CreateDevice** | Engine Native Hook Entry | hide windows cursor in windowed mode | MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:223` |
| `0x0049432E` | **Direct3D_CreateDevice** | Engine Native Hook Entry | save refresh rate from current set mode | Main Hooks, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:66`<br>`_code_mainhooks.asm:201` |
| `0x00494336` | **Direct3D_CreateDevice** | Native Function Call Target | Direct3D_CreateDevice - save refresh rate from current set mode | [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:70`<br>`_code_mainhooks.asm:212` |
| `0x0049467C` | **Direct3D_CreateDevice** | Native Function Call Target | Direct3D_CreateDevice - save refresh rate from current set mode | [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:68`<br>`_code_mainhooks.asm:204` |
| `0x00494FB1` | **Direct3D_WinProcCB** | Engine Native Hook Entry | window resizing | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:294`<br>`_code_mainhooks.asm:980` |
| `0x00494FC8` | **Direct3D_WinProcCB** | Native Function Call Target | Direct3D_WinProcCB - window resizing | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:296`<br>`_code_mainhooks.asm:989` |
| `0x00497DE7` | **TransContext_Create** | Engine Native Hook Entry | set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:445`<br>`_code_mainhooks.asm:1955` |
| `0x00497E70` | **TransContext_SetActiveTransContext** | Native Function Call Target | Direct3DRender_DrawRigidMesh - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:463`<br>`_code_applypatches_id0.asm:475` |
| `0x0049E006` | **Direct3DRender_DrawRigidMesh** | Engine Native Hook Entry | set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:457`<br>`_code_mainhooks.asm:1990` |
| `0x0049E0B1` | **Direct3DRender_DrawRigidMesh** | Native Function Call Target | read interpolated pos from second context | Main Hooks, NewFPSLimiterPatch, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1447`<br>`_code_applypatches_id0.asm:465` |
| `0x0049E0BA` | **Direct3DRender_DrawRigidMesh** | Native Function Call Target | Direct3DRender_DrawRigidMesh - read interpolated pos from second context | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1451`<br>`_code_mainhooks.asm:7134` |
| `0x0049E272` | **Direct3DRender_DrawRigidMesh** | Engine Native Hook Entry | read interpolated pos from second context | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1459`<br>`_code_mainhooks.asm:7137` |
| `0x0049E27B` | **Direct3DRender_DrawRigidMesh** | Native Function Call Target | Direct3DRender_DrawRigidMesh - read interpolated pos from second context | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1463`<br>`_code_mainhooks.asm:7158` |
| `0x0049F00D` | **Direct3DRender_DrawSortedFaceGroup_Rigid** | Engine Native Hook Entry | set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:469`<br>`_code_mainhooks.asm:2037` |
| `0x0049F0A3` | **Direct3DRender_DrawSortedFaceGroup_Rigid** | Native Function Call Target | Direct3DRender_DrawSortedFaceGroup_Rigid - set viewport rendering mode | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:477`<br>`_code_mainhooks.asm:2081` |
| `0x0049F5AF` | **Direct3DRender_DrawSortedFaceGroup_Lightmap** | Engine Native Hook Entry | set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:481`<br>`_code_mainhooks.asm:2084` |
| `0x0049F645` | **Direct3DRender_DrawSortedFaceGroup_Lightmap** | Native Function Call Target | Direct3DRender_DrawSortedFaceGroup_Lightmap - set viewport rendering mode | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:489`<br>`_code_mainhooks.asm:2128` |
| `0x0049F766` | **Direct3DRender_DrawBoneMesh** | Engine Native Hook Entry | set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:493`<br>`_code_mainhooks.asm:2131` |
| `0x0049F80F` | **Direct3DRender_DrawBoneMesh** | Native Function Call Target | read interpolated pos from second context | Main Hooks, NewFPSLimiterPatch, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1471`<br>`_code_applypatches_id0.asm:501` |
| `0x0049F815` | **Direct3DRender_DrawBoneMesh** | Native Function Call Target | Direct3DRender_DrawBoneMesh - read interpolated pos from second context | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1473`<br>`_code_mainhooks.asm:7177` |
| `0x004A01DC` | **RigidDynCubeObj_EnterHandler** | Engine Native Hook Entry | interpolate (RigidDynCubeObj entities) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1421`<br>`_code_mainhooks.asm:6944` |
| `0x004A01F0` | **RigidDynCubeObj_EnterHandler** | Native Function Call Target | RigidDynCubeObj_EnterHandler - interpolate (RigidDynCubeObj entities) | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1427`<br>`_code_mainhooks.asm:7031` |
| `0x004A53B3` | **_sprintf** | Native Function Call Target | Config_FillRenderDeviceListBox | DisplayModesFix, Main Hooks | Manifest (): `Game_DataSymbolLoad` | `_code_applypatches_id0.asm:362`<br>`_code_applypatches_id0.asm:364` |
| `0x004B0F90` | **RenderMode_PopRenderMode** | Native Function Call Target | Config_Open - call to Config_UpdateDevicesDefMode | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:354`<br>`_code_mainhooks.asm:1332` |
| `0x004B6E60` | **Picture_Register** | Native Function Call Target | MenuScreen_DrawHandler - draw background picture with custom scaling mode | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:780`<br>`_code_mainhooks.asm:3119` |
| `0x004CFE74` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:551`<br>`_code_mainhooks.asm:2429` |
| `0x004CFE7A` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Native Function Call Target | ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD) - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:555`<br>`_code_mainhooks.asm:2436` |
| `0x004CFE95` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:557`<br>`_code_mainhooks.asm:2439` |
| `0x004CFE9B` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Native Function Call Target | ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD) - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:561`<br>`_code_mainhooks.asm:2446` |
| `0x004CFEB2` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:563`<br>`_code_mainhooks.asm:2449` |
| `0x004CFEB8` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Native Function Call Target | ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD) - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:567`<br>`_code_mainhooks.asm:2456` |
| `0x004CFEF2` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:569`<br>`_code_mainhooks.asm:2459` |
| `0x004CFEF8` | **ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD)** | Native Function Call Target | ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD) - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:573`<br>`_code_mainhooks.asm:2466` |
| `0x004D0097` | **Mesh3D_GetBoneMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:577`<br>`_code_mainhooks.asm:2469` |
| `0x004D009D` | **Mesh3D_GetBoneMeshLOD** | Native Function Call Target | Mesh3D_GetBoneMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:581`<br>`_code_mainhooks.asm:2476` |
| `0x004D00B7` | **Mesh3D_GetBoneMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:583`<br>`_code_mainhooks.asm:2479` |
| `0x004D00BD` | **Mesh3D_GetBoneMeshLOD** | Native Function Call Target | Mesh3D_GetBoneMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:587`<br>`_code_mainhooks.asm:2486` |
| `0x004D00D4` | **Mesh3D_GetBoneMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:589`<br>`_code_mainhooks.asm:2489` |
| `0x004D00DA` | **Mesh3D_GetBoneMeshLOD** | Native Function Call Target | Mesh3D_GetBoneMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:593`<br>`_code_mainhooks.asm:2496` |
| `0x004D010A` | **Mesh3D_GetBoneMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:595`<br>`_code_mainhooks.asm:2499` |
| `0x004D0110` | **Mesh3D_GetBoneMeshLOD** | Native Function Call Target | Mesh3D_GetBoneMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:599`<br>`_code_mainhooks.asm:2506` |
| `0x004D02DD` | **Mesh3D_GetSplineMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:603`<br>`_code_mainhooks.asm:2509` |
| `0x004D02E3` | **Mesh3D_GetSplineMeshLOD** | Native Function Call Target | Mesh3D_GetSplineMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:607`<br>`_code_mainhooks.asm:2516` |
| `0x004D02FD` | **Mesh3D_GetSplineMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:609`<br>`_code_mainhooks.asm:2519` |
| `0x004D0303` | **Mesh3D_GetSplineMeshLOD** | Native Function Call Target | Mesh3D_GetSplineMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:613`<br>`_code_mainhooks.asm:2526` |
| `0x004D031A` | **Mesh3D_GetSplineMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:615`<br>`_code_mainhooks.asm:2529` |
| `0x004D0320` | **Mesh3D_GetSplineMeshLOD** | Native Function Call Target | Mesh3D_GetSplineMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:619`<br>`_code_mainhooks.asm:2536` |
| `0x004D0350` | **Mesh3D_GetSplineMeshLOD** | Engine Native Hook Entry | fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:621`<br>`_code_mainhooks.asm:2539` |
| `0x004D0356` | **Mesh3D_GetSplineMeshLOD** | Native Function Call Target | Mesh3D_GetSplineMeshLOD - fix lod quality and rendering distance | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:625`<br>`_code_mainhooks.asm:2546` |
| `0x004D9610` | **BoneDynCubeObj_GetQTaskType** | Native Function Call Target | HumanView_EnterHandler - interpolate (1st person view model) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1399`<br>`_code_mainhooks.asm:6745` |
| `0x004E0C8A` | **Shadow_MagicObjEnterHandler** | Engine Native Hook Entry | interpolate | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1481`<br>`_code_mainhooks.asm:7180` |
| `0x004E0C8F` | **Shadow_MagicObjEnterHandler** | Native Function Call Target | Shadow_MagicObjEnterHandler - interpolate | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1483`<br>`_code_mainhooks.asm:7219` |
| `0x004E7F50` | **ViewportQTask_DrawHandler** | Engine Native Hook Entry | interpolate (camera) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1389`<br>`_code_mainhooks.asm:6633` |
| `0x004E7F62` | **ViewportQTask_DrawHandler** | Native Function Call Target | ViewportQTask_DrawHandler - interpolate (camera) | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1393`<br>`_code_mainhooks.asm:6703` |
| `0x004E7F97` | **ViewportQTask_DrawHandler** | Engine Native Hook Entry | capture trans context for the binoculars overlay | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1219`<br>`_code_mainhooks.asm:5940` |
| `0x004E7FA0` | **ViewportQTask_DrawHandler** | Native Function Call Target | ViewportQTask_DrawHandler - capture trans context for the binoculars overlay | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1223`<br>`_code_mainhooks.asm:5954` |
| `0x005124EB` | **BoneDynCubeObj_EnterHandler** | Engine Native Hook Entry | interpolate (BoneDynCubeObj entities) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1437`<br>`_code_mainhooks.asm:7034` |
| `0x0051250D` | **BoneDynCubeObj_EnterHandler** | Native Function Call Target | BoneDynCubeObj_EnterHandler - interpolate (BoneDynCubeObj entities) | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1443`<br>`_code_mainhooks.asm:7115` |
| `0x005382E8` | **"No D3D drivers..."** | Global Variable / State Field | Config_FillDisplayDevices - un-inlined code from Config_Open turned into a function | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:338`<br>`_code_mainhooks.asm:1419` |
| `0x005488C8` | **Screen_eDrawQTaskEvent** | Global Variable / State Field | Flow_DrawChildren - rewrite inlined code as standalone function | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:929`<br>`_code_mainhooks.asm:4263` |
| `0x00567C74` | **sdefault** | Global Variable / State Field | Config_FillRenderDeviceListBox | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:370`<br>`_code_applypatches_id0.asm:378` |
| `0x0057C108` | **Elevator_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1361`<br>`_code_mainhooks.asm:6367` |
| `0x0057C1B8` | **Switch_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1363`<br>`_code_mainhooks.asm:6374` |
| `0x0057C1C0` | **Door_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1359`<br>`_code_mainhooks.asm:6360` |
| `0x005BDAF8` | **Human_eQTaskType** | Global Variable / State Field | QTask_IsBDCOTaskTypeInterpolable - checks if specified BoneDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1369`<br>`_code_mainhooks.asm:6405` |
| `0x005BDC40` | **HumanView_eQTaskType** | Global Variable / State Field | QTask_IsBDCOTaskTypeInterpolable - checks if specified BoneDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1373`<br>`_code_mainhooks.asm:6417` |
| `0x005BE3C4` | **GunCasing_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1365`<br>`_code_mainhooks.asm:6381` |
| `0x005BE3C6` | **Gun_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1353`<br>`_code_mainhooks.asm:6341` |
| `0x00A774B0` | **PhysicsObj_eQTaskType** | Global Variable / State Field | QTask_IsRDCOTaskTypeInterpolable - checks if specified RigidDynCubeObj is interpolable | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1355`<br>`_code_mainhooks.asm:6348` |
| `0x00A94E84` | **RenderMode_tActiveRenderMode.apfRenderModeMethod** | Global Variable / State Field | RigidDynCubeObj_EnterHandler - interpolate (RigidDynCubeObj entities) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1425`<br>`_code_applypatches_id0.asm:1441` |
| `0x00B46D14` | **Mesh3D_eDrawRigidMeshRenderModeMethod** | Global Variable / State Field | RigidDynCubeObj_EnterHandler - interpolate (RigidDynCubeObj entities) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1423`<br>`_code_mainhooks.asm:7014` |
| `0x00B81700` | **Mesh3D_avOverrideFOV** | Global Variable / State Field | Direct3DRender_DrawRigidMesh - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:461`<br>`_code_applypatches_id0.asm:473` |
| `0x00B81880` | **Mesh3D_eDrawBoneMeshRenderModeMethod** | Global Variable / State Field | BoneDynCubeObj_EnterHandler - interpolate (BoneDynCubeObj entities) | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1439`<br>`_code_mainhooks.asm:7092` |
| `0x00BCAAE0` | **TransContext_tActiveTransContext** | Global Variable / State Field | Direct3DRender_DrawRigidMesh - set viewport rendering mode | Main Hooks, NewFPSLimiterPatch, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1221`<br>`_code_applypatches_id0.asm:459` |
| `0x00BCAB08` | **TransContext_tActiveTransContext.tPos.vX** | Global Variable / State Field | Direct3DRender_DrawRigidMesh - read interpolated pos from second context | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1449`<br>`_code_applypatches_id0.asm:1461` |
| `0x00BCAB24` | **TransContext_tActiveTransContext.vFOVY** | Global Variable / State Field | ModelObj_GetLOD (aka Mesh3D_GetRigidMeshLOD) - fix lod quality and rendering distance | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:553`<br>`_code_applypatches_id0.asm:559` |
| `0x00BCABA0` | **RenderContext_tActiveRenderContext** | Global Variable / State Field | TransContext_UpdateFOVData - helper function to set fov vars based on rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:441`<br>`_code_mainhooks.asm:1894` |
| `0x00BCABB8` | **RenderContext_tActiveRenderContext.tClippingWindow.tClippingRect.vX** | Global Variable / State Field | TransContext_Create - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:447`<br>`_code_mainhooks.asm:1964` |
| `0x00BCABBC` | **RenderContext_tActiveRenderContext.tClippingWindow.tClippingRect.vY** | Global Variable / State Field | TransContext_Create - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:451`<br>`_code_mainhooks.asm:1971` |
| `0x00BCABC8` | **RenderContext_tActiveRenderContext.tClippingWindow.tClippingRect.vHalfWidth** | Global Variable / State Field | TransContext_Create - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:449`<br>`_code_mainhooks.asm:1966` |
| `0x00BCABCC` | **RenderContext_tActiveRenderContext.tClippingWindow.tClippingRect.vHalfHeight** | Global Variable / State Field | TransContext_Create - set viewport rendering mode | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:453`<br>`_code_mainhooks.asm:1973` |


### 1. Input & Cursor Management (39 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00401530` | **QTask_LinkQTaskEventHandlerNoContextFn** | Native Function Call Target | Cursor_Open - add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1007`<br>`_code_applypatches_id0.asm:1019` |
| `0x00424C08` | **Cursor_Open** | Engine Native Hook Entry | add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:971`<br>`_code_mainhooks.asm:5150` |
| `0x00424C20` | **Cursor_CreateHandler** | Engine Native Hook Entry | fix cursor not being properly centered | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:822`<br>`_code_mainhooks.asm:3374` |
| `0x00424C2E` | **Cursor_CreateHandler** | Native Function Call Target | Cursor_CreateHandler - fix cursor not being properly centered | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:828`<br>`_code_mainhooks.asm:3389` |
| `0x00424C3B` | **Cursor_CreateHandler** | Engine Native Hook Entry | register interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:993`<br>`_code_mainhooks.asm:5204` |
| `0x00424DEA` | **Cursor_RunHandler** | Engine Native Hook Entry | do not update buttons down state during interpolation | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:983`<br>`_code_mainhooks.asm:5186` |
| `0x00424DF0` | **Cursor_RunHandler** | Native Function Call Target | Cursor_RunHandler - do not update buttons down state during interpolation | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:985`<br>`_code_mainhooks.asm:5201` |
| `0x0048FC20` | **Mouse_Update** | Native Function Call Target | Flow_UpdateCInput - new function to handle short interval input updates | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1497`<br>`_code_mainhooks.asm:7236` |
| `0x0048FC7B` | **Mouse_Update** | Engine Native Hook Entry | fix mouse sensitivity being tied to fps / add option for custom mouse sensitivty multiplier / add IsLocked support | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1527`<br>`_code_mainhooks.asm:7463` |
| `0x0048FC9B` | **Mouse_Update** | Native Function Call Target | Mouse_Update - fix mouse sensitivity being tied to fps / add option for custom mouse sensitivty multiplier / add IsLocked support | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1529`<br>`_code_mainhooks.asm:7481` |
| `0x0048FED0` | **Mouse_WinProcCB** | Engine Native Hook Entry | fix mouse motion/buttons getting stuck when window gets out of focus | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1519`<br>`_code_mainhooks.asm:7434` |
| `0x0048FED9` | **Mouse_WinProcCB** | Native Function Call Target | Mouse_WinProcCB - fix mouse motion/buttons getting stuck when window gets out of focus | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1523`<br>`_code_mainhooks.asm:7453` |
| `0x00490230` | **Keyboard_Update** | Native Function Call Target | InputPort_RunHandler_NEW - rewrite function | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1547`<br>`_code_mainhooks.asm:7620` |
| `0x004C1790` | **QTaskList_Register** | Native Function Call Target | Cursor_CreateHandler - register interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1031`<br>`_code_applypatches_id0.asm:1051` |
| `0x004C17C0` | **QTaskList_Remove** | Native Function Call Target | Cursor_DeleteHandler_NEW - unregister interpolation list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1037`<br>`_code_applypatches_id0.asm:1057` |
| `0x004E7B80` | **DebugText_SetCursor** | Native Function Call Target | DebugText_Reset | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:891`<br>`_code_mainhooks.asm:3957` |
| `0x004ED420` | **InputPort_InputHandler** | Engine Native Hook Entry | point to Mouse_vAnalogX/Y | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1583`<br>`_code_mainhooks.asm:7805` |
| `0x00507EC0` | **Input_Run** | Native Function Call Target | InputPort_Update - new function equivalent to InputPort_RunHandler minus the per-device update calls | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1557`<br>`_code_mainhooks.asm:7658` |
| `0x00509CF0` | **Joypad_Update** | Native Function Call Target | InputPort_RunHandler_NEW - rewrite function | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1549`<br>`_code_mainhooks.asm:7622` |
| `0x0057BC58` | **Cursor_nMouseX** | Global Variable / State Field | Cursor_GetWindowedCursorPos | Main Hooks, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:227`<br>`_code_mainhooks.asm:561` |
| `0x0057BC5C` | **Cursor_nMouseY** | Global Variable / State Field | Cursor_GetWindowedCursorPos | Main Hooks, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:229`<br>`_code_mainhooks.asm:563` |
| `0x0057BC60` | **Cursor_eQTaskType** | Global Variable / State Field | Cursor_Open - add interpolation handler | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:973`<br>`_code_mainhooks.asm:5154` |
| `0x005C8BC4` | **AppContext_tAppContext.hWnd** | Global Variable / State Field | Cursor_GetCursorPos | Main Hooks, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:233`<br>`_code_mainhooks.asm:600` |
| `0x005C8BE0` | **AppContext_tAppContext.isWinMessageInput** | Global Variable / State Field | Mouse_ClearInput - new function to clear mouse input | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1507`<br>`_code_applypatches_id0.asm:1521` |
| `0x005C8C00` | **AppContext_tAppContext.isFullscreen** | Global Variable / State Field | Cursor_GetCursorPos | BorderlessPatch, Main Hooks, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:235`<br>`_code_applypatches_id0.asm:264` |
| `0x005C8E08` | **Mouse_nMouseWheel** | Global Variable / State Field | Mouse_ClearInput - new function to clear mouse input | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1515`<br>`_code_mainhooks.asm:7427` |
| `0x005C8E0C` | **Mouse_nRButtonDown** | Global Variable / State Field | Mouse_ClearInput - new function to clear mouse input | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1511`<br>`_code_mainhooks.asm:7423` |
| `0x005C8E10` | **Mouse_nLButtonDown** | Global Variable / State Field | Mouse_ClearInput - new function to clear mouse input | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1509`<br>`_code_mainhooks.asm:7421` |
| `0x005C8E18` | **Mouse_nMButtonDown** | Global Variable / State Field | Mouse_ClearInput - new function to clear mouse input | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1513`<br>`_code_mainhooks.asm:7425` |
| `0x00A5EF9C` | **InputPort_eCurrentPort** | Global Variable / State Field | InputPort_Update - new function equivalent to InputPort_RunHandler minus the per-device update calls | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1555`<br>`_code_applypatches_id0.asm:1561` |
| `0x00BC20A0` | **InputPort.atInputPort[0]** | Global Variable / State Field | InputPort_Update - new function equivalent to InputPort_RunHandler minus the per-device update calls | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1553` |
| `0x00BC20AC` | **InputPort.atInputPort[0].eAnalogInputPortDevice** | Global Variable / State Field | InputPort_UpdateOnRun - new function to update analog x/y from current phase | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1563`<br>`_code_applypatches_id0.asm:1569` |
| `0x00BC20B0` | **InputPort.atInputPort[0].vX** | Global Variable / State Field | InputPort_InputHandler - point to Mouse_vAnalogX/Y | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1585`<br>`_code_mainhooks.asm:7814` |
| `0x00BC20B4` | **InputPort.atInputPort[0].vY** | Global Variable / State Field | InputPort_InputHandler - point to Mouse_vAnalogX/Y | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1587`<br>`_code_mainhooks.asm:7818` |
| `0x00BC20B8` | **InputPort.atInputPort[0].v0** | Global Variable / State Field | InputPort_InputHandler - point to Mouse_vAnalogX/Y | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1589`<br>`_code_mainhooks.asm:7822` |
| `0x00BC20BC` | **InputPort.atInputPort[0].v1** | Global Variable / State Field | InputPort_InputHandler - point to Mouse_vAnalogX/Y | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1591`<br>`_code_mainhooks.asm:7826` |
| `0x00C28B44` | **Display_tActiveMode.nWidth** | Global Variable / State Field | Cursor_UpdatePosition | Main Hooks, MainMenuPatch, MouseCursorPatch, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:239`<br>`_code_applypatches_id0.asm:509` |
| `0x00C28B48` | **Display_tActiveMode.nHeight** | Global Variable / State Field | Cursor_UpdatePosition | Main Hooks, MainMenuPatch, MouseCursorPatch, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:241`<br>`_code_applypatches_id0.asm:507` |
| `0x00C28F8C` | **Mouse_tMouse.bButton** | Global Variable / State Field | Cursor_RunHandler_NEW - rewritten function | Main Hooks, MouseCursorPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1505`<br>`_code_applypatches_id0.asm:1533` |


### 1. Game Loop & Flow Management (39 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00401810` | **QTask_NewQTaskEventNoContextFn** | Native Function Call Target | Flow_Open - allocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:953`<br>`_code_mainhooks.asm:5108` |
| `0x004018E0` | **QTask_DeleteQTaskEventNoContext** | Native Function Call Target | Flow_Close - deallocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:963`<br>`_code_mainhooks.asm:5135` |
| `0x00401A20` | **QTask_DeleteQTaskType** | Native Function Call Target | Flow_Close - deallocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:967`<br>`_code_mainhooks.asm:5143` |
| `0x00402065` | **Flow_Open** | Engine Native Hook Entry | allocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:949`<br>`_code_mainhooks.asm:5102` |
| `0x004020B0` | **Flow_CreateHandler** | Engine Native Hook Entry | init new members | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:855`<br>`_code_mainhooks.asm:3802` |
| `0x004020C9` | **Flow_CreateHandler** | Native Function Call Target | Flow_CreateHandler - init new members | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:859`<br>`_code_mainhooks.asm:3815` |
| `0x004021E7` | **Flow_CreateHandler** | Engine Native Hook Entry | Hook entry for Flow_CreateHandler | CDCheckPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:181`<br>`_code_mainhooks.asm:260` |
| `0x00402239` | **Flow_CreateHandler** | Native Function Call Target | Flow_CreateHandler | CDCheckPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:183`<br>`_code_mainhooks.asm:265` |
| `0x00402260` | **Flow_RunHandler** | Engine Native Hook Entry | rewrite fps limiter | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:935`<br>`_code_mainhooks.asm:4330` |
| `0x0040262E` | **Flow_RunHandler** | Native Function Call Target | Flow_RunHandler - rewrite fps limiter | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:937`<br>`_code_mainhooks.asm:4614` |
| `0x004027E0` | **Flow_Close** | Engine Native Hook Entry | deallocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:959`<br>`_code_mainhooks.asm:5123` |
| `0x00402820` | **Flow_SetFrequency** | Engine Native Hook Entry | init new members | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:867` |
| `0x00415F41` | **Game_CreateHandler** | Engine Native Hook Entry | Hook / relocation point in binary | CDCheckPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:177` |
| `0x004162A9` | **Game_RunHandler** | Engine Native Hook Entry | Hook entry for Game_RunHandler | CDCheckPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:169`<br>`_code_mainhooks.asm:250` |
| `0x00416300` | **Game_RunHandler** | Native Function Call Target | Game_RunHandler | CDCheckPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:173`<br>`_code_mainhooks.asm:257` |
| `0x004675B0` | **ComputerObject_ProjectRotatedPos** | Engine Native Hook Entry | fix aspect ratio | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:505` |
| `0x00467620` | **ComputerObject_ProjectWorldPos** | Engine Native Hook Entry | fix aspect ratio | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:517` |
| `0x0046A531` | **ComputerMap_Trace** | Engine Native Hook Entry | fix aspect ratio | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:529`<br>`_code_mainhooks.asm:2343` |
| `0x0046A59A` | **ComputerMap_Trace** | Native Function Call Target | ComputerMap_Trace - fix aspect ratio | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:539`<br>`_code_mainhooks.asm:2392` |
| `0x0046BD84` | **Computer_RunHandler** | Engine Native Hook Entry | fix aspect ratio | Main Hooks, WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:543`<br>`_code_mainhooks.asm:2395` |
| `0x0046BD8A` | **Computer_RunHandler** | Native Function Call Target | Computer_RunHandler - fix aspect ratio | WidescreenPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:545`<br>`_code_mainhooks.asm:2426` |
| `0x004C1800` | **QTaskList_New** | Native Function Call Target | Flow_Open - allocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:955`<br>`_code_mainhooks.asm:5115` |
| `0x004C1830` | **QTaskList_Delete** | Native Function Call Target | Flow_Close - deallocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:961`<br>`_code_mainhooks.asm:5128` |
| `0x004E6030` | **SoundSys_SetFrequency** | Native Function Call Target | Flow_SetFrequency - init new members | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:871`<br>`_code_mainhooks.asm:3903` |
| `0x004F5245` | **CutScene_RunHandler** | Engine Native Hook Entry | save CutScene_isRunning | Main Hooks, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:75`<br>`_code_mainhooks.asm:215` |
| `0x004F524C` | **CutScene_RunHandler** | Native Function Call Target | CutScene_RunHandler - save CutScene_isRunning | [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:77`<br>`_code_mainhooks.asm:219` |
| `0x004F525B` | **CutScene_RunHandler** | Engine Native Hook Entry | save CutScene_isRunning | Main Hooks, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:81`<br>`_code_mainhooks.asm:229` |
| `0x004F5261` | **CutScene_RunHandler** | Native Function Call Target | CutScene_RunHandler - save CutScene_isRunning | [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:79`<br>`_code_applypatches_id0.asm:83` |
| `0x005362F4` | **'LOCAL:config.qsc'** | Global Variable / State Field | Flow_CreateHandler - init new members | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:857` |
| `0x00539560` | **Game_iMissionID** | Global Variable / State Field | Game_RunHandler | CDCheckPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:171`<br>`_code_mainhooks.asm:253` |
| `0x00567C7C` | **Flow_eQTaskType** | Global Variable / State Field | Flow_Open - allocate interpolation task event/list | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:951`<br>`_code_applypatches_id0.asm:965` |
| `0x00567C8C` | **stdcall Patcher_WriteAddressReloc,Flow_CalcInterpolatedPos32.fixup1,edi,0x00567C8C,esi,FALSE ;Flow_ptFlow** | Global Variable / State Field | Flow_GetTicks (TODO: Flow_GetPlayTicks/Flow_GetPausedTicks) | Main Hooks, MainMenuPatch, NewFPSLimiterPatch, Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1073`<br>`_code_applypatches_id0.asm:1109` |
| `0x0057B194` | **GameFunctions_isEnableDebugKeys** | Global Variable / State Field | GameFunctions_SetEnableDebugKeys | DebugFeaturesPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:640`<br>`_code_mainhooks.asm:2650` |
| `0x0057BABC` | **Game_ptGame** | Global Variable / State Field | Game_IsPaused | Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:106`<br>`_code_sharedfuncs.asm:21` |
| `0x005BDD98` | **ComputerMap_ptComputerMap** | Global Variable / State Field | Flow_IsInterpSuppressed | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:907`<br>`_code_mainhooks.asm:4058` |
| `0x005C8BFC` | **AppContext_tAppContext.isActive** | Global Variable / State Field | Flow_IsFPSLocked | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:903`<br>`_code_mainhooks.asm:4018` |
| `0x00A96AE0` | **QTask_aatQTaskFinalEvent** | Global Variable / State Field | Flow_RunChildren - rewrite inlined code as standalone function | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1023`<br>`_code_applypatches_id0.asm:1025` |
| `0x00AFA6E0` | **QTask_atProtectedQTaskStack** | Global Variable / State Field | Flow_RunChildren - rewrite inlined code as standalone function | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:911`<br>`_code_applypatches_id0.asm:925` |
| `0x00AFA7E0` | **QTask_nProtectedQTaskStackOffset** | Global Variable / State Field | Flow_RunChildren - rewrite inlined code as standalone function | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:913`<br>`_code_applypatches_id0.asm:927` |


### 1. Windowing, Display & Video Modes (37 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x004035F5` | **Config_Open** | Engine Native Hook Entry | init new display devices struct | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:342`<br>`_code_mainhooks.asm:1128` |
| `0x0040362D` | **Config_Open** | Native Function Call Target | Config_Open - init new display devices struct | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:344`<br>`_code_mainhooks.asm:1137` |
| `0x00404450` | **Config_FillScreenResolutionListBox** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:392` |
| `0x00404510` | **Config_GraphicOptionsSetResolution** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:404` |
| `0x004045B0` | **Config_GraphicOptionsGetResolution** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:414` |
| `0x00418B38` | **MenuManager_New** | Engine Native Hook Entry | custom main menu resolution | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:700`<br>`_code_mainhooks.asm:2683` |
| `0x00418B50` | **MenuManager_New** | Native Function Call Target | MenuManager_New - custom main menu resolution | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:702`<br>`_code_mainhooks.asm:2732` |
| `0x00421AB9` | **MenuScreen_UpdateInternalDataHandler** | Engine Native Hook Entry | fix decentered EU logo | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:728`<br>`_code_mainhooks.asm:2779` |
| `0x00421AE2` | **MenuScreen_UpdateInternalDataHandler** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix decentered EU logo | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:732`<br>`_code_mainhooks.asm:2786` |
| `0x00421AEC` | **MenuScreen_UpdateInternalDataHandler** | Engine Native Hook Entry | fix logo position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:736`<br>`_code_mainhooks.asm:2789` |
| `0x00421B53` | **MenuScreen_UpdateInternalDataHandler** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix logo position | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:752`<br>`_code_mainhooks.asm:2851` |
| `0x0048A452` | **LoadingScreen_New** | Engine Native Hook Entry | scale loading screen properly | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:300`<br>`_code_mainhooks.asm:992` |
| `0x0048A499` | **LoadingScreen_New** | Native Function Call Target | LoadingScreen_New - scale loading screen properly | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:302`<br>`_code_mainhooks.asm:1029` |
| `0x0048A4AD` | **LoadingScreen_New** | Engine Native Hook Entry | scale loading screen properly | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:306`<br>`_code_mainhooks.asm:1032` |
| `0x0048A4B3` | **LoadingScreen_New** | Native Function Call Target | LoadingScreen_New - scale loading screen properly | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:308`<br>`_code_mainhooks.asm:1041` |
| `0x0048A50D` | **LoadingScreen_New** | Engine Native Hook Entry | scale loading screen properly | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:312`<br>`_code_mainhooks.asm:1044` |
| `0x0048A53E` | **LoadingScreen_New** | Native Function Call Target | LoadingScreen_New - scale loading screen properly | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:314`<br>`_code_mainhooks.asm:1073` |
| `0x0048F0A0` | **AppContext_GethWnd** | Native Function Call Target | Display_SetMode - resolution change | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:286`<br>`_code_mainhooks.asm:956` |
| `0x0048F6F3` | **WinMain** | Engine Patch Point | window creation | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:272` |
| `0x0048F724` | **WinMain** | Engine Native Hook Entry | window creation | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:274`<br>`_code_mainhooks.asm:909` |
| `0x0048F781` | **WinMain** | Native Function Call Target | WinMain - window creation | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:280`<br>`_code_mainhooks.asm:940` |
| `0x0048F790` | **WinMain** | Native Function Call Target | WinMain - window creation | BorderlessPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:278`<br>`_code_mainhooks.asm:935` |
| `0x00491A90` | **Display_SetMode** | Native Function Call Target | MenuManager_New - change background color (only visible if bg pic does not fully cover the client area) | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:708`<br>`_code_mainhooks.asm:2744` |
| `0x00491B7B` | **Display_SetMode** | Engine Native Hook Entry | resolution change | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:284`<br>`_code_mainhooks.asm:943` |
| `0x00491C05` | **Display_SetMode** | Engine Native Hook Entry | save width, height and aspect ratio from current active display mode | BorderlessPatch, Main Hooks, [WidescreenPatch, NewFPSLimiterPatch, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:288`<br>`_code_applypatches_id0.asm:39` |
| `0x00491C0D` | **Display_SetMode** | Native Function Call Target | Display_SetMode - save width, height and aspect ratio from current active display mode | [WidescreenPatch, NewFPSLimiterPatch, MouseCursorPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:41`<br>`_code_mainhooks.asm:15` |
| `0x00491CF0` | **Display_GetActiveMode** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix logo position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:742`<br>`_code_applypatches_id0.asm:750` |
| `0x00491E70` | **Display_SetBackgroundColourFn** | Native Function Call Target | MenuManager_New - change background color (only visible if bg pic does not fully cover the client area) | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:710`<br>`_code_mainhooks.asm:2752` |
| `0x004B6E70` | **Picture_GetWidth** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix logo position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:738`<br>`_code_applypatches_id0.asm:758` |
| `0x004B6E80` | **Picture_GetHeight** | Native Function Call Target | MenuScreen_UpdateInternalDataHandler - fix logo position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:746`<br>`_code_applypatches_id0.asm:760` |
| `0x004E76E0` | **TextWindow_vprintf** | Native Function Call Target | DebugText_printf_NEW | Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:130`<br>`_code_sharedfuncs.asm:84` |
| `0x00533504` | **flt_533504** | Global Variable / State Field | MenuScreen_UpdateInternalDataHandler - fix logo position | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:740`<br>`_code_applypatches_id0.asm:748` |
| `0x00541D68` | **zClassName** | Global Variable / State Field | WinMain - window creation | BorderlessPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:276`<br>`_code_mainhooks.asm:923` |
| `0x0057BC0C` | **dword_57BC0C** | Global Variable / State Field | MenuScreen_UpdateInternalDataHandler - fix decentered EU logo | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:730`<br>`_code_applypatches_id0.asm:744` |
| `0x00A5EBD0` | **DebugText_ptWindow** | Global Variable / State Field | DebugText_printf_NEW | Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:128`<br>`_code_sharedfuncs.asm:78` |
| `0x00A70C5A` | **ScreenGrab_isGrabSingle** | Global Variable / State Field | Flow_IsUnlimitedFPS | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:899`<br>`_code_mainhooks.asm:3977` |
| `0x00A70C5B` | **ScreenGrab_isRecord** | Global Variable / State Field | Flow_IsUnlimitedFPS | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:897`<br>`_code_mainhooks.asm:3974` |


### 1. World Objects & Interactive Entities (12 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x0043D273` | **Elevator_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1311`<br>`_code_mainhooks.asm:6235` |
| `0x0043D28A` | **Elevator_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1319`<br>`_code_mainhooks.asm:6255` |
| `0x0043D290` | **Elevator_RunHandler** | Native Function Call Target | Elevator_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1321`<br>`_code_mainhooks.asm:6262` |
| `0x0043DD90` | **Elevator_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1315`<br>`_code_mainhooks.asm:6246` |
| `0x004450DD` | **Switch_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1325`<br>`_code_mainhooks.asm:6265` |
| `0x004450F4` | **Switch_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1333`<br>`_code_mainhooks.asm:6286` |
| `0x004450F9` | **Switch_RunHandler** | Native Function Call Target | Switch_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1335`<br>`_code_mainhooks.asm:6297` |
| `0x004452D1` | **Switch_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1329`<br>`_code_mainhooks.asm:6277` |
| `0x00446AAF` | **Door_CreateHandler** | Engine Native Hook Entry | alloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1297`<br>`_code_mainhooks.asm:6205` |
| `0x00446B49` | **Door_DeleteHandler** | Engine Native Hook Entry | dealloc interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1301`<br>`_code_mainhooks.asm:6214` |
| `0x00446D1F` | **Door_RunHandler** | Engine Native Hook Entry | update interp data | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1305`<br>`_code_mainhooks.asm:6223` |
| `0x00446D27` | **Door_RunHandler** | Native Function Call Target | Door_RunHandler - update interp data | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1307`<br>`_code_mainhooks.asm:6232` |


### 1. Core Engine Subsystem (11 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x0048F360` | **AppMain_ParseCmdLineArgs** | Global Variable / State Field | WinMain - parse command-line commands | Main Hooks, [BorderlessPatch, DebugFeaturesPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:55`<br>`_code_mainhooks.asm:115` |
| `0x0048F53F` | **WinMain** | Engine Native Hook Entry | allow multiinstance | DebugFeaturesPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:684`<br>`_code_mainhooks.asm:2655` |
| `0x0048F544` | **WinMain** | Native Function Call Target | WinMain - allow multiinstance | DebugFeaturesPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:688`<br>`_code_mainhooks.asm:2676` |
| `0x0048F578` | **WinMain** | Native Function Call Target | WinMain - allow multiinstance | DebugFeaturesPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:686`<br>`_code_mainhooks.asm:2670` |
| `0x0048F674` | **WinMain** | Engine Native Hook Entry | init command-line commands | Main Hooks, [BorderlessPatch, DebugFeaturesPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:46`<br>`_code_mainhooks.asm:88` |
| `0x0048F679` | **WinMain** | Native Function Call Target | WinMain - init command-line commands | [BorderlessPatch, DebugFeaturesPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:48`<br>`_code_mainhooks.asm:107` |
| `0x0048F6D8` | **WinMain** | Engine Native Hook Entry | parse command-line commands | Main Hooks, [BorderlessPatch, DebugFeaturesPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:53`<br>`_code_mainhooks.asm:110` |
| `0x0048F6E0` | **WinMain** | Native Function Call Target | WinMain - parse command-line commands | [BorderlessPatch, DebugFeaturesPatch, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:57`<br>`_code_mainhooks.asm:137` |
| `0x004B0C60` | **Mem_AllocFn** | Native Function Call Target | DynCubeObj_AllocInterpData - function to automatically allocate/init the interp buffer | Main Hooks, NewFPSLimiterPatch | Manifest (): `Mem_Alloc` | `_code_applypatches_id0.asm:1243`<br>`_code_mainhooks.asm:6047` |
| `0x004B0D10` | **Mem_DeAllocFn** | Native Function Call Target | DynCubeObj_DeallocInterpData - function to automatically deallocate/init the interp buffer | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1247`<br>`_code_mainhooks.asm:6072` |
| `0x004B53B0` | **QSprite_Register4AZ** | Native Function Call Target | Q3DPicture_RegisterSize - implement picture scaling function fom IGI2 | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:774`<br>`_code_mainhooks.asm:3077` |


### 1. Configuration, Storage & Profiles (9 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00402C32` | **J_Open** | Engine Native Hook Entry | Hook / relocation point in binary | CDCheckPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:165` |
| `0x00403642` | **Config_Open** | Engine Native Hook Entry | call to Config_UpdateDevicesDefMode | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:352`<br>`_code_mainhooks.asm:1324` |
| `0x00403647` | **Config_Open** | Native Function Call Target | Config_Open - call to Config_UpdateDevicesDefMode | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:356`<br>`_code_mainhooks.asm:1334` |
| `0x00404590` | **Config_GetActiveGraphicOptions** | Native Function Call Target | Config_GraphicOptionsGetDevice | DisplayModesFix, Main Hooks | Manifest (): `Config_GetActiveGraphicOptions` | `_code_applypatches_id0.asm:376`<br>`_code_applypatches_id0.asm:386` |
| `0x00404710` | **Config_GraphicOptionsGetDevice** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:374` |
| `0x004047F0` | **Config_GraphicOptionsSetDevice** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:382` |
| `0x00405980` | **Config_VerifyGraphicConfig** | Engine Native Hook Entry | Hook / relocation point in binary | DisplayModesFix | New IGIPatch Finding | `_code_applypatches_id0.asm:424` |
| `0x004B8A20` | **Script_EvalArgInt** | Native Function Call Target | Config_GraphicOptionsSetDevice | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:384`<br>`_code_applypatches_id0.asm:394` |
| `0x00BC2380` | **Config_tConfig** | Global Variable / State Field | Config_VerifyGraphicConfig | DisplayModesFix, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:426`<br>`_code_mainhooks.asm:1786` |


### 1. UI, Menus & Loading Screen (7 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00418BDB` | **MenuManager_New** | Engine Native Hook Entry | change background color (only visible if bg pic does not fully cover the client area) | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:706`<br>`_code_mainhooks.asm:2735` |
| `0x00418BE8` | **MenuManager_New** | Native Function Call Target | MenuManager_New - change background color (only visible if bg pic does not fully cover the client area) | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:712`<br>`_code_mainhooks.asm:2757` |
| `0x00418CB5` | **MenuManager_New** | Engine Native Hook Entry | add BackgroundFX switch | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:716`<br>`_code_mainhooks.asm:2760` |
| `0x00418CC1` | **MenuManager_New** | Native Function Call Target | MenuManager_New - add BackgroundFX switch | MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:722`<br>`_code_mainhooks.asm:2776` |
| `0x00418CF7` | **MenuManager_New** | Engine Native Hook Entry | Hook entry for MenuManager_New | CDCheckPatch, Main Hooks | New IGIPatch Finding | `_code_applypatches_id0.asm:187`<br>`_code_mainhooks.asm:268` |
| `0x00418D53` | **MenuManager_New** | Native Function Call Target | MenuManager_New | CDCheckPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:189`<br>`_code_mainhooks.asm:274` |
| `0x004199D0` | **BackgroundFX_New** | Native Function Call Target | MenuManager_New - add BackgroundFX switch | Main Hooks, MainMenuPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:718`<br>`_code_mainhooks.asm:2768` |


### 1. Diagnostics, Logging & Debug (5 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x0048F1A0` | **AppContext_SetDebugged** | Native Function Call Target | Main_ParseDebugCB | DebugFeaturesPatch, Main Hooks | Manifest (): `App_ContextSetDebugged` | `_code_applypatches_id0.asm:656`<br>`_code_mainhooks.asm:2626` |
| `0x0048F1E0` | **AppContext_SetDebugtextState** | Native Function Call Target | Main_ParseDebugTextCB | DebugFeaturesPatch, Main Hooks | Manifest (): `App_ContextSetDebugtextState` | `_code_applypatches_id0.asm:652`<br>`_code_mainhooks.asm:2617` |
| `0x004AF7B0` | **LDebug_Error** | Native Function Call Target | LDebug_AbortWithError | Shared Functions, [NewFPSLimiterPatch | Manifest (): `Error_Show` | `_code_applypatches_id0.asm:101`<br>`_code_sharedfuncs.asm:9` |
| `0x004E7BB0` | **DebugText_Clear** | Native Function Call Target | DebugText_Reset | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:889`<br>`_code_mainhooks.asm:3953` |
| `0x00A5EA75` | **DebugText_isEnabled** | Global Variable / State Field | DebugText_printf_NEW | Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:126`<br>`_code_sharedfuncs.asm:71` |


### 1. Core Data & State Tables (4 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00533A80` | **fix inconsistent zoom level** | Engine Patch Point | Hook / relocation point in binary | MinorBugFixes (TODO) | New IGIPatch Finding | `_code_applypatches_id0.asm:1681` |
| `0x005C8E00` | **AppContext_isFixmeSmall** | Global Variable / State Field | Main_ParseSmallCB | DebugFeaturesPatch, Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:660`<br>`_code_applypatches_id0.asm:895` |
| `0x005C8E70` | **some movie related var** | Global Variable / State Field | Movie_IsPlaying_NEW | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:877` |
| `0x005C8E71` | **some movie related var** | Global Variable / State Field | Movie_IsPlaying_NEW | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:875` |


### 1. Animation Controller (3 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x004D4B95` | **sub_4D4B60** | Engine Native Hook Entry | scale 1st person anims progress | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1487`<br>`_code_mainhooks.asm:7222` |
| `0x004D4B9B` | **sub_4D4B60** | Native Function Call Target | sub_4D4B60 - scale 1st person anims progress | NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:1489`<br>`_code_mainhooks.asm:7230` |
| `0x00A54658` | **AnimController_nTicksPerFrame** | Global Variable / State Field | AnimController_GetTPFValue | Shared Functions, [NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:116`<br>`_code_applypatches_id0.asm:121` |


### 1. Engine BSS & Runtime Globals (3 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00A4438C` | **World_nTraceLines** | Global Variable / State Field | World_ResetTracedLines | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:881`<br>`_code_mainhooks.asm:3939` |
| `0x00A44390` | **World_nTraceLinesActual** | Global Variable / State Field | World_ResetTracedLines | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:883`<br>`_code_mainhooks.asm:3941` |
| `0x00A44394` | **World_nLineOfSights** | Global Variable / State Field | World_ResetTracedLines | Main Hooks, NewFPSLimiterPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:885`<br>`_code_mainhooks.asm:3943` |


### 1. Timing & Performance Counters (2 Symbols)

| Address (VA) | Native / Symbol Name | Symbol Type | IGIPatch Context / Description | Patch Module | OpenIGI Status | Source File |
|:---|:---|:---|:---|:---|:---|:---|
| `0x00490360` | **Timer_Open** | Engine Native Hook Entry | Hook / relocation point in binary | TimerTaskPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:201` |
| `0x00490370` | **Timer_Read** | Engine Native Hook Entry | Hook / relocation point in binary | TimerTaskPatch | New IGIPatch Finding | `_code_applypatches_id0.asm:205` |


## Key Insights & Discoveries for OpenIGI Engine

1. **Complete Handler Function VAs for Interactive World Entities**:
   - `Door` (`0x00446AAF` Create, `0x00446B49` Delete, `0x00446D1F` Run)
   - `Elevator` (`0x0043D273` Create, `0x0043DD90` Delete, `0x0043D28A` Run)
   - `Switch` (`0x004450DD` Create, `0x004452D1` Delete, `0x004450F4` Run)
   - `StationaryGun` (`0x00473EB8` Create, `0x0047497B` Delete, `0x00474195` Run, `0x00474186` Open)
   - `Gun` (`0x004785E5` Create, `0x004788C6` Delete, `0x00478649` Run)
   - `GunCasing` (`0x004782C1` Create, `0x00478505` Delete, `0x004782D0` Run)
   - `Computer` (`0x0046BD84` Run, `0x0046A531` Map Trace)

2. **Rendering Pipeline & Transformation Context Structures**:
   - `0x00497DE7`: `TransContext_Create`
   - `0x00497E70`: `TransContext_SetActiveTransContext`
   - `0x00BCAAE0`: `TransContext_tActiveTransContext` (Global active transform context pointer)
   - `0x00BCAB24`: `TransContext_tActiveTransContext.vFOVY` (Vertical FOV float)
   - `0x00BCAB08`: `TransContext_tActiveTransContext.tPos.vX` (World camera position)
   - `0x0049E006` / `0x0049E0B1` / `0x0049E272`: `Direct3DRender_DrawRigidMesh`
   - `0x0049F766` / `0x0049F80F`: `Direct3DRender_DrawBoneMesh`
   - `0x0049F00D`: `Direct3DRender_DrawSortedFaceGroup_Rigid`
   - `0x0049F5AF`: `Direct3DRender_DrawSortedFaceGroup_Lightmap`

3. **Input & Player Motion Controls**:
   - `0x0048FC20`: `Mouse_Update`
   - `0x00490230`: `Keyboard_Update`
   - `0x00509CF0`: `Joypad_Update`
   - `0x00507EC0`: `Input_Run`
   - `0x004ED420`: `InputPort_InputHandler`
   - `0x00BC20A0`: `InputPort.atInputPort[0]` array
   - `0x005E07E` / `0x0045E0E4`: `HumanPlayerInput_ReadChannels`
   - `0x004612CD`: `Human_UpdateBody`
   - `0x00464800`: `HumanView_UpdateView`

4. **Task Scheduling & Flow Management**:
   - `0x00401810`: `QTask_NewQTaskEventNoContextFn`
   - `0x00AFA6E0`: `QTask_atProtectedQTaskStack`
   - `0x00AFA7E0`: `QTask_nProtectedQTaskStackOffset`
   - `0x00A96AE0`: `QTask_aatQTaskFinalEvent`
   - `0x00567C8C`: `Flow_ptFlow`
   - `0x0057BABC`: `Game_ptGame`
   - `0x00539560`: `Game_iMissionID`
