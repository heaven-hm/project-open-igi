// IGI native provenance: ComputerMap_Init_2 - 0x00469660 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00469660-ComputerMap_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ComputerMap_Init_2.h"

int ComputerMap_Init_2(void* self, int args)

{
    if (!args)
        AI_AlertAllies_25(*(self + 0xa0), 4);
    else if (args != 1)
    {
        if (args != 2)
            return args - 2;

        return AI_AlertAllies_25(*(self + 0xa0), 0x1f);
    }

    return AI_AlertAllies_25(*(self + 0xa0), 0x15);
}
