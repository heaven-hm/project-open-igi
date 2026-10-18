// IGI native provenance: AI_UpdateSensors_4 - 0x00453270 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453270-AI_UpdateSensors_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_4.hpp"
void AI_UpdateSensors_4()

{
    int* i_1 = g_58aa18;

    for (int* i = *i_1; i; i = *i)
    {
        int* j_1 = i_1[3];

        for (int* j = *j_1; j; j = *j)
        {
            HeapList_Remove(j_1);
            HeapList_Insert(&g_57e1b8, j_1);
            j_1 = j;
        }

        HeapList_Remove(i_1);
        HeapList_Insert(&g_57c748, i_1);
        i_1 = i;
    }
}
