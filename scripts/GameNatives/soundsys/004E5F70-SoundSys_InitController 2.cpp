// IGI native provenance: SoundSys_InitController - 0x004E5F70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E5F70-SoundSys_InitController.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_InitController.h"

void* SoundSys_InitController(void* self, int args)

{
    void* result = self;
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
            unsigned int result_val_5 = SoundSys_StopSoundSample_5();
            int scratch_val_2 = 0;
            scratch_val_2 = i[7];
            result = *(((result_val_5 * 0x180 + scratch_val_2) << 2) + &g_a96ae0);

            if (result)
                result = result(i, args);

            afa7e0_1 = g_afa7e0 - 1;
            g_afa7e0 = afa7e0_1;
            i = *((afa7e0_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
