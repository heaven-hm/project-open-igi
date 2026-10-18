// IGI native provenance: Display_CloseDevice_15 - 0x00497D70 - CFG score 80.0% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00497D70-Display_CloseDevice_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CloseDevice_15.hpp"
int Display_CloseDevice_15(int* context, int arg2, int arg3, float arg4)

{
    memcpy(context, arg2, 0x28);
    int entity_val = context[3];
    int temp_val = context[6];
    int item_28 = *context;
    int item_24 = entity_val;
    int item_20 = temp_val;
    int item_1c = context[1];
    int item_18 = context[4];
    int item_14 = context[7];
    int item_10 = context[2];
    int item_c = context[5];
    int item_8 = context[8];
    int item_4 = context[9];
    memcpy(context, &item_28, 0x28);
    memcpy(&context[0xa], arg3, 0x18);
    context[0x10] = arg4;
    context[0x11] = g_bcabc4 * arg4 / (Display_ClearSurfaces_4() * g_bcabc0);
    context[0x12] = g_bcabc8 / context[0x10];
    context[0x13] = Display_ClearSurfaces_4() * context[0x12];
    context[0x14] = g_bcabb8 + g_bcabc8;
    long double fpu_val_7_12 = g_bcabbc + g_bcabcc;
    context[0x16] = 1;
    context[0x18] = 1;
    context[0x15] = fpu_val_7_12;
    return 1;
}
