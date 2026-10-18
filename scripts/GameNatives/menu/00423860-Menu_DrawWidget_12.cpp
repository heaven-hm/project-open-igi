// IGI native provenance: Menu_DrawWidget_12 - 0x00423860 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00423860-Menu_DrawWidget_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_DrawWidget_12.hpp"
int Menu_DrawWidget_12(int context, int arg2)

{
    int dest_2 = context << 0x1e | arg2;
    int i;

    for (i = 0; i < 0x7f; i += 1)
    {
        KEY_ANGLE(i, &context, nullptr);

        if (context == dest_2)
            break;
    }

    if (i != 0x7f)
        return i;

    return 0xffffffff;
}
