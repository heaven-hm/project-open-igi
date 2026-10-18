// IGI native provenance: Light_UpdateGlobalLight_8 - 0x0052B9F0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052B9F0-Light_UpdateGlobalLight_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_UpdateGlobalLight_8.hpp"
void* Light_UpdateGlobalLight_8(void* context, int arg2, int arg3, void* arg4)

{
    void* result = context;

    if (arg2 <= *(result + 0x6c))
    {
        void* source_1 = arg4;

        if (Light_SetLightColor_8(result, arg3, source_1) == 1)
            return Light_RenderLensflare_9(context, source_1, arg2);

        long double fpu_val_7_1 = arg3;
        int item_90 = 0;
        int data_4;
        int temp_1;
        temp_1 = HIGHD(arg3);
        data_4 = LOWD(arg3);
        double item_10_1 = fpu_val_7_1;
        int item_8c_1 = 0x3fd00000;
        int data_6 = (data_4 - temp_1) >> 1;
        int item_88_1 = 0;
        double item_f8_1 = fpu_val_7_1 * 0.5;
        int item_84_1 = 0x3fe80000;
        int item_80_1 = 0;
        int item_7c_1 = 0x3fd00000;
        int item_78_1 = 0;
        int item_74_1 = 0x3fe80000;
        int item_70_1 = 0;
        int item_6c_1 = 0x3fd00000;
        int item_68_1 = 0;
        int item_64_1 = 0x3fe80000;
        int item_60_1 = 0;
        int item_5c_1 = 0x3fd00000;
        int item_58_1 = 0;
        int item_54_1 = 0x3fe80000;
        int item_50 = 0;
        int item_4c_1 = 0x3fd00000;
        int item_48_1 = 0;
        int item_44_1 = 0x3fd00000;
        int item_40_1 = 0;
        int item_3c_1 = 0x3fe80000;
        int item_38_1 = 0;
        int item_34_1 = 0x3fe80000;
        int item_30_1 = 0;
        int item_2c_1 = 0x3fd00000;
        int item_28_1 = 0;
        int item_24_1 = 0x3fd00000;
        int item_20_1 = 0;
        int item_1c_1 = 0x3fe80000;
        int item_18_1 = 0;
        int item_14_1 = 0x3fe80000;
        int item_d0 = 0;
        int item_cc_1 = 0x3fd00000;
        int item_c8_1 = 0;
        int item_c4_1 = 0x3fd00000;
        int item_c0_1 = 0;
        int item_bc_1 = 0x3fd00000;
        int item_b8_1 = 0;
        int item_b4_1 = 0x3fd00000;
        int item_b0_1 = 0;
        int item_ac_1 = 0x3fe80000;
        int item_a8_1 = 0;
        int item_a4_1 = 0x3fe80000;
        int item_a0_1 = 0;
        int item_9c_1 = 0x3fe80000;
        int item_98_1 = 0;
        int item_94_1 = 0x3fe80000;
        int item_fc_1 = data_6;
        int context_2 = 0;

        while (true)
        {
            long double fpu_val_6_4 = fpu_val_7_1 * *(&item_90 + context_2);
            double item_e8;
            memcpy(&item_e8, source_1, 0x18);
            item_e8 = fpu_val_6_4 - item_f8_1 + item_e8;
            double item_e0 = fpu_val_7_1 * *(&item_50 + context_2) - item_f8_1 + item_e0;
            double item_d8 = fpu_val_7_1 * *(&item_d0 + context_2) - item_f8_1 + item_d8;
            result = Light_UpdateGlobalLight_8(context, arg2 + 1, data_6, &item_e8);
            context_2 += 8;

            if (context_2 >= 0x40)
                break;

            fpu_val_7_1 = item_10_1;
            data_6 = item_fc_1;
            source_1 = arg4;
        }
    }

    return result;
}
