// IGI native provenance: Script_ExecuteLevelScript_31 - 0x004FE610 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FE610-Script_ExecuteLevelScript_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FE610-Script_ExecuteLevelScript_31.hpp"
double* Script_ExecuteLevelScript_31(void* context, double* arg2)

{
    int i = 0;
    double* result = *(context + 0xf0);

    if (result > 0)
    {
        do
        {
            i += 1;
            result = *(*(context + 0x6c) + (i << 2) - 4);
            *result = *result + *arg2;
            result[1] = arg2[1] + result[1];
            result[2] = result[2] + arg2[2];
        } while (i < *(context + 0xf0));
    }

    return result;
}
