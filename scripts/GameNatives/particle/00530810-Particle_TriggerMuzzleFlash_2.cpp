// IGI native provenance: Particle_TriggerMuzzleFlash_2 - 0x00530810 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530810-Particle_TriggerMuzzleFlash_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_TriggerMuzzleFlash_2.hpp"
int* Particle_TriggerMuzzleFlash_2(int* context, int arg2)

{
    if (arg2)
        context[2] |= context[1];

    int entity_3 = context[1] >> 1;
    context[1] = entity_3;

    if (!entity_3)
    {
        **context = context[2];
        int entity_6 = *context + 1;
        context[2] = 0;
        *context = entity_6;
        context[1] = 0x80;
    }

    return context;
}
