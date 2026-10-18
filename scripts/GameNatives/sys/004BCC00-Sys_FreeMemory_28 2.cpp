// IGI native provenance: Sys_FreeMemory_28 - 0x004BCC00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BCC00-Sys_FreeMemory_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_FreeMemory_28.h"
int Sys_FreeMemory_28(char* context, int* arg2, int* arg3, int* arg4)

{
    char* source_val = context;

    if (*(*(source_val + 0x28) + (*(source_val + 0x1c) << 2)) == arg4)
    {
        Sys_UnknownDataType_3(source_val, arg2, &context, nullptr);
        int dest_2 = *arg2;
        int i = 0xffffffff;
        context = *(*(source_val + 0x24) + (*(source_val + 0x18) << 2));

        while (i)
        {
            bool flag_0_1 = 0 != *dest_2;
            dest_2 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        *arg3 = ~i;
        return 1;
    }

    if (Sys_OpenFileStream_28(source_val, &arg4))
    {
        int* entity_3 = arg4;
        int data_5 = *entity_3;

        if (!data_5 || data_5 == 2)
        {
            if (entity_3[1] == 3)
            {
                int dest_3 = entity_3[6];
                *arg2 = dest_3;
                int i_1 = 0xffffffff;

                while (i_1)
                {
                    bool flag_1_1 = 0 != *dest_3;
                    dest_3 += 1;
                    i_1 -= 1;

                    if (!flag_1_1)
                        break;
                }

                *arg3 = ~i_1;
            }

            return 1;
        }
    }

    *arg2 = 0;
    *arg3 = 0;
    return 0;
}
