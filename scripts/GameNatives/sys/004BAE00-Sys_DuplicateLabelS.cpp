// IGI native provenance: Sys_DuplicateLabelS - 0x004BAE00 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BAE00-Sys_DuplicateLabelS.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BAE00-Sys_DuplicateLabelS.hpp"
int* Sys_DuplicateLabelS(void* context, int arg2)

{
    int dest_val = arg2;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_val = ~i;
    int* item_84 = nullptr;
    void item_80;
    int source_1;
    int dest_2;
    dest_2 = memcpy(&item_80, dest_val - entity_val, entity_val & 0xfffffffc);
    memcpy(dest_2, source_1, entity_val & 3);
    void* dest_3 = &item_80;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_3;
        dest_3 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int saved_target;

    if (*((&item_84 + 3) + ~i_1 - 1) == 0x3a)
        *(&saved_target + ~i_1 - 1 + 0xb) = 0;
    DynObj_CreateMagicObject_2(*(context + 0x1c), &item_84, &item_80);
    int* data_3 = item_84;

    if (data_3)
    {
        if (!data_3[5])
            return data_3;

        *(context + 4) = 1;
        void* saved_frame_1 = &item_80;
        void* item_98_1 = context;
        Sys_FreeMemory_26("Duplicate label: '%s'");
        return 0;
    }

    int* data_5 = Mem_Alloc(0x9c, 4);
    Symbol_CheckRegister(*(context + 0x1c), &item_80, data_5);
    data_5[1] = 0;
    *data_5 = 0;
    HeapList_Insert(context + 0x20, data_5);
    void* dest_4 = &item_80;
    int i_2 = 0xffffffff;
    data_5[5] = 0;

    while (i_2)
    {
        bool flag_2_1 = 0 != *dest_4;
        dest_4 += 1;
        i_2 -= 1;

        if (!flag_2_1)
            break;
    }

    int entity_8 = ~i_2;
    int source_5;
    int dest_7;
    dest_7 = memcpy(&data_5[7], dest_4 - entity_8, entity_8 & 0xfffffffc);
    memcpy(dest_7, source_5, entity_8 & 3);
    data_5[3] = 0;
    data_5[4] = &data_5[2];
    data_5[2] = &data_5[3];
    return data_5;
}
