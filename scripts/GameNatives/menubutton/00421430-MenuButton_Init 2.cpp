// IGI native provenance: MenuButton_Init - 0x00421430 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00421430-MenuButton_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuButton_Init.h"
int* MenuButton_Init()

{
    int item_9c = 0;
    short data_1 = TaskType_Register("MenuButton", 0x98, Menu_PlayMusic_13(), 0);
    g_53b7f0 = data_1;
    QTask_TriggerEvent(data_1, 1, sub_4215d0);
    int data_2 = StatusScreen_GetEvent();
    int item_a0 = data_2;
    data_2 = g_53b7f0;
    QTask_TriggerEvent(data_2, item_a0, MenuButton_LoadParameters);
    QTask_TriggerEvent(g_53b7f0, 2, AIGraph_LoadParameters);
    QTask_RemoveEventCallback(g_53b7f0, 0, sub_421510);
    int data_3 = Script_ParseEnumInt32_8();
    int item_a0_1 = data_3;
    data_3 = g_53b7f0;
    QTask_RemoveEventCallback(data_3, item_a0_1, sub_4216f0);
    int data_4 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_53b7f0, data_4, MenuButton_Process);
    g_57bbfc = Script_EvaluateCondition_7(1);
    int data_6 = Script_FormatString_3();
    void item_98;
    void item_44;
    return Script_UndefinedParameterTy(g_57bbfc, &item_98, &item_44, data_6, 7, 0);
}
