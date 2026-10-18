// IGI native provenance: HumanPlayer_UpdateStamina_8 - 0x0040F640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040F640-HumanPlayer_UpdateStamina_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateStamina_8.hpp"
void* HumanPlayer_UpdateStamina_8(void* context, int* arg2)

{
    void* flagP = arg2[0x13b];
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(&arg2[0x131], 0);
    int temp_val;
    void* data_2 = HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    arg2[0x138] += 1;
    void* data_3 = *(flagP + 0x20c);

    if (data_3 == 3)
    {
        data_3 = arg2[0x139];
        *(arg2 + 0x4d1) = 1;

        if (data_3)
        {
            data_3 = SoundSys_SetFrequency_7(data_3);
            arg2[0x139] = 0;
        }
    }

    if (*(context + 8) & 1)
    {
        Bones_PlayAnimation(flagP + 0x68, 0, *(data_2 + 0x67c), 0, 0.0f);
        AI_UpdateSensors_22(arg2[0x13b], -0.610865235f, 0xbeb2b8c3);
        AI_SelectTarget_22(arg2[0x13b], 0.0f, 0, 0x3d6e4bae, 0x3d082b3f);
        void item_8;
        Script_ParseEnumReal32_13(data_1, &item_8);
        int item_4;
        int data_5 = item_4;
        bool flag_0_1 = data_5;
        data_5 = g_56e214;

        if (!flag_0_1 && !data_5)
        {
            HumanPlayer_UpdateHealth_8(arg2, 1);
            *(context + 0x14) = HumanPlayer_CheckCollision_7;
            return context;
        }

        data_3 = Script_SpawnNewTask_13(data_1, data_5);
        *(context + 0x14) = HumanPlayer_PlayFootstep_8;
    }

    return data_3;
}
