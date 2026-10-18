// IGI native provenance: Human_Cleanup_6 - 0x004606F0 - CFG score 97.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004606F0-Human_Cleanup_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Human_Cleanup_6.hpp"
int* Human_Cleanup_6(void* context, int* arg2)

{
    int* result = arg2[2];

    if (!result)
    {
    label_46074b:
        void* temp_1 = *arg2;
        double item_318[0x3];
        memcpy(&item_318, context + 0x20, 0x18);
        double item_300[0x60];
        result = Weapon_CheckAmmo_6(&item_300, 0x20, temp_1, &item_318);

        if (result > 0)
        {
            double (* source_3)[0x60] = &item_300;
            int* result_1 = result;
            bool flag_3_1;

            do
            {
                if (!AI_CheckHearing_19(context))
                {
                    int* data_6 = arg2[2];
                    int entity_1 = *arg2;
                    int item_334 = 0x14;
                    int item_330 = 1;
                    int item_32c_1 = 0x46;
                    int item_328_1 = 0x46000000;
                    double (* item_324_1)[0x60] = source_3;
                    int item_320_1 = entity_1;

                    if (!data_6)
                        int item_31c_2 = 0;
                    else
                    {
                        void* data_7 = *data_6;
                        void* item_31c_1 = data_7;

                        if (data_7)
                            item_330 = AI_CombatTarget_19(data_7);
                    }

                    AI_EventBroadcast(&item_334);
                }
                else if (Frames_GetUpdateCounter() > *(context + 0x848))
                {
                    struct status_msg* data_3 = SoundSys_SetMasterVolume_7(context, "gunshotmiss");

                    if (data_3)
                        SoundSys_StopSoundSample_7(data_3, source_3);

                    *(context + 0x848) = Frames_GetUpdateCounter() + 5;
                }

                source_3 = &(*source_3)[3];
                result = result_1 - 1;
                flag_3_1 = result_1 != 1;
                result_1 = result;
            } while (flag_3_1);
        }
    }
    else
    {
        void* source_1 = *result;
        int dest_1 = 1;

        if (source_1 != context)
        {
            if (source_1)
            {
                int data_val = AI_UpdateOrders_17();
                short item_34c_1 = data_val;
                data_val = *(source_1 + 0x1c);
                result = TaskType_IsDerivedFrom(data_val, item_34c_1);

                if (result)
                    dest_1 = *(source_1 + 0xfc);
            }

            if (*(context + 0xfc) != dest_1)
                goto label_46074b;
        }
    }

    return result;
}
