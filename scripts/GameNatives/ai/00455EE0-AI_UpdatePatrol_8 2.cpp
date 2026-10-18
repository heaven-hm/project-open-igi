// IGI native provenance: AI_UpdatePatrol_8 - 0x00455EE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00455EE0-AI_UpdatePatrol_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_8.h"
int AI_UpdatePatrol_8(void* context, void* arg2)

{
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    void item_8;
    Script_ParseEnumReal32_13(data_1, &item_8);
    int temp_1 = *(context + 8);

    if (!(temp_1 & 4))
    {
        void* data_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xb;
        int data_3 = *(data_2 + 0x2c);
        *(context + 0x14) = data_3;
        return data_3(context, arg2);
    }

    if (temp_1 & 0x10000)
    {
        void* data_5 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xc;
        int data_6 = *(data_5 + 0x30);
        *(context + 0x14) = data_6;
        return data_6(context, arg2);
    }

    if (temp_1 & 0x20000)
    {
        void* data_8 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xd;
        int data_9 = *(data_8 + 0x34);
        *(context + 0x14) = data_9;
        return data_9(context, arg2);
    }

    long double fpu7 = *(arg2 + 0x1c8);
    long double f0 = 0.0f;
    fpu7 - f0;
    short result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        *(arg2 + 0x4d1) = 1;

    if (!(temp_1 & 1) || *(&temp_1 + 1) & 2)
        return result;

    int* data_11 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_12 = *data_11;
    *(context + 0x14) = data_12;
    return data_12(context, arg2);
}
