// IGI native provenance: Particle_UpdateParticlePhysics_2 - 0x00530740 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530740-Particle_UpdateParticlePhysics_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_UpdateParticlePhysics_2.hpp"
int Particle_UpdateParticlePhysics_2(int* context , int* arg2)

{
    int entity_val;
    int item_4_1 = entity_val;
    char* dest_val = *arg2;
    char* data_1 = Mem_Alloc(0x800, 4);
    char* source_val = 1;

    while (true)
    {
        if (!Particle_FadeParticles_2(context))
        {
            int flagP2 = Particle_ResetEmitters_2(context, 0xb);
            int item_4 = flagP2;

            if (!flagP2)
                break;

            int data_5 = Particle_ResetEmitters_2(context, 7);
            int temp_1 = 0;

            if (data_5 + 2 >= 0)
            {
                while (true)
                {
                    dest_val = &dest_val[1];
                    char entity_3 = data_1[(flagP2 + temp_1) & 0x7ff];
                    dest_val[0xffffffff] = entity_3;
                    *(&source_val[1] + data_1 - 1) = entity_3;
                    source_val = &source_val[1] & 0x7ff;
                    temp_1 += 1;

                    if (temp_1 > data_5 + 2)
                        break;

                    flagP2 = item_4;
                }
            }
        }
        else
        {
            char data_3 = Particle_ResetEmitters_2(context, 8);
            *dest_val = data_3;
            dest_val = &dest_val[1];
            *(source_val + data_1) = data_3;
            source_val = &source_val[1] & 0x7ff;
        }
    }

    *arg2 = dest_val;
    return Sys_ExecuteBytecode_2(data_1);
}
