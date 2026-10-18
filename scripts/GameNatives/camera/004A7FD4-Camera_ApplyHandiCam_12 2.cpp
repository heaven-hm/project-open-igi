// IGI native provenance: Camera_ApplyHandiCam_12 - 0x004A7FD4 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7FD4-Camera_ApplyHandiCam_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyHandiCam_12.h"
long double Camera_ApplyHandiCam_12(int context, int arg2, int arg3, int arg4, double arg5, int arg6)

{
    if (g_546af0)
    {
        g_936064 = 0x21;
        short fpu_control;
        Camera_UpdateTransform_14(fpu_control, arg6, 0xffff);
        return arg5;
    }

    int item_c = arg3;
    int item_10 = arg3;
    item_10 = arg5;
    int item_18 = arg3;
    item_18 = 0;
    int item_20 = arg3;
    item_20 = arg5;
    return Camera_ResetMatrix_12(1, arg4, item_20, arg3, item_18, arg3, item_10, arg6);
}
