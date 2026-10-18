// IGI native provenance: Render_ClipMesh_2 - 0x004D0DE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0DE0-Render_ClipMesh_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_ClipMesh_2.h"
int* Render_ClipMesh_2(void* context)

{
    int* result = *(context + 0xc);
    int i_1 = result[4];

    if (i_1)
    {
        result = result[5];

        if (i_1 > 0)
        {
            result = &result[1];
            int i;

            do
            {
                int entity_1 = result[1];
                int source_1 = *result;
                result = &result[5];
                i = i_1;
                i_1 -= 1;
                result[-3] = *(*(context + 0x10) + (entity_1 << 2));
                int temp_3 = *(context + 0x10);
                *(temp_3 + (entity_1 << 2)) = *(temp_3 + (source_1 << 2));
            } while (i != 1);
        }
    }

    return result;
}
