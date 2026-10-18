// IGI native provenance: Particle_ResetEmitters_5 - 0x00532344 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00532344-Particle_ResetEmitters_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_ResetEmitters_5.hpp"
char* Particle_ResetEmitters_5(char* context, char* arg2, int arg3, int arg4)

{
    char* result = context;
    char* dest_val = arg2;

    if (dest_val > result)
    {
        do
        {
            char* i_1 = context;

            for (char* i = &result[arg3]; i <= dest_val; i = &i[arg3])
            {
                if (arg4(i, i_1) > 0)
                    i_1 = i;
            }

            result = Particle_EmitParticles_6(i_1, dest_val, arg3);
            dest_val -= arg3;
        } while (dest_val > context);
    }

    return result;
}
