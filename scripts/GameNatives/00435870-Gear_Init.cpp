// IGI native provenance: Gear_Init - 0x00435870 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00435870-Gear_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00435870-Gear_Init.hpp"
void* Gear_Init()

{
    int item_4 = 0;
    short data_1 = TaskType_Register("Gear", 0x140, MagicObj_GetTypeId(), 0);
    g_57bd64 = data_1;
    QTask_RemoveEventCallback(data_1, 0, sub_435460);
    QTask_TriggerEvent(g_57bd64, 1, sub_435930);
    QTask_TriggerEvent(g_57bd64, 2, sub_51b940);
    int data_3 = MagicObj_GetEventId();
    QTask_RemoveEventCallback(g_57bd64, data_3, Gear_LoadParameters);
    int data_4 = DynObj_RemoveSymbol_5();
    int item_8_1 = data_4;
    data_4 = g_57bd64;
    QTask_RemoveEventCallback(data_4, item_8_1, Gear_Setup);
    int data_5 = DynObj_RegisterSymbol_6();
    QTask_RemoveEventCallback(g_57bd64, data_5, sub_4eff20);
    return TaskType_Set("TASKTYPE_GEAR", *g_57bd64);
}
