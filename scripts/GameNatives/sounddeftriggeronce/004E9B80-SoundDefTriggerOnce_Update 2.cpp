// IGI native provenance: SoundDefTriggerOnce_Update - 0x004E9B80 - CFG score 71.4% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E9B80-SoundDefTriggerOnce_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefTriggerOnce_Update.h"
int SoundDefTriggerOnce_Update(void* context, int* arg2)

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
                bool flag_0_1 = 0 != *dest_1;
                dest_1 += 1;
                j -= 1;

                if (!flag_0_1)
                    break;
            }

            int entity_1 = ~j;
            int dest_3 = *context_val;
            int j_1 = 0xffffffff;

            while (j_1)
            {
                bool flag_1_1 = 0 != *dest_3;
                dest_3 += 1;
                j_1 -= 1;

                if (!flag_1_1)
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

    char* entity_6 = "TRUE";

    if (!*(context + 0xe1))
        entity_6 = "FALSE";

    char* data_3 = "TRUE";

    if (!*(context + 0xe0))
        data_3 = "FALSE";

    char* item_41c = entity_6;
    char* item_420 = data_3;
    int item_424 = *(context + 0xdc);
    int item_428 = *(context + 0xd8);
    int item_42c = *(context + 0xd4);
    int item_430 = *(context + 0xd0);
    double item_438 = *(context + 0x12c);
    double item_440 = *(context + 0xc8);
    double item_448 = *(context + 0xc4);
    double item_450 = *(context + 0xb8);
    int item_454 = *(context + 0xb4);
    int item_458 = *(context + 0xb0);
    int item_45c = *(context + 0xac);
    int item_460 = *(context + 0xa8);
    int item_464 = *(context + 0xa4);
    int item_468 = *(context + 0xa0);
    void* item_46c = context + 0x103;
    void* item_470 = context + 0xe3;
    char* item_474 = "DefineTriggerOnce";
    void item_408;
    Game_DataSymbolLoad(&item_408,
        "%s("%s", "%s", %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %d, %d, %d, %d, %s, %s");
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

    int entity_11 = ~i_1;
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
    dest_10 = memcpy(dest_8 - 1, dest_6 - entity_11, entity_11 & 0xfffffffc);
    memcpy(dest_10, source_4, entity_11 & 3);
    char* dest_11;

    if (!arg2[1])
        dest_11 = ");\r\n";
    else
    {
        void* entity_17 = *(*(context + 0x14) + 0x10);
        int data_10 = *(entity_17 + 4);
        int data_11 = -(data_10);

        dest_11 = context == ((data_11 - data_11) & entity_17) ? &g_547a40 : "),\r\n";
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

    int entity_18 = ~i_3;
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
    dest_15 = memcpy(dest_13 - 1, dest_11 - entity_18, entity_18 & 0xfffffffc);
    memcpy(dest_15, source_6, entity_18 & 3);
    return 0;
}
