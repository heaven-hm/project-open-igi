// IGI native provenance: Light_RenderLensflare_11 - 0x0052D610 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D610-Light_RenderLensflare_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_RenderLensflare_11.h"
int Light_RenderLensflare_11(void* context, char* arg2)

{
    int item_4 = *(context + 0x18);
    return Game_DataSymbolLoad(arg2, "%04d");
}
