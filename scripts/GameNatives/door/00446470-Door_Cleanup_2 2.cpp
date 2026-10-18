// IGI native provenance: Door_Cleanup_2 - 0x00446470 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00446470-Door_Cleanup_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Door_Cleanup_2.h"
int Door_Cleanup_2(void* context, double* arg2)

{
    long double fp0 = Sys_PushStack_11(arg2, context + 0x20);
    long double fpu6 = arg2[3];
    long double f0 = 0.0;
    fpu6 - f0;
    short result = (fpu6 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu6, f0) ? 1 : 0) << 0xa
        | (fpu6 == f0 ? 1 : 0) << 0xe | 0x3800;

    if (!(*(&result + 1) & 1))
    {
        long double f1_1 = arg2[3];
        fp0 - f1_1;
        result = (fp0 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fp0, f1_1) ? 1 : 0) << 0xa
            | (fp0 == f1_1 ? 1 : 0) << 0xe | 0x3800;

        if (!(*(&result + 1) & 1))
            return result;
    }

    arg2[3] = fp0;
    arg2[4] = context;
    return result;
}
