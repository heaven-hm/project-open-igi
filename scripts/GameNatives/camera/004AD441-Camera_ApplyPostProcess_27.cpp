// IGI native provenance: Camera_ApplyPostProcess_27 - 0x004AD441 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD441-Camera_ApplyPostProcess_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD441-Camera_ApplyPostProcess_27.hpp"
int Camera_ApplyPostProcess_27(double context, int arg2 )

{
    short temp_1 = arg2 & 0x7ff0;

    if (temp_1 == 0x7ff0)
    {
        int item_c = 0x7ff0;
        item_c = context;
        int data_1 = Camera_ApplyWideScreen_13(item_c, 0x7ff0);

        if (data_1 == 1)
            return 0x200;

        if (data_1 == 2)
            return 4;

        if (data_1 == 3)
            return 2;

        return 1;
    }

    int data_7 = arg2 & 0x8000;

    if (!temp_1 && (*(&context + 4) & 0xfffff || context))
    {
        int data_9 = -(data_7);
        int data_10 = data_9 - data_9;
        data_10 &= 0x90;
        return data_10 + 0x80;
    }

    long double fpu_val_7_1 = context;
    long double f1 = 0.0;
    fpu_val_7_1 - f1;

    if (!TEST_BITB(
        *(&((fpu_val_7_1 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f1 ? 1 : 0) << 0xe) + 1),
        6))
    {
        int data_17 = -(data_7);
        int data_18 = data_17 - data_17;
        data_18 &= 8;
        return data_18 + 0x100;
    }

    int data_14 = -(data_7);
    int data_15 = data_14 - data_14;
    data_15 &= 0xe0;
    return data_15 + 0x40;
}
