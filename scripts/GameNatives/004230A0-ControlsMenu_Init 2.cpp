// IGI native provenance: ControlsMenu_Init - 0x004230A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004230A0-ControlsMenu_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ControlsMenu_Init.h"
void* ControlsMenu_Init()

{
    int item_4 = 0;
    short data_1 = TaskType_Register("ControlsMenu", 0x364, Menu_PlayMusic_13(), 0);
    g_53b874 = data_1;
    QTask_TriggerEvent(data_1, 1, ControlsMenu_LoadParameters);
    int data_2 = StatusScreen_GetEvent();
    int item_8 = data_2;
    data_2 = g_53b874;
    QTask_TriggerEvent(data_2, item_8, ControlsMenu_Setup);
    QTask_TriggerEvent(g_53b874, 2, sub_423350);
    QTask_RemoveEventCallback(g_53b874, 0, ControlsMenu_Process);
    int data_3 = Script_ParseEnumInt32_8();
    int item_8_1 = data_3;
    data_3 = g_53b874;
    QTask_RemoveEventCallback(data_3, item_8_1, sub_423ca0);
    int data_4 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_53b874, data_4, ControlsMenu_Update);
    int data_5 = Script_FormatString_5();
    QTask_TriggerEvent(g_53b874, data_5, ControlsMenu_OnEvent);
    int data_6 = Script_SpawnNewTask_6();
    int item_8_4 = data_6;
    data_6 = g_53b874;
    QTask_TriggerEvent(data_6, item_8_4, ControlsMenu_Action);
    int data_7 = Menu_HandleInput_13();
    QTask_TriggerEvent(g_53b874, data_7, ControlsMenu_Cleanup);
    int data_8 = Menu_RequestScreen_14();
    QTask_TriggerEvent(g_53b874, data_8, sub_423eb0);
    Game_DefineOptions("ControlsMenu_ConvertFloatFromOffset", ControlsMenu_ConvertFloatFromOffset,
        0, 1);
    return Game_DefineOptions("ControlsMenu_ConvertOffsetFromFloat",
        ControlsMenu_ConvertOffsetFromFloat, 0, 1);
}
