// IGI native provenance: Display_ClearSurfaces_12 - 0x004972B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004972B0-Display_ClearSurfaces_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_12.h"
int Display_ClearSurfaces_12(int context)

{
    int source_val = context;
    int f0 = *(*(source_val + 0xc) + 0x1c) / g_limit_b;
    int* data_4 = *(source_val + 0x10);
    (*(*data_4 + 0x10))(data_4, &context, 0);
    void* entity_1 = *(source_val + 0xc);
    int data_7;
    int temp_2;
    temp_2 = HIGHD(*(entity_1 + 0x18) * *(entity_1 + 0x16));
    data_7 = LOWD(*(entity_1 + 0x18) * *(entity_1 + 0x16));
    int f0_1 = context / ((data_7 + (temp_2 & 7)) >> 3);
    int result = *(source_val + 0x3c);
    *(source_val + 0x20) = f0_1;

    if (result == 1 && g_c28c5c > *(source_val + 0x2c))
    {
        *(source_val + 0x3c) = 2;
        result = 0;
        return result;
    }

    if (result && result != 1)
    {
    label_49732b:
        result = 0;
        return result;
    }

    int context_2 = *(source_val + 0x28);
    int temp_11;

    if (f0_1 <= context_2)
        temp_11 = context_2 - f0_1;
    else
    {
        int data_14;
        int temp_7;
        temp_7 = HIGHD(0x10624dd3 * *(entity_1 + 0x1c) * 0xfa0);
        data_14 = LOWD(0x10624dd3 * *(entity_1 + 0x1c) * 0xfa0);
        int temp_8 = temp_7 >> 6;
        temp_11 = temp_8 + (temp_8 >> 0x1f) - f0_1 + context_2;
    }

    result = (*(source_val + 0x34) - temp_11) / f0;

    for (int i = result; i > 0; i -= 1)
    {
        Display_RestoreDevice_11(source_val, f0);
        result = *(source_val + 0x3c);

        if (result == 0xffffffff)
            goto label_49732b;
    }

    result = 1;
    return result;
}
