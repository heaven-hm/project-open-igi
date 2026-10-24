// IGI native provenance: DynObj_UpdateDynTree - 0x004C0260 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0260-DynObj_UpdateDynTree.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C0260-DynObj_UpdateDynTree.hpp"
int DynObj_UpdateDynTree(int* context, int arg2, int arg3)

{
    int entity_val;
    int item_4 = entity_val;
    int* entity_1 = context;
    int flagP = 0;
    int i = entity_1[1];
    int i_1 = 0;

    if (i > 0)
    {
        do
        {
            int* j_1 = *(*entity_1 + flagP);
            int* j = *j_1;

            if (j)
            {
                do
                {
                    arg2(j_1[2], j_1[3], arg3);
                    j_1 = j;
                    j = *j;
                } while (j);

                entity_1 = context;
            }

            i = i_1 + 1;
            flagP += 0xc;
            i_1 = i;
        } while (i < entity_1[1]);
    }

    return i;
}
