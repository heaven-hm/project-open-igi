// IGI native provenance: Terrain_SampleHeightmap_3 - 0x0051682D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051682D-Terrain_SampleHeightmap_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SampleHeightmap_3.hpp"
long double Terrain_SampleHeightmap_3(int context, short arg2 , long double arg3 , int arg4 , int arg5 )

{
    int temp_val;
    int item_4 = temp_val;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(arg2);
    item_4 = f0;
    long double result;
    bool z;

    if (z)
    {
        int data_2;

        if (context & 0xfffff || arg4)
        {
            long double result_1;
            result_1 = __convertTOStoQNaN(context, arg3);
            result = result_1;
        }
        else
        {
        label_5168b1:
            result = g_5443b0;
            data_2 = 1;
        }

        if (!g_936058)
            return __startOneArgErrorHandling(data_2, 0xd, result, arg4, arg5);
    }
    else
    {
        if (item_4 != 0x27f)
            __load_CW(item_4);

        bool c1;

        if (context >= 0x3ff00000)
        {
            if (context > 0x3ff00000 || (arg5 & 0xfffff) | arg4)
                goto label_5168b1;

            result = !(arg5 & 0x80000000) ? 0 : 3.1415926534682512 + 1.2154201012607932e-10;
        }
        else
        {
            long double result_2;
            result_2 = __fpatan(arg3, sqrtl((1 + arg3) * (1 - arg3)));
            result = result_2;
        }

        if (!g_936058)
        {
            short data_3 = item_4;

            if (data_3 != 0x27f)
            {
                bool c0;
                bool c2;
                bool c3;

                if (data_3 & 0x20 && ((c0 ? 1 : 0) << 8 | (c1 ? 1 : 0) << 9 | (c2 ? 1 : 0) << 0xa
                        | (c3 ? 1 : 0) << 0xe) & 0x20)
                    return __startOneArgErrorHandling(8, 0xd, result, arg4, arg5);

                short x87control_1;
                short x87status_2;
                x87control_1 = __fldcw_memmem16(item_4);
            }

            return result;
        }
    }

    short fpu_control;
    short x87status_1;

    if (item_4 != 0x27f)
        fpu_control = __fldcw_memmem16(item_4);
    return result;
}
