// IGI native provenance: ProximityMine_Init_2 - 0x004752B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004752B0-ProximityMine_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004752B0-ProximityMine_Init_2.hpp"
int ProximityMine_Init_2(void* self, char* args)

{
    int result = *(args + 8);

    if (result != self)
    {
        int scratch_val_1 = *(self + 0x208);

        if (result != scratch_val_1)
        {
            result = *(args + 0xc);

            if (result != self && result != scratch_val_1)
            {
                *args = 1;
                return result;
            }
        }
    }

    *args = 0;
    return result;
}
