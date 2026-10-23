// IGI native provenance: CameraBase_Cleanup_2 - 0x004EF8C0 - CFG score 73.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004EF8C0-CameraBase_Cleanup_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004EF8C0-CameraBase_Cleanup_2.hpp"
int CameraBase_Cleanup_2(float* context, void* arg2)

{
    char* data_1 = *(arg2 + 4);
    char* item_20 = data_1;

    if (*data_1)
    {
        char i;

        do
        {
            i = data_1[1];
            data_1 = &data_1[1];
        } while (i);
        item_20 = data_1;
    }

    int i_1 = 0;
    float* item_1c = context;

    do
    {
        double item_38_1 = *item_1c;
        void item_18;
        sprintf(&item_18, "%f");
        void* dest_1 = &item_18;
        int j = 0xffffffff;

        while (j)
        {
            bool flag_3_1 = 0 != *dest_1;
            dest_1 += 1;
            j -= 1;

            if (!flag_3_1)
                break;
        }

        int entity_2 = ~j;
        char* dest_3 = item_20;
        int j_1 = 0xffffffff;

        while (j_1)
        {
            bool flag_4_1 = 0 != *dest_3;
            dest_3 = &dest_3[1];
            j_1 -= 1;

            if (!flag_4_1)
                break;
        }

        int source_2;
        int dest_5;
        dest_5 = memcpy(dest_3 - 1, dest_1 - entity_2, entity_2 & 0xfffffffc);
        memcpy(dest_5, source_2, entity_2 & 3);

        if (i_1 != 0xa)
        {
            void* dest_6 = &g_547a44;
            int j_2 = 0xffffffff;

            while (j_2)
            {
                bool flag_1_1 = 0 != *dest_6;
                dest_6 += 1;
                j_2 -= 1;

                if (!flag_1_1)
                    break;
            }

            int entity_7 = ~j_2;
            char* dest_8 = item_20;
            int j_3 = 0xffffffff;

            while (j_3)
            {
                bool flag_2_1 = 0 != *dest_8;
                dest_8 = &dest_8[1];
                j_3 -= 1;

                if (!flag_2_1)
                    break;
            }

            int source_4;
            int dest_10;
            dest_10 = memcpy(dest_8 - 1, dest_6 - entity_7, entity_7 & 0xfffffffc);
            memcpy(dest_10, source_4, entity_7 & 3);
        }

        i_1 += 1;
        item_1c = &item_1c[1];
    } while (i_1 < 0xb);

    return 0;
}
