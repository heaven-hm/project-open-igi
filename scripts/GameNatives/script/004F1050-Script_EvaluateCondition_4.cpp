// IGI native provenance: Script_EvaluateCondition_4 - 0x004F1050 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1050-Script_EvaluateCondition_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F1050-Script_EvaluateCondition_4.hpp"
int Script_EvaluateCondition_4(int context)

{
    int result = 0;

    for (int* i = &g_a71890; i < &g_a75890; )
    {
        int* temp_1 = *i;

        if (temp_1 && *temp_1 == context)
            return result;

        i = &i[1];
        result += 1;
    }

    return 0xffffffff;
}
