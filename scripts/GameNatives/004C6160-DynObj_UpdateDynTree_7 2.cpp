// IGI native provenance: DynObj_UpdateDynTree_7 - 0x004C6160 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6160-DynObj_UpdateDynTree_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_UpdateDynTree_7.h"
int DynObj_UpdateDynTree_7(void* context)

{
    int* source_val = context + 0xe00;
    int i = 0xffffffff;
    int* dest_val = source_val;
    int result = 0;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    if (~i != 1)
    {
        void* entity_2;
        int temp_1;
        result = Resource_FileExists(source_val);

        if (result)
        {
            int* item_20_1 = source_val;
            int item_8;
            int* data_1;
            int entity_3;
            data_1 = Resource_Load(&item_8, temp_1, entity_2, &item_8);
            int* dest_1 = data_1;
            *(context + 0xdf8) = dest_1;

            if (dest_1)
            {
                int flagP1 = 0;

                if (item_8 > 0)
                {
                    int* context_1 = &g_b81b20;

                    do
                    {
                        int source_1 = *dest_1;

                        if (source_1)
                        {
                            if (Sys_PushStack_8(entity_3) > 1)
                                Sys_ExecuteBytecode_8(1);

                            int data_5;
                            data_5 = Sys_PushStack_8(Sys_OpenFileStream_8(&context_1[2], *(context + 0xdf4), source_1, source_1,
                                source_1, 1, &dest_1[1]));

                            if (data_5 > 1)
                                entity_3 = Sys_ExecuteBytecode_8(0xffffffff);

                            *context_1 = source_1;
                            context_1[1] = &dest_1[1];
                        }

                        context_1 = &context_1[0xb];
                        int data_7 = source_1 * source_1;
                        dest_1 = &dest_1[1] + data_7;
                        flagP1 = flagP1 + 4 + data_7;
                    } while (flagP1 < item_8);
                }
            }

            return Resource_Unload(source_val);
        }
    }

    return result;
}
