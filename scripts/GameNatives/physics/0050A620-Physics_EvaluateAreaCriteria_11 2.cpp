// IGI native provenance: Physics_EvaluateAreaCriteria_11 - 0x0050A620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050A620-Physics_EvaluateAreaCriteria_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_EvaluateAreaCriteria_11.h"
long double Physics_EvaluateAreaCriteria_11(int* context, int arg2, int arg3)

{
    int i_1 = arg2 * arg3;
    int entity_2 = i_1 * 0xff;
    arg2 = entity_2;

    if (i_1 > 0)
    {
        int* temp_1 = context;
        int i;

        do
        {
            int source_1 = *temp_1;
            temp_1 = &temp_1[1];
            entity_2 -= source_1 >> 0x18;
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
    }

    int item_4 = 0;
    return entity_2 / arg2;
}
