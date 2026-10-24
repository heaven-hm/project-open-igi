// IGI native provenance: PhysicsRigid_UpdateState_4DE240 - 0x004DE240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DE240-PhysicsRigid_UpdateState_4DE240.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DE240-PhysicsRigid_UpdateState_4DE240.hpp"
int* PhysicsRigid_UpdateState_4DE240(void* arg1, int arg2)

{
    (*((*g_548694 << 2) + &g_a9a6e0))(arg1, arg2);
    int* i = *(arg1 + 0x148);

    if (i)
    {
        i = i[2];

        if (*i && i)
        {
            int afa7e0_1 = g_afa7e0;

            do
            {
                int* esi_1 = *i;
                void* ecx_4;

                if (!esi_1)
                    ecx_4 = nullptr;
                else
                {
                    int ecx_1 = *esi_1;
                    int ecx_2 = -(ecx_1);
                    ecx_4 = (ecx_2 - ecx_2) & esi_1;
                }

                *((afa7e0_1 << 2) + &g_afa6e0) = ecx_4;
                int ecx_5 = 0;
                g_afa7e0 = afa7e0_1 + 1;
                ecx_5 = i[7];
                (*((ecx_5 << 2) + &g_a9a6e0))(i, arg2);
                afa7e0_1 = g_afa7e0 - 1;
                g_afa7e0 = afa7e0_1;
                i = *((afa7e0_1 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    return i;
}
