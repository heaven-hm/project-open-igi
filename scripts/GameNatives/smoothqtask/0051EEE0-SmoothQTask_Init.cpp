// IGI native provenance: SmoothQTask_Init - 0x0051EEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051EEE0-SmoothQTask_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SmoothQTask_Init.hpp"
void* SmoothQTask_Init()

{
    int item_104 = 0;
    short data_1 = TaskType_Register("SmoothQTask", 0x100, EditorRigidObj_OnEvent(), 0);
    g_a837a0 = data_1;
    QTask_RemoveEventCallback(data_1, 0, SmoothQTask_LoadParameters);
    QTask_TriggerEvent(g_a837a0, 1, SmoothQTask_Setup);
    QTask_TriggerEvent(g_a837a0, 2, SmoothQTask_Process);
    int data_3 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_a837a0, data_3, sub_51f330);
    int data_4 = Script_ParseEnumReal32_8();
    int item_108_1 = data_4;
    data_4 = g_a837a0;
    QTask_RemoveEventCallback(data_4, item_108_1, sub_51f4b0);
    int data_5 = Script_FormatString_5();
    QTask_TriggerEvent(g_a837a0, data_5, SmoothQTask_Update);
    int data_6 = Script_SpawnNewTask_6();
    QTask_TriggerEvent(g_a837a0, data_6, SmoothQTask_OnEvent);
    g_a837a4 = Script_EvaluateCondition_7(0xa);
    int data_8 = Script_ParseEnumReal32_4();
    void item_100;
    void item_e0;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_e0, data_8, 7, 0);
    int data_9 = Script_SpawnNewTask_3();
    void item_c8;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_c8, data_9, 7, 0);
    int data_10 = Script_CallNativeHandler_3();
    void item_58;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_58, data_10, 7, 0);
    int data_11 = Script_ParseEnumInt32_3();
    void item_90;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_90, data_11, 7, 0);
    int data_12 = Script_ParseEnumInt32_3();
    void item_88;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_88, data_12, 7, 0);
    int data_13 = Script_ParseEnumInt32_3();
    void item_80;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_80, data_13, 7, 0);
    int data_14 = Script_ParseEnumInt32_3();
    void item_78;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_78, data_14, 7, 0);
    int data_15 = Script_ParseEnumInt32_3();
    void item_70;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_70, data_15, 7, 0);
    int data_16 = Script_ParseEnumInt32_3();
    void item_68;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_68, data_16, 7, 0);
    int data_17 = Script_ParseEnumInt32_3();
    void item_60;
    Script_UndefinedParameterTy(g_a837a4, &item_100, &item_60, data_17, 7, 0);
    return TaskType_Set("TASKTYPE_SMOOTHQTASK", *g_a837a0);
}
