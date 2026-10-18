// IGI native provenance: Menu_DrawWidget_5 - 0x00419330 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00419330-Menu_DrawWidget_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_DrawWidget_5.hpp"
int Menu_DrawWidget_5(int* context, float* arg2)

{
    void item_2c;
    int* data_val = &item_2c;
    int i_2 = 2;
    int dest_val = 0;
    int i;

    do
    {
        data_val[-1] = 0;
        *data_val = 0x3f800000;
        data_val[1] = 0;
        data_val[-2] = 0x3d99999a;
        data_val[-4] = 0;
        data_val[-3] = 0;
        data_val[-5] = 0x3f800000;
        data_val = &data_val[9];
        i = i_2;
        i_2 -= 1;
    } while (i != 1);
    float item_48;
    int* item_4c = &item_48;
    long double fpu_val_7_1 = context[2] * *arg2;
    long double fpu_val_6_1 = context[3] * arg2[2];
    int i_1 = 0;
    int item_58 = 0x80000;
    int item_5c = 1;
    int item_54 = 0;
    int item_50 = 2;
    float item_64 = context[4] * *arg2;
    float item_68 = context[4] * arg2[1];
    float item_6c = context[5] * arg2[2];
    float item_78 = context[5] * arg2[3];
    float item_60 = fpu_val_7_1 + fpu_val_6_1 + arg2[4] - (item_6c + item_64) * 0.5f;
    float item_80 = item_60;
    long double fpu_val_7_8 =
        context[3] * arg2[3] + context[2] * arg2[1] + arg2[5] - (item_78 + item_68) * 0.5f;
    float item_7c_1 = fpu_val_7_8;
    float item_84 = item_7c_1;

    if (*context >= 0)
    {
        do
        {
            item_48 = item_80;
            float item_44_1 = item_84;
            float item_24_1 = item_80 + item_64;
            float item_20_1 = item_84 + item_68;
            item_80 = item_6c * 0.0500000007f + item_80;
            item_84 = item_78 * 0.0500000007f + item_84;
            (*((g_a84a54 << 2) + &g_a94e84))(&item_5c);
            i_1 += 1;
        } while (i_1 <= *context);
    }

    int result = context[1];
    float item_80_1 = item_60;
    float item_84_1 = item_7c_1;

    if (result >= 0)
    {
        do
        {
            item_48 = item_80_1;
            float item_44_2 = item_84_1;
            float item_24_2 = item_80_1 + item_6c;
            float item_20_2 = item_84_1 + item_78;
            item_80_1 = item_64 * 0.0500000007f + item_80_1;
            item_84_1 = item_68 * 0.0500000007f + item_84_1;
            (*((g_a84a54 << 2) + &g_a94e84))(&item_5c);
            result = context[1];
            dest_val += 1;
        } while (dest_val <= result);
    }

    return result;
}
