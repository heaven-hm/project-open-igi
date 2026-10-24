// IGI native provenance: Shadow_Init - 0x004E0950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E0950-Shadow_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Shadow_Init.h"
int Shadow_Init()

{
    int item_4 = 0;
    g_a5e36c = TaskType_Register("Shadow", 0x124, QTask_OpenEvent_2(), 0);
    int data_2 = DynObj_FindSymbol_6();
    int item_8 = data_2;
    data_2 = g_a5e36c;
    QTask_RemoveEventCallback(data_2, item_8, Shadow_LoadParameters);
    QTask_TriggerEvent(g_a5e36c, 1, AIGraph_LoadParameters);
    return QTask_TriggerEvent(g_a5e36c, 2, sub_4e09c0);
}
