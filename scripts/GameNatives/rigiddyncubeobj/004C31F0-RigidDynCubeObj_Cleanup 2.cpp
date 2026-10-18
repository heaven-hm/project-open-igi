// IGI native provenance: RigidDynCubeObj_Cleanup - 0x004C31F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C31F0-RigidDynCubeObj_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "RigidDynCubeObj_Cleanup.h"
void* RigidDynCubeObj_Cleanup()

{
    float* flagP;
    float* item_4 = flagP;
    Camera_UpdateTransform_5(0x27144);
    void* result_1;
    void* result_2 = result_1;
    result_1 = 1;
    void* result = *(result_2 + 0x6c);

    if (result)
    {
        unsigned int data_1 = SoundSys_AttachLipsync_2();
        int entity_1 = 0;
        entity_1 = *(result_2 + 0x1c);
        int data_5 = *(((data_1 * 0x180 + entity_1) << 2) + &g_a96ae0);

        if (data_5)
            data_5(result_2, &result_1);

        result = result_1;

        if (result)
        {
            int* data_6 = *(result_2 + 0x20);
            int temp_1 = *(result_2 + 0x28);
            int arg_c = *(result_2 + 0x24);
            int arg_14 = *(result_2 + 0x2c);
            return_addr = 2;
            int* entity_5 =
                *(*(*(*(*(result_2 + 0x6c) + (*(result_2 + 0xa4) << 2)) + 0xc) + 0xc) + 0x2c) + 4;
            int* item_8_1 = entity_5;
            bool flag_0_1;

            do
            {
                float* temp_4 = *entity_5;

                if (temp_4 > 0)
                {
                    int* entity_6 = entity_5[4];
                    void arg_40;
                    void* data_10 = &arg_40;
                    item_4 = temp_4;
                    bool flag_1_1;

                    do
                    {
                        int* temp_5 = entity_6;
                        entity_6 = &entity_6[4];
                        data_10 += 8;
                        long double fpu_val_7_2 = *temp_5 * *(result_2 + 0x98);
                        long double fpu_val_6_2 = temp_5[1] * *(result_2 + 0x9c);
                        long double fpu_val_5_2 = temp_5[2] * *(result_2 + 0xa0);
                        flag_1_1 = item_4 != 1;
                        item_4 -= 1;
                        *(data_10 - 0xc) = fpu_val_5_2 * *(result_2 + 0x78)
                            + fpu_val_7_2 * *(result_2 + 0x70) + fpu_val_6_2 * *(result_2 + 0x74);
                        *(data_10 - 8) = fpu_val_5_2 * *(result_2 + 0x84)
                            + fpu_val_7_2 * *(result_2 + 0x7c) + fpu_val_6_2 * *(result_2 + 0x80);
                    } while (flag_1_1);
                    entity_5 = item_8_1;
                }

                int i = 0;

                if (entity_5[-1] > 0)
                {
                    do
                    {
                        int data_12 = entity_5[3];
                        int entity_7 = 0;
                        entity_7 = *(data_12 + (i << 3));
                        int temp_9 = 0;
                        temp_9 = *(data_12 + (i << 3) + 2);
                        long double fpu_val_7_8 = data_6[1] * 0.5f / data_6[3];
                        int temp_11 = 0;
                        temp_11 = *(data_12 + (i << 3) + 4);
                        double* data_14 = data_6[2];
                        int saved_target;
                        SoundSys_SetFrequency_3(*data_6,
                            ((&saved_target)[entity_7 * 2 + 0x14] + data_6 - *data_14 + data_6[3])
                                * fpu_val_7_8,
                            ((&saved_target)[entity_7 * 2 + 0x15] + temp_1 - data_14[1] + data_6[3])
                            * fpu_val_7_8,
                            ((&saved_target)[temp_9 * 2 + 0x14] + data_6 - *data_14 + data_6[3])
                                * fpu_val_7_8,
                            ((&saved_target)[temp_9 * 2 + 0x15] + temp_1 - data_14[1] + data_6[3])
                            * fpu_val_7_8,
                            ((&saved_target)[temp_11 * 2 + 0x14] + data_6 - *data_14 + data_6[3])
                                * fpu_val_7_8,
                            ((&saved_target)[temp_11 * 2 + 0x15] + temp_1 - data_14[1] + data_6[3])
                            * fpu_val_7_8);
                        entity_5 = item_8_1;
                        i += 1;
                    } while (i < entity_5[-1]);
                }

                entity_5 = &entity_5[8];
                result = return_addr - 1;
                flag_0_1 = return_addr != 1;
                item_8_1 = entity_5;
                return_addr = result;
            } while (flag_0_1);
        }
    }

    return result;
}
