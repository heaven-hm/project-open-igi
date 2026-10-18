// IGI native provenance: Physics_SolveConstraints_9 - 0x00509A40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00509A40-Physics_SolveConstraints_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_SolveConstraints_9.h"
int __stdcall Physics_SolveConstraints_9(void* context)

{
    int item_3c = *(context + 4);
    int item_38 = *(context + 8);
    int item_34 = *(context + 0xc);
    int* data_2 = g_a7b4f0;
    int item_30 = *(context + 0x10);
    void* item_48 = (g_a7b638 << 2) + &g_a7b2e0;

    if ((*(*data_2 + 0xc))(data_2, &item_3c) < 0)
    {
        Error_Show("Failed to create device.");

        while (true)
            /* nop */

        return;
    }

    int entity_3 = g_a7b638;
    int context_val;
    int item_4c_1 = context_val;
    int source_val;
    int item_50_1 = source_val;
    int* data_4 = *((entity_3 << 2) + &g_a7b2e0);
    int data_5 = (**data_4)(data_4, 0x5349c0, (entity_3 << 2) + &g_a7b4e0);
    int* data_6 = *((g_a7b638 << 2) + &g_a7b2e0);
    (*(*data_6 + 8))(data_6);

    if (data_5 >= 0)
    {
        int* data_7 = *((g_a7b638 << 2) + &g_a7b4e0);
        int* data_9;

        if ((*(*data_7 + 0x2c))(data_7, 0x534c38) >= 0)
        {
            int* source_3 = *((g_a7b638 << 2) + &g_a7b4e0);

            if ((*(*source_3 + 0x34))(source_3, FX_ExplosionSpawn_22(), 5) < 0)
            {
                int* data_14 = *((g_a7b638 << 2) + &g_a7b4e0);
                (*(*data_14 + 8))(data_14);
                *((g_a7b638 << 2) + &g_a7b4e0) = 0;
                return 1;
            }

            int item_18 = 0x18;
            int item_14_1 = 0x10;
            int item_c_1 = 1;
            int item_8_1 = 0xffffff80;
            int item_4_1 = 0x80;
            int* source_5 = &g_54e8ac;

            while (true)
            {
                int item_10_1 = source_5[-1];
                int* data_17 = *((g_a7b638 << 2) + &g_a7b2e0);
                int data_18 = (*(*data_17 + 0x18))(data_17, 4, &item_18);

                if (data_18)
                {
                    if (data_18 != 0x80070002)
                        break;

                    *source_5 = 0;
                }

                source_5 = &source_5[2];

                if (source_5 >= "d not get capabilities for one or more joysticks .")
                {
                    int data_19 = g_a7b638;
                    int item_2c = 0x14;
                    int item_28_1 = 0x10;
                    int* data_20 = *((data_19 << 2) + &g_a7b2e0);
                    int item_24_1 = 0;
                    int item_20_1 = 0;
                    int item_1c_1 = 1;
                    (*(*data_20 + 0x18))(data_20, 9, &item_2c);
                    int item_20_2 = 0;
                    int item_1c_2 = 0x2710;
                    int* data_22 = *((g_a7b638 << 2) + &g_a7b2e0);
                    (*(*data_22 + 0x18))(data_22, 6, &item_2c);
                    int item_20_3 = 1;
                    int item_1c_3 = g_a7b63c;
                    int* source_6 = &g_54e8a8;

                    while (true)
                    {
                        if (source_6[1])
                        {
                            int item_24_2 = *source_6;
                            int* data_24 = *((g_a7b638 << 2) + &g_a7b2e0);

                            if ((*(*data_24 + 0x18))(data_24, 5, &item_2c))
                                break;
                        }

                        source_6 = &source_6[2];

                        if (source_6 >= "Could not get capabilities for one or more joysticks .")
                        {
                            g_a7b638 += 1;
                            return 1;
                        }
                    }

                    break;
                }
            }

            data_9 = *((g_a7b638 << 2) + &g_a7b4e0);
        }
        else
            data_9 = *((g_a7b638 << 2) + &g_a7b4e0);

        (*(*data_9 + 8))(data_9);
        *((g_a7b638 << 2) + &g_a7b4e0) = 0;
        return 1;
    }

    Error_Show("Failed to obtain interface.");

    while (true)
        /* nop */
}
