// IGI native provenance: AI_CheckSector_24 - 0x00467620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00467620-AI_CheckSector_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckSector_24.h"
float* AI_CheckSector_24(float* context, float* arg2, float arg3, float arg4)

{
    double* entity_val = arg3;
    int* source_val = arg4;
    double* data_val = *source_val;
    long double fpu_val_7_1 = *entity_val - *data_val;
    long double fpu_val_6_1 = entity_val[1] - data_val[1];
    long double fpu_val_5_1 = entity_val[2] - data_val[2];
    float* data_1 = source_val[1];
    float item_8 = fpu_val_5_1 * data_1[5] + fpu_val_6_1 * data_1[4] + fpu_val_7_1 * data_1[3];
    float item_4 = fpu_val_5_1 * data_1[8] + fpu_val_6_1 * data_1[7] + fpu_val_7_1 * data_1[6];
    arg4 = g_c28b44 * 0.5f / (item_4 * source_val[2]);
    *context =
        g_c28b44 * 0.5f + arg4 * (fpu_val_5_1 * data_1[2] + fpu_val_6_1 * data_1[1] + fpu_val_7_1 * *data_1);
    context[1] = Display_ClearSurfaces_4() * arg4 * item_8 + g_c28b48 * 0.5f;

    if (arg2)
        *arg2 = item_4;

    return arg2;
}
