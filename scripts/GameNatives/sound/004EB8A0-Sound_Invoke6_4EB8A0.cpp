// IGI native provenance: Sound_Invoke6_4EB8A0 - 0x004EB8A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EB8A0-Sound_Invoke6_4EB8A0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sound_Invoke6_4EB8A0.hpp"

void* Sound_Invoke6_4EB8A0(void* arg1, int arg2)

{
    void* result = arg1;
    int* i = *(result + 8);

    if (*i && i)
    {
        int afa7e0_1 = g_afa7e0;

        do
        {
            int* edx_1 = *i;
            void* eax_3;

            if (!edx_1)
                eax_3 = nullptr;
            else
            {
                int result_val = *edx_1;
                int eax_1 = -(result_val);
                eax_3 = (eax_1 - eax_1) & edx_1;
            }

            *((afa7e0_1 << 2) + &g_afa6e0) = eax_3;
            g_afa7e0 = afa7e0_1 + 1;
            unsigned int eax_5 = SoundSys_PlaySoundSample_6();
            int edx_2 = 0;
            edx_2 = i[7];
            result = *(((eax_5 * 0x180 + edx_2) << 2) + &g_a96ae0);

            if (result)
                result = result(i, arg2);

            afa7e0_1 = g_afa7e0 - 1;
            g_afa7e0 = afa7e0_1;
            i = *((afa7e0_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
