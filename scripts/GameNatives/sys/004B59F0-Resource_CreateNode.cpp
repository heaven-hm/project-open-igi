// IGI native provenance: Resource_CreateNode - 0x004B59F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B59F0-Resource_CreateNode.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B59F0-Resource_CreateNode.hpp"
int* Resource_CreateNode(char* context, int arg2, int* arg3, int arg4, int arg5, int* arg6)

{
    int data_10 = g_943e28;
    int* result = *(g_943e24 + (data_10 << 2)) * g_943e30 + g_943e20;
    *(g_943e34 + result) = data_10;
    g_943e28 += 1;
    Resource_CanonicalizePath(&result[0xe], context);
    char i = result[0xe];

    if (i)
    {
        char* data_3 = &result[0xe];

        do
        {
            char entity_3 = 0x2f;

            if (i != 0x5c)
                entity_3 = *data_3;

            *data_3 = entity_3;
            i = data_3[1];
            data_3 = &data_3[1];
        } while (i);
    }

    int* result_1 = arg3;

    if (!result_1)
        result_1 = result;

    result[0xd] = result_1;
    result[0xa] = arg4;
    result[9] = arg5;
    result[0xc] = 1;
    result[0xb] = 0;
    result[0x8e] = arg2;
    result[8] = Resource_CalculatePathHash(&result[0xe]);
    result[1] = 0;
    *result = 0;
    result[3] = 0;
    result[2] = 0;
    result[5] = &result[6];
    result[7] = &result[5];
    result[6] = 0;
    HeapList_Insert(&g_943e38, result);
    result[4] = result;

    if (!arg6)
    {
        result[0x8f] = 0;
        result[0x90] = 0;
        return result;
    }

    result[0x8f] = *arg6;
    result[0x90] = arg6[1];
    return result;
}
