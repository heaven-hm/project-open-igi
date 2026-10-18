// IGI native provenance: Particle_ResetEmitters - 0x005305F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005305F0-Particle_ResetEmitters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_ResetEmitters.hpp"
int Particle_ResetEmitters(int context, short arg2)

{
    short entity_val = *(context + arg2 * 6 + 2);
    int data_2 = entity_val * 3;
    data_2 = *(context + (data_2 << 1) + 4);

    while (data_2)
    {
        entity_val = data_2;
        data_2 = entity_val * 3;
        data_2 = *(context + (data_2 << 1) + 4);
    }

    return entity_val;
}
