// IGI native provenance: Camera_ApplyHandiCam_17 - 0x004A9B8E - CFG score 62.5% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A9B8E-Camera_ApplyHandiCam_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9B8E-Camera_ApplyHandiCam_17.hpp"
void __fastcall Camera_ApplyHandiCam_17(short context, void* arg2 , long double arg3 )

{
    long double fpu7 = fabsl(arg3);
    long double fpu6 = g_54455e;
    fpu6 - fpu7;
    *(arg2 - 0xa0) = (fpu6 < fpu7 ? 1 : 0) << 8 | (FCMP_UO(fpu6, fpu7) ? 1 : 0) << 0xa
        | (fpu6 == fpu7 ? 1 : 0) << 0xe;

    if (*(arg2 - 0x9f) & 0x41)
    {
        long double f1 = 0;
        arg3 - f1;
        *(arg2 - 0xa0) = (arg3 < f1 ? 1 : 0) << 8 | (FCMP_UO(arg3, f1) ? 1 : 0) << 0xa
            | (arg3 == f1 ? 1 : 0) << 0xe;

        if (*(arg2 - 0x9f) & 1)
        {
            *(arg2 - 0x90) = 4;
            /* tailcall */
            return sub_4a8bd6();
        }

        g_544540;
        *(&context + 1) = *(&context + 1);
        return;
    }

    long double fpu_val_7_2 = roundl(arg3);
    long double f2 = 0;
    fpu_val_7_2 - f2;
    *(arg2 - 0xa0) = (fpu_val_7_2 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2) ? 1 : 0) << 0xa
        | (fpu_val_7_2 == f2 ? 1 : 0) << 0xe | 0x3800;
    int temp_val;
    temp_val = *(arg2 - 0x9f);
    long double fpu_val_7_4 = arg3 - fpu_val_7_2;
    long double f3 = 0;
    fpu_val_7_4 - f3;
    *(arg2 - 0xa0) = (fpu_val_7_4 < f3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f3) ? 1 : 0) << 0xa
        | (fpu_val_7_4 == f3 ? 1 : 0) << 0xe | 0x3800;
    long double fp0;
    bool c1;
    fp0 = __f2xm1(fabsl(fpu_val_7_4));
}
