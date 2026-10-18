// IGI native provenance: Light_UpdateGlobalLight_7 - 0x00529C20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00529C20-Light_UpdateGlobalLight_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_UpdateGlobalLight_7.h"
int* Light_UpdateGlobalLight_7(void* context, void** arg2)

{
    int* result;
    result = g_c28c83;

    if (0x80 & result && !(g_c28d83 & 0x80))
    {
        int data_val = *(context + 0x1bc);
        double item_90;
        double item_88;
        long double fpu_val_7_8;
        double item_80;

        if (data_val < 0)
            fpu_val_7_8 = item_80;
        else
        {
            void* data_1 = Script_ExecuteLevelScript_4(data_val);

            if (!data_1)
                fpu_val_7_8 = item_80;
            else
            {
                int entity_1 = 0;
                int item_10_1 = 0;
                entity_1 = *(data_1 + 0x1c);
                int entity_2 = *((entity_1 << 2) + &g_a994e0);

                if (!entity_2)
                    fpu_val_7_8 = item_80;
                else
                {
                    double item_60;
                    entity_2(data_1, &item_60);
                    float item_3c;
                    float item_38;
                    item_90 = item_60 + item_38 * 204800.0 + item_3c * 40960.0;
                    double item_58;
                    float item_30;
                    float item_2c;
                    item_88 = item_58 + item_30 * 40960.0 + item_2c * 204800.0;
                    double item_50;
                    float item_24;
                    float item_20;
                    fpu_val_7_8 = item_50 + item_24 * 40960.0 + item_20 * 204800.0;
                }
            }
        }
        **arg2 = 0x40000000;
        *(*arg2 + 4) = 0xbf800000;
        *(*arg2 + 8) = *(context + 0x1bc);
        *(*arg2 + 0xc) = item_90;
        *(*arg2 + 0x10) = item_88;
        *(*arg2 + 0x14) = fpu_val_7_8;
        *(*arg2 + 0x18) = *(context + 0x1d8);
        *(*arg2 + 0x1c) = *(context + 0x1e0);
        *(*arg2 + 0x20) = *(context + 0x1e8);
        *(*arg2 + 0x48) = *(context + 0x214);
        result = *arg2;
        result[0x13] = *(context + 0x215);
        *(*arg2 + 0x50) = *(context + 0x216);
    }

    if (g_c28c90 & 0x80 && !(g_c28d90 & 0x80))
    {
        **arg2 = 0x3f800000;
        *(*arg2 + 4) = *(context + 0x1b8);
        *(*arg2 + 8) = *(context + 0x1bc);
        *(*arg2 + 0xc) = *(context + 0x1c0);
        *(*arg2 + 0x10) = *(context + 0x1c8);
        *(*arg2 + 0x14) = *(context + 0x1d0);
        *(*arg2 + 0x18) = *(context + 0x1d8);
        *(*arg2 + 0x1c) = *(context + 0x1e0);
        *(*arg2 + 0x20) = *(context + 0x1e8);
        *(*arg2 + 0x48) = *(context + 0x214);
        result = *arg2;
        result[0x13] = *(context + 0x215);
        *(*arg2 + 0x50) = *(context + 0x216);
    }

    if (g_c28c80 & 0x80 && !(g_c28d80 & 0x80))
    {
        **arg2 = 0x40000000;
        *(*arg2 + 4) = *(context + 0x1b8);
        *(*arg2 + 8) = *(context + 0x1bc);
        *(*arg2 + 0xc) = *(context + 0x1c0);
        *(*arg2 + 0x10) = *(context + 0x1c8);
        *(*arg2 + 0x14) = *(context + 0x1d0);
        *(*arg2 + 0x18) = *(context + 0x1d8);
        *(*arg2 + 0x1c) = *(context + 0x1e0);
        *(*arg2 + 0x20) = *(context + 0x1e8);
        *(*arg2 + 0x48) = *(context + 0x214);
        result = *arg2;
        result[0x13] = *(context + 0x215);
        *(*arg2 + 0x50) = *(context + 0x216);
    }

    if (g_c28c75 & 0x80 && !(g_c28d75 & 0x80))
    {
        **arg2 = 0x40400000;
        *(*arg2 + 4) = *(context + 0x1b8);
        *(*arg2 + 8) = *(context + 0x1bc);
        *(*arg2 + 0xc) = *(context + 0x1c0);
        *(*arg2 + 0x10) = *(context + 0x1c8);
        *(*arg2 + 0x14) = *(context + 0x1d0);
        *(*arg2 + 0x18) = *(context + 0x1d8);
        *(*arg2 + 0x1c) = *(context + 0x1e0);
        *(*arg2 + 0x20) = *(context + 0x1e8);
        *(*arg2 + 0x48) = *(context + 0x214);
        result = *arg2;
        result[0x13] = *(context + 0x215);
        *(*arg2 + 0x50) = *(context + 0x216);
    }

    if (g_c28c74 & 0x80)
    {
        result = *arg2;
        *result = 0x40800000;
    }

    return result;
}
