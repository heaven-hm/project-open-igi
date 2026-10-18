// IGI native provenance: HumanPlayer_LoadParameters_14 - 0x00413730 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413730-HumanPlayer_LoadParameters_14.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_LoadParameters_14.h"

int HumanPlayer_LoadParameters_14(void* self, float args)

{
    *(args + 0xc) = self + 0x20;
    *(args + 0x10) = self + 0x78;
    *(args + 0x14) = *(self + 0xa0);
    *(args + 0x1c) = 2;
    *(args + 0x20) = 3;
    return AI_UpdateOrders_25(args);
}
