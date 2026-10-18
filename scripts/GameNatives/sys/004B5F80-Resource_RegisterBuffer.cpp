// IGI native provenance: Resource_RegisterBuffer - 0x004B5F80 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5F80-Resource_RegisterBuffer.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B5F80-Resource_RegisterBuffer.hpp"
int* Resource_RegisterBuffer(char* context, int* arg2, void* arg3)

{
    void item_8;
    Sys_AllocateMemory_5(&item_8, context);

    if (Sys_OpenFileStream_20(arg2) == 0x46464c49 && Sys_FreeMemory_20(arg2) == 0x53455249)
    {
        int* data_2 = Resource_CreateNode(context, 0, arg2, 0, 0, &item_8);
        Sys_ErrorInResourceFileS(context, data_2, arg2, &item_8);
        data_2[0xb] = 1;
        return arg2;
    }

    if (arg3 < 8)
        Resource_CreateNode(context, 0, arg2, arg3, 2, &item_8)[0xb] = 1;
    else if (Resource_MatchPrefix(arg2, "RESOURCE", 8))
        Resource_CreateNode(context, 0, arg2, arg3, 2, &item_8)[0xb] = 1;
    else
    {
        int* data_5 = Resource_CreateNode(context, 0, arg2, 0, 0, &item_8);
        data_5[0xb] = 1;
        char* entity_1 = arg2[2];
        void* context_2 = &arg2[8];

        if (entity_1 > 0)
        {
            context = entity_1;
            bool flag_0_1;

            do
            {
                void* data_7 = *(context_2 + 8);
                int temp_1 = *(context_2 + 4);
                int source_2;
                int dest_1;
                dest_1 = memcpy(0x943c20, context_2 + 0xc, data_7 & 0xfffffffc);
                void* data_8 = data_7 + 0x2b;
                memcpy(dest_1, source_2, data_7 & 3);
                *(data_8 + 0x943bf5) = 0;
                data_8 &= 0xe0;
                int* context_3 = context_2 + data_8;
                context_2 = context_3 + ((temp_1 + 0x1f) & 0xffffffe0);
                HeapList_Insert(&data_5[5], &Resource_CreateNode(0x943c20, 0, context_3, temp_1, 1, &item_8)[2]);
                flag_0_1 = context != 1;
                context -= 1;
            } while (flag_0_1);
        }
    }

    return arg2;
}
