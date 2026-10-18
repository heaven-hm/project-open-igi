// IGI native provenance: Menu_RequestScreen_6 - 0x00419630 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00419630-Menu_RequestScreen_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_RequestScreen_6.h"
int Menu_RequestScreen_6(float* context, float* arg2)

{
    long double fpu7 = 0.0f;
    float item_5e0 = context[1] * arg2[2] + *context * *arg2 + arg2[4];
    float item_5dc = context[1] * arg2[3] + arg2[1] * *context + arg2[5];
    long double fpu_val_6_9 = context[2] * arg2[6];
    float item_5ec = fpu_val_6_9;
    long long item_5e8 = fpu_val_6_9 * 0.314159274f;
    int i_2 = item_5e8;

    if (i_2 >= 0x28)
        i_2 = 0x28;

    int i_3 = 0;
    item_5e8 = i_2;

    if (i_2 > 0)
    {
        void item_5a8;
        int* data_1 = &item_5a8;
        int i_1 = i_2;
        i_3 = i_2;
        long double fpu_val_6_12 = 6.28318548f / item_5e8;
        int i;

        do
        {
            long double fp0_1;
            bool c1_1;
            bool c2_1;
            fp0_1 = __fcos(fpu7);
            data_1[-1] = 0;
            *data_1 = 0x3f800000;
            data_1[1] = 0;
            data_1[-2] = 0x3d99999a;
            data_1[-4] = 0;
            data_1[-3] = 0;
            data_1[-5] = 0x3f800000;
            data_1 = &data_1[9];
            i = i_1;
            i_1 -= 1;
            data_1[-0x10] = fp0_1 * item_5ec + item_5e0;
            long double fp0_2;
            bool c1_2;
            bool c2_2;
            fp0_2 = __fsin(fpu7);
            data_1[-0xf] = fp0_2 * item_5ec + item_5dc;
            fpu7 = fpu7 + fpu_val_6_12;
        } while (i != 1);
    }

    int item_5d0 = 0;
    int data_3 = g_a84a54;
    int saved_target;
    void item_5c4;
    memcpy(&(&saved_target)[i_3 * 9 + 0xe], &item_5c4, 0x24);
    int item_5d4 = 0x80000;
    int item_5d8 = 1;
    int item_5cc = i_2 + 1;
    void* item_5c8 = &item_5c4;
    return (*((data_3 << 2) + &g_a94e84))(&item_5d8);
}
