// IGI native provenance: Env_CheckAmbientArea_13 - 0x0044FF80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044FF80-Env_CheckAmbientArea_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044FF80-Env_CheckAmbientArea_13.hpp"
void* Env_CheckAmbientArea_13(void* context)

{
    void* result = context;
    int* i_1 = *(result + 0xe0);

    for (int* i = *i_1; i; i = *i)
    {
        HeapList_Remove(i_1);
        i_1[1] = 0;
        *i_1 = 0;
        result = HeapList_Insert(&g_58a9c8, i_1);
        i_1 = i;
    }

    return result;
}
