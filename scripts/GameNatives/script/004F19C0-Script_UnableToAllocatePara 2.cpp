// IGI native provenance: Script_UnableToAllocatePara - 0x004F19C0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F19C0-Script_UnableToAllocatePara.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_UnableToAllocatePara.h"
int Script_UnableToAllocatePara(int context, int arg2)

{
    int result = 0;
    int* entity_val = &g_a75928;

    while (*entity_val)
    {
        entity_val = &entity_val[0x13];
        result += 1;

        if (entity_val >= &g_a76c28)
        {
            Error_Show("Unable to allocate parameter type");

            while (true)
                /* nop */
        }
    }

    if (arg2 != 0xffffffff)
        memcpy(result * 0x4c + &g_a758e0, arg2 * 0x4c + &g_a758e0, 0x4c);

    int entity_7 = result * 0x4c;
    *(entity_7 + &g_a75928) = 1;
    *(entity_7 + &g_a758e0) = context;
    *(entity_7 + &g_a758e4) = arg2;
    return result;
}
