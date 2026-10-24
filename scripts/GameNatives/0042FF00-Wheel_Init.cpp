// IGI native provenance: Wheel_Init - 0x0042FF00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042FF00-Wheel_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0042FF00-Wheel_Init.hpp"
void* Wheel_Init()

{
    int item_4 = 0;
    short data_1 = TaskType_Register("Wheel", 0x158, MagicObj_GetTypeId(), 0);
    g_57bd30 = data_1;
    QTask_RemoveEventCallback(data_1, 0, Wheel_LoadParameters);
    QTask_TriggerEvent(g_57bd30, 1, Wheel_Tick);
    QTask_TriggerEvent(g_57bd30, 2, Wheel_Process);
    int data_3 = MagicObj_GetEventId();
    QTask_RemoveEventCallback(g_57bd30, data_3, Wheel_Update);
    int data_4 = Level_LoadLevel_3();
    int item_8_1 = data_4;
    data_4 = g_57bd30;
    QTask_RemoveEventCallback(data_4, item_8_1, AIGraph_LoadParameters);
    int data_5 = Level_InitTextures_3();
    QTask_RemoveEventCallback(g_57bd30, data_5, AIGraph_LoadParameters);
    int data_6 = SoundSys_SetFrequency_18();
    QTask_RemoveEventCallback(g_57bd30, data_6, PhysicsObj_Setup);
    int data_7 = FX_TrailUpdate_12();
    int item_8_4 = data_7;
    data_7 = g_57bd30;
    QTask_RemoveEventCallback(data_7, item_8_4, sub_426d30);
    return TaskType_Set("TASKTYPE_WHEEL", *g_57bd30);
}
