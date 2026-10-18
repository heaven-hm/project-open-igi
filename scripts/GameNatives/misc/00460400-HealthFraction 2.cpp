// IGI native provenance: HealthFraction - 0x00460400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00460400-HealthFraction.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HealthFraction.h"
int HealthFraction(int context, int* arg2)

{
    int* context_val = arg2;
    void* flagP = *context_val;
    void* source_val = *(Level_LoadLevel_6() + 0x5c);
    int data_1 = g_c28b44;
    int entity_val = g_c28b48;
    int result = *(source_val + 0xe8);

    if (result == 1)
    {
        void* source_1 = Table_GetAt(context + 0x4c4, 0);

        if (source_1)
        {
            unsigned int data_4 = FX_DebrisSpawn_8();
            int entity_1 = 0;
            entity_1 = *(source_1 + 0x1c);
            void* data_2 = *(((data_4 * 0x180 + entity_1) << 2) + &g_a96ae0);

            if (data_2)
                data_2(source_1, context_val);
        }

        if (*(context + 0x7b8))
            FX_FragmentSimulate_9(*context_val);
        else if (!AI_CheckHearing_21(context))
            FX_FragmentSimulate_9(*context_val);
        else
        {
            arg2 = *context_val;
            int entity_2 = 0;
            entity_2 = *(AI_CheckHearing_21(context) + 0x1c);
            int source_4 = *(((entity_2 + Level_SetMission_2() * 0x180) << 2) + &g_a96ae0);

            if (source_4)
                source_4(AI_CheckHearing_21(context), &arg2);
        }

        long double fpu_val_7_3 = 1.0f - *(context + 0x254) / *(context + 0x258);
        float item_14_1 = fpu_val_7_3;
        long double f0_1 = 0.0f;
        fpu_val_7_3 - f0_1;

        if (*(&((fpu_val_7_3 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0_1) ? 1 : 0) << 0xa
                | (fpu_val_7_3 == f0_1 ? 1 : 0) << 0xe) + 1) & 1)
            item_14_1 = 0.0f;

        float item_10_1 = data_1 * 0.0625f;
        arg2 = Sys_CloseFileStream_17(flagP + 0x8c);
        arg2 = item_10_1 - arg2 * 0.5f;
        float item_c_1 = entity_val * 0.895833313f;
        int data_16 = Sys_ExecuteBytecode_17(flagP + 0x8c);
        result = Sys_ResolveSymbol_14(flagP + 0x8c, arg2, item_c_1 - data_16 * 0.5f, 128.0f, 128.0f, 128.0f, 0, 0, 0);
        long double fpu_val_7_14 = item_14_1;
        long double f1_1 = 0.0f;
        fpu_val_7_14 - f1_1;
        result = (fpu_val_7_14 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_14, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_14 == f1_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x40))
        {
            arg2 = item_10_1 - 2.0f;
            context = item_c_1 - 2.0f;
            long double fpu_val_7_20 = Sys_CloseFileStream_17(flagP + 0xa4) * 0.5f;
            float item_4_1 = arg2 - fpu_val_7_20;
            float item_10_2 = context - Sys_ExecuteBytecode_17(flagP + 0xa4) * 0.5f;
            float item_c_2 = Sys_CloseFileStream_17(flagP + 0xa4) * 0.5f + arg2;
            arg2 = Sys_ExecuteBytecode_17(flagP + 0xa4);
            float item_8_4 = arg2 * 0.5f + context;
            arg2 = 1.0f - item_14_1;
            int data_22 = Sys_ExecuteBytecode_17(flagP + 0xa4);
            int* data_23 = arg2;
            float item_10_3 = data_22 * arg2 + item_10_2;
            return Sys_CloseFileStream_15(flagP + 0xa4, item_4_1, item_10_3, item_c_2, item_10_3, item_4_1, item_8_4,
                item_c_2, item_8_4, 0, data_23, 0x3f800000, data_23, 0, 0x3f800000, 0x3f800000,
                0x3f800000, 128.0f, 128.0f, 128.0f, 0, 0x3f800000, 0, 0x80000, 0);
        }
    }

    return result;
}
