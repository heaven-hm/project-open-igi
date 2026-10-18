// IGI native provenance: Script_FormatString_22 - 0x004F9290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9290-Script_FormatString_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9290-Script_FormatString_22.hpp"
void* Script_FormatString_22(int context)

{
    if (context != 0xffffffff)
    {
        int* i_1 = g_a77248;

        for (int* i = *i_1; i; i = *i)
        {
            int* data_1 = i_1[2];

            if (data_1)
            {
                void* result = *data_1;

                if (*(result + 0x50) == 0xffffffff)
                    *(result + 0x50) = Script_EvaluateCondition_4(result);

                if (context == *(result + 0x50))
                    return result;
            }

            i_1 = i;
        }
    }

    return 0;
}
