// IGI native provenance: HumanAI_InvokeCheckHearing - 0x0045FDA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045FDA0-HumanAI_InvokeCheckHearing.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_InvokeCheckHearing.hpp"

void* HumanAI_InvokeCheckHearing(void* arg1, void* arg2)

{
    void* result = arg1;

    if (result != *(arg2 + 0x14) || result != *(arg2 + 0x18))
    {
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
                unsigned int eax_5 = AI_CheckHearing();
                int ecx_2 = 0;
                ecx_2 = i[7];
                result = *(((eax_5 * 0x180 + ecx_2) << 2) + &g_a96ae0);

                if (result)
                    result = result(i, arg2);

                afa7e0_1 = g_afa7e0 - 1;
                g_afa7e0 = afa7e0_1;
                i = *((afa7e0_1 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    return result;
}
