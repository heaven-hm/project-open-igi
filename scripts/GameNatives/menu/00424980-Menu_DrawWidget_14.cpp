// IGI native provenance: Menu_DrawWidget_14 - 0x00424980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424980-Menu_DrawWidget_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_DrawWidget_14.hpp"
int Menu_DrawWidget_14(void* context, float arg2, float arg3, int arg4)

{
    int dest_val = (g_c28b44 >> 1) + *(context + 0x20) - 0x140;
    int data_2 = *(context + 0x24);
    int temp_1 = g_c28b48 >> 1;
    int item_d8 = *(context + 0x28);
    int i_1 = 4;
    int temp_2 = *(context + 0x2c);
    void item_98;
    int* data_3 = &item_98;
    int i;

    do
    {
        long double fpu_val_7_1;

        fpu_val_7_1 = !arg4 ? 0.0f : 0.5f;

        data_3[-1] = fpu_val_7_1;
        *data_3 = 0x3f800000;
        long double fpu_val_7_2;

        fpu_val_7_2 = !arg4 ? 0.0f : 0.5f;

        data_3[1] = fpu_val_7_2;
        data_3[-2] = arg3;
        data_3[-4] = 0;
        data_3[-3] = 0;
        data_3[-5] = 0x3f800000;
        data_3 = &data_3[9];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    long double fpu_val_7_3 = dest_val + 1;
    int temp_5 = item_d8 + dest_val;
    float item_b4 = fpu_val_7_3;
    long double fpu6 = temp_1 + data_2 - 0xef;
    int item_c4 = 0;
    float item_b0 = fpu6;
    long double fpu5 = temp_5;
    float item_90 = fpu5;
    float item_8c = fpu6;
    long double fpu_val_6_1 = temp_2 + temp_1 + data_2 - 0xf0;
    float item_6c = fpu5;
    float item_68 = fpu_val_6_1;
    float item_48 = fpu_val_7_3;
    long double fpu_val_7_4 = arg3;
    long double f1 = 0.99000001f;
    fpu_val_7_4 - f1;
    float item_44 = fpu_val_6_1;
    void item_24;
    memcpy(&item_24, &item_b4, 0x24);

    if (*(&((fpu_val_7_4 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f1) ? 1 : 0) << 0xa
            | (fpu_val_7_4 == f1 ? 1 : 0) << 0xe) + 1) & 0x41)
        int item_c4_1 = 0x80000;

    int temp_7 = g_a84a54;
    int item_c8 = 1;
    int item_c0 = 0;
    int item_bc = 5;
    float* item_b8 = &item_b4;
    g_a84a50 = 0;
    g_a84a58 = 0;
    int entity_5 = (*((temp_7 << 2) + &g_a94e84))(&item_c8);
    int item_f8 = entity_5;
    int item_108 = entity_5;
    int item_10c = entity_5;
    int item_110 = entity_5;
    int item_114 = entity_5;
    return Sys_ExecuteBytecode_14(dest_val, temp_1 + data_2 - 0xf0, item_d8, temp_2, 0.0f, 0.0f, 0.0f, arg2 * arg3, 0x80000,
        0);
}
