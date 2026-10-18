// IGI native provenance: Sys_UnknownDataType_2 - 0x004BCAE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BCAE0-Sys_UnknownDataType_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_UnknownDataType_2.h"
int Sys_UnknownDataType_2(char* context, int arg2, float arg3, int arg4, int arg5)

{
    float source_val = arg3;
    void* data_val = *(source_val + 0xc);
    int result = 0;
    int entity_val = *(data_val + 0x18);
    int flagP = *(data_val + 0x14);
    int dest_val = *(data_val + 0x1c);
    *(data_val + 0x14) = *(*(source_val + 0x10) + (arg4 << 2));
    Sys_ExecuteBytecode_28(*(source_val + 0xc));
    char* data_2 = *(source_val + 0xc);

    if (*(data_2 + 0x1c) < dest_val && !*(data_2 + 0x20))
    {
        if (arg5 == 1)
        {
            if (Sys_CloseFileStream_28(data_2, &arg4, &arg3))
            {
                *context = arg4;
                result = 1;
            }
        }
        else if (arg5 == 2)
        {
            if (Sys_CloseFileStream_28(data_2, &arg4, &arg3))
            {
                *context = arg3;
                result = 1;
            }
        }
        else
        {
            char* item_c;
            int item_4;

            if (arg5 == 3)
            {
                if (Sys_FreeMemory_28(data_2, &item_c, &item_4, 3))
                {
                    Camera_ApplyLineFilter_5(context, item_c, arg2 - 1);
                    context[arg2 - 1] = 0;
                    result = 1;
                }
            }
            else
            {
                Error_Show("Unknown data type.");

                while (true)
                    /* nop */
            }
        }
    }

    *(*(source_val + 0xc) + 0x14) = flagP;
    *(*(source_val + 0xc) + 0x1c) = dest_val;
    *(*(source_val + 0xc) + 0x18) = entity_val;
    return result;
}
