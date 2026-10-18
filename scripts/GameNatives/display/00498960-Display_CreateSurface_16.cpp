// IGI native provenance: Display_CreateSurface_16 - 0x00498960 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00498960-Display_CreateSurface_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_16.hpp"
struct status_msg* Display_CreateSurface_16(unsigned int context, int arg2, int arg3, float arg4, float arg5)

{
    int kind;
    kind = g_c311c0;
    struct status_msg* result = QTask_HandleInput(nullptr, kind, 0);
    *(result + 0xe3c) = 0;
    *(result + 0x564c) = 0;
    DynObj_RegisterSymbol_8(context, result, 0);
    int entity_val = 0;
    result->head[0x1a] = context;
    entity_val = !arg3;
    *(result + 0x5650) = entity_val;
    *(result + 0x5654) = arg2;
    *(result + 0x56a8) = 0;
    *(result + 0x5648) = 0;
    void* __offset(status_msg, 0xe5c) data_1 = &result->head[0x397];
    int i_1 = 0x240;
    int i;

    do
    {
        *(ADJ(data_1) + 0xe5c) = 0;
        data_1 += 0x20;
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    *(result + 0x56a0) = arg4;
    *(result + 0x5698) = arg4 + arg5;
    Display_CheckCapabilities_17(result, 0.3125f);
    int entity_1 = g_bcad74;
    int entity_2 = -(entity_1);
    int item_10 = 0;
    int item_c = 0;
    int item_8 = 0;
    int item_4 = 0;
    int* data_2 = g_5ca110;
    int item_c_1 = (((entity_2 - entity_2) & 0xfffff800) + 0x800) | 0x10000;
    item_10 = 0x10;
    int item_4_1 = 0x780;
    int item_8_1 = 0x142;
    (*(*data_2 + 0x14))(data_2, &item_10, &result->head[0x15b3], 0);
    *(result + 0x56ad) = 0;
    g_6e5bd0 = result;
    return result;
}
