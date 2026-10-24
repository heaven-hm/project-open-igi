// IGI native provenance: Camera_ResetMatrix_12 - 0x004A8589 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8589-Camera_ResetMatrix_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A8589-Camera_ResetMatrix_12.hpp"
long double Camera_ResetMatrix_12(int context, int arg2, int arg3, int arg4, int arg5, int arg6, double arg7, int arg8)

{
    int data_val = Camera_SetFieldOfView_13(arg2);
    int item_20 = data_val;
    short fpu_control;

    if (!data_val)
    {
        Camera_UpdateTransform_14(fpu_control, arg8, 0xffff);
        Camera_UpdateTransform_13(context);
        return arg7;
    }

    int item_1c = arg3;
    int item_18 = arg4;
    int item_14 = arg5;
    int item_10 = arg6;
    int item_24 = context;
    int item_8 = *(&arg7 + 4);
    Camera_UpdateTransform_14(fpu_control, arg8, 0xffff);
    int* item_34 = &item_24;
    Camera_UpdateTransform_13(context);
    return arg7;
}
