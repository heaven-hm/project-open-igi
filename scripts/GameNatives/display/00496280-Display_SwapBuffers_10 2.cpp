// IGI native provenance: Display_SwapBuffers_10 - 0x00496280 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496280-Display_SwapBuffers_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SwapBuffers_10.h"
int Display_SwapBuffers_10(char* context, char* arg2, int arg3, short* arg4)

{
    short* flagP = arg4;
    int source_val = flagP[3];
    short temp_val = flagP[2];
    int item_18 = flagP[1];
    int entity_1 = flagP[4];
    int item_8 = *((source_val << 1) + &g_543930);
    char* item_1c = arg2;
    int item_4 = *((entity_1 << 1) + &g_543930);
    short temp_3;
    temp_3 = flagP[5];
    arg2 = context;
    int item_10 = source_val;

    if (temp_3)
    {
        char* temp_4 = nullptr;
        temp_4 = *context;
        context = temp_4;
        arg2 = &context[1];
    }

    int result = arg3;

    if (result > 0)
    {
        arg3 = result;
        bool flag_1_1;

        do
        {
            int i = 0;

            if (*flagP > 0)
            {
                do
                {
                    bool entity_3 = flagP[5];
                    char* data_3;

                    if (!entity_3)
                    {
                        char* data_4 = nullptr;
                        data_4 = *arg2;
                        context = data_4;
                        arg2 = &arg2[1];
                        data_3 = data_4 >> 4;
                    }
                    else
                        data_3 = context;

                    void* data_5 = data_3 & 0xf;
                    flagP[5] = !entity_3;
                    int entity_4 = (&item_10)[i];
                    int temp_7 = *(data_5 + 0x543920);
                    int entity_5 = entity_4 + temp_7;
                    (&item_10)[i] = entity_5;
                    int context_1;

                    if (entity_4 + temp_7 >= 0)
                    {
                        context_1 = 0x58;

                        if (entity_5 <= 0x58)
                            context_1 = entity_5;
                    }
                    else
                        context_1 = 0;

                    int source_1 = (&item_8)[i];
                    char data_6 = data_5 & 7;
                    (&item_10)[i] = context_1;
                    int entity_7 = source_1 >> 3;

                    if (data_6 & 4)
                        entity_7 += source_1;

                    if (data_6 & 2)
                    {
                        entity_7 += source_1 >> 1;
                        flagP = arg4;
                    }

                    if (data_6 & 1)
                        entity_7 += source_1 >> 2;

                    int data_7 = (&item_18)[i];
                    int data_8;

                    data_8 = !(data_5 & 8) ? data_7 + entity_7 : data_7 - entity_7;

                    (&item_18)[i] = data_8;

                    if (data_8 < 0xffff8000)
                        data_8 = 0xffff8000;
                    else if (data_8 > 0x7fff)
                        data_8 = 0x7fff;

                    (&item_18)[i] = data_8;
                    short entity_2 = (&item_18)[i];
                    (&item_8)[i] = *((context_1 << 1) + &g_543930);
                    *item_1c = entity_2;
                    i += 1;
                    item_1c = &item_1c[2];
                } while (i < *flagP);
            }

            result = arg3 - 1;
            flag_1_1 = arg3 != 1;
            arg3 = result;
        } while (flag_1_1);
        source_val = item_10;
    }

    result = item_18;
    flagP[3] = source_val;
    flagP[1] = result;
    flagP[2] = temp_val;
    flagP[4] = entity_1;
    return result;
}
