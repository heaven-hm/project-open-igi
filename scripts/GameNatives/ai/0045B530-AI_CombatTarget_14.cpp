// IGI native provenance: AI_CombatTarget_14 - 0x0045B530 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045B530-AI_CombatTarget_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_14.hpp"
int* AI_CombatTarget_14(void* context, void* arg2)

{
    int context_val = *(context + 8);
    void* flagP = *(arg2 + 0x800);

    if (!(context_val & 4))
    {
        void* entity_val = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1d;
        int data_1 = *(entity_val + 0x74);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (context_val & 0x10000)
    {
        void* data_3 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1e;
        int data_4 = *(data_3 + 0x78);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (context_val & 0x20000)
    {
        void* temp_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x20;
        int data_6 = *(temp_2 + 0x80);
        *(context + 0x14) = data_6;
        return data_6(context, arg2);
    }

    int* result = Render_DrawSplineMesh_10(arg2 + 0x68, 0);

    if (*(arg2 + 0x53c) == result && *(arg2 + 0x538) == 0xc)
    {
        result = *(arg2 + 0xd19);

        if (!result)
        {
            *(arg2 + 0xd19) = 1;
            long double fp0_1;
            fp0_1 = Sys_AllocateMemory_14(Level_SetMission_5());
            long double f0_1 = *(flagP + 0x3da8);
            fp0_1 - f0_1;
            result = (fp0_1 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fp0_1, f0_1) ? 1 : 0) << 0xa
                | (fp0_1 == f0_1 ? 1 : 0) << 0xe;

            if (*(&result + 1) & 0x41)
            {
                result = *(flagP + 0x3aa8);

                if (result)
                {
                    result = result[3];

                    if (result && *result)
                    {
                        void* temp_3 = *(flagP + 0x3aa8);
                        int item_8 = *(flagP + 0x3dac);
                        void* item_4_1 = arg2;
                        void* flagP1 = **(temp_3 + 0xc);
                        unsigned int data_12 = AI_CheckSector_17();
                        int temp_4 = 0;
                        temp_4 = *(flagP1 + 0x1c);
                        result = *(((data_12 * 0x180 + temp_4) << 2) + &g_a96ae0);

                        if (result)
                            result = result(flagP1, &item_8);
                    }
                }
            }
        }
    }

    if (!(context_val & 1))
        return result;

    int* data_13 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_14 = *data_13;
    *(context + 0x14) = data_14;
    return data_14(context, arg2);
}
