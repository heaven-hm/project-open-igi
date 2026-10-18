// IGI native provenance: PhysicsTerrain_Invoke12_4F44A0 - 0x004F44A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F44A0-PhysicsTerrain_Invoke12_4F44A0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_Invoke12_4F44A0.hpp"

int PhysicsTerrain_Invoke12_4F44A0(void* arg1, int arg2)

{
    void* result_val = *(arg1 + 0xe8);
    *(arg1 + 0x745) = 0;

    if (result_val)
    {
        int* i_1 = *(result_val + 8);

        if (*i_1)
        {
            int* i = i_1;

            if (i)
            {
                int afa7e0_1 = g_afa7e0;

                do
                {
                    int* edx_1 = *i;
                    void* eax_4;

                    if (!edx_1)
                        eax_4 = nullptr;
                    else
                    {
                        int eax_1 = *edx_1;
                        int eax_2 = -(eax_1);
                        eax_4 = (eax_2 - eax_2) & edx_1;
                    }

                    *((afa7e0_1 << 2) + &g_afa6e0) = eax_4;
                    g_afa7e0 = afa7e0_1 + 1;
                    unsigned int eax_6 = DynObj_LinkDynamicCube_12();
                    int ecx_2 = 0;
                    ecx_2 = i[7];
                    int eax_10 = *(((eax_6 * 0x180 + ecx_2) << 2) + &g_a96ae0);

                    if (eax_10)
                        eax_10(i, arg2);

                    afa7e0_1 = g_afa7e0 - 1;
                    g_afa7e0 = afa7e0_1;
                    i = *((afa7e0_1 << 2) + &g_afa6e0);
                } while (i);
            }
        }
    }

    return (
        *(((DynObj_LinkDynamicCube_12() * 0x180 + DynObj_LookupHashItem_5()) << 2) + &g_a96ae0))(arg1,
        arg2);
}
