// IGI native provenance: Script_ParseEnumInt32_21 - 0x004F8140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F8140-Script_ParseEnumInt32_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumInt32_21.h"
void* Script_ParseEnumInt32_21(int context, int arg2, int* arg3)

{
    *arg3 = 0;
    void* result;

    for (int* i = &g_a76ca4; i < &g_a76cc4; i = &i[1])
    {
        result = *i;

        if (result)
        {
            *(context + (*arg3 << 2)) = result + 0x2d18;
            *(arg2 + (*arg3 << 2)) = *(result + 0x4288);
            result = *arg3 + 1;
            *arg3 = result;
        }
    }

    return result;
}
