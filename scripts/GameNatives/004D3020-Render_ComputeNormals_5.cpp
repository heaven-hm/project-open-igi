// IGI native provenance: Render_ComputeNormals_5 - 0x004D3020 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D3020-Render_ComputeNormals_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D3020-Render_ComputeNormals_5.hpp"
int Render_ComputeNormals_5(long double context , float* arg2, float* arg3, float arg4)

{
    float item_30;
    float item_24;
    int data_1 = Render_BindTexture_5(&item_30, &item_24, Render_SetupLighting_6(arg4, 0));
    float item_18;
    float item_c;
    int result = Render_BindTexture_5(&item_18, &item_c, Render_SetupLighting_6(arg4, 1));

    if (!data_1 && !result)
        return result;

    if (!data_1)
    {
        if (result)
        {
            if (arg2)
            {
                *arg2 = item_18;
                int item_14;
                arg2[1] = item_14;
                int item_10;
                arg2[2] = item_10;
            }

            if (arg3)
            {
                *arg3 = item_c;
                int item_8;
                arg3[1] = item_8;
                int item_4;
                arg3[2] = item_4;
            }
        }
    }
    else if (!result)
    {
        if (arg2)
        {
            *arg2 = item_30;
            int item_2c;
            arg2[1] = item_2c;
            int item_28;
            arg2[2] = item_28;
        }

        if (arg3)
        {
            *arg3 = item_24;
            int item_20;
            arg3[1] = item_20;
            int item_1c;
            arg3[2] = item_1c;
        }
    }
    else
    {
        (*(arg4 + 0xc))(*(arg4 + 0x18));
        arg4 = context;

        if (arg2)
            Render_SetupLighting_5(arg2, &item_30, &item_18, arg4);

        if (arg3)
            Render_SetupLighting_5(arg3, &item_24, &item_c, arg4);
    }

    return 1;
}
