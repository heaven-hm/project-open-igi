// IGI native provenance: Physics_CheckHullCollision_13 - 0x0050DE00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050DE00-Physics_CheckHullCollision_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CheckHullCollision_13.hpp"
int Physics_CheckHullCollision_13(void* context)

{
    if (!*(context + 4))
    {
        int* data_15 = g_5ca114;
        (*(*data_15 + 0x94))(data_15, 0, 4, 3);
        int* data_16 = g_5ca114;
        (*(*data_16 + 0x94))(data_16, 0, 5, 2);
        int* data_17 = g_5ca114;
        (*(*data_17 + 0x94))(data_17, 0, 6, 0);
        int* data_18 = g_5ca114;
        (*(*data_18 + 0x94))(data_18, 0, 1, 3);
        int* data_19 = g_5ca114;
        (*(*data_19 + 0x94))(data_19, 0, 2, 2);
        int* data_20 = g_5ca114;
        (*(*data_20 + 0x94))(data_20, 0, 3, 0);
        int* data_21 = g_5ca114;
        (*(*data_21 + 0x94))(data_21, 1, 1, 1);
        int* data_22 = g_5ca114;
        return (*(*data_22 + 0x8c))(data_22, 0, 0);
    }

    int* data_1 = g_5ca114;
    int entity_val = *data_1;
    int item_8;
    int* data_2;
    int temp_val;

    if (!(*context & 0x1000000))
    {
        (*(entity_val + 0x94))(data_1, 0, 0x11, 1);
        data_2 = g_5ca114;
        item_8 = 1;
        temp_val = *data_2;
    }
    else
    {
        (*(entity_val + 0x94))(data_1, 0, 0x11, 2);
        data_2 = g_5ca114;
        item_8 = 2;
        temp_val = *data_2;
    }

    (*(temp_val + 0x94))(data_2, 0, 0x10, item_8);
    int* data_3 = g_5ca114;

    if (!(*context & &__dos_header))
        (*(*data_3 + 0x94))(data_3, 0, 0xc, 3);
    else
        (*(*data_3 + 0x94))(data_3, 0, 0xc, 1);

    int* data_4 = g_5ca114;
    (*(*data_4 + 0x94))(data_4, 0, 0x12, 1);
    int* data_5 = g_5ca114;
    (*(*data_5 + 0x94))(data_5, 0, 0xb, 0);
    int* data_6 = g_5ca114;
    (*(*data_6 + 0x94))(data_6, 0, 4, 4);
    int* data_7 = g_5ca114;
    (*(*data_7 + 0x94))(data_7, 0, 5, 2);
    int* data_8 = g_5ca114;
    (*(*data_8 + 0x94))(data_8, 0, 6, 0);
    int* data_9 = g_5ca114;
    (*(*data_9 + 0x94))(data_9, 0, 1, 4);
    int* data_10 = g_5ca114;
    (*(*data_10 + 0x94))(data_10, 0, 2, 2);
    int* data_11 = g_5ca114;
    (*(*data_11 + 0x94))(data_11, 0, 3, 0);
    int* data_12 = g_5ca114;
    (*(*data_12 + 0x94))(data_12, 1, 1, 1);
    int* data_13 = g_5ca114;
    return (*(*data_13 + 0x8c))(data_13, 0, *(*(context + 4) + 0x20));
}
