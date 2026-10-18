// IGI native provenance: Display_SetDisplayMode_19 - 0x0049BD30 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0049BD30-Display_SetDisplayMode_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetDisplayMode_19.h"
int Display_SetDisplayMode_19()

{
    int i = g_72350c;
    int i_1 = 0;

    if (i > 0)
    {
        int* flagP1 = &g_7ac0a0;

        do
        {
            int* data_val = *flagP1;
            (*(*data_val + 8))(data_val);
            int* data_1 = g_5ca110;
            *flagP1 = 0;
            (*(*data_1 + 0x14))(data_1, &flagP1[1], flagP1, 0);
            int* context_1 = flagP1[6];
            int* source_1 = *context_1;
            int* item_c_1 = source_1;

            if (source_1)
            {
                while (true)
                {
                    int* data_2 = *context_1[4];
                    int item_4;

                    if (!(*(*data_2 + 0xc))(data_2, 0x20, &item_4, 0))
                    {
                        int data_4 = flagP1[5];
                        int dest_1 = 0;
                        dest_1 = context_1[3];
                        int entity_4 = 0;
                        entity_4 = *(context_1 + 0xe);
                        int entity_5 = entity_4 * data_4;
                        int source_3;
                        int dest_4;
                        dest_4 =
                            memcpy(dest_1 * data_4 + item_4, context_1[5], entity_5 & 0xfffffffc);
                        memcpy(dest_4, source_3, entity_5 & 3);
                        int* data_7 = *context_1[4];
                        (*(*data_7 + 0x10))(data_7);
                        source_1 = item_c_1;
                    }

                    context_1 = source_1;
                    int* source_4 = *source_1;
                    item_c_1 = source_4;

                    if (!source_4)
                        break;

                    source_1 = item_c_1;
                }
            }

            i = i_1 + 1;
            flagP1 = &flagP1[9];
            i_1 = i;
        } while (i < g_72350c);
    }

    return i;
}
