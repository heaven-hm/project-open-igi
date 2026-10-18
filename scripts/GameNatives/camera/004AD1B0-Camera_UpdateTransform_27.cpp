// IGI native provenance: Camera_UpdateTransform_27 - 0x004AD1B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD1B0-Camera_UpdateTransform_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD1B0-Camera_UpdateTransform_27.hpp"
unsigned int __stdcall __aullrem(int context, unsigned int arg2, int arg3, unsigned int arg4) {
    unsigned int result;

    if (arg4)
    {
        unsigned int i = arg4;
        int context_1 = arg3;
        unsigned int temp_4 = arg2;
        int data_4 = context;

        do
        {
            context_1 = RRCD(context_1, 1, i & 1);
            unsigned int f4_1 = temp_4;
            temp_4 u>>= 1;
            data_4 = RRCD(data_4, 1, f4_1 & 1);
            i u>>= 1;
        } while (i);

        unsigned int f0_3 = COMBINE(temp_4, data_4) / context_1;
        int data_6 = f0_3 * arg4;
        int data_8;
        int temp_5;
        temp_5 = HIGHD(f0_3 * arg3);
        data_8 = LOWD(f0_3 * arg3);
        int temp_6 = temp_5 + data_6;

        if (temp_5 + data_6 < temp_5 || temp_6 > arg2)
            data_8 -= arg3;
        else if (temp_6 >= arg2 && data_8 > context)
            data_8 -= arg3;

        result = -((data_8 - context));
    }
    else
        result = COMBINE(COMBINE(0, arg2) % arg3, context) % arg3;

    return result;
}
