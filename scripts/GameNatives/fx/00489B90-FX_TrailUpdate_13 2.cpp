// IGI native provenance: FX_TrailUpdate_13 - 0x00489B90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489B90-FX_TrailUpdate_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_TrailUpdate_13.h"
int* FX_TrailUpdate_13(void* context)

{
    int* i_2 = *(context + 0x18);

    for (int* i = *i_2; i; i = *i)
    {
        Qtask_UpdateList(i_2[2]);
        i_2 = i;
    }

    int* result = *(context + 0x24);

    for (int* i_1 = *result; i_1; i_1 = *i_1)
    {
        Qtask_UpdateList(result[2]);
        result = i_1;
    }

    return result;
}
