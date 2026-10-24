// IGI native provenance: Ladder_GetOnTop - 0x0040D920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040D920-Ladder_GetOnTop.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Ladder_GetOnTop.h"
int* Ladder_GetOnTop(void* context, void* arg2)

{
    int* result = *(arg2 + 0x538);
    int context_val = *(context + 8);

    if (result == 8)
    {
        long double fpu_val_7_1 = *(arg2 + 0x540);
        long double f3_1 = *(arg2 + 0xd88);
        fpu_val_7_1 - f3_1;
        result = (fpu_val_7_1 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f3_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f3_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x40))
        {
            *(arg2 + 0xd88) = *(arg2 + 0x540);
            int item_50_1 = *((*(arg2 + 0xd8c) << 2) + &g_538c18) + 1;
            int data_1;
            *(arg2 + 0xd8c) = COMBINE(0, data_1 + 1) % 0xc;
            char item_20[0x20];
            Game_DataSymbolLoad(&item_20, "walk_ladder_%d");
            struct status_msg* data_5 = SoundSys_SetMasterVolume_7(Script_FormatString(), &item_20);

            if (data_5)
                SoundSys_StopSoundSample_7(data_5, arg2 + 0x20);

            int temp_3 = *(arg2 + 0xfc);
            int item_3c = 0xe;
            int item_38_1 = temp_3;
            int item_34_1 = 0x46;
            int item_30_1 = 0x46a00000;
            int item_2c_1 = DynObj_UpdateDynTree_4(arg2);
            void* item_28_1 = arg2;
            void* item_24_1 = arg2;
            result = AI_EventBroadcast(&item_3c);
        }
    }

    if (!(context_val & 1))
    {
        if (context_val & 0x20000)
            *(arg2 + 0xd7a) = 1;

        return result;
    }

    void* temp_4 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0x25;
    int data_7 = *(temp_4 + 0x94);
    *(context + 0x14) = data_7;
    return data_7(context, arg2);
}
