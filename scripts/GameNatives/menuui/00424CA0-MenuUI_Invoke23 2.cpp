// IGI native provenance: MenuUI_Invoke23 - 0x00424CA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424CA0-MenuUI_Invoke23.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuUI_Invoke23.h"

int MenuUI_Invoke23(int arg1, int arg2)

{
    if (arg1 == 0x200 && !FX_FragmentSimulate_23())
    {
        g_57bc58 = arg2;
        g_57bc5c = arg2 >> 0x10;
    }

    return 1;
}
