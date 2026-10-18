// IGI native provenance: Sys_ScriptInternalErrorI - 0x004BA880 - CFG score 84.4% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004BA880-Sys_ScriptInternalErrorI.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BA880-Sys_ScriptInternalErrorI.hpp"
char* Sys_ScriptInternalErrorI(int context, int* arg2, int arg3, char* arg4)

{
    int entity_val = 0;

    if (arg4)
    {
        int dest_1 = *(arg4 + 8);
        int i = 0xffffffff;

        while (i)
        {
            bool flag_1_1 = 0 != *dest_1;
            dest_1 += 1;
            i -= 1;

            if (!flag_1_1)
                break;
        }

        int* dest_2 = arg2;
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_2_1 = 0 != *dest_2;
            dest_2 += 1;
            i_1 -= 1;

            if (!flag_2_1)
                break;
        }

        entity_val = ~i - 1 + ~i_1 - 1 + 1;
    }
    else if (context == 2 || context == 5)
    {
        int* dest_3 = arg2;
        int i_2 = 0xffffffff;

        while (i_2)
        {
            bool flag_3_1 = 0 != *dest_3;
            dest_3 += 1;
            i_2 -= 1;

            if (!flag_3_1)
                break;
        }

        entity_val = ~i_2;
    }

    char* result = Mem_Alloc(entity_val + 0x28, 4);
    memset(result, 0, 0x28);
    *result = context;
    *(result + 0x24) = arg3 + 1;

    if (context - 1 > 7)
    {
        Error_Show("Script: Internal Error: Illegal expression type.");

        while (true)
            /* nop */

        return;
    }

    void* temp_3;

    switch (context)
    {
        case 1:
        {
            *(result + 8) = *arg2;
            return result;
            break;
        }
        case 2:
        {
            temp_3 = &result[0x28];
            *(result + 8) = temp_3;

            if (!arg4)
                goto label_4baa31;

            int dest_5 = *(arg4 + 8);
            int i_3 = 0xffffffff;

            while (i_3)
            {
                bool flag_7_1 = 0 != *dest_5;
                dest_5 += 1;
                i_3 -= 1;

                if (!flag_7_1)
                    break;
            }

            int entity_8 = ~i_3;
            int source_2;
            int dest_8;
            dest_8 = memcpy(temp_3, dest_5 - entity_8, entity_8 & 0xfffffffc);
            memcpy(dest_8, source_2, entity_8 & 3);
            int* dest_9 = arg2;
            int i_4 = 0xffffffff;

            while (i_4)
            {
                bool flag_8_1 = 0 != *dest_9;
                dest_9 += 1;
                i_4 -= 1;

                if (!flag_8_1)
                    break;
            }

            int entity_12 = ~i_4;
            int dest_11 = *(result + 8);
            int i_5 = 0xffffffff;

            while (i_5)
            {
                bool flag_9_1 = 0 != *dest_11;
                dest_11 += 1;
                i_5 -= 1;

                if (!flag_9_1)
                    break;
            }

            int source_4;
            int dest_13;
            dest_13 = memcpy(dest_11 - 1, dest_9 - entity_12, entity_12 & 0xfffffffc);
            memcpy(dest_13, source_4, entity_12 & 3);
            char* data_9 = *(arg4 + 0x20);

            if (data_9)
            {
                if (*(data_9 + 0x18) == arg4)
                {
                    *(data_9 + 0x18) = result;
                    *(result + 0x20) = *(arg4 + 0x20);
                    Sys_PushStack_24(arg4);
                    return result;
                }

                if (*(data_9 + 0x1c) == arg4)
                {
                    *(data_9 + 0x1c) = result;
                    *(result + 0x20) = *(arg4 + 0x20);
                    Sys_PushStack_24(arg4);
                    return result;
                }

                if (*data_9 != 6)
                {
                    Error_Show("Internal error!!!");

                    while (true)
                        /* nop */
                }
                else
                {
                    *(data_9 + 8) = result;
                    *(result + 0x20) = *(arg4 + 0x20);
                }
            }

            Sys_PushStack_24(arg4);
            return result;
            break;
        }
        case 3:
        {
            *(result + 8) = *arg2;
            *(result + 0xc) = arg2[1];
            return result;
            break;
        }
        case 4:
        case 6:
        {
            *(result + 8) = arg2;
            return result;
            break;
        }
        case 5:
        {
            temp_3 = &result[0x28];
            *(result + 8) = 0;
            *(result + 0x10) = temp_3;
            *(result + 0xc) = 0;
        label_4baa31:
            int* dest_14 = arg2;
            int i_6 = 0xffffffff;

            while (i_6)
            {
                bool flag_6_1 = 0 != *dest_14;
                dest_14 += 1;
                i_6 -= 1;

                if (!flag_6_1)
                    break;
            }

            int entity_19 = ~i_6;
            int source_6;
            int dest_17;
            dest_17 = memcpy(temp_3, dest_14 - entity_19, entity_19 & 0xfffffffc);
            memcpy(dest_17, source_6, entity_19 & 3);
            return result;
            break;
        }
        case 7:
        {
            result[1] = 0x1d;
            result[2] = Sys_ScriptInternalErrorU(0x1d);
            *(result + 8) = arg2;
            return result;
            break;
        }
        case 8:
        {
            char context_1 = *arg2;
            result[1] = context_1;
            result[2] = Sys_ScriptInternalErrorU(context_1);
            return result;
            break;
        }
    }
}
