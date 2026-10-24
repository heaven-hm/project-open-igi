// IGI native provenance: FX_CouldNotGetMouseCaps - 0x0048FD90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048FD90-FX_CouldNotGetMouseCaps.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048FD90-FX_CouldNotGetMouseCaps.hpp"
int FX_CouldNotGetMouseCaps()

{
    int* data_val = g_5c8e1c;
    int item_2c = 0x2c;
    int result = (*(*data_val + 0xc))(data_val, &item_2c);

    if (!result)
    {
        char item_1c;
        result = item_1c;
        g_c28f80 = 1;
        g_c28f81 = result;
        return result;
    }

    Error_Show("Could not get mouse caps!");

    while (true)
        /* nop */
}
