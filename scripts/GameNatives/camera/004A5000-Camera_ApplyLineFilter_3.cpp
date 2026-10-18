// IGI native provenance: Camera_ApplyLineFilter_3 - 0x004A5000 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A5000-Camera_ApplyLineFilter_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5000-Camera_ApplyLineFilter_3.hpp"
int Camera_ApplyLineFilter_3(int context , int arg2 , int arg3 , int* arg4, int arg5, struct _EXCEPTION_REGISTRATION_RECORD* arg6, int arg7)

{
    *arg4 = context;
    int entry_base;
    arg4[1] = entry_base;
    arg4[2] = arg3;
    arg4[3] = arg2;
    arg4[4] = &return_addr;
    arg4[5] = return_addr;
    strncpy(&arg4[8], "02CV", 8);
    TEB* fsbase;
    struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList = fsbase->NtTib.ExceptionList;
    arg4[6] = ExceptionList;

    if (ExceptionList == 0xffffffff)
        arg4[7] = 0xffffffff;
    else if (!arg5)
        arg4[7] = *(ExceptionList + 0xc);
    else
    {
        arg4[9] = arg6;

        if (arg5 != 1)
        {
            arg4[7] = arg7;
            int entity_4 = arg5 - 2;

            if (arg5 != 2)
            {
                int item_4_1 = arg2;
                int item_8_1 = arg3;

                if (entity_4 > 6)
                    entity_4 = 6;

                void arg_14;
                memcpy(&arg4[0xa], &arg_14, entity_4 << 2);
            }
        }
        else
            arg4[7] = *(arg6 + 0xc);
    }

    return 0;
}
