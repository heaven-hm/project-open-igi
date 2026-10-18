// IGI native provenance: Qtask_UpdateList - 0x00401B20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401B20-Qtask_UpdateList.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Qtask_UpdateList.h"
int Qtask_UpdateList(int* context)

// Sig: void Qtask_UpdateList(int* p1)
// Note: Update all QTask list, Params: p1 ??
{
    void* data_val = *context;

    if (data_val)
        *(data_val + 0x18) -= 1;

    int* source_val = g_567c4c;
    int result = g_567c40 - 1;
    int entity_val = *(source_val + context);
    g_567c40 = result;

    if (entity_val != result)
    {
        int temp_1 = g_567c3c;
        result = *(temp_1 + (result << 2)) * g_567c48 + g_567c38;
        int dest_2 = *(temp_1 + (entity_val << 2));
        int source_1 = *(source_val + result);
        *(temp_1 + (entity_val << 2)) = *(temp_1 + (source_1 << 2));
        *(g_567c3c + (source_1 << 2)) = dest_2;
        *(g_567c4c + result) = entity_val;
    }

    return result;
}
