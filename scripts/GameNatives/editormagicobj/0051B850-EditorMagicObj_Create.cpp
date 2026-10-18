// IGI native provenance: EditorMagicObj_Create - 0x0051B850 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051B850-EditorMagicObj_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0051B850-EditorMagicObj_Create.hpp"
int* EditorMagicObj_Create(void* self, float args)

{
    void* result_val_1 = DynObj_Missing(*args);
    DynObj_LookupHashItem_6(*(args + 4), self, result_val_1);
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

    int scratch_val = *(self + 0xd4);
    long double fpu_val_7_6 = Render_ComputeNormals(*(self + 0x6c)) * fpu_val_7_3;
    DynObj_LinkDynamicCube_8(*(args + 4), self, args + 8, fpu_val_7_6, scratch_val);
    memcpy(self + 0x70, args + 0x38, 0x28);
    memcpy(self + 0x108, args + 0x60, 0x28);
    *(self + 0x138) = *(args + 0x8c);
    int frame_ptr_1 = *(args + 0x88);
    *(self + 0x130) = frame_ptr_1;
    return Car_GunCreate(self, self, frame_ptr_1);
}
