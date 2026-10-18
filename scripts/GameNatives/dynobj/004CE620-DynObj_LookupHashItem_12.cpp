// IGI native provenance: DynObj_LookupHashItem_12 - 0x004CE620 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004CE620-DynObj_LookupHashItem_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CE620-DynObj_LookupHashItem_12.hpp"
int DynObj_LookupHashItem_12(int context, int arg2)

{
    int data_val = arg2;
    int context_val = context;

    if (!*(context_val + (data_val << 2) + 0xe8))
    {
        void* data_1 = *(context_val + (data_val << 2) + 0x10);
        int entity_4 = -(*(context_val + 0xde4));
        int entity_5 = *(context_val + 0xde8);
        double item_30 = -(*(context_val + 0xde0));
        int entity_7 = 0;
        entity_7 = *(data_1 + 0x30);
        double item_28_1 = entity_4;
        void item_70;
        int* temp_1 = &item_70;
        double item_20_1 = -(entity_5);
        memcpy(&item_70, data_1, 0x40);
        int i = 0;
        void item_48;
        int* item_10_1 = &item_48;
        int item_14_1 = 0x1e;
        char item_40 = 0;
        int item_8_1 = 7;
        void item_64;
        int* dest_1 = &item_64;
        context = data_1 + 0xc;
        int* item_c_1 = data_1 + 0x28;

        do
        {
            *dest_1 = *item_c_1;
            *item_10_1 = *context;
            item_40 |= (entity_7 >> (i & 0x1f) & 1) << (item_8_1 & 0x1f);
            i += 1;
            context += 4;
            dest_1 = &dest_1[1];
            item_8_1 -= 1;
            item_c_1 -= 4;
            item_10_1 -= 4;
        } while (i < 4);

        if (item_40)
        {
            unsigned int data_12 = item_40;
            int item_44;

            if ((~(data_12 - 1) & data_12) == data_12 && !item_44)
            {
                char* source_2 = &item_40;

                do
                {
                    int data_14 = 0;
                    data_14 = *source_2;
                    int eflags_1;
                    int data_15;
                    data_15 = __bsf_gprv_memv(data_14);
                    temp_1 = temp_1[data_15 + 3];
                    int entity_25 = *temp_1;
                    item_30 = ((data_15 & 1) * 2 - 1) * entity_25 + item_30;
                    item_28_1 = ((data_15 & 2) - 1) * entity_25 + item_28_1;
                    char data_21 = temp_1[0xc];
                    item_14_1 -= 1;
                    source_2 = &temp_1[0xc];
                    item_20_1 = ((data_15 >> 1 & 2) - 1) * entity_25 + item_20_1;

                    if (!data_21)
                        break;

                    unsigned int data_22 = data_21;

                    if ((~(data_22 - 1) & data_22) != data_22)
                        break;
                } while (!temp_1[0xb]);

                *(context_val + (arg2 << 2) + 0x28) = temp_1;
                memcpy(context_val + arg2 * 0x18 + 0x40, &item_30, 0x18);
                *(context_val + (arg2 << 2) + 0xd0) = item_14_1;
                *(context_val + (arg2 << 2) + 0xe8) = 1;
                return arg2;
            }
        }

        data_val = arg2;
        *(context_val + (data_val << 2) + 0x28) = *(context_val + (data_val << 2) + 0x10);
        memcpy(context_val + data_val * 0x18 + 0x40, &item_30, 0x18);
        *(context_val + (data_val << 2) + 0xd0) = 0x1e;
        *(context_val + (data_val << 2) + 0xe8) = 1;
    }

    return data_val;
}
