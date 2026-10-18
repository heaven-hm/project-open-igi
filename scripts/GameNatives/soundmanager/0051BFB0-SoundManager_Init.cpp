// IGI native provenance: SoundManager_Init - 0x0051BFB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051BFB0-SoundManager_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundManager_Init.hpp"
int* SoundManager_Init()

{
    int item_48 = 0;
    short data_1 = TaskType_Register("SoundManager", 0x44, QTask_OpenEvent_2(), 0);
    g_54ee3c = data_1;
    QTask_RemoveEventCallback(data_1, 0, sub_51c090);
    QTask_TriggerEvent(g_54ee3c, 1, sub_488700);
    QTask_TriggerEvent(g_54ee3c, 2, sub_51c0a0);
    int data_3 = SoundSys_SetFrequency_5();
    QTask_RemoveEventCallback(g_54ee3c, data_3, sub_51c0c0);
    int data_4 = Script_ParseEnumReal32_8();
    int item_4c_1 = data_4;
    data_4 = g_54ee3c;
    QTask_RemoveEventCallback(data_4, item_4c_1, SoundManager_LoadParameters);
    int data_5 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_54ee3c, data_5, sub_51c280);
    g_a83780 = Script_EvaluateCondition_7(1);
    int data_7 = Script_FormatString_3();
    void item_44;
    void item_24;
    return Script_UndefinedParameterTy(g_a83780, &item_44, &item_24, data_7, 7, 0);
}
