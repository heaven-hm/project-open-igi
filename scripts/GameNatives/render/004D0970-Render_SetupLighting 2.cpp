// IGI native provenance: Render_SetupLighting - 0x004D0970 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0970-Render_SetupLighting.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_SetupLighting.h"
void** Render_SetupLighting(void** context, void* arg2, int arg3)

{
    void** result = *(arg2 + 0x6c);

    if (result)
    {
        result = *result;

        if (result)
        {
            int* temp_3 = *(*(result[3] + 0xc) + 0x88) + (arg3 << 4);
            result = context;
            *result = *temp_3;
            result[1] = temp_3[1];
            result[2] = temp_3[2];
            *result = *(arg2 + 0x98) * *result;
            result[1] = *(arg2 + 0x9c) * result[1];
            result[2] = *(arg2 + 0xa0) * result[2];
        }
    }

    return result;
}
