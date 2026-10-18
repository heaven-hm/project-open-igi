// IGI native provenance: Camera_UpdateViewCone_5 - 0x004A5782 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5782-Camera_UpdateViewCone_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5782-Camera_UpdateViewCone_5.hpp"
long double Camera_UpdateViewCone_5(int context, short arg2 , short arg3 , long double arg4 , int arg5 , int arg6 , int arg7 , char arg8 , int arg9 )

{
    int item_4 = context;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(arg2);
    item_4 = f0;

    if (item_4 != 0x27f)
        arg2 = __load_CW(item_4);

    int entity_val = context & 0x7ff00000;
    long double result;
    int arg_4;
    int data_1;
    int entity_6;
    long double fpu7;

    if (entity_val != 0x7ff00000)
    {
        int data_val;
        long double fp0;
        fp0 = __fload_withFB(entity_val, &arg_4);
        fpu7 = fp0;

        if (entity_val == 0x7ff00000)
        {
            entity_6 = 0;
            goto label_4a5870;
        }

        if (data_val & 0x7ff00000 || (arg5 & 0xfffff) | arg_4)
        {
            char entity_2 = *(&arg5 + 3) & 0x80;

            if (entity_2)
            {
                entity_2 = _test_whether_TOS_is_int(arg4);
                fpu7 = -(fpu7);

                if (!entity_2)
                {
                    result = g_5443b0;
                    data_1 = 1;
                    goto label_4a57fa;
                }
            }

            long double fp1_1;
            bool c1_1;
            fp1_1 = __fyl2x(fpu7, arg4);
            result = __twoToTOS(fp1_1);

            if (entity_2 == 1)
                result = -(result);

            if (!g_936058)
            {
                int864_t item_70;
                *(&item_70 + 0x64) = result;
                bool c1_2 = /* bool c1_2 = unknown  {fst qword [stack_ptr], fp0} */;
                int data_14 = *(&item_70 + 0x68) & 0x7ff00000;
                int data_16;

                if (!data_14)
                {
                    long double result_1;
                    bool c1_3;
                    result_1 = __fscale(result, 1536.0);
                    result = result_1;
                    long double fpu_val_7_7 = fabsl(result);
                    long double f1_1 = 2.2250738585072014e-308;
                    fpu_val_7_7 - f1_1;
                    data_16 = 4;

                    if (TEST_BITB(
                            *(&((fpu_val_7_7 < f1_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_7, f1_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_7 == f1_1 ? 1 : 0) << 0xe) + 1),
                            0))
                        result = result * 0.0;
                }
                else if (data_14 == 0x7ff00000)
                {
                    long double result_2;
                    bool c1_4;
                    result_2 = __fscale(result, -1536.0);
                    result = result_2;
                    long double fpu_val_7_12 = fabsl(result);
                    long double f2_1 = 1.7976931348623157e+308;
                    fpu_val_7_12 - f2_1;
                    short data_18 = (fpu_val_7_12 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_12, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_12 == f2_1 ? 1 : 0) << 0xe;
                    data_16 = 3;

                    if (!TEST_BITB(*(&data_18 + 1), 6) && !TEST_BITB(*(&data_18 + 1), 0))
                        result = result * inf.0;
                }
                else
                {
                    short data_15 = item_4;

                    if (data_15 == 0x27f)
                        return result;

                    bool c0;
                    bool c2;
                    bool c3;

                    if (data_15 & 0x20 || !(((c0 ? 1 : 0) << 8 | (c1_2 ? 1 : 0) << 9
                        | (c2 ? 1 : 0) << 0xa | (c3 ? 1 : 0) << 0xe) & 0x20))
                    {
                        short x87control_3;
                        short x87status_3;
                        x87control_3 = __fldcw_memmem16(item_4);
                        return result;
                    }

                    data_16 = 8;
                }

                return __startTwoArgErrorHandling(data_16, 0x1d, result, arg_4, arg5, arg6, arg7);
            }
        }
        else if (arg7 & 0x7ff00000)
        {
            short entity_7 = _test_whether_TOS_is_int(arg4);
            *(&entity_7 + 1) = *(&arg5 + 3);
            *(&entity_7 + 1) u>>= 7;

            if (arg9 & 0x80)
            {
                result = g_544540;

                if (*(&entity_7 + 1) & entity_7)
                    result = -(result);

                data_1 = 2;
                goto label_4a57fa;
            }

            result = 0;

            if (*(&entity_7 + 1) & entity_7)
                result = -(result);
        }
        else if ((arg7 & 0xfffff) | arg6)
            result = 0;
        else
            result = 1;
    }
    else if ((context & 0xfffff) | arg6)
    {
        int entity_5;
        long double fp0_3;
        fp0_3 = __fload_withFB(0, &arg_4);
        fpu7 = fp0_3;

        if (!(arg8 & 8))
        {
            entity_6 = entity_5 + 1;
            goto label_4a5870;
        }

    label_4a582c:
        result = arg4 + fpu7;
        data_1 = 1;
    label_4a57fa:

        if (!g_936058)
            return __startTwoArgErrorHandling(data_1, 0x1d, result, arg_4, arg5, arg6, arg7);
    }
    else
    {
        long double fp0_4;
        fp0_4 = __fload_withFB(0, &arg_4);
        fpu7 = fp0_4;
    label_4a5870:

        if ((arg5 & 0x7ff00000) == 0x7ff00000 && (arg5 & 0xfffff) | arg_4)
        {
            if (*(&arg5 + 2) & 8)
                goto label_4a582c;

            goto label_4a583c;
        }

        if (entity_6)
        {
        label_4a583c:
            result = arg4 + fpu7;
            data_1 = 7;
            goto label_4a57fa;
        }

        short fpu_control;
        short x87status_1;
        short x87tag;
        short f0_1;
        f0_1 = __fnsave_memmem108(arg2, arg3, x87status);
        uint864_t item_70_1 = f0_1;
        void item_78;
        int data_6 = Camera_ResetMatrix_18(fpu7, arg4, &item_78);
        __frstor_memmem108(item_70_1);
        result = item_78;

        if (data_6)
        {
            data_1 = 1;
            goto label_4a57fa;
        }
    }
    short x87control_2;
    short x87status_2;

    if (item_4 != 0x27f)
        x87control_2 = __fldcw_memmem16(item_4);
    return result;
}
