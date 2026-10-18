// IGI native provenance: HumanAI_ExecutePipeline_453AE0 - 0x00453AE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453AE0-HumanAI_ExecutePipeline_453AE0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_ExecutePipeline_453AE0.hpp"

void* HumanAI_ExecutePipeline_453AE0(void* arg1, int arg2)

{
    int context_val = *(arg1 + 8);

    if (!(context_val & 4))
    {
        void* edx_1 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 8;
        int ecx_2 = *(edx_1 + 0x20);
        *(arg1 + 0x14) = ecx_2;
        return ecx_2(arg1, arg2);
    }

    if (context_val & 0x10000)
    {
        void* edx_3 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 9;
        int ecx_4 = *(edx_3 + 0x24);
        *(arg1 + 0x14) = ecx_4;
        return ecx_4(arg1, arg2);
    }

    if (context_val & 0x20000)
    {
        void* edx_5 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 0xa;
        int ecx_6 = *(edx_5 + 0x28);
        *(arg1 + 0x14) = ecx_6;
        return ecx_6(arg1, arg2);
    }

    if (context_val & 0x200000)
    {
        void* edx_7 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 6;
        int ecx_8 = *(edx_7 + 0x18);
        *(arg1 + 0x14) = ecx_8;
        return ecx_8(arg1, arg2);
    }

    if (*(&context_val + 1) & 8)
    {
        void* edx_9 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 4;
        int ecx_10 = *(edx_9 + 0x10);
        *(arg1 + 0x14) = ecx_10;
        return ecx_10(arg1, arg2);
    }

    if (context_val & 0x80000)
    {
        void* edx_11 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 0xb;
        int ecx_12 = *(edx_11 + 0x2c);
        *(arg1 + 0x14) = ecx_12;
        return ecx_12(arg1, arg2);
    }

    if (!(context_val & 0x40000))
    {
        if (context_val & 0x80)
        {
            void* edx_15 = *(arg1 + 4);
            *(arg1 + 0xc) = *(arg1 + 0x10);
            *(arg1 + 0x10) = 1;
            int ecx_16 = *(edx_15 + 4);
            *(arg1 + 0x14) = ecx_16;
            return ecx_16(arg1, arg2);
        }
    }
    else if (context_val & 0x80)
    {
        void* edx_13 = *(arg1 + 4);
        *(arg1 + 0xc) = *(arg1 + 0x10);
        *(arg1 + 0x10) = 2;
        int ecx_14 = *(edx_13 + 8);
        *(arg1 + 0x14) = ecx_14;
        return ecx_14(arg1, arg2);
    }

    return arg1;
}
