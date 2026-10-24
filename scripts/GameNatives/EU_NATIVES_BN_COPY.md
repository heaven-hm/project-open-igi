# EU natives copied from Binary Ninja (mvp/level1-integration)

Generated: 2026-10-24T03:25:57+05:30
Source: Binary Ninja MCP http://127.0.0.1:24642/mcp (authorization=disabled)

## Summary

| Result | Count |
|---|---|
| Copied (new human-readable `.cpp` + valid `.hpp`) | 42 |
| Skipped (already present, BN body re-validated) | 12 |
| Not found in Binary Ninja | 1 |
| **Total in scope** | **55** |

"Already present" means a `.cpp` with the fresh `Source: Binary Ninja MCP decompile` marker already existed; each of those bodies was re-decompiled from BN and compared byte-for-byte, so they were left untouched.

## Status table

| Retail VA | BN native name | Status | File |
|---|---|---|---|
| `0x00401530` | QTask_LinkQTaskEventHandlerNoContextFn | copied | 00401530-QTask_LinkQTaskEventHandlerNoContextFn.cpp |
| `0x00401810` | QTask_NewQTaskEventNoContextFn | copied | 00401810-QTask_NewQTaskEventNoContextFn.cpp |
| `0x004018e0` | QTask_DeleteQTaskEventNoContext | copied | 004018e0-QTask_DeleteQTaskEventNoContext.cpp |
| `0x00401a20` | QTask_DeleteQTaskType | copied | 00401a20-QTask_DeleteQTaskType.cpp |
| `0x00401cf0` | QTask_IsQTaskTypeDerived | copied | 00401cf0-QTask_IsQTaskTypeDerived.cpp |
| `0x004028b0` | Flow_GetTicks | copied | 004028b0-Flow_GetTicks.cpp |
| `0x00404590` | Config_GetActiveGraphicOptions | copied | 00404590-Config_GetActiveGraphicOptions.cpp |
| `0x00406220` | Config_GetActivePlayerProfile | copied | 00406220-Config_GetActivePlayerProfile.cpp |
| `0x00414fd0` | GameFunctions_IsDebugKeyPressed | copied | 00414fd0-GameFunctions_IsDebugKeyPressed.cpp |
| `0x00415020` | GameFunctions_IsDebugKeyPressedOnce | skipped | - |
| `0x00416d20` | Game_GetRandomState | skipped | - |
| `0x004199d0` | BackgroundFX_New | skipped | - |
| `0x0041f2d0` | ListBox_AddItem | skipped | - |
| `0x00424ce0` | Cursor_RunHandler | copied | 00424ce0-Cursor_RunHandler.cpp |
| `0x004444e0` | - | not in BN | - |
| `0x004610c0` | AI_UpdateOrders_18 | copied | 004610c0-AI_UpdateOrders_18.cpp |
| `0x00464800` | HumanView_UpdateView | copied | 00464800-HumanView_UpdateView.cpp |
| `0x00464bd0` | HumanView_UpdateBody | copied | 00464bd0-HumanView_UpdateBody.cpp |
| `0x0048f0a0` | AppContext_GethWnd | copied | 0048f0a0-AppContext_GethWnd.cpp |
| `0x0048f1a0` | AppContext_SetDebugged | copied | 0048f1a0-AppContext_SetDebugged.cpp |
| `0x0048f1e0` | AppContext_SetDebugtextState | copied | 0048f1e0-AppContext_SetDebugtextState.cpp |
| `0x0048f240` | AppContext_SetLightmapsUsed | skipped | - |
| `0x0048f260` | AppContext_SetTerrainLightmapsUsed | copied | 0048f260-AppContext_SetTerrainLightmapsUsed.cpp |
| `0x0048f360` | AppMain_ParseCmdLineArgs | copied | 0048f360-AppMain_ParseCmdLineArgs.cpp |
| `0x0048fc20` | Mouse_Update | copied | 0048fc20-Mouse_Update.cpp |
| `0x00490230` | Keyboard_Update | copied | 00490230-Keyboard_Update.cpp |
| `0x00491a90` | Display_SetMode | copied | 00491a90-Display_SetMode.cpp |
| `0x00491cf0` | Display_GetActiveMode | copied | 00491cf0-Display_GetActiveMode.cpp |
| `0x00491e70` | Display_SetBackgroundColourFn | copied | 00491e70-Display_SetBackgroundColourFn.cpp |
| `0x00497e70` | Transcontext_SetActiveTransContext | copied | 00497e70-Transcontext_SetActiveTransContext.cpp |
| `0x004a53b3` | sprintf | copied | 004a53b3-sprintf.cpp |
| `0x004af7b0` | Debug_Error | skipped | - |
| `0x004b0c60` | Mem_AllocFn | copied | 004b0c60-Mem_AllocFn.cpp |
| `0x004b0d10` | Mem_DeAllocFn | copied | 004b0d10-Mem_DeAllocFn.cpp |
| `0x004b0f90` | RenderMode_PopRenderMode | skipped | - |
| `0x004b3320` | Real64x3_NullVector | skipped | - |
| `0x004b38e0` | Real32x9_Create | copied | 004b38e0-Real32x9_Create.cpp |
| `0x004b53b0` | Qsprite_Register4Az | skipped | - |
| `0x004b6e60` | Picture_Register | skipped | - |
| `0x004b6e70` | Picture_GetWidth | copied | 004b6e70-Picture_GetWidth.cpp |
| `0x004b6e80` | Picture_GetHeight | copied | 004b6e80-Picture_GetHeight.cpp |
| `0x004b8a20` | Script_EvalArgInt | copied | 004b8a20-Script_EvalArgInt.cpp |
| `0x004c1790` | QTaskList_Register | copied | 004c1790-QTaskList_Register.cpp |
| `0x004c17c0` | QTaskList_Remove | copied | 004c17c0-QTaskList_Remove.cpp |
| `0x004c1800` | QTaskList_New | copied | 004c1800-QTaskList_New.cpp |
| `0x004c1830` | QTaskList_Delete | copied | 004c1830-QTaskList_Delete.cpp |
| `0x004d3210` | AnimController_UpdateChannels | copied | 004d3210-AnimController_UpdateChannels.cpp |
| `0x004d9610` | BoneDynCubeObj_GetQTaskType | copied | 004d9610-BoneDynCubeObj_GetQTaskType.cpp |
| `0x004e6030` | SoundSys_SetFrequency | copied | 004e6030-SoundSys_SetFrequency.cpp |
| `0x004e76e0` | TextWindow_vprintf | copied | 004e76e0-TextWindow_vprintf.cpp |
| `0x004e7b80` | DebugText_SetCursor | copied | 004e7b80-DebugText_SetCursor.cpp |
| `0x004e7bb0` | DebugText_Clear | copied | 004e7bb0-DebugText_Clear.cpp |
| `0x004e8100` | SoundSys_EvaluateTrigger_11 | copied | 004e8100-SoundSys_EvaluateTrigger_11.cpp |
| `0x00507ec0` | Input_Run | skipped | - |
| `0x00509cf0` | Joypad_Update | skipped | - |

