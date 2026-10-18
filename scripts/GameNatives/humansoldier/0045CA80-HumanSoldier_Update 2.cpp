// IGI native provenance: HumanSoldier_Update - 0x0045CA80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045CA80-HumanSoldier_Update.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanSoldier_Update.h"

void* HumanSoldier_Update(void* self, int args)

{
    if (!(*(self + 8) & 1))
        return self;

    int* scratch_val_1 = *(self + 4);
    *(self + 0xc) = *(self + 0x10);
    *(self + 0x10) = 0;
    int counter_val_2 = *scratch_val_1;
    *(self + 0x14) = counter_val_2;
    return counter_val_2(self, args);
}
