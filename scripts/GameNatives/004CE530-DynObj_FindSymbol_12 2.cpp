// IGI native provenance: DynObj_FindSymbol_12 - 0x004CE530 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE530-DynObj_FindSymbol_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_FindSymbol_12.h"
short* DynObj_FindSymbol_12(int context, void* arg2)

{
    int* source_val = arg2 + 0xc;
    int i_2 = 8;
    short* result;
    int i;

    do
    {
        result = *source_val;

        if (result)
            result = DynObj_FindSymbol_12(context, result);

        source_val = &source_val[1];
        i = i_2;
        i_2 -= 1;
    } while (i != 1);

    for (void* i_1 = *(arg2 + 0x2c); i_1; i_1 = *(i_1 + 0x58))
    {
        int entity_1 = 0;
        int source_1 = 0;
        int j_1 = 0xfe;
        int j;

        do
        {
            short context_1 = 0;
            result = context + ((entity_1 + *(arg2 + 0x34) * 0xfe) << 1) + 0x118;
            int temp_5 = 0;
            temp_5 = *(i_1 + 0x1c);
            context_1 = *(((temp_5 + source_1) << 2) + &g_a96ae0);
            *result += context_1;
            entity_1 += 1;
            source_1 += 0x180;
            j = j_1;
            j_1 -= 1;
        } while (j != 1);
    }

    return result;
}
