// IGI native provenance: Particle_RenderTrails_2 - 0x00530850 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530850-Particle_RenderTrails_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_RenderTrails_2.hpp"
int* Particle_RenderTrails_2(int* context, int arg2, int* arg3)

{
    int* result = arg3;
    int i = 1 << ((result - 1) & 0x1f);

    if (i)
    {
        result = context;
        int context_val;
        int item_4_1 = context_val;

        do
        {
            if (arg2 & i)
                result[2] |= result[1];

            int entity_3 = result[1] >> 1;
            result[1] = entity_3;

            if (!entity_3)
            {
                **result = result[2];
                int entity_6 = *result + 1;
                result[2] = 0;
                *result = entity_6;
                result[1] = 0x80;
            }

            i u>>= 1;
        } while (i);
    }

    return result;
}
