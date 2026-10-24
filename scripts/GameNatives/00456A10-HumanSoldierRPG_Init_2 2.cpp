// IGI native provenance: HumanSoldierRPG_Init_2 - 0x00456A10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456A10-HumanSoldierRPG_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanSoldierRPG_Init_2.h"

void* HumanSoldierRPG_Init_2(void* self, void* args)

{
    int counter_val = *(self + 8);

    if (!(counter_val & 4))
    {
        void* scratch_val = *(self + 4);
        *(self + 0xc) = *(self + 0x10);
        *(self + 0x10) = 0xb;
        int counter_val_2 = *(scratch_val + 0x2c);
        *(self + 0x14) = counter_val_2;
        return counter_val_2(self, args);
    }

    if (counter_val & 0x10000)
    {
        void* scratch_val_2 = *(self + 4);
        *(self + 0xc) = *(self + 0x10);
        *(self + 0x10) = 0xc;
        int counter_val_4 = *(scratch_val_2 + 0x30);
        *(self + 0x14) = counter_val_4;
        return counter_val_4(self, args);
    }

    if (counter_val & 0x20000)
    {
        void* scratch_val_4 = *(self + 4);
        *(self + 0xc) = *(self + 0x10);
        *(self + 0x10) = 0xd;
        int counter_val_6 = *(scratch_val_4 + 0x34);
        *(self + 0x14) = counter_val_6;
        return counter_val_6(self, args);
    }

    if (!(counter_val & 0x81fe1) && *(args + 0xd04) != 0xffffffff)
        return self;

    int counter_val_7 = *(self + 0x10);
    *(self + 0x10) = 0;
    *(self + 0xc) = counter_val_7;
    int counter_val_9 = **(self + 4);
    *(self + 0x14) = counter_val_9;
    return counter_val_9(self, args);
}
