// IGI native provenance: Script_FormatString_29 - 0x004FC8E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FC8E0-Script_FormatString_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FC8E0-Script_FormatString_29.hpp"
float Script_FormatString_29(float context, float* arg2, float arg3, float* arg4, float arg5, float arg6 ) {
    float source_val = context;
    float data_val = *(source_val + 4);
    float item_28;
    Sys_PushStack_12(&item_28, *(source_val + 0xc), *(source_val + 0x10), *(source_val + 0x14));
    float* temp_1 = arg3;
    long double fpu_val_7_2 = arg4[1] * temp_1[2] - temp_1[1] * arg4[2];
    long double fpu_val_6_5 = -((*arg4 * temp_1[2] - *temp_1 * arg4[2]));
    long double fpu_val_5_4 = temp_1[1] * *arg4 - *temp_1 * arg4[1];
    float item_34 = fpu_val_7_2 * *arg2 + fpu_val_5_4 * arg2[6] + fpu_val_6_5 * arg2[3];
    float item_30 = fpu_val_7_2 * arg2[1] + fpu_val_5_4 * arg2[7] + fpu_val_6_5 * arg2[4];
    long double fpu_val_7_6 = fpu_val_7_2 * arg2[2] + fpu_val_5_4 * arg2[8] + fpu_val_6_5 * arg2[5];
    float item_24;
    float item_20;
    float item_40 = item_20 * fpu_val_7_6 + item_24 * item_30 + item_28 * item_34;
    float item_1c;
    float item_18;
    float item_14;
    float item_3c = item_14 * fpu_val_7_6 + item_18 * item_30 + item_1c * item_34;
    float item_10;
    float item_c;
    float item_8;
    long double fpu_val_7_9 = fpu_val_7_6 * item_8 + item_c * item_30 + item_10 * item_34;
    float item_34_1 = item_3c * arg2[1] + item_40 * *arg2 + fpu_val_7_9 * arg2[2];
    float item_30_1 = item_3c * arg2[4] + item_40 * arg2[3] + fpu_val_7_9 * arg2[5];
    long double fpu_val_7_11 = item_3c * arg2[7] + item_40 * arg2[6] + fpu_val_7_9 * arg2[8];
    long double fpu_val_5_37 = -((item_34_1 * arg4[2] - fpu_val_7_11 * *arg4)) * temp_1[1]
        + (item_30_1 * arg4[2] - fpu_val_7_11 * arg4[1]) * *temp_1;
    long double fpu_val_7_16 = -(((arg6 + 1.0f) * arg5))
        / (fpu_val_5_37 + (item_34_1 * arg4[1] - item_30_1 * *arg4) * temp_1[2] + data_val);
    float item_40_3 = *temp_1 * fpu_val_7_16;
    float item_3c_2 = temp_1[1] * fpu_val_7_16;
    float item_38_1 = temp_1[2] * fpu_val_7_16;
    long double fpu_val_7_20 = item_38_1 * arg4[1] - item_3c_2 * arg4[2];
    long double fpu_val_6_56 = -((item_38_1 * *arg4 - item_40_3 * arg4[2]));
    long double fpu_val_5_44 = item_3c_2 * *arg4 - item_40_3 * arg4[1];
    float item_30_2 = fpu_val_5_44 * arg2[7] + fpu_val_6_56 * arg2[4] + fpu_val_7_20 * arg2[1];
    long double fpu_val_7_24 = fpu_val_7_20 * arg2[2] + fpu_val_5_44 * arg2[8] + fpu_val_6_56 * arg2[5];
    long double fpu_val_6_59 =
        fpu_val_5_44 * arg2[6] + fpu_val_6_56 * arg2[3] + fpu_val_7_20 * *arg2 + *(source_val + 0x30);
    arg3 = fpu_val_6_59;
    *(source_val + 0x30) = fpu_val_6_59;
    long double fpu_val_6_61 = item_30_2 + *(source_val + 0x34);
    context = fpu_val_6_61;
    *(source_val + 0x34) = fpu_val_6_61;
    long double fpu_val_7_25 = fpu_val_7_24 + *(source_val + 0x38);
    *(source_val + 0x38) = fpu_val_7_25;
    *(source_val + 0x48) = fpu_val_7_25 * item_20 + context * item_24 + arg3 * item_28;
    *(source_val + 0x4c) = fpu_val_7_25 * item_14 + context * item_18 + arg3 * item_1c;
    *(source_val + 0x50) = fpu_val_7_25 * item_8 + context * item_c + arg3 * item_10;
    long double fpu_val_7_36 =
        arg2[4] * *(source_val + 0x4c) + *(source_val + 0x50) * arg2[5] + *(source_val + 0x48) * arg2[3];
    long double fpu_val_7_39 = arg2[7] * *(source_val + 0x4c) + *(source_val + 0x50) * arg2[8];
    long double fpu_val_6_85 = *(source_val + 0x48) * arg2[6];
    *(source_val + 0x48) = arg2[1] * *(source_val + 0x4c) + *arg2 * *(source_val + 0x48) + arg2[2] * *(source_val + 0x50);
    *(source_val + 0x4c) = fpu_val_7_36;
    long double fpu_val_7_42 = item_40_3 * *(source_val + 4);
    long double fpu_val_6_87 = item_3c_2 * *(source_val + 4);
    *(source_val + 0x50) = fpu_val_7_39 + fpu_val_6_85;
    float item_34_5 = item_40_3;
    float item_2c_3 = item_38_1 * *(source_val + 4);
    float entity_5 = *(source_val + 0x20);
    long double fpu_val_7_45 = *(source_val + 0x1c) + fpu_val_6_87;
    *(source_val + 0x18) = *(source_val + 0x18) + fpu_val_7_42;
    *(source_val + 0x1c) = fpu_val_7_45;
    *(source_val + 0x20) = entity_5 + item_2c_3;
    return SoundSys_InitSoundSys_17(source_val);
}