## Notes

* `0x004444e0` cannot be copied: `bn_function_prototype_get` returns `{"errorCode":"function_not_found"}` for that VA, consistent with the earlier check.
* Name corrections observed in BN: `0x004610c0` is `AI_UpdateOrders_18` (not `Human_UpdateBody`); `0x004af7b0` is `Debug_Error`; `0x00490230` is `Keyboard_Update`.
* Readability pass renames identifiers only. Branches, loops, call order, literals, casts and memory offsets are unchanged from BN Pseudo C; the structural checker in this run compared those sequences (42/42 pass).
* Two functions (`AI_UpdateOrders_18`, `HumanView_UpdateView`) and one more (`HumanView_UpdateBody`) contain x87 FPU-stack code BN could not lift. Their `unimplemented` blocks, FPU condition flags and register temporaries are preserved verbatim rather than guessed, as required by the project rule not to invent semantics. Their FPU constant globals were read from retail memory and named by value (0.0, pi, pi/2, -pi/2, 0.06, 2.0, ...).
* There is no runtime/behavioral verification here; these files are decompiled source recovery, not verified implementations.

## Stale files replaced (backups)

Where the destination already held a stale catalog `.cpp`/`.hpp` under the same name, the old file was copied to `/tmp/eu_overwritten/` before replacement:

