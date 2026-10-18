// IGI native provenance: Siren_Stop - 0x004492A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004492A0-Siren_Stop.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004492A0-Siren_Stop.hpp"
int* Siren_Stop(void* self, int args)

{
    (*((FX_TrailUpdate() << 2) + &g_a96ae0))(self, args);
    FX_FireBallUpdate_2(self, 1.0f, 0x3f000000, 0x3f000000, 0x3f800000, 0, 5, 4, "explosion_1");
    *(self + 0x2d0) = 0;
    *(self + 0x2d1) = 0;
    *(self + 0x2d2) = 0;
    *(self + 0x238) = 0;
    *(self + 0x258) = 0;
    *(self + 0x2c8) = 0;
    *(self + 0x2cc) = 0;
    return Script_ExecuteLevelScript_6(self + 0x278);
}
