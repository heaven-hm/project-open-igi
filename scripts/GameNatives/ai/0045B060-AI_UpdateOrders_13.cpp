// IGI native provenance: AI_UpdateOrders_13 - 0x0045B060 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045B060-AI_UpdateOrders_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_13.hpp"
int AI_UpdateOrders_13(void* context, void* arg2)

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
        *(context + 0x10) = 0x1d;
        int data_3 = *(data_2 + 0x74);
        *(context + 0x14) = data_3;
        return data_3(context, arg2);
    }

    if (temp_1 & 0x10000)
    {
        void* data_5 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1e;
        int data_6 = *(data_5 + 0x78);
        *(context + 0x14) = data_6;
        return data_6(context, arg2);
    }

    if (temp_1 & 0x20000)
    {
        void* data_8 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x20;
        int data_9 = *(data_8 + 0x80);
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

    *(context + 0xc) = *(context + 0x10);

    if (!(temp_1 & 0x2000000))
    {
        void* data_14 = *(context + 4);
        *(context + 0x10) = 0xc;
        int data_15 = *(data_14 + 0x30);
        *(context + 0x14) = data_15;
        return data_15(context, arg2);
    }

    void* data_11 = *(context + 4);
    *(context + 0x10) = 0xa;
    int data_12 = *(data_11 + 0x28);
    *(context + 0x14) = data_12;
    return data_12(context, arg2);
}
