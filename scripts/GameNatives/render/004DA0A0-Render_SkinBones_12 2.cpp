// IGI native provenance: Render_SkinBones_12 - 0x004DA0A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DA0A0-Render_SkinBones_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_SkinBones_12.h"
int Render_SkinBones_12(void* context, double* arg2, double* arg3, int* arg4, float arg5, int arg6, int arg7, int arg8, int arg9)

{
    *(context + 0x6c) = arg5;

    if (!arg4)
    {
        *(context + 0x80) = 0;
        *(context + 0x84) = 0;
        *(context + 0x88) = 0;
    }
    else
    {
        *(context + 0x80) = *arg4;
        *(context + 0x84) = arg4[1];
        *(context + 0x88) = arg4[2];
    }

    *(context + 0x70) = arg6;
    *(context + 0x74) = arg7;
    *(context + 0x78) = arg8;
    *(context + 0x7c) = arg9;
    double item_20 = (*arg3 - *arg2) * 0.5 + *arg2;
    double item_18 = (arg3[1] - arg2[1]) * 0.5 + arg2[1];
    double item_10 = (arg3[2] - arg2[2]) * 0.5 + arg2[2];
    return DynObj_UpdateDynTree_8(*(context + 0x68), context, &item_20, arg2, arg3);
}
