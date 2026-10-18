// IGI native provenance: SoundDefGraph_Process - 0x004EA8D0 - CFG score 74.4% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004EA8D0-SoundDefGraph_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefGraph_Process.h"
int SoundDefGraph_Process(void* context, int* arg2)

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
                bool flag_2_1 = 0 != *dest_1;
                dest_1 += 1;
                j -= 1;

                if (!flag_2_1)
                    break;
            }

            int entity_1 = ~j;
            int dest_3 = *context_val;
            int j_1 = 0xffffffff;

            while (j_1)
            {
                bool flag_3_1 = 0 != *dest_3;
                dest_3 += 1;
                j_1 -= 1;

                if (!flag_3_1)
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

    int item_424 = *(context + 0xe0);
    int item_428 = *(context + 0xe4);
    int item_42c = *(context + 0xdc);
    int item_430 = *(context + 0xd8);
    double item_438 = *(context + 0xcc);
    double item_440 = *(context + 0xc8);
    double item_448 = *(context + 0xb8);
    int item_44c = *(context + 0xb4);
    int item_450 = *(context + 0xb0);
    int item_454 = *(context + 0xac);
    int item_458 = *(context + 0xa8);
    int item_45c = *(context + 0xa4);
    int item_460 = *(context + 0xa0);
    void* item_464 = context + 0x160;
    void* item_468 = context + 0x140;
    char* item_46c = "DefineGraph";
    void item_408;
    Game_DataSymbolLoad(&item_408,
        "%s("%s", "%s", %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %8.08f, %d, %d, %d, %d");
    void* dest_6 = &item_408;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_0_1 = 0 != *dest_6;
        dest_6 += 1;
        i_1 -= 1;

        if (!flag_0_1)
            break;
    }

    int item_410_2 = 0xb;
    int entity_10 = ~i_1;
    int dest_8 = *arg2;
    int i_2 = 0xffffffff;

    while (i_2)
    {
        bool flag_1_1 = 0 != *dest_8;
        dest_8 += 1;
        i_2 -= 1;

        if (!flag_1_1)
            break;
    }

    int source_4;
    int dest_10;
    dest_10 = memcpy(dest_8 - 1, dest_6 - entity_10, entity_10 & 0xfffffffc);
    memcpy(dest_10, source_4, entity_10 & 3);
    float* item_40c = context + 0xe8;
    bool flag_6_1;

    do
    {
        item_428 = *item_40c;
        Game_DataSymbolLoad(&item_408, ", %8.08f");
        void* dest_11 = &item_408;
        int i_3 = 0xffffffff;

        while (i_3)
        {
            bool flag_4_1 = 0 != *dest_11;
            dest_11 += 1;
            i_3 -= 1;

            if (!flag_4_1)
                break;
        }

        int entity_16 = ~i_3;
        int dest_13 = *arg2;
        int i_4 = 0xffffffff;

        while (i_4)
        {
            bool flag_5_1 = 0 != *dest_13;
            dest_13 += 1;
            i_4 -= 1;

            if (!flag_5_1)
                break;
        }

        int source_6;
        int dest_15;
        dest_15 = memcpy(dest_13 - 1, dest_11 - entity_16, entity_16 & 0xfffffffc);
        memcpy(dest_15, source_6, entity_16 & 3);
        flag_6_1 = item_410_2 != 1;
        item_40c = &item_40c[1];
        item_410_2 -= 1;
    } while (flag_6_1);
    int item_410_3 = 0xb;
    float* item_40c_1 = context + 0x114;
    bool flag_9_1;

    do
    {
        item_428 = *item_40c_1;
        Game_DataSymbolLoad(&item_408, ", %8.08f");
        void* dest_16 = &item_408;
        int i_5 = 0xffffffff;

        while (i_5)
        {
            bool flag_7_1 = 0 != *dest_16;
            dest_16 += 1;
            i_5 -= 1;

            if (!flag_7_1)
                break;
        }

        int entity_24 = ~i_5;
        int dest_18 = *arg2;
        int i_6 = 0xffffffff;

        while (i_6)
        {
            bool flag_8_1 = 0 != *dest_18;
            dest_18 += 1;
            i_6 -= 1;

            if (!flag_8_1)
                break;
        }

        int source_8;
        int dest_20;
        dest_20 = memcpy(dest_18 - 1, dest_16 - entity_24, entity_24 & 0xfffffffc);
        memcpy(dest_20, source_8, entity_24 & 3);
        flag_9_1 = item_410_3 != 1;
        item_40c_1 = &item_40c_1[1];
        item_410_3 -= 1;
    } while (flag_9_1);
    char* dest_21;

    if (!arg2[1])
        dest_21 = ");\r\n";
    else
    {
        void* entity_32 = *(*(context + 0x14) + 0x10);
        int data_15 = *(entity_32 + 4);
        int data_16 = -(data_15);

        dest_21 = context == ((data_16 - data_16) & entity_32) ? &g_547a40 : "),\r\n";
    }

    int i_7 = 0xffffffff;

    while (i_7)
    {
        bool flag_10_1 = 0 != *dest_21;
        dest_21 = &dest_21[1];
        i_7 -= 1;

        if (!flag_10_1)
            break;
    }

    int entity_33 = ~i_7;
    int dest_23 = *arg2;
    int i_8 = 0xffffffff;

    while (i_8)
    {
        bool flag_11_1 = 0 != *dest_23;
        dest_23 += 1;
        i_8 -= 1;

        if (!flag_11_1)
            break;
    }

    int source_10;
    int dest_25;
    dest_25 = memcpy(dest_23 - 1, dest_21 - entity_33, entity_33 & 0xfffffffc);
    memcpy(dest_25, source_10, entity_33 & 3);
    return 0;
}
