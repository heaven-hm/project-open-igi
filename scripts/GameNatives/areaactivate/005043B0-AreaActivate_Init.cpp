// IGI native provenance: AreaActivate_Init - 0x005043B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005043B0-AreaActivate_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AreaActivate_Init.hpp"
int* AreaActivate_Init()

{
    int item_bc = 0;
    g_54e69c = TaskType_Register("AreaActivate", 0xb8, EditorRigidObj_OnEvent(), 0);
    g_54e69e = QTask_UnableToAllocateEvent(0);
    QTask_RemoveEventCallback(g_54e69c, 0, sub_5049d0);
    QTask_TriggerEvent(g_54e69c, 2, AreaActivate_LoadParameters);
    int data_3 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_54e69c, data_3, AreaActivate_Setup);
    int data_4 = Script_ParseEnumInt32_8();
    int item_c0_1 = data_4;
    data_4 = g_54e69c;
    QTask_RemoveEventCallback(data_4, item_c0_1, sub_504890);
    QTask_TriggerEvent(g_54e69c, 1, AreaActivate_Process);
    int data_5 = Script_FormatString_5();
    QTask_TriggerEvent(g_54e69c, data_5, AreaActivate_Update);
    int data_6 = Script_SpawnNewTask_6();
    int item_c0_3 = data_6;
    data_6 = g_54e69c;
    QTask_TriggerEvent(data_6, item_c0_3, AreaActivate_OnEvent);
    g_a7a7b4 = Script_EvaluateCondition_7(4);
    int data_8 = Script_ParseEnumReal32_4();
    void item_b8;
    void item_68;
    Script_UndefinedParameterTy(g_a7a7b4, &item_b8, &item_68, data_8, 7, 0);
    int data_10 = Script_SpawnNewTask_3();
    void item_50;
    Script_UndefinedParameterTy(g_a7a7b4, &item_b8, &item_50, data_10, 7, 0);
    int data_12 = Script_FormatString_2();
    void item_1c;
    Script_UndefinedParameterTy(g_a7a7b4, &item_b8, &item_1c, data_12, 7, 0);
    int data_14 = Script_CallNativeHandler_3();
    void item_8;
    return Script_UndefinedParameterTy(g_a7a7b4, &item_b8, &item_8, data_14, 7, 0);
}
