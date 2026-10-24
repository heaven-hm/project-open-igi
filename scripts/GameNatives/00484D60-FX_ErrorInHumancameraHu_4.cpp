// IGI native provenance: FX_ErrorInHumancameraHu_4 - 0x00484D60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00484D60-FX_ErrorInHumancameraHu_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00484D60-FX_ErrorInHumancameraHu_4.hpp"
void* FX_ErrorInHumancameraHu_4(void* context, int arg2)

{
    int context_val;
    int item_4 = context_val;
    int entity_val = 0;

    for (int* i = &g_5c1188; i < 0x5c1588; )
    {
        if (*(i + 5) && **i == arg2)
        {
            *(context + 0x68) = *((entity_val << 3) + &g_5c1188);
            *(context + 0x120) = *((entity_val << 3) + &g_5c1188);
            *(context + 0x118) = entity_val;
            *(context + 0x11c) = entity_val;
            return context;
        }

        i = &i[2];
        entity_val += 1;
    }

    Error_Show("ERROR IN HUMANCAMERA HUMANLIST");

    while (true)
        /* nop */
}
