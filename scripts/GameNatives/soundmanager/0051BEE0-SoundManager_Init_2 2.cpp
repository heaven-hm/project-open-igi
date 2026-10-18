// IGI native provenance: SoundManager_Init_2 - 0x0051BEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051BEE0-SoundManager_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundManager_Init_2.h"

int SoundManager_Init_2(void* self, char* args)

{
    if (!args || !*args)
    {
        int* i = *(self + 8);

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
                unsigned int result_val_5 = Script_ParseEnumReal32_8();
                int counter_val_2 = 0;
                counter_val_2 = i[7];
                int result_val_9 = *(((result_val_5 * 0x180 + counter_val_2) << 2) + &g_a96ae0);

                if (result_val_9)
                    result_val_9(i, 0);

                afa7e0_1 = g_afa7e0 - 1;
                g_afa7e0 = afa7e0_1;
                i = *((afa7e0_1 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    Script_Isexists(self + 0x6c, self);
    Script_Isexists(self + 0xbc, self);
    return 0;
}
