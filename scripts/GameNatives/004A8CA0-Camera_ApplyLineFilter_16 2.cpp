// IGI native provenance: Camera_ApplyLineFilter_16 - 0x004A8CA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8CA0-Camera_ApplyLineFilter_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_16.h"
long double Camera_ApplyLineFilter_16(short context , long double arg2, long double arg3)

{
    long double fpu7 = arg3;
    long double fpu6 = arg2;

    while (true)
    {
        int data_1 = *(&arg2 + 4);

        if (data_1 * 2 < data_1)
        {
            int data_3 = (data_1 * 2) ^ 0xe000000;

            if (data_3 & 0xe000000)
                return fpu7 / fpu6;

            if (!*((data_3 >> 0x1c) + 0x5443e0))
                return fpu7 / fpu6;

            int data_6 = *(&arg2 + 8) & 0x7fff;

            if (!data_6 || data_6 == 0x7fff)
                return fpu7 / fpu6;

            short x87status_1;
            short f0_1;
            f0_1 = __fnstcw_memmem16(context);
            short fpu_control;
            short x87status_2;
            fpu_control = __fldcw_memmem16((f0_1 | 0x33f) & 0xf3ff);

            if ((*(&arg3 + 8) & 0x7fff) == 1)
            {
                long double fpu_val_7_6 = fpu6 * g_5443f4;
                long double fpu_val_7_7 = fpu7 * g_5443f4;
                short x87control_2;
                short x87status_4;
                x87control_2 = __fldcw_memmem16(f0_1);
                return fpu_val_7_7 / fpu_val_7_6;
            }

            long double fpu_val_7_3 = fpu6 * g_5443f0;
            long double fpu_val_7_4 = fpu7 * g_5443f0;
            short x87control_1;
            short x87status_3;
            x87control_1 = __fldcw_memmem16(f0_1);
            return fpu_val_7_4 / fpu_val_7_3;
        }

        if (!(arg2 | *(&arg2 + 4)) || *(&arg2 + 8) & 0x7fff)
            return fpu7 / fpu6;

        short x87status_5;
        short f0_6;
        f0_6 = __fnstcw_memmem16(context);
        short x87control_3;
        short x87status_6;
        x87control_3 = __fldcw_memmem16((f0_6 | 0x33f) & 0xf3ff);
        int data_20 = *(&arg3 + 8) & 0x7fff;
        short x87control_4;
        short x87status_8;

        if (!data_20)
        {
            int data_23 = *(&arg3 + 4);

            if (data_23 * 2 < data_23)
            {
                x87control_4 = __fldcw_memmem16(f0_6);
                return fpu7 / fpu6;
            }
        }
        else
        {
            if (data_20 == 0x7fff)
            {
                x87control_4 = __fldcw_memmem16(f0_6);
                return fpu7 / fpu6;
            }

            int data_21 = *(&arg3 + 4);

            if (data_21 * 2 >= data_21)
            {
                x87control_4 = __fldcw_memmem16(f0_6);
                return fpu7 / fpu6;
            }
        }
        arg2 = fpu6 * g_5443f8;
        fpu7 = arg3;
        short x87status_7;
        context = __fldcw_memmem16(f0_6);
    }
}
