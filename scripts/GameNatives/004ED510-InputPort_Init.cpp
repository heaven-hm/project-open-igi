// IGI native provenance: InputPort_Init - 0x004ED510 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ED510-InputPort_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ED510-InputPort_Init.hpp"
int InputPort_Init()

{
    int item_8 = 0;
    short data_1 = TaskType_Register("InputPort", 0x20, QTask_OpenEvent_2(), 0);
    g_a5efa0 = data_1;
    int result = QTask_TriggerEvent(data_1, 1, InputPort_LoadParameters);
    g_bc20a0 = 0;
    g_bc2150 = 0;
    g_bc2200 = 0;
    g_bc22b0 = 0;
    return result;
}
