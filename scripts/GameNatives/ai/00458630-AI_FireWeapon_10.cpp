// IGI native provenance: AI_FireWeapon_10 - 0x00458630 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00458630-AI_FireWeapon_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_10.hpp"
void AI_FireWeapon_10(void* context, void* arg2)

{
    int context_val = *(context + 8);
    void* flagP = *(arg2 + 0x800);

    if (!(context_val & 4))
    {
        void* entity_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1d;
        int data_2 = *(entity_1 + 0x74);
        *(context + 0x14) = data_2;
        data_2(context, arg2);
        return;
    }

    if (context_val & 0x10000)
    {
        void* data_4 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1e;
        int data_5 = *(data_4 + 0x78);
        *(context + 0x14) = data_5;
        data_5(context, arg2);
        return;
    }

    if (context_val & 0x20000)
    {
        void* temp_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x20;
        int data_7 = *(temp_2 + 0x80);
        *(context + 0x14) = data_7;
        data_7(context, arg2);
        return;
    }

    if (*(arg2 + 0x538) == 0xc)
    {
        int data_10 = Render_DrawSplineMesh_10(arg2 + 0x68, 0);

        if (*(arg2 + 0x53c) == data_10 && !*(arg2 + 0xd19))
        {
            *(arg2 + 0xd19) = 1;
            long double fp0_1 = Sys_AllocateMemory_14(Level_SetMission_5());
            long double f0_1 = *(flagP + 0x3da8);
            fp0_1 - f0_1;

            if (*(&((fp0_1 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fp0_1, f0_1) ? 1 : 0) << 0xa
                | (fp0_1 == f0_1 ? 1 : 0) << 0xe) + 1) & 0x41)
            {
                void* data_15 = *(flagP + 0x3aa8);

                if (data_15)
                {
                    int* data_16 = *(data_15 + 0xc);

                    if (data_16 && *data_16)
                    {
                        void* temp_3 = *(flagP + 0x3aa8);
                        int item_8 = *(flagP + 0x3dac);
                        void* item_4_1 = arg2;
                        void* flagP1 = **(temp_3 + 0xc);
                        unsigned int data_19 = AI_CheckSector_17();
                        int temp_4 = 0;
                        temp_4 = *(flagP1 + 0x1c);
                        int data_20 = *(((data_19 * 0x180 + temp_4) << 2) + &g_a96ae0);

                        if (data_20)
                            data_20(flagP1, &item_8);
                    }
                }
            }
        }
    }

    if (!(context_val & 1))
        return;

    int* data_22 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_23 = *data_22;
    *(context + 0x14) = data_23;
    data_23(context, arg2);
}
