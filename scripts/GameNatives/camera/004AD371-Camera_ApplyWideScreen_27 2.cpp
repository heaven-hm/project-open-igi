// IGI native provenance: Camera_ApplyWideScreen_27 - 0x004AD371 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD371-Camera_ApplyWideScreen_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_27.h"
int Camera_ApplyWideScreen_27(int context, int* arg2, unsigned int arg3)

{
    unsigned int entity_val = *arg3;
    int data_1 = *arg2;
    arg3 = entity_val;
    int item_6c_1;

    if (data_1 == 1)
    {
    label_4ad3b6:
        item_6c_1 = 8;
    label_4ad3bf:

        if (!Camera_UpdateViewCone_12(item_6c_1, &arg2[6], entity_val))
        {
            int item_24;

            if (context == 0x10 || context == 0x16 || context == 0x1d)
            {
                double item_34_1 = *(arg2 + 0x10);
                int item_24_2 = (item_24 & 0xffffffe3) | 3;
            }
            else
                int item_24_1 = item_24 & 0xfffffffe;
            void item_5c;
            Camera_ApplyPostProcess_12(&item_5c, &arg3, item_6c_1, context, &arg2[2], &arg2[6]);
            /* no return */
        }
    }
    else
    {
        if (data_1 == 2)
        {
            item_6c_1 = 4;
            goto label_4ad3bf;
        }

        if (data_1 == 3)
        {
            item_6c_1 = 0x11;
            goto label_4ad3bf;
        }

        if (data_1 == 4)
        {
            item_6c_1 = 0x12;
            goto label_4ad3bf;
        }

        if (data_1 == 5)
            goto label_4ad3b6;

        if (data_1 == 7)
            *arg2 = 1;
        else if (data_1 == 8)
        {
            item_6c_1 = 0x10;
            goto label_4ad3bf;
        }
    }
    short fpu_control;
    Camera_UpdateTransform_14(fpu_control, arg3, 0xffff);

    if (*arg2 != 8 && !g_546af0)
    {
        int* item_6c_4 = arg2;
        Camera_ApplyHandiCam_27();
    }

    return Camera_UpdateTransform_13(*arg2);
}
