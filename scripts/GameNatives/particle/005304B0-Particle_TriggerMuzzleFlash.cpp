// IGI native provenance: Particle_TriggerMuzzleFlash - 0x005304B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005304B0-Particle_TriggerMuzzleFlash.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_TriggerMuzzleFlash.hpp"
short* Particle_TriggerMuzzleFlash(int context, short arg2)

{
    int data_1 = arg2 * 3;
    void* result = context + (data_1 << 1);

    if (!*(context + (data_1 << 1)))
        return result;

    short entity_val = *(result + 4);

    if (!entity_val)
        return Particle_RenderTrails(context, arg2, *(result + 2));

    if (!*(result + 2))
        return Particle_RenderTrails(context, arg2, entity_val);

    short data_4 = Particle_ResetEmitters(context, arg2);
    Particle_TriggerMuzzleFlash(context, data_4);
    return Particle_FadeParticles(context, arg2, data_4);
}
