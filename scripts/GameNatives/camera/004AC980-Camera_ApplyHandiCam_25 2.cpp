// IGI native provenance: Camera_ApplyHandiCam_25 - 0x004AC980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC980-Camera_ApplyHandiCam_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyHandiCam_25.h"
int Camera_ApplyHandiCam_25(short* context, int* arg2, int* arg3)

{
    unsigned int entity_val = context[5];
    short* entity_1 = entity_val & 0x8000;
    int item_10 = *(context + 6);
    int entity_3 = *(context + 2);
    int context_2 = (entity_val & 0x7fff) - 0x3fff;
    unsigned int item_8 = *context << 0x10;
    int result;
    int source_1;

    if (context_2 != 0xffffc001)
    {
        void item_1c;
        Camera_UpdateTransform_25(&item_1c, &item_10);

        if (Camera_ResetMatrix_24(&item_10, arg3[2]))
            context_2 += 1;

        int data_5 = arg3[1];

        if (context_2 >= data_5 - arg3[2])
        {
            if (context_2 <= data_5)
            {
                Camera_UpdateTransform_25(&item_10, &item_1c);
                Camera_ApplyBandFilter_25(&item_10, data_5 - context_2);
                Camera_ResetMatrix_24(&item_10, arg3[2]);
                Camera_ApplyBandFilter_25(&item_10, arg3[3] + 1);
                goto label_4aca62;
            }

            if (context_2 < *arg3)
            {
                int item_2c_7 = arg3[3];
                int source_4 = arg3[5];
                *(&item_10 + 3) &= 0x7f;
                source_1 = source_4 + context_2;
                Camera_ApplyBandFilter_25(&item_10, item_2c_7);
                result = 0;
            }
            else
            {
                Camera_SetFieldOfView_25(&item_10);
                int item_30_3 = arg3[3];
                *(&item_10 + 3) |= 0x80;
                Camera_ApplyBandFilter_25(&item_10, item_30_3);
                source_1 = arg3[5] + *arg3;
                result = 1;
            }
        }
        else
        {
            Camera_SetFieldOfView_25(&item_10);
        label_4aca62:
            source_1 = 0;
            result = 2;
        }
    }
    else
    {
        source_1 = 0;

        if (Camera_ApplyLineFilter_25(&item_10))
            result = 0;
        else
        {
            Camera_SetFieldOfView_25(&item_10);
            result = 2;
        }
    }

    int dest_1 = arg3[4];
    int entity_12 = -(entity_1);
    int source_7 = source_1 << ((0x1f - arg3[3]) & 0x1f) | ((entity_12 - entity_12) & 0x80000000) | item_10;

    if (dest_1 == 0x40)
    {
        arg2[1] = source_7;
        *arg2 = entity_3;
    }
    else if (dest_1 == 0x20)
        *arg2 = source_7;

    return result;
}
