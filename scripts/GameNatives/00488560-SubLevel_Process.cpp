// IGI native provenance: SubLevel_Process - 0x00488560 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00488560-SubLevel_Process.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00488560-SubLevel_Process.hpp"
void* SubLevel_Process(void* self, int args)

{
    void* result = self;

    if (*(result + 0x72))
    {
        int* i = *(result + 8);

        if (*i && i)
        {
            int afa7e0_1 = g_afa7e0;

            do
            {
                int* scratch_val_1 = *i;
                void* result_val_3;

                if (!scratch_val_1)
                    result_val_3 = nullptr;
                else
                {
                    int result_val = *scratch_val_1;
                    int result_val_1 = -(result_val);
                    result_val_3 = (result_val_1 - result_val_1) & scratch_val_1;
                }

                *((afa7e0_1 << 2) + &g_afa6e0) = result_val_3;
                g_afa7e0 = afa7e0_1 + 1;
                unsigned int result_val_5 = AI_CheckHearing_25();
                int counter_val_2 = 0;
                counter_val_2 = i[7];
                result = *(((result_val_5 * 0x180 + counter_val_2) << 2) + &g_a96ae0);

                if (result)
                    result = result(i, args);

                afa7e0_1 = g_afa7e0 - 1;
                g_afa7e0 = afa7e0_1;
                i = *((afa7e0_1 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    return result;
}
