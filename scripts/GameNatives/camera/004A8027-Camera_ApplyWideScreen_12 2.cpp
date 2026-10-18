// IGI native provenance: Camera_ApplyWideScreen_12 - 0x004A8027 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8027-Camera_ApplyWideScreen_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_12.h"
long double Camera_ApplyWideScreen_12(int context, int arg2, int arg3 )

{
    double arg_c;
    double arg_14;

    if (!Camera_UpdateViewCone_12(context, &arg_14, arg3))
    {
        int item_24;
        int item_24_1 = item_24 & 0xfffffffe;
        void item_5c;
        Camera_ApplyPostProcess_12(&item_5c, &arg3, context, arg2, &arg_c, &arg_14);
        /* no return */
    }

    int data_1 = Camera_ApplyLineFilter_13(context);

    if (g_546af0 || !data_1)
    {
        Camera_UpdateTransform_13(data_1);
        short fpu_control;
        Camera_UpdateTransform_14(fpu_control, arg3, 0xffff);
        return arg_14;
    }

    int item_64_2 = context;
    int item_68_2 = context;
    item_68_2 = arg_14;
    int item_70_1 = context;
    item_70_1 = 0;
    int item_78 = context;
    item_78 = arg_c;
    return Camera_ResetMatrix_12(data_1, arg2, item_78, context, item_70_1, context, item_68_2, arg3);
}
