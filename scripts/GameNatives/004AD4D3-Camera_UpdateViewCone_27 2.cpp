// IGI native provenance: Camera_UpdateViewCone_27 - 0x004AD4D3 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD4D3-Camera_UpdateViewCone_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateViewCone_27.h"
int Camera_UpdateViewCone_27(unsigned int context, unsigned int arg2, unsigned char* arg3, void* arg4, PSTR arg5, int arg6, unsigned int arg7, int arg8)

{
    int item_8 = 0xffffffff;
    int item_c = 0x534878;
    int item_10 = 0x4a67ec;
    TEB* fsbase;
    struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList = fsbase->NtTib.ExceptionList;
    fsbase->NtTib.ExceptionList = &ExceptionList;
    int saved_target;
    int* item_1c = &saved_target;

    if (g_93624c)
        goto label_4ad54c;

    int result;

    if (!LCMapStringW(0, 0x100, &g_534864, 1, nullptr, 0))
    {
        if (LCMapStringA(0, 0x100, &g_534860, 1, nullptr, 0))
        {
            g_93624c = 2;
            goto label_4ad54c;
        }

        result = 0;
    }
    else
    {
        g_93624c = 1;
    label_4ad54c:

        if (arg4 > 0)
            arg4 = Camera_ApplyBandFilter_28(arg3, arg4);

        int data_4 = g_93624c;

        if (data_4 == 2)
            result = LCMapStringA(context, arg2, arg3, arg4, arg5, arg6);
        else if (data_4 != 1)
            result = 0;
        else
        {
            if (!arg7)
                arg7 = g_9361f4;

            int data_7 = -(arg8);
            int data_11 =
                MultiByteToWideChar(arg7, ((data_7 - data_7) & 8) + 1, arg3, arg4, nullptr, 0);

            if (!data_11)
                result = 0;
            else
            {
                int item_8_1 = 0;
                int data_13 = data_11 * 2 + 3;
                data_13 &= 0xfc;
                Camera_UpdateTransform_5(data_13);
                int* item_1c_1 = &saved_target;
                int* item_28_1 = &saved_target;
                int item_8_2 = 0xffffffff;

                if (!item_28_1)
                    result = 0;
                else if (!MultiByteToWideChar(arg7, MB_PRECOMPOSED, arg3, arg4, item_28_1, data_11))
                    result = 0;
                else
                {
                    int result_1 = LCMapStringW(context, arg2, item_28_1, data_11, nullptr, 0);
                    int result_2 = result_1;

                    if (!result_1)
                        result = 0;
                    else if (!(*(&arg2 + 1) & 4))
                    {
                        int item_8_3 = 1;
                        int data_18 = result_1 * 2 + 3;
                        data_18 &= 0xfc;
                        Camera_UpdateTransform_5(data_18);
                        int* item_1c_2 = &saved_target;
                        int* item_24_1 = &saved_target;
                        int item_8_4 = 0xffffffff;

                        if (!&saved_target)
                            result = 0;
                        else if (
                                !LCMapStringW(context, arg2, item_28_1, data_11, &saved_target, result_1))
                            result = 0;
                        else
                        {
                            PSTR lpMultiByteStr;
                            int cbMultiByte;

                            if (arg6)
                            {
                                cbMultiByte = arg6;
                                lpMultiByteStr = arg5;
                            }
                            else
                            {
                                cbMultiByte = 0;
                                lpMultiByteStr = nullptr;
                            }

                            result_1 = WideCharToMultiByte(arg7, 0x220, &saved_target, result_1,
                                lpMultiByteStr, cbMultiByte, nullptr, nullptr);

                            result = !result_1 ? 0 : result_1;
                        }
                    }
                    else if (!arg6)
                        result = result_1;
                    else if (result_1 > arg6)
                        result = 0;
                    else if (LCMapStringW(context, arg2, item_28_1, data_11, arg5, arg6))
                        result = result_1;
                    else
                        result = 0;
                }
            }
        }
    }

    fsbase->NtTib.ExceptionList = ExceptionList;
    return result;
}
