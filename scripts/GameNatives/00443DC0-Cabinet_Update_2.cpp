// IGI native provenance: Cabinet_Update_2 - 0x00443DC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00443DC0-Cabinet_Update_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00443DC0-Cabinet_Update_2.hpp"
int Cabinet_Update_2(float self, int args)

{
    *(args + 0xc) = self + 0x20;
    *(args + 0x10) = self + 0x70;
    long double fpu_val_7 = *(self + 0x9c);
    long double fpu_val_6 = *(self + 0xa0);
    fpu_val_6 - fpu_val_7;
    long double fpu_val_7_1;

    if (!(*(&((fpu_val_6 < fpu_val_7 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6, fpu_val_7) ? 1 : 0) << 0xa
            | (fpu_val_6 == fpu_val_7 ? 1 : 0) << 0xe) + 1) & 1))
        fpu_val_7_1 = *(self + 0xa0);
    else
        fpu_val_7_1 = *(self + 0x9c);

    long double fpu_val_6_1 = *(self + 0x98);
    fpu_val_6_1 - fpu_val_7_1;
    long double fpu_val_7_3;

    if (*(&((fpu_val_6_1 < fpu_val_7_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fpu_val_7_1) ? 1 : 0) << 0xa
        | (fpu_val_6_1 == fpu_val_7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
    {
        long double fpu_val_7_4 = *(self + 0x9c);
        long double fpu_val_6_2 = *(self + 0xa0);
        fpu_val_6_2 - fpu_val_7_4;

        if (!(*(&((fpu_val_6_2 < fpu_val_7_4 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_2, fpu_val_7_4) ? 1 : 0) << 0xa
                | (fpu_val_6_2 == fpu_val_7_4 ? 1 : 0) << 0xe) + 1) & 1))
            fpu_val_7_3 = *(self + 0xa0);
        else
            fpu_val_7_3 = *(self + 0x9c);
    }
    else
        fpu_val_7_3 = *(self + 0x98);

    *(args + 0x14) = Render_ComputeNormals(*(self + 0x6c)) * fpu_val_7_3;
    AI_TakeCover_25(args);
    AI_UpdateOrders_25(args);
    return AI_RmS(args);
}
