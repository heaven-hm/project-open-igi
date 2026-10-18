// IGI native provenance: Camera_UpdateTransform_17 - 0x004A975E - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A975E-Camera_UpdateTransform_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateTransform_17.h"
long double Camera_UpdateTransform_17(int context, int arg2, short arg3 , long double arg4, int arg5 , long double arg6, long double arg7)

{
    int data_1 = arg5 ^ 0x700;
    short arg_28;
    long double result;
    bool c0;
    bool c1;
    bool c2_3;
    bool c3;

    if (data_1 & 0x700 || !*((data_1 >> 0xb & 0xf) + 0x5443fc) || (arg5 & 0x7fff0000) == 0x7fff0000)
    {
    label_4a9900:
        long double result_1;
        unsigned char f0_7;
        result_1 = __fprem1(arg7, arg4);
        result = result_1;

        if (!c2_3)
        {
            c0 = f0_7 & 4;
            c1 = f0_7 & 1;
            c3 = f0_7 & 2;
        }
    }
    else
    {
        int data_7 = *(&arg7 + 6) & 0x7fff0000;

        if (!data_7 || data_7 == 0x7fff0000)
            goto label_4a9900;

        int data_8 = *(&arg7 + 4);

        if (data_8 != -(data_8))
            goto label_4a9900;

        int data_10 = *(&arg4 + 4);

        if (data_10 != -(data_10))
            goto label_4a9900;

        int context_2 = *(&arg7 + 8) & 0x7fff;

        if (context_2 <= (*(&arg5 + 2) & 0x7fff) + 0x3f)
        {
            while ((*(&arg7 + 8) & 0x7fff) - ((*(&arg5 + 2) & 0x7fff) + 0xa) >= 0)
            {
                int data_18 = *(&arg5 + 2);
                int context_8 = *(&arg7 + 8) & 0x7fff;
                *(&arg5 + 2) = (context_8 - (((context_8 - data_18) & 7) | 4)) | (data_18 & 0x8000);
                *(&arg5 + 2) = data_18;
                long double fp0_1;
                unsigned char f0_2;
                bool c2_1;
                fp0_1 = __fprem(arg7, arg4);

                if (!c2_1)
                {
                    c0 = f0_2 & 4;
                    c1 = f0_2 & 1;
                    c3 = f0_2 & 2;
                }

                arg7 = fp0_1;
            }

            goto label_4a9900;
        }

        if (!((context_2 - ((*(&arg5 + 2) & 0x7fff) + 0x3f)) & 2))
            arg6 = arg4;

        short x87status_1;
        char f0_3;
        f0_3 = __fnstcw_memmem16(arg3);
        arg_28 = f0_3;
        short fpu_control;
        short x87status_2;
        fpu_control = __fldcw_memmem16(arg_28 | 0x33f);
        int i_1 = ((((*(&arg7 + 8) & 0x7fff) - (*(&arg5 + 2) & 0x7fff)) & 0x3f) | 0x20) + 1;
        int data_24 = *(&arg5 + 2) & 0x8000;
        *(&arg5 + 2) = (*(&arg7 + 8) & 0x7fff) | data_24;
        long double fpu_val_7_6 = fabsl(arg4);
        long double result_2 = fabsl(arg7);
        int i;

        do
        {
            result_2 - fpu_val_7_6;
            c0 = result_2 < fpu_val_7_6;
            c1 = false;
            c3 = result_2 == fpu_val_7_6;
            data_24 = (c0 ? 1 : 0) << 8 | (FCMP_UO(result_2, fpu_val_7_6) ? 1 : 0) << 0xa
                | (c3 ? 1 : 0) << 0xe | 0x3000;
            data_24 &= 0x100;

            if (!data_24)
                result_2 = result_2 - fpu_val_7_6;

            fpu_val_7_6 = fpu_val_7_6 * g_54442c;
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
        long double fp0_2;
        unsigned char f0_6;
        fp0_2 = __fprem1(g_544434, arg6);

        if (!c2_3)
        {
            c0 = f0_6 & 4;
            c1 = f0_6 & 1;
            c3 = f0_6 & 2;
        }

        result = result_2;
        short x87status_3;
        arg3 = __fldcw_memmem16(arg_28);

        if (*(&arg7 + 8) & 0x8000)
        {
            result = -(result);
            c1 = false;
        }
    }

    if (arg2 & 3)
    {
        if (arg2 & 1)
        {
            short x87status_4;
            char f0_8;
            f0_8 = __fnstcw_memmem16(arg3);
            arg_28 = f0_8;
            short x87control_1;
            short x87status_5;
            x87control_1 = __fldcw_memmem16(arg_28 | 0x300);
            result = result * g_54441c;
            short x87control_2;
            short x87status_6;
            x87control_2 = __fldcw_memmem16(arg_28);
        }

        short x87status_7;
        char f0_11;
        f0_11 = __fnstenv_memmem28();
        uint224_t item_28_1 = f0_11;
        *(&item_28_1 + 4) &= 0xbcff;
        *(&item_28_1 + 4) |= ((c0 ? 1 : 0) << 8 | (c1 ? 1 : 0) << 9 | (c2_3 ? 1 : 0) << 0xa
            | (c3 ? 1 : 0) << 0xe | 0x3000) & 0x4300;
        __fldenv_memmem28(item_28_1);
    }

    return result;
}
