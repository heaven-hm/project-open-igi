// IGI native provenance: AI_AFCaF - 0x004513F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004513F0-AI_AFCaF.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004513F0-AI_AFCaF.hpp"
int AI_AFCaF(long double context , float arg2)

{
    int saved_source_1;
    int saved_src = saved_source_1;
    float source_val = arg2;
    void* item_8 = source_val + 0x20;
    AI_CheckVisibility_3();
    arg2 = context;
    long double fpu_val_7_1 = -(*(source_val + 0x3ec8));
    long double fpu6 = arg2;
    fpu6 - fpu_val_7_1;
    short result = (fpu6 < fpu_val_7_1 ? 1 : 0) << 8 | (FCMP_UO(fpu6, fpu_val_7_1) ? 1 : 0) << 0xa
        | (fpu6 == fpu_val_7_1 ? 1 : 0) << 0xe | 0x3800;

    if (*(&result + 1) & 1)
        arg2 = fpu_val_7_1;
    else
    {
        long double fpu_val_7_3 = *(source_val + 0x3ec8);
        long double f0_1 = arg2;
        fpu_val_7_3 - f0_1;
        result = (fpu_val_7_3 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0_1) ? 1 : 0) << 0xa
            | (fpu_val_7_3 == f0_1 ? 1 : 0) << 0xe;

        if (*(&result + 1) & 1)
            arg2 = *(source_val + 0x3ec8);
    }

    double item_c_1 = arg2;
    double item_14 = context;
    char* item_18 = "A:  %f, CA: %f";
    int item_1c = 0xffffffff;
    float item_20 = source_val;
    *(source_val + 0x3c78) = arg2 * 0.318309873f;
    return result;
}
