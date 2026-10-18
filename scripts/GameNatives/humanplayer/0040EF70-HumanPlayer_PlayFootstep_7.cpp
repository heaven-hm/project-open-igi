// IGI native provenance: HumanPlayer_PlayFootstep_7 - 0x0040EF70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040EF70-HumanPlayer_PlayFootstep_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_7.hpp"
void** HumanPlayer_PlayFootstep_7(void* context, void* arg2)

{
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    void* data_2 = HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    void** result_1;
    Script_ParseEnumReal32_13(data_1, &result_1);
    short data_4;
    data_4 = *(arg2 + 0xd44);
    short context_val = *(context + 8);

    if (data_4)
    {
        int* data_5 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0;
        int data_6 = *data_5;
        *(context + 0x14) = data_6;
        return data_6(context, arg2);
    }

    long double fpu7 = *(arg2 + 0x1c8);
    long double f0 = 0.0f;
    fpu7 - f0;
    void** result;

    if (*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40)
    {
        long double fpu_val_7_1 = *(arg2 + 0x1d0);
        long double f1_1 = 0.0f;
        fpu_val_7_1 - f1_1;
        result = result_1;
        int item_4;

        if ((!(*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40) || !result) && item_4
            && result < *(data_2 + 0x648))
        {
            Human_TaskViewReset(*(arg2 + 0x4ec));
            void* temp_3 = *(context + 4);
            *(context + 0xc) = *(context + 0x10);
            *(context + 0x10) = 3;
            int data_13 = *(temp_3 + 0xc);
            *(context + 0x14) = data_13;
            return data_13(context, arg2);
        }
    }
    else
    {
        result = result_1;

        if (!result && context_val & 1)
        {
            void* temp_2 = *(context + 4);
            *(context + 0xc) = *(context + 0x10);
            *(context + 0x10) = 0xc;
            int data_8 = *(temp_2 + 0x30);
            *(context + 0x14) = data_8;
            return data_8(context, arg2);
        }

        *(arg2 + 0x4d1) = 1;
    }

    result = *(arg2 + 0x4d3);

    if (result)
        result = HumanPlayer_CheckCollision_9(arg2, 8.40779079e-45.0f);

    if (!(context_val & 1) || *(&context_val + 1) & 2)
        return result;

    void* entity_2 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 1;
    int data_11 = *(entity_2 + 4);
    *(context + 0x14) = data_11;
    return data_11(context, arg2);
}
