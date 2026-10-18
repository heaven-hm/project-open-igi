// IGI native provenance: HumanPlayer_UpdateAnimation_8 - 0x0040FA80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040FA80-HumanPlayer_UpdateAnimation_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateAnimation_8.hpp"
int HumanPlayer_UpdateAnimation_8(void* context, void* arg2)

{
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    void item_8;
    int result = Script_ParseEnumReal32_13(data_1, &item_8);
    long double fpu7 = *(arg2 + 0x1c8);
    long double f0 = 0.0f;
    fpu7 - f0;
    result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40) && *(arg2 + 0x4e8) < 3)
        *(arg2 + 0x4d1) = 1;

    result = *(arg2 + 0x4d3);
    char context_val = *(context + 8);

    if (result)
    {
        HumanPlayer_CheckCollision_9(arg2, 1.12103877e-44.0f);
        result = *(arg2 + 0x4e8) + 1;
        *(arg2 + 0x4e8) = result;
    }

    if (!(context_val & 1))
        return result;

    void* data_3 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 1;
    int data_4 = *(data_3 + 4);
    *(context + 0x14) = data_4;
    return data_4(context, arg2);
}