* `00401530-QTask_LinkQTaskEventHandlerNoContextFn.cpp` (sha256 before: `3e867622aff6b110...`) -> backup `/tmp/eu_overwritten/00401530-QTask_LinkQTaskEventHandlerNoContextFn.cpp`
* `00401530-QTask_LinkQTaskEventHandlerNoContextFn.hpp` (sha256 before: `99507c040a4ee6b4...`) -> backup `/tmp/eu_overwritten/00401530-QTask_LinkQTaskEventHandlerNoContextFn.hpp`
* `00401810-QTask_NewQTaskEventNoContextFn.cpp` (sha256 before: `544b4aeb6748e527...`) -> backup `/tmp/eu_overwritten/00401810-QTask_NewQTaskEventNoContextFn.cpp`
* `00401810-QTask_NewQTaskEventNoContextFn.hpp` (sha256 before: `0841d067b4a501cc...`) -> backup `/tmp/eu_overwritten/00401810-QTask_NewQTaskEventNoContextFn.hpp`
* `004018e0-QTask_DeleteQTaskEventNoContext.cpp` (sha256 before: `e2c6f6fd01b809b9...`) -> backup `/tmp/eu_overwritten/004018e0-QTask_DeleteQTaskEventNoContext.cpp`
* `00401a20-QTask_DeleteQTaskType.cpp` (sha256 before: `65c4024012db9657...`) -> backup `/tmp/eu_overwritten/00401a20-QTask_DeleteQTaskType.cpp`
* `00401cf0-QTask_IsQTaskTypeDerived.cpp` (sha256 before: `6824bb50da6c01a5...`) -> backup `/tmp/eu_overwritten/00401cf0-QTask_IsQTaskTypeDerived.cpp`
* `00401cf0-QTask_IsQTaskTypeDerived.hpp` (sha256 before: `5231cea4fcec9be5...`) -> backup `/tmp/eu_overwritten/00401cf0-QTask_IsQTaskTypeDerived.hpp`
* `004028b0-Flow_GetTicks.cpp` (sha256 before: `f5924f93110f7c25...`) -> backup `/tmp/eu_overwritten/004028b0-Flow_GetTicks.cpp`
* `004028b0-Flow_GetTicks.hpp` (sha256 before: `27f158c3cb00c38c...`) -> backup `/tmp/eu_overwritten/004028b0-Flow_GetTicks.hpp`
* `00404590-Config_GetActiveGraphicOptions.cpp` (sha256 before: `221c6aed0965fa63...`) -> backup `/tmp/eu_overwritten/00404590-Config_GetActiveGraphicOptions.cpp`
* `00404590-Config_GetActiveGraphicOptions.hpp` (sha256 before: `a1c6b63c53a858d2...`) -> backup `/tmp/eu_overwritten/00404590-Config_GetActiveGraphicOptions.hpp`
* `00406220-Config_GetActivePlayerProfile.cpp` (sha256 before: `332999a5342eec1a...`) -> backup `/tmp/eu_overwritten/00406220-Config_GetActivePlayerProfile.cpp`
* `00406220-Config_GetActivePlayerProfile.hpp` (sha256 before: `819392ff475c2113...`) -> backup `/tmp/eu_overwritten/00406220-Config_GetActivePlayerProfile.hpp`
* `00414fd0-GameFunctions_IsDebugKeyPressed.cpp` (sha256 before: `2153a2df045a3ee7...`) -> backup `/tmp/eu_overwritten/00414fd0-GameFunctions_IsDebugKeyPressed.cpp`
* `00414fd0-GameFunctions_IsDebugKeyPressed.hpp` (sha256 before: `b1b51c2398619cd9...`) -> backup `/tmp/eu_overwritten/00414fd0-GameFunctions_IsDebugKeyPressed.hpp`
* `00424ce0-Cursor_RunHandler.cpp` (sha256 before: `46225ea6cf65ac16...`) -> backup `/tmp/eu_overwritten/00424ce0-Cursor_RunHandler.cpp`
* `00424ce0-Cursor_RunHandler.hpp` (sha256 before: `6188086e2453ac55...`) -> backup `/tmp/eu_overwritten/00424ce0-Cursor_RunHandler.hpp`
* `00464800-HumanView_UpdateView.cpp` (sha256 before: `aa004d0f1abc711d...`) -> backup `/tmp/eu_overwritten/00464800-HumanView_UpdateView.cpp`
* `00464800-HumanView_UpdateView.hpp` (sha256 before: `97a4f902fc174106...`) -> backup `/tmp/eu_overwritten/00464800-HumanView_UpdateView.hpp`
* `0048f0a0-AppContext_GethWnd.cpp` (sha256 before: `c7964612d35c94fd...`) -> backup `/tmp/eu_overwritten/0048f0a0-AppContext_GethWnd.cpp`
* `0048f0a0-AppContext_GethWnd.hpp` (sha256 before: `056362ffaa07d131...`) -> backup `/tmp/eu_overwritten/0048f0a0-AppContext_GethWnd.hpp`
* `0048f1a0-AppContext_SetDebugged.cpp` (sha256 before: `2c038832695dea6c...`) -> backup `/tmp/eu_overwritten/0048f1a0-AppContext_SetDebugged.cpp`
* `0048f1e0-AppContext_SetDebugtextState.cpp` (sha256 before: `90b6ef91929af705...`) -> backup `/tmp/eu_overwritten/0048f1e0-AppContext_SetDebugtextState.cpp`
* `0048f260-AppContext_SetTerrainLightmapsUsed.cpp` (sha256 before: `7bfed79736facca3...`) -> backup `/tmp/eu_overwritten/0048f260-AppContext_SetTerrainLightmapsUsed.cpp`
* `0048f360-AppMain_ParseCmdLineArgs.cpp` (sha256 before: `bc4ed4f23649db7c...`) -> backup `/tmp/eu_overwritten/0048f360-AppMain_ParseCmdLineArgs.cpp`
* `0048f360-AppMain_ParseCmdLineArgs.hpp` (sha256 before: `c4a0cb1e2662fe7c...`) -> backup `/tmp/eu_overwritten/0048f360-AppMain_ParseCmdLineArgs.hpp`
* `0048fc20-Mouse_Update.cpp` (sha256 before: `66cc66a0eae2b13e...`) -> backup `/tmp/eu_overwritten/0048fc20-Mouse_Update.cpp`
* `0048fc20-Mouse_Update.hpp` (sha256 before: `4668d515c8931140...`) -> backup `/tmp/eu_overwritten/0048fc20-Mouse_Update.hpp`
* `00490230-Keyboard_Update.hpp` (sha256 before: `25c69d5e84d2e862...`) -> backup `/tmp/eu_overwritten/00490230-Keyboard_Update.hpp`
* `00497e70-Transcontext_SetActiveTransContext.cpp` (sha256 before: `89c0d47ef196f66c...`) -> backup `/tmp/eu_overwritten/00497e70-Transcontext_SetActiveTransContext.cpp`
* `004b0c60-Mem_AllocFn.hpp` (sha256 before: `bca5c0c38a38698a...`) -> backup `/tmp/eu_overwritten/004b0c60-Mem_AllocFn.hpp`
* `004b0d10-Mem_DeAllocFn.hpp` (sha256 before: `efce889e3dcc3fed...`) -> backup `/tmp/eu_overwritten/004b0d10-Mem_DeAllocFn.hpp`
* `004b38e0-Real32x9_Create.hpp` (sha256 before: `a90c761f6c6d9eeb...`) -> backup `/tmp/eu_overwritten/004b38e0-Real32x9_Create.hpp`
* `004b6e70-Picture_GetWidth.cpp` (sha256 before: `7ca61b2e0d045b02...`) -> backup `/tmp/eu_overwritten/004b6e70-Picture_GetWidth.cpp`
* `004b6e70-Picture_GetWidth.hpp` (sha256 before: `28aa60edc6ebf662...`) -> backup `/tmp/eu_overwritten/004b6e70-Picture_GetWidth.hpp`
* `004b6e80-Picture_GetHeight.cpp` (sha256 before: `1deed6151e0d4d8a...`) -> backup `/tmp/eu_overwritten/004b6e80-Picture_GetHeight.cpp`
* `004b6e80-Picture_GetHeight.hpp` (sha256 before: `2d75b82ecfe62f19...`) -> backup `/tmp/eu_overwritten/004b6e80-Picture_GetHeight.hpp`
* `004b8a20-Script_EvalArgInt.hpp` (sha256 before: `7065cb544b05d75e...`) -> backup `/tmp/eu_overwritten/004b8a20-Script_EvalArgInt.hpp`
* `004c1790-QTaskList_Register.cpp` (sha256 before: `efdf88adad388625...`) -> backup `/tmp/eu_overwritten/004c1790-QTaskList_Register.cpp`
* `004c1790-QTaskList_Register.hpp` (sha256 before: `f8f3ddea3bb3b06a...`) -> backup `/tmp/eu_overwritten/004c1790-QTaskList_Register.hpp`
* `004c17c0-QTaskList_Remove.cpp` (sha256 before: `5c6b7507c781ffa7...`) -> backup `/tmp/eu_overwritten/004c17c0-QTaskList_Remove.cpp`
* `004c17c0-QTaskList_Remove.hpp` (sha256 before: `a3e2d0d8f087b26d...`) -> backup `/tmp/eu_overwritten/004c17c0-QTaskList_Remove.hpp`
* `004c1800-QTaskList_New.cpp` (sha256 before: `4cd481c5828292d4...`) -> backup `/tmp/eu_overwritten/004c1800-QTaskList_New.cpp`
* `004c1800-QTaskList_New.hpp` (sha256 before: `7f5122e298f20072...`) -> backup `/tmp/eu_overwritten/004c1800-QTaskList_New.hpp`
* `004c1830-QTaskList_Delete.cpp` (sha256 before: `f03614eadfdccc60...`) -> backup `/tmp/eu_overwritten/004c1830-QTaskList_Delete.cpp`
* `004c1830-QTaskList_Delete.hpp` (sha256 before: `d5a4a4b422defbb3...`) -> backup `/tmp/eu_overwritten/004c1830-QTaskList_Delete.hpp`
* `004d3210-AnimController_UpdateChannels.cpp` (sha256 before: `30ec9f6c5c662a86...`) -> backup `/tmp/eu_overwritten/004d3210-AnimController_UpdateChannels.cpp`
* `004d9610-BoneDynCubeObj_GetQTaskType.cpp` (sha256 before: `6de18b1ebb2efeea...`) -> backup `/tmp/eu_overwritten/004d9610-BoneDynCubeObj_GetQTaskType.cpp`
