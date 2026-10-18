// IGI native provenance: GFXCapsViewer_Init_2 - 0x005196F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005196F0-GFXCapsViewer_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GFXCapsViewer_Init_2.hpp"

int* GFXCapsViewer_Init_2(int* self )

{
    (*((Display_CloseDevice_5() << 2) + &g_a94e84))(0xa81028);
    *self = 0xa81028;
    return self;
}
