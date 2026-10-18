// IGI native provenance: Camera_ResetMatrix_26 - 0x004AD140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD140-Camera_ResetMatrix_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD140-Camera_ResetMatrix_26.hpp"
unsigned int __stdcall __aulldiv(int context, unsigned int arg2, int arg3, unsigned int arg4) {
    if (!arg4)
        return COMBINE(COMBINE(0, arg2) % arg3, context) / arg3;

    unsigned int i = arg4;
    int context_1 = arg3;
    unsigned int temp_3 = arg2;
    int data_6 = context;

    do
    {
        context_1 = RRCD(context_1, 1, i & 1);
        unsigned int f4_1 = temp_3;
        temp_3 u>>= 1;
        data_6 = RRCD(data_6, 1, f4_1 & 1);
        i u>>= 1;
    } while (i);

    unsigned int result = COMBINE(temp_3, data_6) / context_1;
    int data_8 = result * arg4;
    int data_10;
    int temp_4;
    temp_4 = HIGHD(arg3 * result);
    data_10 = LOWD(arg3 * result);
    int temp_5 = temp_4 + data_8;

    if (temp_4 + data_8 >= temp_4 && temp_5 <= arg2 && (temp_5 < arg2 || data_10 <= context))
        return result;

    return result - 1;
}
