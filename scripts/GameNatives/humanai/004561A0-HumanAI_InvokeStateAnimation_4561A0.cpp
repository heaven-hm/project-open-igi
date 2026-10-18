// IGI native provenance: HumanAI_InvokeStateAnimation_4561A0 - 0x004561A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004561A0-HumanAI_InvokeStateAnimation_4561A0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_InvokeStateAnimation_4561A0.hpp"

void* HumanAI_InvokeStateAnimation_4561A0(void* arg1, int arg2)

{
    int context_val = *(arg1 + 8);

    if (!(context_val & 4))
    {
        void* edx_1 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 0xb;
        int ecx_2 = *(edx_1 + 0x2c);
        *(arg1 + 0x14) = ecx_2;
        return ecx_2(arg1, arg2);
    }

    if (context_val & 0x20000)
    {
        void* edx_3 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 0xd;
        int ecx_4 = *(edx_3 + 0x34);
        *(arg1 + 0x14) = ecx_4;
        return ecx_4(arg1, arg2);
    }

    if (!(context_val & 1))
        return arg1;

    int* edx_5 = *(arg1 + 4);
    *(arg1 + 0xc) = *(arg1 + 0x10);
    *(arg1 + 0x10) = 0;
    int ecx_6 = *edx_5;
    *(arg1 + 0x14) = ecx_6;
    return ecx_6(arg1, arg2);
}
