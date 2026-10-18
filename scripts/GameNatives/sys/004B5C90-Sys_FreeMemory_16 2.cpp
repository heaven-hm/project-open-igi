// IGI native provenance: Resource_FreeNode - 0x004B5C90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5C90-Resource_FreeNode.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Resource_FreeNode.h"
int Resource_FreeNode(int* context)

{
    int result = context[9];
    int* temp_1;
    int source_1;

    switch (result)
    {
        case 0:
        {
            int* i_1 = context[5];

            for (int* i = *i_1; i; i = *i)
            {
                Resource_FreeNode(i_1[2]);
                i_1 = i;
            }

            HeapList_Remove(context);
            Sys_OpenFileStream_5(context[0xd]);
            goto label_4b5cf2;
        }
        case 1:
        case 4:
        {
            HeapList_Remove(&context[2]);
            goto label_4b5ced;
        }
        case 2:
        {
            Sys_OpenFileStream_5(context[0xd]);
        label_4b5ced:
            HeapList_Remove(context);
        label_4b5cf2:
            temp_1 = g_943e34;
            result = g_943e28 - 1;
            source_1 = *(temp_1 + context);
            g_943e28 = result;

            if (source_1 != result)
                goto label_4b5d30;
            break;
        }
        case 3:
        {
            HeapList_Remove(context);
            temp_1 = g_943e34;
            result = g_943e28 - 1;
            source_1 = *(temp_1 + context);
            g_943e28 = result;

            if (source_1 != result)
            {
            label_4b5d30:
                int entity_2 = g_943e24;
                result = *(entity_2 + (result << 2)) * g_943e30 + g_943e20;
                int dest_2 = *(entity_2 + (source_1 << 2));
                int temp_2 = *(temp_1 + result);
                *(entity_2 + (source_1 << 2)) = *(entity_2 + (temp_2 << 2));
                *(g_943e24 + (temp_2 << 2)) = dest_2;
                *(g_943e34 + result) = source_1;
            }
            break;
        }
    }

    return result;
}
