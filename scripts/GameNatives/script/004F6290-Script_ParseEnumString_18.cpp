// IGI native provenance: Script_ParseEnumString_18 - 0x004F6290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F6290-Script_ParseEnumString_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F6290-Script_ParseEnumString_18.hpp"
int Script_ParseEnumString_18(double context, double arg2, double arg3, double arg4, double arg5, double arg6, double arg7, double arg8, double arg9) {
    int item_28 = 0;
    long double fpu_val_7_2 = (arg4 - arg2) * 0.5;
    long double fpu_val_6_1 = arg7 - arg5;
    long double f0 = 0.0;
    fpu_val_6_1 - f0;

    if (*(&((fpu_val_6_1 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, f0) ? 1 : 0) << 0xa
        | (fpu_val_6_1 == f0 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x40)
    {
        arg4 = 0;
        *(&arg4 + 4) = 0;
    }
    else
    {
        long double fpu_val_5_2 = arg6 - arg5;
        arg4 = (fpu_val_5_2 + fpu_val_5_2) / fpu_val_6_1 * (arg3 - context) * 0.5;
    }

    long double fpu_val_6_5 = arg8 - arg6;
    long double f1 = 0.0;
    fpu_val_6_5 - f1;

    if (*(&((fpu_val_6_5 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_5, f1) ? 1 : 0) << 0xa
        | (fpu_val_6_5 == f1 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x40)
    {
        arg5 = 0;
        *(&arg5 + 4) = 0;
    }
    else
    {
        long double fpu_val_5_7 = arg7 - arg6;
        arg5 = (fpu_val_5_7 + fpu_val_5_7) / fpu_val_6_5 * fpu_val_7_2;
    }

    int item_2c = *(&arg5 + 4);
    int item_34 = *(&arg4 + 4);
    int item_3c = *(&arg3 + 4);
    double item_20[0x4];
    Terrain_ConnectGraphNode_7(&item_20, arg2, *(&arg2 + 4), arg3, arg4, arg5);
    long double fpu_val_7_6 = arg7 - arg6;
    long double f2 = 0.0;
    fpu_val_7_6 - f2;

    if (!(*(&((fpu_val_7_6 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f2) ? 1 : 0) << 0xa
            | (fpu_val_7_6 == f2 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
        item_28 = (arg9 - arg6) / fpu_val_7_6;

    int item_2c_1 = 0;
    return Terrain_SampleHeightmap_7(&item_20, item_28);
}
