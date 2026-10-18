// IGI native provenance: Light_UpdateGlobalLight_9 - 0x0052CFA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052CFA0-Light_UpdateGlobalLight_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_UpdateGlobalLight_9.hpp"
short* Light_UpdateGlobalLight_9(void* context)

{
    short temp_val = 0x13;
    short* result = context + 0x54;

    for (int i = 0; i < 0x14; i += 1)
    {
        *result = 0;
        result[1] = temp_val + 1;
        result[2] = i + 1;
        result = &result[3];
        temp_val = i;
    }

    for (int i_1 = 0; i_1 < 0xf; i_1 += 1)
    {
        int j = 0;
        short source_1 = 0x13;
        short temp_4 = i_1 * 0x14;

        do
        {
            *result = j + temp_4 + 1;
            result[1] = source_1 + temp_4 + 1;
            result[2] = j + temp_4 + 0x15;
            result[3] = source_1 + temp_4 + 1;
            result[4] = source_1 + temp_4 + 0x15;
            source_1 = j;
            result[5] = j + temp_4 + 0x15;
            result = &result[6];
            j += 1;
        } while (j < 0x14);
    }

    return result;
}
