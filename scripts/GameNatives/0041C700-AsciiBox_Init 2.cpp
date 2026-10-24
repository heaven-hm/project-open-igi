// IGI native provenance: AsciiBox_Init - 0x0041C700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041C700-AsciiBox_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AsciiBox_Init.h"
int* AsciiBox_Init()

{
    int item_fc = 0;
    short data_1 = TaskType_Register("AsciiBox", 0xf8, Menu_PlayMusic_13(), 0);
    g_53b5f4 = data_1;
    QTask_TriggerEvent(data_1, 1, AsciiBox_LoadParameters);
    int data_2 = StatusScreen_GetEvent();
    int item_100 = data_2;
    data_2 = g_53b5f4;
    QTask_TriggerEvent(data_2, item_100, AsciiBox_Setup);
    QTask_TriggerEvent(g_53b5f4, 2, AsciiBox_Process);
    QTask_RemoveEventCallback(g_53b5f4, 0, sub_41c8d0);
    int data_3 = Script_ParseEnumInt32_8();
    int item_100_1 = data_3;
    data_3 = g_53b5f4;
    QTask_RemoveEventCallback(data_3, item_100_1, sub_41d000);
    int data_4 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_53b5f4, data_4, sub_41cc40);
    int data_5 = Menu_HandleInput_13();
    QTask_TriggerEvent(g_53b5f4, data_5, sub_424300);
    int data_6 = Menu_RequestScreen_14();
    int item_100_4 = data_6;
    data_6 = g_53b5f4;
    QTask_TriggerEvent(data_6, item_100_4, sub_41a120);
    int data_7 = Script_FormatString_5();
    QTask_TriggerEvent(g_53b5f4, data_7, sub_41d010);
    int data_8 = Script_SpawnNewTask_6();
    QTask_TriggerEvent(g_53b5f4, data_8, sub_41d030);
    int data_9 = Menu_LeaveMenus_14();
    int item_100_7 = data_9;
    data_9 = g_53b5f4;
    QTask_TriggerEvent(data_9, item_100_7, AsciiBox_Update);
    g_57bbbc = Script_EvaluateCondition_7(4);
    int data_11 = Script_FormatString_3();
    void item_f8;
    void item_a4;
    Script_UndefinedParameterTy(g_57bbbc, &item_f8, &item_a4, data_11, 7, 0);
    int data_13 = Script_FormatString_3();
    void item_64;
    Script_UndefinedParameterTy(g_57bbbc, &item_f8, &item_64, data_13, 7, 0);
    int data_15 = Script_ParseEnumString_3();
    void item_d0;
    Script_UndefinedParameterTy(g_57bbbc, &item_f8, &item_d0, data_15, 7, 0);
    int data_17 = Script_ParseEnumString_3();
    void item_cc;
    return Script_UndefinedParameterTy(g_57bbbc, &item_f8, &item_cc, data_17, 7, 0);
}
