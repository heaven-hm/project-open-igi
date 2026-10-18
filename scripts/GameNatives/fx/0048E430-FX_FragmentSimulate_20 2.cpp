// IGI native provenance: FX_FragmentSimulate_20 - 0x0048E430 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E430-FX_FragmentSimulate_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FragmentSimulate_20.h"
int FX_FragmentSimulate_20(void* context, int* arg2, double* arg3)

{
    double item_18[0x3];
    Script_CallNativeHandler_23(&item_18, arg2);
    long double fp0 = Sys_DispatchOpcode_11(arg3, &item_18);
    long double fpu_val_6_1 = arg3[3] * 4096.0;
    fpu_val_6_1 - fp0;
    short data_val = (fpu_val_6_1 < fp0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fp0) ? 1 : 0) << 0xa
        | (fpu_val_6_1 == fp0 ? 1 : 0) << 0xe;

    if (*(&data_val + 1) & 1)
    {
        data_val = FX_SmokeEmit_18(context, arg2, 0, 0);

        if (data_val)
        {
            int data_1 = *arg2;
            arg3[4] = data_1;
            data_1 = 1;
            return data_1;
        }
    }

    data_val = 0;
    return data_val;
}
