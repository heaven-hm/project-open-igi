// IGI native provenance: Particle_FadeParticles - 0x00530580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530580-Particle_FadeParticles.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_FadeParticles.hpp"
int Particle_FadeParticles(int context, short arg2, short arg3)

{
    int entity_val = arg2 * 3;
    short* source_val = context + (entity_val << 1);
    int entity_2 = *(context + (entity_val << 1)) * 3;
    void* entity_3 = context + (entity_2 << 1);

    if (*(context + (entity_2 << 1) + 2) != arg2)
        *(entity_3 + 4) = arg3;
    else
        *(entity_3 + 2) = arg3;

    int* dest_val = source_val;
    int* entity_6 = context + arg3 * 6;
    *entity_6 = *dest_val;
    entity_6[1] = dest_val[1];
    *(context + *(entity_6 + 2) * 6) = arg3;
    *(context + entity_6[1] * 6) = arg3;
    *source_val = 0;
    return context;
}
