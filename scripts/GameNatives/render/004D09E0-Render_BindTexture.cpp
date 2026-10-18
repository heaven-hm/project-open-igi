// IGI native provenance: Render_BindTexture - 0x004D09E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D09E0-Render_BindTexture.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D09E0-Render_BindTexture.hpp"
void** Render_BindTexture(void** context, void* arg2, int arg3, char* arg4)

{
    void** result = DynObj_Missing(arg4);

    if (result)
    {
        result = *result;

        if (result)
        {
            int* entity_3 = *(*(result[3] + 0xc) + 0x88) + (arg3 << 4);
            result = context;
            *result = *entity_3;
            result[1] = entity_3[1];
            result[2] = entity_3[2];
            *result = *(arg2 + 0x98) * *result;
            result[1] = *(arg2 + 0x9c) * result[1];
            result[2] = *(arg2 + 0xa0) * result[2];
        }
    }

    return result;
}
