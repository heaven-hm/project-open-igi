// IGI native provenance: Vehicle_InitHeli_5 - 0x004355A0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004355A0-Vehicle_InitHeli_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004355A0-Vehicle_InitHeli_5.hpp"
int Vehicle_InitHeli_5(void* context, float* arg2)

{
    float item_54 = 0.0f;

    if (*(*(context + 0x138) + 4))
    {
        void* data_1 = *(context + 0x130);

        if (*(data_1 + 0x82d))
            item_54 = *(data_1 + 0x72c) * 0.785398185f;
    }

    float item_50;
    Sys_ReadFileBlock_12(&item_50, *(context + 0x13c), 0.0f, item_54);
    float item_44;
    float item_38;
    long double fpu_val_7_6 =
        item_50 * *(context + 0x108) + item_38 * *(context + 0x110) + item_44 * *(context + 0x10c);
    float item_4c;
    float item_40;
    float item_34;
    float item_24 = item_4c * *(context + 0x108) + item_34 * *(context + 0x110) + item_40 * *(context + 0x10c);
    float item_48;
    float item_3c;
    float item_30;
    float item_20 = item_48 * *(context + 0x108) + item_30 * *(context + 0x110) + item_3c * *(context + 0x10c);
    long double fpu_val_6_15 =
        item_50 * *(context + 0x114) + item_38 * *(context + 0x11c) + item_44 * *(context + 0x118);
    float item_18 = item_4c * *(context + 0x114) + item_34 * *(context + 0x11c) + item_40 * *(context + 0x118);
    float item_14 = item_48 * *(context + 0x114) + item_30 * *(context + 0x11c) + item_3c * *(context + 0x118);
    long double fpu_val_5_23 =
        item_38 * *(context + 0x128) + item_44 * *(context + 0x124) + item_50 * *(context + 0x120);
    int item_2c;
    int entity_3 = *(context + 0x12c) + item_2c + 1;
    float item_c = item_34 * *(context + 0x128) + item_40 * *(context + 0x124) + item_4c * *(context + 0x120);
    float item_8 = item_30 * *(context + 0x128) + item_3c * *(context + 0x124) + item_48 * *(context + 0x120);
    item_50 = fpu_val_5_23 * arg2[2] + fpu_val_6_15 * arg2[1] + fpu_val_7_6 * *arg2;
    float item_4c_1 = item_c * arg2[2] + item_18 * arg2[1] + item_24 * *arg2;
    float item_48_1 = item_8 * arg2[2] + item_14 * arg2[1] + item_20 * *arg2;
    float item_44_1 = fpu_val_7_6 * arg2[3] + fpu_val_5_23 * arg2[5] + fpu_val_6_15 * arg2[4];
    float item_40_1 = item_24 * arg2[3] + item_c * arg2[5] + item_18 * arg2[4];
    float item_3c_1 = item_20 * arg2[3] + item_8 * arg2[5] + item_14 * arg2[4];
    float item_38_1 = fpu_val_7_6 * arg2[6] + fpu_val_5_23 * arg2[8] + fpu_val_6_15 * arg2[7];
    float item_34_1 = item_24 * arg2[6] + item_c * arg2[8] + item_18 * arg2[7];
    int result = arg2[9];
    int item_2c_1 = result + entity_3 + 1;
    float item_30_1 = item_20 * arg2[6] + item_8 * arg2[8] + item_14 * arg2[7];
    memcpy(context + 0x70, &item_50, 0x28);
    return result;
}
