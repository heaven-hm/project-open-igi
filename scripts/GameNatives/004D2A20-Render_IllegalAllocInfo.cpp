// IGI native provenance: Render_IllegalAllocInfo - 0x004D2A20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D2A20-Render_IllegalAllocInfo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D2A20-Render_IllegalAllocInfo.hpp"
int Render_IllegalAllocInfo()

{
    int entity_val;
    int item_4 = entity_val;
    int i_1 = 0;
    int* context_val = &g_b04358;
    int i;

    do
    {
        int* data_1 = *context_val;

        if (data_1)
        {
            int entity_1 = context_val[-1];
            int data_6;
            int temp_1;

            if (!entity_1)
            {
                int* source_1 = g_b12b74;
                temp_1 = *(data_1 + source_1);
                int data_3 = g_b12b68 - 1;
                g_b12b68 = data_3;

                if (temp_1 != data_3)
                {
                    int entity_2 = g_b12b64;
                    data_6 = *(entity_2 + (data_3 << 2)) * g_b12b70 + g_b12b60;
                    int dest_2 = *(entity_2 + (temp_1 << 2));
                    int source_2 = *(source_1 + data_6);
                    *(entity_2 + (temp_1 << 2)) = *(entity_2 + (source_2 << 2));
                    *(g_b12b64 + (source_2 << 2)) = dest_2;
                    *(g_b12b74 + data_6) = temp_1;
                }
            }
            else if (entity_1 == 1)
            {
                int* source_3 = g_b12c54;
                temp_1 = *(data_1 + source_3);
                int data_8 = g_b12c48 - 1;
                g_b12c48 = data_8;

                if (temp_1 != data_8)
                {
                    int entity_5 = g_b12c44;
                    data_6 = *(entity_5 + (data_8 << 2)) * g_b12c50 + g_b12c40;
                    int dest_4 = *(entity_5 + (temp_1 << 2));
                    int source_4 = *(source_3 + data_6);
                    *(entity_5 + (temp_1 << 2)) = *(entity_5 + (source_4 << 2));
                    *(g_b12c44 + (source_4 << 2)) = dest_4;
                    *(g_b12c54 + data_6) = temp_1;
                }
            }
            else if (entity_1 != 2)
            {
                Error_Show("Illegal alloc info");

                while (true)
                    /* nop */
            }
            else
            {
                int* source_5 = g_b16cd4;
                temp_1 = *(data_1 + source_5);
                int data_12 = g_b16cc8 - 1;
                g_b16cc8 = data_12;

                if (temp_1 != data_12)
                {
                    int entity_7 = g_b16cc4;
                    data_6 = *(entity_7 + (data_12 << 2)) * g_b16cd0 + g_b16cc0;
                    int dest_6 = *(entity_7 + (temp_1 << 2));
                    int source_6 = *(source_5 + data_6);
                    *(entity_7 + (temp_1 << 2)) = *(entity_7 + (source_6 << 2));
                    *(g_b16cc4 + (source_6 << 2)) = dest_6;
                    *(g_b16cd4 + data_6) = temp_1;
                }
            }
            *context_val = 0;
            *context_val = 0;
        }

        context_val[-2] = 0xffffffff;
        i = i_1 + 1;
        context_val = &context_val[7];
        i_1 = i;
    } while (i < 0x800);

    return i;
}
