// IGI native provenance: Qvm_Invoke28 - 0x004BD730 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BD730-Qvm_Invoke28.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Qvm_Invoke28.h"

int __fastcall Qvm_Invoke28(float arg1)

{
    float var_4 = arg1;
    int arg_4;
    int* source_ptr = arg_4;
    int data_val = 0;
    int context_val = source_ptr[5];
    char* eax_1 = *source_ptr + context_val;
    data_val = *eax_1;
    int dest_ptr = *(eax_1 + 1);

    if (data_val != 0x14)
    {
        if (data_val == 0x15)
        {
            int eax_15 = Sys_CloseFileStream_28(source_ptr, &arg_4, &var_4);

            if (eax_15 != 1)
            {
                if (eax_15 != 2)
                {
                label_4bd836:
                    int edx_1 = source_ptr[5];
                    source_ptr[8] = 0xc;
                    source_ptr[4] = edx_1;
                    dest_ptr = 0;
                }
                else
                {
                    long double x87_r7_2 = var_4;
                    long double temp3_1 = 0f;
                    x87_r7_2 - temp3_1;

                    if (!(*(&((x87_r7_2 < temp3_1 ? 1 : 0) << 8
                        | (FCMP_UO(x87_r7_2, temp3_1) ? 1 : 0) << 0xa
                        | (x87_r7_2 == temp3_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    {
                        int eax_21 = source_ptr[5] + 5;
                        source_ptr[5] = eax_21;
                        return eax_21;
                    }
                }
            }
            else if (arg_4)
            {
                int eax_18 = source_ptr[5] + 5;
                source_ptr[5] = eax_18;
                return eax_18;
            }
        }
        else
        {
            if (data_val != 0x16)
            {
                source_ptr[8] = 3;
                source_ptr[4] = context_val;
                source_ptr[5] = context_val + 5;
                return context_val + 5;
            }

            int eax_8 = Sys_CloseFileStream_28(source_ptr, &arg_4, &var_4);

            if (eax_8 != 1)
            {
                if (eax_8 != 2)
                    goto label_4bd836;

                long double x87_r7_1 = var_4;
                long double temp4_1 = 0f;
                x87_r7_1 - temp4_1;

                if (*(&((x87_r7_1 < temp4_1 ? 1 : 0) << 8
                    | (FCMP_UO(x87_r7_1, temp4_1) ? 1 : 0) << 0xa
                    | (x87_r7_1 == temp4_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                {
                    int eax_14 = source_ptr[5] + 5;
                    source_ptr[5] = eax_14;
                    return eax_14;
                }
            }
            else if (!arg_4)
            {
                int eax_11 = source_ptr[5] + 5;
                source_ptr[5] = eax_11;
                return eax_11;
            }
        }
    }

    int eax_23 = source_ptr[5] + dest_ptr + 5;
    source_ptr[5] = eax_23;
    return eax_23;
}
