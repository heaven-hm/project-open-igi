// IGI native provenance: Container_Init - 0x00505C90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00505C90-Container_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Container_Init.h"
int Container_Init()

{
    int item_4 = 0;
    short data_1 = TaskType_Register("Container", 0x20, QTask_OpenEvent_2(), 0);
    g_a7a7bc = data_1;
    QTask_TriggerEvent(data_1, 1, sub_488700);
    int data_2 = DynObj_FindSymbol_6();
    int item_8 = data_2;
    data_2 = g_a7a7bc;
    QTask_RemoveEventCallback(data_2, item_8, sub_4e0890);
    int data_3 = DynObj_CreateMagicObject_6();
    QTask_RemoveEventCallback(g_a7a7bc, data_3, sub_4879e0);
    int data_4 = DynObj_LinkDynamicCube_12();
    QTask_RemoveEventCallback(g_a7a7bc, data_4, sub_487e20);
    int data_5 = DynObj_UpdateDynTree_12();
    int item_8_3 = data_5;
    data_5 = g_a7a7bc;
    return QTask_RemoveEventCallback(data_5, item_8_3, sub_487d90);
}
