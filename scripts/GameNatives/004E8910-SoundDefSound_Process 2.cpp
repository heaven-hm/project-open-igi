// IGI native provenance: SoundDefSound_Process - 0x004E8910 - CFG score 71.4% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E8910-SoundDefSound_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefSound_Process.h"
int SoundDefSound_Process(void* context, int* arg2)

{
    int* context_val = arg2;
    int i = 0;

    if (context_val[1] > 0)
    {
        do
        {
            void* dest_1 = &g_54da30;
            int j = 0xffffffff;

            while (j)
            {
                bool flag_1_1 = 0 != *dest_1;
                dest_1 += 1;
                j -= 1;

                if (!flag_1_1)
                    break;
            }

            int entity_1 = ~j;
            int dest_3 = *context_val;
            int j_1 = 0xffffffff;

            while (j_1)
            {
                bool flag_2_1 = 0 != *dest_3;
                dest_3 += 1;
                j_1 -= 1;

                if (!flag_2_1)
                    break;
            }

            context_val = arg2;
            int source_2;
            int dest_5;
            dest_5 = memcpy(dest_3 - 1, dest_1 - entity_1, entity_1 & 0xfffffffc);
            i += 1;
            memcpy(dest_5, source_2, entity_1 & 3);
        } while (i < context_val[1]);
    }

    char* data_3 = "TRUE";

    if (!*(context + 0xdc))
        data_3 = "FALSE";

    char* item_41c = data_3;
    double item_424 = *(context + 0x124);
    int item_428 = *(context + 0xc0);
    int item_42c = *(context + 0xbc);
    int item_430 = *(context + 0xb8);
    double item_438 = *(context + 0xd4);
    double item_440 = *(context + 0xd0);
    double item_448 = *(context + 0xc4);
    int item_44c = *(context + 0xb4);
    int item_450 = *(context + 0xb0);
    int item_454 = *(context + 0xac);
    int item_458 = *(context + 0xa8);
    int item_45c = *(context + 0xa4);
    int item_460 = *(context + 0xa0);
    void* item_464 = context + 0xff;
    void* item_468 = context + 0xdf;
    char* item_46c = "DefineSound";
    void item_408;
    Game_DataSymbolLoad(&item_408,
        "%s("%s", "%s", %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %d, %d, %d, %8.08f, %s");
    void* dest_6 = &item_408;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_3_1 = 0 != *dest_6;
        dest_6 += 1;
        i_1 -= 1;

        if (!flag_3_1)
            break;
    }

    int entity_10 = ~i_1;
    int dest_8 = *arg2;
    int i_2 = 0xffffffff;

    while (i_2)
    {
        bool flag_4_1 = 0 != *dest_8;
        dest_8 += 1;
        i_2 -= 1;

        if (!flag_4_1)
            break;
    }

    int source_4;
    int dest_10;
    dest_10 = memcpy(dest_8 - 1, dest_6 - entity_10, entity_10 & 0xfffffffc);
    memcpy(dest_10, source_4, entity_10 & 3);
    char* dest_11;

    if (!arg2[1])
        dest_11 = ");\r\n";
    else
    {
        void* entity_16 = *(*(context + 0x14) + 0x10);
        int data_9 = *(entity_16 + 4);
        int data_10 = -(data_9);

        dest_11 = context == ((data_10 - data_10) & entity_16) ? &g_547a40 : "),\r\n";
    }

    int i_3 = 0xffffffff;

    while (i_3)
    {
        bool flag_5_1 = 0 != *dest_11;
        dest_11 = &dest_11[1];
        i_3 -= 1;

        if (!flag_5_1)
            break;
    }

    int entity_17 = ~i_3;
    int dest_13 = *arg2;
    int i_4 = 0xffffffff;

    while (i_4)
    {
        bool flag_6_1 = 0 != *dest_13;
        dest_13 += 1;
        i_4 -= 1;

        if (!flag_6_1)
            break;
    }

    int source_6;
    int dest_15;
    dest_15 = memcpy(dest_13 - 1, dest_11 - entity_17, entity_17 & 0xfffffffc);
    memcpy(dest_15, source_6, entity_17 & 3);
    return 0;
}
