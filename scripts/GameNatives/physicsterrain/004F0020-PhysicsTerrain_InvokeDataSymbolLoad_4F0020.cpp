// IGI native provenance: PhysicsTerrain_InvokeDataSymbolLoad_4F0020 - 0x004F0020 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0020-PhysicsTerrain_InvokeDataSymbolLoad_4F0020.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_InvokeDataSymbolLoad_4F0020.hpp"

int PhysicsTerrain_InvokeDataSymbolLoad_4F0020(int* arg1, void* arg2)

{
    char* result_val = *(arg2 + 4);

    if (*result_val)
    {
        char i;

        do
        {
            i = result_val[1];
            result_val = &result_val[1];
        } while (i);
    }

    int var_4 = arg1[5];
    int var_8 = arg1[4];
    int var_c = arg1[3];
    int var_10 = arg1[2];
    int var_14 = arg1[1];
    int var_18 = *arg1;
    return sprintf(result_val, "%f, %f, %f");
}
