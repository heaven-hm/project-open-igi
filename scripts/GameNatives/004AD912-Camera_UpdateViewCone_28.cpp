// IGI native provenance: Camera_UpdateViewCone_28 - 0x004AD912 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD912-Camera_UpdateViewCone_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD912-Camera_UpdateViewCone_28.hpp"
int Camera_UpdateViewCone_28(unsigned int context, int arg2)

{
    Camera_UpdateTransform_5(0x1000);
    int result = 0;

    if (context >= g_c32540 || !(*((&g_c32440)[context >> 5] + ((context & 0x1f) << 3) + 4) & 1))
        g_936064 = 9;
    else
    {
        unsigned int data_4 = Camera_ResetMatrix_9(context, 0, FILE_CURRENT);

        if (data_4 != 0xffffffff)
        {
            unsigned int data_5 = Camera_ResetMatrix_9(context, 0, FILE_END);

            if (data_5 != 0xffffffff)
            {
                unsigned int i = arg2 - data_5;

                if (i > 0)
                {
                    char item_1004[0xfe0];
                    Camera_ApplyBandFilter_26(&item_1004, 0, 0x1000);
                    int data_6 = Camera_ApplyWideScreen_29(context, 0x8000);

                    do
                    {
                        unsigned int i_1 = 0x1000;

                        if (i < 0x1000)
                            i_1 = i;

                        int data_7 = Camera_ApplyHandiCam_23(context, &item_1004, i_1);

                        if (data_7 == 0xffffffff)
                        {
                            if (g_936068 == 5)
                                g_936064 = 0xd;

                            result = 0xffffffff;
                            break;
                        }

                        i -= data_7;
                    } while (i > 0);

                    Camera_ApplyWideScreen_29(context, data_6);
                }
                else if (i < 0)
                {
                    Camera_ResetMatrix_9(context, arg2, FILE_BEGIN);
                    BOOL data_9 = SetEndOfFile(Camera_ApplyLineFilter_23(context));
                    int source_1 = -(data_9);
                    result = -((source_1 - source_1)) - 1;

                    if (result == 0xffffffff)
                    {
                        g_936064 = 0xd;
                        g_936068 = GetLastError();
                    }
                }

                Camera_ResetMatrix_9(context, data_4, FILE_BEGIN);
                return result;
            }
        }
    }

    return 0xffffffff;
}
