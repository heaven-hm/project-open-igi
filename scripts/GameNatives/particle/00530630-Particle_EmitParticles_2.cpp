// IGI native provenance: Particle_EmitParticles_2 - 0x00530630 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530630-Particle_EmitParticles_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_EmitParticles_2.hpp"
int Particle_EmitParticles_2(int context, int arg2, short arg3, short* arg4)

{
    if (!arg3)
        return 0;

    int data_1 = context;
    int entity_val = arg3;
    data_1 = *(data_1 + 0x3004);
    int i_2 = 0;
    short item_c = data_1;
    short* entity_4;
    short temp_2;
    int i_1;
    int source_1;

    while (true)
    {
        int temp_1 = entity_val;
        int i = 0;
        int entity_1 = data_1;
        int context_1;

        do
        {
            context_1 = 0;
            context_1 = (entity_1 & 0x7ff)[arg2];
            data_1 = nullptr;
            data_1 = (temp_1 & 0x7ff)[arg2];

            if (data_1 != context_1)
                break;

            i += 1;
            entity_1 += 1;
            temp_1 += 1;
        } while (i < 0x82);

        i_1 = i_2;

        if (i < i_1)
            temp_2 = item_c;
        else
        {
            temp_2 = item_c;
            i_1 = i;
            i_2 = i_1;
            *arg4 = temp_2;

            if (i >= 0x82)
            {
                Particle_FadeParticles(context, temp_2, arg3);
                return i;
            }
        }

        int entity_3 = temp_2;

        if (data_1 - context_1 < 0)
        {
            source_1 = context;
            entity_4 = source_1 + entity_3 * 6 + 2;
        }
        else
        {
            source_1 = context;
            data_1 = entity_3 * 3;
            entity_4 = source_1 + (data_1 << 1) + 4;
        }

        data_1 = *entity_4;

        if (!data_1)
            break;

        item_c = data_1;
    }

    *entity_4 = arg3;
    short* entity_8 = source_1 + entity_val * 6;
    *entity_8 = temp_2;
    entity_8[2] = 0;
    entity_8[1] = 0;
    return i_1;
}
