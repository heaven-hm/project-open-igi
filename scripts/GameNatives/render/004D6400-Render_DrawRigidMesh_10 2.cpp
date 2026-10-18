// IGI native provenance: Render_DrawRigidMesh_10 - 0x004D6400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D6400-Render_DrawRigidMesh_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawRigidMesh_10.h"
int Render_DrawRigidMesh_10(int context)

{
    int* source_val = g_a54654;
    int temp_val = *(source_val + context);
    int result = g_a54648 - 1;
    g_a54648 = result;

    if (temp_val != result)
    {
        int entity_1 = g_a54644;
        result = *(entity_1 + (result << 2)) * g_a54650 + g_a54640;
        int dest_2 = *(entity_1 + (temp_val << 2));
        int source_1 = *(source_val + result);
        *(entity_1 + (temp_val << 2)) = *(entity_1 + (source_1 << 2));
        *(g_a54644 + (source_1 << 2)) = dest_2;
        *(g_a54654 + result) = temp_val;
    }

    return result;
}
