// IGI native provenance: Vec3_ProjectRayPlane - 0x004B2C70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2C70-Sys_PopStack_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2C70-Vec3_ProjectRayPlane 2.hpp"
int Vec3_ProjectRayPlane(int* context, float* arg2, float* arg3, float* arg4)

{
    long double fpu_val_7_3 = arg3[1] * arg4[1] + *arg3 * *arg4 + arg4[2] * arg3[2];
    long double f0 = 9.99999975e-06.0f;
    fpu_val_7_3 - f0;

    if (*(&((fpu_val_7_3 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f0 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
    {
        long double f1_1 = -9.99999975e-06.0f;
        fpu_val_7_3 - f1_1;

        if (!(*(&((fpu_val_7_3 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_3 == f1_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
        {
            *context = 0;
            context[1] = 0;
            context[2] = 0;
            return 0;
        }
    }

    long double fpu_val_6_7 = arg2[2] * arg4[2] + *arg2 * *arg4 + arg2[1] * arg4[1];
    long double fpu_val_7_5 = fpu_val_6_7 / fpu_val_6_7;
    *context = *arg2 - fpu_val_7_5 * *arg3;
    context[1] = arg2[1] - fpu_val_7_5 * arg3[1];
    context[2] = arg2[2] - fpu_val_7_5 * arg3[2];
    return 1;
}
