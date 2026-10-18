// IGI native provenance: ParticleSystemDynCubeObj_LoadParameters - 0x00530CD0 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00530CD0-ParticleSystemDynCubeObj_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00530CD0-ParticleSystemDynCubeObj_LoadParameters.hpp"
int ParticleSystemDynCubeObj_LoadParameters(void* context)

{
    if (*(context + 0xb4))
    {
        memcpy(context + 0x88, &g_bcab08, 0x18);
        *(context + 0xb4) = 0;
    }

    *(context + 0xa0) = g_bcab08 - *(context + 0x88);
    int data_1 = *(context + 0xac);
    *(context + 0xa4) = g_bcab10 - *(context + 0x90);
    *(context + 0xa8) = g_bcab18 - *(context + 0x98);
    memcpy(context + 0x88, &g_bcab08, 0x18);

    if (data_1)
        data_1(context);

    int item_74 = 0;
    int data_3 = ~*(context + 0x70);
    int source_1 = g_bcab10 & data_3;
    int item_40 = source_1;
    int entity_1 = g_bcab18 & data_3;
    int result = *(context + 0x78);
    int entity_2 = *(context + 0x6c);
    int result_1 = result;

    if (entity_2 * 2 - 1 > 0)
    {
        int entity_3 = entity_2 * result;
        int item_7c_1 = entity_3;
        int dest_5 = -(result);
        int item_80_1 = (g_bcab08 & data_3) + entity_3;
        int result_2 = entity_2 * 2 - 1;

        while (true)
        {
            int item_84_1 = entity_3 + source_1;
            int dest_7 = -(result);
            int item_64_1 = entity_2 * 2 - 1;

            while (true)
            {
                int data_4;
                int temp_6;
                temp_6 = HIGHD(result);
                data_4 = LOWD(result);
                double item_58_1 = item_80_1;
                double item_50_3 = item_84_1;
                int source_5 = ((data_4 - temp_6) >> 1) + entity_3 + entity_1;
                int item_70_1 = source_5;
                int item_68_1 = entity_2 * 2 - 1;
                bool flag_2_1;

                do
                {
                    int item_1c_1 = *(&item_58_1 + 4);
                    long double fpu_val_7_11 = item_70_1;
                    float temp_8 = *(context + 0x84);
                    int item_18_1 = item_50_3;
                    int item_20 = item_58_1;
                    item_70_1 = fpu_val_7_11;
                    double item_10_1 = fpu_val_7_11;
                    int item_14_1 = *(&item_50_3 + 4);

                    if (Particle_FadeParticles_3(&item_20, temp_8))
                    {
                        int item_38 = item_58_1;
                        int item_34_1 = *(&item_58_1 + 4);
                        int item_2c_1 = *(&item_50_3 + 4);
                        int item_30_1 = item_50_3;
                        int item_28_1 = item_70_1;
                        int item_6c;
                        int item_24_1 = item_6c;
                        (*(context + 0xb0))(context, &item_38);
                        item_74 += 1;
                    }

                    source_5 += dest_7;
                    flag_2_1 = item_68_1 != 1;
                    item_70_1 = source_5;
                    item_68_1 -= 1;
                } while (flag_2_1);
                bool flag_3_1 = item_64_1 != 1;
                item_84_1 += dest_7;
                item_64_1 -= 1;

                if (!flag_3_1)
                    break;

                entity_3 = item_7c_1;
                result = result_1;
            }

            result = result_2 - 1;
            bool flag_4_1 = result_2 != 1;
            item_80_1 += dest_5;
            result_2 = result;

            if (!flag_4_1)
                break;

            source_1 = item_40;
            entity_3 = item_7c_1;
            result = result_1;
        }
    }

    return result;
}
