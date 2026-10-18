// IGI native provenance: AIStationaryGunHolder_Init_2 - 0x0046E210 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046E210-AIStationaryGunHolder_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0046E210-AIStationaryGunHolder_Init_2.hpp"
int* AIStationaryGunHolder_Init_2(float self)

{
    DynObj_LookupHashItem_6(Level_StartMission_4(), self, 0);
    long double fpu_val_7 = *(self + 0x9c);
    long double fpu_val_6 = *(self + 0xa0);
    fpu_val_6 - fpu_val_7;
    memset(self + 0x2598, 0, 0x18);
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

    float local_18_2 = Render_ComputeNormals(*(self + 0x6c)) * fpu_val_7_3;
    DynObj_LinkDynamicCube_8(Level_StartMission_4(), self, self + 0x2598, local_18_2, 1);
    *(self + 0x25b0) = 0x3f800000;
    *(self + 0x25b4) = 0;
    *(self + 0x25b8) = 0;
    *(self + 0x25bc) = 0;
    *(self + 0x25c0) = 0x3f800000;
    *(self + 0x25c4) = 0;
    *(self + 0x25c8) = 0;
    *(self + 0x25cc) = 0;
    *(self + 0x25d0) = 0x3f800000;
    *(self + 0x25d4) = 0;
    memcpy(self + 0x70, self + 0x25b0, 0x28);
    *(self + 0x25e8) = 0x43b40000;
    *(self + 0x25ec) = 0x43b40000;
    *(self + 0x25f0) = 0x41200000;
    *(self + 0x25d8) = 0;
    *(self + 0x2694) = 1;
    Script_ExecuteLevelScript_6(self + 0x25f4);
    int* result = Script_ExecuteLevelScript_6(self + 0x2644);
    *(self + 0x2698) = 0;
    return result;
}
