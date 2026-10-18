// IGI native provenance: Camera_UpdateViewCone - 0x004A2490 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A2490-Camera_UpdateViewCone.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateViewCone.h"
struct status_msg* Camera_UpdateViewCone(void* context, unsigned int arg2, float arg3, float arg4, float arg5, unsigned int arg6, unsigned int arg7, unsigned int arg8)

{
    int kind;
    kind = g_543d50;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    result->head[8] = arg2;
    result->head[9] = 0;
    result->head[0xa] = arg8;
    Sys_AllocateMemory_8(&result->head[0xb], 0, 0x40, 0x10, 0x10, 3, 0, 0);
    long double fpu7 = 0.0f;
    long double f0 = arg3;
    fpu7 - f0;
    result->head[0x16] = arg7;
    result->head[0x17] = arg7;
    result->head[0x14] = 0;
    result->head[0x15] = arg6;

    if (*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x41)
    {
        long double fpu_val_7_1 = 1.0f;
        long double f1_1 = arg3;
        fpu_val_7_1 - f1_1;

        if (*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 1)
            arg3 = 1.0f;
    }
    else
        arg3 = 0.0f;

    long double fpu_val_7_2 = 0.0f;
    long double f2 = arg4;
    fpu_val_7_2 - f2;

    if (*(&((fpu_val_7_2 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2) ? 1 : 0) << 0xa
        | (fpu_val_7_2 == f2 ? 1 : 0) << 0xe) + 1) & 0x41)
    {
        long double fpu_val_7_3 = 1.0f;
        long double f3_1 = arg4;
        fpu_val_7_3 - f3_1;

        if (*(&((fpu_val_7_3 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f3_1) ? 1 : 0) << 0xa
                | (fpu_val_7_3 == f3_1 ? 1 : 0) << 0xe) + 1) & 1)
            arg4 = 1.0f;
    }
    else
        arg4 = 0.0f;

    long double fpu_val_7_5 = arg5;
    long double fpu6 = 0.0f;
    fpu6 - fpu_val_7_5;

    if (*(&((fpu6 < fpu_val_7_5 ? 1 : 0) << 8 | (FCMP_UO(fpu6, fpu_val_7_5) ? 1 : 0) << 0xa
        | (fpu6 == fpu_val_7_5 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
    {
        long double fpu_val_6_1 = 1.0f;
        fpu_val_6_1 - fpu_val_7_5;

        if (*(&((fpu_val_6_1 < fpu_val_7_5 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fpu_val_7_5) ? 1 : 0) << 0xa
                | (fpu_val_6_1 == fpu_val_7_5 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
            fpu_val_7_5 = 1.0f;
    }
    else
        fpu_val_7_5 = 0.0f;

    result->head[0x1b] = fpu_val_7_5;
    result->head[0x19] = arg3;
    result->head[0x1a] = arg4;
    result->head[0x18] = g_543d54;
    char data_4 = QHash_ValueGet();

    if (!data_4)
        QHash_ValueSet();

    result->head[0x1c] = 0;
    result->head[0x1c] = Sys_DispatchOpcode_18(g_935c04);

    if (!data_4)
        Sys_DispatchOpcode_2();

    return result;
}
