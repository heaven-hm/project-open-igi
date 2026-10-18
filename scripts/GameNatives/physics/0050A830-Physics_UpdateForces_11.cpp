// IGI native provenance: Physics_UpdateForces_11 - 0x0050A830 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050A830-Physics_UpdateForces_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_UpdateForces_11.hpp"
int* Physics_UpdateForces_11(int* context)

{
    int data_val = g_bcac90;
    int item_c = data_val;

    if (data_val >= 0x100)
        item_c = 0x100;

    int source_val = *context;
    int dest_val = context[1];

    if ((~(source_val - 1) & source_val) != source_val)
    {
        int eflags_1;
        char data_5;
        data_5 = __bsr_gprv_memv(source_val);
        source_val = 1 << ((data_5 + 1) & 0x1f);
    }

    if ((~(dest_val - 1) & dest_val) != dest_val)
    {
        int eflags_2;
        char data_6;
        data_6 = __bsr_gprv_memv(dest_val);
        dest_val = 1 << ((data_6 + 1) & 0x1f);
    }

    int data_8 = g_bcac7c;

    if (data_8 <= 1)
        data_8 = 1;

    int entity_4 = g_bcac80;
    int temp_3 = g_bcac84;

    if (entity_4 <= 1)
        entity_4 = 1;

    int context_val = g_bcac88;

    if (source_val < data_8)
        source_val = data_8;
    else if (source_val > temp_3)
        source_val = temp_3;

    if (dest_val < entity_4)
        dest_val = entity_4;
    else if (dest_val > context_val)
        dest_val = context_val;

    if (source_val <= dest_val)
    {
        unsigned int f0_2 = COMBINE(0, dest_val) / source_val;

        if (f0_2 > item_c && item_c)
            source_val = COMBINE(0, f0_2 * source_val) / item_c;
    }
    else
    {
        unsigned int f0 = COMBINE(0, source_val) / dest_val;

        if (f0 > item_c && item_c)
        {
            context[3] = COMBINE(0, f0 * dest_val) / item_c;
            context[2] = source_val;
            return context;
        }
    }

    context[3] = dest_val;
    context[2] = source_val;
    return context;
}
