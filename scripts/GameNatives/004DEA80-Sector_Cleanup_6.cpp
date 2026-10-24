// IGI native provenance: Sector_Cleanup_6 - 0x004DEA80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DEA80-Sector_Cleanup_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DEA80-Sector_Cleanup_6.hpp"
int Sector_Cleanup_6(void* context, int arg2)

{
    Render_SkinBones_15(context);
    void* data_val = *(context + 0x148);
    int result;

    if (!data_val)
        result = g_afa7e0;
    else
    {
        int* i_2 = *(data_val + 8);

        if (!*i_2)
            result = g_afa7e0;
        else
        {
            int* i = i_2;

            if (!i)
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
                    int entity_5 = 0;
                    entity_5 = i[7];
                    int data_7 = *(((data_3 * 0x180 + entity_5) << 2) + &g_a96ae0);

                    if (data_7)
                        data_7(i, arg2);

                    result = g_afa7e0 - 1;
                    g_afa7e0 = result;
                    i = *((result << 2) + &g_afa6e0);
                } while (i);
            }
        }
    }

    void* entity_6 = *(context + 0xe8);

    if (entity_6)
    {
        int* i_1 = *(entity_6 + 8);

        if (*i_1 && i_1)
        {
            do
            {
                int* temp_2 = *i_1;
                void* entity_10;

                if (!temp_2)
                    entity_10 = nullptr;
                else
                {
                    int entity_7 = *temp_2;
                    int entity_8 = -(entity_7);
                    entity_10 = (entity_8 - entity_8) & temp_2;
                }

                *((result << 2) + &g_afa6e0) = entity_10;
                g_afa7e0 = result + 1;
                unsigned int data_11 = DynObj_LinkDynamicCube_12();
                int data_12 = 0;
                data_12 = i_1[7];
                int data_13 = *(((data_11 * 0x180 + data_12) << 2) + &g_a96ae0);

                if (data_13)
                    data_13(i_1, arg2);

                result = g_afa7e0 - 1;
                g_afa7e0 = result;
                i_1 = *((result << 2) + &g_afa6e0);
            } while (i_1);
        }
    }

    return result;
}
