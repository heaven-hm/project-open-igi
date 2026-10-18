// IGI native provenance: Particle_EmitParticles_3 - 0x00530950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530950-Particle_EmitParticles_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_EmitParticles_3.hpp"
int Particle_EmitParticles_3(int context, int arg2)

{
    *((context << 2) + &g_a845d8) = arg2;
    return arg2;
}
