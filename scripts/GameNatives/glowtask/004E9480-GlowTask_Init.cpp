// IGI native provenance: GlowTask_Init - 0x004E9480 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E9480-GlowTask_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004E9480-GlowTask_Init.hpp"
int GlowTask_Init()

{
    int item_4 = 0;
    g_54da50 = TaskType_Register("GlowTask", 0x20, QTask_OpenEvent_2(), 0);
    int data_2 = StatusScreen_GetEvent();
    int item_8 = data_2;
    data_2 = g_54da50;
    QTask_TriggerEvent(data_2, item_8, j_Terrain_SampleHeightmap_2);
    return QTask_TriggerEvent(g_54da50, 2, AIGraph_LoadParameters);
}
