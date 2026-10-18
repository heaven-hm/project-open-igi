// IGI native provenance: Light_ComputeDirectionalLight_11 - 0x0052D5E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D5E0-Light_ComputeDirectionalLight_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_ComputeDirectionalLight_11.h"
int Light_ComputeDirectionalLight_11(void* context, char* arg2)

{
    int item_4 = *(context + 0x14);
    int item_8 = *(context + 0x10);
    int item_c = *(context + 0xc);
    return Game_DataSymbolLoad(arg2, "%02d-%02d-%02d");
}
