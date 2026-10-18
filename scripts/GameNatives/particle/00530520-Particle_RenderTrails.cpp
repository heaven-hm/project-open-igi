// IGI native provenance: Particle_RenderTrails - 0x00530520 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530520-Particle_RenderTrails.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_RenderTrails.hpp"
short* Particle_RenderTrails(int context, short arg2, short arg3)

{
    int data_1 = arg2 * 3;
    short* result = context + (data_1 << 1);
    *(context + arg3 * 6) = *(context + (data_1 << 1));
    int temp_3 = *result * 3;
    void* entity_1 = context + (temp_3 << 1);

    if (*(context + (temp_3 << 1) + 4) != arg2)
    {
        *(entity_1 + 2) = arg3;
        *result = 0;
        return result;
    }

    *(entity_1 + 4) = arg3;
    *result = 0;
    return result;
}
