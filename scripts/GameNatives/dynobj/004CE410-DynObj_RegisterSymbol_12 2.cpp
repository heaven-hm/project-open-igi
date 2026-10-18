// IGI native provenance: DynObj_RegisterSymbol_12 - 0x004CE410 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE410-DynObj_RegisterSymbol_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RegisterSymbol_12.h"
int DynObj_RegisterSymbol_12(float context, double* arg2, double* arg3, char arg4, float arg5, int arg6, int arg7, float* arg8)

{
    g_a44394 += 1;
    short data_2;
    data_2 = arg4;
    double* item_8c = arg2;
    double* item_88 = arg3;
    char item_84 = data_2;
    float item_80 = arg5;
    float item_7c = context;
    char item_78 = 0;
    float item_74 = 1.0f;

    for (int i = 0; i < 6; i += 1)
    {
        if ((i == 1 || !i || i == 4 || i == 5) && arg7 & DynObj_InvalidDyncubetree(i))
            DynObj_ParseConfigFile_9(context, 8, &item_8c, arg2, arg3, 1, arg6, i);
    }

    long double fpu7 = item_74;
    long double f0 = 0.0f;
    fpu7 - f0;

    if (!(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40) && arg6 & 0x80000000)
    {
        void item_70;
        DynObj_FindSymbol_11(context, &item_70, arg2, arg3, 1, 0xff, nullptr, 0x80000000);
        int item_18;

        if (item_18)
        {
            item_74 = 0.0f;
            item_78 = 1;
        }
    }

    if (arg8)
        *arg8 = item_74;

    int result = 0;
    result = !item_78;
    return result;
}
