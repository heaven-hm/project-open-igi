// IGI native provenance: QObj_Init - 0x004D9730 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D9730-QObj_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QObj_Init.h"
int QObj_Init()

{
    QTask_CleanupEvents(0, 4);
    QTask_CleanupEvents(0, 5);
    QTask_CleanupEvents(0, 6);
    QTask_CleanupEvents(0, 7);
    g_54863a = QTask_UnableToAllocateEvent(0);
    int item_4 = 0;
    short result = TaskType_Register("QObj", 0x50, QTask_OpenEvent_2(), 0);
    g_548638 = result;
    return result;
}
