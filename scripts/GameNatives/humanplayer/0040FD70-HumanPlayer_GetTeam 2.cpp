// IGI native provenance: HumanPlayer_GetTeam - 0x0040FD70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040FD70-HumanPlayer_GetTeam.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_GetTeam.h"

void* HumanPlayer_GetTeam(void* self, int args)

{
    if (!(*(self + 8) & 1))
        return self;

    int scratch_val_1 = *(self + 0x10);
    *(self + 0x10) = 1;
    void* counter_val_1 = *(self + 4);
    *(self + 0xc) = scratch_val_1;
    int counter_val_2 = *(counter_val_1 + 4);
    *(self + 0x14) = counter_val_2;
    return counter_val_2(self, args);
}
