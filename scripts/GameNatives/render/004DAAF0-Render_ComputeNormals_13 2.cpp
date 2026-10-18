// IGI native provenance: Render_ComputeNormals_13 - 0x004DAAF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DAAF0-Render_ComputeNormals_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_ComputeNormals_13.h"
int Render_ComputeNormals_13(int context)

{
    int* source_val = g_b041f4;
    int temp_val = *(source_val + context);
    int result = g_b041e8 - 1;
    g_b041e8 = result;

    if (temp_val != result)
    {
        int entity_1 = g_b041e4;
        result = *(entity_1 + (result << 2)) * g_b041f0 + g_b041e0;
        int dest_2 = *(entity_1 + (temp_val << 2));
        int source_1 = *(source_val + result);
        *(entity_1 + (temp_val << 2)) = *(entity_1 + (source_1 << 2));
        *(g_b041e4 + (source_1 << 2)) = dest_2;
        *(g_b041f4 + result) = temp_val;
    }

    return result;
}
