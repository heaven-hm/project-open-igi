// IGI native provenance: Display_NoMoreDisplayCallbac - 0x00491650 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491650-Display_NoMoreDisplayCallbac.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_NoMoreDisplayCallbac.h"
int* Display_NoMoreDisplayCallbac(int context, int* arg2)

{
    int entity_val = 0;

    for (int* i = &g_c28a44; i < &g_c28b44; )
    {
        if (!*i)
        {
            *((entity_val << 3) + &g_c28a40) = *arg2;
            *((entity_val << 3) + &g_c28a44) = context;
            *arg2 = (entity_val << 3) + &g_c28a40;
            return (entity_val << 3) + &g_c28a40;
        }

        i = &i[2];
        entity_val += 1;
    }

    Error_Show("No more Display callbacks available.");

    while (true)
        /* nop */
}
