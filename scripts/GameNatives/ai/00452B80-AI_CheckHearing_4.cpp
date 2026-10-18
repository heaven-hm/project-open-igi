// IGI native provenance: AI_CheckHearing_4 - 0x00452B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00452B80-AI_CheckHearing_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_4.hpp"
int* AI_CheckHearing_4(void* context, int arg2, int arg3, char arg4)

{
    int* result;

    for (int* i = &g_53caa0; i < "IENDLYDETECTION"; i = &i[3])
    {
        result = i[-2] + arg2;
        int entity_2 = i[-1] + arg3;

        if (result >= 0 && result < *(context + 0x14) && entity_2 >= 0 && entity_2 < *(context + 0x18))
        {
            result = context + ((entity_2 + result * 0x14) << 2) + 0x20;
            int entity_4 = *i;

            if (!arg4)
                *result -= entity_4;
            else
                *result += entity_4;
        }
    }

    return result;
}
