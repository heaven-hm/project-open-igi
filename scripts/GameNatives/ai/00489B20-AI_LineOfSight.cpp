// IGI native provenance: AI_LineOfSight - 0x00489B20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489B20-AI_LineOfSight.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_LineOfSight.hpp"
int AI_LineOfSight(float context, double* arg2, int arg3, double* arg4)

{
    float data_val = context;
    float item_c = data_val;
    float item_8 = data_val;
    int item_4 = arg3;
    char data_2 = FX_TrailUpdate_12();
    void item_10;
    short result = DynObj_RegisterSymbol_12(Level_StartMission_4(), arg2, arg4, data_2, &item_10, 0xffffffff,
        0xffffffff, &context);

    if (result)
    {
        result = 1;
        return result;
    }

    long double fpu7 = context;
    long double f0 = 0.790000021f;
    fpu7 - f0;
    result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe;

    if (*(&result + 1) & 1)
    {
        result = 0;
        return result;
    }

    result = 1;
    return result;
}
