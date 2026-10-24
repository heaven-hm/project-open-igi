// IGI native provenance: Render_BindTexture_11 - 0x004D74F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D74F0-Render_BindTexture_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_BindTexture_11.h"
int Render_BindTexture_11(int* context, int arg2, int arg3)

{
    if (context)
    {
        int data_1 = *context;

        if (context[2] && arg2 >= 0 && arg2 < *(data_1 * 0xc + &g_a542f0))
        {
            int data_3 = data_1 * 0x28;
            int entity_1 = *(data_3 + 0xa53914);

            if (entity_1 && arg3 < entity_1)
            {
                int result = 0;
                result = *(*(data_3 + &g_a538f4) + (arg3 << 2));
                return result;
            }
        }
    }

    return 0;
}
