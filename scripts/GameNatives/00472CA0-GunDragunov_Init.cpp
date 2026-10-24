// IGI native provenance: GunDragunov_Init - 0x00472CA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00472CA0-GunDragunov_Init.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00472CA0-GunDragunov_Init.hpp"
int GunDragunov_Init(void* self)

{
    (*((Weapon_ComputeAccuracy_5() << 2) + &g_a96ae0))(self, 0);
    *(self + 0x104) = data_540394[0];
    *(self + 0x108) = data_540394[4][0];
    *(self + 0x10c) = data_540394[8][0];
    int result_1 = data_540394[0xc][0];
    *(self + 0x110) = result_1;
    *(self + 0x114) = data_540394[0x10][0];
    return result_1;
}
