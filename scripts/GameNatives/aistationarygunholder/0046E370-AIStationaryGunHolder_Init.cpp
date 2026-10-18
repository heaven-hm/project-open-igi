// IGI native provenance: AIStationaryGunHolder_Init - 0x0046E370 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046E370-AIStationaryGunHolder_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0046E370-AIStationaryGunHolder_Init.hpp"
❓  int* AIStationaryGunHolder_Init()
// This function has unresolved stack usage. View graph of stack usage to resolve.

{
    Camera_UpdateTransform_5(0x26a0);
    int item_4 = 0;
    short data_1 = TaskType_Register("AIStationaryGunHolder", 0x26a0, DynObj_LookupHashItem_5(), 0);
    g_5be318 = data_1;
    QTask_RemoveEventCallback(data_1, 0, sub_46e210);
    QTask_TriggerEvent(g_5be318, 1, AIStationaryGunHolder_LoadParameters);
    QTask_TriggerEvent(g_5be318, 2, AIStationaryGunHolder_Setup);
    int data_3 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_5be318, data_3, AIStationaryGunHolder_Process);
    int data_4 = Script_ParseEnumInt32_8();
    int item_8_1 = data_4;
    data_4 = g_5be318;
    QTask_RemoveEventCallback(data_4, item_8_1, sub_46e9d0);
    int data_5 = FX_FireBallUpdate_13();
    QTask_RemoveEventCallback(g_5be318, data_5, sub_46e9e0);
    int data_6 = FX_TrailUpdate_12();
    QTask_RemoveEventCallback(g_5be318, data_6, sub_42fc50);
    int data_7 = AI_CheckHearing_25();
    int item_8_4 = data_7;
    data_7 = g_5be318;
    QTask_RemoveEventCallback(data_7, item_8_4, sub_46e9f0);
    g_5be320 = 0;
    g_5be324 = 0x42c80000;
    g_5be328 = Script_EvaluateCondition_7(8);
    int data_9 = Script_ParseEnumReal32_4();
    void arg_2598;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_2598, data_9, 7, 0);
    int data_11 = Script_SpawnNewTask_3();
    void arg_25b0;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25b0, data_11, 7, 0);
    int data_13 = Script_EvaluateCondition_3();
    void arg_25d8;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25d8, data_13, 7, 0);
    int data_15 = Script_ExecuteLevelScript_2();
    void arg_25e8;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25e8, data_15, 7, 0);
    int data_17 = Script_ExecuteLevelScript_2();
    void arg_25ec;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25ec, data_17, 7, 0);
    int data_19 = Script_EvaluateCondition_2();
    void arg_25f0;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25f0, data_19, 7, &g_5be320);
    int data_21 = Script_CallNativeHandler_3();
    void arg_25f4;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_25f4, data_21, 7, 0);
    int data_23 = Script_CallNativeHandler_3();
    void arg_2644;
    Script_UndefinedParameterTy(g_5be328, &return_addr, &arg_2644, data_23, 7, 0);
    int* result = Mem_Alloc8(0x10);
    g_5be330 = result;
    return result;
}
