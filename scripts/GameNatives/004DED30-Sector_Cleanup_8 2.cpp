// IGI native provenance: Sector_Cleanup_8 - 0x004DED30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DED30-Sector_Cleanup_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sector_Cleanup_8.h"
int Sector_Cleanup_8(void* context, int* arg2)

{
    Render_SetupLighting_14(context);
    DynObj_RegisterSymbol_9(*arg2, context);
    int* i = *(context + 8);
    int result;

    if (!*i || !i)
        result = g_afa7e0;
    else
    {
        result = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* entity_4;

            if (!temp_1)
                entity_4 = nullptr;
            else
            {
                int entity_1 = *temp_1;
                int entity_2 = -(entity_1);
                entity_4 = (entity_2 - entity_2) & temp_1;
            }

            *((result << 2) + &g_afa6e0) = entity_4;
            g_afa7e0 = result + 1;
            unsigned int data_3 = DynObj_LinkDynamicCube_12();
            int temp_2 = 0;
            temp_2 = i[7];
            int data_4 = *(((data_3 * 0x180 + temp_2) << 2) + &g_a96ae0);

            if (data_4)
                data_4(i, arg2);

            result = g_afa7e0 - 1;
            g_afa7e0 = result;
            i = *((result << 2) + &g_afa6e0);
        } while (i);
    }

    void* entity_8 = *(context + 0xe8);

    if (entity_8)
    {
        int* i_1 = *(entity_8 + 8);

        if (*i_1 && i_1)
        {
            do
            {
                int* temp_3 = *i_1;
                void* entity_12;

                if (!temp_3)
                    entity_12 = nullptr;
                else
                {
                    int entity_9 = *temp_3;
                    int entity_10 = -(entity_9);
                    entity_12 = (entity_10 - entity_10) & temp_3;
                }

                *((result << 2) + &g_afa6e0) = entity_12;
                g_afa7e0 = result + 1;
                unsigned int data_8 = DynObj_LinkDynamicCube_12();
                int entity_13 = 0;
                entity_13 = i_1[7];
                int data_12 = *(((data_8 * 0x180 + entity_13) << 2) + &g_a96ae0);

                if (data_12)
                    data_12(i_1, arg2);

                result = g_afa7e0 - 1;
                g_afa7e0 = result;
                i_1 = *((result << 2) + &g_afa6e0);
            } while (i_1);
        }
    }

    return result;
}
