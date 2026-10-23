// IGI native provenance: DynObj_InsertHashItem_4 - 0x004C1600 - CFG score 87.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C1600-DynObj_InsertHashItem_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C1600-DynObj_InsertHashItem_4.hpp"
void* DynObj_InsertHashItem_4(int* context, int* arg2)

{
    short item_200;
    memset(&item_200, 0x20202020, 0x200);
    item_200 = 0xa0d;
    int data_3 = context[1] + 2;
    char item_1fe = 0;
    char item_1fe_1 = 0x20;
    int item_228 = 0x200;

    if (data_3 <= 0x200)
        item_228 = data_3;

    int source_val = *(context[2] + *arg2 + 1);
    int item_224 = source_val;
    Sys_CloseFileStream_4(*context, &g_547a4c, 1);
    int flagP = 0;
    int item_22c = 0;

    if (source_val > 0)
    {
        do
        {
            int item_240_1 = *(context[2] + (flagP << 2) + *arg2 + 5);
            void item_220;
            sprintf(&item_220, "@%08X");

            if (flagP >= source_val - 1)
                item_22c = 0;
            else
            {
                void* dest_1 = &g_547a44;
                int i = 0xffffffff;

                while (i)
                {
                    bool flag_2_1 = 0 != *dest_1;
                    dest_1 += 1;
                    i -= 1;

                    if (!flag_2_1)
                        break;
                }

                int entity_4 = ~i;
                void* dest_3 = &item_220;
                int i_1 = 0xffffffff;

                while (i_1)
                {
                    bool flag_3_1 = 0 != *dest_3;
                    dest_3 += 1;
                    i_1 -= 1;

                    if (!flag_3_1)
                        break;
                }

                int source_2;
                int dest_5;
                dest_5 = memcpy(dest_3 - 1, dest_1 - entity_4, entity_4 & 0xfffffffc);
                memcpy(dest_5, source_2, entity_4 & 3);
                source_val = item_224;
            }

            void* dest_6 = &item_220;
            int i_2 = 0xffffffff;

            while (i_2)
            {
                bool flag_5_1 = 0 != *dest_6;
                dest_6 += 1;
                i_2 -= 1;

                if (!flag_5_1)
                    break;
            }

            Sys_CloseFileStream_4(*context, &item_220, ~i_2 - 1);

            if (item_22c >= 4)
            {
                item_22c = 0xffffffff;
                Sys_CloseFileStream_4(*context, &item_200, item_228);
            }

            flagP += 1;
            item_22c += 1;
        } while (flagP < source_val);
    }

    Sys_CloseFileStream_4(*context, &g_547ee8, 3);
    context[2] += (source_val << 2) + 5;
    return (source_val << 2) + 5;
}
