// IGI native provenance: ExplodeMagicObj_Init_2 - 0x0043BE80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043BE80-ExplodeMagicObj_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043BE80-ExplodeMagicObj_Init_2.hpp"
int* ExplodeMagicObj_Init_2(float self, int* args)

{
    DynObj_QtasklistIsFull(Level_InitTextures_4(), self);
    void* result_val_2 = DynObj_Missing(*args);

    if (result_val_2)
    {
        memcpy(self + 0xf0, &args[8], 0x18);
        memcpy(self + 0x108, &args[0x18], 0x28);
        DynObj_LookupHashItem_6(args[1], self, result_val_2);
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

            if (!(*(&((fpu_val_6_2 < fpu_val_7_4 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_2, fpu_val_7_4) ? 1 : 0) << 0xa
                    | (fpu_val_6_2 == fpu_val_7_4 ? 1 : 0) << 0xe) + 1) & 1))
                fpu_val_7_3 = *(self + 0xa0);
            else
                fpu_val_7_3 = *(self + 0x9c);
        }
        else
            fpu_val_7_3 = *(self + 0x98);

        long double fpu_val_7_6 = Render_ComputeNormals(*(self + 0x6c)) * fpu_val_7_3;
        DynObj_LinkDynamicCube_8(args[1], self, &args[2], fpu_val_7_6, 2);
        memcpy(self + 0x70, &args[0xe], 0x28);
        void* result_val_6 = args[0x23];
        *(self + 0x13c) = 0;
        *(self + 0x138) = result_val_6;
        int counter_val_2 = *(result_val_6 + 0x10);
        *(self + 0x148) = 0;
        *(self + 0x140) = counter_val_2;
        *(self + 0x144) = *(result_val_6 + 0x1c);
        int base_ptr_1 = args[0x22];
        *(self + 0x130) = base_ptr_1;
        return Car_GunCreate(self, self, base_ptr_1);
    }

    int saved_source_1 = *args;
    Error_Show("VirModel "%s" not available");

    while (true)
        /* nop */
}
