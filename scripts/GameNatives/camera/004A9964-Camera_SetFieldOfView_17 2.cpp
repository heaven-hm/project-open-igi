// IGI native provenance: Camera_SetFieldOfView_17 - 0x004A9964 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9964-Camera_SetFieldOfView_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_SetFieldOfView_17.h"
long double Camera_SetFieldOfView_17(short context , long double arg2 , long double arg3 )

{
    long double item_1c = arg2;
    int data_val = *(&arg3 + 6);

    if (data_val & 0x7fff0000)
        return Camera_UpdateTransform_17(data_val, 0, context, arg3);

    if (!(arg3 | *(&arg3 + 4)))
    {
        long double result;
        long double f0_4;
        bool c2;
        result = __fprem(item_1c, arg3);
        return result;
    }

    int temp_val = 2;
    short x87status;
    short f0_2;
    f0_2 = __fnstcw_memmem16(context);
    short item_10 = f0_2;
    int item_c = item_10 | 0x33f;
    short fpu_control;
    short x87status_1;
    fpu_control = __fldcw_memmem16(item_c);
    int data_6 = *(&item_1c + 8) & 0x7fff;
    long double item_34_1;

    if (data_6 > 0x7fbe)
    {
        short x87status_2;
        short f0_3;
        f0_3 = __fnstcw_memmem16(fpu_control);
        item_10 = f0_3;
        data_6 = item_10 | 0x300;
        item_c = data_6;
        short x87control_1;
        short x87status_3;
        x87control_1 = __fldcw_memmem16(item_c);
        item_34_1 = arg3 * g_544414;
    }
    else
    {
        temp_val = 3;
        item_1c = item_1c * g_544414;
        item_34_1 = arg3 * g_544414;
    }

    short x87control_2;
    short x87status_4;
    x87control_2 = __fldcw_memmem16(item_10);
    return Camera_UpdateTransform_17(data_6, temp_val, x87control_2, item_34_1, arg3, item_1c, item_10, item_c);
}
