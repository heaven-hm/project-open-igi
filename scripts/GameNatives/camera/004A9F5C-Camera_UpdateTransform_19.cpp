// IGI native provenance: Camera_UpdateTransform_19 - 0x004A9F5C - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9F5C-Camera_UpdateTransform_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9F5C-Camera_UpdateTransform_19.hpp"
int Camera_UpdateTransform_19(int context, int arg2, int arg3, double arg4)

{
    int item_c = arg3;
    int item_14 = arg3;
    item_14 = arg4;

    if (!(Camera_ApplyPostProcess_27(item_14, arg3, item_c, arg3) & 0x90))
    {
        int item_14_1 = arg3;
        item_14_1 = arg4;
        item_c = Camera_ApplyBandFilter_13(item_14_1);
        long double fpu_val_7_3 = item_c;
        long double f0_1 = arg4;
        fpu_val_7_3 - f0_1;

        if (TEST_BITB(
            *(&((fpu_val_7_3 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0_1) ? 1 : 0) << 0xa
                | (fpu_val_7_3 == f0_1 ? 1 : 0) << 0xe) + 1),
            6))
        {
            int item_10_2 = arg3;
            int item_14_2 = arg3;
            arg4 = arg4 / 2.0;
            item_14_2 = arg4;
            long double fp0_2 = Camera_ApplyBandFilter_13(item_14_2);
            long double f1 = arg4;
            fp0_2 - f1;

            if (!TEST_BITB(
                    *(&((fp0_2 < f1 ? 1 : 0) << 8 | (FCMP_UO(fp0_2, f1) ? 1 : 0) << 0xa
                        | (fp0_2 == f1 ? 1 : 0) << 0xe) + 1),
                    6))
                return 1;

            return 2;
        }
    }

    return 0;
}
