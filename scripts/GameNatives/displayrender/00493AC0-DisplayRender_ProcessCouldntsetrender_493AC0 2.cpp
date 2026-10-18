// IGI native provenance: DisplayRender_ProcessCouldntsetrender_493AC0 - 0x00493AC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00493AC0-DisplayRender_ProcessCouldntsetrender_493AC0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DisplayRender_ProcessCouldntsetrender_493AC0.h"

int DisplayRender_ProcessCouldntsetrender_493AC0()

{
    int* 5ca114_1 = g_5ca114;
    int result = (*(*5ca114_1 + 0x50))(5ca114_1, 0x88, 0);

    if (!result)
        return result;

    void var_400;
    Game_DataSymbolLoad(&var_400, "Couldn't set renderstate");
    int var_414 = DDERR_GENERIC(result);
    void* var_418 = &var_400;
    Error_Show("%s\n\nDirectX error message: %s");

    while (true)
        /* nop */
}
