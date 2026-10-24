// IGI native provenance: Medipack_Init - 0x00470410 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00470410-Medipack_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Medipack_Init.h"
void* Medipack_Init()

{
    int item_4 = 0;
    short data_1 = TaskType_Register("Medipack", 0x1b0, Weapon_ComputeAccuracy_5(), 0);
    g_5be334 = data_1;
    QTask_RemoveEventCallback(data_1, 0, sub_470500);
    int data_2 = Script_ParseEnumString_11();
    int item_8 = data_2;
    data_2 = g_5be334;
    QTask_RemoveEventCallback(data_2, item_8, Binocular_Process_2);
    int data_3 = AI_AlertAllies_17();
    QTask_RemoveEventCallback(g_5be334, data_3, AIGraph_LoadParameters);
    int data_4 = AI_SelectTarget_17();
    QTask_RemoveEventCallback(g_5be334, data_4, AIGraph_LoadParameters);
    int data_5 = HumanPlayer_ApplyPhysics_10();
    int item_8_3 = data_5;
    data_5 = g_5be334;
    QTask_RemoveEventCallback(data_5, item_8_3, sub_470540);
    int data_6 = HumanPlayer_HandleInput_10();
    QTask_RemoveEventCallback(g_5be334, data_6, sub_427d70);
    int data_7 = AIConfig_LoadSettings();
    QTask_RemoveEventCallback(g_5be334, data_7, Medipack_Update);
    return TaskType_Set("TASKTYPE_MEDIPACK", *g_5be334);
}
