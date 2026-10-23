// IGI native provenance: Ladder_ClimbTick - 0x0040D6B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040D6B0-Ladder_ClimbTick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0040D6B0-Ladder_ClimbTick.hpp"
int Ladder_ClimbTick(void* context, void* arg2)

{
    void* context_val = *(context + 8);

    if (*(arg2 + 0x538) == 8)
    {
        long double fpu_val_7_1 = *(arg2 + 0x540);
        long double f3_1 = *(arg2 + 0xd88);
        fpu_val_7_1 - f3_1;

        if (!(*(&((fpu_val_7_1 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f3_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f3_1 ? 1 : 0) << 0xe) + 1) & 0x40))
        {
            Render_SetupLighting_10(arg2 + 0x68, 0, 0.0f);
            int temp_1 = *(arg2 + 0xd84);
            int data_3 = *(arg2 + 0xd8c);
            *(arg2 + 0xd88) = *(arg2 + 0x540);
            int entity_2 = *(arg2 + 0xd80);
            *(arg2 + 0xd84) = 0;
            *(arg2 + 0xd80) = entity_2 + temp_1;
            int item_58_1 = *(((data_3 + 1) << 2) + &g_538c14) + 1;
            *(arg2 + 0xd8c) = COMBINE(0, data_3 + 1) % 0xc;
            char item_20[0x20];
            sprintf(&item_20, "walk_ladder_%d");
            struct status_msg* data_6 = SoundSys_SetMasterVolume_7(Script_FormatString(), &item_20);

            if (data_6)
                SoundSys_StopSoundSample_7(data_6, arg2 + 0x20);

            int data_7 = *(arg2 + 0xfc);
            int item_3c = 0xe;
            int item_38_1 = data_7;
            int item_34_1 = 0x46;
            int item_30_1 = 0x46a00000;
            int item_2c_1 = DynObj_UpdateDynTree_4(arg2);
            void* item_28_1 = arg2;
            void* item_24_1 = arg2;
            AI_EventBroadcast(&item_3c);
        }
    }

    int result = *(arg2 + 0xd84);

    if (!result)
    {
        if (context_val & 0x80)
        {
            if (*(arg2 + 0xd80) == *(arg2 + 0xd7c) - 6)
            {
                void* entity_6 = *(context + 4);
                *(context + 0xc) = *(context + 0x10);
                *(context + 0x10) = 0x27;
                int data_11 = *(entity_6 + 0x9c);
                *(context + 0x14) = data_11;
                return data_11(context, arg2);
            }

            *(arg2 + 0xd84) = 1;
            result = Render_SetupLighting_10(arg2 + 0x68, 0, 1.0f);
        }

        if (*(&context_val + 1) & 1)
        {
            if (!*(arg2 + 0xd80))
            {
                *(arg2 + 0xd7a) = 1;
                int* entity_7 = *(context + 4);
                *(context + 0xc) = *(context + 0x10);
                *(context + 0x10) = 0;
                int data_15 = *entity_7;
                *(context + 0x14) = data_15;
                return data_15(context, arg2);
            }

            *(arg2 + 0xd84) = 0xffffffff;
            result = Render_SetupLighting_10(arg2 + 0x68, 0, -1.0f);
        }
    }

    long double fpu_val_7_2 = *(arg2 + 0x1d4);
    long double f4 = 0.0f;
    fpu_val_7_2 - f4;
    result = (fpu_val_7_2 < f4 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f4) ? 1 : 0) << 0xa
        | (fpu_val_7_2 == f4 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
    {
        long double fpu_val_7_3 = *(arg2 + 0x134);
        long double f5_1 = 0.0f;
        fpu_val_7_3 - f5_1;
        result = (fpu_val_7_3 < f5_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f5_1) ? 1 : 0) << 0xa
            | (fpu_val_7_3 == f5_1 ? 1 : 0) << 0xe;

        if (*(&result + 1) & 0x40)
        {
            void* entity_8 = *(context + 4);
            *(context + 0xc) = *(context + 0x10);
            *(context + 0x10) = 0x28;
            int data_18 = *(entity_8 + 0xa0);
            *(context + 0x14) = data_18;
            return data_18(context, arg2);
        }
    }

    if (context_val & 0x20000)
        *(arg2 + 0xd7a) = 1;

    return result;
}
