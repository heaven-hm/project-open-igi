// IGI native provenance: Camera_ApplyPostProcess_11 - 0x004A7E61 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7E61-Camera_ApplyPostProcess_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A7E61-Camera_ApplyPostProcess_11.hpp"
int* Camera_ApplyPostProcess_11(int context, int* arg2, int* arg3)

{
    int f0 = arg2[1];
    arg2[1] -= 1;
    unsigned int data_1;

    if (f0 - 1 < 0)
    {
        int item_c_1 = context;
        data_1 = _flsbuf(arg2);
    }
    else
    {
        char data_val = context;
        **arg2 = data_val;
        *arg2 += 1;
        data_1 = data_val;
    }

    if (data_1 != 0xffffffff)
    {
        *arg3 += 1;
        return arg3;
    }

    *arg3 = 0xffffffff;
    return arg3;
}
