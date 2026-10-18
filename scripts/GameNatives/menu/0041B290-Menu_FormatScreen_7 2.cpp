// IGI native provenance: Menu_FormatScreen_7 - 0x0041B290 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0041B290-Menu_FormatScreen_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_FormatScreen_7.h"
int Menu_FormatScreen_7(int context, int arg2)

{
    void* temp_2 = g_57bba0;
    int result = *(temp_2 + 0x21a0);
    void* flagP = temp_2 + result * 0x108 + 0x9c;
    int entity_3 = 0;

    if (result > 0)
    {
        int* temp_val = temp_2 + 0x19c;

        do
        {
            if (*temp_val == arg2)
                return result;

            entity_3 += 1;
            temp_val = &temp_val[0x42];
        } while (entity_3 < result);
    }

    int dest_1 = context;
    int i = 0xffffffff;
    *(flagP + 0x100) = arg2;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_1;
        dest_1 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_4 = ~i;
    int source_2;
    int dest_4;
    dest_4 = memcpy(flagP, dest_1 - entity_4, entity_4 & 0xfffffffc);
    memcpy(dest_4, source_2, entity_4 & 3);
    *(temp_2 + 0x21a0) += 1;
    return 0;
}
